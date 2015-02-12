#-------------------------------------------------
#
# Project created by QtCreator 2014-04-06T18:47:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = openBKZ
TEMPLATE = app
#CONFIG-=app_bundle
DESTDIR = release
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp

SOURCES += src/main.cpp\
    src/openbkz.cpp \
    src/library.cpp \
    src/statistics.cpp \
    src/statsviewer/statsviewer.cpp 

HEADERS  += src/openbkz.h \
    src/library.h \
    src/statistics.h \
    src/statsviewer/statsviewer.h 

FORMS    += src/openbkz.ui
FORMS    += src/statsviewer/statsviewer.ui
