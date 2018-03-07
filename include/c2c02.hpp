#ifndef CLASS_C2C02
#define CLASS_C2C02

#include <cstdlib>
#include <iostream>
#include <vector>

// define PPU registers
#define PPUCTRL 0x2000
#define PPUMASK 0x2001
#define PPUSTATUS 0x2002
#define OAMADDR 0x2003
#define OAMDATA 0x2004
#define PPUSCROLL 0x2005
#define PPUADDR 0x2006
#define PPUDATA 0x2007
#define OAMDMA 0x4014

class C2C02
{
private:

    // memory
    uint8_t **m_Mem;
    unsigned int m_MemSize;

    // PPU registers
    uint8_t *m_PPUCTRL;
    uint8_t *m_PPUMASK;
    uint8_t *m_PPUSTATUS;
    uint8_t *m_OAMADDR;
    uint8_t *m_OAMDATA;
    uint8_t *m_PPUSCROLL;
    uint8_t *m_PPUADDR;
    uint8_t *m_PPUDATA;
    uint8_t *m_OAMDMA;


    bool init();

public:
    C2C02(uint8_t **memory, unsigned int memory_size);
    ~C2C02();

    // map PPU registers to CPU memory
    void mapRegisters(uint8_t **cpumem);
    void reset();

    // debug
    void debugConsole(std::string prompt);
    void show();

};
#endif // CLASS_C2C02
