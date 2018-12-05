#ifndef XMLREADER_H
#define XMLREADER_H
#include <QObject>
#include <QQueue>
#include "../command/command.h"
#include "musicplayer.h"

class PreciseTimer;
class QListWidget;
class SyncTimer;
class QDomDocument;
class QDomElement;
class MusicPlayer;

extern const int TickInterval;

class XMLReader : public QObject
{
    Q_OBJECT

public:
    XMLReader(int = 0);
    virtual ~XMLReader();
    void setInterval(int);
    bool isXMLLoaded();
    bool loadXML(const QString&);
    void start();
    void stop();
    void pause();
    void resume();
    void setDynamic(Dynamic, int=0, int=0);
    void randomizeDynamic();
    void randomizeSpeed();
    void randomizeSpeed(int, int);
    void normalizeSpeed();
    void clear();
    const QQueue<SyncTimer*>* getTimers();

private:
    QQueue<SyncTimer*> timers;
    int interval;
    int timeoutCount;
    bool parseXML(const QDomDocument&);
    bool parseCommand(const QDomElement&);
    bool parseMusic(const QDomElement&);
    void parseVolume(const QDomElement&);
    void parseTempo(const QDomElement&);
    void parseGroup(const QDomElement&);
    void addCommand(int, const QString&);
    void addDynamic(int, const QString&);
    void addGroup(int, const QString&);
    void addMusic(const QString&, int, bool = false);
    void setVolume(int, int, int);
    MusicPlayer* musicPlayer;
    bool volumeChangeable;
    bool tempoChangeable;
    bool groupEnabled;


signals:
    void infoSignal(QString);
    void warningSignal(QString);
    void errorSignal(QString);
    void dynamicSignal(QString);
    void commandSignal(QString);
    void groupSignal(int);
    void clearSignal();
    void endSignal();
    void tickSignal();
    void groupEnabledSignal(bool);
};

#endif // XMLREADER_H
