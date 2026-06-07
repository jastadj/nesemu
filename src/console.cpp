#include "console.h"

#include <iomanip>
#include <sstream>

#include "tools.h"

#include "cpu6502.h"

// Statics
Console* Console::m_Instance = nullptr;
CPU6502* Console::m_CPU = nullptr;

Console::Console():
    m_IOSState(nullptr)
{
    // Capture IOS starting state
    m_IOSState.copyfmt(std::cout);

    m_Commands.emplace_back(Command("help", "Show help", Console::doHelp, 0, -1));
    m_Commands.emplace_back(Command("quit", "Quit", Console::doQuit));

    m_Commands.emplace_back(Command("john", "Run a john test", Console::doJohn, 0, -1));
    
    m_Commands.emplace_back(Command("mem", "Memory commands"));
    m_Commands.back().sub_commands.emplace_back(Command("read", "Read memory (usage: read <offset> <len>)", doMemRead, 2, 2));
    m_Commands.back().sub_commands.emplace_back(Command("write", "Write memory (usage: write <offset> <b0> <b1> <bn...>)", doMemWrite, 2, -1));
    
    m_Commands.emplace_back(Command("cpu", "CPU commands"));
    m_Commands.back().sub_commands.emplace_back(Command("show", "Show CPU info", doCPUShow));
    m_Commands.back().sub_commands.emplace_back(Command("execute", "Execute instruction at current PC", doCPUExecute));
    m_Commands.back().sub_commands.emplace_back(Command("pc", "Set program counter (usage: pc <offset>)", doCPUPC, 1, 1));
}

Console::~Console()
{
}

Console* Console::getInstance()
{
    if (!m_Instance)
    {
        m_Instance = new Console();
    }
    return m_Instance;
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
            if (isMenu(cmd) && arg_count)
            {
                std::cout << "Unknown command." << std::endl;
                showMenu(cmd);
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
    }
    else
    {
        std::cout << "Unknown command.  Try 'help'." << std::endl;
    }

    // Reset cout ios state
    std::cout.copyfmt(m_IOSState);
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

// TEMP JOHN TEST COMMAND

void Console::doJohn(std::vector<std::string> args)
{
    m_Instance->parseCommand("mem write 0x00 0xa9 0x7f 0x85 0x10");
    m_Instance->parseCommand("cpu execute");
    m_Instance->parseCommand("cpu execute");
    m_Instance->parseCommand("cpu show");
    m_Instance->parseCommand("mem read 0 32");
}


//////////////////
// MEMORY COMMANDS

void Console::doMemRead(std::vector<std::string> args)
{
    uint16_t offset = uint16_t(Tools::toInt(args[0]));
    uint16_t len = uint16_t(Tools::toInt(args[1]));

    std::cout << "Dumping memory @ 0x" << std::hex << std::setw(4) << std::setfill('0') << offset << ", len " << std::dec << len << std::endl;

    if (offset + len > m_CPU->getMemorySize())
    {
        std::cout << "Range out of bounds ( > " << m_CPU->getMemorySize() << ")" << std::endl;
        return;
    }

    // Calculate display range
    uint16_t display_start = offset - (offset % 16);
    uint16_t display_end = (offset + len);
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
    std::cout << std::dec << std::setw(0) << std::endl;
}

void Console::doMemWrite(std::vector<std::string> args)
{
    uint16_t offset = uint16_t(Tools::toInt(args[0]));
    // convert args to data
    std::size_t byte_count = args.size();

    // Out-Of-Bounds?
    if (offset + byte_count > m_CPU->getMemorySize())
    {
        std::cout << "Offset out of bounds ( > " << m_CPU->getMemorySize() << ")" << std::endl;
        return;
    }
    // Write each byte to memory
    for (auto i = 1; i < byte_count; i++)
    {
        std::size_t bpos = offset + i - 1;
        uint8_t* bp = &m_CPU->m_Memory[bpos];
        *bp = uint8_t(Tools::toInt(args[i]));
        std::cout << "Wrote " << std::hex << std::setw(2) << std::setfill('0') << int(*bp);
        std::cout << " @ 0x" << std::hex << std::setw(4) << std::setfill('0') << bpos << std::endl;
    }
}


///////////////
// CPU COMMANDS

void Console::doCPUShow(std::vector<std::string> args)
{
    std::cout << "Memory Size: " << m_CPU->getMemorySize() << std::endl;
    std::cout << "Registers" << std::endl;
    std::cout << "---------" << std::endl;
    std::cout << "      PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << int(m_CPU->m_PC) << std::endl;
    std::cout << "   Stack: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(m_CPU->m_Stack) << std::endl;
    std::cout << "     ACC: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(m_CPU->m_ACC) << std::endl;
    std::cout << "       X: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(m_CPU->m_RX) << std::endl;
    std::cout << "       Y: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(m_CPU->m_RY) << std::endl;
    std::cout << "  Status: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(m_CPU->m_Status) << std::endl;

    std::cout << std::dec << std::endl;
}

void Console::doCPUExecute(std::vector<std::string> args)
{
    std::cout << "Executing CPU Instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(m_CPU->m_Memory[m_CPU->m_PC]);
    std::cout << " @ 0x" << std::hex << std::setw(4) << std::setfill('0') << m_CPU->m_PC << std::endl;
    uint8_t opcode = m_CPU->m_Memory[m_CPU->m_PC];
    bool result = m_CPU->execute();
    if (!result)
    {
        std::cout << "Error executing opcode 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode) << std::endl;
    }
}

void Console::doCPUPC(std::vector<std::string> args)
{
    uint16_t val = uint16_t(Tools::toInt(args[0]));
    std::cout << "Setting program counter to " << std::hex << std::setw(4) << std::setfill('0') << val << std::endl;
    m_CPU->m_PC = val;
}