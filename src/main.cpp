#include <iostream>

#include <thread>

#include "console.h"
#include "glwindow.h"

int main()
{
    bool quit = false;
    GLWindow window;
    Console console;

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