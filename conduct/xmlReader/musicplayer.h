#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H
#include <QObject>
#include <QThread>
#include <QVector>
#include "fmod.h"
#include "fmod_common.h"
#include "fmod_errors.h"

extern const int TickInterval;

#define FMOD_FOREACH(mark, statement) \
    for(int mark = 0; mark < musicCount; ++mark){ \
        statement; \
    } \

#define VOLUME(volume) \
    static_cast<float>(volume) / 100 \

using uint = unsigned int;
class QString;

class MusicTimer : public QThread{
    Q_OBJECT

public:
    MusicTimer(uint);
    virtual ~MusicTimer() = default;
    void start() = delete;
    void start(FMOD_CHANNEL*);
    void stop();
    bool isActive();
    void setChannel(FMOD_CHANNEL*);

signals:
    void tickSignal();

private:
    void run();
    uint tickInterval;
    bool isRunning;
    bool isEnded;
    FMOD_CHANNEL *channel;
};

class MusicPlayer : public QObject{
    Q_OBJECT

public:
    MusicPlayer(uint);
    virtual ~MusicPlayer();
    void addMusic(const QString&, int, bool = false);
    void playAll();
    void stopAll();
    void pauseAll();
    void resumeAll();
    void clear();
    void rewind();
    int count() const;
    void setVolume(int, int);
    void setVolumeAll(int);
    bool empty() const;
    void setSpeed(int, float);
    void setSpeedSpecific(int, float);
    void setSpeedAll(float);
    void multSpeed(int, float);
    void multSpeedSpecific(int, float);
    void multSpeedAll(float);
    void setMain(int);

signals:
    void tickSignal();

private:
    int musicCount;
    FMOD_SYSTEM *system;
    QVector<FMOD_SOUND*> sound;
    QVector<FMOD_CHANNEL*> channel;
    QVector<int> volumes;
    QVector<float> speeds;
    QVector<int> mainSound;
    FMOD_RESULT result;
    FMOD_DSP* dsp;
    uint tickInterval;
    MusicTimer* timer;
};



#endif // MUSICPLAYER_H