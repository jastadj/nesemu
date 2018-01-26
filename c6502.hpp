#ifndef CLASS_C6502
#define CLASS_C6502

#include <cstdlib>
#include <iostream>

class C6502
{
private:

    // memory
    uint8_t m_Mem[0xffff];
    // memory map
    // 0x0000 - 0x00ff : zero page
    // 0x0100 - 0x01ff : stack
    // 0x0200 - 0x07ff : RAM
    // 0x0800 - 0x1fff : Mirrors 0x0000 - 0x07ff
    // 0x2000 - 0x2007 : I/O registers
    // 0x2008 - 0x3fff : Mirrors 0x2000 - 0x2007
    // 0x4000 - 0x401f : I/O registers
    // 0x4020 - 0x5fff : Expansion ROM
    // 0x6000 - 0x7fff : SRAM
    // 0x8000 - 0xbfff : PRG-ROM Lower
    // 0xc000 - 0xffff : PRG-ROM Upper

    // registers
    uint8_t m_RegA; // accumulator
    uint8_t m_RegX;
    uint8_t m_RegY;

    uint8_t m_RegSP; // stack pointer (first empty place on the stack)
    uint16_t m_RegPC; // program counter, position of current instruction

    // status register
    unsigned char m_RegStat;
    // b7 = S - Sign flag, 1 = negative
    // b6 = V - overflow flag
    // b5 = not used, should always be logical 1
    // b4 = B - software interrupt is executed (BRK)
    // b3 = D - decimal mode (when add/sub with carry, source values treated as BCD
    // b2 = I - interrupt enable/disable, if set, interrupts are disabled
    // b1 = Z - Zero flag, set when arithmetic or logical op produces 0
    // b0 = C - carry flag

    //
    bool init();

public:
    C6502();
};
#endif // CLASS_C6502
