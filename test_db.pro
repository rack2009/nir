#-------------------------------------------------
#
# Project created by QtCreator 2016-11-06T16:59:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = DMS

QT += sql

CONFIG += fly c++11
FLY += ui


INCLUDEPATH += /usr/include/fly


SOURCES += main.cpp\
        mainwindow.cpp \
    newfile.cpp \
    adduser.cpp

HEADERS  += mainwindow.h \
    newfile.h \
    adduser.h \
    properties.h

FORMS    += mainwindow.ui \
    newfile.ui \
    adduser.ui
