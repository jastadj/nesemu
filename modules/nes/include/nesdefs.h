#ifndef NESDEFS_H
#define NESDEFS_H

// NTSC 1.789773 MHz (~559 ns per cycle)
// PAL  1.662607 MHz (~601 ns per cycle)
#define NES_CLOCK_HZ 1789773

namespace NES
{
    enum TIMING_MODE
    {
        NTSC,
        PAL,
        MULTI,
        DENDY
    };
    const char* getTimingModeString(TIMING_MODE type);

    enum CONSOLE_TYPE {
        STANDARD,
        VS_SYSTEM,
        PLAYCHOICE10,
        EXTENDED
    };
    const char* getConsoleTypeString(CONSOLE_TYPE type);
}

#endif