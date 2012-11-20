#include "signaturequery.h"

// Конструктор по умолчанию
SignatureQuery::SignatureQuery(): Query(Query::Ans)
{
}

// Создать запрос с заданными данными блоков
bool SignatureQuery::create(Nb::Data &signature)
{
  Log::Tab tab;
  Log::write("_signatureBlock");
  _sigBlock.Block::push(signature, true);

  this->push(&_sigBlock);
  return true;
}

// Получить данные блоков запроса
bool SignatureQuery::get(Nb::Data &signature)
{
  if (!isOk()) return false;

  DataBlock *sigBlock;
  sigBlock = this->at(0);

  int pos;
  pos = 0;
  sigBlock->Block::pop(pos, signature, true);
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool SignatureQuery::isOk()
{
  return ((type() == Query::Ans) && (count() == blockCount));
}
