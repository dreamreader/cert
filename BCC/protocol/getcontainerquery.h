#ifndef GETCONTAINERQUERY_H
#define GETCONTAINERQUERY_H

#include "query.h"

class GetContainerQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _userIdBlock;           ///< блок идентификатора пользователя

public:
  /* Конструктор
   *
   */
  GetContainerQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(QString userId);

  /* Получить данные блоков запроса
   *
   */
  bool get(QString &userId);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // GETCONTAINERQUERY_H
