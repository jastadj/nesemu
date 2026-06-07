#include "console.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "tools.h"

#include "cpu6502.h"

// Statics
CPU6502* Console::m_CPU = nullptr;

Console::Console()
{
    m_Commands.emplace_back(Command("help", "Show help", Console::doHelp, 0, -1));
    m_Commands.emplace_back(Command("quit", "Quit", Console::doQuit));
    
    m_Commands.emplace_back(Command("mem", "Memory commands"));
    m_Commands.back().sub_commands.emplace_back(Command("dump", "Print memory (usage: dump <offset> <len>)", doMemDump, 2, 2));
    
}

Console::~Console()
{
}

void Console::parseCommand(std::string cmd_str)
{
    if (cmd_str.empty())
    {
        return;
    }

    std::vector<std::string> tokens = Tools::split(cmd_str);

    Command* cmd = findCommand(tokens);

    if (cmd)
    {
        int arg_count = int(tokens.size());

        if (arg_count >= cmd->min_args && (arg_count <= cmd->max_args || cmd->max_args == -1) )
        {
            // Help is a special command
            if (cmd->cmd_str == "help")
            {
                showHelp(tokens);
            }
            else
            {
                // Is this a menu?
                if (isMenu(cmd))
                {
                    showMenu(cmd);
                }
                else if (isCommand(cmd))
                {
                    cmd->func(tokens);
                }
                else
                {
                    std::cout << "Command error: command has sub commands." << std::endl;
                }   
            }
        }
        else
        {
            if (arg_count < cmd->min_args)
            {
                std::cout << "Missing arguments." << std::endl;
            }
            else
            {
                std::cout << "Too many arguments." << std::endl;
            }
            showCommandHelp(cmd);
        }
    }
    else
    {
        std::cout << "Unknown command.  Try 'help'." << std::endl;
    }
}

Console::Command* Console::findCommand(std::vector<std::string>& tokens)
{
    Command* last_cmd_found = nullptr;
    std::vector<Command>* commands = &m_Commands;

    while (!tokens.empty())
    {
        bool found = false;
        for (auto i = 0; i < commands->size(); i++)
        {
            if ( (*commands)[i].cmd_str == tokens[0])
            {
                last_cmd_found = &(*commands)[i];
                commands = &last_cmd_found->sub_commands;
                found = true;
                tokens.erase(tokens.begin());
                break;
            }
        }
        if (!found)
        {
            break;
        }
    }
    return last_cmd_found;
}

void Console::showHelp(std::vector<std::string> tokens)
{
    std::vector<Command>* commands = &m_Commands;
    Command* cmd = nullptr;
    if (!tokens.empty())
    {
        cmd = findCommand(tokens);
        if (cmd == nullptr)
        {
            std::cout << "Could not find command." << std::endl;
            return;
        }
        commands = &cmd->sub_commands;
    }

    if (commands)
    {
        if (commands->empty() && isCommand(cmd))
        {
            showCommandHelp(cmd);
        }
        else
        {
            showHelpCommands(*commands);
        }
    }
}

void Console::showHelpCommands(std::vector<Console::Command>& commands)
{
    for (auto& cmd : commands)
    {
        std::cout << std::setw(16) << cmd.cmd_str << " - " << cmd.help_str << std::endl;
    }
}

void Console::showCommandHelp(Command* cmd)
{
    if (isCommand(cmd))
    {
        std::cout << cmd->help_str << std::endl;
    }
}

bool Console::isMenu(Command* cmd)
{
    return (cmd && cmd->func == nullptr && !cmd->sub_commands.empty());
}

bool Console::isCommand(Command* cmd)
{
    return (cmd && cmd->func && cmd->sub_commands.empty());
}

void Console::showMenu(Command* cmd)
{
    if (cmd)
    {
        std::stringstream oss;
        oss << cmd->cmd_str << " menu";
        
        std::cout << std::endl;
        std::cout << Tools::toUpper(oss.str()) << std::endl;
        std::cout << std::string(oss.str().length(), '=') << std::endl;
        std::cout << std::endl;
        showHelpCommands(cmd->sub_commands);
    }
}

// Built-In Dummy Commands
void Console::doHelp(std::vector<std::string> args) {};
void Console::doQuit(std::vector<std::string> args) {};


//////////////////
// MEMORY COMMANDS

void Console::doMemDump(std::vector<std::string> args)
{
    std::size_t offset = Tools::toUnsignedLongLong(args[0]);
    std::size_t len = Tools::toUnsignedLongLong(args[1]);

    std::cout << "Dumping memory @ 0x" << std::hex << std::setw(4) << std::setfill('0') << offset << ", len " << std::dec << len << std::endl;

    if (offset + len > m_CPU->getMemorySize())
    {
        std::cout << "Range out of bounds ( > " << m_CPU->getMemorySize() << ")" << std::endl;
        return;
    }

    // Calculate display range
    std::size_t display_start = offset - (offset % 16);
    std::size_t display_end = (offset + len);
    if (display_end % 16)
    {
        display_end += 16 - display_end % 16;
    }

    // Print Header
    std::cout << "Addr    ";
    for (int i = 0; i < 16; i++)
    {
        if (i == 8)
        {
            std::cout << "  ";
        }
        std::cout << std::hex << std::setw(2) << std::setfill('0') << i << " ";
    }
    std::cout << std::endl << std::string(58, '-') << std::endl;

    for (auto i = display_start; i < display_end; i++)
    {
        if (i % 16 == 0)
        {
            if (i != display_start)
            {
                std::cout << std::endl;
            }
            std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << i << ": ";
        }
        else if (i != display_start && i % 8 == 0)
        {
            std::cout << "  ";
        }

        if (i >= offset && i < offset + len)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << int(m_CPU->m_Memory[i]) << " ";
        }
        else
        {
            std::cout << "   ";
        }
    }
 
    std::cout << std::endl;
}