#include "config.h"
#include "conduct/module/utility.h"
#include "widget/menu/config/menu_config.h"

const int THREAD_COUNT = 4;

Configuration::Configuration() : ratio(0.5), thresholdVal(20), kernel(3), queueSize(6), history(25), cam(new VideoCapture()), camNumber(0){
}

Configuration::~Configuration(){
    cam->release();
    delete cam;
}

QString Configuration::refreshRgb(){
    this->hsvLowerBound = hsvMult(currentHsv, 1 - ratio / 4, 1 - ratio / 2, 1 - ratio * 1.2);
    this->hsvUpperBound = hsvMult(currentHsv, 1 + ratio / 4, 1 + ratio / 2, 1 + ratio * 1.2);
    this->wideHsvLowerBound = hsvMult(currentHsv, 1 - (ratio + 0.5) / 3, 1 - (ratio + 0.5) / 2, 1 - (ratio + 0.5) * 1.2);
    this->wideHsvUpperBound = hsvMult(currentHsv, 1 + (ratio + 0.5) / 3, 1 + (ratio + 0.5) / 2, 1 + (ratio + 0.5) * 1.2);

    QString text = QString("rgb : (%1, %2, %3) / hsv : (%4, %5, %6) ~ (%7, %8, %9)")
            .arg(this->currentRgb.red()).arg(this->currentRgb.green()).arg(this->currentRgb.blue())
            .arg(hsvLowerBound[0]).arg(hsvLowerBound[1]).arg(hsvLowerBound[2])
            .arg(hsvUpperBound[0]).arg(hsvUpperBound[1]).arg(hsvUpperBound[2]);
    return text;
}

void Configuration::setCurrentRgb(const QColor& rgb){
    this->currentRgb = rgb;
}

void Configuration::setCurrentHsv(const Scalar& hsv){
    this->currentHsv = hsv;
}

double Configuration::getRatio() const{
    return this->ratio;
}

int Configuration::getHistory() const{
    return this->history;
}

int Configuration::getKernel() const{
    return this->kernel;
}

int Configuration::getQueueSize() const{
    return this->queueSize;
}

int Configuration::getThreshold() const{
    return this->thresholdVal;
}

Scalar Configuration::getLowerBound() const{
    return this->hsvLowerBound;
}

Scalar Configuration::getUpperBound() const{
    return this->hsvUpperBound;
}

Scalar Configuration::getWideLowerBound() const{
    return this->wideHsvLowerBound;
}

Scalar Configuration::getWideUpperBound() const{
    return this->wideHsvUpperBound;
}


void Configuration::setRatio(double ratio){
    this->ratio = ratio;
}

void Configuration::setHistory(int history){
    this->history = history;
}

void Configuration::setKernel(int kernel){
    this->kernel = kernel;
}

void Configuration::setQueueSize(int queueSize){
    this->queueSize = queueSize;
}

void Configuration::setThreshold(int thresholdVal){
    this->thresholdVal = thresholdVal;
}

void Configuration::readFrame(Mat& frame){
    Q_ASSERT(cam != nullptr);
    cam->read(frame);
}

void Configuration::cameraOn(){
    cam->open(this->camNumber);
}

void Configuration::cameraOff(){
    cam->release();
}

bool Configuration::isCamOpened(){
    return cam->isOpened();

}

void Configuration::setCamera(int number){
    cam->release();
    cam->open(number);
    this->camNumber = number;
}

void Configuration::setEyeDetectorPath(const QString& path){
    this->eyeDetectorPath = path;
}

QString Configuration::getEyeDetectorPath() const{
    return this->eyeDetectorPath;
}
