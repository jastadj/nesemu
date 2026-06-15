#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "memorybank.h"

class SubMemoryMap;

class MemoryMap: public MemoryBank
{

public:
    MemoryMap(std::size_t init_size);
    ~MemoryMap();

    bool addMirror(std::size_t source_addr, std::size_t dest_addr, std::size_t len);

    const uint8_t get(std::size_t addr, bool* ok = nullptr) const;
    bool set(std::size_t addr, const uint8_t val);

    // Sub-Banks
    SubMemoryMap* addSubMap(std::size_t offset, std::size_t len);
    std::size_t getSubMaps() const;
    SubMemoryMap* getSubMap(unsigned int index);

protected:

    std::vector<SubMemoryMap*> m_SubMemoryMaps;
};

#endif