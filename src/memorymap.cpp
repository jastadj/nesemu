#include "memorymap.h"

// Debug
#include <iostream>

MemoryMap::MemoryMap(std::size_t init_size)
{
    for (auto i = 0; i < init_size; i++)
    {
        m_Bank.emplace_back(std::make_shared<uint8_t>());
    }
}

MemoryMap::~MemoryMap()
{
    m_Bank.clear();
}

const std::size_t MemoryMap::size() const
{
    return m_Bank.size();
}

const uint8_t MemoryMap::get(std::size_t addr) const
{
    return *m_Bank[addr];
}

void MemoryMap::set(std::size_t addr, const uint8_t val)
{
    *m_Bank[addr] = val;
}

void MemoryMap::fill(const uint8_t val)
{
    for (auto i = 0; i < m_Bank.size(); i++)
    {
        set(i, val);
    }
}

void MemoryMap::fillRandom()
{
    for (auto i = 0; i < m_Bank.size(); i++)
    {
        set(i, rand() % 256);
    }
}

bool MemoryMap::addMirror(std::size_t source_addr, std::size_t dest_addr, std::size_t len)
{
    if ((source_addr + len > m_Bank.size()) || (dest_addr + len > m_Bank.size()))
    {
        return false;
    }

    // Delete destination map and replace with source pointer
    for (auto i = 0; i < len; i++)
    {
        m_Bank[dest_addr + i] = m_Bank[source_addr + i];
    }
    return true;
}
