#ifndef POINTQUEUE_H
#define POINTQUEUE_H
#include <QQueue>
#include <opencv2/core/core.hpp>
using namespace cv;
using iter = QList<Point>::iterator;
using const_iter = QList<Point>::const_iterator;
class PointQueue : private QQueue<Point>{
public:
    PointQueue();
    virtual ~PointQueue() = default;
    int minX();
    int maxX();
    int minY();
    int maxY();
    Point minXPoint();
    Point maxXPoint();
    Point minYPoint();
    Point maxYPoint();
    void refreshMinMax();
    void clearMinMax();
    void setMinMax(const Point&);
    void enqueue(const Point&);
    Point dequeue();
    void clear();
    iter erase(iter);
    iter erase(iter, iter);
    iter begin();
    const_iter begin() const;
    iter end();
    const_iter end() const;
    int size() const;
    int count() const;
    int length() const;
private:
    int maximumX;
    int minimumX;
    int maximumY;
    int minimumY;
    Point maximumXPoint;
    Point minimumXPoint;
    Point maximumYPoint;
    Point minimumYPoint;
};
#endif // POINTQUEUE_H
