#include "Statistics.h"

Statistics::Statistics ()
{
  _e1 = 0;
  _e2 = 0;
  _e2d = 0;
  _tries = 0;
  _successes = 0;
}

Statistics::~Statistics ()
{
}

float &Statistics::auto_e1 ()                //  Оценка вероятности ошибки первого рода
{
  return _e1;
}

float &Statistics::auto_e2 ()                //  Оценка вероятности ошибки второго рода
{
  return _e2;
}

float &Statistics::auto_e2d ()                //  Оценка вероятности ошибки второго рода защищенного контейнера
{
  return _e2d;
}

float &Statistics::tries ()                //  Количество попыток при ручном тестировании
{
  return _tries;
}

float &Statistics::successes ()              //  Количество успешных входов при ручном тестировании
{
  return _successes;
}

Statistics &Statistics::operator= (nbTestResult statistics)
{
  _e1 = statistics.e1Bio;
  _e2 = statistics.e2Bio;
  _e2d = statistics.e2Code;
  return *this;
}
