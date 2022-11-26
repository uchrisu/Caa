QT       += core gui

VERSION = 0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += qwt
# CONFIG += optimize_full
QMAKE_CXXFLAGS_RELEASE += -O3 -ffast-math

# QMAKE_CXXFLAGS_RELEASE += -march=native

# QMAKE_CXXFLAGS_RELEASE += -fopt-info-vec-optimized

# QMAKE_CXXFLAGS += -fopenmp
# LIBS += -fopenmp

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -L"/usr/lib" -ljack
LIBS += -lfftw3 -lsndfile

SOURCES += \
    audiosystemanalyzer.cpp \
    channelconfigwidget.cpp \
    config.cpp \
    jaudiobuffer.cpp \
    main.cpp \
    mainwindow.cpp \
    plotfreqresp.cpp \
    plotir.cpp \
    plotsignal.cpp \
    windowfunc.cpp

HEADERS += \
    audiosystemanalyzer.h \
    channelconfigwidget.h \
    config.h \
    jaudiobuffer.h \
    mainwindow.h \
    plotfreqresp.h \
    plotir.h \
    plotsignal.h \
    windowfunc.h

FORMS += \
    channelconfigwidget.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usw/local/bin
!isEmpty(target.path): INSTALLS += target
