#include "startsignaturequery.h"

// Конструктор по умолчанию
StartSignatureQuery::StartSignatureQuery(): Query(Query::WantSignature)
{
}

// Создать запрос с заданными данными блоков
bool StartSignatureQuery::create(QString &userId, QString &schemeId)
{
  Log::Tab tab;
  Log::write("_userIdBlock");
  _userIdBlock.Block::push(Nb::Data(userId), true);
  Log::write("_schemeIdBlock");
  _schemeIdBlock.Block::push(Nb::Data(schemeId), true);

  this->push(&_userIdBlock);
  this->push(&_schemeIdBlock);
  return true;
}

// Получить данные блоков запроса
bool StartSignatureQuery::get(QString &userId, QString &schemeId)
{
  if (!isOk()) return false;

  DataBlock *userIdBlock, *schemeIdBlock;
  userIdBlock = this->at(0);
  schemeIdBlock = this->at(1);

  int pos;
  pos = 0;
  Nb::Data userData;
  userIdBlock->Block::pop(pos, userData, true);
  userId = userData.toString();

  pos = 0;
  Nb::Data schemeData;
  schemeIdBlock->Block::pop(pos,schemeData, true);
  schemeId = schemeData.toString();

  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool StartSignatureQuery::isOk()
{
  return ((type() == Query::WantSignature) && (count() == blockCount));
}
