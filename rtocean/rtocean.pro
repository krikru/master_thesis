#-------------------------------------------------
#
# Project created by QtCreator 2012-03-27T15:46:19
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = rtocean
TEMPLATE = app


SOURCES += main.cpp\
        mainwin.cpp \
    viswidget.cpp \
    octcell.cpp \
    octcellface.cpp \
    fvoctree.cpp \
    message_handler.cpp \
    octface.cpp

HEADERS  += mainwin.h \
    viswidget.h \
    definitions.h \
    octcell.h \
    octcellface.h \
    base_int_vec3.h \
    base_float_vec3.h \
    fvoctree.h \
    callback.h \
    message_handler.h \
    octface.h

FORMS    += mainwin.ui

LIBS += -L"C:\Program1\cygwin\usr\local\lib"

LIBS += -lglut

INCLUDEPATH += "C:\Program1\cygwin\usr\local\include"
