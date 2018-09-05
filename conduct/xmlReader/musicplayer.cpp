#include <QString>
#include "musicplayer.h"
MusicPlayer::MusicPlayer(uint tickInterval) : musicCount(0), system(nullptr),
    tickInterval(tickInterval), timer(new MusicTimer(tickInterval))
{
    Q_ASSERT(tickInterval >= 100);
    uint version;
    FMOD_System_Create(&system);
    result = FMOD_System_GetVersion(system, &version);
    Q_ASSERT(version >= FMOD_VERSION);
    FMOD_System_Init(system, 320, FMOD_INIT_NORMAL, nullptr);
    FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
    connect(timer, &MusicTimer::tickSignal, this, &MusicPlayer::tickSignal);
}

MusicPlayer::~MusicPlayer(){
    timer->stop();
    timer->wait();
    delete timer;
    clear();
    FMOD_DSP_Release(dsp);
    FMOD_System_Release(system);
}

void MusicPlayer::addMusic(const QString& path, int volume, bool isMain){
    ++musicCount;
    sound.append(nullptr);
    channel.append(nullptr);
    volumes.append(0);
    speeds.append(0);
    FMOD_System_CreateSound(system, path.toStdString().c_str(), FMOD_LOOP_OFF | FMOD_2D, nullptr, &sound[musicCount-1]);
    FMOD_Channel_SetVolume(channel[musicCount-1], VOLUME(volume));
    volumes[musicCount - 1] = volume;
    if(isMain)
        this->setMain(musicCount - 1);

}

void MusicPlayer::playAll(){
    Q_ASSERT(musicCount > 0);
    Q_ASSERT(!mainSound.empty());
    FMOD_DSP_SetParameterFloat(dsp, 0, 1.0f);
    FMOD_FOREACH(i, FMOD_System_PlaySound(system, sound[i], nullptr, false, &channel[i]));
    FMOD_FOREACH(i, FMOD_Channel_SetVolume(channel[i], VOLUME(volumes[i])));
    FMOD_FOREACH(i, FMOD_Channel_SetPitch(channel[i], 1.0f));
    FMOD_FOREACH(i, FMOD_Channel_AddDSP(channel[i], 0, dsp));
    FMOD_FOREACH(i, speeds[i] = 1.0f);
    timer->start(channel[mainSound[0]]);
}

void MusicPlayer::stopAll(){
    FMOD_FOREACH(i, FMOD_Channel_Stop(channel[i]));
    timer->stop();
}


void MusicPlayer::pauseAll(){
    FMOD_FOREACH(i, FMOD_Channel_SetPaused(channel[i], true));
    timer->stop();
}

void MusicPlayer::resumeAll(){
    Q_ASSERT(musicCount > 0);
    FMOD_FOREACH(i, FMOD_Channel_SetPaused(channel[i], false));
    timer->start(channel[0]);
}


void MusicPlayer::clear(){
    timer->stop();
    FMOD_FOREACH(i, FMOD_Sound_Release(sound[i]));
    channel.clear();
    sound.clear();
    volumes.clear();
    speeds.clear();
    mainSound.clear();
    musicCount = 0;
}

int MusicPlayer::count() const{
    return musicCount;
}

bool MusicPlayer::empty() const{
    return musicCount == 0;
}

void MusicPlayer::setVolume(int index, int volume){
    Q_ASSERT(index < musicCount);
    if(mainSound.contains(index)){
        foreach(int number, mainSound){
            FMOD_Channel_SetVolume(channel[number], VOLUME(volume));
        }
    }
    else
        FMOD_Channel_SetVolume(channel[index], VOLUME(volume));
}

void MusicPlayer::setVolumeAll(int volume){
    FMOD_FOREACH(i, FMOD_Channel_SetVolume(channel[i], VOLUME(volume)));
}

void MusicPlayer::setSpeed(int index, float speed){
    Q_ASSERT(index < musicCount);
    if(mainSound.contains(index)){
        foreach(int number, mainSound){
            setSpeedSpecific(number, speed);
        }
    }
    else{
        setSpeedSpecific(index, speed);
    }
}

void MusicPlayer::setSpeedSpecific(int index, float speed){
    speeds[index] = speed;
    FMOD_DSP_SetParameterFloat(dsp, 0, 1/speeds[index]);
    FMOD_Channel_SetPitch(channel[index], speeds[index]);
    FMOD_Channel_AddDSP(channel[index], 0, dsp);
}

void MusicPlayer::setSpeedAll(float speed){
    FMOD_FOREACH(i, speeds[i] = speed);
    FMOD_FOREACH(i, FMOD_DSP_SetParameterFloat(dsp, 0, 1/speeds[i]);
                    FMOD_Channel_SetPitch(channel[i], speeds[i]);
                    FMOD_Channel_AddDSP(channel[i], 0, dsp));
}

void MusicPlayer::multSpeed(int index, float speed){
    Q_ASSERT(index < musicCount);
    if(mainSound.contains(index)){
        foreach(int number, mainSound){
            multSpeedSpecific(number, speed);
        }
    }
    else{
        multSpeedSpecific(index, speed);
    }
}

void MusicPlayer::multSpeedSpecific(int index, float speed){
    speeds[index] = speeds[index] * speed;
    FMOD_DSP_SetParameterFloat(dsp, 0, 1/speeds[index]);
    FMOD_Channel_SetPitch(channel[index], speeds[index]);
    FMOD_Channel_AddDSP(channel[index], 0, dsp);
}

void MusicPlayer::multSpeedAll(float speed){
    FMOD_FOREACH(i, speeds[i] = speeds[i] * speed);
    FMOD_FOREACH(i, FMOD_DSP_SetParameterFloat(dsp, 0, 1/speeds[i]);
                    FMOD_Channel_SetPitch(channel[i], speeds[i]);
                    FMOD_Channel_AddDSP(channel[i], 0, dsp));
}

void MusicPlayer::setMain(int index){
    Q_ASSERT(index < musicCount);
    mainSound.append(index);
}


MusicTimer::MusicTimer(uint tickInterval) : tickInterval(tickInterval), isRunning(false), isEnded(true),
    channel(nullptr)
{
    Q_ASSERT(tickInterval >= 100);
}

void MusicTimer::run(){
    isRunning = true;
    isEnded = false;
    uint position;
    FMOD_Channel_GetPosition(channel, &position, FMOD_TIMEUNIT_MS);
    uint tick = position / tickInterval;
    while(isRunning){
        FMOD_Channel_GetPosition(channel, &position, FMOD_TIMEUNIT_MS);
        if(tick != position / tickInterval){
            tick = position / tickInterval;
            emit this->tickSignal();
        }
    }
    isEnded = true;
}

void MusicTimer::start(FMOD_CHANNEL* channel){
    Q_ASSERT(channel != nullptr);
    setChannel(channel);
    QThread::start();
}

void MusicTimer::stop(){
    this->isRunning = false;
    while(this->isActive());
}

bool MusicTimer::isActive(){
    return !this->isEnded;
}

void MusicTimer::setChannel(FMOD_CHANNEL* channel){
    Q_ASSERT(channel != nullptr);
    this->channel = channel;
}
