#include "c6502.hpp"

#include <sstream>
#include <vector>
#include <iomanip>
#include <fstream>

//////////////////////////////
// DEBUG CONSOLE
void C6502::debugConsole(std::string prompt)
{
    bool quit = false;

    while(!quit)
    {
        std::string buf;
        std::vector<std::string> words;

        std::cout << prompt;

        std::getline(std::cin, buf);

        // strip words and white space
        while(!buf.empty())
        {
            size_t cpos = buf.find_first_of(' ');

            if(cpos == 0)
            {
                buf.erase(0,1);
                continue;
            }

            words.push_back( buf.substr(0, cpos));
            buf.erase(0, cpos);
        }

        if(words.empty()) continue;

        if(words[0] == "quit" || words[0] == "exit") quit = true;
        else if(words[0] == "help")
        {
            std::cout << "quit - exit console" << std::endl;
            std::cout << "help - show this menu" << std::endl;
            std::cout << "show - show relevant CPU information" << std::endl;
            std::cout << "step - step next instruction" << std::endl;
            std::cout << "r <addr> [count] - read value at memory address and optional additional bytes" << std::endl;
            std::cout << "w <addr> <byte> - write byte to memory address" << std::endl;
            std::cout << "reset - clear registers, stack pointer, p counter" << std::endl;
            std::cout << "clearmem - clear all memory" << std::endl;
            std::cout << "dumpmem [file] - dump memory, optionally to file" << std::endl;
            std::cout << "loadmem <file> [offset] - load memory from file at optional offset" << std::endl;
            std::cout << "seta <byte> - set accumulator" << std::endl;
            std::cout << "setx <byte> - set register x" << std::endl;
            std::cout << "sety <byte> - set register y" << std::endl;
            std::cout << "setpc <address> - set program counter to address" << std::endl;
            std::cout << "setcarry <0|1> - set flag" << std::endl;
            std::cout << "setzero <0|1> - set flag" << std::endl;
            std::cout << "setinterruptenable <0|1> - set flag" << std::endl;
            std::cout << "setdecimalmode <0|1> - set flag" << std::endl;
            std::cout << "setsoftwareinterrupt <0|1> - set flag" << std::endl;
            std::cout << "setoverflow <0|1> - set flag" << std::endl;
            std::cout << "setsign <0|1> - set flag" << std::endl;
        }
        else if(words[0] == "show")
        {
            std::cout << "C6502" << std::endl;
            std::cout << "-----" << std::endl;
            std::cout << "Cycles           = " << std::dec << m_Cycles << std::endl;
            std::cout << "Accumulator      = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegA) << std::endl;
            std::cout << "Register X       = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegX) << std::endl;
            std::cout << "Register Y       = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegY) << std::endl;
            std::cout << "Stack Pointer    = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegSP) << std::endl;
            std::cout << "Program Counter  = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegPC) << std::endl;
            std::cout << "Flags:" << std::endl;
            std::cout << "  Carry            = " << getFlag(FLAG_CARRY) << std::endl;
            std::cout << "  Zero             = " << getFlag(FLAG_ZERO) << std::endl;
            std::cout << "  Interrupt Enable = " << getFlag(FLAG_INTERRUPT_DISABLE) << std::endl;
            std::cout << "  Decimal Mode     = " << getFlag(FLAG_DECIMAL_MODE) << std::endl;
            std::cout << "  SW Interrupt     = " << getFlag(FLAG_SOFTWARE_INTERRUPT) << std::endl;
            std::cout << "  NOT USED         = " << getFlag(FLAG_NOT_USED) << std::endl;
            std::cout << "  Overflow         = " << getFlag(FLAG_OVERFLOW) << std::endl;
            std::cout << "  Sign             = " << getFlag(FLAG_SIGN) << std::endl;

        }
        else if(words[0] == "w" || words[0] == "r")
        {
            uint16_t addr;
            std::stringstream addrss;

            if(words[1].size() >= 3)
                if(words[1][1] == 'x') words[1].erase(0,2);

            // convert hex string to value
            addrss << std::hex << words[1];
            addrss >> addr;

            // write memory
            if(words[0] == "w")
            {
                if(words.size() == 3)
                {
                    int wval;
                    std::stringstream wss;

                    if(words[2].size() >= 3)
                        if(words[2][1] == 'x') words[2].erase(0,2);

                    wss << std::hex << words[2];
                    wss >> wval;

                    if(wval <= 0xff)
                    {
                        std::cout << std::hex << std::setfill('0') << std::setw(4) << addr << " = " << wval << std::endl;
                        *m_Mem[addr] = uint8_t(wval);
                    }
                    else std::cout << "Value larger than 1 byte!" << std::endl;
                }
                else std::cout << "Invalid parameters!  w <addr> <byte>" << std::endl;
            }
            // read memory
            else
            {
                int bcount = 1;
                if(words.size() == 3) bcount = atoi(words[2].c_str());

                for(int i = 0; i < bcount; i++)
                {
                    std::cout << std::hex << std::setfill('0') << std::setw(4) << addr+i << ": ";
                    std::cout << std::setfill('0') << std::setw(2) << int(*m_Mem[addr+i]) << std::endl;
                }

            }
        }
        else if(words[0] == "step")
        {
            std::cout << "Executing opcode : " << std::hex << int(*m_Mem[m_RegPC]) << std::endl;
            if(!execute(*m_Mem[m_RegPC]))
            {
                std::cout << "Opcode undefined : " << std::hex << int(*m_Mem[m_RegPC]) << std::endl;
            }
        }
        else if(words[0] == "reset")
        {
            std::cout << "Resetting C6502..." << std::endl;
            init();
        }
        else if(words[0] == "clearmem")
        {
            for(unsigned int i = 0; i < m_MemSize; i++)
            {
                *m_Mem[i] = 0x0;
            }
        }
        else if(words[0] == "dumpmem")
        {
            if(words.size() == 1)
            {
                for(unsigned int i = 0; i < m_MemSize/16; i++)
                {
                    std::cout << std::hex << std::setfill('0') << std::setw(4) << i*16 << ": ";
                    for(int n = 0; n < 16; n++)
                        std::cout << std::hex << std::setfill('0') << std::setw(2) << int(*m_Mem[i*16 + n]) << " ";
                    std::cout << std::endl;
                }
            }
            else if(words.size() == 2)
            {
                std::ofstream ofile;
                unsigned int lastentry = 0;

                // find last non zero data
                for(int i = m_MemSize-1; i >= 0; i--)
                {
                    if(*m_Mem[i] != 0)
                    {
                        lastentry = i;
                        break;
                    }
                }

                // open output file for writing
                ofile.open(words[1].c_str(), std::ios::binary);

                if(ofile.is_open())
                {

                    // write all memory to file, stopping at last non-zero address
                    for(unsigned int i = 0; i <= lastentry; i++) ofile.put( (unsigned char)( int(*m_Mem[i])) );
                    ofile.close();
                    std::cout << "Wrote " << std::dec << lastentry + 1 << " bytes to " << words[1] << std::endl;

                }
                else std::cout << "Error opening file " << words[1] << std::endl;
            }

        }
        else if(words[0] == "loadmem")
        {
            int loffset = 0;

            if(words.size() >= 2 && words.size() <= 3)
            {
                if(words.size() == 3) loffset = atoi(words[2].c_str());

                std::ifstream ifile;

                ifile.open(words[1].c_str(), std::ios::binary);

                if(ifile.is_open())
                {
                    int bytes = 0;

                    while(!ifile.eof())
                    {
                        uint8_t data = uint8_t(ifile.get());
                        if(!ifile.eof())
                        {
                            *m_Mem[loffset + bytes] = data;
                            bytes++;
                        }
                    }
                    std::cout << "Loaded " << std::dec << bytes << " bytes from " << words[1];
                    std::cout << " starting at 0x" << std::hex << std::setfill('0') << std::setw(4) << loffset << std::endl;
                }
                else std::cout << "Error opening file " << words[1] << std::endl;

            }
            else std::cout << "Incorrect parameters : loadmem <file> [offset]" << std::endl;
        }
        else if(words[0] == "seta")
        {
            if(words.size() == 2)
            {
                int val;

                std::stringstream bss;
                bss << std::hex << words[1];

                bss >> val;
                if( val <= 0xff)
                {
                    m_RegA = val;
                    std::cout << "Accumulator = " << std::hex << std::setfill('0') << std::setw(2) << int(m_RegA) << std::endl;
                }
                else std::cout << "Value is larger than 1 byte : " << val << std::endl;
            }
            else std::cout << "Invalid parameters." << std::endl;
        }
        else if(words[0] == "setx")
        {
            if(words.size() == 2)
            {
                int val;

                std::stringstream bss;
                bss << std::hex << words[1];

                bss >> val;
                if( val <= 0xff)
                {
                    m_RegX = val;
                    std::cout << "Register X = " << std::hex << std::setfill('0') << std::setw(2) << int(m_RegX) << std::endl;
                }
                else std::cout << "Value is larger than 1 byte : " << val << std::endl;
            }
            else std::cout << "Invalid parameters." << std::endl;
        }
        else if(words[0] == "sety")
        {
            if(words.size() == 2)
            {
                int val;

                std::stringstream bss;
                bss << std::hex << words[1];

                bss >> val;
                if( val <= 0xff)
                {
                    m_RegY = val;
                    std::cout << "Register Y = " << std::hex << std::setfill('0') << std::setw(2) << int(m_RegY) << std::endl;
                }
                else std::cout << "Value is larger than 1 byte : " << val << std::endl;
            }
            else std::cout << "Invalid parameters." << std::endl;
        }
        else if(words[0] == "setpc")
        {
            if(words.size() == 2)
            {
                unsigned int addr;

                std::stringstream wss;
                // strip 0x prefix if found
                if(words[1][1] == 'x') words[1].erase(0,2);

                wss << std::hex << words[1];
                wss >> addr;

                m_RegPC = uint16_t(addr);

            }
        }
        else if(words[0] == "setcarry")
        {
            if(words.size() == 2) {setFlag(FLAG_CARRY, atoi(words[1].c_str())); std::cout << "Flag set." << std::endl;}
            else std::cout << "Invalid parameters.  Set flag 1 or 0." << std::endl;
        }
        else if(words[0] == "setzero")
        {
            if(words.size() == 2) {setFlag(FLAG_ZERO, atoi(words[1].c_str())); std::cout << "Flag set." << std::endl;}
            else std::cout << "Invalid parameters.  Set flag 1 or 0." << std::endl;
        }
        else if(words[0] == "setinterruptenable")
        {
            if(words.size() == 2) {setFlag(FLAG_INTERRUPT_DISABLE, atoi(words[1].c_str())); std::cout << "Flag set." << std::endl;}
            else std::cout << "Invalid parameters.  Set flag 1 or 0." << std::endl;
        }
        else if(words[0] == "setdecimalmode")
        {
            if(words.size() == 2) {setFlag(FLAG_DECIMAL_MODE, atoi(words[1].c_str())); std::cout << "Flag set." << std::endl;}
            else std::cout << "Invalid parameters.  Set flag 1 or 0." << std::endl;
        }
        else if(words[0] == "setsoftwareinterrupt")
        {
            if(words.size() == 2) {setFlag(FLAG_SOFTWARE_INTERRUPT, atoi(words[1].c_str())); std::cout << "Flag set." << std::endl;}
            else std::cout << "Invalid parameters.  Set flag 1 or 0." << std::endl;
        }
        else if(words[0] == "setoverflow")
        {
            if(words.size() == 2) {setFlag(FLAG_OVERFLOW, atoi(words[1].c_str())); std::cout << "Flag set." << std::endl;}
            else std::cout << "Invalid parameters.  Set flag 1 or 0." << std::endl;
        }
        else if(words[0] == "setsign")
        {
            if(words.size() == 2) {setFlag(FLAG_SIGN, atoi(words[1].c_str())); std::cout << "Flag set." << std::endl;}
            else std::cout << "Invalid parameters.  Set flag 1 or 0." << std::endl;
        }
        else
        {
            std::cout << "Unknown command - type help" << std::endl;
        }
    }
}
