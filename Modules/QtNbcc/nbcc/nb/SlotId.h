//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Объявление идентификатора слота
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_SLOT_ID
#define _NB_SLOT_ID

#include <QtCore/QList>

#include "Enums.h"

namespace Nb {

  class SlotId;
  typedef QList<SlotId> SlotIds;

//...........................................................................
//  Идентификатор слота
//...........................................................................
  class SlotId {
    nbSlotID _m;
  public:
    SlotId();
    SlotId(nbSlotID id);
    SlotId(SlotType st, uint16_t index);
    SlotId(SlotId id, SlotType st);

    inline operator nbSlotID*(){return &_m;}
    inline operator nbSlotID() const { return _m; }

    inline bool operator ==(const SlotId &id) const { return _m == id._m; }
    inline bool operator !=(const SlotId &id) const { return _m != id._m; }

    SlotType  type() const;
    uint16_t  index() const;
    uint8_t   bindex() const;
    BlockType btype() const;

    //Выходной слот
    SlotId static out;
    //Входной слот с заданным номером
    SlotId static prm(SlotType st, uint16_t i);
    //Входной слот с заданным номером
    SlotId static in(uint16_t i);
    //Слот преобразования
    SlotId static conv(BlockType bt, uint8_t i);
    //Слот преобразования по умолчанию
    SlotId static convDef(BlockType bt);
    //Специальный входной слот
    SlotId static sin(uint16_t i);
    //Специальный выходной слот
    SlotId static sout(uint16_t i);

    //Быстрое изменение типа
    SlotId as(SlotType st);
    static SlotId as(SlotId id, SlotType st);
    SlotId retype(SlotType st);
    static SlotId retype(SlotId id, SlotType st);

    static SlotIds fromIDs(const nbSlotID *id, int32_t count);

    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const SlotId &x);
    #endif
  };


#if !defined(QT_NO_DEBUG_STREAM)
  QDebug operator<<(QDebug  dbg, const SlotIds &x);
#endif

}

#endif
