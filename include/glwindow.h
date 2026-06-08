#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <thread>
#include <mutex>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLWindow
{
public:
    enum STATE{NONE, STARTED, RUNNING, CLOSED, ERROR};

    GLWindow();
    ~GLWindow();

    bool start();
    bool running();
    void closeWindow();

private:

    STATE m_State;
    bool init();
    void initShaders();
    void initVertexObjects();

    // Callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    // Window
    GLFWwindow* m_Window;

    // Render Thread
    std::thread m_RenderThread;
    void renderLoop();

    // Shaders
    unsigned int m_ShaderProgram;

    // Vertex Objects
    unsigned int m_VAO;
    unsigned int m_VBO;
};

#endif