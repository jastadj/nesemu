#include "cart.h"

#include <fstream>

NES::Cart::Cart(std::string filename):
    data(nullptr),
    data_size(0),
    format(FORMAT::UNK)
{
    std::ifstream ifile(filename, std::ios::in | std::ios::binary);
    if (ifile.is_open())
    {
        ifile.seekg(0, std::ios::end);
        data_size = ifile.tellg();
        ifile.seekg(0, std::ios::beg);

        if (data_size > 16)
        {
            this->filename = filename;
            data = new unsigned char[data_size]();
            ifile.read((char*)data, data_size);

            // iNES / iNES 2.0 Format
            if (data[0] == 0x4e || data[1] == 0x45 || data[2] == 0x53 || data[3] == 0x1a)
            {
                if (data[7] & 0x8)
                {
                    format = INES2;
                }
                else
                {
                    format = INES;
                }
            }

            // If unknown format, delete data
            if (format == FORMAT::UNK)
            {
                delete[] data;
                data_size = 0;
            }
        }
        ifile.close();
    }
}

NES::Cart::~Cart()
{
    delete[] data;
}

const char* NES::Cart::getFormatString(Cart::FORMAT fmt)
{
    switch (fmt)
    {
    case FORMAT::INES: return "iNES";
    case FORMAT::INES2: return "iNES 2.0";
    default:
        break;
    }
    return "unk";
}

std::size_t NES::Cart::getHeaderSize() const
{
    if (format == FORMAT::INES || format == FORMAT::INES2)
    {
        return 16;
    }
    return 0;
}

std::size_t NES::Cart::getPRGROMSize() const
{
    if (format == FORMAT::INES || format == FORMAT::INES2)
    {
        return data[4] * 1024 * 16;
    }
    return 0;
}

std::size_t NES::Cart::getCHRROMSize() const
{
    if (format == FORMAT::INES || format == FORMAT::INES2)
    {
        return data[5] * 1024 * 8;
    }
    return 0;
}

std::size_t NES::Cart::getPRGRAMSize() const
{
    if (format == FORMAT::INES)
    {
        return data[8] * 1024 * 8;
    }
    else if (format == FORMAT::INES2)
    {
        int shift_count = int(data[10] & 0xf);
        if (shift_count)
        {
            // Shift 64 by shift count
            return 0x40 << shift_count;
        }
    }
    return 0;
}

std::size_t NES::Cart::getCHRRAMSize() const
{
    if (format == FORMAT::INES2)
    {
        int shift_count = int(data[11] & 0xf);
        if (shift_count)
        {
            // Shift 64 by shift count
            return 0x40 << shift_count;
        }

    }
    return 0;
}

std::size_t NES::Cart::getPRGNVRAMSize() const
{
    if (format == FORMAT::INES2)
    {
        int shift_count = int((data[10] >> 4) & 0xf);
        if (shift_count)
        {
            // Shift 64 by shift count
            return 0x40 << shift_count;
        }
    }
    return 0;
}

std::size_t NES::Cart::getCHRNVRAMSize() const
{
    if (format == FORMAT::INES2)
    {
        int shift_count = int((data[11] >> 4) & 0xf);
        if (shift_count)
        {
            // Shift 64 by shift count
            return 0x40 << shift_count;
        }
    }
    return 0;
}

bool NES::Cart::isNametableHorizontallyArranged() const
{
    if (format == FORMAT::INES || format == FORMAT::INES2)
    {
        return data[6] & 0x1;
    }
    return false;
}

bool NES::Cart::hasNVRam() const
{
    if (format == FORMAT::INES || format == FORMAT::INES2)
    {
        return data[6] & 0x2;
    }
    return false;
}

bool NES::Cart::hasTrainer() const
{
    if (format == FORMAT::INES || format == FORMAT::INES2)
    {
        return data[6] & 0x4;
    }
    return false;
}

bool NES::Cart::hasAlternativeNametableLayout() const
{
    if (format == FORMAT::INES || format == FORMAT::INES2)
    {
        return data[6] & 0x8;
    }
    return false;
}

uint8_t NES::Cart::getMapper() const
{
    if (format == FORMAT::INES)
    {
        return (data[6] >> 4) | (data[7] & 0xf0);
    }
    else if (format == FORMAT::INES2)
    {
        return data[8] & 0xf;
    }
    return 0;
}

uint8_t NES::Cart::getSubMapper() const
{
    if (format == FORMAT::INES2)
    {
        return (data[8] >> 4) & 0xf;
    }
    return 0;
}

NES::CONSOLE_TYPE NES::Cart::getConsoleType() const
{
    if (format == FORMAT::INES || format == FORMAT::INES2)
    {
        return CONSOLE_TYPE(data[7] & 0x3);
    }
    return CONSOLE_TYPE::STANDARD;
}

NES::TIMING_MODE NES::Cart::getTimingMode() const
{
    if (format == FORMAT::INES)
    {
        return TIMING_MODE(data[10] & 0x1);
    }
    return TIMING_MODE::NTSC;
}

