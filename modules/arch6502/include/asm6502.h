#ifndef ASM6502_H
#define ASM6502_H

#include <string>
#include <vector>

#include "defs6502.h"

namespace Arch6502
{
    class OpCode;

    namespace ASM
    {
        bool assemble(std::string input_asm_file, std::string output_nes_file);

        std::vector<uint8_t> getBytes(std::string asm_line);
        std::vector<uint16_t> parseOperand(std::string operand, ADDRESS_MODE& mode);
        OpCode* getOpCode(std::string mnemonic, ADDRESS_MODE mode);
    };
}

#endif