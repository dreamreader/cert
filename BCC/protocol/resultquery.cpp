#include "resultquery.h"

// Конструктор по умолчанию
ResultQuery::ResultQuery(): Query(Query::Denied)
{
}

// Создать запрос с заданными данными блоков
bool ResultQuery::create(bool accessGranted, Nb::Matrix &indicator)
{
  Log::Tab tab;

  Log::write("_indicatorBlock");
  this->Query::create( accessGranted ? Query::Granted : Query::Denied );
  _indicatorBlock.push(indicator);

  this->push(&_indicatorBlock);
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool ResultQuery::isOk()
{
  return (((type() == Query::Granted) || (type() == Query::Denied)) && (count() == blockCount));
}

// Проверить результат аутентификации
bool ResultQuery::accessGranted()
{
  return (type() == Query::Granted);
}
