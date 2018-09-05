#include "detector.h"
#include "../../module/utility.h"
#include "../module/pointqueue.h"
#include <cmath>
Detector::Detector(PointQueue& queue, int queueSize, const Point& point, int count, Mat& canvas, bool drawFlag) :
    queue(queue), queueSize(queueSize), count(count), point(point), canvas(canvas), drawFlag(drawFlag)
{}


void Detector::run()
{
    if(count > 0){
        if(this->queueSize <= 0)
            addPoint(this->queue, point);
        else
            addPoint(this->queue, this->queueSize, point);
    }
    this->check(this->queue);
}

void AccentDetector::check(PointQueue& queue)
{
    if(drawFlag){
        drawPoints(canvas, this->queue, Scalar(255, 0, 0));
    }
    if(queue.length() < 3)
        return;
    auto tail = queue.end() - 1;
    auto tail2 = queue.end() - 2;
    if(queue.maxY() - queue.minY() > 370 && (*tail).y < (*tail2).y){
        queue.clear();
        line(this->canvas, queue.minXPoint(), queue.maxXPoint(), Scalar(255, 255, 0));
        emit this->detected();
    }
}

void ReverseAccentDetector::check(PointQueue& queue)
{
    if(drawFlag){
        drawPoints(canvas, this->queue, Scalar(255, 0, 0));
    }
    if(queue.length() < 3)
        return;
    auto tail = queue.end() - 1;
    auto tail2 = queue.end() - 2;
    if(queue.maxY() - queue.minY() > 370 && (*tail).y > (*tail2).y){
        queue.clear();
        line(this->canvas, queue.minXPoint(), queue.maxXPoint(), Scalar(255, 255, 0));
        emit this->detected();
    }
}

void VerticalBeatDetector::check(PointQueue& queue)
{
    if(drawFlag){
        drawPoints(canvas, this->queue, Scalar(0, 255, 0));
    }
    if(queue.length() < 4)
        return;
    int first = (*(queue.end()-1)).y - (*(queue.end()-2)).y;
    int third = (*(queue.end()-3)).y - (*(queue.end()-4)).y;
    int distance = queue.maxY() - queue.minY();
    if(first * third < 0 && first > 0 && distance > 70){
        line(this->canvas, (*(queue.end()-1)), (*(queue.end()-2)), Scalar(0, 255, 0));
        line(this->canvas, (*(queue.end()-2)), (*(queue.end()-3)), Scalar(0, 255, 0));
        line(this->canvas, (*(queue.end()-3)), (*(queue.end()-4)), Scalar(0, 255, 0));
        queue.clear();
        emit this->detected(distance);
    }
}


void HorizontalBeatDetector::check(PointQueue& queue)
{
    if(drawFlag){
        drawPoints(canvas, this->queue, Scalar(0, 0, 255));
    }
    if(queue.length() < 4)
        return;
    int first = (*(queue.end()-1)).x - (*(queue.end()-2)).x;
    int third = (*(queue.end()-3)).x - (*(queue.end()-4)).x;
    int distance = queue.maxX() - queue.minX();
    if(first * third < 0 && distance > 70){
        line(this->canvas, (*(queue.end()-1)), (*(queue.end()-2)), Scalar(0, 255, 0));
        line(this->canvas, (*(queue.end()-2)), (*(queue.end()-3)), Scalar(0, 255, 0));
        line(this->canvas, (*(queue.end()-3)), (*(queue.end()-4)), Scalar(0, 255, 0));
        queue.clear();
        emit this->detected(distance);
    }
}


void WhipDetector::check(PointQueue& queue)
{
    if(drawFlag){
        drawPoints(canvas, this->queue, Scalar(255, 255, 0));
    }
    if(queue.maxX() - queue.minX() > 370 && queue.maxY() - queue.minY() < 70){
        queue.clear();
        line(this->canvas, queue.minXPoint(), queue.maxXPoint(), Scalar(255, 255, 0));
        emit this->detected();
    }
}
