#include "cpu6502.h"

// debug
#include <iostream.>

CPU6502::CPU6502(std::size_t memory_size):
    m_MemorySize(memory_size)
{
    // Allocate Memory
    m_Memory = new unsigned char[memory_size]();
    std::cout << "Allocated " << memory_size << " bytes of memory." << std::endl;
}

CPU6502::~CPU6502()
{
    delete[] m_Memory;
}

std::size_t CPU6502::getMemorySize() const
{
    return m_MemorySize;
}
