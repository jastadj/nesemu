#include "defs6502.h"

using namespace Arch6502;

const char* Arch6502::getAddressModeString(ADDRESS_MODE mode)
{
    switch (mode)
    {
    case ADDRESS_MODE::ABSOLUTE: return "Absolute";
    case ADDRESS_MODE::ABSOLUTE_X: return "Absolute X";
    case ADDRESS_MODE::ABSOLUTE_Y: return "Absolute";
    case ADDRESS_MODE::IMMEDIATE: return "Immediate";
    case ADDRESS_MODE::IMPLIED: return "Implied";
    case ADDRESS_MODE::INDIRECT_X: return "Indirect X";
    case ADDRESS_MODE::INDIRECT_Y: return "Indirect Y";
    case ADDRESS_MODE::ZERO_PAGE: return "Zero Page";
    case ADDRESS_MODE::ZERO_PAGE_X: return "Zero Page X";
    case ADDRESS_MODE::ZERO_PAGE_Y: return "Zero Page Y";
    default:
        break;
    };
    return "unk";
}