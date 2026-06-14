#include "console.h"

#include <iomanip>
#include <sstream>
#include <fstream>

#include "tools.h"

#include "nes.h"
#include "asm6502.h"
#include "opcodes6502.h"
#include "nesdefs.h"
#include "cart.h"

// Statics
Console* Console::m_Instance = nullptr;
NES::Console* Console::nes = nullptr;

using namespace Arch6502;

Console::Console():
    m_IOSState(nullptr)
{
    // Capture IOS starting state
    m_IOSState.copyfmt(std::cout);

    m_Commands.emplace_back(Command("help", "Show help", Console::doHelp, 0, -1));
    m_Commands.emplace_back(Command("quit", "Quit", Console::doQuit));
    m_Commands.emplace_back(Command("ls", "List files in PWD", Console::doLS));

    m_Commands.emplace_back(Command("john", "Run a john test", Console::doJohn, 0, -1));
    
    m_Commands.emplace_back(Command("mem", "Memory commands"));
    m_Commands.back().sub_commands.emplace_back(Command("read", "Read memory (usage: read <offset> <len>)", doMemRead, 2, 2));
    m_Commands.back().sub_commands.emplace_back(Command("write", "Write memory (usage: write <offset> <b0> <b1> <bn...>)", doMemWrite, 2, -1));
    m_Commands.back().sub_commands.emplace_back(Command("save", "Save memory to file (usage: save <filepath>)", doMemSave, 1, 1));
    m_Commands.back().sub_commands.emplace_back(Command("fill", "Fill memory with value (usage: fill <byte>)", doMemFill, 1, 1));
    m_Commands.back().sub_commands.emplace_back(Command("fillrand", "Fill memory with random values", doMemFillRand, 0, 0));
    
    m_Commands.emplace_back(Command("cpu", "CPU commands"));
    m_Commands.back().sub_commands.emplace_back(Command("show", "Show CPU info", doCPUShow));
    m_Commands.back().sub_commands.emplace_back(Command("execute", "Execute instruction at current PC (usage: execute [count=1])", doCPUExecute, 0, 1));
    m_Commands.back().sub_commands.emplace_back(Command("pc", "Set program counter (usage: pc <offset>)", doCPUPC, 1, 1));
    m_Commands.back().sub_commands.emplace_back(Command("start", "Start the CPU Clock", doCPUStart, 0, 0));
    m_Commands.back().sub_commands.emplace_back(Command("stop", "Stop the CPU Clock", doCPUStop, 0, 0));
    m_Commands.back().sub_commands.emplace_back(Command("opcodes", "Show opcodes", doCPUShowOpcodes, 0, 0));

    m_Commands.emplace_back(Command("asm", "Compile assembly file to NES rom (usage: asm <input_asm>)", doASM, 1,1));

    m_Commands.emplace_back(Command("nes", "NES commands"));
    m_Commands.back().sub_commands.emplace_back(Command("show", "Show NES info", doNESShow));
    m_Commands.back().sub_commands.emplace_back(Command("on", "Power On NES", doNESOn));
    m_Commands.back().sub_commands.emplace_back(Command("off", "Power Off NES", doNESOff));
    m_Commands.back().sub_commands.emplace_back(Command("reset", "Reset NES", doNESReset));
    m_Commands.back().sub_commands.emplace_back(Command("load", "Load ROM", doNESLoad, 1, 1));
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
void Console::doQuit(std::vector<std::string> args) {}
void Console::doLS(std::vector<std::string> args)
{
    for (auto& file : Tools::getFilesAtPath("."))
    {
        std::cout << file << std::endl;
    }
}

// TEMP JOHN TEST COMMAND

void Console::doJohn(std::vector<std::string> args)
{
    //m_Instance->parseCommand("mem write 0x00 0xb6 0x01 0xb4 0x02 0xa9 0x03");
    m_Instance->parseCommand("mem write 0x00 0xa9 0x05");
    m_Instance->parseCommand("cpu execute 3");
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

    if (offset + len > nes->m_CPU.getMemSize())
    {
        std::cout << "Range out of bounds ( > " << nes->m_CPU.getMemSize() << ")" << std::endl;
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
            std::cout << std::hex << std::setw(2) << std::setfill('0') << int(nes->m_CPU.getAddr(i, false)) << " ";
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
    if (offset + byte_count > nes->m_CPU.getMemSize())
    {
        std::cout << "Offset out of bounds ( > " << nes->m_CPU.getMemSize() << ")" << std::endl;
        return;
    }
    // Write each byte to memory
    for (auto i = 1; i < byte_count; i++)
    {
        std::size_t bpos = offset + i - 1;
        //uint8_t* bp = &m_CPU->m_Mem.get(bpos);
        nes->m_CPU.setAddr(bpos, uint8_t(Tools::toInt(args[i])), false);
        std::cout << "Wrote " << std::hex << std::setw(2) << std::setfill('0') << int(nes->m_CPU.getAddr(bpos, false));
        std::cout << " @ 0x" << std::hex << std::setw(4) << std::setfill('0') << bpos << std::endl;
    }
}

void Console::doMemSave(std::vector<std::string> args)
{
    std::string filename = args[0];
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (file.is_open())
    {
        for (auto i = 0; i < nes->m_CPU.getMemSize(); i++)
        {
            file.put(nes->m_CPU.getAddr(i, false));
        }
        file.flush();
        file.close();
        std::cout << "Wrote " << nes->m_CPU.getMemSize() << " bytes to " << filename << std::endl;
    }
    else
    {
        std::cout << "Error saving memory to " << filename << std::endl;
    }
}

void Console::doMemFill(std::vector<std::string> args)
{
    uint8_t val = Tools::toInt(args[0]);
    nes->m_CPU.fillMem(val);
    std::cout << "Memory filled with 0x" << std::hex << std::setw(2) << std::setfill('0') << int(val) << std::endl;
}

void Console::doMemFillRand(std::vector<std::string> args)
{
    nes->m_CPU.fillMemRandom();
    std::cout << "Memory filled with random values." << std::endl;
}


///////////////
// CPU COMMANDS

void Console::doCPUShow(std::vector<std::string> args)
{
    std::cout << "CPU" << std::endl;
    std::cout << "===" << std::endl;
    std::cout << "Clock Speed: " << nes->m_Clock.getClockSpeed() << " Hz";
    std::cout << std::setprecision(4) << " (" << nes->m_Clock.getClockSpeed() * 1e-6 << " MHz)" << std::endl;
    std::cout << "Cycle Speed: " << 1e9 / nes->m_Clock.getClockSpeed() << " ns" << std::endl;
    std::cout << "Process Batches/Sec: " << nes->m_Clock.getBatchesPerSec() << std::endl;
    std::cout << "Cycles/Batch: " << nes->m_Clock.getCyclesPerBatch() << std::endl;
    std::cout << "Running: " << Tools::getYesNo(nes->m_Clock.isRunning()) << std::endl;
    std::cout << "Ticks: " << nes->m_Clock.getTicks() << std::endl;
    std::cout << "Memory Size: " << nes->m_CPU.getMemSize() << std::endl;
    std::cout << "Registers:" << std::endl;
    std::cout << "      PC: 0x" << std::hex << std::setw(4) << std::setfill('0') << int(nes->m_CPU.getPC()) << std::endl;
    std::cout << "   Stack: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(nes->m_CPU.getStackPtr()) << std::endl;
    std::cout << "     ACC: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(nes->m_CPU.getAcc()) << std::endl;
    std::cout << "       X: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(nes->m_CPU.getX()) << std::endl;
    std::cout << "       Y: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(nes->m_CPU.getY()) << std::endl;
    std::cout << std::endl;
    std::cout << " Status: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(nes->m_CPU.getStatus()) << std::endl;
    for (int i = 0; i < 8; i++)
    {
        Arch6502::STATUS_BIT status_bit = Arch6502::STATUS_BIT(i);
        std::cout << "  " << std::setw(9) << std::setfill(' ') << Arch6502::CPU::getStatusBitString(status_bit) << ": " << int(nes->m_CPU.getStatusBit(status_bit)) << std::endl;
    }
    
    std::cout << std::dec << std::endl;
}

void Console::doCPUExecute(std::vector<std::string> args)
{
    unsigned int count = 1;
    if (!args.empty())
    {
        count = (unsigned int)Tools::toInt(args[0]);
    }

    for (auto i = 0; i < count; i++)
    {
        uint16_t addr = nes->m_CPU.getPC();
        uint8_t opcode = nes->m_CPU.getAddr(nes->m_CPU.getPC(), false);
        int cycles = nes->m_CPU.execute();

        std::cout << "Executed CPU Instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode);
        std::cout << " @ 0x" << std::hex << std::setw(4) << std::setfill('0') << int(addr);
        std::cout << " in " << cycles << " cycle(s)" << std::endl;
    }
}

void Console::doCPUPC(std::vector<std::string> args)
{
    uint16_t val = uint16_t(Tools::toInt(args[0]));
    std::cout << "Setting program counter to " << std::hex << std::setw(4) << std::setfill('0') << val << std::endl;
    nes->m_CPU.setPC(val);
}

void Console::doCPUStart(std::vector<std::string> args)
{
    std::cout << "CPU Started: " << Tools::getYesNo(nes->m_Clock.start()) << std::endl;
}

void Console::doCPUStop(std::vector<std::string> args)
{
    std::cout << "CPU Stopped: " << Tools::getYesNo(nes->m_Clock.stop()) << std::endl;
}

void Console::doCPUShowOpcodes(std::vector<std::string> args)
{
    std::vector<uint8_t> opcodes;
    bool errors = false;
    for (int i = 0; i < 256; i++)
    {
        Arch6502::OpCode::initCodes();
        Arch6502::OpCode* opcode = Arch6502::OpCode::codes[i];
        if (opcode != nullptr)
        {
            if (opcode->getCode() == i)
            {
                std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << i << " == ";
                std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode->getCode()) << " ";
                std::cout << opcode->getMnemonic() << " " << Arch6502::getAddressModeString(opcode->getAddressMode()) << std::endl;
            }
            else
            {
                std::cerr << "ERROR: 0x" << std::hex << std::setw(2) << std::setfill('0') << i << " ";
                std::cout << " != 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode->getCode()) << std::endl;
                errors = true;
            }
        }
        if (errors)
        {
            std::cout << "Errors were found in opcodes." << std::endl;
        }
    }
}

void Console::doASM(std::vector<std::string> args)
{
    std::string infile = args[0];
    std::string outfile = Tools::getFilename(infile);
    std::string extension = Tools::getFileExtension(infile);
    outfile.resize(outfile.size() - extension.size());
    outfile += ".nes";

    std::cout << "Compiling assembly file \"" << infile << "\" to binary \"" << outfile << "\"" << std::endl;
    bool result = Arch6502::ASM::assemble(infile, outfile);
    std::cout << "Assembly successful = " << result << std::endl;
}

// NES

void Console::doNESOn(std::vector<std::string> args)
{
    std::cout << "Powering on NES." << std::endl;
    nes->on();
}

void Console::doNESOff(std::vector<std::string> args)
{
    std::cout << "Powering off NES." << std::endl;
    nes->off();
}

void Console::doNESReset(std::vector<std::string> args)
{
    std::cout << "Resetting NES to 0x" << std::hex << std::setw(4) << std::setfill('0') << int(nes->m_ResetVector) << std::endl;
    nes->reset();
}

void Console::doNESShow(std::vector<std::string> args)
{
    std::cout << "NES" << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "NES_CLOCK_HZ: " << NES_CLOCK_HZ << " Hz" << std::endl;
    std::cout << "On..........: " << Tools::getYesNo(nes->isOn()) << std::endl;
    std::cout << "Reset Vector: 0x" << std::hex << std::setw(4) << std::setfill('0') << int(nes->m_ResetVector) << std::endl;
}

void Console::doNESLoad(std::vector<std::string> args)
{
    NES::Cart cart(args[0]);
    if (cart.data)
    {
        std::cout << "Cart:" << std::endl;
        std::cout << "  Filename....: \"" << cart.filename << "\"" << std::endl;
        std::cout << "  Format......: " << NES::Cart::getFormatString(cart.format) << std::endl;
        std::cout << "  Size........: " << cart.data_size << std::endl;
        std::cout << "  Mapper......: " << int(cart.getMapper()) << std::endl;
        std::cout << "  SubMapper...: " << int(cart.getSubMapper()) << std::endl;
        std::cout << "  Console.....: " << NES::getConsoleTypeString(cart.getConsoleType()) << std::endl;
        std::cout << "  Timing Mode.: " << NES::getTimingModeString(cart.getTimingMode()) << std::endl;
        std::cout << "  Reset Vector: " << std::hex << std::setw(2) << std::setfill('0') << int(cart.getResetVector()) << std::endl;
        std::cout << "  Header......: ";
        for (int i = 0; i < 16; i++)
        {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << int(cart.data[i]) << " ";
        }
        std::cout << std::endl;
        std::cout << std::dec << std::setw(0);
        std::cout << "  PRG-ROM Size: " << cart.getPRGROMSize() << std::endl;
        std::cout << "  CHR-ROM Size: " << cart.getCHRROMSize() << std::endl;
        std::cout << "  RAM:" << std::endl;
        std::cout << "    Battery-backed/NV Ram: " << cart.hasNVRam() << std::endl;
        std::cout << "    PRG-RAM Size.........: " << cart.getPRGRAMSize() << std::endl;
        std::cout << "    CHR-RAM Size.........: " << cart.getCHRRAMSize() << std::endl;
        std::cout << "    PRG-NVRAM Size..: " << cart.getPRGNVRAMSize() << std::endl;
        std::cout << "    CHR-NVRAM Size..: " << cart.getCHRNVRAMSize() << std::endl;
        std::cout << "  Flags:" << std::endl;
        std::cout << "    Nametable Horizontally Arranged: " << cart.isNametableHorizontallyArranged() << std::endl;
        std::cout << "    Trainer........................: " << cart.hasTrainer() << std::endl;
        std::cout << "    Alternative Nametable Layout...: " << cart.hasAlternativeNametableLayout() << std::endl;
    }
    else
    {
        std::cout << "Failed to load \"" << args[0] << "\"" << std::endl;
    }
}