#ifndef UTILITY_H
#define UTILITY_H
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <QPixmap>
#include <QQueue>
extern const int THREAD_COUNT;
using namespace cv;
class PointQueue;

QPixmap mat2QPixmap(const cv::Mat&, const QImage::Format&);
Scalar rgbTohsv(double, double, double);
Scalar hsvMult(const Scalar&, double, double, double);
void drawPoints(Mat& canvas, const PointQueue& queue, const Scalar& scalar = Scalar(255, 0, 0));
void addPoint(PointQueue&, const Point&);
void addPoint(PointQueue&, int, const Point&);
void findDataPixels(Mat&, QQueue<Point>&);
void findDataPixels(Mat&, QSet<QPair<int, int>>&);
Mat fillObject(Mat&, QSet<QPair<int, int>>&, const Scalar&, const Scalar&, int = COLOR_BGR2HSV);
Mat remainPixelsInCircle(const Point&, int, Mat);
void findPixelsInCircle(const Point, int, const QSet<QPair<int, int>>&, QQueue<QPair<int, int>>&);
void findPixelsInCircle(const QPair<int, int>&, int, const QSet<QPair<int, int>>&, QQueue<QPair<int, int>>&);
QPair<int, int> findCentroid(const QQueue<QPair<int, int>>&);
QPair<int, int> findCentroid(const QSet<QPair<int, int>>&);
#endif // UTILITY_H
