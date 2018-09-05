#include "thread.h"

CountDataPixel::CountDataPixel(Mat& matrix, QQueue<QPair<int, int>>& dataPixels, int numerator, int denominator) :
    matrix(matrix), dataPixels(dataPixels), numerator(numerator), denominator(denominator){
}

CountDataPixel::~CountDataPixel(){}

void CountDataPixel::run()
{
    uchar r, g, b;
    for (int i = static_cast<int>(matrix.rows * (numerator-1) / denominator); i < static_cast<int>(matrix.rows * numerator / denominator); ++i){
        cv::Vec3b* pixel = matrix.ptr<cv::Vec3b>(i);
        for (int j = 0; j < matrix.cols; ++j){
            r = pixel[j][2];
            g = pixel[j][1];
            b = pixel[j][0];
            if(r != 0 || g != 0 || b != 0){
                this->dataPixels.enqueue(qMakePair(j, i));
            }
        }
    }
}
