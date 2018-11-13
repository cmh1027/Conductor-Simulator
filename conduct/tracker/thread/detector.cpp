#include "detector.h"
#include "../../module/utility.h"
#include "../module/pointqueue.h"
#include "conduct/command/command.h"
#include <cmath>

Detector::Detector() : queue(nullptr), queueSize(-1), count(0){}

void Detector::setQueue(PointQueue* queue){
    this->queue = queue;
}

void Detector::setQueueSize(int queueSize){
    this->queueSize = queueSize;
}

void Detector::setData(Point& point, int count, Mat& canvas){
    this->count = count;
    this->point = point;
    this->canvas = canvas;
}

void Detector::run()
{
    Q_ASSERT(queue != nullptr);
    if(count > 0){
        if(this->queueSize <= 0)
            addPoint(this->queue, this->point);
        else
            addPoint(this->queue, this->queueSize, this->point);
    }
    this->check(this->queue);
}

void AccentDetector::check(PointQueue* queue)
{
    if(queue->length() < 2)
        return;
    auto tail = queue->end() - 1;
    auto tail2 = queue->end() - 2;
    if(queue->maxY() - queue->minY() > 300 && (*tail).y < (*tail2).y){
        queue->clear();
        line(this->canvas, queue->minXPoint(), queue->maxXPoint(), Scalar(255, 255, 0));
        emit this->detected(Command::Accent);
    }
}

void ShortAccentDetector::check(PointQueue* queue)
{
    if(queue->length() < 2)
        return;
    auto tail = queue->end() - 1;
    auto tail2 = queue->end() - 2;
    if(queue->maxY() - queue->minY() > 200 && (*tail).y < (*tail2).y){
        queue->clear();
        line(this->canvas, queue->minXPoint(), queue->maxXPoint(), Scalar(255, 255, 0));
        emit this->detected(Command::ShortAccent);
    }
}

void ReverseAccentDetector::check(PointQueue* queue)
{
    if(queue->length() < 2)
        return;
    auto tail = queue->end() - 1;
    auto tail2 = queue->end() - 2;
    if(queue->maxY() - queue->minY() > 250 && (*tail).y > (*tail2).y){
        queue->clear();
        line(this->canvas, queue->minXPoint(), queue->maxXPoint(), Scalar(255, 255, 0));
        emit this->detected(Command::ReverseAccent);
    }
}

void VerticalBeatDetector::check(PointQueue* queue)
{
    if(queue->length() < 4)
        return;
    int first = (*(queue->end()-1)).y - (*(queue->end()-2)).y;
    int third = (*(queue->end()-3)).y - (*(queue->end()-4)).y;
    int distance = queue->maxY() - queue->minY();
    if(first * third < 0 && first > 0 && distance > 70){
        line(this->canvas, (*(queue->end()-1)), (*(queue->end()-2)), Scalar(0, 255, 0));
        line(this->canvas, (*(queue->end()-2)), (*(queue->end()-3)), Scalar(0, 255, 0));
        line(this->canvas, (*(queue->end()-3)), (*(queue->end()-4)), Scalar(0, 255, 0));
        queue->clear();
        emit this->detected(Command::Vertical, distance);
    }
}


void HorizontalBeatDetector::check(PointQueue* queue)
{
    if(queue->length() < 4)
        return;
    int first = (*(queue->end()-1)).x - (*(queue->end()-2)).x;
    int third = (*(queue->end()-3)).x - (*(queue->end()-4)).x;
    int distance = queue->maxX() - queue->minX();
    if(first * third < 0 && distance > 70){
        line(this->canvas, (*(queue->end()-1)), (*(queue->end()-2)), Scalar(0, 255, 0));
        line(this->canvas, (*(queue->end()-2)), (*(queue->end()-3)), Scalar(0, 255, 0));
        line(this->canvas, (*(queue->end()-3)), (*(queue->end()-4)), Scalar(0, 255, 0));
        queue->clear();
        emit this->detected(Command::Horizontal, distance);
    }
}


void WhipDetector::check(PointQueue* queue)
{
    if(queue->maxX() - queue->minX() > 300 && queue->maxY() - queue->minY() < 70){
        queue->clear();
        line(this->canvas, queue->minXPoint(), queue->maxXPoint(), Scalar(255, 255, 0));
        emit this->detected(Command::Whip);
    }
}

void DiagonalWhipDetector::check(PointQueue* queue)
{
    if(queue->maxX() - queue->minX() > 300 && queue->maxY() - queue->minY() > 150){
        queue->clear();
        line(this->canvas, queue->minXPoint(), queue->maxXPoint(), Scalar(255, 255, 0));
        emit this->detected(Command::DiagonalWhip);
    }
}

void VerticalSwingDetector::check(PointQueue* queue)
{
    if(queue->length() < 2)
        return;
    auto tail = queue->end() - 1;
    auto tail2 = queue->end() - 2;
    if(std::abs((*tail).y - (*tail2).y) < 10 && queue->maxY() - queue->minY() > 80){
        queue->clear();
        line(this->canvas, queue->minYPoint(), queue->maxYPoint(), Scalar(255, 127, 127));
        emit this->detected(Command::VerticalSwing);
    }
}

void HorizontalSwingDetector::check(PointQueue* queue)
{
    if(queue->length() < 2)
        return;
    auto tail = queue->end() - 1;
    auto tail2 = queue->end() - 2;
    if(std::abs((*tail).x - (*tail2).x) < 10 && queue->maxX() - queue->minX() > 80){
        queue->clear();
        line(this->canvas, queue->minXPoint(), queue->maxXPoint(), Scalar(127, 127, 255));
        emit this->detected(Command::HorizontalSwing);
    }
}

