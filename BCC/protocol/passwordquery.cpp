#include "passwordquery.h"

// Конструктор по умолчанию
PasswordQuery::PasswordQuery(): Query(Query::Password)
{
}

// Создать запрос с заданными данными блоков
bool PasswordQuery::create(Nb::Matrix &password)
{
  Log::Tab tab;
  Log::write("_passwordBlock");
  _passwordBlock.push(password);

  this->push(&_passwordBlock);
  return true;
}

// Получить данные блоков запроса
bool PasswordQuery::get(Nb::Matrix &password)
{
  if (!isOk()) return false;

  DataBlock *passwordBlock;
  passwordBlock = this->at(0);

  int pos;
  pos = 0;
  passwordBlock->pop(pos, password);
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool PasswordQuery::isOk()
{
  return ((type() == Query::Password) && (count() == blockCount));
}

