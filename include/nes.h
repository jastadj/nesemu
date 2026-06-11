#ifndef NES_H
#define NES_H

#include "cpu6502.h"

class NES
{
public:
    NES();
    ~NES();

    CPU6502 m_CPU;
};

#endif