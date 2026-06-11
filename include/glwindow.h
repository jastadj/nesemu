#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <thread>
#include <mutex>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "gltools.h"

class GLFont;

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
    bool initShaders();
    bool initVertexObjects();

    // Callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    // Window
    GLFWwindow* m_Window;

    // Freetype
    FT_Library m_Freetype;

    // Render Thread
    std::thread m_RenderThread;
    void renderLoop();

    // Shaders
    unsigned int m_ShaderProgram;

    // Vertex Objects
    unsigned int m_VAO;
    unsigned int m_VBO;

    // Render Objects
    GLFont* m_GLFont;
};

#endif