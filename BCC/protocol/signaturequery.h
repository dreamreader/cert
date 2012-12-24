#ifndef SIGNATUREQUERY_H
#define SIGNATUREQUERY_H

#include "Query.h"

class SignatureQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _sigBlock;           ///< блок биометрической метки

public:
  /* Конструктор
   *
   */
  SignatureQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(Nb::Data &scheme);

  /* Получить данные блоков запроса
   *
   */
  bool get(Nb::Data &scheme);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // SIGNATUREQUERY_H
