#ifndef CPU6502_H
#define CPU6502_H

#include <cstdlib>
#include <cstdint>

#include "defs6502.h"

#include "memorymap.h"

namespace Arch6502
{
    class CPU
    {
    public:
        CPU();
        ~CPU();

        int execute();

        // Registers
        const uint8_t getAcc() const;
        void setAcc(uint8_t val);
        const uint8_t getX() const;
        void setX(uint8_t val);
        const uint8_t getY() const;
        void setY(uint8_t val);

        // Stack
        const uint8_t getStackPtr() const;
        void setStackPtr(uint8_t val);
        void pushStack(uint16_t val);
        void pushStack(uint8_t val);
        uint8_t popStack();
        void resetStack();

        // Program Counter
        const uint16_t getPC() const;
        void setPC(const uint16_t pc);
        void setPCL(const uint8_t pcl); // Set PC Low Byte
        void setPCH(const uint8_t pch); // Set PC High Byte

        // Memory
        void setMemoryMap(MemoryMap* mem);
        bool hasMemoryMap() const;

        // Status
        const uint8_t getStatus() const;
        void setStatusBit(STATUS_BIT bit, bool enabled);
        void setStatusByte(uint8_t status_byte);
        const uint8_t getStatusBit(STATUS_BIT bit) const;
        static const char* getStatusBitString(STATUS_BIT status_bit);

        // OpCodes
        uint16_t decodeAddress(uint16_t& addr, ADDRESS_MODE address_mode, int& extra_cycles);
        uint16_t getOperandValue(ADDRESS_MODE address_mode);
        bool setMemory(uint16_t addr, uint8_t val);

    private:

        unsigned int m_CyclesToProcess;

        // Registers
        uint8_t m_ACC;
        uint8_t m_RX;
        uint8_t m_RY;
        uint16_t m_PC;
        uint8_t m_StackPtr;
        uint8_t m_Status;

        // Memory
        MemoryMap* m_MemoryMaps;
    };
}

#endif