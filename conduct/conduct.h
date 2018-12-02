#ifndef CONDUCT_H
#define CONDUCT_H
#include <QObject>
#include <QMap>
#include <QQueue>
#include <QMutex>
#include <QTimer>
#include "command/command.h"

class QString;
class SyncTimer;
class XMLReader;
class Tracker;
class QLabel;
class CountDownTimer;

typedef enum {Easy, Normal, Hard} Difficulty;
typedef enum {Perfect, Excellent, Good, Bad, Fail} Precision;
extern const QMap<Difficulty, int> DifInterval;
extern const QMap<Difficulty, double> DifProb;
extern const int LastInterval;
extern const int TickInterval;
extern const int MaxEnergy;

class ConductSimulator : public QObject{
    Q_OBJECT

public:
    ConductSimulator();
    virtual ~ConductSimulator();
    void setDifficulty(Difficulty);
    void clearCommands();
    void applyTracker(Tracker*);
    void loadXML(const QString&);
    bool isPausing() const;


private:
    Tracker* tracker;
    XMLReader* xmlReader;
    QMutex mutex;
    Difficulty difficulty;
    int interval;
    QMap<QString, QQueue<SyncTimer*>*> waitingCommands;
    SyncTimer* lastTimer;
    CountDownTimer* startTimer;
    int score;
    int energy;
    bool isPlaying;
    bool isPause;
    bool groupEnabled;
    Dynamic dynamic;
    int currentGroup;
    void checkVerticalDynamic(int);
    void checkHorizontalDynamic(int);
    Dynamic dynamicByVertical(int);
    Dynamic dynamicByHorizontal(int);
    void makeScore();
    void commandSuccess(int);
    void commandFail();
    void beatFail();
    void commandFail(int, int);
    bool checkDynamic(int, int, int);
    void setScore(int);
    void setEnergy(int);
    void addScore(int);
    void addEnergy(int);
    void gameover();

signals:
    void infoSignal(QString);
    void scoreChangedSignal(int);
    void energyChangedSignal(int);
    void endSignal();
    void initSignal();
    void makeSheetSignal(const QQueue<SyncTimer*>*, int, int);
    void tickSignal();
    void commandSignal(Precision);
    void dynamicSignal(Precision);
    void gameoverSignal();


public slots:
    void start();
    void gameStart();
    void pause();
    void resume();
    void clear();
    void addCommand(QString);
    void setDynamic(const QString&);
    void setGroup(int);
    void removeCommand(const QString&);
    void removeCommand(const QString&, int);

};

#endif // CONDUCT_H
