#include "ppu.h"

#include "nes.h"
#include "memorymap.h"

using namespace NES;

PPU::PPU(NES::Console* nes):
    m_NES(nes)
{

}

PPU::~PPU()
{

}

uint8_t PPU::getPPUCTRL()
{
    return m_NES->m_MemoryMaps->get(0x2000);
}

bool PPU::setPPUCTRL(uint8_t val)
{
    return m_NES->m_MemoryMaps->set(0x2000, val);
}

uint8_t PPU::getPPUMASK()
{
    return m_NES->m_MemoryMaps->get(0x2001);
}

bool PPU::setPPUMASK(uint8_t val)
{
    return m_NES->m_MemoryMaps->set(0x2001, val);
}

uint8_t PPU::getPPUSTATUS()
{
    return m_NES->m_MemoryMaps->get(0x2002);
}

bool PPU::setPPUSTATUS(uint8_t val)
{
    return m_NES->m_MemoryMaps->set(0x2002, val);
}

uint8_t PPU::getOAMADDR()
{
    return m_NES->m_MemoryMaps->get(0x2003);
}

bool PPU::setOAMADDR(uint8_t val)
{
    return m_NES->m_MemoryMaps->set(0x2003, val);
}

uint8_t PPU::getOAMDATA()
{
    return m_NES->m_MemoryMaps->get(0x2004);
}

bool PPU::setOAMDATA(uint8_t val)
{
    return m_NES->m_MemoryMaps->set(0x2004, val);
}

uint8_t PPU::getPPUSCROLL()
{
    return m_NES->m_MemoryMaps->get(0x2005);
}

bool PPU::setPPUSCROLL(uint8_t val)
{
    return m_NES->m_MemoryMaps->set(0x2005, val);
}

uint8_t PPU::getPPUADDR()
{
    return m_NES->m_MemoryMaps->get(0x2006);
}

bool PPU::setPPUADDR(uint8_t val)
{
    return m_NES->m_MemoryMaps->set(0x2006, val);
}

uint8_t PPU::getPPUDATA()
{
    return m_NES->m_MemoryMaps->get(0x2007);
}

bool PPU::setPPUDATA(uint8_t val)
{
    return m_NES->m_MemoryMaps->set(0x2007, val);
}