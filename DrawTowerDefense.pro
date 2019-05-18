#-------------------------------------------------
#
# Project created by QtCreator 2014-09-03T12:40:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

TARGET = DrawTowerDefense
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    view.cpp \
    enemy/baseenemy.cpp \
    map/line.cpp \
    tower/basetower.cpp \
    enemy/enemyhelper.cpp \
    game.cpp \
    wavemanager.cpp \
    gameui.cpp \
    tower/bullet/basebullet.cpp \
    tower/bullet/guidancebullet.cpp \
    tower/bullet/basicbullet.cpp \
    enemy/blueenemy.cpp \
    enemy/enemyanimation.cpp \
    vision/glyphdetector.cpp \
    vision/capturethread.cpp \
    vision/imagebuffer.cpp \
    vision/processingthread.cpp \
    vision/visionmanager.cpp \
    tower/bullet/laserbullet.cpp \
    tower/lasertower.cpp \
    vision/visionview.cpp \
    vision/task/abstractprocesstask.cpp \
    vision/task/glyphprocesstask.cpp \
    vision/task/boundingrecttask.cpp \
    vision/task/lineprocesstask.cpp \
    gameline.cpp \
    vision/task/operators.cpp \
    vision/task/markerprocesstask.cpp

HEADERS  += mainwindow.h \
    view.h \
    enemy/baseenemy.h \
    map/line.h \
    tower/basetower.h \
    enemy/enemyhelper.h \
    game.h \
    wavemanager.h \
    gameui.h \
    tower/bullet/basebullet.h \
    tower/bullet/guidancebullet.h \
    tower/bullet/basicbullet.h \
    gameUIenum.h \
    enemy/blueenemy.h \
    enemy/enemyanimation.h \
    vision/glyphdetector.h \
    vision/capturethread.h \
    vision/imagebuffer.h \
    vision/Buffer.h \
    vision/processingthread.h \
    vision/visionmanager.h \
    tower/bullet/laserbullet.h \
    tower/lasertower.h \
    vision/visionview.h \
    vision/task/abstractprocesstask.h \
    vision/task/glyphprocesstask.h \
    vision/task/boundingrecttask.h \
    vision/task/lineprocesstask.h \
    gameline.h \
    vision/task/operators.h \
    vision/task/markerprocesstask.h

FORMS    += mainwindow.ui \
    vision/visionview.ui


unix {
    LIBS += -lopencv_core  -lopencv_highgui -lopencv_imgproc -lopencv_videoio -laruco -lv4l2
}
win32 {
INCLUDEPATH += "C:\opencv\build\include"
INCLUDEPATH += "C:\aruco-1.2.5\build\include"
LIBS += "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_calib3d249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_contrib249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_core249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_features2d249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_flann249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_gpu249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_highgui249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_imgproc249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_legacy249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_ml249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_objdetect249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_video249.dll" \
 "C:\opencv\build\x86_mingw\CMakeFiles\bin\libopencv_video249.dll" \
 "C:\aruco-1.2.5\build\bin\libaruco125.dll" \

}

yocto {
message("Yocto build")
DEFINES += YOCTO
SOURCES += vision/ov_video_capture.cpp \
	   vision/ipu_csc.c
HEADERS  += vision/ov_video_capture.h \
	    vision/ipu_csc.h 
}


win32:DEFINES  += WIN32
unix:DEFINES     += UNIX

target.path = /home/root/DrawTowerDefense
INSTALLS += target

OTHER_FILES += \
    vision/shaders/harris.fsh
