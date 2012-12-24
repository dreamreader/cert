#ifndef TESTQUERY_H
#define TESTQUERY_H

#include "Query.h"

class TestQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  QList<Query::DataBlock>   _bimBlocks;        ///< блоки тестового образа

public:
  /* Конструктор
   *
   */
  TestQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(QList<Nb::Matrix*> &bimParams);

  /* Получить данные блоков запроса
   *
   */
  bool get(QList<Nb::Matrix*> &bimParams);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // TESTQUERY_H
