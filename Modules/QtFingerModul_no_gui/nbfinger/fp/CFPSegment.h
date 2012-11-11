//////////////////////////////////////////////////////////////////////////
//файл:         CFPSegment.h
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Компонент, определяющий сегмент приложенного ОП     		   
//              
//////////////////////////////////////////////////////////////////////////
#ifndef CFPSEGMENT_H
#define CFPSEGMENT_H

class CFPSegment
{
public:
	// Конструктор
	CFPSegment();
	// Деструктор
	~CFPSegment();
	
float getSegment(int *Img, int *sImg, int nWt, int nHt); // Определение сегмента приложенного пальца

protected:
void Morphology(int *sImg, int nWt, int nHt); // Фильтрация сегмента

private:

};

#endif