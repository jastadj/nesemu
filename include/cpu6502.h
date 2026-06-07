#ifndef CPU6502_H
#define CPU6502_H

#include <cstdlib>

class CPU6502
{
public:
    CPU6502(std::size_t memory_size = 0x10000);
    ~CPU6502();
    
    unsigned char* m_Memory;
    std::size_t getMemorySize() const;

private:

    std::size_t m_MemorySize;
};

#endif