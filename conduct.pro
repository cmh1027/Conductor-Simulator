#-------------------------------------------------
#
# Project created by QtCreator 2018-08-28T18:48:40
#
#-------------------------------------------------

QT       += core gui widgets
QT       += xml
TARGET = conduct
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    main.cpp \
    conduct/conduct.cpp \
    conduct/command/command.cpp \
    conduct/config/config.cpp \
    conduct/module/utility.cpp \
    conduct/module/synctimer.cpp \
    conduct/module/countdowntimer.cpp \
    conduct/xmlReader/xmlreader.cpp \
    conduct/xmlReader/musicplayer.cpp \
    conduct/tracker/module/meanshift.cpp \
    conduct/tracker/module/pointqueue.cpp \
    conduct/tracker/thread/detector.cpp \
    conduct/tracker/thread/thread.cpp \
    conduct/tracker/tracker.cpp \
    widget/mainwindow.cpp \
    widget/clickablelabel.cpp \
    widget/menu/menu.cpp \
    widget/menu/main/menu_main.cpp \
    widget/menu/conductor/menu_conductor.cpp \
    widget/menu/conductor/scoremaker.cpp \
    widget/menu/editor/timelineedit.cpp \
    widget/menu/editor/menu_editor.cpp \
    widget/menu/config/menu_config.cpp \
    widget/menu/conductor/menu_difficulty.cpp \
    conduct/module/random.cpp \
    conduct/tracker/module/eyedector.cpp

HEADERS += \
    conduct/conduct.h \
    conduct/command/command.h \
    conduct/config/config.h \
    conduct/module/countdowntimer.h \
    conduct/module/synctimer.h \
    conduct/module/utility.h \
    conduct/xmlReader/xmlreader.h \
    conduct/xmlReader/musicplayer.h \
    conduct/tracker/module/meanshift.h \
    conduct/tracker/module/pointqueue.h \
    conduct/tracker/thread/detector.h \
    conduct/tracker/thread/thread.h \
    conduct/tracker/tracker.h \
    widget/mainwindow.h \
    widget/clickablelabel.h \
    widget/menu/menu.h \
    widget/menu/main/menu_main.h \
    widget/menu/conductor/menu_conductor.h \
    widget/menu/conductor/scoremaker.h \
    widget/menu/editor/timelineedit.h \
    widget/menu/editor/menu_editor.h \
    widget/menu/config/menu_config.h \
    widget/menu/conductor/menu_difficulty.h \
    conduct/module/random.h \
    conduct/tracker/module/eyedetector.h \
    eyedetector/include/const.h

FORMS += \
    widget/menu/conductor/conductor.ui \
    widget/menu/main/main.ui \
    widget/menu/config/config.ui \
    widget/menu/editor/editor.ui \
    widget/menu/conductor/difficulty.ui

DISTFILES += \
    icon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SUBDIRS += \
    conduct.pro

INCLUDEPATH += C:/MinGW/include
INCLUDEPATH += D:/opencv_3.2.0_32bit/build/include
INCLUDEPATH += D:/FMOD/api/lowlevel/inc


LIBS += -LD:/opencv_build_3.2.0_32bit/bin/ -llibopencv_core320
LIBS += -LD:/opencv_build_3.2.0_32bit/bin/ -llibopencv_highgui320
LIBS += -LD:/opencv_build_3.2.0_32bit/bin/ -llibopencv_imgcodecs320
LIBS += -LD:/opencv_build_3.2.0_32bit/bin/ -llibopencv_imgproc320
LIBS += -LD:/opencv_build_3.2.0_32bit/bin/ -llibopencv_features2d320
LIBS += -LD:/opencv_build_3.2.0_32bit/bin/ -llibopencv_calib3d320
LIBS += -LD:/opencv_build_3.2.0_32bit/bin/ -llibopencv_videoio320
LIBS += -LD:/opencv_build_3.2.0_32bit/bin/ -llibopencv_video320
LIBS += -LD:/opencv_build_3.2.0_32bit/bin/ -llibopencv_objdetect320
LIBS += -LD:/FMOD/api/lowlevel/lib -lfmod

RESOURCES += \
    resources.qrc
