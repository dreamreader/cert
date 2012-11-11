#include "startregisterquery.h"

// Конструктор по умолчанию
StartRegisterQuery::StartRegisterQuery(): Query(Query::WantRegister)
{
}

// Создать запрос с заданными данными блоков
bool StartRegisterQuery::create(QString &userId, QString &containerId, QString &regkey)
{
  Log::Tab tab;
  Log::write("_userIdBlock");
  _userIdBlock.Block::push(Nb::Data(userId), true);
  Log::write("_containerIdBlock");
  _containerIdBlock.Block::push(Nb::Data(containerId), true);
  Log::write("_regkeyBlock");
  _regkeyBlock.Block::push(Nb::Data(regkey), true);

  this->push(&_userIdBlock);
  this->push(&_containerIdBlock);
  this->push(&_regkeyBlock);
  return true;
}

// Получить данные блоков запроса
bool StartRegisterQuery::get(QString &userId, QString &containerId, QString &regkey)
{
  if (!isOk()) return false;

  Query::DataBlock *userIdBlock, *containerIdBlock, *regkeyBlock;
  userIdBlock = this->at(0);
  containerIdBlock = this->at(1);
  regkeyBlock = this->at(2);

  int pos;
  pos = 0;
  Nb::Data userData;
  userIdBlock->Block::pop(pos, userData, true);
  userId = userData.toString();

  pos = 0;
  Nb::Data containerData;
  containerIdBlock->Block::pop(pos,containerData, true);
  containerId = containerData.toString();

  pos = 0;
  Nb::Data regkeyData;
  regkeyBlock->Block::pop(pos,regkeyData, true);
  regkey = regkeyData.toString();

  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool StartRegisterQuery::isOk()
{
  return ((type() == Query::WantRegister) && (count() == blockCount));
}
