#-------------------------------------------------
#
# Project created by QtCreator 2012-07-17T15:19:34
#
#-------------------------------------------------

QT       += core
QT       += network
QT       -= gui

TARGET = BCCTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp\
    client.cpp \
    ../BCC/nb/Vbp.cpp \
    ../BCC/nb/Uuid.cpp \
    ../BCC/nb/SlotId.cpp \
    ../BCC/nb/Slot.cpp \
    ../BCC/nb/Scheme.cpp \
    ../BCC/nb/Nbcc.cpp \
    ../BCC/nb/Nbc.cpp \
    ../BCC/nb/Meta.cpp \
    ../BCC/nb/Matrix.cpp \
    ../BCC/nb/Enums.cpp \
    ../BCC/nb/Dispatcher.cpp \
    ../BCC/nb/Data.cpp \
    ../BCC/nb/Container.cpp \
    ../BCC/nb/Bim.cpp \
    ../BCC/nb/Statistics.cpp \
    ../BCC/nb/Processor.cpp \
    ../BCC/nb/BimBase.cpp \
    ../BCC/nb/Provider.cpp \
    SchemeConstructor.cpp \
    onetimepassword.cpp \
    ../BCC/protocol/testquery.cpp \
    ../BCC/protocol/statsquery.cpp \
    ../BCC/protocol/startsignaturequery.cpp \
    ../BCC/protocol/startregisterquery.cpp \
    ../BCC/protocol/signaturequery.cpp \
    ../BCC/protocol/resultquery.cpp \
    ../BCC/protocol/query.cpp \
    ../BCC/protocol/log.cpp \
    ../BCC/protocol/getcontainerquery.cpp \
    ../BCC/protocol/containerquery.cpp \
    ../BCC/protocol/bimparametersquery.cpp \
    ../BCC/protocol/authquery.cpp \
    ../BCC/protocol/documentquery.cpp

HEADERS += \
    client.h \
    ../BCC/nb/Vbp.h \
    ../BCC/nb/Uuid.h \
    ../BCC/nb/SlotId.h \
    ../BCC/nb/Slot.h \
    ../BCC/nb/Scheme.h \
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
    ../BCC/nb/common.h \
    ../BCC/nb/Bim.h \
    ../BCC/nb/Statistics.h \
    ../BCC/nb/Processor.h \
    ../BCC/nb/ComponentInfos.h \
    ../BCC/nb/BimBase.h \
    ../BCC/nb/Provider.h \
    ../BCC/nb/ModuleInfo.h \
    SchemeConstructor.h \
    onetimepassword.h \
    ../BCC/protocol/testquery.h \
    ../BCC/protocol/statsquery.h \
    ../BCC/protocol/startsignaturequery.h \
    ../BCC/protocol/startregisterquery.h \
    ../BCC/protocol/signaturequery.h \
    ../BCC/protocol/resultquery.h \
    ../BCC/protocol/query.h \
    ../BCC/protocol/queries.h \
    ../BCC/protocol/nbqueryheader.h \
    ../BCC/protocol/log.h \
    ../BCC/protocol/getcontainerquery.h \
    ../BCC/protocol/containerquery.h \
    ../BCC/protocol/bimparametersquery.h \
    ../BCC/protocol/authquery.h \
    ../BCC/protocol/documentquery.h
