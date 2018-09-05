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
    void setDynamic(Dynamic);
    void clear();
    const QQueue<SyncTimer*>* getTimers();

private:
    QQueue<SyncTimer*> timers;
    int interval;
    int timeoutCount;
    bool parseXML(const QDomDocument&);
    bool parseCommand(const QDomElement&);
    bool parseMusic(const QDomElement&);
    bool parseVolume(const QDomElement&);
    bool parseTempo(const QDomElement&);
    void addCommand(int, const QString&);
    void addDynamic(int, const QString&);
    void addMusic(const QString&, bool = false);
    void setVolume(int);
    MusicPlayer* musicPlayer;
    bool volumeChangeable;
    bool tempoChangeable;


signals:
    void infoSignal(QString);
    void warningSignal(QString);
    void errorSignal(QString);
    void dynamicSignal(QString);
    void commandSignal(QString);
    void clearSignal();
    void endSignal();
    void tickSignal();
};

#endif // XMLREADER_H
