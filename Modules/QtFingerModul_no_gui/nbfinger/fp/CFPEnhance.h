//////////////////////////////////////////////////////////////////////////
//файл:         CFPEnhance.h
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Компонент, выполняющий обработку исходного изображения для
//      		улучшения его качества  		     
//              
//////////////////////////////////////////////////////////////////////////
#ifndef CFPENHANCE_H
#define CFPENHANCE_H

class CFPMinutiae;


#define CountPointVector	31 // число позиций векторов

typedef struct tag_RidgesVector{
	int x[CountPointVector];
	int y[CountPointVector];

} RidgesVector;

typedef struct tag_PositionVector{
	RidgesVector	xPosition;
	RidgesVector	yPosition;


} PositionVector;

class CFPEnhance
{
public:
	// Конструктор
	CFPEnhance();
	// Деструктор
	~CFPEnhance();

friend class CFPMinutiae;
void EnhanceMfs8600(int *Img, float *oImg, int *enhImg, int nWt, int nHt); // Улучшение качества изображения
void EnhanceGauss(int *Img, int *enhImg, int nWt, int nHt); // Фильтр Гаусса
void ExtractRidges(int *Img, float *oImg,int *bImg, int nWt, int nHt); // 	Извлечение гребней


protected:
float getDirectPixel( float *oImg, int RowPixel, int ColPixel, int nWt, int nHt); // Определение ориентации изображения в точке
int getSegmentPixel(int *sImg, int RowPixel, int ColPixel, int nWt, int nHt); // Определить находится ли папиллярный узор в точке
int getNumFromDirect(float Direct); // Определить классификатор направления
void Morf2(int *bImg, int nWt, int nHt); //	Фильтрация бинаризованного изображения
void GeneratorXYPosition(PositionVector *Position, int nPosition); // Генератор векторов

private:



};
#endif
