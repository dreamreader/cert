//////////////////////////////////////////////////////////////////////////
//файл:         
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Объявление универсального слота схемы преобразования -
//              параметра, преобразователя, вложенного преобразователя
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_SLOT_H
#define _NB_SLOT_H

#include <QtCore/QList>
#include <QtCore/QDataStream>

#include "Enums.h"
#include "SlotId.h"
#include "Meta.h"

namespace Nb {

class Scheme;
class Slot;
typedef QList<Slot*> Slots;

//....................................................................................
//Слот параметра
//  А. входного, специального входного, специального выходного
//  Б. выходного для преобразователя
//  В. выходного для вложенного преобразователя
//     примечание: список связанных слотов вложенного преобразователя должен формироваться
//                по тем же правилам, что и у обычного преобразователя
//....................................................................................
class Slot {
public:
  Slot();
  Slot(SlotId id, Meta meta);    //Конструктор слота вх., сп.вх., сп.вых. параметра
  Slot(SlotId id, Meta meta, const SlotIds &cids, BlockFlags bf = BfNone, BlockState bs = BsUntrained); //Конструктор слота преобразователя
  Slot(SlotId id, Meta meta, const SlotIds &cids, const SlotIds &rids, BlockFlags bf = BfNone, BlockState bs = BsUntrained, Scheme *r=Null); //Конструктор вложенного преобразователя

  virtual ~Slot();                                  //Деструктор

  inline SlotId  id() const { return _id; }         //Идентификатор
  inline Meta    meta() const { return _meta; }     //Метаописание

  inline Scheme* scheme() const { return _s; }      //Схема преобразования, в которую включен слот
  inline Scheme* rscheme() const { return _r; }     //Схема преобразования вложенного преобразователя

  inline BlockFlags bflags() const { return _bf; }  //Флаги для слота преобразователя
  inline BlockState bstate() const { return _bs; }  //Состояние для слота преобразователя

  inline const SlotIds&  cids() const { return _cids;}     //Список идентификаторов связанных слотов
  inline const SlotIds&  rids() const { return _rids;}     //Список номеров вложенных слотов
  inline int32_t  count() const { return _cids.count(); }  //Число связанных (отображенных) слотов
  void     ccount(int32_t *n);                      //Число связанных слотов по типам (индексы n : 0=вх.,1=сп.вх.,2=сп.вых.,3=вых.)
  void     rcount(int32_t *n);                      //Число отображенных слотов по типам (индексы n : 0=вх.,1=сп.вх.,2=сп.вых.,3=вых.)
  SlotIds cids(SlotType st) const;                  //Список идентификаторов связанных слотов по типу (вх.,сп.вх.,сп.вых)
  //Формирование списков индикаторов по типу (вх.,сп.вх.,сп.вых) путем добавления к имеющимся
  //примечание: io в общем случае содержит номер собственного слота, иначе обнаружена ошибка
  void    cids(SlotIds *ii, SlotIds *isi, SlotIds *iso, SlotIds *io) const;

  virtual bool copy(const Slot &slot);              //Копирование слота

  virtual int32_t sizeSlot(bool first=false);       //Размер для хранения описания слота
  virtual int32_t readSlot(SlotId id, QDataStream &from, int32_t remain, bool first=false); //Чтение описания из потока (идентификатор слота считывается предварительно в id)
  virtual int32_t writeSlot(QDataStream &to, bool first=false) const;      //Запись описания слота в поток

  friend class Scheme;


  //Проверка параметров схемы внутренних связей (C-схемы, если она установлена)
  virtual bool validate() const;
  //Проверка параметров схемы внешних связей (R-схемы, если она и S-схема установлена)
  virtual bool rvalidate() const;

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug virtual debug(QDebug  dbg) const ;
#endif

protected:
  //Обновление связей схемы со слотами C-схемы (должна быть установлена)
  virtual bool update();
  //Обновление связей схемы со слотами R-схемы (должна быть установлена)
  virtual bool rupdate();

  bool scheme(Scheme* s);   //Установка C-схемы преобразования
  bool rscheme(Scheme* r);  //Установка R-схемы преобразования

protected:
  //Получение последнего кода ошибки схемы
  Result rslt() const;
  //Установка кода ошибки (слота)
  //возвращает: false
  bool erslt(ResultCode rc) const;
  //Установка кода результата (например, причины внешней ошибки)
  //возвращает: признак установленной ошибки (false)/успеха (true)
  bool rslt(Result r) const;

private:
  Scheme       *_s;       //ссылка на схему
  Scheme       *_r;       //ссылка на вложенную схему
protected:
  SlotId        _id;      //номер
  Meta          _meta;    //метаописание
  BlockFlags    _bf;      //флаги преобразователя
  BlockState    _bs;      //состояние преобразователя

  SlotIds       _cids;    //список номеров связанных слотов (с измененным полем типа)
  SlotIds       _rids;    //список номеров отображенных слотов (поле тип указывается в _cids)
};

#if !defined(QT_NO_DEBUG_STREAM)
QDebug operator<<(QDebug  dbg, const Slot &x);
QDebug operator<<(QDebug  dbg, const Slots &x);
#endif
}

#endif //_NB_SLOT_H
