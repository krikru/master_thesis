#-------------------------------------------------
#
# Project created by QtCreator 2012-03-27T15:46:19
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -Wall

QT       += core gui opengl

TARGET = rtocean
TEMPLATE = app

SOURCES += main.cpp\
        mainwin.cpp \
    viswidget.cpp \
    octcell.cpp \
    fvoctree.cpp \
    message_handler.cpp \
    octneighbor.cpp \
    dllnode.cpp \
    base_dllnode.cpp \
    nlistset.cpp \
    watersystem.cpp \
    mustinit.cpp \
    physics.cpp

HEADERS  += mainwin.h \
    viswidget.h \
    definitions.h \
    octcell.h \
    base_int_vec3.h \
    base_float_vec3.h \
    fvoctree.h \
    callback.h \
    message_handler.h \
    octneighbor.h \
    dllist.h \
    dllnode.h \
    base_float_vec2.h \
    base_dllnode.h \
    math_functions.h \
    compile_time.h \
    nlset.h \
    watersystem.h \
    base_int_vec2.h \
    naninit.h \
    mustinit.h \
    physics.h

FORMS    += mainwin.ui
