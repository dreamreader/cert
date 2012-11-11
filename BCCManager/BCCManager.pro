#-------------------------------------------------
#
# Project created by QtCreator 2012-07-16T17:35:40
#
#-------------------------------------------------

QT       += core
QT       += sql
QTPLUGIN += qsqlite
QT       -= gui


LIBS     += "advapi32.lib"

TARGET = BCCIntaller
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../BCC/serverdatabase.cpp \
    ../BCC/protocol/log.cpp \
    ../BCC/nb/Vbp.cpp \
    ../BCC/nb/Uuid.cpp \
    ../BCC/nb/Statistics.cpp \
    ../BCC/nb/SlotId.cpp \
    ../BCC/nb/Slot.cpp \
    ../BCC/nb/Scheme.cpp \
    ../BCC/nb/Processor.cpp \
    ../BCC/nb/Nbcc.cpp \
    ../BCC/nb/Nbc.cpp \
    ../BCC/nb/Meta.cpp \
    ../BCC/nb/Matrix.cpp \
    ../BCC/nb/Enums.cpp \
    ../BCC/nb/Dispatcher.cpp \
    ../BCC/nb/Data.cpp \
    ../BCC/nb/Container.cpp \
    ../BCC/nb/BimBase.cpp \
    ../BCC/nb/Bim.cpp \
    ../BCC/nb/Provider.cpp

HEADERS += \
    ../BCC/serverdatabase.h \
    ../BCC/protocol/log.h \
    ../BCC/nb/Vbp.h \
    ../BCC/nb/Uuid.h \
    ../BCC/nb/Statistics.h \
    ../BCC/nb/SlotId.h \
    ../BCC/nb/Slot.h \
    ../BCC/nb/Scheme.h \
    ../BCC/nb/Processor.h \
    ../BCC/nb/nbTypes.h \
    ../BCC/nb/nbResult.h \
    ../BCC/nb/nbIUnknown.h \
    ../BCC/nb/nbINbcc.h \
    ../BCC/nb/nbIEventHandler.h \
    ../BCC/nb/nbIConfigurate.h \
    ../BCC/nb/nbIComponentDispatcher.h \
    ../BCC/nb/nbIBimProvider.h \
    ../BCC/nb/nbIBimProcessor.h \
    ../BCC/nb/nbclasses.h \
    ../BCC/nb/Nbcc.h \
    ../BCC/nb/Nbc.h \
    ../BCC/nb/nbBase.h \
    ../BCC/nb/nb.h \
    ../BCC/nb/Meta.h \
    ../BCC/nb/Matrix.h \
    ../BCC/nb/Enums.h \
    ../BCC/nb/Dispatcher.h \
    ../BCC/nb/Data.h \
    ../BCC/nb/Container.h \
    ../BCC/nb/ComponentInfos.h \
    ../BCC/nb/common.h \
    ../BCC/nb/BimBase.h \
    ../BCC/nb/Bim.h \
    ../BCC/nb/Provider.h \
    ../BCC/nb/ModuleInfo.h
