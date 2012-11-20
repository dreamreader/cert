#include "testquery.h"

// Конструктор по умолчанию
TestQuery::TestQuery(): Query(Query::Test)
{
}

// Создать запрос с заданными данными блоков
bool TestQuery::create(QList<Nb::Matrix*> &bimParams)
{
  LOG

  Query::create(Query::Test);
  Log::write("_bimBlock");

  _bimBlocks.clear();
  for (unsigned i=0; i<bimParams.count(); i++)
  {
    Log::write("bimBlock #", (int)i);
    _bimBlocks.push_back(DataBlock());
    _bimBlocks[i].Block::push(*bimParams[i]);
    this->push(&_bimBlocks[i]);
  }

  return true;
}

// Получить данные блоков запроса
bool TestQuery::get(QList<Nb::Matrix*> &bimParams)
{
  if (!isOk()) return false;
  if (bimParams.count() != this->count())
    return false;

  DataBlock *bimBlock;
  int pos;

  for (unsigned i=0; i<this->count(); i++)
  {
    bimBlock = this->at(i);
    pos = 0;
    bimBlock->Block::pop(pos, *bimParams[i]);
  }
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool TestQuery::isOk()
{
  return (type() == Query::Test);
}
