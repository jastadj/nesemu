#include "nes.h"

#include "nesdefs.h"

// debug
#include <iostream>

NES::Console::Console():
    m_Clock(NES_CLOCK_HZ)
{
    // Init System Ram Mirrors
    m_CPU.addMirror(0x0000, 0x0800, 0x0800);
    m_CPU.addMirror(0x0000, 0x1000, 0x0800);
    m_CPU.addMirror(0x0000, 0x1800, 0x0800);

    // Other Memory
    // 0x6000 - 0x7fff = NV RAM / Battery Back RAM
    // 0x7000 - 0x71ff = Trainer
    // 0x8000 - 0xffff = Cartridge Data
    // 0xfffc = Reset Vector

    // Set Clock Tick Callback
    m_Clock.onTickCallback = [&]() {onTick(); };    
}

NES::Console::~Console()
{
    off();
}

void NES::Console::on()
{
    reset();
}

void NES::Console::off()
{
    m_Clock.stop();
}

void NES::Console::reset()
{
    m_Clock.stop();
    m_WaitCycles = 0;
    m_CPU.setPC(m_ResetVector);
    m_CPU.resetStack();
    m_Clock.start();
    
}

bool NES::Console::isOn()
{
    return m_Clock.isRunning();
}

void NES::Console::onTick()
{
    if (m_WaitCycles)
    {
        m_WaitCycles--;
    }
    else
    {
        m_WaitCycles = m_CPU.execute() - 1;
    }
    
}
