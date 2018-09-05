#ifndef PRECISETIMER_H
#define PRECISETIMER_H
#include <QObject>
#include <QThread>

extern const int MS2NS;
extern const int INTERVAL_DELAY;
using namespace std::this_thread;
using namespace std::chrono;

class ChronoTimer : public QThread{
    Q_OBJECT

public:
    ChronoTimer();
    ChronoTimer(long long);
    virtual ~ChronoTimer() override = default;
    void stop();
    void setInterval(long long);
    bool isActive() const;

private:
    void run() override;

signals:
    void timeout();

private:
    bool isRunning;
    bool isEnded;
    nanoseconds sleepDuration;
};


class PreciseTimer : public QObject{
    Q_OBJECT
public:
    PreciseTimer();
    PreciseTimer(long long);
    virtual ~PreciseTimer();
    void start();
    void start(long long);
    void stop();
    void setInterval(long long);

private:
    long long interval;
    ChronoTimer* timer;

signals:
    void timeout();
};

#endif // PRECISETIMER_H
