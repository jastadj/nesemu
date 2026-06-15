#include "submemorymap.h"

SubMemoryMap::SubMemoryMap(std::size_t offset, std::size_t size) :
    MemoryMap(size),
    m_Offset(offset)
{

}

SubMemoryMap::~SubMemoryMap()
{

}

std::size_t SubMemoryMap::getOffset()
{
    return m_Offset;
}