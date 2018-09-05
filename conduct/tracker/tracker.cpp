#include <QTimer>
#include <QPainter>
#include <QSet>
#include <QPair>
#include <QThreadPool>
#include <cmath>
#include <iostream>
#include "tracker.h"
#include "thread/detector.h"
#include "thread/thread.h"
#include "module/meanshift.h"
#include "conduct/module/utility.h"


Tracker::Tracker() : frameTimer(new QTimer()), inputTimer(new QTimer()),
    colorSelected(false), emptyFlag(0), haveLastPoint(false){
    pMOG2 = createBackgroundSubtractorMOG2(25, 16, false);
    connect(frameTimer, &QTimer::timeout, this, [this](){
        if(mutex.tryLock()){
            this->updatePicture();
            mutex.unlock();
        }
    });
    connect(inputTimer, &QTimer::timeout, this, [this](){
        if(mutex.tryLock()){
            this->clearQueues();
            mutex.unlock();
        }
    });
    inputTimer->setSingleShot(true);
    this->cameraNotOpened = Mat::zeros(480, 640, CV_8UC3);
    putText(this->cameraNotOpened, "Camera is not opened. Please set the camera in the configuration menu", Point(10, 20), 2, 0.4, Scalar::all(255));
}

Tracker::~Tracker(){
    delete frameTimer;
    delete inputTimer;
}


void Tracker::start(){
    if(frameTimer->isActive())
        return;
    frameTimer->start(20);
    inputTimer->start(1000);
    config.cameraOn();

}

void Tracker::stop(){
    if(!frameTimer->isActive())
        return;
    frameTimer->stop();
    inputTimer->stop();
    this->clearQueues();
    config.cameraOff();
}


void Tracker::clearQueues(){
    this->accentQueue.clear();
    this->reverseAccentQueue.clear();
    this->whipQueue.clear();
    this->verticalBeatQueue.clear();
    this->horizontalBeatQueue.clear();
}


void Tracker::updatePicture(){
    if(!config.isCamOpened()){
        emit this->updatePictureSignal(this->cameraNotOpened);
        return;
    }
    Mat frame;
    Mat mogMask;
    Mat moving;
    Mat hsv;
    Mat result;
    Mat object;
    Mat colorMask;
    Mat actionCanvas;
    bool hsvExtension = false;
    config.readFrame(frame);
    actionCanvas = Mat::zeros(frame.rows, frame.cols, frame.type());
    flip(frame, frame, 1);
    pMOG2->apply(frame, mogMask);
    morphologyEx(mogMask, mogMask, CV_MOP_ERODE, getStructuringElement(MORPH_ELLIPSE, Size(config.getKernel(), config.getKernel())));
    morphologyEx(mogMask, mogMask, CV_MOP_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(config.getKernel(), config.getKernel())));
    threshold(mogMask, mogMask, config.getThreshold(), 255, THRESH_BINARY);
    cvtColor(mogMask, mogMask, COLOR_GRAY2BGR);
    bitwise_and(frame, mogMask, moving);
    cvtColor(moving, hsv, COLOR_BGR2HSV);
    inRange(hsv, config.getLowerBound(), config.getUpperBound(), colorMask);
    if(countNonZero(colorMask) == 0){
        if(this->haveLastPoint){
            inRange(hsv, config.getWideLowerBound(), config.getWideUpperBound(), colorMask);
            if(countNonZero(colorMask) != 0){
                colorMask = remainPixelsInCircle(this->lastPoint, SHIFT_RADIAN, colorMask);
            }
            if(countNonZero(colorMask) != 0){
                hsvExtension = true;
            }
        }
    }
    if(countNonZero(colorMask) == 0){
        this->pointNotFound();
    }
    cvtColor(colorMask, colorMask, COLOR_GRAY2BGR);
    bitwise_and(moving, colorMask, result);
    morphologyEx(result, result, CV_MOP_ERODE, getStructuringElement(MORPH_ELLIPSE,Size(3, 3)));
    QSet<QPair<int, int>> dataPixels;
    QSet<QPair<int, int>> objectPixels;
    Point point;
    findDataPixels(result, dataPixels);
    if(dataPixels.size() != 0){
        if(this->haveLastPoint)
            meanShift(dataPixels, this->lastPoint);
        else
            meanShift(dataPixels);
        if(hsvExtension)
            object = fillObject(frame, dataPixels, config.getWideLowerBound(), config.getWideUpperBound());
        else
            object = fillObject(frame, dataPixels, config.getLowerBound(), config.getUpperBound());
        findDataPixels(object, objectPixels);
        if(!objectPixels.empty()){
            emit this->stopTimer(this->inputTimer);
            emit this->startTimer(this->inputTimer, 1000);
            QPair<int, int> pair = findCentroid(objectPixels);
            point.x = pair.first;
            point.y = pair.second;
            this->pointFound(point);
        }
        else{
            this->pointNotFound();
        }
    }
    else{
        this->pointNotFound();
    }
    int count = objectPixels.size();
    if(count > 0)
        circle(actionCanvas, point, 10, Scalar(255, 0, 0), 2);
    this->detectActions(point, count, actionCanvas);
    cvtColor(frame, frame, COLOR_BGR2RGB);
    bitwise_or(frame, actionCanvas, frame);
    emit this->updatePictureSignal(frame);
}


void Tracker::detectActions(Point& point, int count, Mat& canvas){
    auto accentThread = new AccentDetector(this->accentQueue, config.getQueueSize(), point, count, canvas);
    connect(accentThread, QOverload<>::of(&Detector::detected), this, &Tracker::accentSignal);
    auto reverseAccentThread = new ReverseAccentDetector(this->reverseAccentQueue, config.getQueueSize(), point, count, canvas);
    connect(reverseAccentThread, QOverload<>::of(&Detector::detected), this, &Tracker::reverseAccentSignal);
    auto whipThread = new WhipDetector(this->whipQueue, config.getQueueSize(), point, count, canvas);
    connect(whipThread, QOverload<>::of(&Detector::detected), this, &Tracker::whipSignal);
    auto verticalBeatThread = new VerticalBeatDetector(this->verticalBeatQueue, -1, point, count, canvas);
    connect(verticalBeatThread, QOverload<int>::of(&Detector::detected), this, [=](int distance){
        emit this->verticalBeatSignal(distance);
    });
    auto horizontalBeatThread = new HorizontalBeatDetector(this->horizontalBeatQueue, -1, point, count, canvas);
    connect(horizontalBeatThread, QOverload<int>::of(&Detector::detected), this, [=](int distance){
        emit this->horizontalBeatSignal(distance);
    });
    QThreadPool::globalInstance()->start(accentThread);
    QThreadPool::globalInstance()->start(reverseAccentThread);
    QThreadPool::globalInstance()->start(whipThread);
    QThreadPool::globalInstance()->start(horizontalBeatThread);
    QThreadPool::globalInstance()->start(verticalBeatThread);
    QThreadPool::globalInstance()->waitForDone();
}

void Tracker::startTimer(QTimer* timer, int time){
    timer->start(time);
}

void Tracker::stopTimer(QTimer* timer){
    timer->stop();
}

void Tracker::pointFound(const Point& point){
    this->lastPoint = point;
    this->haveLastPoint = true;
}

void Tracker::pointNotFound(){
    this->lastPoint.x = 0;
    this->lastPoint.y = 0;
    this->haveLastPoint = false;
}


