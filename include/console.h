#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <vector>

class CPU6502;

// TODO
// - Allow non-menu commands to have sub commands

class Console
{
public:
    Console();
    ~Console();

    static CPU6502* m_CPU;

    void parseCommand(std::string cmd_str);

private:

    struct Command
    {
        std::string cmd_str;
        std::string help_str;
        void (*func)(std::vector<std::string>) = nullptr;
        int min_args;
        int max_args;
        std::vector<Command> sub_commands;

        Command(std::string cmd_str, std::string help_str, void (*func)(std::vector<std::string>) = nullptr,
            int min_args = 0, int max_args = 0)
        {
            this->cmd_str = cmd_str;
            this->help_str = help_str;
            this->func = func;
            this->min_args = min_args;
            this->max_args = max_args;
        }
    };
    std::vector<Command> m_Commands;
    Command* findCommand(std::vector<std::string>& tokens);
    void showHelp(std::vector<std::string> tokens);
    void showHelpCommands(std::vector<Command>& commands);
    void showCommandHelp(Command* cmd);
    bool isMenu(Command* cmd);
    bool isCommand(Command* cmd);
    void showMenu(Command* cmd);

    static void doHelp(std::vector<std::string> args);
    static void doQuit(std::vector<std::string> args);

    static void doMemDump(std::vector<std::string> args);
};

#endif