#-------------------------------------------------
#
# Project created by QtCreator 2020-01-19T18:49:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TopsensExplorer
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    panel.cpp \
    userpainter.cpp

HEADERS  += mainwindow.h \
    panel.h \
    userpainter.h

FORMS    += mainwindow.ui \
    panel.ui

INCLUDEPATH += ../Topsens/include

LIBS += -L../Topsens/libs -lTopsensSensor

QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\' -no-pie"
