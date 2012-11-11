#-------------------------------------------------
#
# Project created by QtCreator 2011-10-02T13:43:26
#
#-------------------------------------------------

TARGET = nbhandwr_base
TEMPLATE = lib

DEFINES += NBSIGN_LIBRARY

SOURCES += \
    nbclasses/nbcTypes.cpp \
    nbsign.cpp \
    SignProcessor.cpp \
    hw/psnConversionSFN.cpp \
    hw/psnBimHwMath.cpp \
    nbcc/nbccMath.cpp \
    hw/BimHw.cpp \
    psnReadWrite.cpp \
    exp/expTypes.cpp

HEADERS +=\
    nb/nbTypes.h \
    nb/nbResult.h \
    nb/nbIUnknown.h \
    nb/nbINbcc.h \
    nb/nbIEventHandler.h \
    nb/nbIConfigurate.h \
    nb/nbIComponentDispatcher.h \
    nb/nbIBimProvider.h \
    nb/nbIBimProcessor.h \
    nb/nbBase.h \
    nb/nb.h \
    nbclasses/nbcTypes.h \
    nbsign.h \
    SignProcessor.h \
    hw/psnConversionSFN.h \
    hw/psnBimHwMath.h \
    nbsign_global.h \
    nbcc/nbccMath.h \
    hw/BimHw.h \
    psnReadWrite.h \
    exp/expTypes.h


RESOURCES += \
    resources.qrc

OTHER_FILES += \
    nbhandwr.def
