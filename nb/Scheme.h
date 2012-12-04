//////////////////////////////////////////////////////////////////////////
//файл:         
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Объявление схемы преобразования, фабрики слотов
//              для статического описания преобразования биометрия-код
//              по ГОСТ Р 52633.4, ГОСТ Р 52633.7.
//              Поддерживаются только схемы преобразования с фиксированным
//              числом связанных (отображенных) слотов.
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_SCHEME_H
#define _NB_SCHEME_H

#include <QtCore/QList>
#include <QtCore/QDataStream>

#include "Enums.h"
#include "Data.h"
#include "SlotId.h"
#include "Slot.h"

namespace Nb {


//....................................................................................
//Схема преобразования
//....................................................................................
class Scheme {
public:
  //....................................................................................
  //Фабрика слотов
  //(установка позиции выходного слота запрещает изменение схемы)
  //....................................................................................
  class Factory {
  public:
    virtual Slot* create(SlotId id) const;  //Создание слота по его типу
    virtual void  free(Slot* s) const;      //Удаление слота
  };

public:
  Scheme();
  Scheme(const Factory &factory);   //Конструктор
  virtual ~Scheme();                //Деструктор


  bool readScheme(Data &data);
  bool writeScheme(Data &data);




  const Factory& factory() const;   //Доступ к фабрике слотов
  static Factory& defaultFactory(); //Фабрика слотов "по умолчанию"

  bool isSet() const;               //Проверка определения схемы

  int32_t count() const;            //Число слотов в схеме
  SlotId  idOf(SlotId id) const;    //Номера слота по его идентификатору, или SlotId::out
  SlotId  idOf(int32_t pos) const;  //Номер слота по позиции или SlotId::out
  int32_t indexOf(SlotId id) const; //Поиск слота с указанным номером, возвращает его позицию
  int32_t indexOf(SlotId id, int32_t start) const;  //Поиск слота с указанными номером, возвращает его позицию
  int32_t indexOfLast(SlotType st, BlockType bt=BtExtention) const; //Позиция последнего слота с указанным типом
  Slot* at(SlotId id) const;        //Доступ по номеру (допускается SlotId::out)
  Slot* at(int32_t pos) const;      //Доступ по позиции

  //Доступ к списку номеров слотов схемы
  //примечание: номер SlotId::out получают с помощью id(SlotId::out)
  SlotIds ids() const;
  //Формирование списка номеров слотов с заданным типом
  //примечание: номер SlotId::out получают с помощью id(SlotId::out)
  //  st  - тип слота (вх., сп.вх., сп.вых, преобразователь)
  //  bt  - тип преобразователя (BtExtention включает всех слоты)
  SlotIds ids(SlotType st, BlockType bt=BtExtention) const;

  //Поиск слотов по списку номеров
  //примечание: если ordered установлен, то порядок в ids соответствует порядку слотов в схеме
  //            найденные слоты добавляются в список ss
  //возвращает: true, если найдены все слоты (без пропусков)
  //            false, если некоторые слоты не были найдены и для них были добавлены Null значения Slot*
  bool find(Slots &ss, const SlotIds &ids, bool ordered=false) const;

  //Поиск слотов с разделением их по типу параметров
  //примечание: слоты добавляются в имеющийся список
  //  si  - входные
  //  ssi - специальные входные
  //  sso - специальные выходные
  //  so  - выходные (преобразователи)
  bool find(Slots *si, Slots *ssi, Slots *sso, Slots *so) const;


  //Упорядочивание слотов в порядке объявления
  //примечание: пропущенные слоты (Null) исключаются из списка
  bool order(Slots &ss) const;

  //Поиск слотов, использующих слот, в том числе и многократно
  //  id    - номер слота и его тип
  //  multi - признак "ленивого" алгоритма, прекращающего поиск после
  //          нахождения однократного использования слота некоторым слотом
  //примечание: идентификатор слота должен указывать искомый тип использования
  //            (как вх., сп.вх., сп.вых., вых. параметр) по отношению
  //            к используемому слоту
  //            найденные слоты добавляются в список ss
  void used(Slots &ss, SlotId id, bool lazy=true) const;


  //Добавление слота с использованием статических объектов
  //  st  - тип слота для вх., сп.вх., сп.вых
  //  bt  - тип преобразователя для сп.вх.
  //  meta- метаописание
  //  bf  - флаги преобразователя
  //  bs  - состояние преобразователя
  //  (ct,ci)  - пара для связанных параметров: типы и порядковые номера параметров
  //  (rt,ri)  - пара для отображенных параметров: типы и порядковые номера параметров
  //примечание:
  //  номера параметров выбираются последовательно, начиная с 1 для вх., сп.вх., сп.вых. параметров
  //  номера для типов параметров выбираются последовательно для заданного типа, начиная с 1
  Scheme& push(SlotType st, Meta meta);
  Scheme& push(BlockType bt, Meta meta, const SlotTypes &ct, const SlotNums &ci, BlockFlags bf = BfNone);
  Scheme& push(Scheme &r, Meta meta, const SlotTypes &ct, const SlotNums &ci, const SlotNums &ri, BlockFlags bf = BfNone);


  //Создание схемы по образу и подобию
  Scheme& push(const Slot &s);    //Добавление слота в конец схемы с использованием фабрики классов
  Scheme& out(SlotId id);         //Маркировка выходного слота, имеющегося в схеме преобразования
  Scheme& out(int32_t pos);       //Маркировка выходного слота по позиции (считая  с 0)

  virtual void free();                      //Освобождение схемы
  virtual bool copy(const Scheme &scheme);  //Копирование схемы с использованием фабрики классов

  //Проверка корректности схемы (связность элементов и непротиворечивость номеров)
  virtual bool validate();
  virtual bool rvalidate();

  //Полный размер схемы для сохранения
  int32_t sizeOfScheme() const;
  //Чтение схемы преобразования из потока
  virtual int32_t readScheme(QDataStream &from, int32_t remain);
  //Запись схемы преобразования в поток
  //  to  - выходной поток
  //  asData  - признак записи в формате Data (с полем int32_t длины схемы)
  virtual int32_t writeScheme(QDataStream &to, bool withSize = false) const;

  //Чтение значения результата последней операции
  inline Result rslt() const { return _rslt; }
  //Установка кода последней операции
  inline bool rslt(Result r){ _rslt = r; return _rslt.succeeded(); }
  inline bool rslt(SlotId id, ResultCode rc){ _rslt = Result::fail(id,rc); return _rslt.succeeded(); }

#if !defined(QT_NO_DEBUG_STREAM)
  virtual QDebug debug(QDebug  dbg) const;
#endif

protected:
  //Проверка на цикличность
  bool circled() const;
  //Сортировка
  static void mathQSort(int32_t A[], void** P, int32_t nelms);
  template<class T> static inline void mathSwap(T &x, T &y){ T temp = x; x = y;  y = temp; }
protected:
  const Scheme::Factory  &_factory; //фабрика классов
  Slots   _slots;                   //список слотов схемы
  int32_t _posOut;                  //позиция выходного слота схемы (по умолчанию устанавливается в последнюю позицию слота)
  Result  _rslt;                    //код результата
};

#if !defined(QT_NO_DEBUG_STREAM)
QDebug operator<<(QDebug  dbg, const Scheme &x);
#endif
}

#endif //_NB_SCHEME_H
