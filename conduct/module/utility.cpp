#include <QImage>
#include <QSet>
#include <QPair>
#include <QThreadPool>
#include "utility.h"
#include "../tracker/module/pointqueue.h"
#include "../tracker/thread/thread.h"
QPixmap mat2QPixmap(const cv::Mat& mat, const QImage::Format& format){
    QImage image(static_cast<unsigned char*>(mat.data), mat.cols, mat.rows, format);
    return QPixmap::fromImage(image);
}

Scalar rgbTohsv(double r, double g, double b){
    double max = std::max(std::max(r, g), b);
    double min = std::min(std::min(r, g), b);
    double v = max;
    double s = (max != 0.0) ? (max - min) / max : 0.0;
    double h;
    if(s == 0.0)
        h = 0;
    else{
        double delta = max - min;
        if(r == max)
            h = (g - b) / delta;
        else if(g == max)
            h = 2 + (b - r) / delta;
        else if(b == max)
            h = 4 + (r - g) / delta;
        h *= 60.0;
        if(h < 0.0)
            h += 360.0;
    }
    return Scalar(static_cast<int>(h/2), static_cast<int>(s*255), static_cast<int>(v));
}

Scalar hsvMult(const Scalar& scalar, double hRatio, double sRatio, double vRatio){
    int h = static_cast<int>(scalar[0]*hRatio);
    int s = static_cast<int>(scalar[1]*sRatio);
    int v = static_cast<int>(scalar[2]*vRatio);
    if(h > 180)
        h = 180;
    if(h < 0)
        h = 0;
    if(s > 255)
        s = 255;
    if(s < 0)
        s = 0;
    if(v > 255)
        v = 255;
    if(v < 0)
        v = 0;
    return Scalar(h, s, v);
}

void drawPoints(Mat& canvas, const PointQueue* queue, const Scalar& scalar){
    for(auto it = queue->begin(); it != queue->end(); it++){
        circle(canvas, (*it), 2, scalar, CV_FILLED);
    }
}

void drawPoints(Mat& canvas, const PointQueue& queue, const Scalar& scalar){
    for(auto it = queue.begin(); it != queue.end(); it++){
        circle(canvas, (*it), 2, scalar, CV_FILLED);
    }
}

void addPoint(PointQueue* queue, const Point& point){
    queue->enqueue(point);
}


void addPoint(PointQueue* queue, int size, const Point& point){
    if(queue->size() > size){
        while(queue->size() > size)
            queue->dequeue();
    }
    queue->enqueue(point);
}

void addPoint(PointQueue& queue, const Point& point){
    queue.enqueue(point);
}


void addPoint(PointQueue& queue, int size, const Point& point){
    if(queue.size() > size){
        while(queue.size() > size)
            queue.dequeue();
    }
    queue.enqueue(point);
}

void findDataPixels(Mat& mat, QQueue<Point>& dataPixels){
    int channels = mat.channels();
    uchar* data = mat.data;
    for (int i = 0; i < mat.rows; ++i){
        for (int j = 0; j < mat.cols; ++j){
            bool isBlack = false;
            for(int k = 0; k < channels; ++k){
                if(data[i * static_cast<int>(mat.step) + j * channels + k] != 0){
                    isBlack = true;
                    break;
                }
            }
            if(isBlack){
                dataPixels.enqueue(Point(j, i));
            }
        }
    }
}

void findDataPixels(Mat& mat, QSet<QPair<int, int>>& dataPixels){
    int channels = mat.channels();
    uchar* data = mat.data;
    for (int i = 0; i < mat.rows; ++i){
        for (int j = 0; j < mat.cols; ++j){
            bool isBlack = false;
            for(int k = 0; k < channels; ++k){
                if(data[i * static_cast<int>(mat.step) + j * channels + k] != 0){
                    isBlack = true;
                    break;
                }
            }
            if(isBlack){
                dataPixels.insert(qMakePair(j, i));
            }
        }
    }
    /* threaded version
    QQueue<QQueue<QPair<int, int>>*> pixelQueue;
    for(int i = 1; i <= THREAD_COUNT; ++i){
        auto queue = new QQueue<QPair<int, int>>();
        pixelQueue.push_back(queue);
        auto pixelThread = new CountDataPixel(mat, *queue, i, THREAD_COUNT);
        QThreadPool::globalInstance()->start(pixelThread);
    }
    QThreadPool::globalInstance()->waitForDone();
    for(auto it = pixelQueue.begin(); it != pixelQueue.end(); ++it){
        for(auto it2 = (*it)->begin(); it2 != (*it)->end(); ++it2){
            dataPixels.insert(*it2);
        }
        delete *it;
    }
    */
}


Mat fillObject(Mat& frame, QSet<QPair<int, int>>& dataPixels, const Scalar& lowerBound, const Scalar& upperBound, int conversion){
    Mat mask = frame.clone();
    mask = Mat::zeros(mask.rows, mask.cols, mask.type());
    int distance = 7;
    uchar* data = mask.data;
    int channel = mask.channels();
    for(auto it = dataPixels.begin(); it != dataPixels.end(); ++it){
        for(int x = -distance; x <= distance; ++x){
            for(int y = -distance; y <= distance; ++y){
                if((*it).first + x >= 0 && (*it).first + x < mask.cols && (*it).second + y >= 0 && (*it).second + y < mask.rows){
                    for(auto i = 0; i < channel; ++i)
                        data[((*it).second + y) * static_cast<int>(mask.step) + ((*it).first + x) * channel + i] = 255;
                }
            }
        }
    }
    bitwise_and(frame, mask, mask);
    cvtColor(mask, mask, conversion);
    inRange(mask, lowerBound, upperBound, mask);
    cvtColor(mask, mask, COLOR_GRAY2BGR);
    bitwise_and(frame, mask, mask);
    return mask;
}


Mat remainPixelsInCircle(const Point& center, int radian, Mat matrix){
    Mat result = matrix.clone();
    result = Mat::zeros(result.rows, result.cols, result.type());
    QQueue<Point> dataPixels;
    findDataPixels(matrix, dataPixels);
    uchar* data = result.data;
    int channels = result.channels();
    while(!dataPixels.empty()){
        auto point = dataPixels.dequeue();
        if(std::pow(point.x - center.x, 2) + std::pow(point.y - center.y, 2) <= std::pow(radian, 2)){
            for(int i = 0; i < channels; ++i){
                data[point.y * static_cast<int>(result.step) + point.x * channels + i] = 255;
            }
        }
    }

    return result;
}

void findPixelsInCircle(const Point& center, int radian, const QSet<QPair<int, int>>& dataPixels, QQueue<QPair<int, int>>& pixelsInCircle){
    for(auto it = dataPixels.begin(); it != dataPixels.end(); ++it){
        if(std::pow((*it).first - center.x, 2) + std::pow((*it).second - center.y, 2) <= std::pow(radian, 2)){
            pixelsInCircle.enqueue((*it));
        }
    }
}

void findPixelsInCircle(const QPair<int, int>& center, int radian, const QSet<QPair<int, int>>& dataPixels, QQueue<QPair<int, int>>& pixelsInCircle){
    for(auto it = dataPixels.begin(); it != dataPixels.end(); ++it){
        if(std::pow((*it).first - center.first, 2) + std::pow((*it).second - center.second, 2) <= std::pow(radian, 2)){
            pixelsInCircle.enqueue((*it));
        }
    }
}

QPair<int, int> findCentroid(const QQueue<QPair<int, int>>& pixelsInCircle){
    int x = 0, y = 0;
    for(auto it = pixelsInCircle.begin(); it != pixelsInCircle.end(); ++it){
        x += (*it).first;
        y += (*it).second;
    }
    Q_ASSERT(pixelsInCircle.size() != 0);
    return qMakePair(static_cast<int>(x/pixelsInCircle.size()), static_cast<int>(y/pixelsInCircle.size()));
}

QPair<int, int> findCentroid(const QSet<QPair<int, int>>& dataPixels){
    int x = 0, y = 0;
    for(auto it = dataPixels.begin(); it != dataPixels.end(); ++it){
        x += (*it).first;
        y += (*it).second;
    }
    Q_ASSERT(dataPixels.size() != 0);
    return qMakePair(static_cast<int>(x/dataPixels.size()), static_cast<int>(y/dataPixels.size()));
}

