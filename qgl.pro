#-------------------------------------------------
#
# Project created by QtCreator 2011-08-30T22:02:19
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = qgl
TEMPLATE = app


SOURCES += main.cpp\
        glwidget.cpp\
	sceneHandler.cpp \
    shadowmodel.cpp \
    lightingmodel.cpp

HEADERS  += glwidget.h\
	sceneHandler.h \
    shadowmodel.h \
    lightingmodel.h

OTHER_FILES += \
    full.vert \
    minimal.vert \
    minimal.frag \
    ambient.vert \
    full.frag \
    ambient.frag

LIBS += -lGLEW -lassimp -lIL
