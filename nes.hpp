#ifndef CLASS_NES
#define CLASS_NES

#include <cstdlib>
#include <iostream>

#include "c6502.hpp"
#include "cartridge.hpp"

#define MEM_SIZE 65536

class NES
{
private:

    // memory
    uint8_t *m_Mem;
    void clearMemory(uint16_t startaddress = 0x00, uint16_t endaddress = MEM_SIZE-1);

    // rom cartridge
    Cartridge *m_Cartridge;

    // 6502 CPU
    C6502 *m_CPU;

public:
    NES();

    bool loadCartridge(std::string romfile);

    void debugC6502Console();
};
#endif // CLASS_NES
