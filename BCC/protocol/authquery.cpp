#include "authquery.h"

// Конструктор по умолчанию
AuthQuery::AuthQuery(bool bio): _bio(bio), Query(_bio ? Query::WantAuthBio : Query::WantAuth)
{
}

// Создать запрос с заданными данными блоков
bool AuthQuery::create(QString userId, Nb::Data &key)
{
  LOG

  Query::create(_bio ? Query::WantAuthBio : Query::WantAuth);
  Log::write("_userIdBlock");
  _userIdBlock.Block::push(Nb::Data(userId), true);
  Log::write("_passwordBlock");
  _keyBlock.push(key, true);

  this->push(&_userIdBlock);
  this->push(&_keyBlock);
  return true;
}

// Получить данные блоков запроса
bool AuthQuery::get(QString userId, Nb::Data &key)
{
  if (!isOk()) return false;

  DataBlock *keyBlock;
  DataBlock *userBlock;
  userBlock = this->at(0);
  keyBlock = this->at(1);

  int pos;
  pos = 0;
  Nb::Data userData;
  userBlock->Block::pop(pos, userData, true);
  userId = userData.toString();
  pos = 0;
  keyBlock->pop(pos, key, true);
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool AuthQuery::isOk()
{
  return (((type() == Query::WantAuth) || (type() == Query::WantAuthBio)) && (count() == blockCount));
}

