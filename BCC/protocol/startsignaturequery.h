#ifndef STARTSIGNATUREQUERY_H
#define STARTSIGNATUREQUERY_H

#include "Query.h"
#include "../nb/Container.h"
#include "../nb/Matrix.h"

class StartSignatureQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  static const unsigned blockCount = 2;

  Query::DataBlock   _userIdBlock;
  Query::DataBlock   _dataBlock;

public:
  /* Конструктор по умолчанию
   *
   */
  StartSignatureQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(QString &userId, Nb::Data &data);

  /* Получить данные блоков запроса
   *
   */
  bool get(QString &userId, Nb::Data &data);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // STARTSIGNATUREQUERY_H
