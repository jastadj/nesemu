#include "c2c02.hpp"

#include <sstream>
#include <vector>
#include <iomanip>
#include <fstream>


C2C02::C2C02(uint8_t **memory, unsigned int memory_size)
{
    m_MemSize = memory_size;
    m_Mem = memory;

    // these need to be mapped with CPU memory
    // use mapRegisters(CPU MEMORY) to map registers
    m_PPUCTRL = NULL;
    m_PPUMASK = NULL;
    m_PPUSTATUS = NULL;
    m_OAMADDR = NULL;
    m_OAMDATA = NULL;
    m_PPUSCROLL = NULL;
    m_PPUADDR = NULL;
    m_PPUDATA = NULL;
    m_OAMDMA = NULL;

    init();
}

C2C02::~C2C02()
{

}

bool C2C02::init()
{
    return true;
}

void C2C02::mapRegisters(uint8_t **cpumem)
{
    std::cout << "PPU registers exposed to CPU memory." << std::endl;
    m_PPUCTRL = cpumem[PPUCTRL];
    m_PPUMASK = cpumem[PPUMASK];
    m_PPUSTATUS = cpumem[PPUSTATUS];
    m_OAMADDR = cpumem[OAMADDR];
    m_OAMDATA = cpumem[OAMDATA];
    m_PPUSCROLL = cpumem[PPUSCROLL];
    m_PPUADDR = cpumem[PPUADDR];
    m_PPUDATA = cpumem[PPUDATA];
    m_OAMDMA = cpumem[OAMDMA];
}

//////////////////////////////////
// DEBUG

void C2C02::show()
{
    std::cout << "PPU Registers:" << std::endl;
    std::cout << "PPUCTRL   = " << std::hex << std::setfill('0') << std::setw(2) << int(*m_PPUCTRL) << std::endl;
    std::cout << "PPUMASK   = " << std::hex << std::setfill('0') << std::setw(2) << int(*m_PPUMASK) << std::endl;
    std::cout << "PPUSTATUS = " << std::hex << std::setfill('0') << std::setw(2) << int(*m_PPUSTATUS) << std::endl;
    std::cout << "OAMADDR   = " << std::hex << std::setfill('0') << std::setw(2) << int(*m_OAMADDR) << std::endl;
    std::cout << "OAMDATA   = " << std::hex << std::setfill('0') << std::setw(2) << int(*m_OAMDATA) << std::endl;
    std::cout << "PPUSCROLL = " << std::hex << std::setfill('0') << std::setw(2) << int(*m_PPUSCROLL) << std::endl;
    std::cout << "PPUADDR   = " << std::hex << std::setfill('0') << std::setw(2) << int(*m_PPUADDR) << std::endl;
    std::cout << "PPUDATA   = " << std::hex << std::setfill('0') << std::setw(2) << int(*m_PPUDATA) << std::endl;
    std::cout << "OAMDMA    = " << std::hex << std::setfill('0') << std::setw(2) << int(*m_OAMDMA) << std::endl;
}

void C2C02::debugConsole(std::string prompt)
{
    bool quit = false;


    while(!quit)
    {
        std::string buf;
        std::vector<std::string> words;

        std::cout << prompt;

        std::getline(std::cin, buf);

        // strip words and white space
        while(!buf.empty())
        {
            size_t cpos = buf.find_first_of(' ');

            if(cpos == 0)
            {
                buf.erase(0,1);
                continue;
            }

            words.push_back( buf.substr(0, cpos));
            buf.erase(0, cpos);
        }

        if(words.empty()) continue;

        if(words[0] == "quit" || words[0] == "exit") quit = true;
        else if(words[0] == "help")
        {
            std::cout << "quit - exit console" << std::endl;
            std::cout << "help - show this menu" << std::endl;
            std::cout << "show - show relevant NES information" << std::endl;
            std::cout << "r <addr> [count] - read value at memory address and optional additional bytes" << std::endl;
            std::cout << "w <addr> <byte> - write byte to memory address" << std::endl;
            std::cout << "clearmem - clear all memory" << std::endl;
            std::cout << "dumpmem [file] - dump memory, optionally to file" << std::endl;
            std::cout << "loadmem <file> [offset] - load memory from file at optional offset" << std::endl;
            std::cout << "printpattern | showpattern <offset> - print pattern at offset" << std::endl;
        }
        else if(words[0] == "show")
        {
            show();
        }
        else if(words[0] == "w" || words[0] == "r")
        {
            uint16_t addr;
            std::stringstream addrss;

            if(words[1].size() >= 3)
                if(words[1][1] == 'x') words[1].erase(0,2);

            // convert hex string to value
            addrss << std::hex << words[1];
            addrss >> addr;

            // write memory
            if(words[0] == "w")
            {
                if(words.size() == 3)
                {
                    int wval;
                    std::stringstream wss;

                    if(words[2].size() >= 3)
                        if(words[2][1] == 'x') words[2].erase(0,2);

                    wss << std::hex << words[2];
                    wss >> wval;

                    if(wval <= 0xff)
                    {
                        std::cout << std::hex << std::setfill('0') << std::setw(4) << addr << " = " << wval << std::endl;
                        *m_Mem[addr] = uint8_t(wval);
                    }
                    else std::cout << "Value larger than 1 byte!" << std::endl;
                }
                else std::cout << "Invalid parameters!  w <addr> <byte>" << std::endl;
            }
            // read memory
            else
            {
                int bcount = 1;
                if(words.size() == 3) bcount = atoi(words[2].c_str());

                for(int i = 0; i < bcount; i++)
                {
                    std::cout << std::hex << std::setfill('0') << std::setw(4) << addr+i << ": ";
                    std::cout << std::setfill('0') << std::setw(2) << int(*m_Mem[addr+i]) << std::endl;
                }

            }
        }
        else if(words[0] == "clearmem")
        {
            for(unsigned int i = 0; i < m_MemSize; i++)
            {
                *m_Mem[i] = 0x0;
            }
        }
        else if(words[0] == "dumpmem")
        {
            if(words.size() == 1)
            {
                for(unsigned int i = 0; i < m_MemSize/16; i++)
                {
                    std::cout << std::hex << std::setfill('0') << std::setw(4) << i*16 << ": ";
                    for(int n = 0; n < 16; n++)
                        std::cout << std::hex << std::setfill('0') << std::setw(2) << int(*m_Mem[i*16 + n]) << " ";
                    std::cout << std::endl;
                }
            }
            else if(words.size() == 2)
            {
                std::ofstream ofile;
                unsigned int lastentry = 0;

                // find last non zero data
                for(int i = m_MemSize-1; i >= 0; i--)
                {
                    if(*m_Mem[i] != 0)
                    {
                        lastentry = i;
                        break;
                    }
                }

                // open output file for writing
                ofile.open(words[1].c_str(), std::ios::binary);

                if(ofile.is_open())
                {

                    // write all memory to file, stopping at last non-zero address
                    for(unsigned int i = 0; i <= lastentry; i++) ofile.put( (unsigned char)( int(*m_Mem[i])) );
                    ofile.close();
                    std::cout << "Wrote " << std::dec << lastentry + 1 << " bytes to " << words[1] << std::endl;

                }
                else std::cout << "Error opening file " << words[1] << std::endl;
            }

        }
        else if(words[0] == "loadmem")
        {
            int loffset = 0;

            if(words.size() >= 2 && words.size() <= 3)
            {
                if(words.size() == 3) loffset = atoi(words[2].c_str());

                std::ifstream ifile;

                ifile.open(words[1].c_str(), std::ios::binary);

                if(ifile.is_open())
                {
                    int bytes = 0;

                    while(!ifile.eof())
                    {
                        uint8_t data = uint8_t(ifile.get());
                        if(!ifile.eof())
                        {
                            *m_Mem[loffset + bytes] = data;
                            bytes++;
                        }
                    }
                    std::cout << "Loaded " << std::dec << bytes << " bytes from " << words[1];
                    std::cout << " starting at 0x" << std::hex << std::setfill('0') << std::setw(4) << loffset << std::endl;
                }
                else std::cout << "Error opening file " << words[1] << std::endl;

            }
            else std::cout << "Incorrect parameters : loadmem <file> [offset]" << std::endl;
        }
        else if(words[0] == "printpattern" || words[0] == "showpattern")
        {
            if(words.size() == 2)
            {
                std::stringstream patss;
                uint16_t poffset;
                std::vector< std::vector<char> > pat;

                if(words[1][1] == 'x') words[1].erase(0,2);

                // convert hex string to value
                patss << std::hex << words[1];
                patss >> poffset;

                pat.resize(8);
                for(int i = 0; i < 8; i++) pat[i].resize(8);

                if( int(poffset + 15) < int(m_MemSize) )
                {
                    std::cout << "Printing pattern ";
                    std::cout << std::hex << std::setw(4) << std::setfill('0') << int(poffset) << " - ";
                    std::cout << std::hex << std::setw(4) << std::setfill('0') << int(poffset+15) << std::endl;

                    //plane 1
                    for(int i = poffset; i < poffset + 8; i++)
                    {
                        for(int k = 0; k < 8; k++)
                        {
                            if( (*m_Mem[i] >> (7 - k)) & 0x1) pat[i - poffset][k] = '1';
                            else pat[i - poffset][k] = '.';
                        }
                    }

                    // plane 2
                    for(int i = poffset + 8; i < poffset + 16; i++)
                    {
                        for(int k = 0; k < 8; k++)
                        {
                            if( (*m_Mem[i] >> (7 - k)) & 0x1)
                            {
                                if(pat[i - poffset - 8][k]) pat[i - poffset - 8][k] = '3';
                                else pat[i - poffset - 8][k] = '2';
                            }
                        }
                    }

                    std::cout << std::endl;
                    for(int i = 0; i < 8; i++)
                    {
                        for(int n = 0; n < 8; n++) std::cout << pat[i][n];
                        std::cout << std::endl;
                    }
                    std::cout << std::endl;
                }
                else std::cout << "Out of bounds!" << std::endl;
            }
            else std::cout << "Invalid parameters!" << std::endl;
        }
        else std::cout << "Unknown command - type help" << std::endl;
    }
}
