#include "nes.hpp"

NES::NES()
{
    // init memory
    m_Mem = new uint8_t [MEM_SIZE];
    std::cout << "Allocated " << MEM_SIZE << " bytes of memory.\n";
    clearMemory();

    // rom cartridge
    m_Cartridge = NULL;

    // init CPU
    m_CPU = new C6502(m_Mem, MEM_SIZE);
}

// clear memory
void NES::clearMemory(uint16_t startaddress, uint16_t endaddress)
{
    for(int i = startaddress; i <= endaddress; i++)
    {
        m_Mem[i] = 0x0;
    }
}

bool NES::loadCartridge(std::string romfile)
{
    if(m_Cartridge) delete m_Cartridge;

    m_Cartridge = new Cartridge(romfile);

    if(m_Cartridge->loadSuccessful())
    {
        uint16_t coffset = 0x8000;

        // clear exisiting memory
        clearMemory(coffset, 0xfff9);

        // load PRG ROM from cartridge to nes
        if(m_Cartridge->getPRGROMSize())
        {
            const uint8_t *rom = m_Cartridge->getPRGROM();

            for(int i = 0; i < m_Cartridge->getPRGROMSize(); i++)
            {
                m_Mem[coffset + i] = rom[i];
            }
        }


        std::cout << "Successfully loaded ROM : " << romfile << std::endl;
        m_Cartridge->show();
        return true;
    }
    else
    {
        std::cout << "Error loading ROM : " << romfile << std::endl;
        m_Cartridge->show();
        delete m_Cartridge;
        m_Cartridge = NULL;
    }

    return false;
}

/////////////////////////////////////////////
// DEBUG
void NES::debugC6502Console()
{
    m_CPU->debugConsole();
}
