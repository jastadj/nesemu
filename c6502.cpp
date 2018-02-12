#include "c6502.hpp"

#include <sstream>
#include <vector>
#include <iomanip>
#include <fstream>

C6502::C6502(uint8_t *memory, unsigned int memory_size)
{
    m_MemSize = memory_size;
    m_Mem = memory;

    init();
}

bool C6502::init()
{
    // clear registers
    m_RegA = 0x0;
    m_RegX = 0x0;
    m_RegY = 0x0;

    // reset program counter and clear stack pointer
    m_RegPC = 0x0;
    m_RegSP = 0x0;

    // clear the status register
    m_RegStat = 0x0 | (0x1 << FLAG_NOT_USED); // bit 5 (not used) is always high

    // clear temporary variable that stores immediate values
    m_ImmediateTemp = 0x0;

    return true;
}

void C6502::printError(std::string errormsg)
{
    std::cout << errormsg << std::endl;
}

bool C6502::execute(uint8_t opcode)
{
    switch(opcode)
    {

    // ADC - Add memory to accumulator with carry
    case 0x69:
        ADC(IMMEDIATE);
        break;
    case 0x65:
        ADC(ZERO_PAGE);
        break;
    case 0x75:
        ADC(ZERO_PAGE_X);
        break;
    case 0x60:
        ADC(ABSOLUTE);
        break;
    case 0x70:
        ADC(ABSOLUTE_X);
        break;
    case 0x79:
        ADC(ABSOLUTE_Y);
        break;
    case 0x61:
        ADC(INDIRECT_X);
        break;
    case 0x71:
        ADC(INDIRECT_Y);
        break;

    //  AND - And memory with accumulator, store in accumulator
    case 0x29:
        AND(IMMEDIATE);
        break;
    case 0x25:
        AND(ZERO_PAGE);
        break;
    case 0x35:
        AND(ZERO_PAGE_X);
        break;
    case 0x2d:
        AND(ABSOLUTE);
        break;
    case 0x3d:
        AND(ABSOLUTE_X);
        break;
    case 0x39:
        AND(ABSOLUTE_Y);
        break;
    case 0x21:
        AND(INDIRECT_X);
        break;
    case 0x31:
        AND(INDIRECT_Y);
        break;

    //  ASL  - Shift left one bit (memory or accumulator)
    case 0x0a:
        ASL(ACCUMULATOR);
        break;
    case 0x06:
        ASL(ZERO_PAGE);
        break;
    case 0x16:
        ASL(ZERO_PAGE_X);
        break;
    case 0x0e:
        ASL(ABSOLUTE);
        break;
    case 0x1e:
        ASL(ABSOLUTE_X);
        break;

    // .....

    // LDA - Load accumulator with memory
    case 0xa9:
        LDA(IMMEDIATE);
        break;
    case 0xa5:
        LDA(ZERO_PAGE);
        break;
    case 0xb5:
        LDA(ZERO_PAGE_X);
        break;
    case 0xad:
        LDA(ABSOLUTE);
        break;
    case 0xbd:
        LDA(ABSOLUTE_X);
        break;
    case 0xb9:
        LDA(ABSOLUTE_Y);
        break;
    case 0xa1:
        LDA(INDIRECT_X);
        break;
    case 0xb1:
        LDA(INDIRECT_Y);
        break;

    // LDX - Load register x with memory
    case 0xa2:
        LDX(IMMEDIATE);
        break;
    case 0xa6:
        LDX(ZERO_PAGE);
        break;
    case 0xb6:
        LDX(ZERO_PAGE_Y);
        break;
    case 0xae:
        LDX(ABSOLUTE);
        break;
    case 0xbe:
        LDX(ABSOLUTE_Y);
        break;

    // LDY - Load register y with memory
    case 0xa0:
        LDY(IMMEDIATE);
        break;
    case 0xa4:
        LDY(ZERO_PAGE);
        break;
    case 0xb4:
        LDY(ZERO_PAGE_X);
        break;
    case 0xac:
        LDY(ABSOLUTE);
        break;
    case 0xbc:
        LDY(ABSOLUTE_X);
        break;

    default:
        return false;
        break;
    }
    return true;
}

// get status flag bit
bool C6502::getFlag(STAT_FLAG flag)
{
    return (m_RegStat >> flag) & 0x1;
}

// set status flag bit on/off
void C6502::setFlag(STAT_FLAG flag, bool on)
{
    if( flag == FLAG_NOT_USED) return;

    if(getFlag(flag) == on) return;

    if(on) m_RegStat |= (0x1 << flag);
    else m_RegStat ^= (0x1 << flag);
}

// return operand based on address mode
// IMMEDIATE, ZERO_PAGE, ZERO_PAGE_X, ABSOLUTE, ABSOLUTE_X, ABSOLUTE_Y, INDIRECT_X, INDIRECT_Y
uint8_t *C6502::getAddress(ADDRESS_MODE amode)
{
    switch(amode)
    {
    // immediate gets next mem byte
    case IMMEDIATE:
        m_ImmediateTemp = m_Mem[m_RegPC + 1];
        return &m_ImmediateTemp;
        break;
    // zero page gets address of 0x00YY where YY is next mem byte
    case ZERO_PAGE:
        return &m_Mem[ m_Mem[m_RegPC + 1] ];
        break;
    // zero page x gets address of 0x00YY where YY is REGX + next mem byte
    case ZERO_PAGE_X:
        return &m_Mem[ m_RegX + m_Mem[m_RegPC + 1] ];
        break;
    // zero page y gets address ox 0x00YY where YY is REGY + next mem byte
    case ZERO_PAGE_Y:
        return &m_Mem[ m_RegY + m_Mem[m_RegPC + 1] ];
        break;
    // ABSOLUTE gets address from next two bytes (LSB first)
    case ABSOLUTE:
        return &m_Mem[ ( (m_Mem[m_RegPC + 2]) << 8) + m_Mem[m_RegPC+1] ];
        break;
    // ABSOLUTE X gets address from next two bytes + REGX
    case ABSOLUTE_X:
        return &m_Mem[ ( (m_Mem[m_RegPC + 2]) << 8) + m_Mem[m_RegPC+1] + m_RegX];
        break;
    // ABSOLUTE X gets address from next two bytes + REGY
    case ABSOLUTE_Y:
            return &m_Mem[ ( (m_Mem[m_RegPC + 2]) << 8) + m_Mem[m_RegPC+1] + m_RegY];
        break;
    case INDIRECT_X:
        {
            uint16_t addr = m_RegX + m_Mem[m_RegPC + 1];
            if(addr > 0xff) addr -= 0xff; // rollover zero-page index
            return &m_Mem[  (m_Mem[addr+1] << 8) + m_Mem[addr]    ];
        }
        break;
    case INDIRECT_Y:
        {
            uint16_t lobyte = m_Mem[m_RegPC + 1];
            return &m_Mem[  (m_Mem[lobyte+1] << 8) + m_Mem[lobyte] + m_RegY ];
        }
        break;
    default:
        return NULL;
        break;
    }

    return NULL;
}

// add memory to accumulator with carry
// A + M + C -> A, C
void C6502::ADC(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    unsigned int temp = m_RegA + (*addr) + (getFlag(FLAG_CARRY) ? 1 : 0);
    setFlag(FLAG_ZERO, (temp == 0x0));

    switch(amode)
    {
        case IMMEDIATE:
            m_RegPC += 2;
            m_Cycles += 2;
            break;
        case ZERO_PAGE:
            m_RegPC += 2;
            m_Cycles += 3;
            break;
        case ZERO_PAGE_X:
            m_RegPC += 2;
            m_Cycles += 4;
            break;
        case ABSOLUTE:
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case ABSOLUTE_X:
            if(m_RegX + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case ABSOLUTE_Y:
            if(m_RegY + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case INDIRECT_X:
            m_RegPC += 2;
            m_Cycles += 6;
            break;
        case INDIRECT_Y:
            if( m_Mem[m_RegPC+1] + m_RegY > 0xff) m_Cycles++;
            m_RegPC += 2;
            m_Cycles += 5;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "ADC error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }


    if(getFlag(FLAG_DECIMAL_MODE))
    {
        if (((m_RegA & 0xf) + ( (*addr) & 0xf) + (getFlag(FLAG_CARRY) ? 1 : 0)) > 9) temp += 6;
        setFlag(FLAG_SIGN, (temp >> 7) & 0x1);
        setFlag(FLAG_OVERFLOW,!((m_RegA ^ (*addr) ) & 0x80) && ((m_RegA ^ temp) & 0x80));
        if (temp > 0x99) temp += 96;
    }
    else
    {
        setFlag(FLAG_SIGN, temp & 0x80 );
        setFlag(FLAG_OVERFLOW,!((m_RegA ^ (*addr) ) & 0x80) && ((m_RegA ^ temp) & 0x80));
        setFlag(FLAG_CARRY, temp > 0xff);
    }
    m_RegA = temp&0xff;
}

// AND memory with accumulator
// A&M -> A
void C6502::AND(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
        case IMMEDIATE:
            m_RegPC += 2;
            m_Cycles += 2;
            break;
        case ZERO_PAGE:
            m_RegPC += 2;
            m_Cycles += 3;
            break;
        case ZERO_PAGE_X:
            m_RegPC += 2;
            m_Cycles += 4;
            break;
        case ABSOLUTE:
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case ABSOLUTE_X:
            if(m_RegX + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case ABSOLUTE_Y:
            if(m_RegY + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case INDIRECT_X:
            m_RegPC += 2;
            m_Cycles += 6;
            break;
        case INDIRECT_Y:
            if( m_Mem[m_RegPC+1] + m_RegY > 0xff) m_Cycles++;
            m_RegPC += 2;
            m_Cycles += 5;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "AND error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegA = m_RegA & (*addr);
    setFlag(FLAG_SIGN, m_RegA & 0x80);
    setFlag(FLAG_ZERO, (m_RegA == 0x0));
}

// shift left one bit (memory or accumulator)
// M|A << 1
void C6502::ASL(ADDRESS_MODE amode) // null = accumulator
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
        case ACCUMULATOR:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        case ZERO_PAGE:
            m_RegPC += 2;
            m_Cycles += 5;
            break;
        case ZERO_PAGE_X:
            m_RegPC += 2;
            m_Cycles += 6;
            break;
        case ABSOLUTE:
            m_RegPC += 3;
            m_Cycles += 6;
            break;
        case ABSOLUTE_X:
            m_RegPC += 3;
            m_Cycles += 7;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "ASL error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(!addr)
    {
        setFlag(FLAG_CARRY, m_RegA & 0x80);
        m_RegA = (m_RegA << 1) & 0xff;
        setFlag(FLAG_ZERO, m_RegA == 0x00);
        setFlag(FLAG_SIGN, m_RegA & 0x80);
    }
    else
    {
        setFlag(FLAG_CARRY, *addr & 0x80);
        *addr = (*addr << 1) & 0xff;
        setFlag(FLAG_ZERO, *addr == 0x00);
        setFlag(FLAG_SIGN, *addr & 0x80);
    }
}

// load accumator with memory, a = m
// LDA
void C6502::LDA(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
        case IMMEDIATE:
            m_RegPC += 2;
            m_Cycles += 2;
            break;
        case ZERO_PAGE:
            m_RegPC += 2;
            m_Cycles += 3;
            break;
        case ZERO_PAGE_X:
            m_RegPC += 2;
            m_Cycles += 4;
            break;
        case ABSOLUTE:
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case ABSOLUTE_X:
            if(m_RegX + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case ABSOLUTE_Y:
            if(m_RegY + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case INDIRECT_X:
            m_RegPC += 2;
            m_Cycles += 6;
            break;
        case INDIRECT_Y:
            if( m_Mem[m_RegPC+1] + m_RegY > 0xff) m_Cycles++;
            m_RegPC += 2;
            m_Cycles += 5;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "LDA error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegA = *addr;
}

// load register x with memory, regx = m
// LDX
void C6502::LDX(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
        case IMMEDIATE:
            m_RegPC += 2;
            m_Cycles += 2;
            break;
        case ZERO_PAGE:
            m_RegPC += 2;
            m_Cycles += 3;
            break;
        case ZERO_PAGE_Y:
            m_RegPC += 2;
            m_Cycles += 4;
            break;
        case ABSOLUTE:
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case ABSOLUTE_Y:
            if(m_RegY + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "LDX error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegX = *addr;
}

// load register y with memory, regy = m
// LDY
void C6502::LDY(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
        case IMMEDIATE:
            m_RegPC += 2;
            m_Cycles += 2;
            break;
        case ZERO_PAGE:
            m_RegPC += 2;
            m_Cycles += 3;
            break;
        case ZERO_PAGE_X:
            m_RegPC += 2;
            m_Cycles += 4;
            break;
        case ABSOLUTE:
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        case ABSOLUTE_X:
            if(m_RegY + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "LDY error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegY = *addr;
}


//////////////////////////////
// DEBUG CONSOLE
void C6502::debugConsole()
{
    bool quit = false;

    while(!quit)
    {
        std::string buf;
        std::vector<std::string> words;

        std::cout << "C6502> ";

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

        }
        else if(words[0] == "show")
        {
            std::cout << "C6502" << std::endl;
            std::cout << "-----" << std::endl;
            std::cout << "Accumulator      = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegA) << std::endl;
            std::cout << "Register X       = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegX) << std::endl;
            std::cout << "Register Y       = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegY) << std::endl;
            std::cout << "Stack Pointer    = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegSP) << std::endl;
            std::cout << "Program Counter  = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegPC) << std::endl;
            std::cout << "Flags:" << std::endl;
            std::cout << "  Carry            = " << getFlag(FLAG_CARRY) << std::endl;
            std::cout << "  Zero             = " << getFlag(FLAG_ZERO) << std::endl;
            std::cout << "  Interrupt Enable = " << getFlag(FLAG_INTERRUPT_ENABLE) << std::endl;
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
                        m_Mem[addr] = uint8_t(wval);
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
                    std::cout << std::setfill('0') << std::setw(2) << int(m_Mem[addr+i]) << std::endl;
                }

            }
        }
        else if(words[0] == "step")
        {
            std::cout << "Executing opcode : " << std::hex << int(m_Mem[m_RegPC]) << std::endl;
            if(!execute(m_Mem[m_RegPC]))
            {
                std::cout << "Opcode undefined : " << std::hex << int(m_Mem[m_RegPC]) << std::endl;
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
                m_Mem[i] = 0x0;
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
                        std::cout << std::hex << std::setfill('0') << std::setw(2) << int(m_Mem[i*16 + n]) << " ";
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
                    if(m_Mem[i] != 0)
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
                    for(unsigned int i = 0; i <= lastentry; i++) ofile.put( (unsigned char)( int(m_Mem[i])) );
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
                            m_Mem[loffset + bytes] = data;
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
        else
        {
            std::cout << "Unknown command - type help" << std::endl;
        }
    }
}
