#include "nes.h"

#include "nesdefs.h"
#include "cart.h"
#include "memorymap.h"
#include "submemorymap.h"

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
    m_CartBank = m_MemoryMaps->addSubMap(0x8000, 0x8000);
    


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

        const std::size_t size_16k = (1024 * 16);
        int prg_rom_chunks = m_Cart->getPRGROMSize() / size_16k;
        for (int i = 0; i < prg_rom_chunks; i++)
        {
            int bank32 = int(i / 2);
            std::size_t offset = 0;
            bool mirror = false;
            // 32k bank
            if (i % 2 == 0)
            {
                m_CartBank->addBank();
                m_CartBank->selectBank(i + 1);
                mirror = (i == prg_rom_chunks - 1);
            }
            else
            {
                offset = 0x4000;
            }

            for (auto b = 0; b < size_16k; b++)
            {
                m_CartBank->set(b + offset, m_Cart->data[m_Cart->prg_rom_offset + (i * size_16k) + b]);
                if (mirror)
                {
                    m_CartBank->set(b + 0x4000, m_Cart->data[m_Cart->prg_rom_offset + (i * size_16k) + b]);
                }
            }
        }
        m_CartBank->selectBank(1);
        return true;
    }
    return false;
}

bool NES::Console::unloadCart()
{
    while(m_CartBank->getBanks() > 1)
    {
        m_CartBank->deleteBank();
    }

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
    return (m_MemoryMaps->get(NES_NMI_ADDR)) | (m_MemoryMaps->get(NES_NMI_ADDR + 1) << 8);
}

uint16_t NES::Console::getResetVector()
{
    return (m_MemoryMaps->get(NES_RES_ADDR)) | (m_MemoryMaps->get(NES_RES_ADDR + 1) << 8);
}

uint16_t NES::Console::getIRQVector()
{
    return (m_MemoryMaps->get(NES_IRQ_ADDR )) | (m_MemoryMaps->get(NES_IRQ_ADDR + 1) << 8);
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
