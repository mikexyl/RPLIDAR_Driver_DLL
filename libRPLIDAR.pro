#-------------------------------------------------
#
# Project created by QtCreator 2017-11-07T14:47:26
#
#-------------------------------------------------

QT       -= core gui

TARGET = libRPLIDAR
TEMPLATE = lib
CONFIG += dll
DEFINES += LIBRPLIDAR_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        librplidar.cpp \
    src/rplidar_driver.cpp \
    src/arch/win32/net_serial.cpp \
    src/arch/win32/timer.cpp \
    src/hal/thread.cpp

HEADERS += \
        librplidar_global.h \ 
    src/rplidar_driver_serial.h \
    src/sdkcommon.h \
    src/arch/win32/arch_win32.h \
    src/arch/win32/net_serial.h \
    src/arch/win32/timer.h \
    src/arch/win32/winthread.hpp \
    src/hal/abs_rxtx.h \
    src/hal/event.h \
    src/hal/locker.h \
    src/hal/thread.h \
    src/hal/util.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += C:/lxy/RPLIDAR/include
