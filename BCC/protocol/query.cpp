#include "Query.h"


// Конструктор по умолчанию
Query::Query(): _skipHeader(false)
{
  _headerBlock = new QueryHeaderBlock();
  _headerBlock->setCount(0);
}

// Конструктор с типом
Query::Query(Type type): _skipHeader(false)
{
  _headerBlock = new QueryHeaderBlock();
  _headerBlock->setCount(0);
  _headerBlock->setType(type);
}

// Деструктор
Query::~Query()
{
  delete _headerBlock;
  free();
}

// Освободить выделенные блоки
bool Query::free()
{
  for (int b = 0; b < _allocBlocks.count(); b++)
  {
    delete _allocBlocks[b];
  }
  _allocBlocks.clear();
  return true;
}

// Получить поличество блоков
int Query::count()
{
  return _headerBlock->getCount();
}

// Очистить запрос
bool Query::clear()
{
  _headerBlock->setType(Unknown);
  _headerBlock->setCount(0);
  _dataBlocks.clear();
  free();
  return true;
}

// Создать запрос
bool Query::create(Type type)
{
  clear();
  _headerBlock->setType(type);
  return true;
}

// Создать запрос с заданным заголовком
bool Query::create(Query::QueryHeaderBlock &header)
{
  _skipHeader = true;
  _headerBlock->copy(header);
  return true;
}

// Проверить запрос
bool Query::typeIs(Type type)
{
  return (_headerBlock->getType() == type);
}

// Получить тип
Query::Type Query::type()
{
  return _headerBlock->getType();
}

// Добавить блок в конец
bool Query::push(DataBlock* block)
{
  _dataBlocks.push_back(block);
  _headerBlock->setCount(_headerBlock->getCount()+1);
  return true;
}

// Получить блок
Query::DataBlock* Query::at(unsigned pos)
{
  return _dataBlocks.at(pos);
}

// Удалить первый блок
bool Query::pop(DataBlock* block)
{
  if (_headerBlock->getCount() == 0) return false;
  block = _dataBlocks.at(0);
  _dataBlocks.pop_front();
  _headerBlock->setCount(_headerBlock->getCount()-1);
  return true;
}

// Принять запрос
bool Query::read(QTcpSocket &socket)
{
  Log::Tab tab;
  Log::write("start read query");

  if (!_skipHeader)
  {
    if (!_headerBlock->read(socket)) return false;
  }
  else
    Log::write("skip header (already readed)");

  // Считать все прикреплённые блоки
  Log::write("data blocks:", (int)_headerBlock->getCount());
  for (int b = 0; b < _headerBlock->getCount(); b++)
  {
    DataBlock* nBlock = new DataBlock();
    if (!nBlock->read(socket)) return false;
    _dataBlocks.push_back(nBlock);
    _allocBlocks.push_back(nBlock);
  }

  Log::write("available:", (int)socket.bytesAvailable());

  return true;
}

// Отправить запрос
bool Query::write(QTcpSocket &socket)
{
  Log::Tab tab;
  Log::write("start write query");

  bool res = _headerBlock->write(socket);
  if (!res) return false;

  // Записать все прикреплённые блоки
  Log::write("data blocks:", (int)_headerBlock->getCount());
  for (int b = 0; b < _headerBlock->getCount(); b++)
  {
    // Записать блок
    DataBlock* nblock = _dataBlocks[b];
    res = nblock->write(socket);
    if (!res) return false;
  }

  return true;
}


Query::DataBlock::DataBlock(): Nb::Block(nbBT_PROTOCOL), _ref(nbNULL)
{
}

Query::DataBlock::DataBlock(Nb::Block *block)
{
  _ref = block;
}

// Считать заголовок
bool Query::DataBlock::readHeader(QTcpSocket &socket)
{
  Log::Tab tab;

  _ref = nbNULL;
  // Считать заголовок блока
  Log::write("start read block header");
  while(socket.bytesAvailable() < sizeof(nbBlockHeader))
  {
    Log::write("wait...");
    if (!socket.waitForReadyRead(300000))
    {
      Log::write("connection closed");
      return false;
    }
    Log::write("ready read, available", (int)socket.bytesAvailable());
  }

  // Считать блок
  bool res = (socket.read((char*)&_h, sizeof(nbBlockHeader)) != -1);
  Log::write("reading header:", res);
  Log::write("        body size:", (int)((_h.sizeHi << 16) | _h.sizeLo));
  return res;
}

// Считать тело
bool Query::DataBlock::readBody(QTcpSocket &socket)
{
  Log::Tab tab;

  _ref = nbNULL;
  qint64 dataSize = Block::getHSize();
  if (dataSize < 0)
    return false;
  if (dataSize == 0)
    return true;

  while(socket.bytesAvailable() < dataSize)
  {
    Log::write("wait...");
    if (!socket.waitForReadyRead(300000))
    {
      Log::write("connection closed");
      return false;
    }
    Log::write("ready read, available", (int)socket.bytesAvailable());
  }
  QDataStream stream;
  stream.setDevice(&socket);
  bool res = (Block::read(_h, stream) != -1);
  Log::write("reading body:", res);
  return res;
}

// Считать блок
bool Query::DataBlock::read(QTcpSocket &socket)
{
  return (readHeader(socket) &&
          readBody(socket));
}

// Дублировать блок
Query::DataBlock* Query::DataBlock::dup() const
{
  DataBlock* block = new DataBlock;
  block->copy(*this);
  return block;
}

// Записать блок
bool Query::DataBlock::write(QTcpSocket &socket) const
{
  QDataStream stream;
  stream.setDevice(&socket);

  Log::write("start write block");

  bool res = false;
  if (_ref)
    res = (_ref->write(stream) != -1);
  else
    res = (Block::write(stream) != -1);

  Log::write("writing header:", res);
  Log::write("        body size:", (int)(expectedSize()));

  return res;
}

// Получить размер ожидаемого блока данных (по данным заголовка)
int32_t Query::DataBlock::expectedSize() const
{
  if (_ref)
    return _ref->size() - sizeof(nbBlockHeader);
  return Block::getHSize();
}

Query::QueryHeaderBlock::QueryHeaderBlock()
{
  nbQueryHeader h;
  push((uint8_t*)&h, sizeof(h));
}

void Query::QueryHeaderBlock::free()
{
  DataBlock::free();
  nbQueryHeader h;
  push((uint8_t*)&h, sizeof(h));
}

// Получить количество блоков данных запроса
uint16_t& Query::QueryHeaderBlock::getCount()
{
  return ((nbQueryHeader*)Nb::Block::data())->qcount;
}

// Установить количество блоков данных запроса
void Query::QueryHeaderBlock::setCount(uint16_t count)
{
  ((nbQueryHeader*)Nb::Block::data())->qcount = count;
}


// Получить тип запроса
Query::Type Query::QueryHeaderBlock::getType()
{
  return (Query::Type)(((nbQueryHeader*)Nb::Block::data())->qtype);
}

// Установить тип запроса
void Query::QueryHeaderBlock::setType(Query::Type type)
{
  ((nbQueryHeader*)Nb::Block::data())->qtype = type;
}

// Проверить тип запроса
bool Query::QueryHeaderBlock::typeIs(Type type)
{
  return (this->getType() == type);
}


// Получить флаги запроса
nbQueryFlags& Query::QueryHeaderBlock::getFlags()
{
  return ((nbQueryHeader*)Nb::Block::data())->qflags;
}

// Установить флаги запроса
void Query::QueryHeaderBlock::setFlags(nbQueryFlags flags)
{
  ((nbQueryHeader*)Nb::Block::data())->qflags = flags;
}

// Считать тело
bool Query::QueryHeaderBlock::readBody(QTcpSocket &socket)
{
  Log::Tab tab;

  _ref = nbNULL;
  qint64 dataSize = Block::getHSize();
  if (dataSize < 0)
    return false;
  if (dataSize == 0)
    return true;

  while(socket.bytesAvailable() < dataSize)
  {
    Log::write("wait...");
    if (!socket.waitForReadyRead(300000))
    {
      Log::write("connection closed");
      return false;
    }
    Log::write("ready read, available", (int)socket.bytesAvailable());
  }
  QDataStream stream;
  stream.setDevice(&socket);
  bool res = (Block::read(_h, stream) != -1);
  Log::write("reading query header:", res);
  Log::write("        query header: type", ((nbQueryHeader*)this->data())->qtype);
  Log::write("        query header: count ", ((nbQueryHeader*)this->data())->qcount);
  return res;
}

