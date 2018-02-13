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
    case 0x7D:
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

    // BCC - branch on carry clear (c==0)
    case 0x90:
        BCC(RELATIVE);
        break;

    // BCS - branch on carry set (c == 1)
    case 0xb0:
        BCS(RELATIVE);
        break;

    // BIT - test bits in memory with accumulator
    case 0x24:
        BIT(ZERO_PAGE);
        break;
    case 0x2c:
        BIT(ABSOLUTE);
        break;

    // BMI - branch on result minus, n == 1
    case 0x30:
        BMI(RELATIVE);
        break;

    // BNE - branch on result not zero, Z == 0
    case 0xd0:
        BNE(RELATIVE);
        break;

    // BPL - branch on result plus, n == 0
    case 0x10:
        BPL(RELATIVE);
        break;

    // BRK - force break
    case 0x00:
        BRK(IMPLIED);
        break;

    // BVC - branch on overflow clear, v == 0
    case 0x50:
        BVC(RELATIVE);
        break;

    // BVS - branch on overflow set, v == 1
    case 0x70:
        BVS(RELATIVE);
        break;

    // CLC - clear carry flag, c = 0
    case 0x18:
        CLC(IMPLIED);
        break;

    // CLD - clear decimal mode, d = 0
    case 0xd8:
        CLD(IMPLIED);
        break;

    // CLI - clear interrupt disable flag, i = 0
    case 0x58:
        CLI(IMPLIED);
        break;

    // CLV - clear overflow flag, v = 0
    case 0xb8:
        CLV(IMPLIED);
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
    // accumulator address
    case ACCUMULATOR:
        return &m_RegA;
        break;
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
    case RELATIVE:
        {
            int8_t offset = m_Mem[ m_RegPC + 1];
            int32_t pc = m_RegPC + offset;
            return &m_Mem[ uint16_t(pc)];
        }
        break;
    case IMPLIED:
        return NULL;
        break;
    default:
        std::cout << "Error, access mode " << amode << " is undefined.  Returning NULL." << std::endl;
        return NULL;
        break;
    }

    return NULL;
}
