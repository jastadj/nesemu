#include "gltools.h"

#include <iostream>

unsigned int GLTools::compileShader(unsigned int type, const char* source)
{
    // Create and compile shader
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Successful?
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

unsigned int GLTools::createShaderProgram(const char* vertex_source, const char* frag_source)
{
    // Compile Vertex and Frag shaders
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertex_source);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, frag_source);

    // Create Shader Program (attach shaders)
    unsigned int prg = glCreateProgram();
    glAttachShader(prg, vertexShader);
    glAttachShader(prg, fragmentShader);
    glLinkProgram(prg);

    // Success?
    int success;
    glGetProgramiv(prg, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(prg, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed: " << infoLog << std::endl;
    }

    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return prg;
}