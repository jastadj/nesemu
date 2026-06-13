#include "asm6502.h"

#include <iostream>
#include <fstream>

#include "opcodes6502.h"
#include "defs6502.h"

#include "tools.h"

using namespace Arch6502;

bool ASM::assemble(std::string input_asm_file, std::string output_bin_file)
{
    if (!Tools::fileExists(input_asm_file))
    {
        std::cerr << "Aassembly file doesn't exist: \"" << input_asm_file << "\"" << std::endl;
        return false;
    }

    // Can't write over iteself
    if (input_asm_file == output_bin_file)
    {
        std::cerr << "Cannot set output as input file." << std::endl;
        return false;
    }

    // Try to open input file
    std::ifstream ifile(input_asm_file, std::ios::in);
    if (!ifile.is_open())
    {
        std::cerr << "Error opening input file: \"" << input_asm_file << "\"" << std::endl;
        return false;
    }

    // Try to open output file
    std::ofstream ofile(output_bin_file, std::ios::out | std::ios::binary);
    if (!ifile.is_open())
    {
        std::cerr << "Error opening output file: \"" << output_bin_file << "\"" << std::endl;
        return false;
    }

    // Parse Input File
    // TODO make pass to identify labels/symbols
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
                std::cout << int(b) << " ";
            }
            std::cout << std::endl;
        }
    }

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

    std::vector<std::string> words = Tools::split(asm_line);
    std::string op = words[0];

    // LDX
    if (op == "ldx")
    {

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
            else if(operand.size() == 3)
            {
                bytes.push_back(Tools::toInt(operand));
            }
        }
    }


    return bytes;
}
