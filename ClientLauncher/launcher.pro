#-------------------------------------------------
#
# Project created by QtCreator 2012-11-06T18:51:31
#
#-------------------------------------------------

QT       += core
QT       += gui
QMAKE_CXXFLAGS += -fpermissive -Wno-unused-parameter -Wno-write-strings

TARGET = launcher

TEMPLATE = app

SOURCES += main.cpp \
    qtnb/Vbp.cpp \
    qtnb/Uuid.cpp \
    qtnb/SlotId.cpp \
    qtnb/Slot.cpp \
    qtnb/Scheme.cpp \
    qtnb/Meta.cpp \
    qtnb/Matrix.cpp \
    qtnb/Enums.cpp \
    qtnb/Data.cpp \
    qtnb/Container.cpp \
    qtnb/Bim.cpp

HEADERS += \
    qtnb/Vbp.h \
    qtnb/Uuid.h \
    qtnb/SlotId.h \
    qtnb/Slot.h \
    qtnb/Scheme.h \
    qtnb/Meta.h \
    qtnb/Matrix.h \
    qtnb/Enums.h \
    qtnb/Data.h \
    qtnb/Container.h \
    qtnb/Bim.h
