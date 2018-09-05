#ifndef COUNTDOWNTIMER_H
#define COUNTDOWNTIMER_H
#include <QObject>

class QTimer;


class CountDownTimer : public QObject{
    Q_OBJECT

public:
    CountDownTimer();
    virtual ~CountDownTimer();
    void start(int);
    void pause();
    void resume();
    bool isTriggered();
    bool isPause();

signals:
    void tickSignal(int);
    void timeout();

private:
    QTimer* timer;
    int tick;
    bool isRunning;
    bool isPausing;
};

#endif // COUNTDOWNTIMER_H
