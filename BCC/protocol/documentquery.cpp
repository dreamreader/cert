#include "documentquery.h"

// Конструктор по умолчанию
DocumentQuery::DocumentQuery(): Query(Query::Data)
{
}

// Создать запрос с заданными данными блоков
bool DocumentQuery::create(Nb::Data &document)
{
  LOG

  Query::create(Query::Data);
  Log::write("_documentBlock");
  _documentBlock.push(document, true);

  this->push(&_documentBlock);
  return true;
}

// Получить данные блоков запроса
bool DocumentQuery::get(Nb::Data &document)
{
  if (!isOk()) return false;

  DataBlock *documentBlock;
  documentBlock = this->at(0);

  int pos;
  pos = 0;
  documentBlock->pop(pos, document, true);
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool DocumentQuery::isOk()
{
  return ((type() == Query::Data) && (count() == blockCount));
}
