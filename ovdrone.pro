TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11
LIBS += -lboost_system `pkg-config --libs opencv protobuf`

SOURCES += \
    CameraStreamer.cpp \
    ComClient.cpp \
    main.cpp \
    ovdrone.pb.cpp \
    MotorController.cpp \
    Utils.cpp \
    BlackADC.cpp \
    BlackCore.cpp \
    BlackGPIO.cpp \
    BlackPWM.cpp

#include(deployment.pri)
#qtcAddDeployment()

HEADERS += \
    CameraStreamer.h \
    ComClient.h \
    ovdrone.pb.h \
    MotorController.h \
    Utils.h \
    BlackADC.h \
    BlackCore.h \
    BlackDef.h \
    BlackErr.h \
    BlackGPIO.h \
    BlackLib.h \
    BlackPWM.h

