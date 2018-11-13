#include <QTimer>
#include <QPainter>
#include <QSet>
#include <QPair>
#include <cmath>
#include <vector>
// #include <opencv2/face/facemark.hpp>
// #include <opencv2/face/facemarkLBF.hpp>
#include "tracker.h"
#include "thread/detector.h"
#include "thread/thread.h"
#include "module/meanshift.h"
#include "conduct/module/utility.h"

CascadeClassifier faceClassifier;
CascadeClassifier eyeClassifier;

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
    this->prepareDetectors();
    Q_ASSERT(faceClassifier.load("C:/Users/Coder/Desktop/qt/Conductor-Simulator/resources/haarcascade_frontalface_alt2.xml"));
    Q_ASSERT(eyeClassifier.load("C:/Users/Coder/Desktop/qt/Conductor-Simulator/resources/haarcascade_eye.xml"));
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
    foreach(auto queue, this->pointQueues){
        queue->clear();
    }
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
    Mat eyes;
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
    eyes = this->detectEyes(frame);
    bitwise_or(frame, actionCanvas, frame);
    bitwise_or(frame, eyes, frame);
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

/*
Mat Tracker::detectEyes(Mat& frame){
    // Create an instance of Facemark
    Ptr<face::Facemark> facemark = face::FacemarkLBF::create();
    // Load landmark detector
    facemark->loadModel("C:/Users/Coder/Desktop/qt/Conductor-Simulator/resources/lbfmodel.yaml");
    // Variable to store a video frame and its grayscale
    Mat gray;
    // Find face
    std::vector<Rect> faces;
    // Convert frame to grayscale because
    // faceDetector requires grayscale image.
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // Detect faces
    faceDetector.detectMultiScale(gray, faces);

    // Variable for landmarks.
    // Landmarks for one face is a vector of points
    // There can be more than one face in the image. Hence, we
    // use a vector of vector of points.
    std::vector<std::vector<Point2f>> landmarks;

    // Run landmark detector
    bool success = facemark->fit(frame,faces,landmarks);

    if(success){
      // If successful, render the landmarks on the face
      for(uint i = 0; i < landmarks.size(); i++){
        drawLandmarks(frame, landmarks[i]);
      }
    }
    return frame;
}
*/

Mat Tracker::detectEyes(Mat& img){
    Mat gray;
    Mat result = Mat::zeros(img.rows, img.cols, img.type());
    cvtColor(img, gray, CV_BGR2GRAY);
    std::vector<Rect> face_pos;
    faceClassifier.detectMultiScale(gray, face_pos, 1.25, 5, 0 | CV_HAAR_SCALE_IMAGE, Size(10, 10));
    for (int i = 0; i < static_cast<int>(face_pos.size()); i++)    {
        rectangle(result, face_pos[i], Scalar(0, 255, 0), 2);
    }
    for (int i = 0; i < static_cast<int>(face_pos.size()); i++) {
        std::vector<Rect> eye_pos;
        Mat roi = gray(face_pos[i]);
        eyeClassifier.detectMultiScale(roi, eye_pos, 1.25, 7, 0 | CV_HAAR_SCALE_IMAGE, Size(10, 10));
        for (int j = 0; j < static_cast<int>(eye_pos.size()); j++) {
            Point center(face_pos[i].x + eye_pos[j].x + (eye_pos[j].width / 2),
                       face_pos[i].y + eye_pos[j].y + (eye_pos[j].height / 2));
            // int radius = 25;
            int radius = cvRound((eye_pos[j].width + eye_pos[j].height) * 0.2);
            circle(result, center, radius, Scalar(0, 0, 255), 2);
        }
    }
    return result;
}
