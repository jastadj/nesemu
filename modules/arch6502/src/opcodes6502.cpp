#include "opcodes6502.h"

// debug
#include <iostream>
#include <iomanip>
#include <sstream>

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
            modes[opinfo.code] = opinfo.mode;
            std::stringstream oss;
            oss << "Registering opcode 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opinfo.code);
            oss << " - " << opfunc->getMnemonic() << " \"" << getAddressModeString(opinfo.mode) << "\"" << std::endl;
            std::cout << oss.str();
        }
        else
        {
            std::stringstream oss;
            oss << "Error registering opcode 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opinfo.code);
            oss << " - " << opfunc->getMnemonic() << ", conflicting code 0x";
            oss << std::setw(2) << std::setfill('0') << int(opinfo.code) << " already registered." << std::endl;
            std::cerr << oss.str();
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
    registerOpCode(new OpFuncADC);
    registerOpCode(new OpFuncAND);
    registerOpCode(new OpFuncASL);
    registerOpCode(new OpFuncBCC);
    registerOpCode(new OpFuncBCS);
    registerOpCode(new OpFuncBEQ);
    registerOpCode(new OpFuncBIT);
    registerOpCode(new OpFuncBMI);
    registerOpCode(new OpFuncBNE);
    registerOpCode(new OpFuncBPL);
    registerOpCode(new OpFuncBRK);
    registerOpCode(new OpFuncBVC);
    registerOpCode(new OpFuncBVS);
    registerOpCode(new OpFuncCLC);
    registerOpCode(new OpFuncCLD);
    registerOpCode(new OpFuncCLI);
    registerOpCode(new OpFuncCLV);
    registerOpCode(new OpFuncCMP);
    registerOpCode(new OpFuncCPX);
    registerOpCode(new OpFuncCPY);
    registerOpCode(new OpFuncDEC);
    registerOpCode(new OpFuncDEX);
    registerOpCode(new OpFuncDEY);
    registerOpCode(new OpFuncEOR);
    registerOpCode(new OpFuncINC);
    registerOpCode(new OpFuncINX);
    registerOpCode(new OpFuncINY);
    registerOpCode(new OpFuncJMP);
    registerOpCode(new OpFuncLDX);
    registerOpCode(new OpFuncLDY);
    registerOpCode(new OpFuncNOP);
    registerOpCode(new OpFuncORA);
    registerOpCode(new OpFuncPHA);
    registerOpCode(new OpFuncPHP);
    registerOpCode(new OpFuncPLA);
    registerOpCode(new OpFuncROL);
    registerOpCode(new OpFuncROR);
    registerOpCode(new OpFuncRTI);
    registerOpCode(new OpFuncRTS);
    registerOpCode(new OpFuncSBC);
    registerOpCode(new OpFuncSEC);
    registerOpCode(new OpFuncSED);
    registerOpCode(new OpFuncSEI);
    registerOpCode(new OpFuncSTA);
    registerOpCode(new OpFuncSTX);
    registerOpCode(new OpFuncSTY);
    registerOpCode(new OpFuncTAX);
    registerOpCode(new OpFuncTAY);
    registerOpCode(new OpFuncTSX);
    registerOpCode(new OpFuncTXA);
    registerOpCode(new OpFuncTXS);
    registerOpCode(new OpFuncTYA);

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

bool OpFunc::updateStatusBitFromResult(CPU& cpu, STATUS_BIT status_bit, uint16_t result)
{
    bool high;
    switch (status_bit)
    {
    case STATUS_BIT::STATUS_CARRY:
        high = result > 0xff;
        break;
    case STATUS_BIT::STATUS_NEGATIVE:
        high = result & 0x80;
        break;
    case STATUS_BIT::STATUS_ZERO:
        high = ((result & 0xff) == 0x00);
        break;
    case STATUS_BIT::STATUS_OVERFLOW:
        high = result > 0xff;
        break;
    default:
        return false;
        break;
    }
    cpu.setStatusBit(status_bit, high);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

void Arch6502::OpCodes::OpFuncADC::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getAcc() + cpu.getOperandValue(mode) + cpu.getStatusBit(STATUS_BIT::STATUS_CARRY);
    updateStatusBitFromResult(cpu, STATUS_BIT::STATUS_NEGATIVE, result);
    updateStatusBitFromResult(cpu, STATUS_BIT::STATUS_CARRY, result);
    updateStatusBitFromResult(cpu, STATUS_BIT::STATUS_ZERO, result);
    updateStatusBitFromResult(cpu, STATUS_BIT::STATUS_OVERFLOW, result);
    cpu.setAcc(result & 0xff);
}

void Arch6502::OpCodes::OpFuncAND::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getAcc() & cpu.getOperandValue(mode);
    updateStatusBitFromResult(cpu, STATUS_BIT::STATUS_NEGATIVE, result);
    updateStatusBitFromResult(cpu, STATUS_BIT::STATUS_ZERO, result);
    cpu.setAcc(result & 0xff);

}

void Arch6502::OpCodes::OpFuncASL::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getOperandValue(mode);
    result = (result << 1);
    updateStatusBitFromResult(cpu, STATUS_BIT::STATUS_NEGATIVE, result);
    updateStatusBitFromResult(cpu, STATUS_BIT::STATUS_CARRY, result);
    updateStatusBitFromResult(cpu, STATUS_BIT::STATUS_ZERO, result);
    // no write?
}

void Arch6502::OpCodes::OpFuncBCC::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getOperandValue(mode);
    if (!cpu.getStatusBit(STATUS_BIT::STATUS_CARRY))
    {
        cpu.setPC(result);
    }
}

void Arch6502::OpCodes::OpFuncBCS::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getOperandValue(mode);
    if (cpu.getStatusBit(STATUS_BIT::STATUS_CARRY))
    {
        cpu.setPC(result);
    }
}

void Arch6502::OpCodes::OpFuncBEQ::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getOperandValue(mode);
    if (cpu.getStatusBit(STATUS_BIT::STATUS_ZERO))
    {
        cpu.setPC(result);
    }
}

void Arch6502::OpCodes::OpFuncBIT::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t result = uint8_t(cpu.getOperandValue(mode));
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, result & 0x80);
    cpu.setStatusBit(STATUS_BIT::STATUS_OVERFLOW, result & 0x40);
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, (cpu.getAcc() & result) == 0);
}

void Arch6502::OpCodes::OpFuncBMI::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getOperandValue(mode);
    if (cpu.getStatusBit(STATUS_BIT::STATUS_NEGATIVE))
    {
        cpu.setPC(result);
    }
}

void Arch6502::OpCodes::OpFuncBNE::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getOperandValue(mode);
    if (!cpu.getStatusBit(STATUS_BIT::STATUS_ZERO))
    {
        cpu.setPC(result);
    }
}

void Arch6502::OpCodes::OpFuncBPL::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getOperandValue(mode);
    if (!cpu.getStatusBit(STATUS_BIT::STATUS_NEGATIVE))
    {
        cpu.setPC(result);
    }
}

void Arch6502::OpCodes::OpFuncBRK::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.pushStack(cpu.getPC());
    cpu.pushStack(cpu.getStatus());
}

void Arch6502::OpCodes::OpFuncBVC::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getOperandValue(mode);
    if (!cpu.getStatusBit(STATUS_BIT::STATUS_OVERFLOW))
    {
        cpu.setPC(result);
    }
}

void Arch6502::OpCodes::OpFuncBVS::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint16_t result = cpu.getOperandValue(mode);
    if (cpu.getStatusBit(STATUS_BIT::STATUS_OVERFLOW))
    {
        cpu.setPC(result);
    }
}

void Arch6502::OpCodes::OpFuncCLC::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, false);
}

void Arch6502::OpCodes::OpFuncCLD::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setStatusBit(STATUS_BIT::STATUS_DECIMAL, false);
}

void Arch6502::OpCodes::OpFuncCLI::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setStatusBit(STATUS_BIT::STATUS_INTERRUPT, false);
}

void Arch6502::OpCodes::OpFuncCLV::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setStatusBit(STATUS_BIT::STATUS_OVERFLOW, false);
}

void Arch6502::OpCodes::OpFuncCMP::execute(CPU& cpu, ADDRESS_MODE mode)
{
    std::uint8_t a = cpu.getAcc();
    std::uint8_t b = cpu.getOperandValue(mode);
    if (a < b)
    {
        cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, false);
        cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, (a-b) & 0x80);
        cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
    }
    else if (a == b)
    {
        cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, true);
        cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, false);
        cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
    }
    else
    {
        cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, (a - b) & 0x80);
        cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, false);
        cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
    }
}

void Arch6502::OpCodes::OpFuncCPX::execute(CPU& cpu, ADDRESS_MODE mode)
{
    std::uint8_t a = cpu.getX();
    std::uint8_t b = cpu.getOperandValue(mode);
    if (a < b)
    {
        cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, false);
        cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, (a - b) & 0x80);
        cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
    }
    else if (a == b)
    {
        cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, true);
        cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, false);
        cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
    }
    else
    {
        cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, (a - b) & 0x80);
        cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, false);
        cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
    }
}

void Arch6502::OpCodes::OpFuncCPY::execute(CPU& cpu, ADDRESS_MODE mode)
{
    std::uint8_t a = cpu.getX();
    std::uint8_t b = cpu.getOperandValue(mode);
    if (a < b)
    {
        cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, false);
        cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, (a - b) & 0x80);
        cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
    }
    else if (a == b)
    {
        cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, true);
        cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, false);
        cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
    }
    else
    {
        cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, (a - b) & 0x80);
        cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, false);
        cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
    }
}

void Arch6502::OpCodes::OpFuncDEC::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles = 0;
    uint16_t pc = cpu.getPC();
    uint16_t decoded_addr = cpu.decodeAddress(pc, mode, extra_cycles);
    std::uint8_t val = uint8_t(cpu.getOperandValue(mode)) - 1;
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setMemory(decoded_addr, val);
}

void Arch6502::OpCodes::OpFuncDEX::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t x = cpu.getX() - 1;
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, x == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, x & 0x80);
    cpu.setX(x);
}

void Arch6502::OpCodes::OpFuncDEY::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t y = cpu.getY() - 1;
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, y == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, y & 0x80);
    cpu.setY(y);
}


void Arch6502::OpCodes::OpFuncEOR::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getAcc() ^ cpu.getOperandValue(mode);
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setAcc(val);
}

void Arch6502::OpCodes::OpFuncINC::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles = 0;
    uint16_t pc = cpu.getPC();
    uint16_t decoded_addr = cpu.decodeAddress(pc, mode, extra_cycles);
    std::uint8_t val = uint8_t(cpu.getOperandValue(mode)) + 1;
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setMemory(decoded_addr, val);
}

void Arch6502::OpCodes::OpFuncINX::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t x = cpu.getX() + 1;
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, x == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, x & 0x80);
    cpu.setX(x);
}

void Arch6502::OpCodes::OpFuncINY::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t y = cpu.getY() + 1;
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, y == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, y & 0x80);
    cpu.setY(y);
}

void Arch6502::OpCodes::OpFuncJMP::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles;
    uint16_t pc = cpu.getPC();
    uint16_t jump_addr = cpu.decodeAddress(pc, mode, extra_cycles);
    cpu.setPC(jump_addr);
}

void Arch6502::OpCodes::OpFuncJSR::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles;
    uint16_t pc = cpu.getPC();
    uint16_t jump_addr = cpu.decodeAddress(pc, mode, extra_cycles);
    pc += 2;
    cpu.pushStack(pc);
    cpu.setPC(jump_addr);
}

void Arch6502::OpCodes::OpFuncLDA::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getOperandValue(mode);
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setAcc(val);
}

void Arch6502::OpCodes::OpFuncLDX::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getOperandValue(mode);
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setX(val);
}

void Arch6502::OpCodes::OpFuncLDY::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getOperandValue(mode);
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setY(val);
}

void Arch6502::OpCodes::OpFuncLSR::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles;
    uint16_t pc = cpu.getPC();
    uint16_t addr = cpu.decodeAddress(pc, mode, extra_cycles);
    uint8_t val = cpu.getOperandValue(mode);
    cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, val & 0x1);
    val = val >> 1;
    if (mode == ADDRESS_MODE::ACCUMULATOR)
    {
        cpu.setAcc(val);
    }
    else
    {
        cpu.setMemory(addr, val);
    }
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, 0);
}

void Arch6502::OpCodes::OpFuncNOP::execute(CPU& cpu, ADDRESS_MODE mode)
{
    // no op
}

void Arch6502::OpCodes::OpFuncORA::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getOperandValue(mode) | cpu.getAcc();
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setAcc(val);
}

void Arch6502::OpCodes::OpFuncPHA::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.pushStack(cpu.getAcc());
}

void Arch6502::OpCodes::OpFuncPHP::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.pushStack(cpu.getStatus());
}

void Arch6502::OpCodes::OpFuncPLA::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setAcc(cpu.popStack());
}

void Arch6502::OpCodes::OpFuncROL::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles;
    uint16_t pc = cpu.getPC();
    uint16_t addr = cpu.decodeAddress(pc, mode, extra_cycles);
    uint8_t val = cpu.getOperandValue(mode);
    bool carry = cpu.getStatusBit(STATUS_BIT::STATUS_CARRY);
    cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, val & 0x80);
    val = val << 1;
    if (carry)
    {
        val |= 0x1;
    }

    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);

    if (mode == ADDRESS_MODE::ACCUMULATOR)
    {
        cpu.setAcc(val);
    }
    else
    {
        cpu.setMemory(addr, val);
    }
}

void Arch6502::OpCodes::OpFuncROR::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles;
    uint16_t pc = cpu.getPC();
    uint16_t addr = cpu.decodeAddress(pc, mode, extra_cycles);
    uint8_t val = cpu.getOperandValue(mode);
    bool carry = cpu.getStatusBit(STATUS_BIT::STATUS_CARRY);
    cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, val & 0x1);
    val = val >> 1;
    if (carry)
    {
        val |= 0x80;
    }

    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);

    if (mode == ADDRESS_MODE::ACCUMULATOR)
    {
        cpu.setAcc(val);
    }
    else
    {
        cpu.setMemory(addr, val);
    }
}

void Arch6502::OpCodes::OpFuncRTI::execute(CPU& cpu, ADDRESS_MODE mode)
{
    // Get status with BREAK and BIT 5 ignored
    uint8_t status = (cpu.popStack() & 0xcf);
    cpu.setStatusByte(status);

    // Pop PC
    cpu.setPCL(cpu.popStack());
    cpu.setPCH(cpu.popStack());
}

void Arch6502::OpCodes::OpFuncRTS::execute(CPU& cpu, ADDRESS_MODE mode)
{
    // Pop PC
    cpu.setPCL(cpu.popStack());
    cpu.setPCH(cpu.popStack());
    // Inc PC
    cpu.setPC(cpu.getPC() + 1);
}

void Arch6502::OpCodes::OpFuncSBC::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t acc = cpu.getAcc();
    uint8_t m = cpu.getOperandValue(mode);
    bool carry = cpu.getStatusBit(STATUS_BIT::STATUS_CARRY);
    uint8_t borrow = 1 - (carry ? 1 : 0);
    uint8_t val = acc - m - borrow;

    // todo

    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setStatusBit(STATUS_BIT::STATUS_OVERFLOW, acc < m);
    cpu.setAcc(val);
}

void Arch6502::OpCodes::OpFuncSEC::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setStatusBit(STATUS_BIT::STATUS_CARRY, true);
}

void Arch6502::OpCodes::OpFuncSED::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setStatusBit(STATUS_BIT::STATUS_DECIMAL, true);
}

void Arch6502::OpCodes::OpFuncSEI::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setStatusBit(STATUS_BIT::STATUS_INTERRUPT, true);
}

void Arch6502::OpCodes::OpFuncSTA::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles = 0;
    uint16_t pc = cpu.getPC();
    uint16_t decoded_addr = cpu.decodeAddress(pc, mode, extra_cycles);
    std::uint8_t val = cpu.getAcc();
    cpu.setMemory(decoded_addr, val);
}

void Arch6502::OpCodes::OpFuncSTX::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles = 0;
    uint16_t pc = cpu.getPC();
    uint16_t decoded_addr = cpu.decodeAddress(pc, mode, extra_cycles);
    std::uint8_t val = cpu.getX();
    cpu.setMemory(decoded_addr, val);
}

void Arch6502::OpCodes::OpFuncSTY::execute(CPU& cpu, ADDRESS_MODE mode)
{
    int extra_cycles = 0;
    uint16_t pc = cpu.getPC();
    uint16_t decoded_addr = cpu.decodeAddress(pc, mode, extra_cycles);
    std::uint8_t val = cpu.getY();
    cpu.setMemory(decoded_addr, val);
}

void Arch6502::OpCodes::OpFuncTAX::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getAcc();
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setX(val);
}

void Arch6502::OpCodes::OpFuncTAY::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getAcc();
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setY(val);
}

void Arch6502::OpCodes::OpFuncTSX::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getStackPtr();
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setX(val);
}

void Arch6502::OpCodes::OpFuncTXA::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getX();
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setAcc(val);
}

void Arch6502::OpCodes::OpFuncTXS::execute(CPU& cpu, ADDRESS_MODE mode)
{
    cpu.setStackPtr(cpu.getX());
}

void Arch6502::OpCodes::OpFuncTYA::execute(CPU& cpu, ADDRESS_MODE mode)
{
    uint8_t val = cpu.getY();
    cpu.setStatusBit(STATUS_BIT::STATUS_ZERO, val == 0);
    cpu.setStatusBit(STATUS_BIT::STATUS_NEGATIVE, val & 0x80);
    cpu.setAcc(val);
}
