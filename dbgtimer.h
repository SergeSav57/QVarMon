#ifndef DBGTIMER_H
#define DBGTIMER_H

#include <QObject>
#include <chrono>

using namespace std::chrono;

class DbgTimer : public QObject
{
    Q_OBJECT
public:
    explicit DbgTimer(QObject *parent = nullptr);

    // отладочный таймер -возвращает число мкс между началом и концом измерения
    //:> steady_clock – представляет так называемые устойчивые часы, то есть ход
    //:> которых не подвержен внешним изменениям.
    steady_clock::time_point start_time;// число мкс на старте измерения
    void dbgTimerStart();
    quint32 dbgTimerDuration();
};

#endif // DBGTIMER_H
