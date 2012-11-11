
//////////////////////////////////////////////////////////////////////////
//файл:         CFPEnhance.h
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Компонент, выполняющий утончение линий в изображении до 1 пикселя
//        		      
//              
//////////////////////////////////////////////////////////////////////////
#ifndef _CFPTHINNING_H
#define _CFPTHINNING_H

#include "FPDefine.h"

#define SIZEMASK		9 		// Размер маски для операции свертки
#define SIZETRANSITION	8 		// Число переходов

class CFPThinning
{
public:
	// Конструктор
	CFPThinning();
	// Деструктор
	~CFPThinning();
	
  void thinning(int *Img, int *tImg, int nWt, int nHt);  	// Функция выполняет утоньчение линий до 1-го пиксела

protected:
  int convolutionAdd(int *r, const int *m);                 // Свертка по маске
  int transition01(int *r,  const int *t);                 // Определение числа переходов 0-1




private:

};

#endif
