#ifndef NES_H
#define NES_H

#include "cpu6502.h"
#include "cpu6502clock.h"

// NTSC 1.789773 MHz (~559 ns per cycle)
// PAL  1.662607 MHz (~601 ns per cycle)
#define NES_CLOCK_HZ 1789773

class NES
{
public:
    NES();
    ~NES();

    CPU6502 m_CPU;
    CPUClock6502 m_Clock;
};

#endif