#include "nes.hpp"

#include <iomanip>

NES::NES()
{
    // init memory
    m_MemCPU = new MemoryMap(MEM_SIZE);
    std::cout << "Allocated " << MEM_SIZE << " bytes of memory.\n";

    // configure cpu memory mirroring
    m_MemCPU->mirror(0x0000, 0x07ff, 0x0800, 0x0fff);
    m_MemCPU->mirror(0x0000, 0x07ff, 0x1000, 0x17ff);
    m_MemCPU->mirror(0x0000, 0x07ff, 0x1800, 0x1fff);
    // ppu i/o register mirroring (in cpu)
    for(unsigned int i = 0x2008; i <= 0x3fff; i += 8) m_MemCPU->mirror(0x2000, 0x2007, i, i+7);



    // init PPU memory
    m_MemPPU = new MemoryMap(PPUMEM_SIZE);
    std::cout << "Allocated " << PPUMEM_SIZE << " bytes of PPU memory.\n";

    // rom cartridge
    m_Cartridge = NULL;

    // init CPU
    m_CPU = new RP2A03(m_MemCPU->getMap(), MEM_SIZE);

    // init PPU
    m_PPU = new C2C02(m_MemPPU->getMap(), PPUMEM_SIZE);
}

bool NES::loadCartridge(std::string romfile)
{
    std::cout << "Loading cartridge from rom file : " << romfile << std::endl;

    if(m_Cartridge) delete m_Cartridge;

    m_Cartridge = new Cartridge(romfile);

    if(m_Cartridge->loadSuccessful())
    {
        // clear cpu mirroring
        // note : some mappers layout mirroring differently
        m_MemCPU->clearMirror(0x8000, 0xffff);

        // clear exisiting CPU memory
        m_MemCPU->clear(0x8000, 0xffff);

        // PRG RAM 0x6000 - 0x7fff (battery backed persistent ram)

        // load PRG ROM from cartridge to CPU memory 0x8000 - 0xffff
        if(m_Cartridge->getPRGROMSizeByte())
        {
            const uint8_t *rom = m_Cartridge->getPRGROM();
            const uint16_t prgoffset = 0x8000;

            std::cout << "Copying PRG ROM to CPU memory." << std::endl;

            for(int i = 0; i < 0x8000; i++)  m_MemCPU->write(prgoffset + i, rom[i]);
        }

        // load CHR data from cartridge to PPU memory 0x0000 - 0x1fff
        if(m_Cartridge->getCHRROMSizeByte())
        {
            const uint8_t *rom = m_Cartridge->getCHRROM();

            std::cout << "Copying CHR ROM to PPU memory." << std::endl;

            for(int i = 0; i < 0x2000; i++) m_MemPPU->write(i, rom[i]);
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
