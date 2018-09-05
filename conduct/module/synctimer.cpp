#include <mutex>
#include "synctimer.h"
SyncTimer::SyncTimer(int tickInterval) : mutex(new std::mutex()), time(0), interval(0),
    timeoutInterval(-1), tickInterval(tickInterval), remaining(-1){

}


SyncTimer::SyncTimer(int time, int tickInterval, const QString& command) : command(command), mutex(new std::mutex()),
    time(time), interval(0), tickInterval(tickInterval)
{
    Q_ASSERT(time >= 0);
    this->timeoutInterval = (time < 0) ? 0 : time;
    this->remaining = this->timeoutInterval;
}

SyncTimer::SyncTimer(int time, int interval, int tickInterval, const QString& command) : command(command), mutex(new std::mutex()),
    time(time), interval(interval), tickInterval(tickInterval)
{
    Q_ASSERT(time >= 0);
    Q_ASSERT(interval >= 0);
    this->timeoutInterval = (time - interval < 0) ? 0 : time - interval;
    this->remaining = this->timeoutInterval;
}

SyncTimer::~SyncTimer(){
    this->mutex->unlock();
    delete this->mutex;
}

void SyncTimer::setTime(int time){
    Q_ASSERT(time >= 0);
    this->time = time;
    this->refresh();
}

void SyncTimer::setInterval(int i){
    Q_ASSERT(i >= 0);
    this->interval = i;
    this->refresh();
}

void SyncTimer::setTickInterval(int i){
    Q_ASSERT(i > 0);
    this->tickInterval = i;
}

void SyncTimer::refresh(){
    this->timeoutInterval = (time - interval < 0) ? 0 : time - interval;
}

int SyncTimer::getInterval() const{
    return this->interval;
}

int SyncTimer::getTime() const{
    return this->time;
}

int SyncTimer::getTickInterval() const{
    return this->tickInterval;
}

void SyncTimer::rewind(){
    this->remaining = this->timeoutInterval;
}

void SyncTimer::tick(){
    if(this->timeoutInterval == 0 && this->remaining == 0){ // immediate
        emit this->timeout();
        this->remaining = -1;
    }
    if(this->remaining > 0){
        this->remaining -= this->tickInterval;
        if(this->remaining <= 0)
            emit this->timeout();
    }
}

int SyncTimer::remainingTime() const{
    return (this->remaining < 0) ? 0 : this->remaining;
}

bool SyncTimer::tryLock(){
    return this->mutex->try_lock();
}
