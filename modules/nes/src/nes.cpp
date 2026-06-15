#include "nes.h"

#include "nesdefs.h"
#include "cart.h"
#include "memorymap.h"

// debug
#include <iostream>

NES::Console::Console():
    m_Clock(NES_CLOCK_HZ),
    m_Cart(nullptr)
{
    // Create Memory Map
    m_MemoryMaps = new MemoryMap(1024 * 64);

    // Init System Ram Mirrors
    m_MemoryMaps->addMirror(0x0000, 0x0800, 0x0800);
    m_MemoryMaps->addMirror(0x0000, 0x1000, 0x0800);
    m_MemoryMaps->addMirror(0x0000, 0x1800, 0x0800);

    // Other Memory
    // 0x6000 - 0x7fff = NV RAM / Battery Back RAM
    // 0x7000 - 0x71ff = Trainer
    // 0x8000 - 0xffff = Cartridge Data
    // 0xfffc = Reset Vector

    // PRG-ROM Mapping
    m_MemoryMaps->addSubMap(0x8000, 0x8000);
    


    // Assign Memory Map to CPU
    m_CPU.setMemoryMap(m_MemoryMaps);


    // Set Clock Tick Callback
    m_Clock.onTickCallback = [&]() {onTick(); };    
}

NES::Console::~Console()
{
    off();
}

void NES::Console::on()
{
    reset();
}

void NES::Console::off()
{
    m_Clock.stop();
}

void NES::Console::reset()
{
    m_Clock.stop();
    m_WaitCycles = 0;
    m_CPU.setPC(getResetVector());
    m_CPU.resetStack();
    m_Clock.start();   
}

bool NES::Console::isOn()
{
    return m_Clock.isRunning();
}

bool NES::Console::loadCart(NES::Cart * cart)
{
    if (cart)
    {
        unloadCart();
        m_Cart = cart;

        return true;
    }
    return false;
}

bool NES::Console::unloadCart()
{
    if (m_Cart)
    {
        delete m_Cart;
        m_Cart = nullptr;
        return true;
    }
    return false;
}

const NES::Cart* NES::Console::getCart()
{
    return m_Cart;
}

uint16_t NES::Console::getNMIVector()
{
    return (m_MemoryMaps->get(NES_NMI_ADDR + 1) << 8) | m_MemoryMaps->get(NES_NMI_ADDR + 1);
}

uint16_t NES::Console::getResetVector()
{
    return (m_MemoryMaps->get(NES_RES_ADDR + 1) << 8) | m_MemoryMaps->get(NES_RES_ADDR + 1);
}

uint16_t NES::Console::getIRQVector()
{
    return (m_MemoryMaps->get(NES_IRQ_ADDR + 1) << 8) | m_MemoryMaps->get(NES_IRQ_ADDR + 1);
}

void NES::Console::onTick()
{
    if (m_WaitCycles)
    {
        m_WaitCycles--;
    }
    else
    {
        m_WaitCycles = m_CPU.execute() - 1;
    }
}
