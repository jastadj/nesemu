#include "cpu6502.h"

#include "opcodes6502.h"

// debug
#include <iostream>
#include <sstream>

CPU6502::CPU6502():
    m_Mem(0xffff)
{
    m_CyclesToProcess = 0;

    // Init Registers
    m_ACC = 0;
    m_RX = 0;
    m_RY = 0;
    m_PC = 0;
    m_StackPtr = 0;
    m_Status = 0;
}

CPU6502::~CPU6502()
{
}


///////////
// OP-CODES

int CPU6502::execute()
{
    m_CyclesToProcess = 0;

    switch (m_Mem.get(m_PC++))
    {
/*
*     // ADC - Add memory to Accumulator with Carry
    // A + M + C -> A, C
    ADC_IMMEDIATE =         0x69,
    ADC_ZERO_PAGE =         0x65,
    ADC_ZERO_PAGE_X =       0x75,
    ADC_ABSOLUTE =          0x60,
    ADC_ABSOLUTE_X =        0x70,
    ADC_ABSOLUTE_Y =        0x79,
    ADC_INDIRECT_X =        0x61,
    ADC_INDIRECT_Y =        0x71,
*/
    // ADC
    case OPCODES_6502::ADC_IMMEDIATE:
        m_CyclesToProcess += 2;
        break;
    // LDX
    case OPCODES_6502::LDX_IMMEDIATE:
        m_RX = getOperand(ADDRESS_MODE::IMMEDIATE);
        m_CyclesToProcess += 2;
        break;
    // STX
    case OPCODES_6502::STX_ZERO_PAGE:
        m_Mem.set(getOperand(ADDRESS_MODE::ZERO_PAGE), m_RX);
        m_CyclesToProcess += 3;
        break;
    // LDY
    case OPCODES_6502::LDY_IMMEDIATE:
        m_RY = getOperand(ADDRESS_MODE::IMMEDIATE);
        m_CyclesToProcess += 2;
        break;
    // STY
    case OPCODES_6502::STY_ZERO_PAGE:
        m_Mem.set(getOperand(ADDRESS_MODE::ZERO_PAGE), m_RY);
        m_CyclesToProcess += 3;
        break;
    case OPCODES_6502::LDA_IMMEDIATE:
        m_ACC = getOperand(ADDRESS_MODE::IMMEDIATE);
        m_CyclesToProcess += 2;
        break;
    case OPCODES_6502::LDA_ZERO_PAGE:
        m_ACC = getOperand(ADDRESS_MODE::ZERO_PAGE);
        m_CyclesToProcess += 3;
        break;
    case OPCODES_6502::LDA_ZERO_PAGE_X:
        m_ACC = getOperand(ADDRESS_MODE::ZERO_PAGE_X);
        m_CyclesToProcess += 4;
        break;
    case OPCODES_6502::LDA_ABSOLUTE:
        m_ACC = getOperand(ADDRESS_MODE::ABSOLUTE);
        m_CyclesToProcess += 4;
        break;
    case OPCODES_6502::LDA_ABSOLUTE_X:
        m_ACC = getOperand(ADDRESS_MODE::ABSOLUTE_X);
        m_CyclesToProcess += 4;
        break;
    case OPCODES_6502::LDA_ABSOLUTE_Y:
        m_ACC = getOperand(ADDRESS_MODE::ABSOLUTE_Y);
        m_CyclesToProcess += 4;
        break;
    case OPCODES_6502::LDA_INDIRECT_X:
        m_ACC = getOperand(ADDRESS_MODE::INDIRECT_X);
        m_CyclesToProcess += 6;
        break;
    case OPCODES_6502::LDA_INDIRECT_Y:
        m_ACC = getOperand(ADDRESS_MODE::INDIRECT_Y);
        m_CyclesToProcess += 5;
        break;
    // STA
    case OPCODES_6502::STA_ZERO_PAGE:
        m_Mem.set(getOperand(ADDRESS_MODE::ZERO_PAGE), m_ACC);
        m_CyclesToProcess += 3;
        break;
    default:
        m_CyclesToProcess += 2;
        break;
    }
    return m_CyclesToProcess;
}


////////////
// REGISTERS

const uint8_t CPU6502::getAcc() const
{
    return m_ACC;
}

const uint8_t CPU6502::getX() const
{
    return m_RX;
}

const uint8_t CPU6502::getY() const
{
    return m_RY;
}

const uint8_t CPU6502::getStatus() const
{
    return m_Status;
}

const uint8_t CPU6502::getStackPtr() const
{
    return m_StackPtr;
}

const uint16_t CPU6502::getPC() const
{
    return m_PC;
}

void CPU6502::setPC(const uint16_t pc)
{
    m_PC = pc;
}

void CPU6502::setPCL(const uint8_t pcl)
{
    m_PC |= pcl;
}

void CPU6502::setPCH(const uint8_t pch)
{
    m_PC |= (uint16_t(pch) << 8);
}

uint16_t CPU6502::getOperand(ADDRESS_MODE address_mode)
{
    switch (address_mode)
    {
    case ADDRESS_MODE::IMMEDIATE:
        return m_Mem.get(m_PC++);
    case ADDRESS_MODE::ZERO_PAGE:
        return m_Mem.get(m_Mem.get(m_PC++));
    case ADDRESS_MODE::ZERO_PAGE_X:
        return m_Mem.get(m_PC++) + m_RX;
    case ADDRESS_MODE::ABSOLUTE:
    {
        uint16_t low = m_Mem.get(m_PC++);
        uint16_t high = m_Mem.get(m_PC++);
        return m_Mem.get(low | (high << 8));
    }
    case ADDRESS_MODE::ABSOLUTE_X:
    {
        uint16_t low = m_Mem.get(m_PC++);
        uint16_t high = m_Mem.get(m_PC++);
        m_CyclesToProcess += (((low + m_RX) & 0xff00) ? 1 : 0);
        return m_Mem.get( (low | (high << 8)) + m_RX);
    }
    case ADDRESS_MODE::ABSOLUTE_Y:
    {
        uint16_t low = m_Mem.get(m_PC++);
        uint16_t high = m_Mem.get(m_PC++);
        m_CyclesToProcess += (((low + m_RY) & 0xff00) ? 1 : 0);
        return m_Mem.get((low | (high << 8)) + m_RY);
    }
    case ADDRESS_MODE::INDIRECT_X:
    {
        uint8_t zaddr = m_Mem.get(m_PC++) + m_RX;
        uint16_t low = m_Mem.get(zaddr++);
        uint16_t high = m_Mem.get(zaddr);
        return m_Mem.get(low | (high << 8));
    }
    case ADDRESS_MODE::INDIRECT_Y:
    {
        uint8_t zaddr = m_Mem.get(m_PC++);
        uint16_t low = m_Mem.get(zaddr++);
        uint16_t high = m_Mem.get(zaddr);
        m_CyclesToProcess += (((low + m_RY) && 0xff00) ? 1 : 0);
        return m_Mem.get( (low | (high << 8)) + m_RY);
    }
    default:
        std::cout << "Error getting address, unhandled mode: " << address_mode << std::endl;
        break;
    }
    return 0;
}

void CPU6502::setStatusBit(STATUS_BIT bit, bool enabled)
{
    if (enabled)
    {
        m_Status |= (0x1 << bit);
    }
    else
    {
        m_Status = m_Status & ~(0x1 << bit);
    }
}
