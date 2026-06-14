#include "opcodes6502.h"

// debug
#include <iostream>

#include "cpu6502.h"

using namespace Arch6502::OpCodes;
typedef Arch6502::ADDRESS_MODE ADDRESS_MODE;

// Statics
std::vector<OpFunc*> Arch6502::OpCodes::LUT::codes = std::vector<OpFunc*>();
std::vector<ADDRESS_MODE> Arch6502::OpCodes::LUT::modes = std::vector<ADDRESS_MODE>();

bool LUT::registerOpCode(OpFunc* opcode)
{
    if (opcode == nullptr)
    {
        return false;
    }

    std::map<ADDRESS_MODE, uint8_t> m_ModeMap = opcode->getModeMap();
    for (auto& code : m_ModeMap)
    {
        if (codes[code.second] == nullptr)
        {
            codes[code.second] = opcode;
            modes[code.second] = code.first;
        }
        else
        {
            std::cerr << "Error registering opcode " << opcode->getMnemonic() << ", conflicting code 0x";
            std::cerr << std::hex << code.second << " already registered." << std::endl;
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

    // LDX
    OpFunc* ldx = new OpFuncLDX();
    registerOpCode(ldx);

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

OpFunc::OpFunc(std::string mnemonic, std::string description, std::map<ADDRESS_MODE, uint8_t> modemap)
{
    m_Mnemonic = mnemonic;
    m_Description = description;
    m_ModeMap = modemap;
}

std::string OpFunc::getMnemonic() const
{
    return m_Mnemonic;
}

std::string OpFunc::getDescription() const
{
    return m_Description;
}

std::map<ADDRESS_MODE, uint8_t> OpFunc::getModeMap() const
{
    return m_ModeMap;
}

////////////////////////////////////////////////////////////////////////////////

void OpFuncLDX::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setX(cpu.getOperand(mode));
}
