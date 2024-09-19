#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"

// Windows specific includes and definitions
#if _WIN32
    #include <dwmapi.h>

    #ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
        #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
    #endif

    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif

// Fix __attribute__ warning when using MSVC
#if _MSC_VER
    #define __attribute__(x)
#endif


double scroll = 0.;



static void errorCallback(int error __attribute__((unused)), const char* description)
{
    std::cout << description << "\n";
}

static void framebufferSizeCallback(GLFWwindow* window __attribute__((unused)), int width, int height)
{
    glViewport(0, 0, width, height);
}

static void scrollCallback(GLFWwindow* window __attribute__((unused)), double xoffset __attribute__((unused)), double yoffset)
{
    scroll -= yoffset * 0.25;
}



struct GlInstancedObject
{
    // vertex array object
    unsigned int vao;
    // vertex buffer object
    unsigned int vbo;
    // instance buffer object
    unsigned int ibo;
    // Number of verticies to draw
    unsigned int vertex_count;
    // Number of instances to draw
    unsigned int instance_count;
};

glm::mat4 getCurrentMatrix(GLFWwindow* window, float radius)
{
    int width_i = 1280u;
    int height_i = 720u;
    glfwGetWindowSize(window, &width_i, &height_i);
    float width = static_cast<float>(width_i);
    float height = static_cast<float>(height_i);

    float ratio = width / height;
    ratio *= radius;
    return glm::ortho(-ratio, ratio, -radius, radius);
}

int main(int argc, const char* argv[])
{
    // Initalise GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initalise GLFW.\n";
        return EXIT_FAILURE;
    }
    glfwSetErrorCallback(errorCallback);

    // Calculate primes
    int point_count = 1000000;
    if (argc > 1)
        point_count = std::stoi(argv[1]);
    glm::vec2* points = new glm::vec2[point_count];
    int* primes = new int[point_count];
    std::cout << sizeof(glm::vec2) * point_count + sizeof(int) * point_count << " bytes reserved.\n";


    std::cout << std::setprecision(2) << std::fixed;

    double time_estimation_scale = static_cast<double>(point_count) * glm::sqrt(static_cast<double>(point_count));
    double start_time = glfwGetTime();

    int next_point = 0;
    int i = 1;          //Start at one for prime number checking
    float theta = 1.f;
    while (next_point < point_count)
    {
        i += 1;
        theta = glm::mod(theta + 1, glm::tau<float>());

        bool is_prime = true;
        int root = glm::sqrt(i);
        for (int c = 0; c < next_point; c++)
        {
            int prime = primes[c];
            if (prime >= root)
                break;
            if (i % prime == 0)
            {
                is_prime = false;
                break;
            }
        }
        if (!is_prime) continue;

        float t = static_cast<float>(i);
        primes[next_point] = i;
        points[next_point++] = {t, theta};

        if (next_point % 1000 == 0)
        {
            double time = glfwGetTime() - start_time;
            double estimated = time / (static_cast<double>(next_point) * glm::sqrt(static_cast<double>(next_point))) * time_estimation_scale;

            std::cout << "\r";
            std::cout << next_point;
            std::cout << " / ";
            std::cout << point_count;
            std::cout << " (";
            std::cout << static_cast<float>(next_point) / static_cast<float>(point_count) * 100.f;
            std::cout << "%) primes calculated. ";
            std::cout << time;
            std::cout << "s elapsed ";
            std::cout << estimated - time;
            std::cout << "s remaining.";
        }
    }

    std::cout << "\nData filled.\n";

    delete[] primes;

    std::cout << "Primes deleted.\n";

    // Create Window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);

    GLFWwindow* window;
    {
        #if _WIN32
            int width = 400;
            int height = 300;
        #else
            int width = 1280;
            int height = 720;
        #endif

        window = glfwCreateWindow(width, height, "Polar Primes", NULL, NULL);
    }
    if (!window)
    {
        glfwTerminate();
        std::cout << "Failed to create window.\n";
        return EXIT_FAILURE;
    }

    #if _WIN32
        BOOL value = TRUE;
        ::DwmSetWindowAttribute(glfwGetWin32Window(window), DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));

        // Resize to update decorations
        glfwSetWindowSize(window, 1280, 720);
    #endif

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSwapInterval(1);

    // Setup GLAD and OpenGL with the window context
    gladLoadGL(glfwGetProcAddress);
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
    }

    // Set OpenGL context settings
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

    // Initalise OpenGL objects
    Shader point_shader("assets/point.vert", "assets/point.frag");
    if (!point_shader.valid)
    {
        std::cout << "Point shader is not valid.\n";
        return EXIT_FAILURE;
    }
    
    GlInstancedObject points_object;
    {
        glGenVertexArrays(1, &points_object.vao);
        glBindVertexArray(points_object.vao);

        glGenBuffers(1, &points_object.vbo);

        std::vector<glm::vec2> vertices;
        for (float t = 0.f; t < glm::tau<float>(); t += glm::tau<float>() * 0.2f)
            vertices.push_back({glm::sin(t), glm::cos(t)});

        glBindBuffer(GL_ARRAY_BUFFER, points_object.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        // Instance Data
        glGenBuffers(1, &points_object.ibo);
        glBindBuffer(GL_ARRAY_BUFFER, points_object.ibo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * point_count, points, GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
        glVertexAttribDivisor(1, 1); 

        points_object.instance_count = point_count;

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        points_object.vertex_count = vertices.size();
    }

    std::cout << "OpenGL object created.\n";

    delete[] points;

    std::cout << "Data array deleted.\n";

    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glm::mat4 matrix = getCurrentMatrix(window, 100.f * glm::pow(2., scroll));
        
        glClear(GL_COLOR_BUFFER_BIT);

        point_shader.use();
        point_shader.setUniform("mat", matrix);
        glBindVertexArray(points_object.vao);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, points_object.vertex_count, points_object.instance_count);
        glBindVertexArray(0);

        glfwSwapBuffers(window);

        double time = glfwGetTime();
        double fps = 1. / (time - last_time);
        last_time = time;
        std::string title = "Polar Primes - ";
        title += std::to_string(fps);
        title += "fps";
        glfwSetWindowTitle(window, title.c_str());
    }
    
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

// If get more ram try with 1 billion points