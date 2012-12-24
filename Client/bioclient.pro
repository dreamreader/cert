#-------------------------------------------------
#
# Project created by QtCreator 2012-11-06T17:11:55
#
#-------------------------------------------------

TARGET = untitled
TEMPLATE = lib

QT += network
QMAKE_CXXFLAGS += -fpermissive -Wno-unused-parameter -Wno-write-strings

DEFINES += UNTITLED_LIBRARY

SOURCES += \
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
    ../nb/Bim.cpp \
    interface/SchemeVisualiser.cpp \
    interface/ExtractWizard.cpp \
    interface/SelectDialog.cpp \
    interface/TrainWizard.cpp \
    interface/TestDialog.cpp \
    ../BCC/protocol/testquery.cpp \
    ../BCC/protocol/statsquery.cpp \
    ../BCC/protocol/startsignaturequery.cpp \
    ../BCC/protocol/startregisterquery.cpp \
    ../BCC/protocol/signaturequery.cpp \
    ../BCC/protocol/schemequery.cpp \
    ../BCC/protocol/resultquery.cpp \
    ../BCC/protocol/query.cpp \
    ../BCC/protocol/passwordquery.cpp \
    ../BCC/protocol/log.cpp \
    ../BCC/protocol/getcontainerquery.cpp \
    ../BCC/protocol/enumeratecontainersquery.cpp \
    ../BCC/protocol/documentquery.cpp \
    ../BCC/protocol/containerquery.cpp \
    ../BCC/protocol/containernamesquery.cpp \
    ../BCC/protocol/bimparametersquery.cpp \
    ../BCC/protocol/authquery.cpp \
    dll.cpp \
    client.cpp \
    ../nb/InputScheme.cpp \
    ../nb/Dispatcher.cpp \
    ../nb/Provider.cpp \
    ../nb/Processor.cpp \
    ../nb/Nbcc.cpp \
    ../nb/Nbc.cpp \
    ../nb/BimBase.cpp \
    ../nb/Statistics.cpp \
    ../nb/SchemeConstructor.cpp

HEADERS +=\
    ../nb/Statistics.h \
    ../nb/SchemeConstructor.h \
    ../nb/Provider.h \
    ../nb/ProgramUser.h \
    ../nb/Processor.h \
    ../nb/Nbcc.h \
    ../nb/Nbc.h \
    ../nb/ModuleInfo.h \
    ../nb/InputScheme.h \
    ../nb/Dispatcher.h \
    ../nb/BimBase.h \
    ../nb/Vbp.h \
    ../nb/Uuid.h \
    ../nb/SlotId.h \
    ../nb/Slot.h \
    ../nb/Scheme.h \
    ../nb/Meta.h \
    ../nb/Matrix.h \
    ../nb/Enums.h \
    ../nb/Data.h \
    ../nb/Container.h \
    ../nb/Bim.h \
    interface/SchemeVisualiser.h \
    interface/ExtractWizard.h \
    interface/SelectDialog.h \
    interface/TrainWizard.h \
    interface/TestDialog.h \
    interface/gui.h \
    ../BCC/protocol/testquery.h \
    ../BCC/protocol/statsquery.h \
    ../BCC/protocol/startsignaturequery.h \
    ../BCC/protocol/startregisterquery.h \
    ../BCC/protocol/signaturequery.h \
    ../BCC/protocol/schemequery.h \
    ../BCC/protocol/resultquery.h \
    ../BCC/protocol/Query.h \
    ../BCC/protocol/queries.h \
    ../BCC/protocol/passwordquery.h \
    ../BCC/protocol/nbqueryheader.h \
    ../BCC/protocol/log.h \
    ../BCC/protocol/getcontainerquery.h \
    ../BCC/protocol/enumeratecontainersquery.h \
    ../BCC/protocol/documentquery.h \
    ../BCC/protocol/containerquery.h \
    ../BCC/protocol/containernamesquery.h \
    ../BCC/protocol/bimparametersquery.h \
    ../BCC/protocol/authquery.h \
    dll.h \
    client.h


symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE52C0753
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = untitled.dll
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

FORMS += \
    designer/SelectDialog.ui \
    designer/ExtractWizard.ui \
    designer/TestDialog.ui \
    designer/SchemeDialog.ui \
    designer/TrainWizard.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    todo.txt
