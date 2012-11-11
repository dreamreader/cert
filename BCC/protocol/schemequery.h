#ifndef SCHEMEQUERY_H
#define SCHEMEQUERY_H

#include "query.h"

class SchemeQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _schemeBlock;           ///< блок схемы аутентификации

public:
  /* Конструктор
   *
   */
  SchemeQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(Nb::Scheme &scheme);

  /* Получить данные блоков запроса
   *
   */
  bool get(Nb::Scheme &scheme);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // SCHEMEQUERY_H
