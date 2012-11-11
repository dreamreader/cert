
#ifndef CFUNCTIONAL_H
#define CFUNCTIONAL_H

#include "CFPMinutiae.h"


// Класс содержит функционалы для вычисления коэффициентов

class CFunctional
{

public:
	// Конструктор
	CFunctional();
	// Деструктор
	~CFunctional();
	
  float	otsGoriz( FPPoint *points, int nam, int sv, int sn );   // Горизонтальные отсчеты
  float	otsVertik( FPPoint *points, int nam, int sv, int sn );  // Вертикальные отсчеты
  float otsGoriz2( FPPoint *points, int nam, int sv, int sn ); // Один треугольник
  float otsVertik2( FPPoint *points, int nam, int sv, int sn ); // Один треугольник

  void DiskretCell (FPPoint *pt, int numberMinutiae, int xSizeCell, int ySizeCell, int nWt, int nHt, int *d);

/*void CFunctional::Interpolation(nbPointHw *Point, int NumberPoint, nbPointHw *IPoint); // Интерполяция функции
int CFunctional::SdvigVnoll(nbPointHw *Point, nbPointHw *IPoint); // Сдвиг функции в ноль*/

  //Масштабирование выборки до 512 элементов
  // pt       - [вх]  выборка из произвольного числа элементов
  // ptCount  - [вх]  число элементов в выборке
  // pt2      - [вых] результирующая выборка из 512 элементов
  // shift    - [вх] смещение точки в структуре pt и pt2
  void scaleFpTo512ShiftX(const float *pt,  int ptCount, int shift, int *pt2);
  
  // Преобразование Уолша-Адамара 
  // Возвращает 256 коэффициентов
  float* WelshAdamar( FPPoint *points, int nam);
 
  


};

#endif
