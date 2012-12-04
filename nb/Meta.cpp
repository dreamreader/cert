//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:
//
//цель:
//
//
//
//////////////////////////////////////////////////////////////////////////
#include "Meta.h"

namespace Nb {

const Meta Meta::nil = Meta();

Meta::Meta(){
  set(0,MfAny,MtAny);
}

Meta::Meta(const Meta &meta){
  set(meta._m.count,meta._m.format,meta._m.type);
}

Meta::Meta(const nbMeta meta){
  set(meta.count,meta.format,meta.type);
}

Meta::Meta(uint32_t count, MetaFormat format, MetaType type){
  set(count,format,type);
}

void Meta::set(uint32_t count, MetaFormat format, MetaType type){
  _m.count = count;
  _m.format = format;
  _m.type = type;
}

//Проверка, является ли метаописание подмножеством range
bool Meta::subrange(const Meta &range) const {
  if (((_m.count == range._m.count)  || range.count() == 0) &&
      ((_m.format == range._m.format)|| range.format() == MfAny) &&
      ((_m.type == range._m.type)    || range.type() == MtAny))
    return true;
  return false;
}

//Проверка содержит ли метаописание диапазон
bool Meta::ranged() const {
  return (count() == 0 || format() == MfAny || type() == MtAny);
}

//Сравнение
bool Meta::equal(const Meta &m) const {
  return ((_m.count == m._m.count) &&
          (_m.format== m._m.format) &&
          (_m.type  == m._m.type));
}

//Метаинформация не определена
bool Meta::isEmpty() const {
  return equal(Meta(0,MfAny,MtAny));
}

//Длина элемента ВБП в битах
int32_t Meta::sizeOfItem() const {
  //Таблица, позволяющая выполнить переход от размера элемента в бита к размеру
  static const uint8_t sizes[16*8] = {
  //0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    0, 1, 2, 4, 8,16,32,64, 0, 0, 0, 0, 0, 0, 0, 0, // 0
    0, 1, 2, 4, 8,16,32,64, 0, 0, 0, 0, 0, 0, 0, 0, // 16
    0, 0, 0, 0, 0,16,32,64,80, 0, 0, 0, 0, 0, 0, 0, // 32
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 112
  };
  return sizes[_m.format&0x7F];
}

//Длина ВБП в байтах с выравниванием по байту
int32_t Meta::size() const {
  int32_t bits = sizeOfItem()*_m.count;
  return ((bits>>3) + (0 != (bits & 7)));
}

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug operator<<(QDebug  dbg, const Meta &x){
    return dbg.nospace()<<"("<<x._m.count<<" "<<x.format()<<" "<<x.type()<<")";
  }
#endif

}
