#pragma once

#include <glm/mat4x4.hpp>

class Shader
{
    public:
        // the program ID
        unsigned int shader_program;

        // if the shader is valid and can be used
        bool valid = false;
    
        Shader();
        // constructor reads and builds the shader
        Shader(const char* vertexPath, const char* fragmentPath);
        // use/activate the shader
        void use();

        void setUniform(const char* name, bool value) const;
        void setUniform(const char* name, int value) const;
        void setUniform(const char* name, float value) const;
        void setUniform(const char* name, glm::vec2 value) const;
        void setUniform(const char* name, glm::vec4 value) const;
        void setUniform(const char* name, glm::mat4 value) const;
};