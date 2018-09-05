#ifndef MEANSHIF_H
#define MEANSHIF_H
#include <QSet>
#include <opencv2/core/core.hpp>
using namespace cv;

void meanShift(QSet<QPair<int, int>>&, int = 10);
void meanShift(QSet<QPair<int, int>>&, const Point&, int = 10);
#endif // MEANSHIF_H
