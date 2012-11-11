#-------------------------------------------------
#
# Project created by QtCreator 2012-10-25T11:17:49
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = DllTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../nbcc/bc/VbpMath.cpp \
    ../nbcc/bc/RowMath.cpp \
    ../nbcc/bc/NeuralMath.cpp \
    ../nbcc/bc/md5.cpp \
    ../nbcc/bc/BcSlot.cpp \
    ../nbcc/bc/BcScheme.cpp \
    ../nbcc/bc/BcParam.cpp \
    ../nbcc/bc/BcNeuralConverter.cpp \
    ../nbcc/bc/BcNbcc.cpp \
    ../nbcc/bc/BcConnector.cpp \
    ../nbcc/bc/BcCondition.cpp \
    ../nbcc/bc/BcBitStream.cpp \
    ../nbcc/nb/Vbp.cpp \
    ../nbcc/nb/Uuid.cpp \
    ../nbcc/nb/SlotId.cpp \
    ../nbcc/nb/Slot.cpp \
    ../nbcc/nb/Scheme.cpp \
    ../nbcc/nb/Meta.cpp \
    ../nbcc/nb/Matrix.cpp \
    ../nbcc/nb/Enums.cpp \
    ../nbcc/nb/Data.cpp \
    ../nbcc/nb/Container.cpp \
    ../nbcc/nb/Bim.cpp

HEADERS += \
    ../nbcc/bc/VbpMath.h \
    ../nbcc/bc/RowMath.h \
    ../nbcc/bc/rndmt.h \
    ../nbcc/bc/NeuralMath.h \
    ../nbcc/bc/md5.h \
    ../nbcc/bc/BcSlot.h \
    ../nbcc/bc/BcScheme.h \
    ../nbcc/bc/BcParam.h \
    ../nbcc/bc/BcNeuralConverter.h \
    ../nbcc/bc/BcNbcc.h \
    ../nbcc/bc/BcConnector.h \
    ../nbcc/bc/BcCondition.h \
    ../nbcc/bc/BcBitStream.h \
    ../nbcc/nb/Vbp.h \
    ../nbcc/nb/Uuid.h \
    ../nbcc/nb/SlotId.h \
    ../nbcc/nb/Slot.h \
    ../nbcc/nb/Scheme.h \
    ../nbcc/nb/nbTypes.h \
    ../nbcc/nb/nbResult.h \
    ../nbcc/nb/nbIUnknown.h \
    ../nbcc/nb/nbINbcc.h \
    ../nbcc/nb/nbIEventHandler.h \
    ../nbcc/nb/nbIConfigurate.h \
    ../nbcc/nb/nbIComponentDispatcher.h \
    ../nbcc/nb/nbIBimProvider.h \
    ../nbcc/nb/nbIBimProcessor.h \
    ../nbcc/nb/nbc.h \
    ../nbcc/nb/nbBase.h \
    ../nbcc/nb/nb.h \
    ../nbcc/nb/Meta.h \
    ../nbcc/nb/Matrix.h \
    ../nbcc/nb/Enums.h \
    ../nbcc/nb/Data.h \
    ../nbcc/nb/Container.h \
    ../nbcc/nb/Bim.h
