#include "c6502.hpp"

C6502::C6502()
{
    init();
}

bool C6502::init()
{
    // clear registers
    m_RegA = 0x0;
    m_RegX = 0x0;
    m_RegY = 0x0;

    m_RegPC = 0x0;
    m_RegSP = 0x0;

    m_RegStat = 0x0 | (0x1 << 4); // bit 5 (not used) is always high

}
