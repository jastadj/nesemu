#include "memorymap.h"

// Debug
#include <iostream>

MemoryMap::MemoryMap(std::size_t init_size):
    m_MemorySize(init_size),
    m_SelectedBank(0)
{
    addBank();
}

MemoryMap::~MemoryMap()
{
    m_MemoryMap.clear();
}

const std::size_t MemoryMap::size() const
{
    return m_MemorySize;
}

const uint8_t MemoryMap::get(std::size_t addr, bool* ok) const
{
    if (addr < m_MemorySize)
    {
        if (ok != nullptr)
        {
            *ok = true;
        }
        return *m_MemoryMap[m_SelectedBank][addr];
    }
    if (ok != nullptr)
    {
        *ok = false;
    }
    return 0;
}

bool MemoryMap::set(std::size_t addr, const uint8_t val)
{
    if (addr < m_MemoryMap.size())
    {
        *m_MemoryMap[m_SelectedBank][addr] = val;
        return true;
    }
    return false;
}

void MemoryMap::fill(const uint8_t val)
{
    for (auto i = 0; i < m_MemoryMap.size(); i++)
    {
        set(i, val);
    }
}

void MemoryMap::fillRandom()
{
    for (auto i = 0; i < m_MemoryMap.size(); i++)
    {
        set(i, rand() % 256);
    }
}

bool MemoryMap::addMirror(std::size_t source_addr, std::size_t dest_addr, std::size_t len)
{
    if ((source_addr + len > m_MemoryMap.size()) || (dest_addr + len > m_MemoryMap.size()))
    {
        return false;
    }

    // Delete destination map and replace with source pointer
    for (auto i = 0; i < len; i++)
    {
        m_MemoryMap[dest_addr + i] = m_MemoryMap[source_addr + i];
    }
    return true;
}

// Banks

void MemoryMap::addBank()
{
    std::vector< std::shared_ptr<uint8_t> > bank;
    for (auto i = 0; i < size(); i++)
    {
        bank.emplace_back(std::make_shared<uint8_t>());
    }
    m_MemoryMap.push_back(bank);
}

bool MemoryMap::deleteBank()
{
    if (getBanks() > 1)
    {
        m_MemoryMap.pop_back();
        return true;
    }
    return false;
}

std::size_t MemoryMap::getBanks() const
{
    return m_MemoryMap.size();
}

bool MemoryMap::selectBank(unsigned int index)
{
    if (index < getBanks())
    {
        m_SelectedBank = index;
        return true;
    }
    return false;
}

unsigned int MemoryMap::selectedBank() const
{
    return m_SelectedBank;
}