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
    map/line.cpp \
    tower/basetower.cpp \
    tower/bullet.cpp \
    enemy/baseenemy.cpp \
    enemy/enemyhelper.cpp \
    game.cpp \
    wavemanager.cpp \
    gameui.cpp

HEADERS  += mainwindow.h \
    view.h \
    enemy/baseenemy.h \
    map/line.h \
    tower/basetower.h \
    tower/bullet.h \
    enemy/enemyhelper.h \
    game.h \
    wavemanager.h \
    gameui.h

FORMS    += mainwindow.ui
