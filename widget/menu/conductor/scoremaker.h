#ifndef SCOREMAKER_H
#define SCOREMAKER_H
#include <QQueue>
#include <QLabel>
class SyncTimer;
class QPainter;


class MusicSheet : public QWidget{
public:
    MusicSheet(const QQueue<SyncTimer*>*);
    virtual ~MusicSheet() = default;

public:
    virtual void paintEvent(QPaintEvent*);

private:
    const QQueue<SyncTimer*>* commands;
    void drawBackground(QPainter&);
    void drawDynamic(QPainter&, const QString, int);
    void drawCommand(QPainter&, const QString, int);
    void drawBeat(QPainter&, const QString, int);
    void drawGroup(QPainter&, const QString, int);
};


class ScoreMaker{
public:
    static QWidget* makeScore(const QQueue<SyncTimer*>*, int, int, int);
};

#endif // SCOREMAKER_H
