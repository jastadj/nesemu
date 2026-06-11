#ifndef GLTOOLS_H
#define GLTOOLS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace GLTools
{
    // Shaders
    unsigned int compileShader(unsigned int type, const char* source);
    unsigned int createShaderProgram(const char* vertex_source, const char* frag_source);
};
#endif