#include <QTimer>
#include "countdowntimer.h"


CountDownTimer::CountDownTimer() : timer(new QTimer()), isRunning(false), isPausing(false){
    connect(timer, &QTimer::timeout, this, [=]{
        this->tick -= 1;
        if(this->tick <= 0){
            emit this->timeout();
            this->isRunning = false;
            this->isPausing = false;
            timer->stop();
        }
        else{
            emit this->tickSignal(this->tick);
            timer->start(1000);
        }
    });
}

CountDownTimer::~CountDownTimer(){
    disconnect(this->timer, nullptr, nullptr, nullptr);
    delete this->timer;
}

void CountDownTimer::start(int tick){
    if(this->isRunning)
        return;
    this->tick = tick;
    this->isRunning = true;
    this->isPausing = false;
    this->tickSignal(this->tick);
    this->timer->start(1000);
}

void CountDownTimer::pause(){
    if(!this->isRunning || this->isPausing)
        return;
    this->timer->stop();
    this->isPausing = true;
}

void CountDownTimer::resume(){
    if(!this->isPausing)
        return;
    timer->start(1000);
    this->isPausing = false;
}

bool CountDownTimer::isTriggered(){
    return this->isRunning;
}

bool CountDownTimer::isPause(){
    return this->isPausing;
}

