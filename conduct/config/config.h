#ifndef CONFIG_H
#define CONFIG_H
#include <QColor>
#include <QString>
#include <opencv2/opencv.hpp>

using namespace cv;

extern const int THREAD_COUNT;

namespace Menu{
    class Configuration;
}

class Configuration{
    friend class Menu::Configuration;

public:
    Configuration();
    ~Configuration();
    double getRatio() const;
    int getThreshold() const;
    int getKernel() const;
    int getQueueSize() const;
    int getHistory() const;
    Scalar getLowerBound() const;
    Scalar getUpperBound() const;
    Scalar getWideLowerBound() const;
    Scalar getWideUpperBound() const;
    void readFrame(Mat&);
    void cameraOn();
    void cameraOff();
    bool isCamOpened();
    void setEyeDetectorPath(const QString&);
    QString getEyeDetectorPath() const;

private:
    double ratio;
    int thresholdVal;
    int kernel;
    int queueSize;
    int history;
    QColor currentRgb;
    Scalar currentHsv;
    Scalar hsvLowerBound;
    Scalar hsvUpperBound;
    Scalar wideHsvUpperBound;
    Scalar wideHsvLowerBound;
    VideoCapture* cam;
    int camNumber;
    QString eyeDetectorPath;
    QString refreshRgb();
    void setCurrentRgb(const QColor&);
    void setCurrentHsv(const Scalar&);
    void setRatio(double);
    void setThreshold(int);
    void setKernel(int);
    void setQueueSize(int);
    void setHistory(int);
    void setCamera(int);

};

#endif // CONFIG_H
