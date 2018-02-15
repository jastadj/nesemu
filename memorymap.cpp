#include "memorymap.hpp"

MemoryMap::MemoryMap(unsigned int memsize)
{
    m_MemSize = memsize;

    // init memory array
    m_Mem = new uint8_t[m_MemSize];

    // init memory map array
    m_MemMap = new uint8_t*[m_MemSize];

    // assign memory map indices to match memory indices
    for(unsigned int i = 0; i < m_MemSize; i++) m_MemMap[i] = &m_Mem[i];

    // clear memory
    clear();
}

MemoryMap::~MemoryMap()
{
    delete m_MemMap;
}

void MemoryMap::clear()
{
    for(unsigned int i = 0; i < m_MemSize; i++) *m_MemMap[i] = 0x0;
}

bool MemoryMap::clear(unsigned int startaddress, unsigned int endaddress)
{
    if( startaddress > endaddress)
    {
        std::cout << "MemoryMap clear error, start address > endaddress." << std::endl;
        return false;
    }
    if(startaddress >= m_MemSize || endaddress >= m_MemSize)
    {
        std::cout << "MemoryMap clear error, range outside of memory." << std::endl;
        return false;
    }

    for(unsigned int i = startaddress; i <= endaddress; i++) *m_MemMap[i] = 0x0;

    return true;
}

bool MemoryMap::write(unsigned int address, uint8_t val)
{
    if(address >= m_MemSize)
    {
        std::cout << "MemoryMap write error, address outside of memory range." << std::endl;
        return false;
    }

    *m_MemMap[address] = val;

    return true;
}

uint8_t MemoryMap::read(unsigned int address)
{
    if(address >= m_MemSize)
    {
        std::cout << "MemoryMap read error, address outside of memory range." << std::endl;
        return false;
    }

    return *m_MemMap[address];
}
