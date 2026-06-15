#ifndef OPCODES6502_H
#define OPCODES6502_H

#include <map>
#include <string>

#include "defs6502.h"

#include <vector>

#define OPCODE_COUNT 256

namespace Arch6502
{
    class CPU;

    namespace OpCodes
    {
        // Forward
        class OpFunc;

        // Master opcode list and LUT croff references
        // for addressing modes, etc
        class LUT
        {
        public:
            static bool init();

            // Master OpCode List
            static std::vector<OpFunc*> codes;
            // LUT of Code Modes
            static std::vector<ADDRESS_MODE> modes;

            static OpFunc* getOpFuncByMnemonic(std::string mnemonic);
            
            // Purely static
            LUT() = delete;
        private:
            static bool registerOpCode(OpFunc* opcode);
        };

        // Opcode Function
        // E.g. LDX, LDY, JMP
        class OpFunc
        {
        public:
            struct OpCodeInfo
            {
                ADDRESS_MODE mode;
                uint8_t code;
                unsigned int cycles;
                OpCodeInfo(ADDRESS_MODE mode, uint8_t code, unsigned int cycles)
                {
                    if (!cycles)
                    {
                        cycles = 1;
                    }
                    this->mode = mode;
                    this->code = code;
                    this->cycles = cycles;
                }
            };

        public:
            virtual void execute(CPU& cpu, ADDRESS_MODE mode) = 0;
            OpFunc(std::string mnemonic, std::string desc, std::vector<OpCodeInfo> opcodeinfos);

            std::string getMnemonic() const;
            std::string getDescription() const;
            std::vector<OpCodeInfo> getOpCodeInfos() const;
        protected:
            std::string m_Mnemonic;
            std::string m_Description;
            std::vector<OpCodeInfo> m_OpCodeInfos;
        };


        ////////////////
        // OP FUNC TYPES

        // ADC - Add memory to accumulator with carry
        class OpCodeADC : public OpFunc
        {
        public:
            OpCodeADC() : OpFunc("adc", "Add memory to accumulator with carry.", {
                {ADDRESS_MODE::IMMEDIATE, 0x69,2},
                {ADDRESS_MODE::ZERO_PAGE, 0x65, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x75, 4},
                {ADDRESS_MODE::ABSOLUTE, 0x60, 4},
                {ADDRESS_MODE::ABSOLUTE_X, 0x70, 4},
                {ADDRESS_MODE::ABSOLUTE_Y, 0x79, 4},
                {ADDRESS_MODE::INDIRECT_X, 0x61, 6},
                {ADDRESS_MODE::INDIRECT_Y, 0x71, 5},
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // AND - And memory with Accumulator
        class OpCodeAND : public OpFunc
        {
        public:
            OpCodeAND() : OpFunc("and", "AND memory with accumulator", {
                {ADDRESS_MODE::IMMEDIATE, 0x29, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0x25, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x35, 4},
                {ADDRESS_MODE::ABSOLUTE, 0x2d, 4},
                {ADDRESS_MODE::ABSOLUTE_X, 0x3d, 4},
                {ADDRESS_MODE::ABSOLUTE_Y, 0x39, 4},
                {ADDRESS_MODE::INDIRECT_X, 0x21, 6},
                {ADDRESS_MODE::INDIRECT_Y, 0x31, 5},
                }) {};
            void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BRK - Force Break / Interrupt
        class OpCodeBRK : public OpFunc
        {
        public:
            OpCodeBRK() : OpFunc("brk", "Force break.", {
                {ADDRESS_MODE::IMPLIED, 0x00, 7}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // LDA - Load A with Memory
        class OpFuncLDA : public OpFunc
        {
        public:
            OpFuncLDA() : OpFunc("lda", "Load A with memory.", {
                {ADDRESS_MODE::IMMEDIATE, 0xa9, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0xa5, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0xb5, 4},
                {ADDRESS_MODE::ABSOLUTE, 0xae, 4},
                {ADDRESS_MODE::ABSOLUTE_X, 0xbd, 4},
                {ADDRESS_MODE::ABSOLUTE_Y, 0xb9, 4},
                {ADDRESS_MODE::INDIRECT_X, 0xa1, 6},
                {ADDRESS_MODE::INDIRECT_Y, 0xb1, 5}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // LDX - Load X with Memory
        class OpFuncLDX : public OpFunc
        {
        public:
            OpFuncLDX() : OpFunc("ldx", "Load X with memory.", {
                {ADDRESS_MODE::IMMEDIATE, 0xa2, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0xa6, 3},
                {ADDRESS_MODE::ZERO_PAGE_Y, 0xb6, 4},
                {ADDRESS_MODE::ABSOLUTE, 0xae, 4},
                {ADDRESS_MODE::ABSOLUTE_Y, 0xbe, 4}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // LDY - Load Y with Memory
        class OpFuncLDY : public OpFunc
        {
        public:
            OpFuncLDY() : OpFunc("ldy", "Load Y with memory.", {
                {ADDRESS_MODE::IMMEDIATE, 0xa0, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0xa4, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0xb4, 4},
                {ADDRESS_MODE::ABSOLUTE, 0xac, 4},
                {ADDRESS_MODE::ABSOLUTE_X, 0xbc, 4}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };
    }
}


#endif