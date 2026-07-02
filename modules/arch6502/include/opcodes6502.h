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

            static bool updateStatusBitFromResult(CPU& cpu, STATUS_BIT status_bit, uint16_t result);
        };


        ////////////////
        // OP FUNC TYPES
        
        /*
        // ZZZ - Some description
        class OpFuncZZZ : public OpFunc
        {
        public:
            OpFuncZZZ() : OpFunc("zzz", "Some description", {
                {ADDRESS_MODE::IMMEDIATE, 0x, },
                {ADDRESS_MODE::ZERO_PAGE, 0x, },
                {ADDRESS_MODE::ZERO_PAGE_X, 0x, },
                {ADDRESS_MODE::ABSOLUTE, 0x, },
                {ADDRESS_MODE::ABSOLUTE_X, 0x, },
                {ADDRESS_MODE::ABSOLUTE_Y, 0x, },
                {ADDRESS_MODE::INDIRECT_X, 0x, },
                {ADDRESS_MODE::INDIRECT_Y, 0x, },
                }) {};
            void execute(CPU& cpu, ADDRESS_MODE mode);
        };
        */

        // ADC - Add memory to accumulator with carry
        class OpFuncADC : public OpFunc
        {
        public:
            OpFuncADC() : OpFunc("adc", "Add memory to accumulator with carry.", {
                {ADDRESS_MODE::IMMEDIATE, 0x69,2},
                {ADDRESS_MODE::ZERO_PAGE, 0x65, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x75, 4},
                {ADDRESS_MODE::ABSOLUTE, 0x6d, 4},
                {ADDRESS_MODE::ABSOLUTE_X, 0x7d, 4},
                {ADDRESS_MODE::ABSOLUTE_Y, 0x79, 4},
                {ADDRESS_MODE::INDIRECT_X, 0x61, 6},
                {ADDRESS_MODE::INDIRECT_Y, 0x71, 5},
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // AND - And memory with Accumulator
        class OpFuncAND : public OpFunc
        {
        public:
            OpFuncAND() : OpFunc("and", "AND memory with accumulator", {
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

        // ASL - Shift left one bit
        class OpFuncASL : public OpFunc
        {
        public:
            OpFuncASL() : OpFunc("asl", "Shift left one bit", {
                {ADDRESS_MODE::ACCUMULATOR, 0x0a, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0x06, 5},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x16, 6},
                {ADDRESS_MODE::ABSOLUTE, 0x0e, 6},
                {ADDRESS_MODE::ABSOLUTE_X, 0x1e, 7}
                }) {};
            void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BCC - Branch on Carry Clear
        class OpFuncBCC : public OpFunc
        {
        public:
            OpFuncBCC () : OpFunc("bcc", "Branch on Carry Clear", {
                {ADDRESS_MODE::RELATIVE, 0x90, 2}
                }) {};
            void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BCS - Branch on Carry Set
        class OpFuncBCS : public OpFunc
        {
        public:
            OpFuncBCS() : OpFunc("bcs", "Branch on Carry Set", {
                {ADDRESS_MODE::RELATIVE, 0xb0, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BEQ - Branch on Result Zero
        class OpFuncBEQ : public OpFunc
        {
        public:
            OpFuncBEQ() : OpFunc("beq", "Branch on Result Zero", {
                {ADDRESS_MODE::RELATIVE, 0xf0, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BIT - Test bits in memory with accumulator
        class OpFuncBIT : public OpFunc
        {
        public:
            OpFuncBIT() : OpFunc("bit", "Test bits in memory with accumulator", {
                {ADDRESS_MODE::ZERO_PAGE, 0x24, 3},
                {ADDRESS_MODE::ABSOLUTE, 0x2c, 4}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BMI - Branch on Result Minus
        class OpFuncBMI : public OpFunc
        {
        public:
            OpFuncBMI() : OpFunc("bmi", "Branch on Result Minus", {
                {ADDRESS_MODE::RELATIVE, 0x30, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BNE - Branch on Result Not Zero
        class OpFuncBNE : public OpFunc
        {
        public:
            OpFuncBNE() : OpFunc("bne", "Branch on Result Not Zero", {
                {ADDRESS_MODE::RELATIVE, 0xd0, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BPL - Branch on Result Plus
        class OpFuncBPL : public OpFunc
        {
        public:
            OpFuncBPL() : OpFunc("bpl", "Branch on Result Plus", {
                {ADDRESS_MODE::RELATIVE, 0x10, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BRK - Force Break / Interrupt
        class OpFuncBRK : public OpFunc
        {
        public:
            OpFuncBRK() : OpFunc("brk", "Force break.", {
                {ADDRESS_MODE::IMPLIED, 0x00, 7}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BVC - Branch on Overflow Clear
        class OpFuncBVC : public OpFunc
        {
        public:
            OpFuncBVC() : OpFunc("bvc", "Branch on Overflow Clear", {
                {ADDRESS_MODE::RELATIVE, 0x50, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // BVS - Branch on Overflow Set
        class OpFuncBVS : public OpFunc
        {
        public:
            OpFuncBVS() : OpFunc("bvs", "Branch on Overflow Set", {
                {ADDRESS_MODE::RELATIVE, 0x70, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // CLC - Clear carry flag
        class OpFuncCLC : public OpFunc
        {
        public:
            OpFuncCLC() : OpFunc("clc", "Clear carry flag", {
                {ADDRESS_MODE::IMPLIED, 0x18, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // CLD - Clear decimal mode
        class OpFuncCLD : public OpFunc
        {
        public:
            OpFuncCLD() : OpFunc("cld", "Clear decimal mode", {
                {ADDRESS_MODE::IMPLIED, 0xd8, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // CLI - Clear interrupt disable bit
        class OpFuncCLI : public OpFunc
        {
        public:
            OpFuncCLI() : OpFunc("cli", "Clear interrupt disable bit", {
                {ADDRESS_MODE::IMPLIED, 0x58, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // CLV - Clear overflow flag
        class OpFuncCLV : public OpFunc
        {
        public:
            OpFuncCLV() : OpFunc("clv", "Clear overflow flag", {
                {ADDRESS_MODE::IMPLIED, 0xb8, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // CMP - Compare memory and accumulator
        class OpFuncCMP : public OpFunc
        {
        public:
            OpFuncCMP() : OpFunc("cmp", "Compare memory and accumulator.", {
                {ADDRESS_MODE::IMMEDIATE, 0xc9, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0xc5, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0xd5, 4},
                {ADDRESS_MODE::ABSOLUTE, 0xcd, 4},
                {ADDRESS_MODE::ABSOLUTE_X, 0xdd, 4},
                {ADDRESS_MODE::ABSOLUTE_Y, 0xd9, 4},
                {ADDRESS_MODE::INDIRECT_X, 0xc1, 6},
                {ADDRESS_MODE::INDIRECT_Y, 0xd1, 5}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // CPX - Compare memory and Index X
        class OpFuncCPX : public OpFunc
        {
        public:
            OpFuncCPX() : OpFunc("cpx", "Compare memory and Index X.", {
                {ADDRESS_MODE::IMMEDIATE, 0xe0, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0xe4, 3},
                {ADDRESS_MODE::ABSOLUTE, 0xec, 4}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // CPY - Compare memory and Index Y
        class OpFuncCPY : public OpFunc
        {
        public:
            OpFuncCPY() : OpFunc("cpy", "Compare memory and Index Y.", {
                {ADDRESS_MODE::IMMEDIATE, 0xc0, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0xc4, 3},
                {ADDRESS_MODE::ABSOLUTE, 0xcc, 4}
                }) {};
            void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // DEC - Decrement memory by one.
        class OpFuncDEC : public OpFunc
        {
        public:
            OpFuncDEC() : OpFunc("dec", "Decrement memory by one.", {
                {ADDRESS_MODE::ZERO_PAGE, 0xc6, 5},
                {ADDRESS_MODE::ZERO_PAGE_X, 0xd6, 6},
                {ADDRESS_MODE::ABSOLUTE, 0xce, 6},
                {ADDRESS_MODE::ABSOLUTE_X, 0xde, 7}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // DEX - Decrement index X by one
        class OpFuncDEX : public OpFunc
        {
        public:
            OpFuncDEX() : OpFunc("dex", "Decrement index X by one.", {
                {ADDRESS_MODE::IMPLIED, 0xca, 2}
                }) {};
            void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // DEY - Decrement index Y by one
        class OpFuncDEY : public OpFunc
        {
        public:
            OpFuncDEY() : OpFunc("dey", "Decrement index Y by one.", {
                {ADDRESS_MODE::IMPLIED, 0x88, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // EOR - Exclusive OR with memory with accumulator
        class OpFuncEOR : public OpFunc
        {
        public:
            OpFuncEOR() : OpFunc("eor", "Exlusive-Or memory with accumulator.", {
                {ADDRESS_MODE::IMMEDIATE, 0x49, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0x45, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x55, 4},
                {ADDRESS_MODE::ABSOLUTE, 0x4D, 4},
                {ADDRESS_MODE::ABSOLUTE_X, 0x5D, 4},
                {ADDRESS_MODE::ABSOLUTE_Y, 0x59, 4},
                {ADDRESS_MODE::INDIRECT_X, 0x41, 6},
                {ADDRESS_MODE::INDIRECT_Y, 0x51, 5}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // INC - Increment memory by one.
        class OpFuncINC : public OpFunc
        {
        public:
            OpFuncINC() : OpFunc("inc", "Increment memory by one.", {
                {ADDRESS_MODE::ZERO_PAGE, 0xe6, 5},
                {ADDRESS_MODE::ZERO_PAGE_X, 0xf6, 6},
                {ADDRESS_MODE::ABSOLUTE, 0xee, 6},
                {ADDRESS_MODE::ABSOLUTE_X, 0xfe, 7}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // INX - Increment index X by one
        class OpFuncINX : public OpFunc
        {
        public:
            OpFuncINX() : OpFunc("inx", "Increment index X by one.", {
                {ADDRESS_MODE::IMPLIED, 0xe8, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // INY - Increment index Y by one
        class OpFuncINY : public OpFunc
        {
        public:
            OpFuncINY() : OpFunc("iny", "Increment index Y by one.", {
                {ADDRESS_MODE::IMPLIED, 0xc8, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // JMP - Jump to new location
        class OpFuncJMP : public OpFunc
        {
        public:
            OpFuncJMP() : OpFunc("jmp", "Jump to new location.", {
                {ADDRESS_MODE::ABSOLUTE, 0x4c, 3},
                {ADDRESS_MODE::INDIRECT, 0x6c, 5}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // JSR - Jump to new location saving return address
        class OpFuncJSR : public OpFunc
        {
        public:
            OpFuncJSR() : OpFunc("jsr", "Jump to new location saving return address.", {
                {ADDRESS_MODE::ABSOLUTE, 0x20, 6}
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

        // LSR - Shift right one bit
        class OpFuncLSR : public OpFunc
        {
        public:
            OpFuncLSR() : OpFunc("lsr", "Shift right one bit.", {
                {ADDRESS_MODE::ACCUMULATOR, 0x4a, 2},
                {ADDRESS_MODE::ZERO_PAGE,   0x46, 5},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x56, 6},
                {ADDRESS_MODE::ABSOLUTE,    0x4e, 6},
                {ADDRESS_MODE::ABSOLUTE_X,  0x5e, 7}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // NOP - No operation
        class OpFuncNOP : public OpFunc
        {
        public:
            OpFuncNOP() : OpFunc("nop", "No operation.", {
                {ADDRESS_MODE::IMPLIED, 0xea, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // ORA - OR memory with accumulator
        class OpFuncORA : public OpFunc
        {
        public:
            OpFuncORA() : OpFunc("ora", "OR memory with accumulator.", {
                {ADDRESS_MODE::IMMEDIATE,   0x09, 2},
                {ADDRESS_MODE::ZERO_PAGE,   0x05, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x15, 4},
                {ADDRESS_MODE::ABSOLUTE,    0x0d, 4},
                {ADDRESS_MODE::ABSOLUTE_X,  0x1d, 4},
                {ADDRESS_MODE::ABSOLUTE_Y,  0x19, 4},
                {ADDRESS_MODE::INDIRECT_X,  0x01, 6},
                {ADDRESS_MODE::INDIRECT_Y,  0x11, 5}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // PHA - Push accumulator on stack
        class OpFuncPHA : public OpFunc
        {
        public:
            OpFuncPHA() : OpFunc("pha", "Push accumulator on stack.", {
                {ADDRESS_MODE::IMPLIED, 0x48, 3}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // PHP - Push processor status on stack
        class OpFuncPHP : public OpFunc
        {
        public:
            OpFuncPHP() : OpFunc("php", "Push processor status on stack.", {
                {ADDRESS_MODE::IMPLIED, 0x08, 3}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // PLA - Pull accumulator from stack
        class OpFuncPLA : public OpFunc
        {
        public:
            OpFuncPLA() : OpFunc("pla", "Pull accumulator from stack.", {
                {ADDRESS_MODE::IMPLIED, 0x68, 4}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // ROL - Rotate one bit left (memory or accumulator)
        class OpFuncROL: public OpFunc
        {
        public:
            OpFuncROL() : OpFunc("rol", "Rotate one bit left.", {
                {ADDRESS_MODE::ACCUMULATOR, 0x2a, 2},
                {ADDRESS_MODE::ZERO_PAGE,   0x26, 5},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x36, 6},
                {ADDRESS_MODE::ABSOLUTE,    0x2e, 6},
                {ADDRESS_MODE::ABSOLUTE_X,  0x3e, 7}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // ROR - Rotate one bit right (memory or accumulator)
        class OpFuncROR : public OpFunc
        {
        public:
            OpFuncROR() : OpFunc("ror", "Rotate one bit right.", {
                {ADDRESS_MODE::ACCUMULATOR, 0x6a, 2},
                {ADDRESS_MODE::ZERO_PAGE,   0x66, 5},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x76, 6},
                {ADDRESS_MODE::ABSOLUTE,    0x6e, 6},
                {ADDRESS_MODE::ABSOLUTE_X,  0x7e, 7}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // RTI - Return from interrupt.
        class OpFuncRTI : public OpFunc
        {
        public:
            OpFuncRTI() : OpFunc("rti", "Return from interrupt.", {
                {ADDRESS_MODE::IMPLIED, 0x40, 6}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // RTS - Return from subroutine.
        class OpFuncRTS : public OpFunc
        {
        public:
            OpFuncRTS() : OpFunc("rts", "Return from subroutine.", {
                {ADDRESS_MODE::IMPLIED, 0x60, 6}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // SBC - Subtract memory from accumulator with borrow
        class OpFuncSBC : public OpFunc
        {
        public:
            OpFuncSBC() : OpFunc("sbc", "Subtract memory from accumulator with borrow.", {
                {ADDRESS_MODE::IMMEDIATE, 0xe9, 2},
                {ADDRESS_MODE::ZERO_PAGE, 0xe5, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0xf5, 4},
                {ADDRESS_MODE::ABSOLUTE, 0xeD, 4},
                {ADDRESS_MODE::ABSOLUTE_X, 0xfD, 4},
                {ADDRESS_MODE::ABSOLUTE_Y, 0xf9, 4},
                {ADDRESS_MODE::INDIRECT_X, 0xe1, 6},
                {ADDRESS_MODE::INDIRECT_Y, 0xf1, 5}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // SEC - Set Carry flag
        class OpFuncSEC : public OpFunc
        {
        public:
            OpFuncSEC() : OpFunc("sec", "Set Carry flag.", {
                {ADDRESS_MODE::IMPLIED, 0x38, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // SED - Set Decimal flag
        class OpFuncSED : public OpFunc
        {
        public:
            OpFuncSED() : OpFunc("sed", "Set Decimal flag.", {
                {ADDRESS_MODE::IMPLIED, 0xf8, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // SEI - Set Interrupt Disable flag
        class OpFuncSEI : public OpFunc
        {
        public:
            OpFuncSEI() : OpFunc("sei", "Set Interrupt Disable flag.", {
                {ADDRESS_MODE::IMPLIED, 0x78, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // STA - Store Accumulator in Memory
        class OpFuncSTA : public OpFunc
        {
        public:
            OpFuncSTA() : OpFunc("sta", "Store Accumulator in memory.", {
                {ADDRESS_MODE::ZERO_PAGE, 0x85, 3},
                {ADDRESS_MODE::ZERO_PAGE_X, 0x95, 4},
                {ADDRESS_MODE::ABSOLUTE, 0x8d, 4},
                {ADDRESS_MODE::ABSOLUTE_X, 0x9d, 5},
                {ADDRESS_MODE::ABSOLUTE_Y, 0x99, 5},
                {ADDRESS_MODE::INDIRECT_X, 0x81, 6},
                {ADDRESS_MODE::INDIRECT_Y, 0x91, 6}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // STX - Store Index X in Memory
        class OpFuncSTX : public OpFunc
        {
        public:
            OpFuncSTX() : OpFunc("stx", "Store Index X in memory.", {
                {ADDRESS_MODE::ZERO_PAGE, 0x86, 3},
                {ADDRESS_MODE::ZERO_PAGE_Y, 0x96, 4},
                {ADDRESS_MODE::ABSOLUTE, 0x8e, 4}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // STY - Store Index Y in Memory
        class OpFuncSTY : public OpFunc
        {
        public:
            OpFuncSTY() : OpFunc("sty", "Store Index Y in memory.", {
                {ADDRESS_MODE::ZERO_PAGE, 0x84, 3},
                {ADDRESS_MODE::ZERO_PAGE_Y, 0x94, 4},
                {ADDRESS_MODE::ABSOLUTE, 0x8c, 4}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // TAX - Transfer Accumulator to Index X
        class OpFuncTAX : public OpFunc
        {
        public:
            OpFuncTAX() : OpFunc("tax", "Transfer Accumulator to Index X.", {
                {ADDRESS_MODE::IMPLIED, 0xaa, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // TAY - Transfer Accumulator to Index Y
        class OpFuncTAY : public OpFunc
        {
        public:
            OpFuncTAY() : OpFunc("tay", "Transfer Accumulator to Index Y.", {
                {ADDRESS_MODE::IMPLIED, 0xa8, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // TSX - Transfer Stack Pointer to Index X
        class OpFuncTSX : public OpFunc
        {
        public:
            OpFuncTSX() : OpFunc("tsx", "Transfer Stack Pointer to Index X.", {
                {ADDRESS_MODE::IMPLIED, 0xba, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // TXA - Transfer Index X to Accumulator
        class OpFuncTXA : public OpFunc
        {
        public:
            OpFuncTXA() : OpFunc("txa", "Transfer Index X to Accumulator.", {
                {ADDRESS_MODE::IMPLIED, 0x8a, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // TXS - Transfer Index X to Stack Pointer
        class OpFuncTXS : public OpFunc
        {
        public:
            OpFuncTXS() : OpFunc("txs", "Transfer Index X to Stack Pointer.", {
                {ADDRESS_MODE::IMPLIED, 0x9a, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };

        // TYA - Transfer Index Y to Accumulator
        class OpFuncTYA : public OpFunc
        {
        public:
            OpFuncTYA() : OpFunc("tya", "Transfer Index Y to Accumulator.", {
                {ADDRESS_MODE::IMPLIED, 0x98, 2}
                }) {};
                void execute(CPU& cpu, ADDRESS_MODE mode);
        };
    }
}


#endif