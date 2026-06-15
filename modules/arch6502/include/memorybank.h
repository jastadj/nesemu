#ifndef MEMORYBANK_H
#define MEMORYBANK_H

#include <cstdlib>
#include <vector>
#include <memory>
#include <string>

class MemoryBank
{

public:
    MemoryBank(std::size_t init_size);
    virtual ~MemoryBank();

    const std::size_t size() const;

    virtual const uint8_t get(std::size_t addr, bool* ok = nullptr) const;
    virtual bool set(std::size_t addr, const uint8_t val);

    void fill(const uint8_t val);
    void fillRandom();

    // Banks
    void addBank();
    bool deleteBank();
    std::size_t getBanks() const;
    bool selectBank(unsigned int index);
    unsigned int selectedBank() const;

protected:
    std::size_t m_MemorySize;
    std::vector<std::vector<std::shared_ptr<uint8_t> > > m_MemoryBanks;
    unsigned int m_SelectedBank;
};

#endif