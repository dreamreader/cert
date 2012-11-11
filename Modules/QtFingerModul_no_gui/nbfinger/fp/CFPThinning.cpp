
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
//#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "CFPThinning.h"

	// Маски свертки
const int	Mask1[SIZEMASK]={1,1,1,1,0,1,1,1,1};
const int	Mask2[SIZEMASK]={0,1,0,0,0,1,0,1,0};
const int	Mask3[SIZEMASK]={0,0,0,1,0,1,0,1,0};
const int	Mask4[SIZEMASK]={0,1,0,1,0,1,0,0,0};
const int	Mask5[SIZEMASK]={0,1,0,1,0,0,0,1,0};
const int	Mask6[SIZEMASK]={1,0,0,0,0,1,0,1,0};
const int	Mask7[SIZEMASK]={0,1,0,1,0,0,0,0,1};
const int	Mask8[SIZEMASK]={0,0,1,1,0,0,0,1,0};
const int	Mask9[SIZEMASK]={0,1,0,0,0,1,1,0,0};


	// Таблица переходов
const int 	Tabl[SIZETRANSITION]={0,1,2,5,8,7,6,3};

	// Конструктор
CFPThinning::CFPThinning(){
}
	// Деструктор
CFPThinning::~CFPThinning(){
}


// Алгоритм Зонга-Суня (утоньшение линий до 1-го пиксела) 
// Img [IN] - входное бинарное изображение
// tImg [OUT] - выходное утонченное бинарное изображение
// nWt [IN] - ширина изображения
// nHt [IN] - высота изображения
void CFPThinning::thinning(int *Img, int *tImg,  int nWt, int nHt)
{

int *Region;
Region=new int[SIZEMASK];


int nChange, Term;
bool NoChange;

	NoChange=false;
	while (!NoChange) 
	{
	
		nChange=0;

		// Подытерация "1"
		for(int i=1; i<nHt-1; i++)
		for(int j=1; j<nWt-1; j++){


			tImg[i*nWt+j]=Img[i*nWt+j];
			if(Img[i*nWt+j]==0) continue;

			// заполнение сегмента
				for(int r=-1; r<=1; r++)
					for(int c=-1; c<=1; c++)
						Region[(r+1)*3+(c+1)]=Img[(i+r)*nWt+(j+c)];
				
			

			// условие "1"
			Term=CFPThinning::convolutionAdd(Region,Mask1);	// свертка
			if((Term<3)||(Term>6)) continue;

			// условие "2"
			Term=CFPThinning::transition01(Region,Tabl); 
			if(Term!=1) continue;

			// условие "3"
			Term=CFPThinning::convolutionAdd(Region,Mask2);	
			if(Term==3) continue;

			// условие "4"
			Term=CFPThinning::convolutionAdd(Region,Mask3);	
			if(Term==3) continue;

			tImg[i*nWt+j]=0; // Удаляем пиксел
			nChange++;
		}
	

		
		// Подытерация "2"
		for(int i=1; i<nHt-1; i++)
		for(int j=1; j<nWt-1; j++){
		
			Img[i*nWt+j]=tImg[i*nWt+j];
			if(Img[i*nWt+j]==0) continue;
			
				// заполнение сегмента
					for(int r=-1; r<=1; r++)
						for(int c=-1; c<=1; c++)
							Region[(r+1)*3+(c+1)]=tImg[(i+r)*nWt+(j+c)];
					

				// условие "1"
				Term=CFPThinning::convolutionAdd(Region,Mask1); // свертка
				if((Term<3)||(Term>6)) continue; // 

				// условие "2"
				Term=CFPThinning::transition01(Region,Tabl); // определить число переходов
				if(Term!=1) continue;

				// условие "3"
				Term=CFPThinning::convolutionAdd(Region,Mask4); 
				if(Term==3) continue;

				// условие "4"
				Term=CFPThinning::convolutionAdd(Region,Mask5);
				if(Term==3) continue;

				Img[i*nWt+j]=0; // Удаляем пиксел
				nChange++;
			}
				
			if (nChange==0) NoChange=true; // Изменений нет
			
	} 

		for(int i=1; i<nHt-1; i++)
		for(int j=1; j<nWt-1; j++){
			if(Img[i*nWt+j]==0) continue;

				// заполняем регион	
					for(int r=-1; r<=1; r++){
						for(int c=-1; c<=1; c++){
								Region[(r+1)*3+(c+1)]=Img[(i+r)*nWt+(j+c)];}
						}

			// условие "1"
			Region[0]^=1;
			Term=CFPThinning::convolutionAdd(Region,Mask6); // свертка
			Region[0]^=1;
			if(Term==3){
				Img[i*nWt+j]=0; // Удаляем пиксел
					nChange++;
						continue;
			}

				// условие "2"
			Region[8]^=1;
			Term=CFPThinning::convolutionAdd(Region,Mask7);
			Region[8]^=1;
			if(Term==3){
				Img[i*nWt+j]=0; // Удаляем пиксел
					nChange++;
					continue;
				}

				// условие "3"
			Region[2]^=1;
			Term=CFPThinning::convolutionAdd(Region,Mask8);
			Region[2]^=1;
			if(Term==3) {
				Img[i*nWt+j]=0; // Удаляем пиксел
					nChange++;
					continue;
				}

				// условие "4"
			Region[6]^=1;
			Term=CFPThinning::convolutionAdd(Region,Mask9);
			Region[6]^=1;
			if(Term==3){
				Img[i*nWt+j]=0; // Удаляем пиксел
					nChange++;
					continue;
				}
			}	
			
		for(int i=0; i<nWt*nHt; i++)
			tImg[i]=Img[i];
			
			delete [] Region;

}


// Свертка по маске
// m[IN] - маска
// r[IN] - массив содержащий сегмент изображения
// Функция возвращает значение свертки

inline int CFPThinning::convolutionAdd(int *r, const int *m)
{

/*

		return	(r[0]*m[0])+(r[1]*m[1])+(r[2]*m[2])+\
			(r[3]*m[3])+(r[4]*m[4])+(r[5]*m[5])+\
			(r[6]*m[6])+(r[7]*m[7])+(r[8]*m[8]);
*/		

			int sum=0;
			for(int i=0; i<SIZEMASK; i++)
				sum+=r[i]*m[i];
			
			return sum;	
		
}

// Определение числа переходов 0-1 по таблице переходов
// r[IN] - массив содержащий сегмент изображения
// t[IN] - таблица переходов
// Функция возвращает  число переходов
inline int CFPThinning::transition01(int *r,  const int *t)
{
	int n=0;
/*	
	if(r[t[0]]<r[t[1]]) n++; 
	if(r[t[1]]<r[t[2]]) n++; 
	if(r[t[2]]<r[t[3]]) n++;
	if(r[t[3]]<r[t[4]]) n++; 
	if(r[t[4]]<r[t[5]]) n++; 
	if(r[t[5]]<r[t[6]]) n++;
	if(r[t[6]]<r[t[7]]) n++;
	if(r[t[7]]<r[t[0]]) n++;

	return n;
*/
	
	for(int i=0; i<SIZETRANSITION; i++)
		if (r[t[i]] < r[t[(i+1)%8]]) n++;
		
		return n;
	
}
