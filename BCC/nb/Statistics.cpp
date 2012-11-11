#include "Statistics.h"

Statistics::Statistics ()
{
	data.e1Bio = 0;
	data.e2Bio = 0;
	data.e2HumanCompromised = 0;
	data.e2BioCompromised = 0;
	data.e2Code = 0;
	data.e2BioWhiteNoise = 0;
	data.e2BioCorrNoise = 0;
	data.e2Conv = 0;
}
Statistics::~Statistics ()
{

}

double Statistics::e1 ()                //  Оценка вероятности ошибки первого рода
{
  return data.e1Bio;
}
double Statistics::e2 ()                //  Оценка вероятности ошибки второго рода
{
  return data.e2Bio;
}
double Statistics::e2d ()                //  Оценка вероятности ошибки второго рода защищенного контейнера
{
  return data.e2Code;
}

bool Statistics::setData (nbTestResult statistics)
{
	data = statistics;
	return true;
}
