#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>

class Console
{
public:
    Console();
    ~Console();

    void parseCommand(std::string cmd_str);
};

#endif