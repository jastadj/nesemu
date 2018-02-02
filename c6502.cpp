#include "c6502.hpp"

C6502::C6502(uint8_t **memory, unsigned int memory_size)
{
    m_MemSize = memory_size;

    init();
}

bool C6502::init()
{
    // clear registers
    m_RegA = 0x0;
    m_RegX = 0x0;
    m_RegY = 0x0;

    m_RegPC = 0x0;
    m_RegSP = 0x0;

    m_RegStat = 0x0 | (0x1 << 4); // bit 5 (not used) is always high
}

void C6502::execute(uint8_t opcode)
{
    switch(opcode)
    {
        // ADC - Add memory to accumulator with carry
    case 0x69: // immediate
        m_RegA += m_Mem[m_RegPC + 1] + getcarryflag() ? 1 : 0
    break;

    }
}
