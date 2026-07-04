#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <vector>
#include <iostream>

namespace NES
{
    class Console;
}


// TODO
// - Allow non-menu commands to have sub commands

class Console
{
public:
    static Console* getInstance();

    static NES::Console* nes;

    void parseCommand(std::string cmd_str);

private:
    // Singleton
    static Console* m_Instance;
    Console();
    ~Console();

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

    std::ios m_IOSState;

    static void doHelp(std::vector<std::string> args);
    static void doQuit(std::vector<std::string> args);

    // General
    static void doLS(std::vector<std::string> args);

    // temp debug
    static void doJohn(std::vector<std::string> args);

    // Memory
    static void doMemShow(std::vector<std::string> args);
    static void doMemRead(std::vector<std::string> args);
    static void doMemWrite(std::vector<std::string> args);
    static void doMemSave(std::vector<std::string> args);
    static void doMemFill(std::vector<std::string> args);
    static void doMemFillRand(std::vector<std::string> args);

    // CPU
    static void doCPUShow(std::vector<std::string> args);
    static void doCPUExecute(std::vector<std::string> args);
    static void doCPUPC(std::vector<std::string> args);
    static void doCPUStart(std::vector<std::string> args);
    static void doCPUStop(std::vector<std::string> args);
    static void doCPUShowOpcodes(std::vector<std::string> args);

    // ASM
    static void doASM(std::vector<std::string> args);

    // NES
    static void doNESOn(std::vector<std::string> args);
    static void doNESOff(std::vector<std::string> args);
    static void doNESReset(std::vector<std::string> args);
    static void doNESShow(std::vector<std::string> args);
    static void doNESLoad(std::vector<std::string> args);
    static void doNESUnload(std::vector<std::string> args);
    static void doNESDebug(std::vector<std::string> args);

};

#endif