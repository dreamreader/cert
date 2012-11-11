//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Уникальный универсальный идентификатор
//
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_UUID_H
#define _NB_UUID_H

#include <QtCore/QUuid>
#include <QtCore/QString>
#include <QDebug>

#include "Enums.h"

namespace Nb {

//...........................................................................
//  Уникальный универсальный идентификатор
//...........................................................................
using namespace std;

class Uuid:public QUuid {
public:
  Uuid();
  Uuid(uint32_t d, uint16_t w1, uint16_t w2,
        uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7);
  Uuid( const QString & text);
  Uuid( const char* text);
  Uuid( const nbUuid & guid);

  operator QString () const;
  operator nbUuid() const;
  nbUuid* data();
  const nbUuid* data() const;

  enum {
    uuidSIZE = 16,
    uuidBITS = 128
  };

  static const Uuid nil;

  #if !defined(QT_NO_DEBUG_STREAM)
    QDebug friend operator<<(QDebug  dbg, const Uuid &x);
  #endif
};

typedef QList<Uuid> Uuids;

}

#endif //_NB_UUID_H
