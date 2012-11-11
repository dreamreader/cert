#ifndef CONTAINERNAMESQUERY_H
#define CONTAINERNAMESQUERY_H

#include "query.h"
#include <QStringList>

class ContainerNamesQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  static const unsigned blockCount = 1;

  QStringList         names;                ///< имена
  Query::DataBlock   _namesBlock;           ///< блок имён

public:
  /* Конструктор
   *
   */
  ContainerNamesQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(QStringList &names);

  /* Получить данные блоков запроса
   *
   */
  bool get(QStringList &names);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // CONTAINERNAMESQUERY_H
