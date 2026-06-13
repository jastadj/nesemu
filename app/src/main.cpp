#include <iostream>

#include <thread>
#include <string>

#include "console.h"
#include "nes.h"
//#include "glwindow.h"

int main(int argc, char* argv[])
{
    bool quit = false;
    /*
    GLWindow window;
    */
    Console* console = Console::getInstance();
    NES nes;

    // NES reference for console
    console->nes = &nes;

    // Start window
    /*
    if (!window.start())
    {
        std::cerr << "Error starting GL window." << std::endl;
        return 1;
    }
    */

    // Console loop
    while (!quit)
    {
        /*
        if(!window.running())
        {
            break;
        }
        */

        std::cout << "> ";

        std::string buf;
        std::getline(std::cin, buf);

        if (buf == "quit")
        {
            /*
            window.closeWindow();
            */
            quit = true;
        }
        else
        {
            console->parseCommand(buf);
        }
    }

    /*
    std::cout << "Shutting down GLFW..." << std::endl;
    glfwTerminate();
    std::cout << "Done." << std::endl;
    */
    
    return 0;
}