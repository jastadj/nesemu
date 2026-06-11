#ifndef OPCODES6502_H
#define OPCODES6502_H

/*
*                 The following notation applies to this summary:


     A       Accumulator                  EOR     Logical Exclusive Or

     X, Y    Index Registers              fromS   Transfer from Stack

     M       Memory                       toS     Transfer to Stack

     P       Processor Status Register    ->      Transfer to

     S       Stack Pointer                <-      Transfer from

     /       Change                       V       Logical OR

     _       No Change                    PC      Program Counter

     +       Add                          PCH     Program Counter High

     /\      Logical AND                  PCL     Program Counter Low

     -       Subtract                     OPER    OPERAND

                                          #       IMMEDIATE ADDRESSING MODE
*/

enum OPCODES_6502
{
    // ADC - Add memory to Accumulator with Carry
    // A + M + C -> A, C
    ADC_IMMEDIATE =         0x69,
    ADC_ZERO_PAGE =         0x65,
    ADC_ZERO_PAGE_X =       0x75,
    ADC_ABSOLUTE =          0x60,
    ADC_ABSOLUTE_X =        0x70,
    ADC_ABSOLUTE_Y =        0x79,
    ADC_INDIRECT_X =        0x61,
    ADC_INDIRECT_Y =        0x71,

    // AND - AND memory with Accumulator
    // A /\ M -> A
    AND_IMMEDIATE =         0x29,
    AND_ZERO_PAGE =         0x25,
    AND_ZERO_PAGE_X =       0x35,
    AND_ABSOLUTE =          0x2D,
    AND_ABSOLUTE_X =        0x3D,
    AND_ABSOLUTE_Y =        0x39,
    AND_INDIRECT_X =        0x21,
    AND_INDIRECT_Y =        0x31,

    // ASL - Shift left one bit
    // C <- |7|6|5|4|3|2|1|0| <- 0
    ASL_ACCUMULATOR =       0x0a,
    ASL_ZERO_PAGE =         0x06,
    ASL_ZERO_PAGE_X =       0x16,
    ASL_ABSOLUTE =          0x0e,
    ASL_ABSOLUTE_X =        0x1e,

    // BCC - Branch on Carry Clear
    // Branch on C = 0
    BCC_RELATIVE =          0x90,

    // BCS - Branch on Carry Set
    // Branch on C = 1
    BCS_RELATIVE =          0xb0,

    // BEQ - Branch on Result Zero
    // Branch on Z = 1
    BEQ_RELATIVE =          0xf0,

    // BIT - Test bits in memory with Accumulator
    // A /\ M, M7 -> N, M6 -> V
    BIT_ZERO_PAGE =         0x24,
    BIT_ABSOLUTE =          0x2c,

    // BMI - Branch on Result Minus
    // Branch on N = 1
    BMI_RELATIVE =          0x30,

    // BNE - Branch on Result Not Zero
    // Branch on Z = 0
    BNE_RELATIVE =          0xd0,

    // BPL - Branch on Result Plus
    // Branch on N = 0
    BPL_RELATIVE =          0x10,

    // BRK - Force Break
    // Forced Interrupt PC + 2 toS P toS
    BRK_IMPLIED =           0x00,

    // BVC - Branch on Overflow Clear
    // Branch on V = 0
    BVC_RELATIVE =          0x50,


    // BVS - Branch on Overflow Set
    // Branch on V = 1
    BVS_RELATIVE =          0x70,

    // CLC - Clear Carry Flag
    // 0 -> C
    CLC_IMPLIED =           0x18,

    // CLD - Clear Decimal Mode
    // 0 -> D
    CLD_IMPLIED =           0xd8,

    // CLI - Clear Interrupt Disable Bit
    // 0 -> I
    CLI_IMPLIED =           0x58,

    // CLV - Clear Overflow Flag
    // 0 -> V
    CLV_IMPLIED =           0xb8,

    // CMP - Compare Memory and Accumulator
    // A - M
    CMP_IMMEDIATE =         0xc9,
    CMP_ZERO_PAGE =         0xc5,
    CMP_ZERO_PAGE_X =       0xd5,
    CMP_ABSOLUTE =          0xcd,
    CMP_ABSOLUTE_X =        0xdd,
    CMP_ABSOLUTE_Y =        0xd9,
    CMP_INDIRECT_X =        0xc1,
    CMP_INDIRECT_Y =        0xd1,

    // CPX - Compare Memory and Index X
    // X - M
    CPX_IMMEDIATE =         0xe0,
    CPX_ZERO_PAGE =         0xe4,
    CPX_ABSOLUTE =          0xec,

    // CPY - Compare Memory and Index Y
    CPY_IMMEDIATE =         0xc0,
    CPY_ZERO_PAGE =         0xc4,
    CPY_ABSOLUTE =          0xcc,

    // DEC - Decrement memory by one
    // M - 1 -> M
    DEC_ZERO_PAGE =         0xc6,
    DEC_ZERO_PAGE_X =       0xd6,
    DEC_ABSOLUTE =          0xce,
    DEC_ABSOLUTE_X =        0xde,

    // DEX - Decrement Index X by one
    // X - 1 -> X
    DEX_IMPLIED =           0xca,

    // DEY - Decrement Index Y by one
    // Y - 1 -> Y
    DEY_IMPLIED =           0x88,

    // EOR - Exclusive-Or memory with Accumulator
    // A EOR M -> A
    EOR_IMMEDIATE =         0x49,
    EOR_ZERO_PAGE =         0x45,
    EOR_ZERO_PAGE_X =       0x55,
    EOR_ABSOLUTE =          0x40,
    EOR_ABSOLUTE_X =        0x50,
    EOR_ABSOLUTE_Y =        0x59,
    EOR_INDIRECT_X =        0x41,
    EOR_INDIRECT_Y =        0x51,

    // INC - Increment memory by one
    // M + 1 -> M
    INC_ZERO_PAGE =         0xe6,
    INC_ZERO_PAGE_X =       0xf6,
    INC_ABSOLUTE =          0xee,
    INC_ABSOLUTE_X =        0xfe,

    // INX - Increment Index X by one
    // X + 1 -> X
    INX_IMPLIED =           0xe8,

    // INY - Increment Index Y by one
    // Y + 1 -> Y
    INY_IMPLIED =           0xc8,

    // JMP - Jump to new location
    // PC + 1 -> PCL
    // PC + 2 -> PCH
    JMP_ABSOLUTE =          0x4c,
    JMP_INDIRECT =          0x6c,

    // JSR - Jump to new location saving return address
    // PC + 2 toS, (PC + 1) -> PCL
    //             (PC + 2) -> PCH
    JSR_ABSOLUTE =          0x20,

    // LDA - Load Accumulator with memory
    // M->A
    LDA_IMMEDIATE =         0xa9,
    LDA_ZERO_PAGE =         0xa5,
    LDA_ZERO_PAGE_X =       0xb5,
    LDA_ABSOLUTE =          0xad,
    LDA_ABSOLUTE_X =        0xbd,
    LDA_ABSOLUTE_Y =        0xb9,
    LDA_INDIRECT_X =        0xa1,
    LDA_INDIRECT_Y =        0xb1,

    // LDX - Load Index X with memory
    // M->X
    LDX_IMMEDIATE =         0xa2,
    LDX_ZERO_PAGE =         0xa6,
    LDX_ZERO_PAGE_Y =       0xb6,
    LDX_ABSOLUTE =          0xae,
    LDX_ABSOLUTE_Y =        0xbe,

    // LDY - Load Index Y with memory
    // M->Y
    LDY_IMMEDIATE =         0xa0,
    LDY_ZERO_PAGE =         0xa4,
    LDY_ZERO_PAGE_X =       0xb4,
    LDY_ABSOLUTE =          0xac,
    LDY_ABSOLUTE_X =        0xbc,

    // LSR - Shift right one bit
    // 0 -> |7|6|5|4|3|2|1|0| -> C
    LSR_ACCUMULATOR =       0x4a,
    LSR_ZERO_PAGE =         0x46,
    LSR_ZERO_PAGE_X =       0x56,
    LSR_ABSOLUTE =          0x4e,
    LSR_ABSOLUTE_X =        0x5e,

    // NOP - No Operation
    // No Operation for 2 cycles
    NOP_IMPLIED =           0xea,

    // ORA - OR memory with Accumulator
    // A V M -> A
    ORA_IMMEDIATE =         0x09,
    ORA_ZERO_PAGE =         0x05,
    ORA_ZERO_PAGE_X =       0x15,
    ORA_ABSOLUTE =          0x0d,
    ORA_ABSOLUTE_X =        0x1d,
    ORA_ABSOLUTE_Y =        0x19,
    ORA_INDIRECT_X =        0x01,
    ORA_INDIRECT_Y =        0x11,

    // PHA - Push Accumulator on Stack
    // A toS
    PHA_IMPLIED =           0x48,

    // PHP - Push processor Status on Stack
    // P toS
    PHP_IMPLIED =           0x08,

    // PLA - Pull Accumulator from Stack
    // A fromS
    PLA_IMPLIED =           0x68,

    // PLP - Pull processor Status from Stack
    // P fromS
    PLP_IMPLIED =           0x28,

    // ROL - Rotate one bit left
    // +-< |7|6|5|4|3|2|1|0| <- |C| <-+
    ROL_ACCUMULATOR =       0x2a,
    ROL_ZERO_PAGE =         0x26,
    ROL_ZERO_PAGE_X =       0x36,
    ROL_ABSOLUTE =          0x2e,
    ROL_ABSOLUTE_X =        0x3e,

    // ROR - Rotate one bit right
    // +-> |C| -> |7|6|5|4|3|2|1|0| >-+
    // Note: ROR available on MCS650X after Jun 1976
    ROR_ACCUMULATOR =       0x6a,
    ROR_ZERO_PAGE =         0x66,
    ROR_ZERO_PAGE_X =       0x76,
    ROR_ABSOLUTE =          0x6e,
    ROR_ABSOLUTE_X =        0x7e,

    // RTI - Return from interrupt
    // P fromS PC fromS
    RTI_IMPLIED =           0x4d,

    // RTS - Return from subroutine
    // PC fromS, PC + 1 -> PC
    RTS_IMPLIED =           0x60,

    // SBC - Subtract memory from Accumulator with borrow
    // A - M - C -> A
    SBC_IMMEDIATE =         0xe9,
    SBC_ZERO_PAGE =         0xe5,
    SBC_ZERO_PAGE_X =       0xf5,
    SBC_ABSOLUTE =          0xed,
    SBC_ABSOLUTE_X =        0xfd,
    SBC_INDIRECT_X =        0xe1,
    SBC_INDIRECT_Y =        0xf1,

    // SEC - Set Carry flag
    // 1 -> C
    SEC_IMPLIED =           0x38,

    // SED - Set Decimal mode
    // 1 -> D
    SED_IMPLIED =           0xf8,

    // SEI - Set Interrupt Disable status
    // 1 -> I
    SEI_IMPLIED =           0x78,

    // STA - Store Accumulator in memory
    // A->M
    STA_ZERO_PAGE =         0x85,
    STA_ZERO_PAGE_X =       0x95,
    STA_ABSOLUTE =          0x80,
    STA_ABSOLUTE_X =        0x90,
    STA_ABSOLUTE_Y =        0x99,
    STA_INDIRECT_X =        0x81,
    STA_INDIRECT_Y =        0x91,

    // STX - Store Index X in memory
    // X->M
    STX_ZERO_PAGE =         0x86,
    STX_ZERO_PAGE_X =       0x96,
    STX_ABSOLUTE =          0x8e,

    // STY - Store Index Y in memory
    // Y->M
    STY_ZERO_PAGE =         0x84,
    STY_ZERO_PAGE_X =       0x94,
    STY_ABSOLUTE =          0x8c,

    // TAX - Transfer Accumulator to Index X
    // A -> X
    TAX_IMPLIED =           0xaa,

    // TAY - Transfer Accumulator to Index Y
    // A -> Y
    TAY_IMPLIED =           0xa8,

    // TSX - Transfer Stack Pointer to Index X
    // S -> X
    TSX_IMPLIED =           0xba,

    // TXA - Transfer Index X to Accumulator
    // X -> A
    TXA_IMPLIED =           0x8a,

    // TXS - Transfer Index X to Stack Pointer
    // X -> S
    TXS_IMPLIED =           0x9a,

    // TYA - Transfer Index Y to Accumulator
    // Y -> A
    TYA_IMPLIED =           0x98,



    TERMINATION
};


#endif