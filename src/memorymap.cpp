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

bool MemoryMap::mirror(unsigned int start1, unsigned int end1, unsigned int start2, unsigned int end2)
{
    if( (end1 - start1) != (end2 - start2))
    {
        std::cout << "Error in MemoryMap mirror: unable to mirror differing range sizes." << std::endl;
        return false;
    }

    if( (start1 >= end1 || start2 >= end2) || (start1 >= m_MemSize || end1 >= m_MemSize || start2 >= m_MemSize || end2 >= m_MemSize) )
    {
        std::cout << "Error in MemoryMap mirror: ranges invalid." << std::endl;
        return false;
    }

    for(int i = 0; i <= int(end1 - start1); i++)
    {
        m_MemMap[start1 + i] = &m_Mem[start1 + i];
        m_MemMap[start2 + i] = &m_Mem[start1 + i];
    }

    return true;

}

bool MemoryMap::clearMirror(unsigned int startaddress, unsigned int endaddress)
{
    if( startaddress > endaddress)
    {
        std::cout << "MemoryMap clearMirror error, start address > endaddress." << std::endl;
        return false;
    }
    if(startaddress >= m_MemSize || endaddress >= m_MemSize)
    {
        std::cout << "MemoryMap clearMirror error, range outside of memory." << std::endl;
        return false;
    }

    for(unsigned int i = startaddress; i <= endaddress; i++) m_MemMap[i] = &m_Mem[i];

    return true;
}
