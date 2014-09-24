#-------------------------------------------------
#
# Project created by QtCreator 2014-04-29T16:36:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RecordingLight
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        RtMidi.cpp \
    midiwrapper.cpp \
    midihandler.cpp \
    bluetoothserialport.cpp \
    rlcmdmaster.cpp \
    recordinglightstatemachine.cpp \
    colorlabel.cpp

HEADERS  += mainwindow.h \
        RtMidi.h \
    midiwrapper.h \
    midihandler.h \
    SerialProtocol.h \
    bluetoothserialport.h \
    rlcmdmaster.h \
    recordinglightstatemachine.h \
    colorlabel.h

FORMS    += mainwindow.ui

DEFINES += __MACOSX_CORE__
LIBS    += -framework CoreMIDI
LIBS    += -framework CoreAudio
LIBS    += -framework CoreFoundation
