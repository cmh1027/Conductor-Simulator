#include <QSet>
#include <QtXml>
#include <QFileInfo>
#include "conduct/module/synctimer.h"
#include "conduct/module/precisetimer.h"
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
            if(!this->parseVolume(component)){
                return false;
            }
        }
        else if(component.tagName() == "tempo"){
            if(!this->parseTempo(component)){
                return false;
            }
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
        else
            this->addCommand(static_cast<int>(time * 1000), action);
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
            this->addMusic(path, isMain);
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

bool XMLReader::parseVolume(const QDomElement& dom){
    if(dom.text() != "0")
        this->volumeChangeable = true;
    else
        this->volumeChangeable = false;
    return true;
}

bool XMLReader::parseTempo(const QDomElement& dom){
    if(dom.text() != "0")
        this->tempoChangeable = true;
    else
        this->tempoChangeable = false;
    return true;
}

void XMLReader::addCommand(int time, const QString& command){
    SyncTimer* timer = new SyncTimer(time, this->interval, TickInterval, command);
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
    SyncTimer* timer = new SyncTimer(time, this->interval, TickInterval, dynamic);
    connect(timer, &SyncTimer::timeout, this, [=](){
        emit this->dynamicSignal(timer->command);
        this->timeoutCount += 1;
        if(this->timeoutCount == this->timers.count()){
            emit this->endSignal();
        }
    });
    this->timers.enqueue(timer);
}


void XMLReader::addMusic(const QString& path, bool isMain){
    this->musicPlayer->addMusic(path, 50, isMain);
}


void XMLReader::setVolume(int volume){
    this->musicPlayer->setVolumeAll(volume);
}

void XMLReader::setDynamic(Dynamic dynamic){
    if(!this->volumeChangeable)
        return;
    switch(dynamic){
        case pp:
            this->setVolume(15);
            break;
        case p:
            this->setVolume(30);
            break;
        case mp:
            this->setVolume(45);
            break;
        case mf:
            this->setVolume(60);
            break;
        case f:
            this->setVolume(75);
            break;
        case ff:
            this->setVolume(90);
            break;
        default:
            this->setVolume(50);
            break;
    }
}

void XMLReader::randomizeDynamic(){
    if(!this->volumeChangeable)
        return;
    for(int index = 0; index < musicPlayer->count(); ++index)
        musicPlayer->setVolume(index, Random::pick(15, 90));
}

const QQueue<SyncTimer*>* XMLReader::getTimers(){
    return &this->timers;
}
