#include "nes.h"

NES::NES()
{
    // Init Memory Map Mirrors
    m_CPU.m_Mem.addMirror(0x0000, 0x0800, 0x0800);
    m_CPU.m_Mem.addMirror(0x0000, 0x1000, 0x0800);
    m_CPU.m_Mem.addMirror(0x0000, 0x1800, 0x0800);
}

NES::~NES()
{

}