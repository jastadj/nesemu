#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include <cstdlib>
#include <vector>
#include <memory>
#include <string>

class MemoryMap
{

public:
    MemoryMap(std::size_t init_size);
    ~MemoryMap();

    const std::size_t size() const;

    const uint8_t get(std::size_t addr, bool* ok = nullptr) const;
    bool set(std::size_t addr, const uint8_t val);

    void fill(const uint8_t val);
    void fillRandom();

    // Memory Mirroring
    bool addMirror(std::size_t source_addr, std::size_t dest_addr, std::size_t len);

private:
    std::vector<std::shared_ptr<uint8_t> > m_Mem;
};

#endif