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
    fvmoctree.cpp \
    octcell.cpp \
    octcellface.cpp

HEADERS  += mainwin.h \
    viswidget.h \
    definitions.h \
    fvmoctree.h \
    octcell.h \
    octcellface.h \
    base_int_vec3.h \
    base_float_vec3.h

FORMS    += mainwin.ui

LIBS += #-lglut
