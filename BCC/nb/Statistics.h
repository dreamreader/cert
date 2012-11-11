//Statistics
//Статистика обучения
#ifndef NBSTATISTICS_H_INCLUDED
#define NBSTATISTICS_H_INCLUDED

#include "nb.h"
#include <math.h>

class Statistics {
public:
  Statistics ();
  ~Statistics ();

  double e1 ();                //  Оценка вероятности ошибки первого рода
  double e2 ();                //  Оценка вероятности ошибки второго рода
  double e2d ();               //  Оценка вероятности ошибки второго рода защищённого контейнера
  bool setData (nbTestResult statistics);

  operator nbTestResult&() {return data;}

private:
  nbTestResult data;
};


#endif
