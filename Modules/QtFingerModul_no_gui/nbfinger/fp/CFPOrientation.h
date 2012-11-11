//////////////////////////////////////////////////////////////////////////
//файл:         CFPOrientation.h
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Компонент, определяющий поле ориентаций по изображеннию ОП
//        		оператором Собеля      
//              
//////////////////////////////////////////////////////////////////////////
#ifndef CFPORIENTATION_H
#define CFPORIENTATION_H

class CFPOrientation
{
public:
	// Конструктор
	CFPOrientation();
	// Деструктор
	~CFPOrientation();



void getOrientation(int *Img, int nWt, int nHt, float *oImg);	// Расчет поля ориентаций изображения

protected:
int getSobelGradX(int *Reg); // Вычисление свертки с ядром Собеля по X
int getSobelGradY(int *Reg); // Вычисление свертки с ядром Собеля по Y
void LowPassFilterDirect(int sizeWND, float *oImg, int nWt, int nHt); // НЧ фильтрация поля ориентаций



private:

};
#endif