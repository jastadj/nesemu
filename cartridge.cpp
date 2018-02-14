#include "cartridge.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>

Cartridge::Cartridge(std::string romfile)
{

    std::ifstream ifile;

    m_LoadedSuccessfully = false;
    m_ROMFileName = romfile;

    // open rom file
    ifile.open(m_ROMFileName.c_str(), std::ios::binary | std::ios::in);

    if(!ifile.is_open()) return;

    // read 16 byte header
    for(int i = 0; i < 16; i++) m_Header[i] = uint8_t(ifile.get());
    if(ifile.eof()) { ifile.close(); return; }

    // read trainer if present
    if(hasTrainerData())
    {
        m_Trainer = new uint8_t[512];

        for(int i = 0; i < 512; i++) m_Trainer[i] = uint8_t(ifile.get());


    }
    else m_Trainer = NULL;


    // read PRG ROM
    if(!getPRGROMSize()) m_PRGROM = NULL;
    else
    {
        m_PRGROM = new uint8_t [getPRGROMSize()];
        for(int i = 0; i < getPRGROMSize(); i++)
        {
            m_PRGROM[i] = uint8_t(ifile.get());

        }

    }

    // read CHR ROM
    if(!getCHRROMSIZE()) m_CHRROM = NULL;
    else
    {
        m_CHRROM = new uint8_t [getCHRROMSIZE()];
        for(int i = 0; i < getCHRROMSIZE(); i++)
        {
            m_CHRROM[i] = uint8_t(ifile.get());
            if(ifile.eof()) { ifile.close(); return; }
        }

    }


    ifile.close();
    m_LoadedSuccessfully = true;
}

Cartridge::~Cartridge()
{
    if(m_Trainer) delete m_Trainer;
    if(m_PRGROM) delete m_PRGROM;
    if(m_CHRROM) delete m_CHRROM;
}

void Cartridge::show()
{
    std::cout << "Cartridge ROM file : " << m_ROMFileName << std::endl;
    std::cout << "Header             : ";
    for(int i = 0; i < 16; i++) std::cout << std::hex << std::setw(2) << std::setfill('0') << int(m_Header[i]) << " ";
    std::cout << std::endl;
    std::cout << "Load successful    : " << loadSuccessful() << std::endl;
    std::cout << "PRG ROM Size       : 0x" << getPRGROMSize() << std::endl;
    std::cout << "CHR ROM Size       : 0x" << getCHRROMSIZE() << std::endl;
    std::cout << "Vertically Mirrored: " << isVerticallyMirrored() << std::endl;
    std::cout << "Battery-backed     : " << isBatteryBacked() << std::endl;
    std::cout << "Trained Data       : " << hasTrainerData() << std::endl;
    std::cout << "Ignore Mirroring   : " << IgnoreMirroring() << std::endl;
    std::cout << "Mapper Number      : 0x" << int(getMapperNumber()) << std::endl;
    std::cout << "VS Unisystem       : " << VSUnisystem() << std::endl;
    std::cout << "Playchoice-10      : " << PlayChoice10() << std::endl;
    std::cout << "PAL                : " << isPAL() << std::endl;
    std::cout << "Video Type         : 0x" << getVideoType() << std::endl;
    std::cout << "PRG RAM Present    : " << isPRGRAMPresent() << std::endl;
    std::cout << "Has Bus Conflicts  : " << hasBusConflicts() << std::endl;



}
