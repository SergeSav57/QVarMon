#include "dbgtimer.h"

DbgTimer::DbgTimer(QObject *parent) : QObject(parent)
{
}

void DbgTimer::dbgTimerStart()//DbgTimer* dbg_timer)
{
    start_time = steady_clock::now();
}

quint32 DbgTimer::dbgTimerDuration()//DbgTimer* dbg_timer)
{
    auto elapsed = duration_cast<microseconds>(steady_clock::now() - start_time);
    return elapsed.count();
}
