TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    CameraStreamer.cpp \
    ComClient.cpp \
    main.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    CameraStreamer.h \
    ComClient.h

