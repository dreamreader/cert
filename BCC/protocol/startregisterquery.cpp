#include "startregisterquery.h"

// Конструктор по умолчанию
StartRegisterQuery::StartRegisterQuery(): Query(Query::WantRegister)
{
}

// Создать запрос с заданными данными блоков
bool StartRegisterQuery::create(QString &userId)
{
  LOG

  Query::create(Query::WantRegister);
  Log::write("_userIdBlock");
  _userIdBlock.Block::push(Nb::Data(userId), true);

  this->push(&_userIdBlock);
  return true;
}

// Получить данные блоков запроса
bool StartRegisterQuery::get(QString &userId)
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
bool StartRegisterQuery::isOk()
{
  return ((type() == Query::WantRegister) && (count() == blockCount));
}
