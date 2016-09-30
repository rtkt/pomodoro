#-------------------------------------------------
#
# Project created by QtCreator 2016-09-24T03:43:17
#
#-------------------------------------------------

QT       += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pomodoro
TEMPLATE = app

SOURCES += main.cpp\
        win.cpp \
    timer.cpp \
    settings.cpp

HEADERS  += win.h \
    timer.h \
    settings.h

FORMS    += win.ui \
    settings.ui

RESOURCES += \
    media/icons.qrc
