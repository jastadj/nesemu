#ifndef CART_H
#define CART_H

#include <string>

#include "nesdefs.h"

// TODO
// - Implement iNES 2.0 bytes 13-15

namespace NES
{
    struct Cart
    {
        Cart(std::string filename);
        ~Cart();

        enum FORMAT
        {
            UNK,
            INES,
            INES2
        };
        static const char* getFormatString(FORMAT fmt);

        std::string filename;
        std::size_t data_size;
        unsigned char* data;
        FORMAT format;
        std::size_t getHeaderSize() const;



        std::size_t getPRGROMSize() const;
        std::size_t getCHRROMSize() const;

        std::size_t getPRGRAMSize() const;
        std::size_t getCHRRAMSize() const;

        std::size_t getPRGNVRAMSize() const;
        std::size_t getCHRNVRAMSize() const;
        
        uint8_t getMapper() const;
        uint8_t getSubMapper() const;
        CONSOLE_TYPE getConsoleType() const;
        TIMING_MODE getTimingMode() const;
        uint16_t getResetVector() const;

        // Flags
        bool isNametableHorizontallyArranged() const;
        bool hasNVRam() const;
        bool hasTrainer() const;
        bool hasAlternativeNametableLayout() const;

    };
}

#endif