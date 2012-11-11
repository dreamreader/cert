//////////////////////////////////////////////////////////////////////////
//файл:
//верси€:
//дата:
//изменен:
//автор:
//
//цель:
//
//
//
//////////////////////////////////////////////////////////////////////////
#include "SlotId.h"

namespace Nb {

SlotId::SlotId():_m(nbSLOT_OUT){

}

SlotId::SlotId(nbSlotID id):_m(id){

}
SlotId::SlotId(SlotId id, SlotType st):_m(st | id.index()){

}

SlotId::SlotId(SlotType st, uint16_t index):_m(st|index){

}

SlotType  SlotId::type() const   {
  return SlotType(_m & nbSLOT_MASK_TYPE);
}

uint16_t  SlotId::index() const  { 
  return _m & nbSLOT_MASK_INDEX; 
}

uint8_t   SlotId::bindex() const {
  return (uint8_t)(_m & nbSLOT_MASK_BINDEX);
}

BlockType SlotId::btype() const  { 
  return BlockType((_m & nbSLOT_MASK_BTYPE)>>7); 
}

SlotId SlotId::out = SlotId(nbSLOT_OUT);


//¬ходной слот с заданным номером
SlotId SlotId::prm(SlotType st, uint16_t i){
  return SlotId(st,i);
}

SlotId SlotId::in(uint16_t i) {
  return SlotId(nbSLOT_IN(i));
}

SlotId SlotId::conv(BlockType bt, uint8_t i) {
  return SlotId(nbSLOT_CONV(bt,i));
}

SlotId SlotId::convDef(BlockType bt){
  return SlotId(nbSLOT_CONV_DEFAULT(bt));
}

SlotId SlotId::sin(uint16_t i){
  return SlotId(nbSLOT_SPEC_IN(i));
}

SlotId SlotId::sout(uint16_t i){
  return SlotId(nbSLOT_SPEC_OUT(i));
}

//Ѕыстрое изменение типа
SlotId SlotId::as(SlotType st){
  return SlotId(st | index());
}

SlotId SlotId::as(SlotId id, SlotType st){
  return SlotId(st | id.index());
}

SlotId SlotId::retype(SlotType st){
  return SlotId(st | index());
}

SlotId SlotId::retype(SlotId id, SlotType st){
  return SlotId(st | id.index());
}


SlotIds SlotId::fromIDs(const nbSlotID *id, int32_t count){
  SlotIds ids;
  for (int32_t i=0; i<count; ++i){
    ids.append(SlotId(id[i]));
  }
  return ids;
}


#if !defined(QT_NO_DEBUG_STREAM)
  QDebug operator<<(QDebug  dbg, const SlotId &x){
    if (x.type() != StOut){
      return dbg.nospace()<<"("<<x.type()<<" "<<x.index()<<")";
    } else {
      return dbg.nospace()<<"("<<x.type()<<" "<<x.btype()<<" "<<x.bindex()<<")";
    }
  }

  QDebug operator<<(QDebug  dbg, const SlotIds &x){
    QDebug d=dbg.nospace();
    d<<"[";
    for (int i=0; i<x.count(); ++i){
      if (i) d<<" ";
      d<<x[i];
    }
    return d<<"]";
  }

#endif

}
