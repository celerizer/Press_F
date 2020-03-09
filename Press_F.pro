QT       += core gui multimedia gamepad

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/emu.c \
    src/file.c \
    src/font.c \
    src/frontend/qt/framebuffer_widget.cpp \
    src/frontend/qt/main.cpp \
    src/frontend/qt/mainwindow.cpp \
    src/frontend/qt/registerswindow.cpp \
    src/hle.c \
    src/input.c \
    src/screen.c \
    src/sound.c \
    src/wave.c

HEADERS += \
    src/config.h \
    src/emu.h \
    src/file.h \
    src/font.h \
    src/frontend/qt/framebuffer_widget.h \
    src/frontend/qt/main.h \
    src/frontend/qt/mainwindow.h \
    src/frontend/qt/registerswindow.h \
    src/hle.h \
    src/input.h \
    src/screen.h \
    src/sound.h \
    src/types.h \
    src/wave.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src/frontend/qt/resources.qrc
