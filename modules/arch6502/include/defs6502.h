#ifndef DEFS6502_H
#define DEFS6502_H

namespace Arch6502
{
    enum ADDRESS_MODE
    {
        IMPLIED,
        RELATIVE,
        ACCUMULATOR,
        IMMEDIATE,
        ZERO_PAGE,
        ZERO_PAGE_X,
        ZERO_PAGE_Y,
        ABSOLUTE,
        ABSOLUTE_X,
        ABSOLUTE_Y,
        INDIRECT,
        INDIRECT_X,
        INDIRECT_Y,
        ADDR_ERROR
    };
    const char* getAddressModeString(ADDRESS_MODE mode);

    enum STATUS_BIT
    {
        STATUS_CARRY,
        STATUS_ZERO,
        STATUS_INTERRUPT,
        STATUS_DECIMAL,
        STATUS_BREAK,
        STATUS_UNUSED,
        STATUS_OVERFLOW,
        STATUS_NEGATIVE
    };
}



#endif