#include "eyedetector.h"
EyeDetector::EyeDetector(){}

EyeDetector::~EyeDetector(){
    CloseHandle(mat_pipe);
    CloseHandle(eye_pipe);
}

void EyeDetector::set(Mat frame, const QString& path, Point* eyes){
    Q_ASSERT(eyes != nullptr);
    this->size = static_cast<int>(frame.step[0]) * frame.rows;
    this->rows = frame.rows;
    this->cols = frame.cols;
    this->type = frame.type();
    this->path = path;
    this->eyes = eyes;
}

void EyeDetector::detectEyes(Mat frame){
    if(!this->isRunning()){
        this->frame = frame;
        this->start();
    }
}

void EyeDetector::run(){
    Point eyes;
    WriteFile(this->mat_pipe, reinterpret_cast<char*>(frame.data), static_cast<DWORD>(this->size), nullptr, nullptr);
    if(PeekNamedPipe(this->eye_pipe, nullptr, 0, nullptr, nullptr, nullptr)){
       ReadFile(this->eye_pipe, reinterpret_cast<char*>(&eyes), sizeof(Point), nullptr, nullptr);
       *this->eyes = eyes;
    }
    else{
       DWORD dwError = GetLastError();
       if(dwError == ERROR_BROKEN_PIPE)
           this->reconnect();
    }
}

EyeDetector* EyeDetector::getInstance(Mat frame, const QString& path, Point* eyes){
    static EyeDetector* instance = nullptr;
    if(instance == nullptr)
        instance = new EyeDetector();
    else
        instance->stop();
    instance->set(frame, path, eyes);
    return instance;
}

void EyeDetector::connect(){
    mat_pipe = CREATEPIPE(MATNAME, PIPE_ACCESS_OUTBOUND, static_cast<DWORD>(size));
    size_pipe = CREATEPIPE(MATSIZENAME, PIPE_ACCESS_OUTBOUND, sizeof(int));
    eye_pipe = CREATEPIPE(EYENAME, PIPE_ACCESS_INBOUND, sizeof(Point));
    row_pipe = CREATEPIPE(ROWNAME, PIPE_ACCESS_OUTBOUND, sizeof(int));
    col_pipe = CREATEPIPE(COLNAME, PIPE_ACCESS_OUTBOUND, sizeof(int));
    type_pipe = CREATEPIPE(TYPENAME, PIPE_ACCESS_OUTBOUND, sizeof(int));
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si);
    wchar_t* file = new wchar_t[path.length()+1];
    path.toWCharArray(file);
    file[path.length()] = 0;
    CreateProcess(nullptr, file, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
    delete[] file;
    ConnectNamedPipe(mat_pipe, nullptr);
    ConnectNamedPipe(size_pipe, nullptr);
    ConnectNamedPipe(eye_pipe, nullptr);
    ConnectNamedPipe(row_pipe, nullptr);
    ConnectNamedPipe(col_pipe, nullptr);
    ConnectNamedPipe(type_pipe, nullptr);
    WriteFile(size_pipe, reinterpret_cast<char*>(&size), sizeof(int), nullptr, nullptr);
    WriteFile(row_pipe, reinterpret_cast<char*>(&rows), sizeof(int), nullptr, nullptr);
    WriteFile(col_pipe, reinterpret_cast<char*>(&cols), sizeof(int), nullptr, nullptr);
    WriteFile(type_pipe, reinterpret_cast<char*>(&type), sizeof(int), nullptr, nullptr);
    CloseHandle(size_pipe);
    CloseHandle(row_pipe);
    CloseHandle(col_pipe);
    CloseHandle(type_pipe);
}

void EyeDetector::reconnect(){
    stop();
    start();
}

void EyeDetector::stop(){
    CloseHandle(mat_pipe);
    CloseHandle(eye_pipe);
    CloseHandle(size_pipe);
    CloseHandle(row_pipe);
    CloseHandle(col_pipe);
    CloseHandle(type_pipe);
}
