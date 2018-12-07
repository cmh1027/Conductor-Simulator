#include <vector>
#include "eyedetector.h"
EyeDetector::EyeDetector(){
    faceDetector = CascadeClassifier("C:/Users/serom/Desktop/qt/Conductor-Simulator/eyedetector/resources/haarcascade_frontalface_alt2.xml");
    facemark = FacemarkLBF::create();
    facemark->loadModel("C:/Users/serom/Desktop/qt/Conductor-Simulator/eyedetector/resources/lbfmodel.yaml");
}

EyeDetector::~EyeDetector(){}

EyeDetector& EyeDetector::getInstance(){
    static EyeDetector instance;
    return instance;
}

void EyeDetector::set(Point* eyes){
    Q_ASSERT(eyes != nullptr);
    this->eyes = eyes;
}

void EyeDetector::detectEyes(Mat frame){
    if(!this->isRunning()){
        this->frame = frame;
        QThread::start();
    }
}

void EyeDetector::run(){
    vector<Rect> faces;
    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    faceDetector.detectMultiScale(gray, faces);
    if(faces.size() != 1) {
        return;
    };
    vector<vector<Point2f>> landmarks;
    bool success = facemark->fit(frame, faces, landmarks);
    if(success){
        if(landmarks.size() == 1)
            *eyes = findEyes(landmarks[0]);
    }
}


Point midpoint(const vector<Point2f>& landmarks, const int start, const int end){
    int x = 0, y = 0;
    for (int i = start; i <= end; i++){
      x += landmarks[i].x;
      y += landmarks[i].y;
    }
    x /= end-start+1;
    y /= end-start+1;
    return Point(x, y);
}


Point findEyes(vector<Point2f>& landmarks){
    int x = 0, y = 0;
    if (landmarks.size() == 68){
      auto left_eye = midpoint(landmarks, 36, 41);
      auto right_eye = midpoint(landmarks, 42, 47);
      x = (left_eye.x + right_eye.x) / 2;
      y = (left_eye.y + right_eye.y) / 2;
    }
    return Point(x, y);
}
