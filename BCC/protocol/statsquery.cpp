#include "statsquery.h"

// Конструктор по умолчанию
StatsQuery::StatsQuery(): Query(Query::Stats)
{
}

// Создать запрос с заданными данными блоков
bool StatsQuery::create(nbTestResult &stats)
{
  LOG

  Query::create(Query::Stats);
  Log::write("_statsBlock");
  _statsBlock.push((uint8_t*)&stats, sizeof(nbTestResult));

  this->push(&_statsBlock);
  return true;
}

// Получить данные блоков запроса
bool StatsQuery::get(nbTestResult &stats)
{
  if (!isOk()) return false;

  DataBlock *statsBlock;
  statsBlock = this->at(0);

  int pos;
  pos = 0;
  statsBlock->pop(pos, (uint8_t*)&stats, sizeof(nbTestResult));
  return true;
}

// Проверить заполнение запроса (тип и число блоков)
bool StatsQuery::isOk()
{
  return ((type() == Query::Stats) && (count() == blockCount));
}

