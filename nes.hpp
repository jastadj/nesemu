#ifndef CLASS_NES
#define CLASS_NES

#include <cstdlib>
#include <iostream>

#include "c6502.hpp"

#define MEM_SIZE 0xffff

class NES
{
private:

    // memory
    uint8_t *m_Mem;

    // 6502 CPU
    C6502 *m_CPU;

public:
    NES();
};
#endif // CLASS_NES
