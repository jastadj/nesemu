#ifndef NES_H
#define NES_H

#include "cpu6502.h"
#include "cpu6502clock.h"

namespace NES
{
    class Console
    {
    public:
        Console();
        ~Console();

        void on();
        void off();
        void reset();
        bool isOn();

        Arch6502::CPU m_CPU;
        CPUClock6502 m_Clock;
        uint16_t m_ResetVector;

    private:
        unsigned int m_WaitCycles;
        void onTick();
    };
}
#endif