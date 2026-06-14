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

        public:
            virtual void execute(CPU& cpu, ADDRESS_MODE mode) = 0;
            OpFunc(std::string mnemonic, std::string desc, std::map<ADDRESS_MODE, uint8_t> modemap);

            std::string getMnemonic() const;
            std::string getDescription() const;
            std::map<ADDRESS_MODE, uint8_t> getModeMap() const;
        protected:
            std::string m_Mnemonic;
            std::string m_Description;
            std::map<ADDRESS_MODE, uint8_t> m_ModeMap;
        };


        ////////////////
        // OP FUNC TYPES


        // BRK - Force Break / Interrupt
        class OpCodeBRK : public OpFunc
        {
        public:
            OpCodeBRK() : OpFunc("brk", "Force break.", {
                {ADDRESS_MODE::IMPLIED, 0x00}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // LDX - Load X with Memory
        class OpFuncLDX : public OpFunc
        {
        public:
            OpFuncLDX() : OpFunc("ldx", "Load X with memory.", {
                {ADDRESS_MODE::IMMEDIATE, 0xa2},
                {ADDRESS_MODE::ZERO_PAGE, 0xa6},
                {ADDRESS_MODE::ZERO_PAGE_Y, 0xb6},
                {ADDRESS_MODE::ABSOLUTE, 0xae},
                {ADDRESS_MODE::ABSOLUTE_Y, 0xbe}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };
    }
}


#endif