#ifndef PASSWORDQUERY_H
#define PASSWORDQUERY_H

#include "query.h"

class PasswordQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _passwordBlock;           ///< блок пароля

public:
  /* Конструктор
   *
   */
  PasswordQuery();

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(Nb::Matrix &password);

  /* Получить данные блоков запроса
   *
   */
  bool get(Nb::Matrix &password);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // PASSWORDQUERY_H
