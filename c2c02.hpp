#ifndef CLASS_C2C02
#define CLASS_C2C02

#include <cstdlib>
#include <iostream>
#include <vector>

class C2C02
{
private:

    // memory
    uint8_t **m_Mem;
    unsigned int m_MemSize;

    bool init();

public:
    C2C02(uint8_t **memory, unsigned int memory_size);
    ~C2C02();

    // debug
    void debugConsole(std::string prompt);

};
#endif // CLASS_C2C02
