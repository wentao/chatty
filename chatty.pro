QT += core network

QT -= gui

TARGET = chatty

CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

SOURCES += main.cpp \
    room.cpp \
    client.cpp \
    tcpserver.cpp \
    hall.cpp \
    protocol.cpp

HEADERS += \
    room.h \
    client.h \
    tcpserver.h \
    hall.h \
    protocol.h
