#include "cpu6502.h"

#include "opcodes6502.h"

// debug
#include <iostream>
#include <sstream>

using namespace Arch6502;

CPU::CPU():
    m_MemoryMaps(nullptr)
{
    // Init OpCodes (if not already initialized)
    OpCodes::LUT::init();

    m_CyclesToProcess = 0;

    // Init Registers
    m_ACC = 0;
    m_RX = 0;
    m_RY = 0;
    m_PC = 0;

    // Stack Pointer
    // Offset from address 0x0100;
    m_StackPtr = 0xff; 
    
    // Status Register
    m_Status = 0;

}

CPU::~CPU()
{
}


///////////
// OP-CODES

int CPU::execute()
{
    if (m_MemoryMaps)
    {
        m_CyclesToProcess = 0;

        uint8_t opcode = m_MemoryMaps->get(m_PC++);

        OpCodes::OpFunc* opfunc = OpCodes::LUT::codes[opcode];
        if (opfunc != nullptr)
        {
            opfunc->execute(*this, OpCodes::LUT::modes[opcode]);
        }
        if (m_CyclesToProcess == 0)
        {
            m_CyclesToProcess = 1;
        }

        return m_CyclesToProcess;
    }
    return 1;
}


////////////
// REGISTERS

const uint8_t CPU::getAcc() const
{
    return m_ACC;
}

void CPU::setAcc(uint8_t val)
{
    m_ACC = val;
}

const uint8_t CPU::getX() const
{
    return m_RX;
}

void CPU::setX(uint8_t val)
{
    m_RX = val;
}

const uint8_t CPU::getY() const
{
    return m_RY;
}

void CPU::setY(uint8_t val)
{
    m_RY = val;
}

const uint8_t CPU::getStackPtr() const
{
    return m_StackPtr;
}

void CPU::pushStack(uint16_t val)
{
    pushStack(uint8_t(val >> 8));
    pushStack(uint8_t(val & 0xff));
}

void CPU::pushStack(uint8_t val)
{
    if (m_MemoryMaps)
    {
        m_MemoryMaps->set(m_StackPtr--, val);
    }
}

uint8_t CPU::popStack()
{
    if (m_MemoryMaps)
    {
        return m_MemoryMaps->get(++m_StackPtr);
    }
    return 0;
}

void Arch6502::CPU::resetStack()
{
    m_StackPtr = 0xff;
}

const uint16_t CPU::getPC() const
{
    return m_PC;
}

void CPU::setPC(const uint16_t pc)
{
    m_PC = pc;
}

void CPU::setPCL(const uint8_t pcl)
{
    m_PC |= pcl;
}

void CPU::setPCH(const uint8_t pch)
{
    m_PC |= (uint16_t(pch) << 8);
}

void CPU::setMemoryMap(MemoryMap* memory)
{
    m_MemoryMaps = memory;
}

bool CPU::hasMemoryMap() const
{
    return m_MemoryMaps != nullptr;
}

const uint8_t CPU::getStatus() const
{
    return m_Status;
}

void CPU::setStatusBit(STATUS_BIT bit, bool enabled)
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

const uint8_t CPU::getStatusBit(STATUS_BIT bit) const
{
    if (bit >= 0 && bit < 8)
    {
        if ((0x1 << bit) & m_Status)
        {
            return 0x01;
        }
    }
    return 0x00;
}

const char* CPU::getStatusBitString(STATUS_BIT status_bit)
{
    switch (status_bit)
    {
    case STATUS_BIT::STATUS_CARRY: return "Carry";
    case STATUS_BIT::STATUS_BREAK: return "Break";
    case STATUS_BIT::STATUS_DECIMAL: return "Decimal";
    case STATUS_BIT::STATUS_INTERRUPT: return "Interrupt";
    case STATUS_BIT::STATUS_NEGATIVE: return "Negative";
    case STATUS_BIT::STATUS_OVERFLOW: return "Overflow";
    case STATUS_BIT::STATUS_UNUSED: return "Unused";
    case STATUS_BIT::STATUS_ZERO: return "Zero";
    defualt:
        break;
    }
    return "unk";
}

uint16_t CPU::getOperand(ADDRESS_MODE address_mode)
{
    if (m_MemoryMaps)
    {
        m_CyclesToProcess++;

        switch (address_mode)
        {
        case ADDRESS_MODE::IMMEDIATE:
            return m_MemoryMaps->get(m_PC++);
        case ADDRESS_MODE::ZERO_PAGE:
            return m_MemoryMaps->get(m_MemoryMaps->get(m_PC++));
        case ADDRESS_MODE::ZERO_PAGE_X:
            return m_MemoryMaps->get(m_PC++) + m_RX;
        case ADDRESS_MODE::ABSOLUTE:
        {
            uint16_t low = m_MemoryMaps->get(m_PC++);
            uint16_t high = m_MemoryMaps->get(m_PC++);
            return m_MemoryMaps->get(low | (high << 8));
        }
        case ADDRESS_MODE::ABSOLUTE_X:
        {
            uint16_t low = m_MemoryMaps->get(m_PC++);
            uint16_t high = m_MemoryMaps->get(m_PC++);
            m_CyclesToProcess += (((low + m_RX) & 0xff00) ? 1 : 0);
            return m_MemoryMaps->get((low | (high << 8)) + m_RX);
        }
        case ADDRESS_MODE::ABSOLUTE_Y:
        {
            uint16_t low = m_MemoryMaps->get(m_PC++);
            uint16_t high = m_MemoryMaps->get(m_PC++);
            m_CyclesToProcess += (((low + m_RY) & 0xff00) ? 1 : 0);
            return m_MemoryMaps->get((low | (high << 8)) + m_RY);
        }
        case ADDRESS_MODE::INDIRECT_X:
        {
            uint8_t zaddr = m_MemoryMaps->get(m_PC++) + m_RX;
            uint16_t low = m_MemoryMaps->get(zaddr++);
            uint16_t high = m_MemoryMaps->get(zaddr);
            return m_MemoryMaps->get(low | (high << 8));
        }
        case ADDRESS_MODE::INDIRECT_Y:
        {
            uint8_t zaddr = m_MemoryMaps->get(m_PC++);
            uint16_t low = m_MemoryMaps->get(zaddr++);
            uint16_t high = m_MemoryMaps->get(zaddr);
            m_CyclesToProcess += (((low + m_RY) && 0xff00) ? 1 : 0);
            return m_MemoryMaps->get((low | (high << 8)) + m_RY);
        }
        default:
            std::cout << "Error getting address, unhandled mode: " << address_mode << std::endl;
            break;
        }
    }

    return 0;
}


