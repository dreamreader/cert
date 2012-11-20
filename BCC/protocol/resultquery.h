#ifndef RESULTQUERY_H
#define RESULTQUERY_H

#include "query.h"

class ResultQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  Query::DataBlock   _indicatorBlock;           ///< блок индикатора

public:
  /* Конструктор
   *
   */
  ResultQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(bool accessGranted, Nb::Matrix* indicator = nbNULL);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();

  /* Проверить результат аутентификации
   *
   */
  bool accessGranted();
};

#endif // RESULTQUERY_H
