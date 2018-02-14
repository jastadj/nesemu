#ifndef CLASS_CARTRIDGE
#define CLASS_CARTRIDGE

#include <string>

class Cartridge
{
private:

    uint8_t m_Header[16];

    uint8_t *m_Trainer;
    uint8_t *m_PRGROM;
    uint8_t *m_CHRROM;
    //uint8_t *m_PC_INST-ROM;
    //uint8_t *m_PC_PROM;

    std::string m_ROMFileName;
    bool m_LoadedSuccessfully;

public:
    Cartridge(std::string romfile);
    ~Cartridge();

    bool loadSuccessful() { return m_LoadedSuccessfully;}

    int getPRGROMSize() { return 0x4000 * m_Header[4];}
    int getCHRROMSIZE() { return 0x2000 * m_Header[5];}

    const uint8_t *getPRGROM() const { return m_PRGROM;}

    // flag 6
    bool isVerticallyMirrored() { return m_Header[6] & 0x1;} // false = horizontally mirrored
    bool isBatteryBacked() { return m_Header[6] & 0x2;} // 0x6000 - 0x7fff persistent memory
    bool hasTrainerData() { return m_Header[6] & 0x4;} // 0x7000 - 0x71ff
    bool IgnoreMirroring() { return m_Header[6] & 0x8;} // ignore mirror control, provide four-screen vram

    // flag 6 & 7 - mapper number
    uint8_t getMapperNumber() { return (m_Header[6] >> 4) + (m_Header[7] & 0xf0);}

    // flag 7
    bool VSUnisystem() { return m_Header[7] & 0x1;}
    bool PlayChoice10() { return m_Header[7] & 0x2;}

    // flag 9
    bool isPAL() { return m_Header[9] & 0x1;}

    // flag 10
    int getVideoType() { return m_Header[10] & 0x3;}
    bool isPRGRAMPresent() { return m_Header[10] & 0x10;}
    bool hasBusConflicts() { return m_Header[10] & 0x20;}

    // debug info
    void show();

};

#endif // CLASS_CARTRIDGE
