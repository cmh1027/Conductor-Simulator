#include <QString>
#include <QTimer>
#include <cmath>
#include "conduct.h"
#include "xmlReader/xmlreader.h"
#include "module/synctimer.h"
#include "module/countdowntimer.h"
#include "tracker/tracker.h"

const int LastInterval = 3000;
const int MaxEnergy = 100;
const QMap<Difficulty, int> DifInterval = {{Difficulty::Easy, 600}, {Difficulty::Normal, 300}, {Difficulty::Hard, 100}};
const QMap<Dynamic, int> minVerticalDistance = {{Dynamic::pp, 70}, {Dynamic::p, 140}, {Dynamic::f, 210}, {Dynamic::ff, 280}};
const QMap<Dynamic, int> maxVerticalDistance = {{Dynamic::pp, 200}, {Dynamic::p, 270}, {Dynamic::f, 340}, {Dynamic::ff, 480}};
const QMap<Dynamic, int> minHorizontalDistance = {{Dynamic::pp, 70}, {Dynamic::p, 180}, {Dynamic::f, 290}, {Dynamic::ff, 400}};
const QMap<Dynamic, int> maxHorizontalDistance = {{Dynamic::pp, 260}, {Dynamic::p, 370}, {Dynamic::f, 480}, {Dynamic::ff, 640}};


ConductSimulator::ConductSimulator() : tracker(nullptr), lastTimer(new SyncTimer(TickInterval)),
    startTimer(new CountDownTimer()), score(0), energy(0), isPlaying(false), isPause(false), dynamic(None){
    this->setDifficulty(Difficulty::Normal);
    xmlReader = new XMLReader(this->interval);
    connect(xmlReader, &XMLReader::tickSignal, this, [=]{
        emit this->tickSignal();
        this->lastTimer->tick();
        for(auto it = waitingCommands.begin(); it != waitingCommands.end(); ++it){
            for(auto it2 = (*it)->begin(); it2 != (*it)->end(); ++it2){
                if((*it2) != nullptr)
                    (*it2)->tick();
            }
        }
    });
    connect(xmlReader, &XMLReader::infoSignal, this, &ConductSimulator::infoSignal);
    connect(xmlReader, &XMLReader::warningSignal, this, [=](const QString& str){
        emit this->infoSignal(QString("Warning : %1").arg(str));
    });
    connect(xmlReader, &XMLReader::errorSignal, this, [=](const QString& str){
        emit this->infoSignal(QString("Error : %1").arg(str));
    });
    connect(xmlReader, &XMLReader::commandSignal, this, &ConductSimulator::addCommand);
    connect(xmlReader, &XMLReader::dynamicSignal, this, &ConductSimulator::setDynamic);
    connect(xmlReader, &XMLReader::clearSignal, this, &ConductSimulator::initSignal);
    connect(xmlReader, &XMLReader::endSignal, this, [=]{
        lastTimer->setTime(this->interval * 2 + LastInterval);
        lastTimer->rewind();
    });
    connect(lastTimer, &SyncTimer::timeout, this, [=]{
        this->isPlaying = false;
        this->isPause = false;
        xmlReader->stop();
        emit this->endSignal();
    });
    connect(startTimer, &CountDownTimer::tickSignal, this, [=](int tick){
        emit this->infoSignal(QString::number(tick));
    });
    connect(startTimer, &CountDownTimer::timeout, this, [=]{
        xmlReader->start();
        emit this->infoSignal("Start");
    });
}

ConductSimulator::~ConductSimulator(){
    delete xmlReader;
    this->clearCommands();
    delete lastTimer;
    delete startTimer;
}

void ConductSimulator::loadXML(const QString& path){
    if(this->xmlReader->loadXML(path)){
        auto timers = this->xmlReader->getTimers();
        emit this->scoreSignal(timers, this->interval, LastInterval);
    }
}

bool ConductSimulator::isPausing() const{
    return this->isPause;
}

void ConductSimulator::applyTracker(Tracker* tracker){
    Q_ASSERT(this->tracker == nullptr);
    this->tracker = tracker;
    connect(tracker, QOverload<QString>::of(&Tracker::commandSignal), this, [=](QString command){
       this->removeCommand(command);
    });
    connect(tracker, QOverload<QString, int>::of(&Tracker::commandSignal), this, [=](QString command, int distance){
       this->removeCommand(command, distance);
    });
}

void ConductSimulator::setDifficulty(Difficulty difficulty){
    this->difficulty = difficulty;
    this->interval = DifInterval.value(this->difficulty);
}


void ConductSimulator::start(){
    tracker->start();
}

void ConductSimulator::gameStart(){
    if(this->isPlaying){
        emit this->infoSignal("The game is playing");
        return;
    }
    if(!xmlReader->isXMLLoaded()){
        emit this->infoSignal("XML file is not loaded");
        return;
    }
    this->dynamic = None;
    this->setScore(0);
    this->setEnergy(MaxEnergy);
    this->clearCommands();
    this->isPlaying = true;
    this->startTimer->start(3);
}

void ConductSimulator::pause(){
    if(!this->isPlaying){
        emit this->infoSignal("The game is not playing");
        return;
    }
    if(this->isPause)
        return;
    if(this->startTimer->isTriggered())
        this->startTimer->pause();
    else
        xmlReader->pause();
    this->isPause = true;
    emit this->infoSignal("Pause");
}

void ConductSimulator::resume(){

    if(this->startTimer->isPause()){
        this->startTimer->resume();
    }
    else{
        xmlReader->resume();
    }
    this->isPause = false;
    emit this->infoSignal("Resume");
}

void ConductSimulator::clear(){
    Q_ASSERT(tracker != nullptr);
    this->isPlaying = false;
    this->isPause = false;
    this->clearCommands();
    tracker->stop();
    xmlReader->clear();
}

void ConductSimulator::gameover(){
    this->isPlaying = false;
    this->isPause = false;
    xmlReader->stop();
    emit this->gameoverSignal();
}

void ConductSimulator::clearCommands(){
    mutex.lock();
    for(auto it = this->waitingCommands.begin(); it != this->waitingCommands.end(); ++it){
        for(auto it2 = (*it)->begin(); it2 != (*it)->end(); ++it2){
            delete (*it2);
        }
        delete (*it);
    }
    this->waitingCommands.clear();
    mutex.unlock();
}

void ConductSimulator::addCommand(const QString& command){
    mutex.lock();
    if(!this->waitingCommands.contains(command)){
        this->waitingCommands.insert(command, new QQueue<SyncTimer*>());
    }
    SyncTimer* timer = new SyncTimer(this->interval * 2, TickInterval);
    connect(timer, &SyncTimer::timeout, this, [=]{
        if(this->waitingCommands.value(command)->contains(timer)){
            this->waitingCommands.value(command)->removeOne(timer);
            if(timer->tryLock()){
                if(Beats.contains(command))
                    this->beatFail();
                else
                    this->commandFail();
                timer->deleteLater();
            }
        }
    });
    this->waitingCommands.value(command)->enqueue(timer);
    timer->rewind();
    mutex.unlock();
}

void ConductSimulator::setDynamic(const QString& dynamic){
    if(dynamic == Command::ff)
        this->dynamic = ff;
    else if(dynamic == Command::f)
        this->dynamic = f;
    else if(dynamic == Command::p)
        this->dynamic = p;
    else if(dynamic == Command::pp)
        this->dynamic = pp;
    else
        this->dynamic = None;
    xmlReader->setDynamic(this->dynamic);
}

void ConductSimulator::removeCommand(const QString& command){
    mutex.lock();
    if(this->waitingCommands.contains(command)){
        if(this->waitingCommands.value(command)->count() > 0){
            auto timer = this->waitingCommands.value(command)->dequeue();
            if(timer->tryLock()){
                this->commandSuccess(timer->remainingTime());
                timer->deleteLater();
            }
        }
    }
    mutex.unlock();
}

void ConductSimulator::removeCommand(const QString& command, int distance){
    mutex.lock();
    if(this->waitingCommands.contains(command)){
        if(this->waitingCommands.value(command)->count() > 0){
            auto timer = this->waitingCommands.value(command)->dequeue();
            if(timer->tryLock()){
                this->commandSuccess(timer->remainingTime());
                timer->deleteLater();
                if(command == Command::Vertical)
                    this->checkVerticalDynamic(distance);
                else if(command == Command::Horizontal)
                    this->checkHorizontalDynamic(distance);
            }
        }
    }
    mutex.unlock();
}

void ConductSimulator::commandSuccess(int remaining){
    double precision = static_cast<double>(std::abs(this->interval - remaining)) / this->interval;
    if(precision < 0.2){
        this->addScore(50);
        emit this->commandSignal(Precision::Perfect);
    }
    else if(precision < 0.45){
        this->addScore(35);
        emit this->commandSignal(Precision::Excellent);
    }
    else if(precision < 0.7){
        this->addScore(20);
        emit this->commandSignal(Precision::Good);
    }
    else{
        emit this->commandSignal(Precision::Bad);
    }
    this->addEnergy(1);
}

void ConductSimulator::beatFail(){
    this->addScore(-100);
    this->addEnergy(-10);
    emit this->commandSignal(Precision::Fail);
    emit this->dynamicSignal(Precision::Fail);
}

void ConductSimulator::commandFail(){
    this->addScore(-50);
    this->addEnergy(-7);
    emit this->commandSignal(Precision::Fail);
}

void ConductSimulator::checkDynamic(int distance, int min, int max){
    int error = std::min(std::abs(distance - min), std::abs(distance - max));
    double precision = static_cast<double>(error) / (max - min);
    if(min <= distance && distance <= max){
        this->addScore(50);
        emit this->dynamicSignal(Precision::Perfect);
    }
    else if(precision < 0.3){
        this->addScore(30);
        emit this->dynamicSignal(Precision::Excellent);
    }
    else if(precision < 0.6){
        this->addScore(10);
        emit this->dynamicSignal(Precision::Good);
    }
    else if(precision < 0.9){
        emit this->dynamicSignal(Precision::Bad);
    }
    else{
        this->addScore(-50);
        this->addEnergy(-1);
        emit this->dynamicSignal(Precision::Fail);
    }
}

void ConductSimulator::checkVerticalDynamic(int distance){
    if(this->dynamic == Dynamic::None)
        return;
    this->checkDynamic(distance, minVerticalDistance[this->dynamic], maxVerticalDistance[this->dynamic]);
    xmlReader->setDynamic(this->dynamicByVertical(distance));
}

void ConductSimulator::checkHorizontalDynamic(int distance){
    if(this->dynamic == Dynamic::None)
        return;
    this->checkDynamic(distance, minHorizontalDistance[this->dynamic], maxHorizontalDistance[this->dynamic]);
    xmlReader->setDynamic(this->dynamicByHorizontal(distance));
}

Dynamic ConductSimulator::dynamicByVertical(int distance){
   if(minVerticalDistance[Dynamic::pp] <= distance && distance <= maxVerticalDistance[Dynamic::pp])
       return Dynamic::pp;
   else if(minVerticalDistance[Dynamic::p] <= distance && distance <= maxVerticalDistance[Dynamic::p])
       return Dynamic::p;
   else if(minVerticalDistance[Dynamic::f] <= distance && distance <= maxVerticalDistance[Dynamic::f])
       return Dynamic::f;
   else
       return Dynamic::ff;
}

Dynamic ConductSimulator::dynamicByHorizontal(int distance){
    if(minHorizontalDistance[Dynamic::pp] <= distance && distance <= maxHorizontalDistance[Dynamic::pp])
        return Dynamic::pp;
    else if(minHorizontalDistance[Dynamic::p] <= distance && distance <= maxHorizontalDistance[Dynamic::p])
        return Dynamic::p;
    else if(minHorizontalDistance[Dynamic::f] <= distance && distance <= maxHorizontalDistance[Dynamic::f])
        return Dynamic::f;
    else
        return Dynamic::ff;
}

void ConductSimulator::setScore(int score){
    this->score = score;
    emit this->scoreChangedSignal(this->score);
}

void ConductSimulator::setEnergy(int energy){
    this->energy = energy;
    emit this->energyChangedSignal(this->energy);
}

void ConductSimulator::addEnergy(int energy){
    if(!this->isPlaying)
        return;
    bool gameover = false;
    this->energy += energy;
    if(this->energy > MaxEnergy)
        this->energy = MaxEnergy;
    else if(this->energy <= 0){
        gameover = true;
        this->energy = 0;
    }
    emit this->energyChangedSignal(this->energy);
    if(gameover)
        this->gameover();
}

void ConductSimulator::addScore(int score){
    this->score += score;
    emit this->scoreChangedSignal(this->score);
}

