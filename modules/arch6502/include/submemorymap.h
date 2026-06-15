#ifndef SUBMEMORYMAP_H
#define SUBMEMORYMAP_H

#include "memorymap.h"

class SubMemoryMap : public MemoryMap
{
public:
    SubMemoryMap(std::size_t offset, std::size_t size);
    ~SubMemoryMap();

    std::size_t getOffset();

private:
    std::size_t m_Offset;
};

#endif