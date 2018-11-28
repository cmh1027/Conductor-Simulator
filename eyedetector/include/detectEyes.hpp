#ifndef _detectEyes_H_
#define _detectEyes_H_
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv; 
using namespace std; 

#define COLOR Scalar(255, 200,0)

Point midpoint(const vector<Point2f> &landmarks, const int start, const int end){
  int x = 0, y = 0;
  for (int i = start; i <= end; i++){
    x += landmarks[i].x;
    y += landmarks[i].y;
  }
  x /= end-start+1;
  y /= end-start+1;
  return Point(x, y);
}


Point findEyes(vector<Point2f> &landmarks){
  int x, y;
  if (landmarks.size() == 68){
    auto left_eye = midpoint(landmarks, 36, 41);    
    auto right_eye = midpoint(landmarks, 42, 47);
    x = (left_eye.x + right_eye.x) / 2;
    y = (left_eye.y + right_eye.y) / 2;
  }
  return Point(x, y);
}

Point findJaw(vector<Point2f> &landmarks){
  int x, y;
  if (landmarks.size() == 68){
    auto nose = midpoint(landmarks, 0, 16);
    x = nose.x;
    y = nose.y;
  }
  return Point(x, y);
}


#endif 