//Nbcc
//Класс-обертка модуля нейросетевого преобразователя

#ifndef Nbcc_H_INCLUDED
#define Nbcc_H_INCLUDED

#include "common.h"
#include <QLibrary>
#include "Nbc.h"
#include "Matrix.h"
#include "Statistics.h"

using namespace Nb;

class Nbcc {
public:
  Nbcc ();
  ~Nbcc();

  bool create(nbINbcc *iface, Uuid id);
  bool load (Uuid id, QString path);
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
};

#endif
