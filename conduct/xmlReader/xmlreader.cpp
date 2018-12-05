#include <QSet>
#include <QtXml>
#include <QFileInfo>
#include "conduct/module/synctimer.h"
#include "conduct/command/command.h"
#include "conduct/module/random.h"
#include "xmlreader.h"

const int TickInterval = 100;

XMLReader::XMLReader(int interval) : interval(interval), timeoutCount(0), musicPlayer(new MusicPlayer(TickInterval)),
    volumeChangeable(false), tempoChangeable(false)
{
    if(this->interval < 0)
        this->interval = 0;
    connect(musicPlayer, &MusicPlayer::tickSignal, this, [=]{
        emit this->tickSignal();
        for(auto it = this->timers.begin(); it != this->timers.end(); ++it){
            (*it)->tick();
        }
    });
}

XMLReader::~XMLReader(){
    delete musicPlayer;
}

void XMLReader::setInterval(int interval){
    this->interval = interval;
    for(auto it = this->timers.begin(); it != this->timers.end(); ++it)
        (*it)->setInterval(interval);
}

bool XMLReader::isXMLLoaded(){
    return this->timers.count() > 0 && this->musicPlayer->count() > 0;
}

bool XMLReader::loadXML(const QString& fileName){
    if(fileName.isEmpty())
        return false;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)){
        emit this->errorSignal("Failed to open the xml file");
        return false;
    }
    this->clear();
    QDomDocument document;
    document.setContent(&file);
    file.close();
    if(!this->parseXML(document)){
        this->clear();
        emit this->errorSignal("Invalid XML file");
    }
    return true;
}


void XMLReader::start(){
    if(this->timers.empty() || this->musicPlayer->empty()){
        emit this->errorSignal("XML file is not loaded");
        return;
    }
    this->timeoutCount = 0;
    for(auto it = this->timers.begin(); it != this->timers.end(); ++it){
        (*it)->rewind();
    }
    this->musicPlayer->playAll();
}


void XMLReader::stop(){
    this->musicPlayer->stopAll();
}

void XMLReader::pause(){
    this->musicPlayer->pauseAll();
}

void XMLReader::resume(){
    this->musicPlayer->resumeAll();
}

void XMLReader::clear(){
    emit this->clearSignal();
    this->musicPlayer->clear();
    while(this->timers.count() > 0){
        auto timer = this->timers.dequeue();
        delete timer;
    }
}


bool XMLReader::parseXML(const QDomDocument& document){
    QDomElement root = document.documentElement();
    QDomElement component = root.firstChild().toElement();
    if(root.toElement().isNull() || root.tagName() != "conduct")
        return false;
    while(!component.isNull()){
        if(component.tagName() == "commands"){
            if(!this->parseCommand(component)){
                return false;
            }
        }
        else if(component.tagName() == "musics"){
            if(!this->parseMusic(component)){
                return false;
            }
        }
        else if(component.tagName() == "volume"){
            this->parseVolume(component);
        }
        else if(component.tagName() == "tempo"){
            this->parseTempo(component);
        }
        else if(component.tagName() == "group"){
            this->parseGroup(component);
        }
        component = component.nextSibling().toElement();
    }
    return true;
}


bool XMLReader::parseCommand(const QDomElement& dom){
    QDomElement&& child = dom.firstChild().toElement();
    int count = 0;
    while(!child.isNull()){
        if(child.tagName() != "command" || !child.hasAttribute("time") || !child.hasAttribute("action"))
            return false;
        bool isDouble;
        double time = child.attribute("time").toDouble(&isDouble);
        if(!isDouble)
            return false;
        QString&& action = child.attribute("action");
        if(Dynamics.contains(action))
            this->addDynamic(static_cast<int>(time * 1000), action);
        else if(Beats.contains(action) || Commands.contains(action))
            this->addCommand(static_cast<int>(time * 1000), action);
        else // group
            this->addGroup(static_cast<int>(time * 1000), action);
        child = child.nextSibling().toElement();
        ++count;
    }
    if(count == 0){
        emit this->errorSignal("No command is contained!");
        return false;
    }
    return true;
}


bool XMLReader::parseMusic(const QDomElement& dom){
    QDomElement&& child = dom.firstChild().toElement();
    int musicCount = 0;
    int mainCount = 0;
    while(!child.isNull()){
        if(child.text().isEmpty())
            return false;
        QString&& path = child.text();
        QFileInfo fileInfo(path);
        if(fileInfo.exists() && fileInfo.isFile()){
            emit this->infoSignal(QString("Load file %1").arg(path));
            bool isMain = child.attribute("main").toInt() != 0;
            int group = child.attribute("group").toInt();
            if(!(1 <= group && group <= GroupCount))
                group = 1;
            this->addMusic(path, group, isMain);
            if(isMain)
                ++mainCount;
            ++musicCount;
        }
        else{
            emit this->warningSignal(QString("file %1 is not found").arg(path));
        }
        child = child.nextSibling().toElement();
    }
    if(musicCount == 0){
        emit this->errorSignal("No sound file is contained!");
        return false;
    }
    if(mainCount == 0){
        emit this->warningSignal("No main music is specified. Set first one as it");
    }
    return true;
}

void XMLReader::parseVolume(const QDomElement& dom){
    if(dom.text() != "0")
        this->volumeChangeable = true;
    else
        this->volumeChangeable = false;
}

void XMLReader::parseTempo(const QDomElement& dom){
    if(dom.text() != "0")
        this->tempoChangeable = true;
    else
        this->tempoChangeable = false;
}

void XMLReader::parseGroup(const QDomElement& dom){
    if(dom.text() != "0")
        this->groupEnabled = true;
    else
        this->groupEnabled = false;
    emit this->groupEnabledSignal(this->groupEnabled);
}

void XMLReader::addCommand(int time, const QString& command){
    SyncTimer* timer = new SyncTimer(time, this->interval, command, TickInterval);
    connect(timer, &SyncTimer::timeout, this, [=](){
        emit this->commandSignal(timer->command);
        this->timeoutCount += 1;
        if(this->timeoutCount == this->timers.count()){
            emit this->endSignal();
        }
    });
    this->timers.enqueue(timer);
}

void XMLReader::addDynamic(int time, const QString& dynamic){
    SyncTimer* timer = new SyncTimer(time, this->interval, dynamic);
    connect(timer, &SyncTimer::timeout, this, [=](){
        emit this->dynamicSignal(timer->command);
        this->timeoutCount += 1;
        if(this->timeoutCount == this->timers.count()){
            emit this->endSignal();
        }
    });
    this->timers.enqueue(timer);
}

void XMLReader::addGroup(int time, const QString& group){
    SyncTimer* timer = new SyncTimer(time, this->interval, group);
    connect(timer, &SyncTimer::timeout, this, [=](){
        int groupNum = timer->command.toInt();
        if(!(0 <= groupNum && groupNum <= GroupCount))
            groupNum = 0;
        emit this->groupSignal(groupNum);
        this->timeoutCount += 1;
        if(this->timeoutCount == this->timers.count()){
            emit this->endSignal();
        }
    });
    this->timers.enqueue(timer);
}


void XMLReader::addMusic(const QString& path, int group, bool isMain){
    this->musicPlayer->addMusic(path, 50, group, isMain);
}

void XMLReader::setVolume(int volume, int group1, int group2){
    if(group1 == 0 || group2 == 0)
        this->musicPlayer->setVolumeAll(volume);
    else{
        this->musicPlayer->setVolumeGroup(group1-1, volume);
        this->musicPlayer->setVolumeGroup(group2-1, volume);
    }
}

void XMLReader::setDynamic(Dynamic dynamic, int group1, int group2){
    if(!this->volumeChangeable)
        return;
    switch(dynamic){
        case pp:
            this->setVolume(15, group1, group2);
            break;
        case p:
            this->setVolume(30, group1, group2);
            break;
        case mp:
            this->setVolume(45, group1, group2);
            break;
        case mf:
            this->setVolume(60, group1, group2);
            break;
        case f:
            this->setVolume(75, group1, group2);
            break;
        case ff:
            this->setVolume(90, group1, group2);
            break;
        default:
            this->setVolume(50, group1, group2);
            break;
    }
}

void XMLReader::randomizeDynamic(){
    if(!this->volumeChangeable)
        return;
    for(int index = 0; index < musicPlayer->count(); ++index)
        musicPlayer->setVolume(index, Random::pick(15, 90));
}

void XMLReader::randomizeSpeed(){
    if(!this->tempoChangeable)
        return;
    bool mainDone = false;
    for(int index = 0; index < musicPlayer->count(); ++index)
        musicPlayer->multSpeed(index, Random::pick(85, 115) / 100.0f, mainDone);
}

void XMLReader::randomizeSpeed(int group1, int group2){
    if(!this->tempoChangeable)
        return;
    if(group1 == 0 || group2 == 0)
        this->randomizeSpeed();
    else{
        musicPlayer->multSpeedGroup(group1-1, Random::pick(85, 115) / 100.0f);
        musicPlayer->multSpeedGroup(group2-1, Random::pick(85, 115) / 100.0f);
    }
}

void XMLReader::normalizeSpeed(){
    if(!this->tempoChangeable)
        return;
    musicPlayer->setSpeedAll(1.0f);
}

const QQueue<SyncTimer*>* XMLReader::getTimers(){
    return &this->timers;
}
