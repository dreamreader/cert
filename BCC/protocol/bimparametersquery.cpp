#include "bimparametersquery.h"

// Конструктор по умолчанию
BimParametersQuery::BimParametersQuery(): Query(Query::Data)
{
}

// Создать запрос с заданными данными блоков
bool BimParametersQuery::create(QList<Nb::Matrix*> &bimParams, bool repeatData)
{
  clear();
  if (repeatData)
    Query::create(Query::RepeatData);
  else
    Query::create(Query::Data);

  Log::Tab tab;

  _bimBlocks.clear();
  for (unsigned i=0; i<bimParams.count(); i++)
  {
    Log::write("bimBlock #", (int)i);
    _bimBlocks.push_back(DataBlock());
    _bimBlocks[i].push(*bimParams[i]);
    this->push(&_bimBlocks[i]);
  }

  return true;
}

// Получить данные блоков запроса
bool BimParametersQuery::get(QList<Nb::Matrix*> &bimParams)
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
    bimBlock->pop(pos, *bimParams[i]);
  }
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool BimParametersQuery::isOk()
{
  return (((type() == Query::Data) || (type() == Query::RepeatData)));
}
