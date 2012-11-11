#ifndef STATSQUERY_H
#define STATSQUERY_H

#include "query.h"
#include "../nb/statistics.h"

class StatsQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _statsBlock;   ///< блок со статистикой

public:
  /* Конструктор
   *
   */
  StatsQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(nbTestResult &stats);

  /* Получить данные блоков запроса
   *
   */
  bool get(nbTestResult &stats);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // STATSQUERY_H
