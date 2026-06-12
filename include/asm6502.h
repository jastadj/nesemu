#ifndef ASM6502_H
#define ASM6502_H

#include <string>

namespace ASM6502
{
    bool assemble(std::string input_asm, std::string output_asm);
};

#endif