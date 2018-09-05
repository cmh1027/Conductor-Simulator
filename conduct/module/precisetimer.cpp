#include <chrono>
#include "precisetimer.h"

const int MS2NS = 1000000;
const int INTERVAL_DELAY = 1;


ChronoTimer::ChronoTimer() : isRunning(false), isEnded(true){
    this->setInterval(1);
}

ChronoTimer::ChronoTimer(long long nanoseconds) : isRunning(false), isEnded(true){
    this->setInterval(nanoseconds);
}

void ChronoTimer::stop(){
    this->isRunning = false;
}

void ChronoTimer::setInterval(long long nanosecond){
    this->sleepDuration = nanoseconds((nanosecond - INTERVAL_DELAY >= 0) ? nanosecond - INTERVAL_DELAY : 0);
}

bool ChronoTimer::isActive() const{
    return !this->isEnded;
}

void ChronoTimer::run(){
    this->isRunning = true;
    this->isEnded = false;
    while(this->isRunning){
        sleep_for(this->sleepDuration);
        if(this->isRunning)
            emit this->timeout();
    }
    this->isEnded = true;
}


PreciseTimer::PreciseTimer() : interval(1), timer(new ChronoTimer(interval * MS2NS)){
    connect(timer, &ChronoTimer::timeout, this, &PreciseTimer::timeout);
}

PreciseTimer::PreciseTimer(long long milliseconds) : interval(milliseconds), timer(new ChronoTimer(interval * MS2NS)){
    connect(timer, &ChronoTimer::timeout, this, &PreciseTimer::timeout);
}

PreciseTimer::~PreciseTimer(){
    disconnect(timer, nullptr, this, nullptr);
    timer->stop();
    delete timer;
}

void PreciseTimer::start(){
    this->timer->stop();
    while(this->timer->isActive());
    this->timer->start();
}

void PreciseTimer::start(long long interval){
    this->timer->setInterval(interval * MS2NS);
    this->timer->stop();
    while(this->timer->isActive());
    this->timer->start();
}

void PreciseTimer::stop(){
    this->timer->stop();
    while(this->timer->isActive());
}

void PreciseTimer::setInterval(long long interval){
    Q_ASSERT(interval >= 100);
    timer->setInterval(interval * MS2NS);
}
