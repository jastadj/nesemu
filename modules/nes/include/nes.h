#ifndef NES_H
#define NES_H

#include "cpu6502.h"
#include "cpu6502clock.h"

namespace NES
{
    class Cart;

    class Console
    {
    public:
        Console();
        ~Console();

        Arch6502::CPU m_CPU;
        CPUClock6502 m_Clock;
        MemoryMap* m_MemoryMaps;

        void on();
        void off();
        void reset();
        bool isOn();

        bool loadCart(Cart* cart);
        bool unloadCart();
        const Cart* getCart();

        uint16_t getNMIVector();
        uint16_t getResetVector();
        uint16_t getIRQVector();

    private:
        unsigned int m_WaitCycles;
        void onTick();

        Cart* m_Cart;
        SubMemoryMap* m_CartBank;
    };
}
#endif