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
        bool setAddr(uint16_t addr, uint8_t val, bool inc_cycles = true);
        uint8_t getAddr(uint16_t addr, bool inc_cycles = true, bool* ok = nullptr);
        bool addMirror(std::size_t source_addr, std::size_t dest_addr, std::size_t len);
        const std::size_t getMemSize() const;
        void fillMem(const uint8_t val);
        void fillMemRandom();

        // Status
        const uint8_t getStatus() const;
        void setStatusBit(STATUS_BIT bit, bool enabled);
        const uint8_t getStatusBit(STATUS_BIT bit) const;
        static const char* getStatusBitString(STATUS_BIT status_bit);

        // OpCodes
        uint16_t getOperand(ADDRESS_MODE address_mode);


    private:

        unsigned int m_CyclesToProcess;

        // Registers
        uint8_t m_ACC;
        uint8_t m_RX;
        uint8_t m_RY;
        uint16_t m_PC;
        uint16_t m_StackPtr;
        uint8_t m_Status;

        // Memory
        MemoryMap m_Mem;
    };
}

#endif