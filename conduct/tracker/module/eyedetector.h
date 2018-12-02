#ifndef EYEDETECTOR_H
#define EYEDETECTOR_H
#include <windows.h>
#include <QString>
#include <QThread>
#include <opencv2/core/core.hpp>
#include "eyedetector/include/const.h"
#define CREATEPIPE(NAME, dwOenMode, size) \
    CreateNamedPipe(NAME, dwOenMode, PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, size, size, NMPWAIT_USE_DEFAULT_WAIT, nullptr);

using namespace cv;

class EyeDetector : public QThread{
    Q_OBJECT

public:
   static EyeDetector* getInstance(Mat, const QString&, Point*);
   void connect();
   void reconnect();
   void stop();
   void detectEyes(Mat);
   void start() = delete;


private:
   EyeDetector();
   virtual ~EyeDetector() override;
   void set(Mat, const QString&, Point*);
   void run() override;
   int size;
   int rows;
   int cols;
   int type;
   QString path;
   HANDLE mat_pipe; // uchar_t
   HANDLE size_pipe; // size_t
   HANDLE eye_pipe; // Point
   HANDLE row_pipe; // int
   HANDLE col_pipe; // int
   HANDLE type_pipe; // int
   Point* eyes;
   Mat frame;
   PROCESS_INFORMATION pi;
};

#endif // EYEDETECTOR_H
