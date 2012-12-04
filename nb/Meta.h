//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Объявление класса метаописания формата данных
//
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_META_H
#define _NB_META_H

#include "Enums.h"

#pragma pack(1)

namespace Nb {

//.............................................................................
//  Метаинформация ВБП
//.............................................................................
class Meta {
  nbMeta _m;
public:
  Meta();
  Meta(const Meta &meta);
  Meta(const nbMeta meta);
  Meta(uint32_t count, MetaFormat format, MetaType type);

  inline operator nbMeta*(){return &_m;}
  inline operator nbMeta() const { return _m; }

  void set(uint32_t count, MetaFormat format, MetaType type);
  
  inline uint32_t count() const{ return _m.count; }
  inline MetaFormat format() const{ return MetaFormat(_m.format); }
  inline MetaType type() const{ return MetaType(_m.type); }
  
  inline void count(uint32_t cnt) { _m.count = cnt; }
  inline void format(MetaFormat mf) { _m.format = mf; }
  inline void type(MetaType mt) { _m.type = mt; }

  inline bool operator==(const Meta &meta) const { return equal(meta); }
  inline bool operator!=(const Meta &meta) const { return !equal(meta); }
  inline Meta& operator=(const Meta &meta) { set(meta.count(),meta.format(),meta.type()); return *this; }

  //Проверка на равенство
  bool equal(const Meta &meta) const;
  //Проверка, является ли метаописание подмножеством range
  bool subrange(const Meta &range) const;
  //Проверка содержит ли метаописание диапазон
  bool ranged() const;
  //Проверка на неопределенное значение
  bool isEmpty() const;

  //Длина элемента ВБП в битах
  int32_t sizeOfItem() const;
  //Длина ВБП в байтах с выравниванием по байту
  int32_t size() const;

  static const Meta nil;

  enum {
    SizeOfByte = 8
  };

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug friend operator<<(QDebug  dbg, const Meta &x);
#endif
};  

}

#pragma pack()
#endif //_NB_META_H
