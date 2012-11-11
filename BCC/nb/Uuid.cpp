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
#include "Uuid.h"

namespace Nb {

  Uuid::Uuid(){
  }

  Uuid::Uuid( const char* text):QUuid(text){
  }

  Uuid::Uuid(uint32_t d, uint16_t w1, uint16_t w2,
        uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7):
        QUuid(d,w1,w2,b0,b1,b2,b3,b4,b5,b6,b7){
  }

  Uuid::Uuid(const QString & text):QUuid(text){
  }

  Uuid::Uuid(const nbUuid & g):
        QUuid(g.d1,g.d2,g.d3,g.d4[0],g.d4[1],g.d4[2],g.d4[3],g.d4[4],g.d4[5],g.d4[6],g.d4[7]){
  }

  Uuid::operator QString () const {
    return *(QUuid*)this;
  }

  Uuid::operator nbUuid() const{
    return *(nbUuid*)this;
  }

  nbUuid* Uuid::data(){
    return (nbUuid*)this;
  }
  const nbUuid* Uuid::data() const{
    return (const nbUuid*)this;
  }

  const Uuid Uuid::nil = Uuid();

  QDebug operator<<(QDebug  dbg, const Uuid &x){
    if (x==Uuid::nil) return dbg.nospace()<<"{Nil}";
    else return dbg.nospace()<<((QString)x).toAscii().data();
  }

}
