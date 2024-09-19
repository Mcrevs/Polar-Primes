#include "shader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader()
{

}

Shader::Shader(const char* vertex_path, const char* fragment_path)
{
    std::string vertex_code;
    std::string fragment_code;
    std::ifstream vertex_shader_file;
    std::ifstream fragment_shader_file;

    // ensure ifstream objects can throw exceptions:
    vertex_shader_file.exceptions(std::ifstream::badbit | std::ofstream::failbit);
    fragment_shader_file.exceptions(std::ifstream::badbit | std::ofstream::failbit);
    try 
    {
        // open files
        vertex_shader_file.open(vertex_path, std::ios_base::in);
        fragment_shader_file.open(fragment_path, std::ios_base::in);
        std::stringstream vertex_shader_stream, fragment_shader_stream;

        // read file's buffer contents into streams
        vertex_shader_stream << vertex_shader_file.rdbuf();
        fragment_shader_stream << fragment_shader_file.rdbuf();

        // close file handlers
        vertex_shader_file.close();
        fragment_shader_file.close();

        // convert stream into string
        vertex_code = vertex_shader_stream.str();
        fragment_code = fragment_shader_stream.str();
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "Unable to read shader file.\n";
        std::cout << e.what() << "\n";
        return;
    }

    const char* vertex_shader_code = vertex_code.c_str();
    const char* fragment_shader_code = fragment_code.c_str();

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    {
        int success;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
            std::cout << "Unable to compile vertex shader.\n";
            std::cout << infoLog;
            return;
        }
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    {
        int success;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
            std::cout << "Unable to compile fragment shader.\n";
            std::cout << infoLog;
            return;
        }
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    int success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "Unable to link shader program.\n";
        std::cout << infoLog;
        return;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    valid = true;
};

void Shader::use()
{
    glUseProgram(shader_program);
};

void Shader::setUniform(const char* name, bool value) const
{
    int location = glGetUniformLocation(shader_program, name);
    if (location == -1)
        std::cout << "Unable to set unused uniform.\n";
    else
        glUniform1i(location, (int)value);
};

void Shader::setUniform(const char* name, int value) const
{
    int location = glGetUniformLocation(shader_program, name);
    if (location == -1)
        std::cout << "Unable to set unused uniform.\n";
    else
        glUniform1i(location, value);
};

void Shader::setUniform(const char* name, float value) const
{
    int location = glGetUniformLocation(shader_program, name);
    if (location == -1)
        std::cout << "Unable to set unused uniform.\n";
    else
        glUniform1f(location, value);
};

void Shader::setUniform(const char* name, glm::vec2 value) const
{
    int location = glGetUniformLocation(shader_program, name);
    if (location == -1)
        std::cout << "Unable to set unused uniform.\n";
    else
        glUniform2fv(location, 1, glm::value_ptr(value));
};

void Shader::setUniform(const char* name, glm::vec4 value) const
{
    int location = glGetUniformLocation(shader_program, name);
    if (location == -1)
        std::cout << "Unable to set unused uniform.\n";
    else
        glUniform4fv(location, 1, glm::value_ptr(value));
};

void Shader::setUniform(const char* name, glm::mat4 value) const
{
    int location = glGetUniformLocation(shader_program, name);
    if (location == -1)
        std::cout << "Unable to set unused uniform.\n";
    else
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
};