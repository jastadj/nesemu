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


};

#endif