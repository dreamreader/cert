#include "containerquery.h"

// Конструктор по умолчанию
ContainerQuery::ContainerQuery(): Query(Query::Data)
{
}

ContainerQuery::~ContainerQuery()
{
}

// Создать запрос с заданными данными блоков
bool ContainerQuery::create(Nb::Container &container)
{
  Log::Tab tab;

  _blocks.clear();
  for (int i=0; i<container.count(); i++)
  {
    DataBlock* db = new DataBlock(container.at(i));
    _blocks.push_back(db);
    this->push(db);
    Log::write("container block ", i);
    Log::write("          size", db->expectedSize());
  }
  return true;
}

// Получить контейнер
Nbc* ContainerQuery::container()
{
  return &_cont;
}

// Принять запрос (override)
bool ContainerQuery::read(QTcpSocket &socket)
{
  Log::Tab tab;
  Log::write("start read query");

  if (!_headerBlock->readHeader(socket)) return false;
  if (!_headerBlock->readBody(socket)) return false;

  // Считать все прикреплённые блоки
  Log::write("containder blocks:", (int)_headerBlock->getCount());

  _cont.free();
  DataBlock *nBlock;
  for (int b = 0; b < _headerBlock->getCount(); b++)
    _cont.push(DataBlock());

  for (int b = 0; b < _headerBlock->getCount(); b++)
  {
    nBlock = (DataBlock*)(_cont.at(b));
    Log::write("read header block #", b);
    if (!nBlock->readHeader(socket)) return false;
    Log::write("read body block #", b);
    if (!nBlock->readBody(socket)) return false;
    _dataBlocks.push_back(nBlock);
  }
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool ContainerQuery::isOk()
{
  return (type() == Query::Data);
}
