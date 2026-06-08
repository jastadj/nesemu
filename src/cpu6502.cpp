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

    switch (m_Memory[m_PC++])
    {
        // REGISTER X
    case OPCODES_6502::LDX_IMMEDIATE:
        m_RX = getOperand(ADDRESS_MODE::IMMEDIATE);
        cycles = 2;
        break;
    case OPCODES_6502::STX_ZERO_PAGE:
        m_Memory[getOperand(ADDRESS_MODE::ZERO_PAGE)] = m_RX;
        cycles = 3;
        break;
        // REGISTER Y
    case OPCODES_6502::LDY_IMMEDIATE:
        m_RY = getOperand(ADDRESS_MODE::IMMEDIATE);
        cycles = 2;
        break;
    case OPCODES_6502::STY_ZERO_PAGE:
        m_Memory[getOperand(ADDRESS_MODE::ZERO_PAGE)] = m_RY;
        cycles = 3;
        break;
        // ACCUMULATOR
    case OPCODES_6502::LDA_IMMEDIATE:
        m_ACC = getOperand(ADDRESS_MODE::IMMEDIATE);
        cycles = 2;
        break;
    case OPCODES_6502::LDA_ZERO_PAGE:
        m_ACC = getOperand(ADDRESS_MODE::ZERO_PAGE);
        cycles = 3;
        break;
    case OPCODES_6502::LDA_ZERO_PAGE_X:
        m_ACC = getOperand(ADDRESS_MODE::ZERO_PAGE_X);
        cycles = 4;
        break;
    case OPCODES_6502::STA_ZERO_PAGE:
        m_Memory[getOperand(ADDRESS_MODE::ZERO_PAGE)] = m_ACC;
        cycles = 3;
        break;
    default:
        return false;
        break;
    }
    return true;
}

uint16_t CPU6502::getOperand(ADDRESS_MODE address_mode)
{
    switch (address_mode)
    {
    case ADDRESS_MODE::IMMEDIATE:
        return m_Memory[m_PC++];
        break;
    case ADDRESS_MODE::ZERO_PAGE:
        return m_Memory[m_PC++];
        break;
    case ADDRESS_MODE::ZERO_PAGE_X:
        return m_Memory[m_PC++] + m_RX;
    default:
        std::cout << "Error getting address, unhandled mode: " << address_mode << std::endl;
        break;
    }
    return 0;
}
