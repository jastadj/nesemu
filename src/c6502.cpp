#include "c6502.hpp"

#include <sstream>
#include <vector>
#include <iomanip>
#include <fstream>

C6502::C6502(uint8_t **memory, unsigned int memory_size)
{
    m_MemSize = memory_size;
    m_Mem = memory;

    reset();
}

C6502::~C6502()
{

}

bool C6502::reset()
{
    // clear registers
    m_RegA = 0x0;
    m_RegX = 0x0;
    m_RegY = 0x0;

    // reset cycles, program counter and clear stack pointer,
    m_Cycles = 0;
    m_RegPC = 0x0;

    // clear stack
    m_RegSP = 0xff;

    // clear the status register
    m_RegStat = 0x0 | (0x1 << FLAG_NOT_USED); // bit 5 (not used) is always high

    // clear temporary variable that stores immediate values
    m_ImmediateTemp = 0x0;

    return true;
}

void C6502::printError(std::string errormsg)
{
    std::cout << errormsg << std::endl;
}

void C6502::pushStack(uint8_t val)
{
    *m_Mem[STACK_END + m_RegSP] = val;
    m_RegSP--;
}

uint8_t C6502::popStack()
{
    m_RegSP++;
    return *m_Mem[STACK_END + m_RegSP];
}

bool C6502::executeNextInstruction()
{
    return execute( *m_Mem[m_RegPC] );
}

bool C6502::execute(uint8_t opcode)
{
    switch(opcode)
    {

    // ADC - Add memory to accumulator with carry
    case 0x69:
        ADC(IMMEDIATE);
        break;
    case 0x65:
        ADC(ZERO_PAGE);
        break;
    case 0x75:
        ADC(ZERO_PAGE_X);
        break;
    case 0x6D:
        ADC(ABSOLUTE);
        break;
    case 0x7D:
        ADC(ABSOLUTE_X);
        break;
    case 0x79:
        ADC(ABSOLUTE_Y);
        break;
    case 0x61:
        ADC(INDIRECT_X);
        break;
    case 0x71:
        ADC(INDIRECT_Y);
        break;

    //  AND - And memory with accumulator, store in accumulator
    case 0x29:
        AND(IMMEDIATE);
        break;
    case 0x25:
        AND(ZERO_PAGE);
        break;
    case 0x35:
        AND(ZERO_PAGE_X);
        break;
    case 0x2d:
        AND(ABSOLUTE);
        break;
    case 0x3d:
        AND(ABSOLUTE_X);
        break;
    case 0x39:
        AND(ABSOLUTE_Y);
        break;
    case 0x21:
        AND(INDIRECT_X);
        break;
    case 0x31:
        AND(INDIRECT_Y);
        break;

    //  ASL  - Shift left one bit (memory or accumulator)
    case 0x0a:
        ASL(ACCUMULATOR);
        break;
    case 0x06:
        ASL(ZERO_PAGE);
        break;
    case 0x16:
        ASL(ZERO_PAGE_X);
        break;
    case 0x0e:
        ASL(ABSOLUTE);
        break;
    case 0x1e:
        ASL(ABSOLUTE_X);
        break;

    // BCC - branch on carry clear (c==0)
    case 0x90:
        BCC(RELATIVE);
        break;

    // BEQ - branch on result 0
    case 0xf0:
        BEQ(RELATIVE);
        break;

    // BCS - branch on carry set (c == 1)
    case 0xb0:
        BCS(RELATIVE);
        break;

    // BIT - test bits in memory with accumulator
    case 0x24:
        BIT(ZERO_PAGE);
        break;
    case 0x2c:
        BIT(ABSOLUTE);
        break;

    // BMI - branch on result minus, n == 1
    case 0x30:
        BMI(RELATIVE);
        break;

    // BNE - branch on result not zero, Z == 0
    case 0xd0:
        BNE(RELATIVE);
        break;

    // BPL - branch on result plus, n == 0
    case 0x10:
        BPL(RELATIVE);
        break;

    // BRK - force break
    case 0x00:
        BRK(IMPLIED);
        break;

    // BVC - branch on overflow clear, v == 0
    case 0x50:
        BVC(RELATIVE);
        break;

    // BVS - branch on overflow set, v == 1
    case 0x70:
        BVS(RELATIVE);
        break;

    // CLC - clear carry flag, c = 0
    case 0x18:
        CLC(IMPLIED);
        break;

    // CLD - clear decimal mode, d = 0
    case 0xd8:
        CLD(IMPLIED);
        break;

    // CLI - clear interrupt disable flag, i = 0
    case 0x58:
        CLI(IMPLIED);
        break;

    // CLV - clear overflow flag, v = 0
    case 0xb8:
        CLV(IMPLIED);
        break;

    // CMP - compare memory and accumulator, a - m
    case 0xc9:
        CMP(IMMEDIATE);
        break;
    case 0xc5:
        CMP(ZERO_PAGE);
        break;
    case 0xd5:
        CMP(ZERO_PAGE_X);
        break;
    case 0xcd:
        CMP(ABSOLUTE);
        break;
    case 0xdd:
        CMP(ABSOLUTE_X);
        break;
    case 0xd9:
        CMP(ABSOLUTE_Y);
        break;
    case 0xc1:
        CMP(INDIRECT_X);
        break;
    case 0xd1:
        CMP(INDIRECT_Y);
        break;

    // CPX - compare memory and register x
    // reg x - m
    case 0xe0:
        CPX(IMMEDIATE);
        break;
    case 0xe4:
        CPX(ZERO_PAGE);
        break;
    case 0xec:
        CPX(ABSOLUTE);
        break;

    // CPY - compare memory and register y
    // reg y - m
    case 0xc0:
        CPY(IMMEDIATE);
        break;
    case 0xc4:
        CPY(ZERO_PAGE);
        break;
    case 0xcc:
        CPY(ABSOLUTE);
        break;

    // DEC - decrement memory by 1
    // m--
    case 0xc6:
        DEC(ZERO_PAGE);
        break;
    case 0xd6:
        DEC(ZERO_PAGE_X);
        break;
    case 0xce:
        DEC(ABSOLUTE);
        break;
    case 0xde:
        DEC(ABSOLUTE_X);
        break;

    // DEX - decrement register x by 1
    case 0xca:
        DEX(IMPLIED);
        break;

    // DEY - decrement register y by 1
    case 0x88:
        DEY(IMPLIED);
        break;

    // EOR - exclusive or memory with accumulator
    // a ^ m -> a
    case 0x49:
        EOR(IMMEDIATE);
        break;
    case 0x45:
        EOR(ZERO_PAGE);
        break;
    case 0x55:
        EOR(ZERO_PAGE_X);
        break;
    case 0x40:
        EOR(ABSOLUTE);
        break;
    case 0x5D:
        EOR(ABSOLUTE_X);
        break;
    case 0x59:
        EOR(ABSOLUTE_Y);
        break;
    case 0x41:
        EOR(INDIRECT_X);
        break;
    case 0x51:
        EOR(INDIRECT_Y);
        break;

    // INC - increment memory by 1
    // m++
    case 0xe6:
        INC(ZERO_PAGE);
        break;
    case 0xf6:
        INC(ZERO_PAGE_X);
        break;
    case 0xee:
        INC(ABSOLUTE);
        break;
    case 0xfe:
        INC(ABSOLUTE_X);
        break;

    // INX - increment register x by 1
    // reg x ++
    case 0xe8:
        INX(IMPLIED);
        break;

    // INY - increment register y by 1
    case 0xc8:
        INY(IMPLIED);
        break;

    // JMP - jump to new location
    // PC + 1 = PCL, PC + 2 = PCH
    case 0x4c:
        JMP(ABSOLUTE);
        break;
    case 0x6c:
        JMP(INDIRECT);
        break;

    // JSR - jump and save return address on stack
    case 0x20:
        JSR(ABSOLUTE);
        break;

    // LDA - Load accumulator with memory
    case 0xa9:
        LDA(IMMEDIATE);
        break;
    case 0xa5:
        LDA(ZERO_PAGE);
        break;
    case 0xb5:
        LDA(ZERO_PAGE_X);
        break;
    case 0xad:
        LDA(ABSOLUTE);
        break;
    case 0xbd:
        LDA(ABSOLUTE_X);
        break;
    case 0xb9:
        LDA(ABSOLUTE_Y);
        break;
    case 0xa1:
        LDA(INDIRECT_X);
        break;
    case 0xb1:
        LDA(INDIRECT_Y);
        break;

    // LDX - Load register x with memory
    case 0xa2:
        LDX(IMMEDIATE);
        break;
    case 0xa6:
        LDX(ZERO_PAGE);
        break;
    case 0xb6:
        LDX(ZERO_PAGE_Y);
        break;
    case 0xae:
        LDX(ABSOLUTE);
        break;
    case 0xbe:
        LDX(ABSOLUTE_Y);
        break;

    // LDY - Load register y with memory
    case 0xa0:
        LDY(IMMEDIATE);
        break;
    case 0xa4:
        LDY(ZERO_PAGE);
        break;
    case 0xb4:
        LDY(ZERO_PAGE_X);
        break;
    case 0xac:
        LDY(ABSOLUTE);
        break;
    case 0xbc:
        LDY(ABSOLUTE_X);
        break;

    // LSR - Shift right one bit
    // memory or accumulator >> 1
    case 0x4a:
        LSR(ACCUMULATOR);
        break;
    case 0x46:
        LSR(ZERO_PAGE);
        break;
    case 0x56:
        LSR(ZERO_PAGE_X);
        break;
    case 0x4e:
        LSR(ABSOLUTE);
        break;
    case 0x5e:
        LSR(ABSOLUTE_X);
        break;

    // NOP - no operation
    // 2 cycles
    case 0xea:
        NOP(IMPLIED);
        break;

    // ORA - or memory with accumulator
    // a | m -> a
    case 0x09:
        ORA(IMMEDIATE);
        break;
    case 0x05:
        ORA(ZERO_PAGE);
        break;
    case 0x15:
        ORA(ZERO_PAGE_X);
        break;
    case 0x0d:
        ORA(ABSOLUTE);
        break;
    case 0x1d:
        ORA(ABSOLUTE_X);
        break;
    case 0x19:
        ORA(ABSOLUTE_Y);
        break;
    case 0x01:
        ORA(INDIRECT_X);
        break;
    case 0x11:
        ORA(INDIRECT_Y);
        break;

    // PHA - push accumulator on stack
    case 0x48:
        PHA(IMPLIED);
        break;

    // PHP - push status register on stack
    case 0x08:
        PHP(IMPLIED);
        break;

    // PLA - pull accumulator from stack
    case 0x68:
        PLA(IMPLIED);
        break;

    // PLP - pull status register from stack
    case 0x28:
        PLP(IMPLIED);
        break;

    // ROL - rotate one bit left
    // memory or accumulator
    case 0x2a:
        ROL(ACCUMULATOR);
        break;
    case 0x26:
        ROL(ZERO_PAGE);
        break;
    case 0x36:
        ROL(ZERO_PAGE_X);
        break;
    case 0x2e:
        ROL(ABSOLUTE);
        break;
    case 0x3e:
        ROL(ABSOLUTE_X);
        break;

    // ROR - rotate one bit right
    // memory or accumulator
    case 0x6a:
        ROR(ACCUMULATOR);
        break;
    case 0x66:
        ROR(ZERO_PAGE);
        break;
    case 0x76:
        ROR(ZERO_PAGE_X);
        break;
    case 0x6e:
        ROR(ABSOLUTE);
        break;
    case 0x7e:
        ROR(ABSOLUTE_X);
        break;

    // RTI - return from interrupt
    // status from stack, PC from stack
    case 0x4d:
        RTI(IMPLIED);
        break;

    // RTS - return from subroutine
    // get pc from stack
    case 0x60:
        RTS(IMPLIED);
        break;

    // SBC - subtract memory from accumulator with borrow
    // a - m - c -> a
    case 0xe9:
        SBC(IMMEDIATE);
        break;
    case 0xe5:
        SBC(ZERO_PAGE);
        break;
    case 0xf5:
        SBC(ZERO_PAGE_X);
        break;
    case 0xed:
        SBC(ABSOLUTE);
        break;
    case 0xfd:
        SBC(ABSOLUTE_X);
        break;
    case 0xf9:
        SBC(ABSOLUTE_Y);
        break;
    case 0xe1:
        SBC(INDIRECT_X);
        break;
    case 0xf1:
        SBC(INDIRECT_Y);
        break;

    // SEC - set carry flag
    case 0x38:
        SEC(IMPLIED);
        break;

    // SED - set decimal mode
    case 0xf8:
        SED(IMPLIED);
        break;

    // SEI - set interrupt disable status
    case 0x78:
        SEI(IMPLIED);
        break;

    // STA - store accumulator in memory
    // m = accumulator
    case 0x85:
        STA(ZERO_PAGE);
        break;
    case 0x95:
        STA(ZERO_PAGE_X);
        break;
    case 0x8D:
        STA(ABSOLUTE);
        break;
    case 0x9D:
        STA(ABSOLUTE_X);
        break;
    case 0x99:
        STA(ABSOLUTE_Y);
        break;
    case 0x81:
        STA(INDIRECT_X);
        break;
    case 0x91:
        STA(INDIRECT_Y);
        break;

    // STX - store register x in memory
    // m = reg x
    case 0x86:
        STX(ZERO_PAGE);
        break;
    case 0x96:
        STX(ZERO_PAGE_Y);
        break;
    case 0x8e:
        STX(ABSOLUTE);
        break;

    // STY - store register y in memory
    // m = reg y
    case 0x84:
        STY(ZERO_PAGE);
        break;
    case 0x94:
        STY(ZERO_PAGE_X);
        break;
    case 0x8c:
        STY(ABSOLUTE);
        break;

    // TAX - transfer accumulator to reg x
    // reg x = a
    case 0xaa:
        TAX(IMPLIED);
        break;

    // TAY - transfer accumulator to reg y
    // reg y = a
    case 0xa8:
        TAY(IMPLIED);
        break;

    // TSX - Transfer stack pointer to reg x
    // reg x = *sp
    case 0xba:
        TSX(IMPLIED);
        break;

    // TXA - transfer reg x to accumulator
    case 0x8a:
        TXA(IMPLIED);
        break;

    // TXS - transfer reg x to stack pointer
    // reg x = *SP
    case 0x9a:
        TSX(IMPLIED);
        break;

    // TYA - transfer reg y to accumulator
    // a = reg y
    case 0x98:
        TYA(IMPLIED);
        break;

    default:
        return false;
        break;
    }
    return true;
}

// get status flag bit
bool C6502::getFlag(STAT_FLAG flag)
{
    return (m_RegStat >> flag) & 0x1;
}

// set status flag bit on/off
void C6502::setFlag(STAT_FLAG flag, bool on)
{
    if( flag == FLAG_NOT_USED) return;

    if(getFlag(flag) == on) return;

    if(on) m_RegStat |= (0x1 << flag);
    else m_RegStat ^= (0x1 << flag);
}

// return operand based on address mode
// IMMEDIATE, ZERO_PAGE, ZERO_PAGE_X, ABSOLUTE, ABSOLUTE_X, ABSOLUTE_Y, INDIRECT_X, INDIRECT_Y
uint8_t *C6502::getAddress(ADDRESS_MODE amode)
{
    switch(amode)
    {
    // accumulator address
    case ACCUMULATOR:
        return &m_RegA;
        break;
    // immediate gets next mem byte
    case IMMEDIATE:
        m_ImmediateTemp = *m_Mem[m_RegPC + 1];
        return &m_ImmediateTemp;
        break;
    // zero page gets address of 0x00YY where YY is next mem byte
    case ZERO_PAGE:
        return m_Mem[ *m_Mem[m_RegPC + 1] ];
        break;
    // zero page x gets address of 0x00YY where YY is REGX + next mem byte
    case ZERO_PAGE_X:
        return m_Mem[ m_RegX + *m_Mem[m_RegPC + 1] ];
        break;
    // zero page y gets address ox 0x00YY where YY is REGY + next mem byte
    case ZERO_PAGE_Y:
        return m_Mem[ m_RegY + *m_Mem[m_RegPC + 1] ];
        break;
    // ABSOLUTE gets address from next two bytes (LSB first)
    case ABSOLUTE:
        return m_Mem[ ( (*m_Mem[m_RegPC + 2]) << 8) + *m_Mem[m_RegPC+1] ];
        break;
    // ABSOLUTE X gets address from next two bytes + REGX
    case ABSOLUTE_X:
        return m_Mem[ ( (*m_Mem[m_RegPC + 2]) << 8) + *m_Mem[m_RegPC+1] + m_RegX];
        break;
    // ABSOLUTE X gets address from next two bytes + REGY
    case ABSOLUTE_Y:
            return m_Mem[ ( (*m_Mem[m_RegPC + 2]) << 8) + *m_Mem[m_RegPC+1] + m_RegY];
        break;
    case INDIRECT_X:
        {
            uint16_t addr = m_RegX + *m_Mem[m_RegPC + 1];
            if(addr > 0xff) addr -= 0xff; // rollover zero-page index
            return m_Mem[  (*m_Mem[addr+1] << 8) + *m_Mem[addr]    ];
        }
        break;
    case INDIRECT_Y:
        {
            uint16_t lobyte = *m_Mem[m_RegPC + 1];
            return m_Mem[  (*m_Mem[lobyte+1] << 8) + *m_Mem[lobyte] + m_RegY ];
        }
        break;
    // only used for JUMP
    case INDIRECT:
        {
            uint16_t lobyte = *m_Mem[m_RegPC + 1] + (*m_Mem[m_RegPC + 2] << 8);
            return m_Mem[ *m_Mem[lobyte] + (*m_Mem[lobyte+1] << 8)  ];
        }
        break;
    case RELATIVE:
        {
            int8_t offset = *m_Mem[ m_RegPC + 1];
            int32_t pc = m_RegPC + offset;
            return m_Mem[ uint16_t(pc)];
        }
        break;
    case IMPLIED:
        return NULL;
        break;
    default:
        std::cout << "Error, access mode " << amode << " is undefined.  Returning NULL." << std::endl;
        return NULL;
        break;
    }

    return NULL;
}

void C6502::show()
{
    std::cout << "C6502" << std::endl;
    std::cout << "-----" << std::endl;
    std::cout << "Cycles           = " << std::dec << m_Cycles << std::endl;
    std::cout << "Accumulator      = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegA) << std::endl;
    std::cout << "Register X       = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegX) << std::endl;
    std::cout << "Register Y       = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegY) << std::endl;
    std::cout << "Stack Pointer    = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegSP) << std::endl;
    if(m_RegSP < 0xff)
        for(int i = int(m_RegSP+1); i <= 0xff; i++)
            std::cout << "     " << std::hex << std::setfill('0') << std::setw(2) << int(STACK_END + i) << std::endl;
    std::cout << "Program Counter  = 0x" << std::hex << std::setfill('0') << std::setw(2) << int(m_RegPC) << std::endl;
    std::cout << "Instruction at PC= 0x" << std::hex << std::setfill('0') << std::setw(2) << int(*m_Mem[m_RegPC]) << std::endl;
    std::cout << "Flags:" << std::endl;
    std::cout << "  Carry            = " << getFlag(FLAG_CARRY) << std::endl;
    std::cout << "  Zero             = " << getFlag(FLAG_ZERO) << std::endl;
    std::cout << "  Interrupt Enable = " << getFlag(FLAG_INTERRUPT_DISABLE) << std::endl;
    std::cout << "  Decimal Mode     = " << getFlag(FLAG_DECIMAL_MODE) << std::endl;
    std::cout << "  SW Interrupt     = " << getFlag(FLAG_SOFTWARE_INTERRUPT) << std::endl;
    std::cout << "  NOT USED         = " << getFlag(FLAG_NOT_USED) << std::endl;
    std::cout << "  Overflow         = " << getFlag(FLAG_OVERFLOW) << std::endl;
    std::cout << "  Sign             = " << getFlag(FLAG_SIGN) << std::endl;
}
