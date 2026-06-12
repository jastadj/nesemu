#include "asm6502.h"

#include <iostream>
#include <fstream>

#include "opcodes6502.h"
#include "defs6502.h"

#include "tools.h"

bool ASM6502::assemble(std::string input_asm, std::string output_asm)
{
    if (!Tools::fileExists(input_asm))
    {
        std::cerr << "Aassembly file doesn't exist: \"" << input_asm << "\"" << std::endl;
        return false;
    }

    // Can't write over iteself
    if (input_asm == output_asm)
    {
        std::cerr << "Cannot set output as input file." << std::endl;
        return false;
    }

    // Try to open input file
    std::ifstream ifile(input_asm, std::ios::in);
    if (!ifile.is_open())
    {
        std::cerr << "Error opening input file: \"" << input_asm << "\"" << std::endl;
        return false;
    }

    // Try to open output file
    std::ofstream ofile(output_asm, std::ios::out | std::ios::binary);
    if (!ifile.is_open())
    {
        std::cerr << "Error opening output file: \"" << output_asm << "\"" << std::endl;
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

        // Parse line
        std::vector<std::string> words = Tools::split(buf, ' ', false);
        std::string opcode;
        std::string operand;
        for (auto i = 0; i < words.size(); i++)
        {
            // OPCODE?
            if (i == 0)
            {
                opcode = words[i];
            }
            // OPERAND?
            else if (i == 1)
            {
                operand = words[i];
            }
        }

        if (!opcode.empty())
        {
            std::cout << "OPCODE: " << opcode << ", OPERAND: " << operand << std::endl;
        }
    }

    // Done
    ifile.close();
    ofile.flush();
    ofile.close();

    return false;
}