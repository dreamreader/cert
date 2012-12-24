//Processor
//Класс-обертка модуля биометрического процессора

#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED

#include <QLibrary>

#include "../nb/nb.h"
#include "../nb/Matrix.h"

#include "ModuleInfo.h"
#include "BimBase.h"

using namespace Nb;

class Processor {
public:
  Processor ();
  ~Processor();

  bool isCreated ();
  bool create(nbIBimProcessor *iface, Uuid id);
  bool reset();
  Uuid getId();

  bool process(Matrix &params, BimBase &images);
  bool getOutsBase(Matrix &base);

  bool getPattern (Matrix &params, BimBase &images);
  bool setPattern (Matrix &params);

  bool load (Uuid id, QString path, ModuleInfo info);

private:
  nbIBimProcessor *_iface;
  bool _created;
  Uuid _id;
  ModuleInfo _info;
};

#endif
