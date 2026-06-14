#ifndef OPCODES6502_H
#define OPCODES6502_H

#include <map>
#include <string>

#include "defs6502.h"

#include <vector>

namespace Arch6502
{
    class CPU;

    class OpCode
    {
    public:
        virtual void execute(CPU& cpu) = 0;
        OpCode(std::string mnemonic, uint8_t code, ADDRESS_MODE mode);
        OpCode(std::string mnemonic, std::map<ADDRESS_MODE, uint8_t> modes);

        std::map<ADDRESS_MODE, uint8_t> modes;

        static std::vector<OpCode*> codes;
        static bool initCodes();

        const std::string getMnemonic() const;
        const uint8_t getCode() const;
        const ADDRESS_MODE getAddressMode() const;
    protected:
        std::string mnemonic;
        uint8_t code;
        ADDRESS_MODE mode;
    };

    // 0x00 - BRK - Force Break / Interrupt
    class OpCodeBRK : public OpCode
    {
    public:
        OpCodeBRK() : OpCode("brk", 0x00, ADDRESS_MODE::IMPLIED) {};
        void execute(CPU& cpu);
    };

    // ...

    ///////////////////////
    // 
    //    LDA
    //

    // 0xa9 - LDA - Load Accumulator With Memory
    class OpCodeLDAImmediate : public OpCode
    {
    public:
        OpCodeLDAImmediate() : OpCode("lda", 0xa9, ADDRESS_MODE::IMMEDIATE) {};
        void execute(CPU& cpu);
    };

    // 0xa5 - LDA - Load Accumulator With Memory
    class OpCodeLDAZeroPage : public OpCode
    {
    public:
        OpCodeLDAZeroPage() : OpCode("lda", 0xa5, ADDRESS_MODE::ZERO_PAGE) {};
        void execute(CPU& cpu);
    };

    ///////////////////////
    // 
    //    LDX
    //

    // 0xa2 - LDX - Load X With Memory
    class OpCodeLDXImmediate : public OpCode
    {
    public:
        OpCodeLDXImmediate() : OpCode("ldx", 0xa2, ADDRESS_MODE::IMMEDIATE) {};
        void execute(CPU& cpu);
    };

    // 0xa6 - LDX - Load X With Memory
    class OpCodeLDXZeroPage : public OpCode
    {
    public:
        OpCodeLDXZeroPage() : OpCode("ldx", 0xa6, ADDRESS_MODE::ZERO_PAGE) {};
        void execute(CPU& cpu);
    };

    // 0xb6 - LDX - Load X With Memory
    class OpCodeLDXZeroPageY : public OpCode
    {
    public:
        OpCodeLDXZeroPageY() : OpCode("ldx", 0xb6, ADDRESS_MODE::ZERO_PAGE_Y) {};
        void execute(CPU& cpu);
    };

    // 0xae - LDX - Load X With Memory
    class OpCodeLDXAbsolute : public OpCode
    {
    public:
        OpCodeLDXAbsolute() : OpCode("ldx", 0xae, ADDRESS_MODE::ABSOLUTE) {};
        void execute(CPU& cpu);
    };

    // 0xbe - LDX - Load X With Memory
    class OpCodeLDXAbsoluteY : public OpCode
    {
    public:
        OpCodeLDXAbsoluteY() : OpCode("ldx", 0xbe, ADDRESS_MODE::ABSOLUTE_Y) {};
        void execute(CPU& cpu);
    };


    ///////////////////////
    // 
    //    LDY
    //

    // 0xa0 - LDY - Load Y With Memory
    class OpCodeLDYImmediate : public OpCode
    {
    public:
        OpCodeLDYImmediate() : OpCode("ldy", 0xa0, ADDRESS_MODE::IMMEDIATE) {};
        void execute(CPU& cpu);
    };

    // 0xa4 - LDY - Load Y With Memory
    class OpCodeLDYZeroPage : public OpCode
    {
    public:
        OpCodeLDYZeroPage() : OpCode("ldy", 0xa4, ADDRESS_MODE::ZERO_PAGE) {};
        void execute(CPU& cpu);
    };

    // 0xb4 - LDY - Load Y With Memory
    class OpCodeLDYZeroPageX : public OpCode
    {
    public:
        OpCodeLDYZeroPageX() : OpCode("ldy", 0xb4, ADDRESS_MODE::ZERO_PAGE_X) {};
        void execute(CPU& cpu);
    };

    // 0xac - LDY - Load Y With Memory
    class OpCodeLDYAbsolute : public OpCode
    {
    public:
        OpCodeLDYAbsolute() : OpCode("ldy", 0xac, ADDRESS_MODE::ABSOLUTE) {};
        void execute(CPU& cpu);
    };

    // 0xbc - LDY - Load Y With Memory
    class OpCodeLDYAbsoluteY : public OpCode
    {
    public:
        OpCodeLDYAbsoluteY() : OpCode("ldy", 0xbc, ADDRESS_MODE::ABSOLUTE_Y) {};
        void execute(CPU& cpu);
    };
}


#endif