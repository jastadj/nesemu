#include "cpu6502clock.h"

CPUClock6502::CPUClock6502(unsigned int clock_speed_hz) :
    CPUClock(clock_speed_hz),
    onTickCallback(nullptr)
{

}

CPUClock6502::~CPUClock6502()
{

}

void CPUClock6502::onTick()
{
    if (onTickCallback)
    {
        onTickCallback();
    }
}