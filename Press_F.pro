QT += core gui multimedia gamepad

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CFLAGS += -std=c89

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
  src/libpressf/src/debug.c \
  src/libpressf/src/dma.c \
  src/libpressf/src/emu.c \
  src/libpressf/src/font.c \
  src/libpressf/src/hle.c \
  src/libpressf/src/input.c \
  src/libpressf/src/romc.c \
  src/libpressf/src/screen.c \
  src/libpressf/src/software.c \
  src/libpressf/src/wave.c \
  src/libpressf/src/hw/2102.c \
  src/libpressf/src/hw/2114.c \
  src/libpressf/src/hw/3850.c \
  src/libpressf/src/hw/3851.c \
  src/libpressf/src/hw/beeper.c \
  src/libpressf/src/hw/f8_device.c \
  src/libpressf/src/hw/fairbug_parallel.c \
  src/libpressf/src/hw/hand_controller.c \
  src/libpressf/src/hw/schach_led.c \
  src/libpressf/src/hw/selector_control.c \
  src/libpressf/src/hw/system.c \
  src/libpressf/src/hw/vram.c

SOURCES += \
  src/framebuffer_widget.cpp \
  src/main.cpp \
  src/mainwindow.cpp \
  src/registerswindow.cpp \
  src/settings.cpp

HEADERS += \
  src/framebuffer_widget.h \
  src/main.h \
  src/mainwindow.h \
  src/registerswindow.h \
  src/settings.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
  src/resources.qrc
