#ifndef DETECTOR_H
#define DETECTOR_H
#include <QObject>
#include <QThread>
#include <QQueue>
#include <QSet>
#include <QString>
#include <opencv2/core/core.hpp>
#include "../tracker.h"

using namespace cv;
class PointQueue;

class Detector : public QThread
{
    Q_OBJECT

public:
    Detector();
    virtual ~Detector() override = default;
    void setQueue(PointQueue*);
    void setQueueSize(int);
    void setData(Point&, int, Mat&, bool = true);

protected:
    void run() override;
    virtual void check(PointQueue*) = 0;
    PointQueue* queue;
    int queueSize;
    int count;
    Point point;
    Mat canvas;
    bool drawFlag;

signals:
    void detected(QString);
    void detected(QString, int distance);
};

class AccentDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~AccentDetector() override = default;

private:
    void check(PointQueue*) override;
};

class ShortAccentDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~ShortAccentDetector() override = default;

private:
    void check(PointQueue*) override;
};

class ReverseAccentDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~ReverseAccentDetector() override = default;

private:
    void check(PointQueue*) override;
};


class HorizontalBeatDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~HorizontalBeatDetector() override = default;

private:
    void check(PointQueue*) override;
};

class VerticalBeatDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~VerticalBeatDetector() override = default;

private:
    void check(PointQueue*) override;
};

class WhipDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~WhipDetector() override = default;

private:
    void check(PointQueue*) override;
};

class DiagonalWhipDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~DiagonalWhipDetector() override = default;

private:
    void check(PointQueue*) override;
};
#endif
