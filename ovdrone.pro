TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    tankV1.cpp \
    CameraStreamer.cpp \
    ComClient.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    CameraStreamer.h \
    ComClient.h

