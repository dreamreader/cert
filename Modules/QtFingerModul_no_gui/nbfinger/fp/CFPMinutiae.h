//////////////////////////////////////////////////////////////////////////
//файл:         CFPMinutiae.h
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Компонент, определяющий минуции отпечатка пальца 
//              по бинаризованному утонченному изображению
//              
//////////////////////////////////////////////////////////////////////////

#include "CFPEnhance.h"  // getDirectPixel

#ifndef CFPMINUTIAE_H
#define CFPMINUTIAE_H


typedef struct tag_FPPoint{
	int type;		// Тип минуции
	int x,y;		// Кординаты минуции
	int direct;	// Направление минуции (в радианах)
} FPPoint;


class CFPMinutiae
{
	
	CFPEnhance EnhanceObject;

public:
	// Конструктор
	CFPMinutiae();
	// Деструктор
	~CFPMinutiae();


int getMinutiae(int *tImg, int *sImg, float *oImg, int nWt, int nHt, FPPoint *Minutiae);

protected:

void getMinutiaeCandidate(int *inImg, int *outImg, int nWt, int nHt);
int  getRightMinutiae(int *Img, float *oImg, int *sImg, FPPoint *Minutiae, int nWt, int nHt);
void getDirectMinutiae(int *Img, int nWt, int nHt, FPPoint *Minutiae, int nMinutiae);
void DFMRule05(int *inImg, int *outImg, int nWt, int nHt);
void DFMRule20(int *inImg, int *outImg, int nWt, int nHt);
void DFMRule10(int *inImg, int *outImg, float *oImg, int nWt, int nHt);
void DFMRule30(int *inImg, int *outImg, int nWt, int nHt);
int  PossibleReachPoint(int *Img, int nWt, int nHt, int X1, int Y1, int X2, int Y2, int Len);





public:

};

#endif  

