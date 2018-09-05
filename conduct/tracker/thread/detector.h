#ifndef DETECTOR_H
#define DETECTOR_H
#include <QObject>
#include <QRunnable>
#include <QQueue>
#include <QSet>
#include <opencv2/core/core.hpp>
#include "../tracker.h"

using namespace cv;
class PointQueue;

class Detector : public QObject, public QRunnable
{
    Q_OBJECT

public:
    Detector(PointQueue&, int, const Point&, int, Mat&, bool = true);
    virtual ~Detector() override = default;

protected:
    void run() override;
    virtual void check(PointQueue&) = 0;
    PointQueue& queue;
    int queueSize;
    int count;
    const Point& point;
    Mat& canvas;
    bool drawFlag;

signals:
    void detected();
    void detected(int distance);
};

class AccentDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~AccentDetector() override = default;

private:
    void check(PointQueue&) override;
};

class ReverseAccentDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~ReverseAccentDetector() override = default;

private:
    void check(PointQueue&) override;
};


class HorizontalBeatDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~HorizontalBeatDetector() override = default;

private:
    void check(PointQueue&) override;
};

class VerticalBeatDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~VerticalBeatDetector() override = default;

private:
    void check(PointQueue&) override;
};

class WhipDetector : public Detector
{
    Q_OBJECT

public:
    using Detector::Detector;
    virtual ~WhipDetector() override = default;

private:
    void check(PointQueue&) override;
};

#endif
