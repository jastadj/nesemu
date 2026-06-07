#include <iostream>

#include <thread>

#include "console.h"
#include "cpu6502.h"
#include "glwindow.h"

int main()
{
    bool quit = false;
    GLWindow window;
    Console console;
    CPU6502 cpu;

    console.m_CPU = &cpu;

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
            console.parseCommand(buf);
        }
    }
    
    return 0;
}