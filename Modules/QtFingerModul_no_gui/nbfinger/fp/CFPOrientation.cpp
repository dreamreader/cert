//////////////////////////////////////////////////////////////////////////
//файл:         CFPOrientation.cpp
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Компонент, определяющий поле ориентаций по изображеннию ОП
//        		оператором Собеля      
//              
//////////////////////////////////////////////////////////////////////////

#include "FPDefine.h"
#include "CFPOrientation.h"
#include <math.h>
#include <memory.h>

#define SizeMaskSobel 9 // размер маски

const  int SobelMaskX[SizeMaskSobel]={-1,0,1,-2,0,2,-1,0,1}; // ядро свертки 
const  int SobelMaskY[SizeMaskSobel]={-1,-2,-1,0,0,0,1,2,1};

//Конструктор
CFPOrientation::CFPOrientation(){

}

//Деструктор
CFPOrientation::~CFPOrientation(){

}

// Вычисление свертки с ядром Собеля по X
inline int CFPOrientation::getSobelGradX(int *Reg)
{

	return	(int)(Reg[0]*SobelMaskX[0])+\
				 (Reg[1]*SobelMaskX[1])+\
				 (Reg[2]*SobelMaskX[2])+\
				 (Reg[3]*SobelMaskX[3])+\
				 (Reg[4]*SobelMaskX[4])+\
				 (Reg[5]*SobelMaskX[5])+\
				 (Reg[6]*SobelMaskX[6])+\
				 (Reg[7]*SobelMaskX[7])+\
				 (Reg[8]*SobelMaskX[8]);
				 
		/*
				int Sum=0;
				 for (int i=0; i<SizeMaskSobel; i++)
					Sum+=Reg[i]*SobelMaskX[i];
					
					
					return Sum;
				
	*/
}


 // Вычисление свертки с ядром Собеля по Y
inline int CFPOrientation::getSobelGradY(int *Reg)
{

	return	(int)(Reg[0]*SobelMaskY[0])+\
				 (Reg[1]*SobelMaskY[1])+\
				 (Reg[2]*SobelMaskY[2])+\
				 (Reg[3]*SobelMaskY[3])+\
				 (Reg[4]*SobelMaskY[4])+\
				 (Reg[5]*SobelMaskY[5])+\
				 (Reg[6]*SobelMaskY[6])+\
				 (Reg[7]*SobelMaskY[7])+\
	     		 (Reg[8]*SobelMaskY[8]);
			 
/*
				int Sum=0;
				 for (int i=0; i<SizeMaskSobel; i++)
					Sum+=Reg[i]*SobelMaskY[i];
					return Sum;
					*/
					
					
}

// НЧ фильтрация поля ориентации
// *oImg [IN] - входное изображение
// sizeWND [IN] - глубина фильтра (нечетное число, больше 1. Если число 
// не удовлетворяет, то выполняется фильтрация со значением sizeWND=3)
// nWt [IN] -
// nHt [IN] - размеры изображения
// oImg [OUT] - поле ориентаций изображения. 
// 
void CFPOrientation::LowPassFilterDirect(int sizeWND, float *oImg, int nWt, int nHt)

{

	float Fx, Fy;
	int Direct;	
	int xBLK=int(nWt/fpBlkNum16); // число блоков по X
	int yBLK=int(nHt/fpBlkNum16); // число блоков по Y
	float *LowDirect=new float[xBLK*yBLK];
	

	if((sizeWND<=1)||(sizeWND%2==0)) sizeWND=3; // по умолчанию

	for(int i=0; i<yBLK; i++)
	for(int j=0; j<xBLK; j++)
	{
		Fx=Fy=0;
		for(int u=sizeWND/-2; u<=sizeWND/2; u++)
		for(int v=sizeWND/-2; v<=sizeWND/2; v++)
		{
			if((i+u<0)||(i+u>=yBLK)||(j+v<0)||(j+v>=xBLK)) continue;

			Fx+=cos(2*0.01745*(oImg[(i+u)*xBLK+(j+v)]));
			Fy+=sin(2*0.01745*(oImg[(i+u)*xBLK+(j+v)]));
		}

		//Direct=int(57.32484*((atan2(Fy,Fx)/2)));
		Direct=int(((atan2(Fy,Fx)/2))/0.01745);
		if(Direct<0) Direct+=180;

		LowDirect[i*xBLK+j]=Direct;
	}

	for(int i=0; i<xBLK*yBLK; i++)
			oImg[i]=LowDirect[i];

	delete [] LowDirect;

}
// Расчет поля ориентаций изображения
// *Img [IN] - входное изображение
// nWt [IN] -
// nHt [IN] - размеры изображения
// oImg [OUT] - поле ориентаций изображения
void CFPOrientation::getOrientation(int *Img, int nWt, int nHt, float *oImg)
{

	int gradX, gradY;
	int *Region=new int [SizeMaskSobel];  
	double Gxy, Gxx, Gyy;
	float Direct;

	memset(Region,0,SizeMaskSobel*sizeof(int));
	int xBLK=int(nWt/fpBlkNum16); // число блоков по X
	int yBLK=int(nHt/fpBlkNum16); // число блоков по Y

		

	// Вычисление градиента пиксела в блоках
	for(int i=0; i<yBLK; i++)
	for(int j=0; j<xBLK; j++)
	{

		Gxy=Gxx=Gyy=0;
		
		for(int u=0, Y=i*fpBlkNum16; u<fpBlkNum16; u++, Y++)
		for(int v=0, X=j*fpBlkNum16; v<fpBlkNum16; v++, X++)
		{
			// поиск градиента пиксела
				if((X!=0)&&(Y!=0)&&(X<nWt-1)&&(Y<nHt-1))
				{
					for (int ii=-1; ii<=1; ii++)
						for (int jj=-1; jj<=1; jj++){
							Region[(ii+1)*3+(jj+1)]=Img[(Y+ii)*320+(X+jj)];}


					// Определяем значение градиента по X,Y
					gradX=getSobelGradX(Region);
					gradY=getSobelGradY(Region);
				} 	
				else
				{
					gradX=0; // Значения на границе
					gradY=0;
				}
			

			Gxy+=gradX*gradY;
			Gxx+=gradX*gradX;
			Gyy+=gradY*gradY;

			

		}
		//Direct=int(57.32484*((atan2(2*Gxy,Gxx-Gyy))/2));
		Direct=int(((atan2(2*Gxy,Gxx-Gyy))/2)/0.01745);

		

		// Перевод в интервал от 0 до 180 градусов
		// Полученное направление ортогонально действительному, -> +90
		if(Direct<0) Direct+=180;

		oImg[i*xBLK+j]=Direct;

	}
		
	delete [] Region;

		
	CFPOrientation::LowPassFilterDirect(3, oImg,  nWt,  nHt); // НЧ фильтр
}



