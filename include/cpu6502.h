#ifndef CPU6502_H
#define CPU6502_H

#include <cstdlib>
#include <cstdint>

#include "memorymap.h"

class CPU6502
{
public:
    CPU6502();
    ~CPU6502();
    
    MemoryMap m_Mem;

    int execute();

    // Registers
    const uint8_t getAcc() const;
    const uint8_t getX() const;
    const uint8_t getY() const;
    const uint8_t getStatus() const;
    const uint8_t getStackPtr() const;
    const uint16_t getPC() const;
    
    // Program Counter
    void setPC(const uint16_t pc);
    void setPCL(const uint8_t pcl); // Set PC Low Byte
    void setPCH(const uint8_t pch); // Set PC High Byte

private:

    uint8_t m_CyclesToProcess;

    // Registers
    uint8_t m_ACC;
    uint8_t m_RX;
    uint8_t m_RY;
    uint16_t m_PC;
    uint8_t m_StackPtr;
    uint8_t m_Status;

    enum ADDRESS_MODE
    {
        IMMEDIATE,
        ZERO_PAGE,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,
        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        INDIRECT_X,
        INDIRECT_Y
    };
    uint16_t getOperand(ADDRESS_MODE address_mode);

    enum STATUS_BIT
    {
        S_CARRY,
        S_ZERO,
        S_INTERRUPT,
        S_DECIMAL,
        S_BREAK,
        S_UNUSED,
        S_OVERFLOW,
        S_NEGATIVE
    };
    void setStatusBit(STATUS_BIT bit, bool enabled);
};

#endif