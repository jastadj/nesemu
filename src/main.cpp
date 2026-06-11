#include <iostream>

#include <thread>

#include "console.h"
#include "nes.h"
#include "glwindow.h"

int main(int argc, char* argv[])
{
    bool quit = false;
    GLWindow window;
    Console* console = Console::getInstance();
    NES nes;

    std::cout << argv[0] << std::endl;

    console->nes = &nes;

    // Start window
    window.start();

    while (!quit)
    {
        if (!window.running())
        {
            break;
        }

        std::cout << "> ";

        std::string buf;
        std::getline(std::cin, buf);

        if (buf == "quit")
        {
            window.closeWindow();
            quit = true;
        }
        else
        {
            console->parseCommand(buf);
        }
    }
    
    return 0;
}