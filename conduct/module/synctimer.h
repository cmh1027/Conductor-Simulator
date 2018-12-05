#ifndef COMMANDTIMER_H
#define COMMANDTIMER_H
#include <QObject>
#include <QString>

namespace std{
    class mutex;
}

class SyncTimer : public QObject{
    Q_OBJECT
public:
    SyncTimer(int=0);
    SyncTimer(int, const QString&, int = 0);
    SyncTimer(int, int, const QString&, int = 0);
    virtual ~SyncTimer();
    QString command;
    void setTime(int);
    void setInterval(int);
    void setTickInterval(int);
    void refresh();
    int getInterval() const;
    int getTime() const;
    int getTickInterval() const;
    void rewind();
    void tick();
    int remainingTime() const;
    bool tryLock();

signals:
    void timeout();

private:
    std::mutex* mutex;
    int time;
    int interval;
    int timeoutInterval;
    int tickInterval;
    int remaining;
};



#endif // COMMANDTIMER_H
