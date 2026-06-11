#include "glwindow.h"

// debug
#include <iostream>

#include "tools.h"
#include "glfont.h"

////////////////////////////////////////////////////////////////////

// Vertex Shader
const char* vertex_test = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

// Fragment Shader
const char* frag_test = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(0.0f, 0.8f, 1.0f, 1.0f); // Cyan triangle
}
)";
////////////////////////////////////////////////////////////////////

GLWindow::GLWindow():
    m_ShaderProgram(0),
    m_VAO(0),
    m_VBO(0),
    m_Window(nullptr),
    m_State(STATE::NONE),
    m_GLFont(nullptr)
{

}

GLWindow::~GLWindow()
{
    if (m_State > STATE::NONE)
    {
        m_RenderThread.join();
    }

    delete m_GLFont;
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
    int err = 0;
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
    err = gladLoadGL();
    if (err == 0) {
        std::cerr << "Failed to initialize GLAD: error " << err << std::endl;
        glfwTerminate();
        return false;
    }

    // Initialize FreeType
    err = FT_Init_FreeType(&m_Freetype);
    if (err)
    {
        std::cerr << "Failed to initialize FreeType: error " << err << std::endl;
        return false;
    }
    // Create Text Renderer
    m_GLFont = new GLFont();
    if (!m_GLFont->init(m_Freetype, "resources/fonts/courier_prime/CourierPrimeCode.ttf"))
    {
        delete m_GLFont;
        m_GLFont = nullptr;
        return false;
    }

    return true;
}

bool GLWindow::initShaders()
{
    m_ShaderProgram = GLTools::createShaderProgram(vertex_test, frag_test);
    if (m_ShaderProgram == 0)
    {
        std::cout << "Error creating test shader program." << std::endl;
        return false;
    }
    return true;
}

bool GLWindow::initVertexObjects()
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

    return true;
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

        m_GLFont->renderText("TEST", 100.0, 100.0, 1.0, { 1.f, 0.f, 0.f });

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    // Cleanup

    // test triangle
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteProgram(m_ShaderProgram);

    FT_Done_FreeType(m_Freetype);
    glfwTerminate();
    std::cout << "Window closed, enter 'quit' command in console.\n";

    m_State = STATE::CLOSED;
}



