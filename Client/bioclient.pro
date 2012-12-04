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
    interface/SchemeVisualiser.cpp \
    interface/ExtractWizard.cpp \
    model/User.cpp \
    model/Statistics.cpp \
    model/SchemeConstructor.cpp \
    model/Provider.cpp \
    model/ProgramUser.cpp \
    model/Processor.cpp \
    model/Nbcc.cpp \
    model/Nbc.cpp \
    model/InputScheme.cpp \
    model/Dispatcher.cpp \
    model/BimBase.cpp \
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
    qtnb/Bim.cpp \
    dll.cpp \
    protocol/testquery.cpp \
    protocol/statsquery.cpp \
    protocol/startsignaturequery.cpp \
    protocol/startregisterquery.cpp \
    protocol/schemequery.cpp \
    protocol/resultquery.cpp \
    protocol/query.cpp \
    protocol/passwordquery.cpp \
    protocol/log.cpp \
    protocol/documentquery.cpp \
    protocol/containerquery.cpp \
    protocol/bimparametersquery.cpp \
    interface/SelectDialog.cpp \
    interface/TrainWizard.cpp \
    interface/TestDialog.cpp \
    protocol/enumeratecontainersquery.cpp \
    protocol/containernamesquery.cpp \
    model/client.cpp

HEADERS +=\
    interface/SchemeVisualiser.h \
    interface/ExtractWizard.h \
    model/User.h \
    model/Statistics.h \
    model/SchemeConstructor.h \
    model/Provider.h \
    model/ProgramUser.h \
    model/Processor.h \
    model/Nbcc.h \
    model/Nbc.h \
    model/ModuleInfo.h \
    model/InputScheme.h \
    model/Dispatcher.h \
    model/BimBase.h \
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
    qtnb/Bim.h \
    dll.h \
    protocol/testquery.h \
    protocol/statsquery.h \
    protocol/startsignaturequery.h \
    protocol/startregisterquery.h \
    protocol/schemequery.h \
    protocol/resultquery.h \
    protocol/Query.h \
    protocol/queries.h \
    protocol/passwordquery.h \
    protocol/nbqueryheader.h \
    protocol/log.h \
    protocol/documentquery.h \
    protocol/containerquery.h \
    protocol/bimparametersquery.h \
    interface/SelectDialog.h \
    interface/TrainWizard.h \
    interface/TestDialog.h \
    interface/gui.h \
    protocol/enumeratecontainersquery.h \
    protocol/containernamesquery.h \
    model/client.h

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
