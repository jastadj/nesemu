#include "nes.hpp"

#include <iomanip>

NES::NES()
{
    // init memory
    m_Mem = new uint8_t [MEM_SIZE];
    std::cout << "Allocated " << MEM_SIZE << " bytes of memory.\n";
    clearMemory(m_Mem);

    // init PPU memory
    m_PPUMem = new uint8_t [PPUMEM_SIZE];
    std::cout << "Allocated " << PPUMEM_SIZE << " bytes of PPU memory.\n";
    clearMemory(m_PPUMem);

    // rom cartridge
    m_Cartridge = NULL;

    // init CPU
    m_CPU = new RP2A03(m_Mem, MEM_SIZE);

    // init PPU
    m_PPU = new C2C02(m_PPUMem, PPUMEM_SIZE);
}

// clear memory
void NES::clearMemory(uint8_t *bank, uint16_t startaddress, uint16_t endaddress)
{
    if(endaddress == 0xffff && bank == m_PPUMem) endaddress = PPUMEM_SIZE - 1;

    std::cout << "Clearing ";
    if(bank == m_Mem) std::cout << "CPU memory ";
    else if(bank == m_PPUMem) std::cout << "PPU memory ";
    std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << int(startaddress) << " - ";
    std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << int(endaddress) << std::endl;



    for(int i = 0; i <= int(endaddress); i++)
    {
        bank[i] = 0x00;
    }
}

bool NES::loadCartridge(std::string romfile)
{
    std::cout << "Loading cartridge from rom file : " << romfile << std::endl;

    if(m_Cartridge) delete m_Cartridge;

    m_Cartridge = new Cartridge(romfile);

    if(m_Cartridge->loadSuccessful())
    {
        // clear exisiting memory
        clearMemory(m_Mem, 0x6000, 0xffff);

        // PRG RAM 0x6000 - 0x7fff (battery backed persistent ram)

        // load PRG ROM from cartridge to CPU memory 0x8000 - 0xffff
        if(m_Cartridge->getPRGROMSizeByte())
        {
            const uint8_t *rom = m_Cartridge->getPRGROM();
            const uint16_t prgoffset = 0x8000;

            std::cout << "Copying PRG ROM to CPU memory." << std::endl;

            for(int i = 0; i < 0x8000; i++)  m_Mem[prgoffset + i] = rom[i];
        }

        // load CHR data from cartridge to PPU memory 0x0000 - 0x1fff
        if(m_Cartridge->getCHRROMSizeByte())
        {
            const uint8_t *rom = m_Cartridge->getCHRROM();

            std::cout << "Copying CHR ROM to PPU memory." << std::endl;

            for(int i = 0; i < 0x2000; i++) m_PPUMem[i] = rom[i];
        }


        std::cout << "Successfully loaded ROM : " << romfile << std::endl;
        return true;
    }
    else
    {
        std::cout << "### Error loading ROM : " << romfile << std::endl;
        delete m_Cartridge;
        m_Cartridge = NULL;
    }

    return false;
}

/////////////////////////////////////////////
// DEBUG

void NES::debugConsole(std::string prompt)
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
            std::cout << "cpu - enter CPU debug console" << std::endl;
            std::cout << "ppu - enter PPU debug console" << std::endl;
            std::cout << "showrom - show rom/cartridge information" << std::endl;
            std::cout << "unloadrom - unload rom/cartridge" << std::endl;
            std::cout << "loadrom <filename> - load rom/cart from file" << std::endl;
        }
        else if(words[0] == "show")
        {
            std::cout << "not implemented yet.." << std::endl;
        }
        else if(words[0] == "cpu")
        {
            m_CPU->debugConsole("NES.CPU> ");
            std::cout << "Exiting CPU console..." << std::endl << std::endl;
        }
        else if(words[0] == "ppu")
        {
            m_PPU->debugConsole("NES.PPU> ");
            std::cout << "Exiting PPU console..." << std::endl << std::endl;
        }
        else if(words[0] == "showrom")
        {
            if(m_Cartridge) m_Cartridge->show();
            else std::cout << "No cartridge loaded." << std::endl;
        }
        else if(words[0] == "unloadrom")
        {
            if(m_Cartridge)
            {
                delete m_Cartridge;
                m_Cartridge = NULL;
                std::cout << "Cartridge deleted." << std::endl;
            }
            else std::cout << "No cartridge loaded!" << std::endl;

        }
        else if(words[0] == "loadrom")
        {
            if(words.size() == 2)
            {
                loadCartridge(words[1]);
            }
            else std::cout << "Invalid parameters!" << std::endl;
        }
        else std::cout << "Unknown command - type help" << std::endl;

    }
}
