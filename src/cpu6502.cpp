#include "cpu6502.h"

#include "opcodes6502.h"

// debug
#include <iostream>
#include <sstream>

CPU6502::CPU6502()
{
    // Allocate Memory
    m_Memory = new unsigned char[getMemorySize()]();
    std::cout << "Allocated " << getMemorySize() << " bytes of memory." << std::endl;

    // Init Registers
    m_ACC = 0;
    m_RX = 0;
    m_RY = 0;
    m_PC = 0;
    m_Stack = 0;
    m_Status = 0;
}

CPU6502::~CPU6502()
{
    delete[] m_Memory;
}

std::size_t CPU6502::getMemorySize() const
{
    return 0xffff + 1;
}

bool CPU6502::execute()
{
    uint8_t cycles = 0;

    switch (m_Memory[m_PC])
    {
    case OPCODES_6502::LDA_IMMEDIATE:
        m_ACC = m_Memory[m_PC + 1];
        m_PC += 2;
        cycles = 2;
        break;
    case OPCODES_6502::STA_ZERO_PAGE:
    {
        uint8_t zaddr = m_Memory[m_PC + 1];
        m_Memory[zaddr] = m_ACC;
    }
        m_PC += 2;
        cycles = 3;
        break;
    default:
        return false;
        break;
    }
    return true;
}