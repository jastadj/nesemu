#ifndef CPU6502_H
#define CPU6502_H

#include <cstdlib>
#include <cstdint>

class CPU6502
{
public:
    CPU6502();
    ~CPU6502();
    
    uint8_t* m_Memory;
    std::size_t getMemorySize() const;

    // Registers
    uint8_t m_ACC;
    uint8_t m_RX;
    uint8_t m_RY;
    uint16_t m_PC;
    uint8_t m_Stack;
    uint8_t m_Status;

    bool execute();

private:

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

};

#endif