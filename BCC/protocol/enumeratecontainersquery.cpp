#include "enumeratecontainersquery.h"

#include "startregisterquery.h"

// Конструктор по умолчанию
EnumerateContainersQuery::EnumerateContainersQuery(): Query(Query::WantContainers)
{
}

// Создать запрос с заданными данными блоков
bool EnumerateContainersQuery::create(QString &userId)
{
  Log::Tab tab;
  Log::write("_userIdBlock");
  _userIdBlock.Block::push(Nb::Data(userId), true);

  this->push(&_userIdBlock);
  return true;
}

// Получить данные блоков запроса
bool EnumerateContainersQuery::get(QString &userId)
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
bool EnumerateContainersQuery::isOk()
{
  return ((type() == Query::WantContainers) && (count() == blockCount));
}
