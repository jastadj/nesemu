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

// CMP - compare memory and accumulator
// A - M
void C6502::CMP(ADDRESS_MODE amode)
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
                emsg << "CMP error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    setFlag(FLAG_ZERO, m_RegA == *addr);
    setFlag(FLAG_CARRY, m_RegA >= *addr); // carry flag = 0 if borrow required, 1 if not
    setFlag(FLAG_SIGN, (m_RegA - *addr) & 0x80);
}

// CPX - comparey register x with memory
// register x - m
void C6502::CPX(ADDRESS_MODE amode)
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
        case ABSOLUTE:
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "CPX error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    setFlag(FLAG_ZERO, m_RegX == *addr);
    setFlag(FLAG_CARRY, m_RegX >= *addr); // carry flag = 0 if borrow required, 1 if not
    setFlag(FLAG_SIGN, (m_RegX - *addr) & 0x80);
}

// CPY - comparey register y with memory
// register y - m
void C6502::CPY(ADDRESS_MODE amode)
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
        case ABSOLUTE:
            m_RegPC += 3;
            m_Cycles += 4;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "CPY error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    setFlag(FLAG_ZERO, m_RegY == *addr);
    setFlag(FLAG_CARRY, m_RegY >= *addr); // carry flag = 0 if borrow required, 1 if not
    setFlag(FLAG_SIGN, (m_RegY - *addr) & 0x80);
}

// DEC - decrement memory by 1
// m--
void C6502::DEC(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
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
                emsg << "DEC error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    *addr = *addr - 1;

    setFlag(FLAG_ZERO, *addr == 0x0);
    setFlag(FLAG_SIGN, *addr & 0x80);

}

// DEX - decrement register x by 1
// reg x --
void C6502::DEX(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "DEX error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegX = m_RegX - 1;

    setFlag(FLAG_ZERO, m_RegX == 0x0);
    setFlag(FLAG_SIGN, m_RegX & 0x80);
}

// DEY - decrement register y by 1
// reg y --
void C6502::DEY(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "DEY error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegY = m_RegY - 1;

    setFlag(FLAG_ZERO, m_RegY == 0x0);
    setFlag(FLAG_SIGN, m_RegY & 0x80);

}

// EOR - exclusive or memory with accumulator
// m ^ a -> a
void C6502::EOR(ADDRESS_MODE amode)
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
                emsg << "EOR error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegA = m_RegA ^ (*addr);

    setFlag(FLAG_ZERO, m_RegA == 0x0);
    setFlag(FLAG_SIGN, m_RegA & 0x80);

}

// INC - increment memory by 1
// m++
void C6502::INC(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
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
                emsg << "INC error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    *addr = *addr + 1;

    setFlag(FLAG_ZERO, *addr == 0x0);
    setFlag(FLAG_SIGN, *addr & 0x80);

}

// INX - increment register x by 1
// reg x ++
void C6502::INX(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "INX error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegX = m_RegX + 1;

    setFlag(FLAG_ZERO, m_RegX == 0x0);
    setFlag(FLAG_SIGN, m_RegX & 0x80);
}

// INY - increment register y by 1
// reg y ++
void C6502::INY(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "INY error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegY = m_RegY + 1;

    setFlag(FLAG_ZERO, m_RegY == 0x0);
    setFlag(FLAG_SIGN, m_RegY & 0x80);
}

// JMP - jump to new location
// pc + 1 = PClowbyte, pc + 2 = PChighbyte
void C6502::JMP(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case ABSOLUTE:
            m_Cycles += 3;
            m_RegPC = ( (m_Mem[m_RegPC + 2]) << 8) + m_Mem[m_RegPC+1];
            break;
        case INDIRECT:
            m_Cycles += 5;
            {
                uint16_t lobyte = m_Mem[m_RegPC + 1] + (m_Mem[m_RegPC + 2] << 8);
                m_RegPC = m_Mem[lobyte] + (m_Mem[lobyte+1] << 8);
            }
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "JMP error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

}

// JSR - jump and save return address to stack
// push high byte first, and lobyte second so that it pops off lo byte first
void C6502::JSR(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case ABSOLUTE:
            m_Cycles += 6;
            pushStack( (m_RegPC >> 8) & 0xff);
            pushStack(m_RegPC & 0xff);
            m_RegPC = ( (m_Mem[m_RegPC + 2]) << 8) + m_Mem[m_RegPC+1];
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "JSR error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
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

// LSR - shift right one bit (memory or accumulator)
// m | a >> 1
void C6502::LSR(ADDRESS_MODE amode)
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
                emsg << "LSR error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    setFlag(FLAG_CARRY, *addr & 0x1);
    *addr = (*addr >> 1) & 0xff;
    setFlag(FLAG_ZERO, *addr == 0x00);
    setFlag(FLAG_SIGN, false);
}

// NOP - no operation
// 2 cycle burn
void C6502::NOP(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "NOP error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }
}

// ORA - or memory with accumulator
// a | m -> a
void C6502::ORA(ADDRESS_MODE amode)
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
            //if( m_Mem[m_RegPC+1] + m_RegY > 0xff) m_Cycles++;
            m_RegPC += 2;
            m_Cycles += 5;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "ORA error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegA = m_RegA | (*addr);

    setFlag(FLAG_ZERO, m_RegA == 0x0);
    setFlag(FLAG_SIGN, m_RegA & 0x80);
}

// PHA - push accumulator on stack
void C6502::PHA(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 3;
            pushStack(m_RegA);
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "PHA error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

}

// PHA - push accumulator on stack
void C6502::PHP(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 3;
            pushStack(m_RegStat);
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "PHP error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

}

// PLA - pull accumulator from stack
void C6502::PLA(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 4;
            m_RegA = popStack();
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "PLA error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

}

// PLP - pull status register from stack
void C6502::PLP(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 4;
            m_RegStat = popStack();
            //m_RegStat = m_RegStat | (0x1 << FLAG_NOT_USED);
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "PLP error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

}

// ROL - rotate one bit left
// memory or accumulator
void C6502::ROL(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    uint8_t original = *addr;

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
                emsg << "ROL error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    *addr = ( (*addr << 1) & 0xff) | ( (original >> 7) & 0x1);

    setFlag(FLAG_CARRY, (original >> 7) & 0x1 );
    setFlag(FLAG_ZERO, *addr == 0x0);
    setFlag(FLAG_SIGN, *addr & 0x80);
}

// ROR - rotate one bit right
// memory or accumulator
void C6502::ROR(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    uint8_t original = *addr;

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
                emsg << "ROR error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    *addr = ( (*addr >> 1) & 0xff) | ( (original & 0x1 ) << 7 );

    setFlag(FLAG_CARRY, (original & 0x1 ) << 7  );
    setFlag(FLAG_ZERO, *addr == 0x0);
    setFlag(FLAG_SIGN, *addr & 0x80);
}

// RTI - return from interrupt
// status from stack, pc from stack
void C6502::RTI(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 6;
            m_RegStat = popStack();
            m_RegPC = popStack();
            m_RegPC = m_RegPC + ( popStack() << 8 );
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "RTI error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }
}

// RTS - return from subroutine
//  pc from stack
void C6502::RTS(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 6;
            m_RegPC = popStack();
            m_RegPC = m_RegPC + ( popStack() << 8 );
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "RTS error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }
}

// SBC - subtract memory from accumulator with borrow
// a - m - c -> a
void C6502::SBC(ADDRESS_MODE amode)
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
                emsg << "SBC error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    unsigned int temp = m_RegA - (*addr) - (getFlag(FLAG_CARRY) ? 1 : 0);

    setFlag(FLAG_SIGN, m_RegA & 0x80);
    setFlag(FLAG_ZERO, m_RegA == 0x0); // not valid in decimal mode
    setFlag(FLAG_OVERFLOW, ((m_RegA ^ temp) & 0x80) && ((m_RegA ^ *addr) & 0x80) );

    if(getFlag(FLAG_DECIMAL_MODE))
    {
        if ( ((m_RegA & 0xf) - ( getFlag(FLAG_CARRY) ? 0 : 1)) < (*addr & 0xf)) temp -= 6;
        if (temp > 0x99) temp -= 0x60;
    }
    setFlag(FLAG_CARRY, temp < 0x100);

    m_RegA = temp & 0xff;

}

// SEC - set carry flag
void C6502::SEC(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            setFlag(FLAG_CARRY, true);
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "SEC error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }
}

// SED - set decimal mode
void C6502::SED(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            setFlag(FLAG_DECIMAL_MODE, true);
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "SED error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }
}

// SEI - set interrupt disable flag
void C6502::SEI(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            setFlag(FLAG_INTERRUPT_DISABLE, true);
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "SEI error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }
}

// STA - store accumulator in memory
// m = accumulator
void C6502::STA(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
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
            //if(m_RegX + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 5;
            break;
        case ABSOLUTE_Y:
            //if(m_RegY + m_RegPC+1 > 0xff) m_Cycles++;
            m_RegPC += 3;
            m_Cycles += 5;
            break;
        case INDIRECT_X:
            m_RegPC += 2;
            m_Cycles += 6;
            break;
        case INDIRECT_Y:
            //if( m_Mem[m_RegPC+1] + m_RegY > 0xff) m_Cycles++;
            m_RegPC += 2;
            m_Cycles += 6;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "STA error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    *addr = m_RegA;
}

// STX - store reg x in memory
// m = reg x
void C6502::STX(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
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
        default:
            {
                std::stringstream emsg;
                emsg << "STX error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    *addr = m_RegX;

}

// STY - store reg y in memory
// m = reg y
void C6502::STY(ADDRESS_MODE amode)
{
    uint8_t *addr = getAddress(amode);

    switch(amode)
    {
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
        default:
            {
                std::stringstream emsg;
                emsg << "STY error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    *addr = m_RegY;

}

// TAX - transfer accumulator to reg x
// reg x = a
void C6502::TAX(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "TAX error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegX = m_RegA;

    setFlag(FLAG_SIGN, m_RegX & 0x80);
    setFlag(FLAG_ZERO, m_RegX == 0x0);
}

// TAY - transfer accumulator to reg y
// reg y = a
void C6502::TAY(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "TAY error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegY = m_RegA;

    setFlag(FLAG_SIGN, m_RegY & 0x80);
    setFlag(FLAG_ZERO, m_RegY == 0x0);
}

// TSX- transfer stack pointer to register x
// S -> reg x
void C6502::TSX(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "TSX error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(!m_Stack.empty())
    {
        m_RegX = m_Stack.back();

        setFlag(FLAG_SIGN, m_RegX & 0x80);
        setFlag(FLAG_ZERO, m_RegX == 0x0);
    }
    else
    {
        std::cout << "TSX : Warning, there is nothing on the stack to transfer!" << std::endl;
    }
}

// TXA- transfer reg x to accumulator
// a = reg x
void C6502::TXA(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "TXA error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegA = m_RegX;

    setFlag(FLAG_SIGN, m_RegA & 0x80);
    setFlag(FLAG_ZERO, m_RegA == 0x0);
}

// TXS- transfer reg x to stack pointer
// a = reg x
void C6502::TXS(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "TXS error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    if(!m_Stack.empty())
    {
        m_Stack.back() = m_RegX;
    }
}

// TYA- transfer reg y to accumulator
// a = reg y
void C6502::TYA(ADDRESS_MODE amode)
{
    switch(amode)
    {
        case IMPLIED:
            m_RegPC += 1;
            m_Cycles += 2;
            break;
        default:
            {
                std::stringstream emsg;
                emsg << "TYA error address mode @ PC:" << std::hex << "0x" << m_RegPC;
                printError(emsg.str());
            }
            return;
            break;
    }

    m_RegA = m_RegY;

    setFlag(FLAG_SIGN, m_RegA & 0x80);
    setFlag(FLAG_ZERO, m_RegA == 0x0);
}
