#include "nesdefs.h"

using namespace NES;

const char* NES::getTimingModeString(NES::TIMING_MODE type)
{
    switch (type)
    {
    case NES::TIMING_MODE::NTSC: return "NTSC";
    case NES::TIMING_MODE::PAL: return "PAL";
    case NES::TIMING_MODE::MULTI: return "Multi-Region";
    case NES::TIMING_MODE::DENDY: return "Dendy";
    default:
        break;
    }
    return "unk";
}

const char* NES::getConsoleTypeString(NES::CONSOLE_TYPE type)
{
    switch (type)
    {
    case NES::CONSOLE_TYPE::STANDARD: return "Nintendo Entertainment System";
    case NES::CONSOLE_TYPE::VS_SYSTEM: return "Vs. System";
    case NES::CONSOLE_TYPE::PLAYCHOICE10: return "PlayChoice 10";
    default:
        break;
    }
    return "unk";
}