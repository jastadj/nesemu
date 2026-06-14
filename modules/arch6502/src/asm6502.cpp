#include "asm6502.h"

#include <iostream>
#include <fstream>
#include <iomanip>

#include "opcodes6502.h"
#include "defs6502.h"

#include "tools.h"

using namespace Arch6502;

bool ASM::assemble(std::string input_file, std::string output_file)
{
    if (!Tools::fileExists(input_file))
    {
        std::cerr << "Aassembly file doesn't exist: \"" << input_file << "\"" << std::endl;
        return false;
    }

    // Can't write over iteself
    if (input_file == output_file)
    {
        std::cerr << "Cannot set output as input file." << std::endl;
        return false;
    }

    // Try to open input file
    std::ifstream ifile(input_file, std::ios::in);
    if (!ifile.is_open())
    {
        std::cerr << "Error opening input file: \"" << input_file << "\"" << std::endl;
        return false;
    }

    // Try to open output file
    std::ofstream ofile(output_file, std::ios::out | std::ios::binary);
    if (!ofile.is_open())
    {
        ifile.close();
        std::cerr << "Error opening output file: \"" << output_file << "\"" << std::endl;
        return false;
    }

    // ROM Header
    // TODO - Use INES 2.0 Format
    unsigned char header[16] = {
        'N','E','S',0x1a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    // Parse Input File
    // TODO make pass to identify labels/symbols
    std::vector<uint8_t> rom_data;
    while (!ifile.eof())
    {
        std::string buf;
        std::getline(ifile, buf);

        // Trim off comment ';'
        std::size_t com_pos = buf.find_first_of(';');
        if (com_pos != std::string::npos)
        {
            buf.resize(com_pos);
        }

        // Parse line into machine code
        std::vector<uint8_t> bytes = getBytes(buf);
        if (!bytes.empty())
        {
            std::cout << buf << " => ";
            for (auto& b : bytes)
            {
                rom_data.push_back(b);
                std::cout << std::hex << std::setw(2) << std::setfill('0') << int(b) << " ";
            }
            std::cout << std::endl;
        }
    }

    // ROM Data Size
    std::cout << "ROM Data Size: " << std::dec << rom_data.size();
    std::cout << " (0x" << std::hex << rom_data.size() << ")" << std::dec << std::endl;
    
    std::size_t padding = (1024 * 16) - rom_data.size();
    std::cout << "Padding: " << padding << std::endl;
    for (auto i = 0; i < padding; i++)
    {
        rom_data.push_back(0);
    }

    std::cout << "Final ROM Data Size: " << std::dec << rom_data.size();
    std::cout << " (0x" << std::hex << rom_data.size() << ")" << std::dec << std::endl;

    // Set Reset Vector
    rom_data[0x8000 - 4] = 0x00;
    rom_data[0x8000 - 3] = 0x80;


    // Write Header Data
    ofile.write((const char*)header, 16);

    // Write ROM Data
    ofile.write((const char*)rom_data.data(), rom_data.size());

    // Done
    ifile.close();
    ofile.flush();
    ofile.close();

    return false;
}

std::vector<uint8_t> ASM::getBytes(std::string asm_line)
{
    std::vector<uint8_t> data;
    if (asm_line.empty())
    {
        return data;
    }

    asm_line = Tools::toLower(asm_line);

    std::vector<std::string> strings = Tools::split(asm_line);
    ADDRESS_MODE mode = ADDRESS_MODE::IMPLIED;

    std::string op_str = strings[0];
    strings.erase(strings.begin());
    std::vector<std::string> operand_strings = strings;
        
    // Parse Op and Operand strings into bytes
    OpCodes::OpFunc* opfunc = OpCodes::LUT::getOpFuncByMnemonic(op_str);
    if (opfunc)
    {
        std::map<ADDRESS_MODE, uint8_t> modemap = opfunc->getModeMap();
        if (modemap.count(mode))
        {
            // Operand Byte
            data.push_back(modemap[mode]);
            // Operand Bytes
            for (auto operand_str : operand_strings)
            {
                for (auto& b : parseOperand(operand_str, mode))
                {
                    data.push_back(b);
                }
            }
        }
    }

    return data;
}

std::vector<uint16_t> ASM::parseOperand(std::string operand, ADDRESS_MODE& mode)
{    
    mode = ADDRESS_MODE::ADDR_ERROR;
    operand = Tools::toLower(Tools::removeSpaces(operand));
    if (operand.empty())
    {
        return std::vector<uint16_t>();
    }
    std::vector<uint16_t> bytes;

    if (operand.size() >= 3)
    {
        // Immediate?
        if (operand[0] == '#')
        {
            mode = ADDRESS_MODE::IMMEDIATE;
            operand = operand.erase(0, 1);
            // Hex Value
            if (operand[0] == '$' && operand.size() == 3)
            {
                operand = operand.erase(0, 1);
                bytes.push_back(Tools::toInt("0x" + operand));
            }
            else
            {
                bytes.push_back(Tools::toInt(operand));
            }
        }
    }

    return bytes;
}
