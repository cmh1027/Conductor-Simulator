#ifndef TRACKER_H
#define TRACKER_H
#include <QObject>
#include <QVector>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "../config/config.h"
#include "module/pointqueue.h"

extern const int THREAD_COUNT;
extern Configuration config;

extern const int SHIFT_RADIAN;
extern const int NUMERATOR_LOW;
extern const int NUMERATOR_HIGH;
extern const int DENOMINATOR;
extern const int SIGN_RADIUS;


using namespace cv;

class QTimer;
class Detector;
class PointQueue;
class Detector;
class EyeDetector;

class Tracker : public QObject{
    Q_OBJECT

public:
    Tracker();
    virtual ~Tracker();
    void start();
    void stop();
    void turnonEyeDetector();
    void setCurrentGroup(int);
    int eyePosition;

private:
    Ptr<BackgroundSubtractorMOG2> pMOG2;
    QTimer* frameTimer;
    QTimer* inputTimer;
    QVector<Detector*> detectors;
    QVector<PointQueue*> pointQueues;
    QMutex mutex;
    bool colorSelected;
    int emptyFlag;
    Point lastPoint;
    bool haveLastPoint;
    Mat cameraNotOpened;
    Rect lastEyesPoint;
    EyeDetector* eyeDetector;
    Mat borders;
    Mat currentGroup;
    QVector<Mat> currentEyesPosMat;
    QVector<Mat> currentGroupMat;
    Point eyes;
    void prepareDetectors();
    void addDetector(Detector*, int = -1);
    void detectActions(Point&, int, Mat&);
    void clearQueues();
    void pointFound(const Point&);
    void pointNotFound();
    void drawBorders(Mat);
    void drawGroupSigns(Mat);
    int currentEyesPosition(Mat);

signals:
    void updatePictureSignal(Mat);
    void commandSignal(QString);
    void commandSignal(QString, int);

public slots:
    void updatePicture();
    void startTimer(QTimer*, int);
    void stopTimer(QTimer*);
};




#endif // TRACKER_H
