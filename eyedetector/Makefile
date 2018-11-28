CC=g++
NAME=eyedetector
INCLUDE += -ID:/opencv_build_4.0.0_64bit/install/include
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_core400
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_highgui400
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_imgcodecs400
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_imgproc400
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_features2d400
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_calib3d400
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_videoio400
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_video400
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_objdetect400
LIBS += -LD:/opencv_build_4.0.0_64bit/bin/ -llibopencv_face400
TARGET=$(NAME).exe
OBJS = $(NAME).o

all:
	make $(NAME)


$(NAME): $(NAME).o
	$(CC) -o $(NAME) $(NAME).o $(LIBS)


$(NAME).o: $(NAME).cpp
	$(CC) -g -c -o $(NAME).o $(NAME).cpp $(INCLUDE)

clean:
	del *.o *.exe