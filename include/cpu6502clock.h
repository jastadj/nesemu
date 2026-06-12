#include "cpuclock.h"

class CPUClock6502: public CPUClock
{
public:
    CPUClock6502(unsigned int clock_speed_hz);
    ~CPUClock6502();

    void onTick();

};