#-------------------------------------------------
#
# Project created by QtCreator 2012-10-24T11:43:00
#
#-------------------------------------------------

QT       -= gui

TARGET = nbcc
TEMPLATE = lib

DEFINES += NBCC_LIBRARY

SOURCES += nbcc.cpp \
    nb/Vbp.cpp \
    nb/Uuid.cpp \
    nb/SlotId.cpp \
    nb/Slot.cpp \
    nb/Scheme.cpp \
    nb/Meta.cpp \
    nb/Matrix.cpp \
    nb/Enums.cpp \
    nb/Data.cpp \
    nb/Container.cpp \
    nb/Bim.cpp \
    bc/VbpMath.cpp \
    bc/RowMath.cpp \
    bc/NeuralMath.cpp \
    bc/md5.cpp \
    bc/BcSlot.cpp \
    bc/BcScheme.cpp \
    bc/BcParam.cpp \
    bc/BcNeuralConverter.cpp \
    bc/BcConnector.cpp \
    bc/BcCondition.cpp \
    bc/BcBitStream.cpp \
    bc/BcNbcc.cpp

HEADERS += nbcc.h\
        nbcc_global.h \
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
    nb/nbc.h \
    nb/nbBase.h \
    nb/nb.h \
    nb/Meta.h \
    nb/Matrix.h \
    nb/Enums.h \
    nb/Data.h \
    nb/Container.h \
    nb/Bim.h \
    bc/VbpMath.h \
    bc/RowMath.h \
    bc/rndmt.h \
    bc/NeuralMath.h \
    bc/md5.h \
    bc/BcSlot.h \
    bc/BcScheme.h \
    bc/BcParam.h \
    bc/BcNeuralConverter.h \
    bc/BcConnector.h \
    bc/BcCondition.h \
    bc/BcBitStream.h \
    bc/BcNbcc.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2C41BB4
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = nbcc.dll
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
