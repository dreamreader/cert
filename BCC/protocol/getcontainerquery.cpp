#include "getcontainerquery.h"

// Конструктор по умолчанию
GetContainerQuery::GetContainerQuery(): Query(Query::WantContainer)
{
}

// Создать запрос с заданными данными блоков
bool GetContainerQuery::create(QString userId)
{
  LOG

  Query::create(Query::WantContainer);
  Log::write("_userIdBlock");
  _userIdBlock.Block::push(Nb::Data(userId), true);

  this->push(&_userIdBlock);
  return true;
}

// Получить данные блоков запроса
bool GetContainerQuery::get(QString &userId)
{
  if (!isOk()) return false;

  Query::DataBlock *userIdBlock;
  userIdBlock = this->at(0);

  int pos;
  pos = 0;
  Nb::Data userData;
  userIdBlock->Block::pop(pos, userData, true);
  userId = userData.toString();
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool GetContainerQuery::isOk()
{
  return ((type() == Query::WantContainer) && (count() == blockCount));
}
