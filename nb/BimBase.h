//BimBase
//Хранилище биометрчиеских образов
#ifndef NBBIMS_H_INCLUDED
#define NBBIMS_H_INCLUDED

#include <vector>

#include <QFile>

#include "../nb/nb.h"

#include "../qtnb/Bim.h"

using namespace std;
using namespace Nb;

#define SIG_BIMS 0x204d4942

typedef vector<nbBim *> nbBims;

//Набор образов
class BimBase {
public:
  BimBase();
  ~BimBase();

  nbBim *at(uint32_t index) const { if (index<size()) return _bims[index]; else return NULL; }
  float  atQual(uint32_t index) const { return _quals[index]; }
  float& atQual(uint32_t index) { return _quals[index]; }

  nbBim** data();
  vector<float>& quals();

  //Получить размер
  uint32_t size() const;
  //Получить тип образов
  Uuid getBimType(uint32_t index);

  bool save(QString filename, Uuid bim_type);
  bool load(QString filename, Uuid bim_type);

  //Установка параметров биометрического образа (bim копируется)
  bool set(uint32_t index, float qual);
  bool set(uint32_t index, nbBim *bim);

  //Добавить в заданную позицию (0 - в начало, >=size - в конец)
  //(биометрический образ копируется)
  bool insert(uint32_t index, nbBim *bim, float qual);

  //Удалить в заданной позиции (0 - из головы, size-1 - последний)
  bool erase(uint32_t index);
  //Удалить все
  bool clear();

  //Признак изменения базы образов
  bool changed();
  //Сбросить признак изменения базы
  void unchange();

  static nbBim* newBim(uint32_t size);
  static bool freeBim(nbBim* bim, nbIBimProvider* provider = NULL);

protected:
  vector<nbBim *> _bims;    //вектор биометрических образов
  vector<float> _quals;   //вектор соответствующих значений качества
  nbIBimProvider *_provider;//провайдер, который выделил память под образы
  bool _changed;
};

#endif
