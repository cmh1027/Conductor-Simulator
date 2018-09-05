#include "meanshift.h"
#include "conduct/module/utility.h"
#include <QQueue>
#include <cmath>
void meanShift(QSet<QPair<int, int>>& dataPixels, int radian){
    if(dataPixels.size() == 0)
        return;
    QQueue<QPair<int, int>> pixelsInCircle;
    while(pixelsInCircle.size() < dataPixels.size() / 5){
        for(auto it = pixelsInCircle.begin(); it != pixelsInCircle.end(); ++it){
            dataPixels.remove((*it));
        };
        QPair<int, int> center = *(dataPixels.begin());
        pixelsInCircle.clear();
        while(true){
            findPixelsInCircle(center, radian, dataPixels, pixelsInCircle);
            QPair<int, int> centroid = findCentroid(pixelsInCircle);
            if(std::pow(center.first - centroid.first, 2) + std::pow(center.second - centroid.second, 2) > 9){
                center = centroid;
                pixelsInCircle.clear();
            }
            else
                break;
        }
    }
}

void meanShift(QSet<QPair<int, int>>& dataPixels, const Point& lastPoint, int radian){
    if(dataPixels.size() == 0)
        return;
    QQueue<QPair<int, int>> pixelsInCircle;
    QPair<int, int> center = qMakePair(lastPoint.x, lastPoint.y);
    bool set = true;
    while(pixelsInCircle.size() < dataPixels.size() / 5){
        for(auto it = pixelsInCircle.begin(); it != pixelsInCircle.end(); ++it){
            dataPixels.remove((*it));
        };
        if(!set)
            center = *(dataPixels.begin());
        pixelsInCircle.clear();
        while(true){
            findPixelsInCircle(center, radian, dataPixels, pixelsInCircle);
            if(pixelsInCircle.empty()){
                center = *(dataPixels.begin());
            }
            else{
                QPair<int, int> centroid = findCentroid(pixelsInCircle);
                if(std::pow(center.first - centroid.first, 2) + std::pow(center.second - centroid.second, 2) > 9){
                    center = centroid;
                    pixelsInCircle.clear();
                }
                else
                    break;
            }
        }
        set = true;
    }
}

