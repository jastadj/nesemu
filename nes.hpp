#ifndef CLASS_NES
#define CLASS_NES

#include "c6502.hpp"

class NES
{
private:

    // 6502 CPU
    C6502 m_CPU;

public:
    NES();
};
#endif // CLASS_NES
