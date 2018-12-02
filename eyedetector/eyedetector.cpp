#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <windows.h>
#include "include/const.h"
#include "include/detectEyes.hpp"
#include <stdio.h>

using namespace cv;
using namespace cv::face;

int main(int argc, char *argv[]){
    CascadeClassifier faceDetector("C:/Users/Coder/Desktop/qt/Conductor-Simulator/eyedetector/resources/haarcascade_frontalface_alt2.xml");
    Ptr<Facemark> facemark = FacemarkLBF::create();
    facemark->loadModel("C:/Users/Coder/Desktop/qt/Conductor-Simulator/eyedetector/resources/lbfmodel.yaml");
    struct Pipes{
        int size;
        int rows;
        int cols;
        int type;
        HANDLE mat_pipe; // uchar_t
        HANDLE size_pipe; // size_t
        HANDLE eye_pipe; // Point
        HANDLE row_pipe; // int
        HANDLE col_pipe; // int
        HANDLE type_pipe; // int
        Pipes(){
            while(1){
                mat_pipe = CONNECTPIPE(MATNAME, GENERIC_READ);
                size_pipe = CONNECTPIPE(MATSIZENAME, GENERIC_READ);
                eye_pipe = CONNECTPIPE(EYENAME, GENERIC_WRITE);
                row_pipe = CONNECTPIPE(ROWNAME, GENERIC_READ);
                col_pipe = CONNECTPIPE(COLNAME, GENERIC_READ);
                type_pipe = CONNECTPIPE(TYPENAME, GENERIC_READ);
                printf("%d %d %d %d %d %d\n", mat_pipe, size_pipe, eye_pipe, row_pipe, col_pipe, type_pipe);
                if(mat_pipe != INVALID_HANDLE_VALUE && size_pipe != INVALID_HANDLE_VALUE && eye_pipe != INVALID_HANDLE_VALUE &&
                row_pipe != INVALID_HANDLE_VALUE && col_pipe != INVALID_HANDLE_VALUE && type_pipe != INVALID_HANDLE_VALUE) 
                break;
            }
            ReadFile(size_pipe, (char*)&size, sizeof(int), NULL, NULL);
            ReadFile(row_pipe, (char*)&rows, sizeof(int), NULL, NULL);
            ReadFile(col_pipe, (char*)&cols, sizeof(int), NULL, NULL);
            ReadFile(type_pipe, (char*)&type, sizeof(int), NULL, NULL);
            CloseHandle(size_pipe);
            CloseHandle(row_pipe);
            CloseHandle(col_pipe);
            CloseHandle(type_pipe);
        }
        ~Pipes(){
            CloseHandle(mat_pipe);
            CloseHandle(eye_pipe);
        }
    } pipes;
    Mat frame, gray;
    frame = Mat(pipes.rows, pipes.cols, pipes.type);
    Point eyes;
    while(1){
        if(PeekNamedPipe(pipes.mat_pipe, NULL, NULL, NULL, NULL, NULL)){
            ReadFile(pipes.mat_pipe, (char*)frame.data, pipes.size, NULL, NULL);
        }
        else{
            DWORD dwError = GetLastError();
            if(dwError == ERROR_BROKEN_PIPE) 
                break;
            else{
                WriteFile(pipes.eye_pipe, (char*)&eyes, sizeof(Point), NULL, NULL);
                continue;
            }
        }
        vector<Rect> faces;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        faceDetector.detectMultiScale(gray, faces);
        if(faces.size() != 1) {
            WriteFile(pipes.eye_pipe, (char*)&eyes, sizeof(Point), NULL, NULL); // last cooridnate
            continue;
        };
        vector<vector<Point2f>> landmarks;
        bool success = facemark->fit(frame, faces, landmarks);
        if(success){
            if(landmarks.size() == 1)
                eyes = findEyes(landmarks[0]);
            WriteFile(pipes.eye_pipe, (char*)&eyes, sizeof(Point), NULL, NULL);
        }
    }
    return 0;
}