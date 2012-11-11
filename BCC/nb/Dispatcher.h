#ifndef DISPATCHER_H
#define DISPATCHER_H

//NOTE временно
//NOTE всё временно
//NOTE мы все умрем

#include <QDir>

#include "Uuid.h"

#include "Provider.h"
#include "Processor.h"
#include "Nbcc.h"

#include "ModuleInfo.h"

using namespace Nb;

class Dispatcher
{
public:
    Dispatcher();
    Dispatcher(QString path);
    bool loadAllModules (QString path);
    bool clear ();
    bool getProvider (Uuid id, Provider &module, ModuleInfo *info);
    bool getProcessor (Uuid id, Processor &module, ModuleInfo *info);
    bool getNbcc (Uuid id, Nbcc &module, ModuleInfo *info);
    ModuleInfos enumerate ();
    size_t size ();

    bool loadModule (QString filename);

private:
    ModuleInfos _modules;
};

#endif // DISPATCHER_H
