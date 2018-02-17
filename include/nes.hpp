#ifndef CLASS_NES
#define CLASS_NES

#include <cstdlib>
#include <iostream>

#include "memorymap.hpp"
#include "rp2a03.hpp"
#include "c2c02.hpp"
#include "cartridge.hpp"


#define MEM_SIZE 65536
#define PPUMEM_SIZE 16384

class NES
{
private:

    bool init();

    // memory
    MemoryMap *m_MemCPU;
    MemoryMap *m_MemPPU;

    // rom cartridge
    Cartridge *m_Cartridge;

    // 6502 CPU / APU
    RP2A03 *m_CPU;

    // PPU
    C2C02 *m_PPU;


public:
    NES();
    ~NES();

    bool loadCartridge(std::string romfile);
    void reset();

    void debugConsole(std::string prompt);
};
#endif // CLASS_NES
