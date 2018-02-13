#include "c6502.hpp"

#include <sstream>
#include <vector>
#include <iomanip>
#include <fstream>

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

// ASL shift left one bit (memory or accumulator)
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

    setFlag(FLAG_CARRY, *addr & 0x80);
    *addr = (*addr << 1) & 0xff;
    setFlag(FLAG_ZERO, *addr == 0x00);
    setFlag(FLAG_SIGN, *addr & 0x80);

}

// BCC branch on carry clear
// branch if c == 0
void C6502::BCC(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case RELATIVE:
            m_Cycles += 2;
            m_RegPC += 2;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "BCC error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(!getFlag(FLAG_CARRY))
    {
        int8_t offset = m_Mem[ (m_RegPC-2) + 1];
        int32_t pc = (m_RegPC-2) + offset;

        if( ( (m_RegPC-2) & 0xff00) != ( pc & 0xff00) ) m_Cycles += 2;
        else m_Cycles += 1;

        m_RegPC = uint16_t(pc);
    }
}

// BCS branch on carry set
// branch if c == 1
void C6502::BCS(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case RELATIVE:
            m_Cycles += 2;
            m_RegPC += 2;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "BCS error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(getFlag(FLAG_CARRY))
    {
        int8_t offset = m_Mem[ (m_RegPC-2) + 1];
        int32_t pc = (m_RegPC-2) + offset;

        if( ( (m_RegPC-2) & 0xff00) != ( pc & 0xff00) ) m_Cycles += 2;
        else m_Cycles += 1;

        m_RegPC = uint16_t(pc);
    }
}

// BEQ branch on zero flag
// branch if z == 1
void C6502::BEQ(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case RELATIVE:
            m_Cycles += 2;
            m_RegPC += 2;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "BEQ error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(getFlag(FLAG_ZERO))
    {
        int8_t offset = m_Mem[ (m_RegPC-2) + 1];
        int32_t pc = (m_RegPC-2) + offset;

        if( ( (m_RegPC-2) & 0xff00) != ( pc & 0xff00) ) m_Cycles += 2;
        else m_Cycles += 1;

        m_RegPC = uint16_t(pc);
    }
}

// BIT - test bits in memory against accumulator
// A & M, M7 -> N, M6 -> V
// if result == 0, Z = 1
void C6502::BIT(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
        case ZERO_PAGE:
            m_RegPC += 2;
            m_Cycles += 3;
            break;
        case ABSOLUTE:
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "BIT error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    setFlag(FLAG_SIGN, (*addr) & 0x80);
    setFlag(FLAG_OVERFLOW, (*addr) & 0x40);
    setFlag(FLAG_ZERO, m_RegA & (*addr));

}

// BMI - branch on result minus
// if n == 1 (negative), branch
void C6502::BMI(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case RELATIVE:
            m_Cycles += 2;
            m_RegPC += 2;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "BMI error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(getFlag(FLAG_SIGN))
    {
        int8_t offset = m_Mem[ (m_RegPC-2) + 1];
        int32_t pc = (m_RegPC-2) + offset;

        if( ( (m_RegPC-2) & 0xff00) != ( pc & 0xff00) ) m_Cycles += 2;
        else m_Cycles += 1;

        m_RegPC = uint16_t(pc);
    }
}

// BNE - branch on result not zero
// if z == 0 (not zero), branch
void C6502::BNE(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case RELATIVE:
            m_Cycles += 2;
            m_RegPC += 2;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "BNE error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(!getFlag(FLAG_ZERO))
    {
        int8_t offset = m_Mem[ (m_RegPC-2) + 1];
        int32_t pc = (m_RegPC-2) + offset;

        if( ( (m_RegPC-2) & 0xff00) != ( pc & 0xff00) ) m_Cycles += 2;
        else m_Cycles += 1;

        m_RegPC = uint16_t(pc);
    }
}

// BPL - branch on result not negative
// if n == 0 (not negative), branch
void C6502::BPL(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case RELATIVE:
            m_Cycles += 2;
            m_RegPC += 2;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "BPL error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(!getFlag(FLAG_SIGN))
    {
        int8_t offset = m_Mem[ (m_RegPC-2) + 1];
        int32_t pc = (m_RegPC-2) + offset;

        if( ( (m_RegPC-2) & 0xff00) != ( pc & 0xff00) ) m_Cycles += 2;
        else m_Cycles += 1;

        m_RegPC = uint16_t(pc);
    }
}

// BRK - force break
// ... not implemented
void C6502::BRK(ADDRESS_MODE amode)
{
    std::cout << "BRK NOT IMPLEMENTED" << std::endl;

    switch(amode)
    {
    case IMPLIED:
        m_RegPC++;
        m_Cycles += 7;
        break;
    default:
        {
            std::stringstream emsg;
            emsg << "BRK error address mode @ PC:" << std::hex << "0x" << m_RegPC;
            printError(emsg.str());
        }
        return;
        break;
    }
}

// BVC - branch on overflow clear
// v == 0
void C6502::BVC(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case RELATIVE:
            m_Cycles += 2;
            m_RegPC += 2;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "BVC error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(!getFlag(FLAG_OVERFLOW))
    {
        int8_t offset = m_Mem[ (m_RegPC-2) + 1];
        int32_t pc = (m_RegPC-2) + offset;

        if( ( (m_RegPC-2) & 0xff00) != ( pc & 0xff00) ) m_Cycles += 2;
        else m_Cycles += 1;

        m_RegPC = uint16_t(pc);
    }
}

// BVS - branch on overflow set
// v == 1
void C6502::BVS(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case RELATIVE:
            m_Cycles += 2;
            m_RegPC += 2;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "BVS error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(getFlag(FLAG_OVERFLOW))
    {
        int8_t offset = m_Mem[ (m_RegPC-2) + 1];
        int32_t pc = (m_RegPC-2) + offset;

        if( ( (m_RegPC-2) & 0xff00) != ( pc & 0xff00) ) m_Cycles += 2;
        else m_Cycles += 1;

        m_RegPC = uint16_t(pc);
    }
}

// CLC - clear carry flag
// c = 0
void C6502::CLC(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_Cycles += 2;
            m_RegPC += 1;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "CLC error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    setFlag(FLAG_CARRY, false);
}

// CLD - clear decimal mode flag
// d = 0
void C6502::CLD(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_Cycles += 2;
            m_RegPC += 1;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "CLD error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    setFlag(FLAG_DECIMAL_MODE, false);
}

// CLI - clear interrupt disable flag
// d = 0
void C6502::CLI(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_Cycles += 2;
            m_RegPC += 1;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "CLI error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    setFlag(FLAG_INTERRUPT_DISABLE, false);
}

// CLV - clear overflow flag
// v = 0
void C6502::CLV(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_Cycles += 2;
            m_RegPC += 1;
        break;
        default:
            {
                std::stringstream emsg;
                emsg << "CLV error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    setFlag(FLAG_OVERFLOW, false);
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




