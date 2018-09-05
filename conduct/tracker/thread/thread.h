#ifndef THREAD_H
#define THREAD_H
#include <QRunnable>
#include <QQueue>
#include <QPair>
#include <opencv2/core/core.hpp>

using namespace cv;

class CountDataPixel : public QRunnable
{

public:
    CountDataPixel(Mat&, QQueue<QPair<int, int>>&, int, int);
    virtual ~CountDataPixel();

private:
    void run();
    Mat& matrix;
    QQueue<QPair<int, int>>& dataPixels;
    int numerator;
    int denominator;

};


#endif // THREAD_H
