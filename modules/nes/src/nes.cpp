#include "nes.h"

NES::NES():
    m_Clock(NES_CLOCK_HZ)
{
    // Init System Ram Mirrors
    m_CPU.addMirror(0x0000, 0x0800, 0x0800);
    m_CPU.addMirror(0x0000, 0x1000, 0x0800);
    m_CPU.addMirror(0x0000, 0x1800, 0x0800);

    // Set Stack Pointer
    
}

NES::~NES()
{
    // Stop the CPU clock
    m_Clock.stop();
}