#include "startsignaturequery.h"

// Конструктор по умолчанию
StartSignatureQuery::StartSignatureQuery(): Query(Query::WantSignature)
{
}

// Создать запрос с заданными данными блоков
bool StartSignatureQuery::create(QString &userId, Nb::Data &data)
{
  LOG

  Query::create(Query::WantSignature);
  Log::write("_userIdBlock");
  _userIdBlock.Block::push(Nb::Data(userId), true);
  Log::write("_dataBlock");
  _dataBlock.Block::push(data);

  this->push(&_userIdBlock);
  this->push(&_dataBlock);
  return true;
}

// Получить данные блоков запроса
bool StartSignatureQuery::get(QString &userId, Nb::Data &data)
{
  if (!isOk()) return false;

  DataBlock *userIdBlock, *dataBlock;
  userIdBlock = this->at(0);
  dataBlock = this->at(1);

  int pos;
  pos = 0;
  Nb::Data userData;
  userIdBlock->Block::pop(pos, userData, true);
  userId = userData.toString();

  pos = 0;
  dataBlock->Block::pop(pos,data, true);
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool StartSignatureQuery::isOk()
{
  return ((type() == Query::WantSignature) && (count() == blockCount));
}
