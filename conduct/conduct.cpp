#include <QString>
#include <QTimer>
#include <cmath>
#include "conduct.h"
#include "xmlReader/xmlreader.h"
#include "module/synctimer.h"
#include "module/countdowntimer.h"
#include "module/random.h"
#include "tracker/tracker.h"

const int LastInterval = 3000;
const int MaxEnergy = 100;
const QMap<Difficulty, int> DifInterval = {{Difficulty::Easy, 500}, {Difficulty::Normal, 400}, {Difficulty::Hard, 300}};
const QMap<Difficulty, double> DifProb = {{Difficulty::Easy, 0.1}, {Difficulty::Normal, 0.3}, {Difficulty::Hard, 0.5}};
const QMap<Difficulty, int> SpeedRollbackCount = {{Difficulty::Easy, 4}, {Difficulty::Normal, 7}, {Difficulty::Hard, 10}};
const QMap<Dynamic, int> MinimumVerticalDistance = {{Dynamic::pp, 70}, {Dynamic::p, 110}, {Dynamic::mp, 150}, {Dynamic::mf, 190}, {Dynamic::f, 230}, {Dynamic::ff, 270}};
const QMap<Dynamic, int> MaximumVerticalDistance = {{Dynamic::pp, 150}, {Dynamic::p, 190}, {Dynamic::mp, 230}, {Dynamic::mf, 270}, {Dynamic::f, 310}, {Dynamic::ff, 480}};
const QMap<Dynamic, int> MinimumHorizontalDistance = {{Dynamic::pp, 70}, {Dynamic::p, 135}, {Dynamic::mp, 200}, {Dynamic::mf, 265}, {Dynamic::f, 330}, {Dynamic::ff, 400}};
const QMap<Dynamic, int> MaximumHorizontalDistance = {{Dynamic::pp, 170}, {Dynamic::p, 235}, {Dynamic::mp, 300}, {Dynamic::mf, 365}, {Dynamic::f, 430}, {Dynamic::ff, 640}};


ConductSimulator::ConductSimulator() : tracker(nullptr), lastTimer(new SyncTimer(TickInterval)),
    startTimer(new CountDownTimer()), score(0), energy(0), isPlaying(false), isPause(false),
    groupEnabled(false), dynamic(None){
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
    connect(xmlReader, &XMLReader::groupSignal, this, &ConductSimulator::setGroup);
    connect(xmlReader, &XMLReader::clearSignal, this, &ConductSimulator::initSignal);
    connect(xmlReader, &XMLReader::endSignal, this, [=]{
        lastTimer->setTime(this->interval * 2 + LastInterval);
        lastTimer->rewind();
    });
    connect(xmlReader, &XMLReader::groupEnabledSignal, this, [=](bool isEnabled){
        this->groupEnabled = isEnabled;
        tracker->groupEnabled = isEnabled;
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
        emit this->makeSheetSignal(timers, this->interval, LastInterval);
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

bool ConductSimulator::gameStart(){
    if(this->isPlaying){
        emit this->infoSignal("The game is playing");
        return false;
    }
    if(!xmlReader->isXMLLoaded()){
        emit this->infoSignal("XML file is not loaded");
        return false;
    }
    this->dynamic = None;
    this->currentGroup = 0;
    if(this->groupEnabled)
        this->tracker->setCurrentGroup(this->currentGroup);
    else
        this->tracker->setCurrentGroup(-1);
    this->setScore(0);
    this->setEnergy(MaxEnergy);
    this->clearCommands();
    this->isPlaying = true;
    this->successCount = 0;
    this->isNormalSpeed = true;
    this->startTimer->start(3);
    return true;
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

void ConductSimulator::addCommand(QString command){
    mutex.lock();
    if(!this->waitingCommands.contains(command)){
        this->waitingCommands.insert(command, new QQueue<SyncTimer*>());
    }
    SyncTimer* timer = new SyncTimer(this->interval * 2, command, TickInterval);
    connect(timer, &SyncTimer::timeout, this, [=]{
        if(this->waitingCommands.value(command)->contains(timer)){
            this->waitingCommands.value(command)->removeOne(timer);
            if(timer->tryLock()){
                if(Beats.contains(command)){
                    this->beatFail();
                    this->commandFail();
                }
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
    if(DynamicMap.contains(dynamic))
        this->dynamic = DynamicMap[dynamic];
    else
        this->dynamic = None;
    xmlReader->setDynamic(this->dynamic);
}

void ConductSimulator::setGroup(int group){
    this->currentGroup = group;
    if(this->groupEnabled)
        this->tracker->setCurrentGroup(group);
}

void ConductSimulator::removeCommand(const QString& command){
    mutex.lock();
    if(this->waitingCommands.contains(command)){
        if(this->waitingCommands.value(command)->count() > 0){
            auto timer = this->waitingCommands.value(command)->dequeue();
            if(timer->tryLock()){
                if((this->groupEnabled && this->currentGroup == tracker->eyePosition) || !this->groupEnabled)
                    this->commandSuccess(timer->remainingTime());
                else
                    this->commandFail(this->currentGroup, tracker->eyePosition);
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
                if((this->groupEnabled && this->currentGroup == tracker->eyePosition) || !this->groupEnabled)
                    this->commandSuccess(timer->remainingTime());
                else
                    this->commandFail(this->currentGroup, tracker->eyePosition);
                if(command == Command::Vertical)
                    this->checkVerticalDynamic(distance);
                else if(command == Command::Horizontal)
                    this->checkHorizontalDynamic(distance);
                timer->deleteLater();
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
        if(this->successCount > 0)
            this->successCount -= 1;
    }
    if(precision < 0.7 && !this->isNormalSpeed){
        this->successCount += 1;
        if(this->successCount == SpeedRollbackCount[this->difficulty]){
            this->successCount = 0;
            xmlReader->normalizeSpeed();
            this->isNormalSpeed = true;
        }
    }
    this->addEnergy(1);
}

void ConductSimulator::beatFail(){
    this->addScore(-100);
    this->addEnergy(-10);
    emit this->dynamicSignal(Precision::Fail);
    if(Random::percent(DifProb[this->difficulty]))
        xmlReader->randomizeDynamic();
}

void ConductSimulator::commandFail(){
    this->addScore(-50);
    this->addEnergy(-7);
    emit this->commandSignal(Precision::Fail);
    this->successCount = 0;
    if(Random::percent(DifProb[this->difficulty])){
        xmlReader->randomizeSpeed();
        this->isNormalSpeed = false;
    }
}


void ConductSimulator::commandFail(int originGroup, int currentGroup){
    this->addScore(-50);
    this->addEnergy(-7);
    emit this->commandSignal(Precision::Fail);
    if(Random::percent(DifProb[this->difficulty])){
        xmlReader->randomizeSpeed(originGroup, currentGroup);
        this->isNormalSpeed = false;
    }
}


bool ConductSimulator::checkDynamic(int distance, int min, int max){
    int error = std::min(std::abs(distance - min), std::abs(distance - max));
    double precision = static_cast<double>(error) / (max - min);
    if(min <= distance && distance <= max){
        this->addScore(50);
        emit this->dynamicSignal(Precision::Perfect);
        return true;
    }
    else if(precision < 0.3){
        this->addScore(30);
        emit this->dynamicSignal(Precision::Excellent);
        return true;
    }
    else if(precision < 0.6){
        this->addScore(10);
        emit this->dynamicSignal(Precision::Good);
        return true;
    }
    else if(precision < 0.9){
        emit this->dynamicSignal(Precision::Bad);
        return false;
    }
    else{
        this->addScore(-50);
        this->addEnergy(-1);
        emit this->dynamicSignal(Precision::Fail);
        return false;
    }
}

void ConductSimulator::checkVerticalDynamic(int distance){
    if(this->dynamic == Dynamic::None)
        return;
    int group1 = 0;
    int group2 = 0;
    if(this->groupEnabled && this->currentGroup != tracker->eyePosition){
        group1 = this->currentGroup;
        group2 = tracker->eyePosition;
    }
    if(this->checkDynamic(distance, MinimumVerticalDistance[this->dynamic], MaximumVerticalDistance[this->dynamic]))
        xmlReader->setDynamic(this->dynamic, group1, group2);
    else{
        if(Random::percent(DifProb[this->difficulty]))
            xmlReader->setDynamic(this->dynamicByVertical(distance), group1, group2);
    }
}

void ConductSimulator::checkHorizontalDynamic(int distance){
    if(this->dynamic == Dynamic::None)
        return;
    int group1 = 0;
    int group2 = 0;
    if(this->groupEnabled && this->currentGroup != tracker->eyePosition){
        group1 = this->currentGroup;
        group2 = tracker->eyePosition;
    }
    if(this->checkDynamic(distance, MinimumHorizontalDistance[this->dynamic], MaximumHorizontalDistance[this->dynamic]))
        xmlReader->setDynamic(this->dynamic, group1, group2);
    else{
        if(Random::percent(DifProb[this->difficulty]))
            xmlReader->setDynamic(this->dynamicByHorizontal(distance), group1, group2);
    }
}

Dynamic ConductSimulator::dynamicByVertical(int distance){
    foreach(Dynamic dynamic, DynamicMap){
        if(MinimumVerticalDistance[dynamic] <= distance && distance <= MaximumVerticalDistance[dynamic])
            return dynamic;
    }
    return Dynamic::None;
}

Dynamic ConductSimulator::dynamicByHorizontal(int distance){
    foreach(Dynamic dynamic, DynamicMap){
        if(MinimumHorizontalDistance[dynamic] <= distance && distance <= MaximumHorizontalDistance[dynamic])
            return dynamic;
    }
    return Dynamic::None;
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

