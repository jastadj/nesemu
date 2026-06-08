#include "glwindow.h"

// debug
#include <iostream>

GLWindow::GLWindow():
    m_ShaderProgram(0),
    m_VAO(0),
    m_VBO(0),
    m_Window(nullptr),
    m_State(STATE::NONE)
{

}

GLWindow::~GLWindow()
{
    if (m_State > STATE::NONE)
    {
        m_RenderThread.join();
    }
    
}

bool GLWindow::start()
{
    m_State = STATE::STARTED;
    m_RenderThread = std::thread(&GLWindow::renderLoop, this);
    return true;
}

bool GLWindow::running()
{
    return m_State == STATE::RUNNING || m_State == STATE::STARTED;
}

void GLWindow::closeWindow()
{
    glfwSetWindowShouldClose(m_Window, true);
}

bool GLWindow::init()
{
    std::cout << "Initializing window..." << std::endl;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    m_Window = glfwCreateWindow(800, 600, "NES EMU", nullptr, nullptr);
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_Window);

    glfwSetFramebufferSizeCallback(m_Window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(m_Window, mouseButtonCallback);

    // Initialize GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return false;
    }

    return true;
}

void GLWindow::initShaders()
{
    // Vertex Shader
    const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

    // Fragment Shader
    const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.0f, 0.8f, 1.0f, 1.0f); // Cyan triangle
}
)";

    // Compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, vertexShader);
    glAttachShader(m_ShaderProgram, fragmentShader);
    glLinkProgram(m_ShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void GLWindow::initVertexObjects()
{
    // Triangle vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLWindow::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void GLWindow::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    
    std::cout << "mouseButtonCallback: button " << button << " action " << action << " mods " << mods << std::endl;
}

void GLWindow::renderLoop()
{
    if (!init())
    {
        m_State = STATE::ERROR;
        return;
    }
    initShaders();
    initVertexObjects();
    m_State = STATE::RUNNING;

    // Render loop
    while (!glfwWindowShouldClose(m_Window))
    {
        if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            closeWindow();
        }

        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_ShaderProgram);
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_ShaderProgram);

    glfwTerminate();
    std::cout << "Window closed, enter 'quit' command in console.\n";

    m_State = STATE::CLOSED;
}
