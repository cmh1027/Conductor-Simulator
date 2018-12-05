#include "module/eyedetector.h"
#include <QTimer>
#include <QPainter>
#include <QSet>
#include <QPair>
#include <QFileInfo>
#include <cmath>
#include <vector>
#include "tracker.h"
#include "thread/detector.h"
#include "thread/thread.h"
#include "module/meanshift.h"
#include "conduct/module/utility.h"

const int SHIFT_RADIAN = 125;
const int NUMERATOR_LOW = 11;
const int NUMERATOR_HIGH = 13;
const int DENOMINATOR = 24;
const int SIGN_RADIUS = 5;


Tracker::Tracker() : groupEnabled(false), frameTimer(new QTimer()), inputTimer(new QTimer()),
    colorSelected(false), emptyFlag(0), haveLastPoint(false), eyeDetector(nullptr){
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
    this->prepareDetectors();
}

Tracker::~Tracker(){
    delete frameTimer;
    delete inputTimer;
    foreach(auto detector, this->detectors){
        delete detector;
    }
    foreach(auto queue, this->pointQueues){
        delete queue;
    }
}

void Tracker::prepareDetectors(){
    Q_ASSERT(this->detectors.empty());
    this->addDetector(new AccentDetector(), config.getQueueSize());
    this->addDetector(new ShortAccentDetector(), config.getQueueSize());
    this->addDetector(new ReverseAccentDetector(), config.getQueueSize());
    this->addDetector(new WhipDetector(), config.getQueueSize());
    this->addDetector(new DiagonalWhipDetector(), config.getQueueSize());
    this->addDetector(new VerticalSwingDetector(), config.getQueueSize());
    this->addDetector(new HorizontalSwingDetector(), config.getQueueSize());
    this->addDetector(new VerticalBeatDetector());
    this->addDetector(new HorizontalBeatDetector());
}


void Tracker::addDetector(Detector* detector, int queueSize){
    this->pointQueues.append(new PointQueue());
    detector->setQueue(this->pointQueues[this->pointQueues.size() - 1]);
    detector->setQueueSize(queueSize);
    this->detectors.append(detector);
    connect(detector, QOverload<QString>::of(&Detector::detected), this, QOverload<QString>::of(&Tracker::commandSignal));
    connect(detector, QOverload<QString, int>::of(&Detector::detected), this, QOverload<QString, int>::of(&Tracker::commandSignal));
}

void Tracker::turnonEyeDetector(){
    Mat frame;
    config.readFrame(frame);
    this->eyeDetector = EyeDetector::getInstance(frame, config.getEyeDetectorPath(), &eyes);
    this->eyeDetector->connect();
    this->groupEnabled = true;
}

void Tracker::start(){
    if(frameTimer->isActive())
        return;
    Mat frame;
    frameTimer->start(20);
    inputTimer->start(1000);
    config.cameraOn();
    config.readFrame(frame);
    this->cameraNotOpened = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
    this->currentGroup = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
    putText(this->cameraNotOpened, "Camera is not opened. Please set the camera in the configuration menu", Point(10, 20), 2, 0.4, Scalar::all(255));
    this->drawBorders(frame);
    this->drawGroupSigns(frame);
    if(QFileInfo::exists(config.getEyeDetectorPath()))
        this->turnonEyeDetector();
}

void Tracker::stop(){
    if(!frameTimer->isActive())
        return;
    frameTimer->stop();
    inputTimer->stop();
    this->clearQueues();
    config.cameraOff();
    if(this->eyeDetector != nullptr)
        this->eyeDetector->stop();
}


void Tracker::clearQueues(){
    foreach(auto queue, this->pointQueues){
        queue->clear();
    }
}

Mat Tracker::subtractBackground(Mat frame){
    Mat mogMask;
    Mat moving;
    pMOG2->apply(frame, mogMask);
    morphologyEx(mogMask, mogMask, CV_MOP_ERODE, getStructuringElement(MORPH_ELLIPSE, Size(config.getKernel(), config.getKernel())));
    morphologyEx(mogMask, mogMask, CV_MOP_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(config.getKernel(), config.getKernel())));
    threshold(mogMask, mogMask, config.getThreshold(), 255, THRESH_BINARY);
    cvtColor(mogMask, mogMask, COLOR_GRAY2BGR);
    bitwise_and(frame, mogMask, moving);
    return moving;
}

Mat Tracker::detectColoredObject(Mat moving, bool& hsvExtension){
    Mat hsv;
    Mat colorMask;
    Mat colored;
    cvtColor(moving, hsv, COLOR_BGR2HSV);
    inRange(hsv, config.getLowerBound(), config.getUpperBound(), colorMask);
    if(countNonZero(colorMask) == 0){
        if(this->haveLastPoint){
            inRange(hsv, config.getWideLowerBound(), config.getWideUpperBound(), colorMask);
            if(countNonZero(colorMask) != 0){
                colorMask = remainPixelsInCircle(this->lastPoint, SHIFT_RADIAN, colorMask);
                hsvExtension = true;
            }
        }
    }
    if(countNonZero(colorMask) == 0){
        this->pointNotFound();
    }
    cvtColor(colorMask, colorMask, COLOR_GRAY2BGR);
    bitwise_and(moving, colorMask, colored);
    morphologyEx(colored, colored, CV_MOP_ERODE, getStructuringElement(MORPH_ELLIPSE,Size(3, 3)));
    return colored;
}

Mat Tracker::detectActions(Mat frame, Mat colored, bool hsvExtension){
    Point point;
    Mat object;
    Mat actionCanvas = Mat::zeros(frame.rows, frame.cols, frame.type());
    QSet<QPair<int, int>> dataPixels;
    QSet<QPair<int, int>> objectPixels;
    findDataPixels(colored, dataPixels);
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
        circle(actionCanvas, point, 10, Scalar(0, 0, 255), 2);
    this->detectActions(point, count, actionCanvas);
    bitwise_or(frame, actionCanvas, frame);
    return frame;
}

void Tracker::updatePicture(){
    if(!config.isCamOpened()){
        emit this->updatePictureSignal(this->cameraNotOpened);
        return;
    }
    Mat frame;
    Mat moving;
    Mat colored;
    Mat currentEyes;
    bool hsvExtension = false;
    config.readFrame(frame);
    flip(frame, frame, 1);
    if(this->eyeDetector != nullptr)
        this->eyeDetector->detectEyes(frame);
    moving = this->subtractBackground(frame);
    colored = this->detectColoredObject(moving, hsvExtension);
    frame = this->detectActions(frame, colored, hsvExtension);
    cvtColor(frame, frame, COLOR_BGR2RGB);
    if(groupEnabled){
        bitwise_or(frame, this->borders, frame);
        currentEyes = this->currentEyesPosMat.at(this->currentEyesPosition(frame));
        bitwise_or(frame, currentEyes, frame);
    }
    bitwise_or(frame, this->currentGroup, frame);
    circle(frame, this->eyes, 3, Scalar(255, 255, 255), -1);
    emit this->updatePictureSignal(frame);
}


void Tracker::detectActions(Point& point, int count, Mat& canvas){
    foreach(auto detector, this->detectors){
        detector->setData(point, count, canvas);
        detector->start();
    }
    foreach(auto detector, this->detectors)
        detector->wait();
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

void Tracker::drawBorders(Mat frame){
    this->borders = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
    int widthUnit = frame.cols / DENOMINATOR;
    int heightUnit = frame.rows / 2;
    line(this->borders, Point(widthUnit * NUMERATOR_LOW, 0), Point(widthUnit * NUMERATOR_LOW, frame.rows), Scalar(255, 0, 0));
    line(this->borders, Point(widthUnit * NUMERATOR_HIGH, 0), Point(widthUnit * NUMERATOR_HIGH, frame.rows), Scalar(255, 0, 0));
    line(this->borders, Point(0, heightUnit), Point(widthUnit * NUMERATOR_LOW, heightUnit), Scalar(255, 0, 0));
    line(this->borders, Point(widthUnit * NUMERATOR_HIGH, heightUnit), Point(frame.cols, heightUnit), Scalar(255, 0, 0));
    this->currentEyesPosMat.clear();
    Mat mat_empty = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
    // (0)
    Mat mat = mat_empty.clone();
    line(mat, Point(widthUnit * NUMERATOR_LOW, 0), Point(widthUnit * NUMERATOR_LOW, frame.rows), Scalar(0, 0, 255));
    line(mat, Point(widthUnit * NUMERATOR_HIGH, 0), Point(widthUnit * NUMERATOR_HIGH, frame.rows), Scalar(0, 0, 255));
    this->currentEyesPosMat.push_back(mat);
    // (1)
    mat = mat_empty.clone();
    line(mat, Point(0, heightUnit), Point(widthUnit * NUMERATOR_LOW, heightUnit), Scalar(0, 0, 255));
    line(mat, Point(widthUnit * NUMERATOR_LOW, 0), Point(widthUnit * NUMERATOR_LOW, heightUnit), Scalar(0, 0, 255));
    this->currentEyesPosMat.push_back(mat);
    // (2)
    mat = mat_empty.clone();
    line(mat, Point(widthUnit * NUMERATOR_HIGH, 0), Point(widthUnit * NUMERATOR_HIGH, heightUnit), Scalar(0, 0, 255));
    line(mat, Point(widthUnit * NUMERATOR_HIGH, heightUnit), Point(frame.cols, heightUnit), Scalar(0, 0, 255));
    this->currentEyesPosMat.push_back(mat);
    // (3)
    mat = mat_empty.clone();
    line(mat, Point(0, heightUnit), Point(widthUnit * NUMERATOR_LOW, heightUnit), Scalar(0, 0, 255));
    line(mat, Point(widthUnit * NUMERATOR_LOW, heightUnit), Point(widthUnit * NUMERATOR_LOW, frame.rows), Scalar(0, 0, 255));
    this->currentEyesPosMat.push_back(mat);
    // (4)
    mat = mat_empty.clone();
    line(mat, Point(widthUnit * NUMERATOR_HIGH, heightUnit), Point(widthUnit * NUMERATOR_HIGH, frame.rows), Scalar(0, 0, 255));
    line(mat, Point(widthUnit * NUMERATOR_HIGH, heightUnit), Point(frame.cols, heightUnit), Scalar(0, 0, 255));
    this->currentEyesPosMat.push_back(mat);
}

void Tracker::drawGroupSigns(Mat frame){
    int widthUnit = frame.cols / DENOMINATOR;
    int heightUnit = frame.rows / 2;
    this->currentGroupMat.clear();
    Mat mat_empty = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
    // (0)
    Mat mat = mat_empty.clone();
    circle(mat, Point(widthUnit * NUMERATOR_LOW + SIGN_RADIUS, SIGN_RADIUS), SIGN_RADIUS, Scalar(255, 255, 0), CV_FILLED);
    this->currentGroupMat.push_back(mat);
    // (1)
    mat = mat_empty.clone();
    circle(mat, Point(SIGN_RADIUS, SIGN_RADIUS), SIGN_RADIUS, Scalar(255, 255, 0), CV_FILLED);
    this->currentGroupMat.push_back(mat);
    // (2)
    mat = mat_empty.clone();
    circle(mat, Point(SIGN_RADIUS + widthUnit * NUMERATOR_HIGH, SIGN_RADIUS), SIGN_RADIUS, Scalar(255, 255, 0), CV_FILLED);
    this->currentGroupMat.push_back(mat);
    // (3)
    mat = mat_empty.clone();
    circle(mat, Point(SIGN_RADIUS, SIGN_RADIUS + heightUnit), SIGN_RADIUS, Scalar(255, 255, 0), CV_FILLED);
    this->currentGroupMat.push_back(mat);
    // (4)
    mat = mat_empty.clone();
    circle(mat, Point(SIGN_RADIUS + widthUnit * NUMERATOR_HIGH, SIGN_RADIUS + heightUnit), SIGN_RADIUS, Scalar(255, 255, 0), CV_FILLED);
    this->currentGroupMat.push_back(mat);
}

int Tracker::currentEyesPosition(Mat frame){
    int widthUnit = frame.cols / DENOMINATOR;
    int heightUnit = frame.rows / 2;
    int pos;
    if(widthUnit * NUMERATOR_LOW < this->eyes.x && this->eyes.x < widthUnit * NUMERATOR_HIGH){
        pos = 0;
    }
    else if(this->eyes.x <= widthUnit * NUMERATOR_LOW){
        if(this->eyes.y <= heightUnit)
            pos = 1;
        else
            pos = 3;
    }
    else{
        if(this->eyes.y <= heightUnit)
            pos = 2;
        else
            pos = 4;
    }
    this->eyePosition = pos;
    return pos;
}

void Tracker::setCurrentGroup(int group){
    Q_ASSERT(group < this->currentGroupMat.size());
    if(group == -1)
        this->currentGroup = Mat::zeros(this->currentGroup.rows, this->currentGroup.cols, CV_8UC3);
    else
        this->currentGroup = this->currentGroupMat.at(group);
}
