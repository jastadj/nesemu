#ifndef NES_H
#define NES_H

#include <mutex>

#include "cpu6502.h"
#include "cpu6502clock.h"

namespace NES
{
    class Cart;
    class PPU;

    class Console
    {
    public:
        Console();
        ~Console();

        Arch6502::CPU m_CPU;
        CPUClock6502 m_PPUClock;
        MemoryMap* m_MemoryMaps;

        // Power
        void on();
        void off();
        void reset();
        bool isOn();

        // Carts
        bool loadCart(Cart* cart);
        bool unloadCart();
        const Cart* getCart();

        // Vectors
        uint16_t getNMIVector();
        uint16_t getResetVector();
        uint16_t getIRQVector();

        // Info
        unsigned long long getTicks();
        static unsigned long long getCPUClockHz();
        static unsigned long long getPPUClockHz();

        // Debug
        void setDebugBreak(bool enabled);
        bool getDebugBreak();

    private:
        
        // Cycles
        unsigned int m_CPUWaitCycles;
        void onTick();
        std::mutex m_TickMutex;
        unsigned long long m_Ticks;
        bool m_DebugBreak;

        // Cart
        Cart* m_Cart;
        SubMemoryMap* m_CartBank;

        // PPU
        PPU* m_PPU;
    };
}
#endif