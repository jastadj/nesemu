#include "memorymap.h"

// Debug
#include <iostream>

MemoryMap::MemoryMap(std::size_t init_size)
{
    for (auto i = 0; i < init_size; i++)
    {
        m_Mem.emplace_back(std::make_shared<uint8_t>());
    }
}

MemoryMap::~MemoryMap()
{
    m_Mem.clear();
}

const std::size_t MemoryMap::size() const
{
    return m_Mem.size();
}

const uint8_t MemoryMap::get(std::size_t addr, bool* ok) const
{
    if (addr < m_Mem.size())
    {
        if (ok != nullptr)
        {
            *ok = true;
        }
        return *m_Mem[addr];
    }
    if (ok != nullptr)
    {
        *ok = false;
    }
    return 0;
}

bool MemoryMap::set(std::size_t addr, const uint8_t val)
{
    if (addr < m_Mem.size())
    {
        *m_Mem[addr] = val;
        return true;
    }
    return false;
}

void MemoryMap::fill(const uint8_t val)
{
    for (auto i = 0; i < m_Mem.size(); i++)
    {
        set(i, val);
    }
}

void MemoryMap::fillRandom()
{
    for (auto i = 0; i < m_Mem.size(); i++)
    {
        set(i, rand() % 256);
    }
}

bool MemoryMap::addMirror(std::size_t source_addr, std::size_t dest_addr, std::size_t len)
{
    if ((source_addr + len > m_Mem.size()) || (dest_addr + len > m_Mem.size()))
    {
        return false;
    }

    // Delete destination map and replace with source pointer
    for (auto i = 0; i < len; i++)
    {
        m_Mem[dest_addr + i] = m_Mem[source_addr + i];
    }
    return true;
}
