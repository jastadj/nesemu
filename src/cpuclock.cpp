#include "cpuclock.h"

// DEBUG
#include <iostream>
#include <iomanip>

CPUClock::CPUClock(unsigned int clock_speed_hz):
    m_Ticks(0),
    m_ClockThread(nullptr),
    m_ClockState(STATE::NONE),
    m_BatchesPerSec(20)
{
    m_ClockSpeedHz = (clock_speed_hz == 0 ? 1 : clock_speed_hz);

    // Configure Cycle Batching
    if ( (1e9/m_ClockSpeedHz) > (1e9 / m_BatchesPerSec))
    {
        m_BatchesPerSec = m_ClockSpeedHz;
        m_CyclesPerBatch = 1;
    }
    else
    {
        m_CyclesPerBatch = 1e9 / m_BatchesPerSec / (1e9 / m_ClockSpeedHz);
    }
}

CPUClock::~CPUClock()
{
    if (isRunning())
    {
        stop();
    }
}

const unsigned int CPUClock::getClockSpeed() const
{
    return m_ClockSpeedHz;
}

const unsigned long long CPUClock::getTicks()
{
    unsigned long long result;
    m_ClockMutex.lock();
    result = m_Ticks;
    m_ClockMutex.unlock();
    return result;
}

bool CPUClock::start()
{
    bool result = false;
    m_ClockMutex.lock();
    if (m_ClockState != STATE::RUNNING)
    {
        m_ClockState = STATE::RUNNING;
        m_ClockThread = new std::thread(&CPUClock::clockLoop, this);
        result = true;
    }
    m_ClockMutex.unlock();
    return result;
}

bool CPUClock::stop()
{
    bool result = false;
    STATE state;
    m_ClockMutex.lock();
    state = m_ClockState;
    m_ClockMutex.unlock();
    if (state == STATE::RUNNING)
    {
        m_ClockMutex.lock();
        m_ClockState = STATE::STOPPED;
        m_ClockMutex.unlock();

        m_ClockThread->join();
        m_ClockMutex.lock();
        delete m_ClockThread;
        m_ClockThread = nullptr;
        m_ClockMutex.unlock();
        result = true;
    }
    return result;
}

bool CPUClock::isRunning()
{
    bool result;
    m_ClockMutex.lock();
    result = m_ClockState == STATE::RUNNING;
    m_ClockMutex.unlock();
    return result;
}

const unsigned int CPUClock::getBatchesPerSec() const
{
    return m_BatchesPerSec;
}

const unsigned int CPUClock::getCyclesPerBatch() const
{
    return m_CyclesPerBatch;
}

void CPUClock::clockLoop()
{
    bool quit = false;
    int i = 0;

    std::chrono::high_resolution_clock::time_point batch_start;
    std::chrono::high_resolution_clock::time_point batch_end;

    while (!quit)
    {
        batch_start = std::chrono::high_resolution_clock::now();

        m_ClockMutex.lock();
        quit = m_ClockState != STATE::RUNNING;
        
        // Batch Cycles
        for (i = 0; i < m_CyclesPerBatch; i++)
        {
            // Do Tick
            m_Ticks++;
            onTick();
        }
        batch_end = std::chrono::high_resolution_clock::now();
        m_ClockMutex.unlock();

        // If batch finished faster than required, wait
        if ((batch_end - batch_start).count() < unsigned long(1e9 / m_BatchesPerSec))
        {
            std::this_thread::sleep_for(
                std::chrono::nanoseconds( (unsigned long long)(1e9 / double(m_BatchesPerSec)) - (batch_end - batch_start).count()));
        }
        
    }
}

