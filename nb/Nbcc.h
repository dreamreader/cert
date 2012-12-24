//Nbcc
//Класс-обертка модуля нейросетевого преобразователя

#ifndef NBCC_H_INCLUDED
#define NBCC_H_INCLUDED

#include <QLibrary>

#include "../nb/nb.h"
#include "../nb/Matrix.h"

#include "ModuleInfo.h"
#include "Statistics.h"
#include "Nbc.h"

using namespace Nb;

class Nbcc {
public:
  Nbcc ();
  ~Nbcc();

  bool create(nbINbcc *iface, Uuid id);
  bool load (Uuid id, QString path, ModuleInfo info);
  bool reset();

  bool isCreated();
  Uuid getId();

  bool isTrained();
  bool isDefended();
  bool isTested();

  //Извлечение выходного кода
  bool extract(Nbc &container, Matrix &code, QList<Matrix *> &params);
  //Вычисление стабильности, уникальности, качества для непрерывных параметров
  bool calcSoq(const Matrix &owns, const Matrix &foreigns, Statistics &stats);
  //Обучение
  bool learn(Nbc &container, QList<Matrix *> &owns, QList<Matrix *> &foreigns, Matrix &code, Statistics &stats);

private:
  nbINbcc *_iface;
  bool _created, _trained, _defended, _tested;
  Uuid _id;
  ModuleInfo _info;
};

#endif
