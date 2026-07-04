#ifndef PPU_H
#define PPU_H

#include <cstdint>

namespace NES
{
    class Console;

    class PPU
    {
    public:
        PPU(Console* nes);
        ~PPU();

        ////////////
        // REGISTERS

        // PPUCTRL 0x2000
        // Configures rendering settings, NMI, and scrolling
        uint8_t getPPUCTRL();
        bool setPPUCTRL(uint8_t val);

        // PPUMASK 0x2001
        // Manages render settings for sprites and backgrounds
        uint8_t getPPUMASK();
        bool setPPUMASK(uint8_t val);

        // PPUSTATUS 0x2002
        // Reports rendering events including vblank flag
        uint8_t getPPUSTATUS();
        bool setPPUSTATUS(uint8_t val);

        // OAMADDR 0x2003
        // 
        uint8_t getOAMADDR();
        bool setOAMADDR(uint8_t val);

        // OAMADDR 0x2004
        // 
        uint8_t getOAMDATA();
        bool setOAMDATA(uint8_t val);

        // PPUSCROLL 0x2005
        // Sets the X and Y scroll positions for rendering
        // 2 Writes, X scroll pos then Y scroll pos
        uint8_t getPPUSCROLL();
        bool setPPUSCROLL(uint8_t val);

        // PPUADDR 0x2006
        // Specifies address in PPU mem for data operations
        // 2 Writes, MSB then LSB
        uint8_t getPPUADDR();
        bool setPPUADDR(uint8_t val);

        // PPUDATA 0x2007
        // PPU memory I/O
        uint8_t getPPUDATA();
        bool setPPUDATA(uint8_t val);

    private:
        Console* m_NES;
    };
}

#endif