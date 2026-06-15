#include "memorybank.h"

#include "memorymap.h"

// Debug
#include <iostream>

MemoryBank::MemoryBank(std::size_t init_size) :
    m_MemorySize(init_size),
    m_SelectedBank(0)
{
    addBank();
}

MemoryBank::~MemoryBank()
{
    m_MemoryBanks.clear();
}

const std::size_t MemoryBank::size() const
{
    return m_MemorySize;
}

const uint8_t MemoryBank::get(std::size_t addr, bool* ok) const
{
    if (addr < m_MemorySize)
    {
        if (ok != nullptr)
        {
            *ok = true;
        }
        return *m_MemoryBanks[m_SelectedBank][addr];
    }
    if (ok != nullptr)
    {
        *ok = false;
    }
    return 0;
}

bool MemoryBank::set(std::size_t addr, const uint8_t val)
{
    if (addr < m_MemoryBanks.size())
    {
        *m_MemoryBanks[m_SelectedBank][addr] = val;
        return true;
    }
    return false;
}

void MemoryBank::fill(const uint8_t val)
{
    for (auto i = 0; i < m_MemoryBanks.size(); i++)
    {
        set(i, val);
    }
}

void MemoryBank::fillRandom()
{
    for (auto i = 0; i < m_MemoryBanks.size(); i++)
    {
        set(i, rand() % 256);
    }
}

// Banks

void MemoryBank::addBank()
{
    std::vector< std::shared_ptr<uint8_t> > bank;
    for (auto i = 0; i < size(); i++)
    {
        bank.emplace_back(std::make_shared<uint8_t>());
    }
    m_MemoryBanks.push_back(bank);
}

bool MemoryBank::deleteBank()
{
    if (getBanks() > 1)
    {
        m_MemoryBanks.pop_back();
        return true;
    }
    return false;
}

std::size_t MemoryBank::getBanks() const
{
    return m_MemoryBanks.size();
}

bool MemoryBank::selectBank(unsigned int index)
{
    if (index < getBanks())
    {
        m_SelectedBank = index;
        return true;
    }
    return false;
}

unsigned int MemoryBank::selectedBank() const
{
    return m_SelectedBank;
}
