#ifndef OPCODES6502_H
#define OPCODES6502_H

#include <map>
#include <string>

#include "defs6502.h"

namespace Arch6502
{
    class OpCode
    {
    public:
        std::string mnemonic;
        uint8_t code;
        virtual bool execute() = 0;
        OpCode() {};
    };

    class OpCodeNOP : public OpCode
    {
        bool execute();
    };
}


#endif