#ifndef EYEDETECTOR_H
#define EYEDETECTOR_H
#include <QThread>
#include <opencv2/core/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;
using namespace cv::face;

class EyeDetector : public QThread{
    Q_OBJECT

public:
   static EyeDetector& getInstance(Point*);
   void detectEyes(Mat);
   void start() = delete;


private:
   EyeDetector();
   virtual ~EyeDetector() override;
   void set(Point*);
   void run() override;
   Point* eyes;
   Mat frame;
   CascadeClassifier faceDetector;
   Ptr<Facemark> facemark;
};

Point midpoint(const vector<Point2f>&, const int, const int);
Point findEyes(vector<Point2f>&);
#endif // EYEDETECTOR_H
