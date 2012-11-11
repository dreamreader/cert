#include "Dispatcher.h"
#include "../protocol/log.h"

#define FILTER_DLL "*.dll"

Dispatcher::Dispatcher()
{
}

Dispatcher::Dispatcher(QString path)
{
  loadAllModules (path);
}

bool Dispatcher::loadAllModules (QString path)
{
  //TODO узнать, как работает NbEnumerateComponents и убрать этот хтонический пиздец
  QDir dir (path);
  QStringList filters;
  filters << "*.dll";
  dir.setNameFilters(filters);
  QStringList files = dir.entryList (QDir::Files);
  for (int i = 0; i != files.size(); i++) {
    loadModule (path + files[i]);
  }

  return true;
}

bool Dispatcher::clear ()
{
  _modules.clear ();
  return true;
}

bool Dispatcher::getProvider (Uuid id, Provider &module, ModuleInfo *info)
{
  for (int i = 0; i != _modules.size (); i++) {
    ModuleInfo t = _modules[i];
    if ( (id == _modules[i].module_id) && ( (Uuid)nbUUID_IBIM_PROVIDER == _modules[i].module_type ) ) {
      module.load (id, _modules[i].filename, _modules[i]);
      if (info) *info = _modules[i];
      return true;
    }
  }
  return false;
}

bool Dispatcher::getProcessor (Uuid id, Processor &module, ModuleInfo *info)
{
  for (int i = 0; i != _modules.size (); i++) {
    ModuleInfo t = _modules[i];
    if ( (id == _modules[i].module_id) && ( (Uuid)nbUUID_IBIM_PROCESSOR == _modules[i].module_type ) ) {
      module.load (id, _modules[i].filename, _modules[i]);
      if (info) *info = _modules[i];
      return true;
    }
  }
  return false;
}

bool Dispatcher::getNbcc (Uuid id, Nbcc &module, ModuleInfo *info)
{
  for (int i = 0; i != _modules.size (); i++) {
    ModuleInfo t = _modules[i];
    if ( (id == _modules[i].module_id) && ( (Uuid)nbUUID_INBCC == _modules[i].module_type ) ) {
      module.load (id, _modules[i].filename);
      if (info) *info = _modules[i];
      return true;
    }
  }
  return false;
}

ModuleInfos Dispatcher::enumerate ()
{
  return _modules;
}

bool Dispatcher::loadModule (QString filename)
{
    Provider test_provider;
    Processor test_processor;
    Nbcc test_nbcc;
    bool result = false;
    try {
      ModuleInfo info;
      info.filename = filename;
      info.module_id = UUID_HANDWR_PROVIDER;
      info.name = QString::fromUtf8 (HANDWR_NAME);
      info.module_type = nbUUID_IBIM_PROVIDER;
      info.bim_type = BIM_TYPE_HANDWR;
      info.vendor = QString::fromUtf8 (HANDWR_VENDOR);
      test_provider.load (UUID_HANDWR_PROVIDER, filename, info);
      _modules.push_back (info);
      result = true;
    } catch (QString str) {}

    try {
      ModuleInfo info;
      info.filename = filename;
      info.module_id = UUID_FINGER_PROVIDER;
      info.name = QString::fromUtf8 (FINGER_NAME);
      info.module_type = nbUUID_IBIM_PROVIDER;
      info.bim_type = BIM_TYPE_FINGER;
      info.vendor = QString::fromUtf8 (FINGER_VENDOR);
      test_provider.load (UUID_FINGER_PROVIDER, filename, info);
      _modules.push_back (info);
      result = true;
    } catch (QString str) {}

    try {
      ModuleInfo info;
      info.filename = filename;
      info.module_id = UUID_HANDWR_PROCESSOR;
      info.name = QString::fromUtf8 (HANDWR_NAME);
      info.module_type = nbUUID_IBIM_PROCESSOR;
      info.bim_type = BIM_TYPE_HANDWR;
      info.vendor = QString::fromUtf8 (HANDWR_VENDOR);
      test_processor.load (UUID_HANDWR_PROCESSOR, filename, info);
      _modules.push_back (info);
      result = true;
    } catch (QString str) {}

    try {
      ModuleInfo info;
      info.filename = filename;
      info.module_id = UUID_FINGER_PROCESSOR;
      info.name = QString::fromUtf8 (HANDWR_NAME);
      info.module_type = nbUUID_IBIM_PROCESSOR;
      info.bim_type = BIM_TYPE_FINGER;
      info.vendor = QString::fromUtf8 (HANDWR_VENDOR);
      test_processor.load (UUID_FINGER_PROCESSOR, filename, info);
      _modules.push_back (info);
      result = true;
    } catch (QString str) {}

    try {
      ModuleInfo info;
      info.filename = filename;
      info.module_id = UUID_NBCC;
      info.name = QString::fromUtf8 (NBCC_NAME);
      info.module_type = nbUUID_INBCC;
      info.bim_type = nbUUID_NIL;
      info.vendor = QString::fromUtf8 (NBCC_VENDOR);
      test_nbcc.load (UUID_NBCC, filename);
      _modules.push_back (info);
      result = true;
    } catch (QString str) {}

    return result;
}
