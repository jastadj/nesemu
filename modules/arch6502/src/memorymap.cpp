#include "memorymap.h"

#include "submemorymap.h"

// Debug
#include <iostream>
#include <iomanip>

MemoryMap::MemoryMap(std::size_t init_size):
    MemoryBank(init_size)
{

}

MemoryMap::~MemoryMap()
{
    for (auto submap : m_SubMemoryMaps)
    {
        delete submap;
    }
    m_SubMemoryMaps.clear();
}


bool MemoryMap::addMirror(std::size_t source_addr, std::size_t dest_addr, std::size_t len)
{
    // Check for mirror range validity
    if (m_MemoryBanks.empty() || (source_addr + len > m_MemoryBanks[0].size()) || (dest_addr + len > m_MemoryBanks[0].size()))
    {
        std::cerr << "Error adding mirror source 0x" << std::hex << std::setw(4) << std::setfill('0') << source_addr;
        std::cerr << " dest 0x" << std::hex << dest_addr;
        std::cerr << " len " << std::dec << std::setw(0) << len;
        std::cerr << " bank_size " << m_MemoryBanks[0].size() << std::endl;
        return false;
    }

    // Delete destination map and replace with source pointer
    for (auto i = 0; i < len; i++)
    {
        m_MemoryBanks[0][dest_addr + i] = m_MemoryBanks[0][source_addr + i];
    }
    return true;
}

const uint8_t MemoryMap::get(std::size_t addr, bool* ok) const
{
    if (addr < m_MemorySize)
    {
        if (ok != nullptr)
        {
            *ok = true;
        }
        for (auto submap : m_SubMemoryMaps)
        {
            if (addr >= submap->getOffset() && addr < (submap->getOffset() + submap->size()))
            {
                return submap->get(addr - submap->getOffset(), ok);
            }
        }
        return *m_MemoryBanks[m_SelectedBank][addr];
    }
    if (ok != nullptr)
    {
        *ok = false;
    }
    return 0;
}

bool MemoryMap::set(std::size_t addr, const uint8_t val)
{
    if (addr < m_MemorySize)
    {
        for (auto submap : m_SubMemoryMaps)
        {
            if (addr >= submap->getOffset() && addr < (submap->getOffset() + submap->size()))
            {
                return submap->set(addr - submap->getOffset(), val);
            }
        }
        *m_MemoryBanks[m_SelectedBank][addr] = val;
        return true;
    }
    return false;
}

// Sub-Banks
SubMemoryMap* MemoryMap::addSubMap(std::size_t offset, std::size_t len)
{
    // Check to make sure this isn't overlapping with any other sub-memory maps
    for (auto& submap : m_SubMemoryMaps)
    {
        if (offset >= submap->getOffset() && offset < submap->getOffset() + submap->size())
        {
            return nullptr;
        }
        if ((offset + len) >= submap->getOffset() && (offset + len) < submap->getOffset() + submap->size())
        {
            return nullptr;
        }
    }

    SubMemoryMap* new_submap = new SubMemoryMap(offset, len);
    m_SubMemoryMaps.push_back(new_submap);
    return new_submap;
}

std::size_t MemoryMap::getSubMaps() const
{
    return m_SubMemoryMaps.size();
}

SubMemoryMap* MemoryMap::getSubMap(unsigned int index)
{
    if (index < (unsigned int)m_SubMemoryMaps.size())
    {
        return m_SubMemoryMaps[index];
    }
    return nullptr;
}