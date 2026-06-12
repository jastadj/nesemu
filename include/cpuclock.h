#ifndef CPUCLOCK_H
#define CPUCLOCK_H

#include <thread>
#include <mutex>

class CPUClock
{
public:
    CPUClock(unsigned int clock_speed_hz);
    ~CPUClock();

    const unsigned int getClockSpeed() const;
    const unsigned long long getTicks();

    virtual void onTick() = 0;

    bool start();
    bool stop();
    bool isRunning();

    const unsigned int getBatchesPerSec() const;
    const unsigned int getCyclesPerBatch() const;

protected:

    unsigned int m_ClockSpeedHz;
    unsigned long long m_Ticks;

    std::mutex m_ClockMutex;

    // Clock State
    enum STATE{NONE, RUNNING, STOPPED};
    STATE m_ClockState;

    // Clock Thread
    std::thread* m_ClockThread;
    bool m_ClockThreadQuit;
    void clockLoop();

    // Clock Batching
    unsigned int m_BatchesPerSec;
    unsigned int m_CyclesPerBatch;
    
};

#endif