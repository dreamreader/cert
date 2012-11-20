#ifndef AUTHQUERY_H
#define AUTHQUERY_H

#include "query.h"

class AuthQuery : public Query
{
public:
  //Включить все сигнатуры перегруженного метода
  using Query::create;

private:
  static const unsigned blockCount = 2;

  bool               _bio;
  Query::DataBlock   _userIdBlock;           ///< блок идентификатора пользователя
  Query::DataBlock   _keyBlock;         ///< блок пароля

public:
  /* Конструктор
   *
   */
  AuthQuery(bool bio = true);

  /* Создать запрос с заданными данными блоков
   *
   */
  bool create(QString userId, Nb::Data &key);

  /* Получить данные блоков запроса
   *
   */
  bool get(QString userId, Nb::Data &key);

  /* Проверить заполнение запроса (тип и число блоков)
   *
   */
  bool isOk();
};

#endif // AUTHQUERY_H
