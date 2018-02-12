#ifndef CLASS_C6502
#define CLASS_C6502

#include <cstdlib>
#include <iostream>

    // b7 = S - Sign flag, 1 = negative
    // b6 = V - overflow flag
    // b5 = not used, should always be logical 1
    // b4 = B - software interrupt is executed (BRK)
    // b3 = D - decimal mode (when add/sub with carry, source values treated as BCD
    // b2 = I - interrupt enable/disable, if set, interrupts are disabled
    // b1 = Z - Zero flag, set when arithmetic or logical op produces 0
    // b0 = C - carry flag
enum STAT_FLAG{FLAG_CARRY, FLAG_ZERO, FLAG_INTERRUPT_ENABLE, FLAG_DECIMAL_MODE, FLAG_SOFTWARE_INTERRUPT,
               FLAG_NOT_USED, FLAG_OVERFLOW, FLAG_SIGN};

enum ADDRESS_MODE{IMMEDIATE, ZERO_PAGE, ZERO_PAGE_X, ZERO_PAGE_Y, ABSOLUTE, ABSOLUTE_X, ABSOLUTE_Y, INDIRECT_X,
                  INDIRECT_Y, ACCUMULATOR, RELATIVE};

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

    uint8_t m_ImmediateTemp; // temporary storage for immediate addressing mode

    // status register
    uint8_t m_RegStat;
    bool getFlag(STAT_FLAG flag);
    void setFlag(STAT_FLAG flag, bool on);
    // b7 = S - Sign flag, 1 = negative
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
    // ....
    void LDA(ADDRESS_MODE amode); // load accumulator with memory, m -> a
    void LDX(ADDRESS_MODE amode); // load register x with memory, m -> reg x
    void LDY(ADDRESS_MODE amode); // load register y with memory, m -> reg y

    void printError(std::string errormsg);

public:
    C6502(uint8_t *memory, unsigned int memory_size);

    void debugConsole();


};
#endif // CLASS_C6502
