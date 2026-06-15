#include "opcodes6502.h"

// debug
#include <iostream>

#include "cpu6502.h"

using namespace Arch6502::OpCodes;
typedef Arch6502::ADDRESS_MODE ADDRESS_MODE;

// Statics
std::vector<OpFunc*> Arch6502::OpCodes::LUT::codes = std::vector<OpFunc*>();
std::vector<ADDRESS_MODE> Arch6502::OpCodes::LUT::modes = std::vector<ADDRESS_MODE>();

bool LUT::registerOpCode(OpFunc* opfunc)
{
    if (opfunc == nullptr)
    {
        return false;
    }

    std::vector<OpFunc::OpCodeInfo> opinfos = opfunc->getOpCodeInfos();
    for (auto& opinfo : opinfos)
    {
        if (codes[opinfo.code] == nullptr)
        {
            codes[opinfo.code] = opfunc;
            modes[opinfo.mode] = opinfo.mode;
        }
        else
        {
            std::cerr << "Error registering opcode " << opfunc->getMnemonic() << ", conflicting code 0x";
            std::cerr << std::hex << opinfo.code << " already registered." << std::endl;
            return false;
        }
    }
    return true;
}

bool Arch6502::OpCodes::LUT::init()
{
    if (!codes.empty() || !modes.empty())
    {
        return false;
    }
    
    //codeLUT = std::vector<OpFunc*>(code_count, nullptr);
    codes = std::vector<OpFunc*>(OPCODE_COUNT, nullptr);
    modes = std::vector<ADDRESS_MODE>(OPCODE_COUNT, ADDRESS_MODE::ADDR_ERROR);

    // Register OpFuncs
    registerOpCode(new OpFuncLDX());
    registerOpCode(new OpFuncLDY());





    return true;
}

OpFunc* Arch6502::OpCodes::LUT::getOpFuncByMnemonic(std::string mnemonic)
{
    init();
    for (int i = 0; i < 256; i++)
    {
        if (codes[i] && codes[i]->getMnemonic() == mnemonic)
        {
            return codes[i];
        }
    }
    return nullptr;
}

OpFunc::OpFunc(std::string mnemonic, std::string description, std::vector<OpCodeInfo> opcodeinfos)
{
    m_Mnemonic = mnemonic;
    m_Description = description;
    m_OpCodeInfos = opcodeinfos;
}

std::string OpFunc::getMnemonic() const
{
    return m_Mnemonic;
}

std::string OpFunc::getDescription() const
{
    return m_Description;
}

std::vector<OpFunc::OpCodeInfo> OpFunc::getOpCodeInfos() const
{
    return m_OpCodeInfos;
}

////////////////////////////////////////////////////////////////////////////////

void Arch6502::OpCodes::OpCodeADC::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t val = cpu.getAcc() + cpu.getOperand(mode) + cpu.getStatusBit(STATUS_BIT::STATUS_CARRY);
    cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, val > 0xff);
    cpu.setAcc(val & 0xff);
}

void Arch6502::OpCodes::OpCodeAND::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setAcc(cpu.getAcc() & cpu.getOperand(mode));
}

void Arch6502::OpCodes::OpFuncLDA::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setAcc(cpu.getOperand(mode));
}

void OpFuncLDX::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setX(cpu.getOperand(mode));
}

void Arch6502::OpCodes::OpFuncLDY::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setY(cpu.getOperand(mode));
}


