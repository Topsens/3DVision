#-------------------------------------------------
#
# Project created by QtCreator 2020-09-09T12:05:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TopsensRealSense
TEMPLATE = app


SOURCES += main.cpp\
           mainwindow.cpp \
           panel.cpp \
           depthview.cpp \
           userpainter.cpp

HEADERS += mainwindow.h \
           panel.h \
           depthview.h \
           userpainter.h

FORMS   += mainwindow.ui \
    panel.ui \
    depthview.ui

INCLUDEPATH += ../Topsens/include

LIBS += -L../Topsens/libs -lTopsensPeople -lrealsense2

QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\' -no-pie"
