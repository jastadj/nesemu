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
    enum STATE{NONE, STARTED, RUNNING, CLOSE_REQUEST, CLOSED, ERROR};

    GLWindow();
    ~GLWindow();

    bool start();
    bool running();
    void closeWindow();
    bool shareWith(GLWindow& window);

private:

    STATE m_State;
    bool init();
    bool initShaders();
    bool initVertexObjects();

    // Callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    // Window
    static bool m_GLFWInitialized;
    GLFWwindow* m_Window;
    GLFWwindow* m_WindowShare;

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