#ifndef TRACKER_H
#define TRACKER_H
#include <QObject>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "../config/config.h"
#include "module/pointqueue.h"

extern const int THREAD_COUNT;
extern Configuration config;
const int SHIFT_RADIAN = 125;

using namespace cv;

class QTimer;
class QRunnable;
class PointQueue;

class Tracker : public QObject{
    Q_OBJECT
    friend class Detector;
    friend class AccentDetector;
    friend class WhipDetector;
    friend class HorizontalBeatDetector;
    friend class VerticalBeatDetector;

public:
    Tracker();
    virtual ~Tracker();
    void start();
    void stop();

private:
    Ptr<BackgroundSubtractorMOG2> pMOG2;
    QTimer* frameTimer;
    QTimer* inputTimer;
    PointQueue accentQueue;
    PointQueue reverseAccentQueue;
    PointQueue whipQueue;
    PointQueue verticalBeatQueue;
    PointQueue horizontalBeatQueue;
    QMutex mutex;
    bool colorSelected;
    int emptyFlag;
    Point lastPoint;
    bool haveLastPoint;
    Mat cameraNotOpened;
    void detectActions(Point&, int, Mat&);
    void clearQueues();
    void pointFound(const Point&);
    void pointNotFound();


signals:
    void updatePictureSignal(Mat);
    void accentSignal();
    void reverseAccentSignal();
    void whipSignal();
    void verticalBeatSignal(int);
    void horizontalBeatSignal(int);

public slots:
    void updatePicture();
    void startTimer(QTimer*, int);
    void stopTimer(QTimer*);

};

#endif // TRACKER_H
