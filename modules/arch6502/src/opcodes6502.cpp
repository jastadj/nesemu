#include "opcodes6502.h"

// debug
#include <iostream>

#include "cpu6502.h"

using namespace Arch6502;

std::vector<OpCode*> OpCode::codes = std::vector<OpCode*>();

bool OpCode::initCodes()
{
    if (!codes.empty())
    {
        return false;
    }
    codes = std::vector<OpCode*>(256, nullptr);

    codes[0x00] = new OpCodeBRK();

    // LDX
    codes[0xa2] = new OpCodeLDXImmediate();
    codes[0xa6] = new OpCodeLDXZeroPage();
    codes[0xb6] = new OpCodeLDXZeroPageY();
    codes[0xae] = new OpCodeLDXAbsolute();
    codes[0xbe] = new OpCodeLDXAbsoluteY();

    // LDY
    codes[0xa0] = new OpCodeLDYImmediate();
    codes[0xa4] = new OpCodeLDYZeroPage();
    codes[0xb4] = new OpCodeLDYZeroPageX();
    codes[0xac] = new OpCodeLDYAbsolute();
    codes[0xbc] = new OpCodeLDYAbsoluteY();



    return true;
}

OpCode::OpCode(std::string mnemonic, uint8_t code, ADDRESS_MODE mode)
{
    this->mnemonic = mnemonic;
    this->code = code;
    this->mode = mode;
}

OpCode::OpCode(std::string mnemonic, std::map<ADDRESS_MODE, uint8_t> modes)
{
    this->mnemonic = mnemonic;
    this->modes = modes;
}

const std::string OpCode::getMnemonic() const
{
    return mnemonic;
}

const uint8_t OpCode::getCode() const
{
    return code;
}

const ADDRESS_MODE OpCode::getAddressMode() const
{
    return mode;
}

void Arch6502::OpCodeBRK::execute(CPU& cpu)
{
    cpu.setStatusBit(STATUS_BIT::S_INTERRUPT, true);
}

void Arch6502::OpCodeLDXImmediate::execute(CPU& cpu)
{
    cpu.setX(cpu.getOperand(mode));
}

void Arch6502::OpCodeLDXZeroPage::execute(CPU& cpu)
{
    cpu.setX(cpu.getOperand(mode));
}

void Arch6502::OpCodeLDXZeroPageY::execute(CPU& cpu)
{
    cpu.setX(cpu.getOperand(mode));
}

void Arch6502::OpCodeLDXAbsolute::execute(CPU& cpu)
{
    cpu.setX(cpu.getOperand(mode));
}

void Arch6502::OpCodeLDXAbsoluteY::execute(CPU& cpu)
{
    cpu.setX(cpu.getOperand(mode));
}

void Arch6502::OpCodeLDYImmediate::execute(CPU& cpu)
{
    cpu.setY(cpu.getOperand(mode));
}

void Arch6502::OpCodeLDYZeroPage::execute(CPU& cpu)
{
    cpu.setY(cpu.getOperand(mode));
}

void Arch6502::OpCodeLDYZeroPageX::execute(CPU& cpu)
{
    cpu.setY(cpu.getOperand(mode));
}

void Arch6502::OpCodeLDYAbsolute::execute(CPU& cpu)
{
    cpu.setY(cpu.getOperand(mode));
}

void Arch6502::OpCodeLDYAbsoluteY::execute(CPU& cpu)
{
    cpu.setY(cpu.getOperand(mode));
}
