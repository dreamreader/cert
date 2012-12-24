#ifndef BIMSQUERY_H
#define BIMSQUERY_H

#include "Query.h"
#include "../nb/Container.h"
#include "../nb/Matrix.h"

class BimParametersQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  QList<Query::DataBlock>   _bimBlocks;    ///< блоки биометрических параметров

public:
  /* Конструктор
   *
   */
  BimParametersQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(QList<Nb::Matrix*> &bimParams, bool repeatData = false);

  /* Получить данные блоков запроса
   *
   */
  bool get(QList<Nb::Matrix*> &bimParams);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // BIMSQUERY_H
