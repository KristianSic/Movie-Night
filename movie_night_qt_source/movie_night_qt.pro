#-------------------------------------------------
#
# Project created by Kristian Šic 2019-01-26T15:47:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = movie_night_qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    aspectratiopixmaplabel.cpp \
    settings.cpp \
    framepicker.cpp \
    player.cpp \
    setting.cpp \
    fps.cpp \
    myslider.cpp

HEADERS += \
        mainwindow.h \
    aspectratiopixmaplabel.h \
    settings.h \
    framepicker.h \
    player.h \
    asmOpenCV.h \
    setting.h \
    fps.h \
    myslider.h

FORMS += \
        mainwindow.ui \
    settings.ui \
    framepicker.ui \
    fps.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

INCLUDEPATH += $$PWD/party/include

LIBS += -L$$PWD/party

QMAKE_CXXFLAGS += -std=gnu++11

INCLUDEPATH += C:\opencv\build\install\include


LIBS += C:\opencv\build\bin\libopencv_core401.dll
LIBS += C:\opencv\build\bin\libopencv_highgui401.dll
LIBS += C:\opencv\build\bin\libopencv_imgcodecs401.dll
LIBS += C:\opencv\build\bin\libopencv_imgproc401.dll
LIBS += C:\opencv\build\bin\libopencv_features2d401.dll
LIBS += C:\opencv\build\bin\libopencv_calib3d401.dll
LIBS += C:\opencv\build\bin\libopencv_video401.dll
LIBS += C:\opencv\build\bin\libopencv_videoio401.dll
