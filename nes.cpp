#include "nes.hpp"

NES::NES()
{
    // init memory
    m_Mem = new uint8_t [MEM_SIZE];
    std::cout << "Allocated " << MEM_SIZE << " bytes of memory.\n";
    clearMemory();

    // init CPU
    m_CPU = new C6502(m_Mem, MEM_SIZE);
}

// clear memory
void NES::clearMemory(uint16_t startaddress, uint16_t endaddress)
{
    for(int i = startaddress; i <= endaddress; i++)
    {
        m_Mem[i] = 0x0;
    }
}

/////////////////////////////////////////////
// DEBUG
void NES::debugC6502Console()
{
    m_CPU->debugConsole();
}
