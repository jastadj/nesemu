#include "nes.hpp"

NES::NES()
{
    // init memory
    m_Mem = new uint8_t [MEM_SIZE];
    for(int i = 0; i < MEM_SIZE; i++)
    {
        m_Mem[i] = 0x0;
    }

    // init CPU
    m_CPU = new C6502(&m_Mem, MEM_SIZE);
}
