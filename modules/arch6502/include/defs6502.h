#ifndef DEFS6502_H
#define DEFS6502_H

namespace Arch6502
{
    enum ADDRESS_MODE
    {
        IMPLIED,
        IMMEDIATE,
        ZERO_PAGE,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,
        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        INDIRECT_X,
        INDIRECT_Y,
        ADDR_ERROR
    };
    const char* getAddressModeString(ADDRESS_MODE mode);

    enum STATUS_BIT
    {
        S_CARRY,
        S_ZERO,
        S_INTERRUPT,
        S_DECIMAL,
        S_BREAK,
        S_UNUSED,
        S_OVERFLOW,
        S_NEGATIVE
    };
}



#endif