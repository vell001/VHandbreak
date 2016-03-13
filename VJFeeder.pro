#-------------------------------------------------
#
# Project created by QtCreator 2016-03-12T19:36:25
#
#-------------------------------------------------

QT       += core gui
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VJFeeder
TEMPLATE = app

INCLUDEPATH += $$PWD/vJoySDK/inc

contains(QMAKE_HOST.arch, x86_64) {
    LIBS += -L$$PWD/vJoySDK/lib/amd64
} else {
    LIBS += -L$$PWD/vJoySDK/lib
}

LIBS += -lvJoyInterface

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
