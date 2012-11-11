//////////////////////////////////////////////////////////////////////////
//файл:         CFPEnhance.h
//верси€:
//дата:
//изменен:
//автор:
//
//описание:      омпонент, выполн€ющий обработку исходного изображени€ дл€
//      		улучшени€ его качества  		     
//              
//////////////////////////////////////////////////////////////////////////
#include "CFPEnhance.h"
#include "FPDefine.h"
#include <math.h>
#include <memory.h>

// онструктор
CFPEnhance::CFPEnhance(){

}

//ƒеструктор
CFPEnhance::~CFPEnhance(){

}

//	”лучшение качества изображени€
// 	Img [IN] - исходное изображение исходного качества 
//	oImg [IN] - поле оринетации 
//  EnhImg [OUT] - изображение улучшенного качества
//	nWt, nHt [IN] - параметры изображени€

void CFPEnhance::EnhanceMfs8600(int *Img, float *oImg, int *enhImg, int nWt, int nHt)
{

  float sum1=0.0f;
	int direct;

	for(int i=0; i<nHt; i++)
	for(int j=0; j<nWt; j++)
	{
	
		if((i<2)||(j<2)||((int)i+2>=nWt)||((int)j+2>=nHt))
		{
		
			enhImg[i*nWt+j]=Img[i*nWt+j];
			continue;
		}
		direct=(int)getDirectPixel(oImg,i,j,nWt,nHt);
		direct=CFPEnhance::getNumFromDirect(direct);
		
		switch(direct)
		{
		case 5:	
			sum1 =Img[(i  )*nWt+(j  )];
			sum1+=Img[(i  )*nWt+(j-1)];
			sum1+=Img[(i  )*nWt+(j-2)];
			sum1+=Img[(i  )*nWt+(j+1)];
			sum1+=Img[(i  )*nWt+(j+2)];
			break;
		case 4:	
			sum1 =Img[(i  )*nWt+(j  )];
			sum1+=Img[(i+1)*nWt+(j-1)];
			sum1+=Img[(i+1)*nWt+(j-2)];
			sum1+=Img[(i-1)*nWt+(j+1)];
			sum1+=Img[(i-1)*nWt+(j+2)];
			break;
		case 3:	
     		sum1 =Img[(i  )*nWt+(j  )];
			sum1+=Img[(i+1)*nWt+(j-1)];
			sum1+=Img[(i+2)*nWt+(j-2)];
			sum1+=Img[(i-1)*nWt+(j+1)];
			sum1+=Img[(i-2)*nWt+(j+2)];
			break;
		case 2:	
			sum1 =Img[(i  )*nWt+(j  )];
			sum1+=Img[(i+1)*nWt+(j-1)];
			sum1+=Img[(i+2)*nWt+(j-1)];
			sum1+=Img[(i-1)*nWt+(j+1)];
			sum1+=Img[(i-2)*nWt+(j+1)];
			break;
		case 1:	
			sum1 =Img[(i  )*nWt+(j  )];
			sum1+=Img[(i+1)*nWt+(j  )];
			sum1+=Img[(i+2)*nWt+(j  )];
			sum1+=Img[(i-1)*nWt+(j  )];
			sum1+=Img[(i-2)*nWt+(j  )];
			break;
		case 8:	
			sum1 =Img[(i  )*nWt+(j  )];
			sum1+=Img[(i-1)*nWt+(j-1)];
			sum1+=Img[(i-2)*nWt+(j-1)];
			sum1+=Img[(i+1)*nWt+(j+1)];
			sum1+=Img[(i+2)*nWt+(j+1)];
			break;
		case 7:	
			sum1 =Img[(i  )*nWt+(j  )];
			sum1+=Img[(i-1)*nWt+(j-1)];
			sum1+=Img[(i-2)*nWt+(j-2)];
			sum1+=Img[(i+1)*nWt+(j+1)];
			sum1+=Img[(i+2)*nWt+(j+2)];
			break;
		case 6:	
			sum1 =Img[(i  )*nWt+(j  )];
			sum1+=Img[(i-1)*nWt+(j-1)];
			sum1+=Img[(i-1)*nWt+(j-2)];
			sum1+=Img[(i+1)*nWt+(j+1)];
			sum1+=Img[(i+1)*nWt+(j+2)];
			break;
			
		default:
				break;
		}
		
		enhImg[i*nWt+j]=(int)sum1/5;
	}
}

// ‘ильтр √аусса
// 	Img [IN] - исходное изображение исходного качества 
//  enhImg [OUT] - обработанное изображение
//	nWt, nHt [IN] - параметры изображени€

void CFPEnhance::EnhanceGauss(int *Img, int *enhImg, int nWt, int nHt)
{

	float sum1;
	float sum;

	for(int i=0; i<nHt; i++)
	for(int j=0; j<nWt; j++)
	{

		if((i==0)||(j==0)||(i+1==nWt)||(j+1==nHt))
		{
			enhImg[i*nWt+j]=Img[i*nWt+j];
			continue;
		}

		sum1 =2*Img[(i  )*nWt+(j  )];
		sum1+=3*Img[(i-1)*nWt+(j-1)];
		sum1+=3*Img[(i-1)*nWt+(j  )];
		sum1+=3*Img[(i-1)*nWt+(j+1)];
		sum1+=3*Img[(i  )*nWt+(j+1)];
		sum1+=3*Img[(i+1)*nWt+(j+1)];
		sum1+=3*Img[(i+1)*nWt+(j  )];
		sum1+=3*Img[(i+1)*nWt+(j-1)];
		sum1+=3*Img[(i  )*nWt+(j-1)];
		sum=sum1/26;
		
		enhImg[i*nWt+j]=(int)sum;
		
	}



}

// ќпределение ориентации изображени€ в точке
// oImg [IN] - поле оринетации 
// RowPixel [IN] -  позици€ пиксел€ по Y
// ColPixel [IN] - позици€ пиксел€ по X
//	nWt, nHt [IN] - параметры изображени€
// ‘ункци€ возвращает значение ориентации в точке с координатами RowPixel, ColPixel

float CFPEnhance::getDirectPixel( float *oImg, int RowPixel, int ColPixel, int nWt, int nHt)
{
	
	int xBLK=int(nWt/fpBlkNum16);
	
	if((RowPixel<0)||(ColPixel<0)||(ColPixel>=nWt)||(RowPixel>=nHt))
		return 0;
	else
		return oImg[(RowPixel/fpBlkNum16)*xBLK+(ColPixel/fpBlkNum16)];
}

// ќпределить находитс€ ли папилл€рный узор в точке
// sImg [IN] - поле оринетации 
// RowPixel [IN] -  позици€ пиксел€ по Y
// ColPixel [IN] - позици€ пиксел€ по X
//	nWt, nHt [IN] - параметры изображени€
// ‘ункци€ возвращает значение 1 - если папилл€рный узор 
// присутствует в точке с координатами RowPixel, ColPixel
// 0 - в противоположном случае

int CFPEnhance::getSegmentPixel(int *sImg, int RowPixel, int ColPixel, int nWt, int nHt)
{
	int xBLK=int(nWt/fpBlkNum16);

	return sImg[(RowPixel/fpBlkNum16)*xBLK+(ColPixel/fpBlkNum16)];
}

// ќпределить классификатор направлени€
// Direct  - значение направлени€
// ‘ункци€ возвращает классификатор

int CFPEnhance::getNumFromDirect(float Direct)
{
	if ((Direct<=11.25)	||	(Direct>168.75))		return 1;
	if ((Direct>11.25)	&&	(Direct<=33.75))		return 2;
	if ((Direct>33.75)	&&	(Direct<=56.25))		return 3;
	if ((Direct>56.25)	&&	(Direct<=78.75))		return 4;
	if ((Direct>78.75)	&&	(Direct<=101.25))		return 5;
	if ((Direct>101.25)	&&	(Direct<=123.75))		return 6;
	if ((Direct>123.75)	&&	(Direct<=146.25))		return 7;
	if ((Direct>146.25)	&&	(Direct<=168.75))		return 8;

	return 0;
}


// √енератор векторов
// Position [IN/OUT]- массив позиций векторов
// nPosition [IN] - число векторов

void CFPEnhance::GeneratorXYPosition(PositionVector *Position, int nPosition)
{
	int n,i, x, y;
	double Direct;



	for(n=0; n<nPosition; n++)
	{
		
		Direct=n*22.5;
		Direct=0.01745*Direct; // перевод в радианы

		for(i=0; i<CountPointVector; i++)
		{
			x=(double)((int)i-CountPointVector/2)*cos(Direct);
			y=(double)((int)i-CountPointVector/2)*sin(Direct);
			Position[n].xPosition.x[i]=x;
			Position[n].xPosition.y[i]=y;
			
		}

		Direct=(n+4)*22.5;
		Direct=0.01745*Direct; // перевод в радианы

		for(i=0; i<CountPointVector; i++)
		{
			x=(double)(i-CountPointVector/2)*cos(Direct);
			y=(double)(i-CountPointVector/2)*sin(Direct);

			Position[n].yPosition.x[i]=x;
			Position[n].yPosition.y[i]=y;
		}

	}
	
}

// 	»звлечение гребней
// 	Img [IN] - исходное изображение 
//	oImg [IN] - поле оринетации 
//  bImg [OUT] - бинаризованное изображение, со структурой гребней.
//	nWt, nHt [IN] - параметры изображени€

void CFPEnhance::ExtractRidges(int *Img, float *oImg, int *bImg, int nWt, int nHt)
{

	int X1,Y1,X2,Y2,N1,N2;
	double Direct;
	int NumDirect;
	int Sum1, Sum2;
	int nPosition=8;
	PositionVector *Position;
	Position=new PositionVector[nPosition];
	memset(Position,0, nPosition*sizeof(PositionVector));


	CFPEnhance::GeneratorXYPosition(Position,nPosition);

	for(int i=0; i<nHt; i++)
	for(int j=0; j<nWt; j++)
	{

		Direct=CFPEnhance::getDirectPixel(oImg,i,j,nWt,nHt);
		NumDirect=CFPEnhance::getNumFromDirect(Direct);
			
		Sum1=Sum2=0;
		N1=N2=0;
		for(int n=9; n<21; n++)	
		{
			X1=j+Position[NumDirect-1].xPosition.x[n];
			Y1=i+Position[NumDirect-1].xPosition.y[n];
			X2=j+Position[NumDirect-1].yPosition.x[n];
			Y2=i+Position[NumDirect-1].yPosition.y[n];
			
			if((X1>=0)||(X1<nWt)||(Y1>=0)||(Y1<nHt)) 
			{
				 if ((Y1*nWt+X1>0) && (Y1*nWt+X1<(nWt*nHt-1))) 
					Sum1+=Img[Y1*nWt+X1];
				
			}
			if((X2>=0)||(X2<nWt)||(Y2>=0)||(Y2<nHt)) 
			{
				
				if ((Y2*nWt+X2>0) && (Y2*nWt+X2<(nWt*nHt-1))) 
					Sum2+=Img[Y2*nWt+X2];

			}
		}

		if(Sum2+40<Sum1) bImg[i*nWt+j]=1; 
		
		else 
			bImg[i*nWt+j]=0;
	}

	CFPEnhance::Morf2(bImg, nWt, nHt);

	delete [] Position;
	
}
//	‘ильтраци€ бинаризованного изображени€
// 	bImg [IN/OUT] - бинаризованное изображение
//	nWt, nHt [IN] - параметры изображени€
void CFPEnhance::Morf2(int *bImg, int nWt, int nHt)
{
	int N;
	int Sum;
	
	do
	{
		N=0;
		for(int i=0; i<nHt; i++)
		for(int j=0; j<nWt; j++)
		{
			
			if((bImg[i*nWt+j]==1)||(i==0)||(i==nHt-1)||(nWt==0)||(nWt==nWt-1)) 
			{
				bImg[i*nWt+j]=1;
				continue;
			}
		
			Sum=bImg[(i-1)*nWt+j];
			Sum+=bImg[(i+1)*nWt+j];
			Sum+=bImg[(i)*nWt+j-1];
			Sum+=bImg[(i)*nWt+j+1];

			if(Sum>=3) 
			{
				bImg[i*nWt+j]=1;
				N++;
			}
			else
			{
				bImg[i*nWt+j]=0;
			}
		}
	} while(N!=0);
}

