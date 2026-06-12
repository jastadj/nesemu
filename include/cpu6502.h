#ifndef CPU6502_H
#define CPU6502_H

#include <cstdlib>
#include <cstdint>

#include "defs6502.h"

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


    uint16_t getOperand(ADDRESS_MODE address_mode);


    void setStatusBit(STATUS_BIT bit, bool enabled);
};

#endif