#include "resultquery.h"

// Конструктор по умолчанию
ResultQuery::ResultQuery(): Query(Query::Denied)
{
}

// Создать запрос с заданными данными блоков
bool ResultQuery::create(bool accessGranted, Nb::Matrix *indicator)
{
  LOG

  this->Query::create( accessGranted ? Query::Granted : Query::Denied );

  if (indicator)
  {
    Log::write("_indicatorBlock");
    _indicatorBlock.push(*indicator);
    this->push(&_indicatorBlock);
  }
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool ResultQuery::isOk()
{
  return ((type() == Query::Granted) || (type() == Query::Denied));
}

// Проверить результат аутентификации
bool ResultQuery::accessGranted()
{
  return (type() == Query::Granted);
}
