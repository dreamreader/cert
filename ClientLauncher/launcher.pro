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
    ../nb/Vbp.cpp \
    ../nb/Uuid.cpp \
    ../nb/SlotId.cpp \
    ../nb/Slot.cpp \
    ../nb/Scheme.cpp \
    ../nb/Meta.cpp \
    ../nb/Matrix.cpp \
    ../nb/Enums.cpp \
    ../nb/Data.cpp \
    ../nb/Container.cpp \
    ../nb/Bim.cpp

HEADERS += \
    ../nb/Vbp.h \
    ../nb/Uuid.h \
    ../nb/SlotId.h \
    ../nb/Slot.h \
    ../nb/Scheme.h \
    ../nb/Meta.h \
    ../tnb/Matrix.h \
    ../nb/Enums.h \
    ../nb/Data.h \
    ../nb/Container.h \
    ../nb/Bim.h
