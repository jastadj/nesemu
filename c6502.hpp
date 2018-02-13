#ifndef CLASS_C6502
#define CLASS_C6502

#include <cstdlib>
#include <iostream>
#include <vector>

    // b7 = S - Sign flag, 1 = negative
    // b6 = V - overflow flag
    // b5 = not used, should always be logical 1
    // b4 = B - software interrupt is executed (BRK)
    // b3 = D - decimal mode (when add/sub with carry, source values treated as BCD
    // b2 = I - interrupt enable/disable, if set, interrupts are disabled
    // b1 = Z - Zero flag, set when arithmetic or logical op produces 0
    // b0 = C - carry flag
enum STAT_FLAG{FLAG_CARRY, FLAG_ZERO, FLAG_INTERRUPT_DISABLE, FLAG_DECIMAL_MODE, FLAG_SOFTWARE_INTERRUPT,
               FLAG_NOT_USED, FLAG_OVERFLOW, FLAG_SIGN};

enum ADDRESS_MODE{IMMEDIATE, ZERO_PAGE, ZERO_PAGE_X, ZERO_PAGE_Y, ABSOLUTE, ABSOLUTE_X, ABSOLUTE_Y, INDIRECT_X,
                  INDIRECT_Y, INDIRECT, ACCUMULATOR, RELATIVE, IMPLIED};

// op codes implemented in c6502_ops.cpp
// debug console implemented in c6502_debug.cpp

class C6502
{
private:

    // memory
    uint8_t *m_Mem;
    unsigned int m_MemSize;

    // memory map
    // 0x0000 - 0x00ff : zero page
    // 0x0100 - 0x01ff : stack
    // 0x0200 - 0x07ff : RAM
    // 0x0800 - 0x1fff : Mirrors 0x0000 - 0x07ff
    // 0x2000 - 0x2007 : I/O registers
    // 0x2008 - 0x3fff : Mirrors 0x2000 - 0x2007
    // 0x4000 - 0x401f : I/O registers
    // 0x4020 - 0x5fff : Expansion ROM
    // 0x6000 - 0x7fff : SRAM
    // 0x8000 - 0xbfff : PRG-ROM Lower
    // 0xc000 - 0xffff : PRG-ROM Upper

    // registers
    uint8_t m_RegA; // accumulator
    uint8_t m_RegX;
    uint8_t m_RegY;

    uint8_t m_RegSP; // stack pointer (first empty place on the stack)
    uint16_t m_RegPC; // program counter, position of current instruction

    std::vector<uint8_t> m_Stack;
    void pushStack(uint8_t val);
    uint8_t popStack();

    uint8_t m_ImmediateTemp; // temporary storage for immediate addressing mode

    // status register
    uint8_t m_RegStat;
    bool getFlag(STAT_FLAG flag);
    void setFlag(STAT_FLAG flag, bool on);
    // b7 = N - Sign flag, 1 = negative
    // b6 = V - overflow flag
    // b5 = not used, should always be logical 1
    // b4 = B - software interrupt is executed (BRK)
    // b3 = D - decimal mode (when add/sub with carry, source values treated as BCD
    // b2 = I - interrupt enable/disable, if set, interrupts are disabled
    // b1 = Z - Zero flag, set when arithmetic or logical op produces 0
    // b0 = C - carry flag

    //
    bool init();

    bool execute(uint8_t opcode);

    // address mode
    uint8_t *getAddress(ADDRESS_MODE amode);

    // counter for an operation's cycle burn time
    unsigned int m_Cycles;

    // operations
    void ADC(ADDRESS_MODE amode); // add accumulator + operand + carry -> accumulator
    void AND(ADDRESS_MODE amode); // and memory with accumulator -> accumulator
    void ASL(ADDRESS_MODE amode); // shift accumulator or memory left <<
    void BCC(ADDRESS_MODE amode); // branch on carry clear, branch if carry flag == 0
    void BCS(ADDRESS_MODE amode); // branch on carry set, branch if carry flag == 1
    void BEQ(ADDRESS_MODE amode); // branch on result zero, branch if zero flag == 1
    void BIT(ADDRESS_MODE amode); // bit - test bits in memory with accumulator
    void BMI(ADDRESS_MODE amode); // branch on result minus, n == 1
    void BNE(ADDRESS_MODE amode); // branch on results not zero, z == 0
    void BPL(ADDRESS_MODE amode); // branch on result plus, n == 0
    void BRK(ADDRESS_MODE amode); // break, force break
    void BVC(ADDRESS_MODE amode); // branch on overflow clear, v == 0
    void BVS(ADDRESS_MODE amode); // branch on overflow set, v == 1
    void CLC(ADDRESS_MODE amode); // clear carry flag, c = 0
    void CLD(ADDRESS_MODE amode); // clear decimal mode, d = 0
    void CLI(ADDRESS_MODE amode); // clear interrupt disable flag, i = 0
    void CLV(ADDRESS_MODE amode); // clear overflow flag, v = 0
    void CMP(ADDRESS_MODE amode); // compare memory and accumulator, a - m
    void CPX(ADDRESS_MODE amode); // compare memory and  x, x - m
    void CPY(ADDRESS_MODE amode); // compare memory and y, y - m
    void DEC(ADDRESS_MODE amode); // decrement memory by 1, m--
    void DEX(ADDRESS_MODE amode); // decrement register x by 1
    void DEY(ADDRESS_MODE amode); // decrement register y by 1
    void EOR(ADDRESS_MODE amode); // exclusive or mem with accumulator, a ^ m -> a
    void INC(ADDRESS_MODE amode); // increment memory by 1, m++
    void INX(ADDRESS_MODE amode); // increment register x by 1, regx++
    void INY(ADDRESS_MODE amode); // increment register y by 1, regy++
    void JMP(ADDRESS_MODE amode); // jump to new location
    void JSR(ADDRESS_MODE amode); // jump and save return address to stack
    void LDA(ADDRESS_MODE amode); // load accumulator with memory, m -> a
    void LDX(ADDRESS_MODE amode); // load register x with memory, m -> reg x
    void LDY(ADDRESS_MODE amode); // load register y with memory, m -> reg y
    void LSR(ADDRESS_MODE amode); // shift right one bit, m | a >> 1
    void NOP(ADDRESS_MODE amode); // no operation (2 cycles)
    void ORA(ADDRESS_MODE amode); // or memory with accumulator a|m -> a
    void PHA(ADDRESS_MODE amode); // push accumulator on stack
    void PHP(ADDRESS_MODE amode); // push status register on stack
    void PLA(ADDRESS_MODE amode); // pull accumulator from stack
    void PLP(ADDRESS_MODE amode); // pull status register from stack
    void ROL(ADDRESS_MODE amode); // rotate one bit left (memory or accumulator)
    void ROR(ADDRESS_MODE amode); // rotate one bit right (memory or accumulator)
    void RTI(ADDRESS_MODE amode); // return from interrupt
    void RTS(ADDRESS_MODE amode); // return from subroutine
    void SBC(ADDRESS_MODE amode); // subtract memory from accumulator with borrow
    void SEC(ADDRESS_MODE amode); // set carry flag
    void SED(ADDRESS_MODE amode); // set decimal mode
    void SEI(ADDRESS_MODE amode); // set interrupt disable status
    void STA(ADDRESS_MODE amode); // store accumulator in memory
    void STX(ADDRESS_MODE amode); // store register x in memory
    void STY(ADDRESS_MODE amode); // store register y in memory
    void TAX(ADDRESS_MODE amode); // transfer accumulator to reg x
    void TAY(ADDRESS_MODE amode); // transfer accumulator to reg y
    void TSX(ADDRESS_MODE amode); // transfer stack pointer to reg x
    void TXA(ADDRESS_MODE amode); // transfer reg x to accumulator
    void TXS(ADDRESS_MODE amode); // transfer reg x to stack pointer
    void TYA(ADDRESS_MODE amode); // transfer reg y to accumulator

    void printError(std::string errormsg);

public:
    C6502(uint8_t *memory, unsigned int memory_size);

    void debugConsole();


};
#endif // CLASS_C6502
