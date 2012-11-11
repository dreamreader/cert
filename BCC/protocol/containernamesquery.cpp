#include "containernamesquery.h"

// Конструктор по умолчанию
ContainerNamesQuery::ContainerNamesQuery(): Query(Query::Ans)
{
}

// Создать запрос с заданными данными блоков
bool ContainerNamesQuery::create(QStringList &names)
{
  Log::Tab tab;
  Log::write("_namesBlock");
  _namesBlock.push((int32_t)names.size());
  int size(0);
  for (int i=0; i<names.size(); i++)
  {
    Nb::Data data;
    data.fromString(names[i], size, true);
    _namesBlock.push(data, true);
  }

  this->push(&_namesBlock);
  return true;
}

// Получить данные блоков запроса
bool ContainerNamesQuery::get(QStringList &names)
{
  if (!isOk()) return false;

  DataBlock *namesBlock;
  namesBlock = this->at(0);

  int pos(0);
  int32_t count(0);
  namesBlock->pop(pos, count);

  names.clear();
  for (int i=0; i<count; i++)
  {
    Nb::Data data;
    namesBlock->pop(pos, data, true);
    names.push_back(data.toString());
  }
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool ContainerNamesQuery::isOk()
{
  return ((type() == Query::Ans) && (count() == blockCount));
}


