#ifndef CLASS_RP2A03
#define CLASS_RP2A03

#include "c6502.hpp"

class RP2A03 : public C6502
{
private:

public:
    RP2A03(uint8_t *memory, unsigned int memory_size);

    void debugConsole(std::string prompt);
};


#endif // CLASS_RP2A03

