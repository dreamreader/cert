#include "schemequery.h"

// Конструктор по умолчанию
SchemeQuery::SchemeQuery(): Query(Query::Ans)
{
}

// Создать запрос с заданными данными блоков
bool SchemeQuery::create(Nb::Scheme &scheme)
{
  Log::Tab tab;
  Log::write("_schemeBlock");
  _schemeBlock.Block::push(scheme);

  this->push(&_schemeBlock);
  return true;
}

// Получить данные блоков запроса
bool SchemeQuery::get(Nb::Scheme &scheme)
{
  if (!isOk()) return false;

  DataBlock *schemeBlock;
  schemeBlock = this->at(0);

  int pos;
  pos = 0;
  schemeBlock->Block::pop(pos, scheme);
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool SchemeQuery::isOk()
{
  return ((type() == Query::Ans) && (count() == blockCount));
}
