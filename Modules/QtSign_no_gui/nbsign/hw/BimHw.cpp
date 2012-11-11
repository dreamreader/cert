/****************************************************************************
** файл:         BimHw.cpp
** версия:
** дата:
** изменен:
** автор:
**
** описание:     Определение биометрического образа (БО), форматов его
**               хранения, а также специфичных типов данных и структур.
****************************************************************************/

#include <string.h>
#include "BimHw.h"

// Конструктор
BimHw::BimHw():_bim(nbNULL){

}
BimHw::BimHw(uint16_t num, nbTabletOptions params):_bim(nbNULL) {
  create(num,params);
}

// Деструктор
BimHw::~BimHw(){
  free();
}

//назначение: выделение памяти
//параметры:
//  num    - [вх] количество точек
//  params - [вх] настройки планшета
//
bool BimHw::create(uint16_t num, nbTabletOptions params) {
  free();

  // Выделить память
  uint32_t size = sizeof(nbBimHwHeader)+ num*sizeof(nbPointHw); // размер заголовка + Х + Y + P + T

  nbBimHwHeader *img = (nbBimHwHeader*) new uint8_t[size];
  if (!img) return false;

  img->num     = num;
  img->maxX    = params.maxX;
  img->maxY    = params.maxY;
  img->maxP    = params.maxP;
  img->dpiX    = (uint16_t)(params.ppiX*params.maxX/params.scrW);
  img->dpiY    = (uint16_t)(params.ppiY*params.maxY/params.scrH);
  memset(img->phrase, 0, sizeof(img->phrase));
  img->size    = size-sizeof(nbBim);
  img->bimType = nbBIM_TYPE_HANDWR;

  _bim = img;
  return true;
}

// Освобождение памяти
bool BimHw::free(){
  if (!empty()) { delete[] _bim; _bim = nbNULL; }
  return true;
}

// Копирование рукописного образа
bool BimHw::fromBim(const nbBim* bim){
  free();
  if (!bim) return true;

  _bim = (nbBim*)new uint8_t[bim->size+sizeof(nbBim)];
  if (!_bim) return false;
  for (uint32_t i=0; i<(bim->size+sizeof(nbBim)); ++i){
    *((uint8_t*)_bim+i) = *((uint8_t*)bim+i);
  }
  return true;
}

// Присоединение биометрического образа
bool BimHw::attach(nbBim *bim) {
  free();
  _bim = bim;
  return true;
}

// Отсоединение биометрического образа
nbBim* BimHw::detach() {
  nbBim *bim = _bim;
  _bim = nbNULL;
  return bim;
}

// Доступ к заголовку рукописного образа
nbBimHwHeader* BimHw::header() const {
  return (nbBimHwHeader*)_bim;
}

// Доступ к точкам рукописного образа
uint8_t* BimHw::data() const {
  if (!empty()) return (uint8_t*)(header()+1);
  return nbNULL;
}

// Доступ к рукописному образу
nbBim* BimHw::handle() const {
  return _bim;
}

// Тип
nbUuid BimHw::type() const {
  if (!empty()) return _bim->bimType;
  return nbUUID_NIL;
}

// Размер
uint32_t BimHw::size() const {
  if (!empty()) return (_bim->size+sizeof(nbBim));
  return nbNIL;
}

// Проверка наличия образа
uint32_t BimHw::empty() const {
  return (_bim == nbNULL);
}
