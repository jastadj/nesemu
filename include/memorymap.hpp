#ifndef CLASS_MEMORYMAP
#define CLASS_MEMORYMAP

#include <iostream>

class MemoryMap
{
protected:

    unsigned int m_MemSize;

    uint8_t *m_Mem;
    uint8_t **m_MemMap;

public:
    MemoryMap(unsigned int memsize);
    ~MemoryMap();

    void clear();
    bool clear(unsigned int startaddress, unsigned int endaddress);

    uint8_t **getMap() { return m_MemMap;}
    unsigned int getSize() { return m_MemSize;}

    bool mirror(unsigned int start1, unsigned int end1, unsigned int start2, unsigned int end2);
    bool clearMirror(unsigned int startaddress, unsigned int endaddress);

    bool write(unsigned int addresss, uint8_t val);
    uint8_t read(unsigned int addresss);
};

#endif // CLASS_MEMORYMAP

