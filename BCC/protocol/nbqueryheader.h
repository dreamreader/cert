#ifndef NBQUERYHEADER_H
#define NBQUERYHEADER_H

#include "../nb/nbTypes.h"

typedef uint8_t nbQueryFlags;
typedef uint8_t nbQueryType;

#define nbBT_PROTOCOL       (129)       // Блок протокола

/// Заголовок запроса
struct nbQueryHeader
{
  uint16_t              qcount;         // 2 байта. Число следующих за заголовком запроса блоков
  nbQueryFlags          qflags;         // 1 байт. Флаги запроса (не определены)
  nbQueryType           qtype;          // 1 байт. Тип запроса
};

#endif // NBQUERYHEADER_H
