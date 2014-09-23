#-------------------------------------------------
#
# Project created by QtCreator 2014-09-03T12:40:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets opengl

TARGET = MassacreTD
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
    tower/bullet/basicbullet.cpp

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
    tower/bullet/basicbullet.h

FORMS    += mainwindow.ui
