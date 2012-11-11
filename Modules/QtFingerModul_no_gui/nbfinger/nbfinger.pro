#-------------------------------------------------
#
# Project created by QtCreator 2011-10-02T13:43:26
#
#-------------------------------------------------

TARGET = nbfinger_base
TEMPLATE = lib

DEFINES += NBFINGER_LIBRARY

SOURCES += nbfinger.cpp \
    FingerProcessor.cpp \
    BimFp.cpp \
    fp/FPProcess.cpp \
    fp/FPMaths.cpp \
    fp/FPFft.cpp \
    fp/CFunctional.cpp \
    fp/CFPThinning.cpp \
    fp/CFPSegment.cpp \
    fp/CFPOrientation.cpp \
    fp/CFPMinutiae.cpp \
    fp/CFPEnhance.cpp \
    nbclasses/nbcTypes.cpp \
    fp/CFPTemplate.cpp

HEADERS += nbfinger.h\
        nbfinger_global.h \
    FingerProcessor.h \
    BimFp.h \
    fp/FPProcess.h \
    fp/FPMaths.h \
    fp/FPFft.h \
    fp/FPDefine.h \
    fp/CFunctional.h \
    fp/CFPThinning.h \
    fp/CFPSegment.h \
    fp/CFPOrientation.h \
    fp/CFPMinutiae.h \
    fp/CFPEnhance.h \
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
    ftrScanAPI.h \
    fp/CFPTemplate.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE0347631
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = nbfinger.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

RESOURCES += \
    resources.qrc
