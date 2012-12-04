//Statistics
//Статистика обучения
#ifndef NBSTATISTICS_H_INCLUDED
#define NBSTATISTICS_H_INCLUDED

#include "../nb/nb.h"


class Statistics {
public:
  Statistics ();
  ~Statistics ();

  float &auto_e1 ();                //  Оценка вероятности ошибки первого рода
  float &auto_e2 ();                //  Оценка вероятности ошибки второго рода
  float &auto_e2d ();               //  Оценка вероятности ошибки второго рода
  float &tries ();
  float &successes ();
  Statistics &operator= (nbTestResult statistics);

private:
  float _e1;
  float _e2;
  float _e2d;
  float _tries;
  float _successes;
};


#endif
