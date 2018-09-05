#include <algorithm>
#include "pointqueue.h"
PointQueue::PointQueue() : QQueue<Point>(), maximumX(-1), minimumX(-1), maximumY(-1), minimumY(-1)
{}

int PointQueue::minX(){
    return this->minimumX;
}


int PointQueue::maxX(){
    return this->maximumX;
}


int PointQueue::minY(){
    return this->minimumY;
}

int PointQueue::maxY(){
    return this->maximumY;
}

Point PointQueue::minXPoint(){
    return this->minimumXPoint;
}

Point PointQueue::maxXPoint(){
    return this->maximumXPoint;
}

Point PointQueue::minYPoint(){
    return this->minimumYPoint;
}
Point PointQueue::maxYPoint(){
    return this->maximumXPoint;
}

void PointQueue::refreshMinMax(){
    QQueue sortQueue(*this);
    if(this->count() == 0){
        this->clearMinMax();
    }
    else{
        auto begin = (*(this->begin()));
        this->maximumX = begin.x;
        this->minimumX = begin.x;
        this->maximumY = begin.y;
        this->minimumY = begin.y;
        this->maximumXPoint = begin;
        this->minimumXPoint = begin;
        this->maximumYPoint = begin;
        this->minimumYPoint = begin;
    }
    for(auto it = this->begin() + 1; it != this->end(); ++it){
        if(this->maximumX < (*it).x){
            this->maximumX = (*it).x;
            this->maximumXPoint.x = (*it).x;
            this->maximumXPoint.y = (*it).y;
        }
        if(this->minimumX > (*it).x){
            this->minimumX = (*it).x;
            this->minimumXPoint.x = (*it).x;
            this->minimumXPoint.y = (*it).y;
        }
        if(this->maximumY < (*it).y){
            this->maximumY = (*it).y;
            this->maximumYPoint.x = (*it).x;
            this->maximumYPoint.y = (*it).y;
        }
        if(this->minimumY > (*it).y){
            this->minimumY = (*it).y;
            this->minimumYPoint.x = (*it).x;
            this->minimumYPoint.y = (*it).y;
        }
    }
}

void PointQueue::clearMinMax(){
    this->maximumX = -1;
    this->minimumX = -1;
    this->maximumY = -1;
    this->minimumY = -1;
    this->maximumXPoint.x = -1;
    this->maximumXPoint.y = -1;
    this->minimumXPoint.x = -1;
    this->minimumXPoint.y = -1;
    this->maximumYPoint.x = -1;
    this->maximumYPoint.y = -1;
    this->minimumYPoint.x = -1;
    this->minimumYPoint.y = -1;
}

void PointQueue::setMinMax(const Point& point){
    if(this->maximumX == -1){
        this->maximumX = point.x;
        this->maximumXPoint = point;
    }
    else{
        if(this->maximumX < point.x){
            this->maximumX = point.x;
            this->maximumXPoint.x= point.x;
            this->maximumXPoint.y = point.y;
        }
    }
    if(this->minimumX == -1){
        this->minimumX = point.x;
        this->minimumXPoint = point;
    }
    else{
        if(this->minimumX > point.x){
            this->minimumX = point.x;
            this->minimumXPoint.x= point.x;
            this->minimumXPoint.y = point.y;
        }
    }
    if(this->maximumY == -1){
        this->maximumY = point.y;
        this->maximumYPoint = point;
    }
    else{
        if(this->maximumY < point.y){
            this->maximumY = point.y;
            this->maximumYPoint.x= point.x;
            this->maximumYPoint.y = point.y;
        }
    }
    if(this->minimumY == -1){
        this->minimumY = point.y;
        this->minimumYPoint = point;
    }
    else{
        if(this->minimumY > point.y){
            this->minimumY = point.y;
            this->minimumYPoint.x= point.x;
            this->minimumYPoint.y = point.y;
        }
    }
}

void PointQueue::enqueue(const Point& point){
    QQueue<Point>::enqueue(point);
    this->setMinMax(point);
}

Point PointQueue::dequeue(){
    Point&& point = QQueue<Point>::dequeue();
    this->refreshMinMax();
    return point;
}

iter PointQueue::erase(iter it){
    iter&& res = QQueue<Point>::erase(it);
    this->refreshMinMax();
    return res;
}

iter PointQueue::erase(iter it1, iter it2){
    iter&& res = QQueue<Point>::erase(it1, it2);
    this->refreshMinMax();
    return res;
}

void PointQueue::clear(){
    QQueue<Point>::clear();
    this->clearMinMax();
}

iter PointQueue::begin(){
    return QQueue<Point>::begin();
}

iter PointQueue::end(){
    return QQueue<Point>::end();
}

const_iter PointQueue::begin() const{
    return QQueue<Point>::begin();
}

const_iter PointQueue::end() const{
    return QQueue<Point>::end();
}

int PointQueue::size() const{
    return QQueue<Point>::size();
}

int PointQueue::count() const{
    return QQueue<Point>::count();
}

int PointQueue::length() const{
    return QQueue<Point>::length();
}
