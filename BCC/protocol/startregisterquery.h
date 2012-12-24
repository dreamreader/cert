#ifndef STARTREGISTERQUERY_H
#define STARTREGISTERQUERY_H

#include "Query.h"
#include "../nb/Container.h"
#include "../nb/Matrix.h"
#include "../nb/Data.h"
#include "../nb/Scheme.h"

class StartRegisterQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _userIdBlock;

public:
  /* Конструктор по умолчанию
   *
   */
  StartRegisterQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(QString &userId);

  /* Получить данные блоков запроса
   *
   */
  bool get(QString &userId);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // STARTREGISTERQUERY_H
