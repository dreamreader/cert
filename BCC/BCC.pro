#-------------------------------------------------
#
# Project created by QtCreator 2012-06-05T20:50:41
#
#-------------------------------------------------

QT        += core
QT        += network
QT        += sql

QTPLUGIN  += qsqlite

LIBS      += "advapi32.lib"

DEFINES   += _WIN32
DEFINES   += _MICROSOFT_SPECIFIC

TARGET     = BioCertificationCenter
CONFIG    += console
CONFIG    -= app_bundle

TEMPLATE   = app


SOURCES   += \
    main.cpp \
    servercontext.cpp \
    server.cpp \
    finishthreadevent.cpp \
    sessioncontext.cpp \
    serverdatabase.cpp \
    sessionthread.cpp \
    sessiontask.cpp \
    onetimepassword.cpp \
    nb/Vbp.cpp \
    nb/Uuid.cpp \
    nb/SlotId.cpp \
    nb/Slot.cpp \
    nb/Scheme.cpp \
    nb/Nbcc.cpp \
    nb/Nbc.cpp \
    nb/Meta.cpp \
    nb/Matrix.cpp \
    nb/Enums.cpp \
    nb/Dispatcher.cpp \
    nb/Data.cpp \
    nb/Container.cpp \
    nb/Bim.cpp \
    nb/Statistics.cpp \
    nb/Processor.cpp \
    nb/BimBase.cpp \
    nb/Provider.cpp \
    protocol/testquery.cpp \
    protocol/statsquery.cpp \
    protocol/startsignaturequery.cpp \
    protocol/startregisterquery.cpp \
    protocol/signaturequery.cpp \
    protocol/resultquery.cpp \
    protocol/query.cpp \
    protocol/log.cpp \
    protocol/getcontainerquery.cpp \
    protocol/documentquery.cpp \
    protocol/containerquery.cpp \
    protocol/containernamesquery.cpp \
    protocol/bimparametersquery.cpp \
    protocol/authquery.cpp


HEADERS   += \
    block.h \
    servercontext.h \
    server.h \
    finishthreadevent.h \
    sessioncontext.h \
    serverdatabase.h \
    sessionthread.h \
    sessiontask.h \
    onetimepassword.h \
    nb/Vbp.h \
    nb/Uuid.h \
    nb/SlotId.h \
    nb/Slot.h \
    nb/Scheme.h \
    nb/nbTypes.h \
    nb/nbResult.h \
    nb/nbIUnknown.h \
    nb/nbINbcc.h \
    nb/nbIEventHandler.h \
    nb/nbIConfigurate.h \
    nb/nbIComponentDispatcher.h \
    nb/nbIBimProvider.h \
    nb/nbIBimProcessor.h \
    nb/nbclasses.h \
    nb/Nbcc.h \
    nb/Nbc.h \
    nb/nbBase.h \
    nb/nb.h \
    nb/Meta.h \
    nb/Matrix.h \
    nb/Enums.h \
    nb/Dispatcher.h \
    nb/Data.h \
    nb/Container.h \
    nb/common.h \
    nb/Bim.h \
    nb/Statistics.h \
    nb/Processor.h \
    nb/ComponentInfos.h \
    nb/BimBase.h \
    nb/Provider.h \
    protocol/testquery.h \
    protocol/statsquery.h \
    protocol/startsignaturequery.h \
    protocol/startregisterquery.h \
    protocol/signaturequery.h \
    protocol/resultquery.h \
    protocol/Query.h \
    protocol/queries.h \
    protocol/nbqueryheader.h \
    protocol/log.h \
    protocol/getcontainerquery.h \
    protocol/enumeratecontainersquery.h \
    protocol/documentquery.h \
    protocol/containerquery.h \
    protocol/containernamesquery.h \
    protocol/bimparametersquery.h \
    protocol/authquery.h
