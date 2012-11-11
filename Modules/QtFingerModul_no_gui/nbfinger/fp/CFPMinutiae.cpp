//////////////////////////////////////////////////////////////////////////
//файл:         CFPMinutiae.cpp
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Компонент, определяющий минуций отпечатка пальца 
//              по бинаризованному утонченному изображению
//              
//////////////////////////////////////////////////////////////////////////

#include "CFPMinutiae.h"
#include <math.h>
#include <memory.h>


#define ThresholdMinutiae	3	// максимальное минуций в окрестности
#define SizeMove 			8   // число переходов
#define SizeDelta			25	// максимальный радиус поиска

const int moveX[SizeMove]={-1, 0, 1, 1, 1, 0,-1,-1};
const int moveY[SizeMove]={-1,-1,-1, 0, 1, 1, 1, 0};				 
const int PosBak[SizeMove]={4,5,6,7,0,1,2,3};


CFPMinutiae::CFPMinutiae(){
}
	// Деструктор
CFPMinutiae::~CFPMinutiae(){
}

//	Поиск минуций в бинаризованном утонченном изображении
//  tImg [IN] - бинаризованное утонченное изображение
//  enhImg [IN] - изображение улучшенного качества
// 	sImg [IN] - изображение сегмента ОП
//	oImg [IN] - поле оринетации 
//  Minutiae [OUT] - массив минуций ОП
//	nWt, nHt [IN] - параметры изображения
//  Функция возвращает число найденных минуций

int CFPMinutiae::getMinutiae(int *tImg, int *sImg, float *oImg, int nWt, int nHt, FPPoint *Minutiae){


	int nMinutiae;
	int *typeImg=new int [nWt*nHt];
	memset (typeImg,0,nWt*nHt*sizeof(int));

			CFPMinutiae::getMinutiaeCandidate( tImg, typeImg, nWt, nHt );

			//CFPMinutiae::DFMRule05( enhImg, tImg, nWt, nHt );
			memset (tImg,0,nWt*nHt*sizeof(int));

			CFPMinutiae::DFMRule05( typeImg, tImg, nWt, nHt );
			memset (typeImg,0,nWt*nHt*sizeof(int));

			CFPMinutiae::DFMRule20( tImg, typeImg, nWt, nHt ); 
			memset (tImg,0,nWt*nHt*sizeof(int));

			//CFPMinutiae::DFMRule10( tImg, enhImg, oImg, nWt, nHt );

			CFPMinutiae::DFMRule10( typeImg, tImg, oImg, nWt, nHt ); // Cоединить гребни
			memset (typeImg,0,nWt*nHt*sizeof(int));

			//CFPMinutiae::DFMRule30( enhImg, tImg, nWt, nHt );

			CFPMinutiae::DFMRule30( tImg, typeImg, nWt, nHt );
			
				for(int i=0; i<nHt; i++)
				for(int j=0; j<nWt; j++){
					if((i>6)&&(i<nHt-6)&&(j>6)&&(j<nWt-6)) continue;
						typeImg[i*nWt+j]=0;}

				


	// финиш
	nMinutiae=CFPMinutiae::getRightMinutiae(typeImg, oImg, sImg, Minutiae, nWt, nHt);
	CFPMinutiae::getDirectMinutiae( typeImg, nWt, nHt, Minutiae, nMinutiae );
	
	delete [] typeImg;	
	return nMinutiae;

}

// Поиск предполагаемых минуций
//  inImg [IN] - входное бинаризованное утонченное изображение
// 	outImg [OUT] - выходной массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
//	nWt, nHt [IN] - параметры изображения
void CFPMinutiae::getMinutiaeCandidate(int *inImg, int *outImg, int nWt, int nHt){

	int sum1,n1,n2,n3,n4,n5,n6,n7,n8;
	int *Region;
	Region=new int [9];

	for(int i=1; i<nHt-1; i++)
	for(int j=1; j<nWt-1; j++)
	{
	
		if(inImg[i*nWt+j]==0) {
			outImg[i*nWt+j]=0;
			continue;}
		
		outImg[i*nWt+j]=1;
		
		// Определение типа минуции
		for(int r=-1; r<=1; r++)
			for(int c=-1; c<=1; c++)
				Region[(r+1)*3+(c+1)]=inImg[(i+r)*nWt+(j+c)];
			
		n1=Region[0];
		n2=Region[1];
		n3=Region[2];
		n4=Region[5];
		n5=Region[8];
		n6=Region[7];
		n7=Region[6];
		n8=Region[3];
		
		
		sum1=	(8*n1-(n1*n8))+\
				(8*n2-(n2*n1))+\
				(8*n3-(n3*n2))+\
				(8*n4-(n4*n3))+\
				(8*n5-(n5*n4))+\
				(8*n6-(n6*n5))+\
				(8*n7-(n7*n6))+\
				(8*n8-(n8*n7));


		if((sum1==16)||(sum1==0)) continue;

		if(sum1==15) sum1=8; // Исключение
				
		if (sum1==8) outImg[i*nWt+j]=11; // Минуция - окончание
				
		if (sum1==24) outImg[i*nWt+j]=21; // Минуция - разветвление		
	
		}

	delete [] Region;
	}	

	

// Определение правильных минуций
//  inImg [IN] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
// 	sImg [IN] - изображение сегмента ОП
//	oImg [IN] - поле оринетации 
//  Minutiae [OUT] - массив минуций ОП
//	nWt, nHt [IN] - параметры изображения
//  Функция возвращает число найденных минуций

int CFPMinutiae::getRightMinutiae(int *Img, float *oImg, int *sImg, FPPoint *Minutiae, int nWt, int nHt){

	
	int nMinutiae;
	int n=0;

	for(int i=0; i<nHt; i++)
	for(int j=0; j<nWt; j++)
	{
		if(Img[i*nWt+j]<10) continue; // точка - не минуция
		if (EnhanceObject.getSegmentPixel(sImg,i,j, nWt, nHt) == 0) continue; // точка находится за пределами сегмента ОП

		Minutiae[n].x=j;
		Minutiae[n].y=i;
		Minutiae[n].type=int(Img[i*nWt+j]/10);
		Minutiae[n].direct=(int)EnhanceObject.getDirectPixel(oImg, i, j, nWt, nHt);
		n++;
	}

	nMinutiae=n;
	return nMinutiae;

}

// Определение направления минуций по изображению
//  inImg [IN] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
//  Minutiae [OUT] - массив минуций ОП
//	nMinutiae - число минуций
//	nWt, nHt [IN] - параметры изображения
void CFPMinutiae::getDirectMinutiae(int *Img, int nWt, int nHt, FPPoint *Minutiae, int nMinutiae){

  int n;
	int X,Y,Sum,Val;
	int SumX, SumY;
	int Q1,Q2;

	for(n=0; n<nMinutiae; n++)
	{
		Q1=Minutiae[n].direct+90;
		X=Minutiae[n].x;
		Y=Minutiae[n].y;
		Val=10;
		SumX=SumY=0;
		Img[Y*nWt+X]=Val;
		

		do{
			Sum=0;
		
			for(int i=-7; i<=7; i++)
			for(int j=-7; j<=7; j++)		
			{
				if((i+Y<0)||(i+Y>=nHt)||(j+X<0)||(j+X>=nWt)) continue;
				
					if ((((i+Y)*(i+Y)+(j+X)*(j+X))/(10*10))<1)	continue;	// по желанию можно закоментировать (- область окружности)
				
						if(Img[(Y+i)*nWt+(X+j)]!=Val) continue;
			
				for(int ii=i+Y-1; ii<=i+Y+1; ii++)
				for(int jj=j+X-1; jj<=j+X+1; jj++)
				{
					if((ii<0)||(ii>=nHt)||(jj<0)||(jj>=nWt)) continue;
					
					if(Img[ii*nWt+jj]!=1) continue;
					
					Img[ii*nWt+jj]=Img[ii*nWt+jj]*(Val+1);

					SumX+=(jj-X);
					SumY+=(ii-Y);
					Sum++;
				}
			}
			Val++;


		}while(Sum!=0);

		
			for(int i=-11; i<=11; i++)
			for(int j=-11; j<=11; j++)		
			{
				if((i+Y<0)||(i+Y>=nHt)||(j+X<0)||(j+X>=nWt)) continue;
		
				if(Img [(Y+i)*nWt+(X+j)]==0) continue;
				Img[(Y+i)*nWt+(X+j)]=1;
		}


		Q2=int((atan2((double)SumY,(double)SumX))/0.01745);
		
		if(Q2<0) Q2+=360;
      if(fabs((float)(Q1-Q2))>90)
				Q2=Q1+180;
				
					else
						Q2=Q1;

		Minutiae[n].direct=Q2;

	}
}



// Удаление минуций  в радиусе 15 пикселей, 
//	если их число превышает порог ThresholdMinutiae
//  inImg [IN] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
// 	outImg [OUT] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
//	nWt, nHt [IN] - параметры изображения

void CFPMinutiae::DFMRule05(int *inImg, int *outImg, int nWt, int nHt) {
	int mSum;
	int Radius=15; // радиус поиска

	for(int i=1; i<nHt-1; i++)
	for(int j=1; j<nWt-1; j++)
	{
			
		if(inImg[i*nWt+j]<10) 
		{
			// не является минуцией
			outImg[i*nWt+j]=inImg[i*nWt+j];
			continue;
		}


		mSum=0;
		for(int ii=-1*Radius; ii<Radius; ii++)
		for(int jj=-1*Radius; jj<Radius; jj++)
		{
			if((ii==0)&&(jj==0)) continue;
				if((i+ii<0)||(i+ii>=nHt)||(j+jj<0)||(j+jj>=nWt)) continue;	// находится за пределами из-я
					if((((jj*jj)+(ii*ii))/(Radius*Radius))>1)	continue;	// по желанию можно закоментировать (- область окружности)	
						if(inImg[(i+ii)*nWt+(j+jj)]>10) 
								mSum++;

		}
			

		if(mSum<=ThresholdMinutiae) outImg[i*nWt+j]=inImg[i*nWt+j]; 
			else outImg[i*nWt+j]=1;

	}
}

//	Удаление коротких гребней
//  inImg [IN] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
// 	outImg [OUT] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
//	nWt, nHt [IN] - параметры изображения
void CFPMinutiae::DFMRule20(int *inImg, int *outImg, int nWt, int nHt)
{
	int Freq;
	int Direct1, X1,Y1, dx1, dy1;
	int Direct2, X2,Y2, dx2, dy2;

	for(int i=1; i<nHt-1; i++)
	for(int j=1; j<nWt-1; j++)
	{
		if(inImg[i*nWt+j]!=11) continue;
		
		X1=X2=j;
		Y1=Y2=i;
		Direct1=Direct2=-1;
		Freq=10;

		for(int n=0; n<Freq; n++) //длина гребня
		{
			//обход по часовой стрелки
			for(int m=0; m<SizeMove; m++)
			{
				Direct1++;
				Direct1%=SizeMove;

				dx1=moveX[Direct1];
				dy1=moveY[Direct1];

						
				if(inImg[(Y1+dy1)*nWt+(X1+dx1)]>0)	
				{
					X1+=dx1;
					Y1+=dy1;
					break;
				}
			}

			//обход против часовой стрелки
			for(int m=0; m<SizeMove; m++)
			{
				Direct2++;
				Direct2%=SizeMove;

				dx2=moveX[(SizeMove-Direct2)%SizeMove];
				dy2=moveY[(SizeMove-Direct2)%SizeMove];

				if(inImg[(Y2+dy2)*nWt+(X2+dx2)]>0)	
				{
					X2+=dx2;
					Y2+=dy2;
					break;
				}
			}
	
			// гребень короткий, удаляем (найденный обходом по часовой стрелки)

			if(inImg[Y1*nWt+X1]>10)
			{
				inImg[i*nWt+j]=1;
				inImg[Y1*nWt+X1]=1;
				break;
			}

			// гребень короткий, удаляем (найденный обходом против часовой стрелки)
			
			if(inImg[Y2*nWt+X2]>10)
			{
				inImg[i*nWt+j]=1;
				inImg[Y2*nWt+X2]=1;
				break;
			}

			Direct1=PosBak[Direct1];
			Direct2=PosBak[Direct2];
		}
	}
	
	for(int i=0; i<nWt*nHt; i++)
		outImg[i]=inImg[i];
		
}



//	Удаление разрывов гребней
//  inImg [IN] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
// 	outImg [OUT] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
//	oImg [IN] - поле оринетации 
//	nWt, nHt [IN] - параметры изображения
void CFPMinutiae::DFMRule10(int *inImg, int *outImg, float *oImg, int nWt, int nHt)
{

	int flag;
	int Direct1, Direct2, Direct3;
	int Radius=10;

	for(int i=1; i<nHt-1; i++)
	for(int j=1; j<nWt-1; j++)
	{

		outImg[i*nWt+j]=inImg[i*nWt+j];
		if(outImg[i*nWt+j]!=11) continue;

		Direct1=(int)EnhanceObject.getDirectPixel(oImg, i, j, nWt, nHt);
		Direct1=EnhanceObject.getNumFromDirect(Direct1);
		
		switch(Direct1)
		{
		case 1:	Direct1=5;
				break;
		case 2:	Direct1=6;
				break;
		case 3:	Direct1=7;
				break;
		case 4:	Direct1=8;
				break;
		case 5:	Direct1=1;
				break;
		case 6:	Direct1=2;
				break;
		case 7:	Direct1=3;
				break;
		case 8:	Direct1=4;
				break;
		default:
				break;
		}

		flag=0;
		for(int n=-1*Radius; n<Radius; n++){
		for(int m=-1*Radius; m<Radius; m++){
		
				if((n==0)&&(m==0)) continue;
					if((i+n<0)||(i+n>=nHt)||(j+m<0)||(j+m>=nWt)) continue;
						if((((m*m)+(n*n))/(Radius*Radius))>1)	continue;	// по желанию можно закоментировать (- область окружности)		
							if(inImg[(i+n)*nWt+(j+m)]!=11) continue;
						
				Direct3=(int)EnhanceObject.getDirectPixel(oImg, i+n, j+m, nWt, nHt);
				Direct3=EnhanceObject.getNumFromDirect(Direct3);
				
				
				switch(Direct3)
				{
				case 1:	Direct3=5;
						break;
				case 2:	Direct3=6;
						break;
				case 3:	Direct3=7;
						break;
				case 4:	Direct3=8;
						break;
				case 5:	Direct3=1;
						break;
				case 6:	Direct3=2;
						break;
				case 7:	Direct3=3;
						break;
				case 8:	Direct3=4;
						break;
				default:
						break;
				}
			
				Direct2=int((atan2((double)(i+n)-i,(double)(j+m)-j))/0.01745);
				
				if(Direct2<0) Direct2+=180;
				
				Direct2=EnhanceObject.getNumFromDirect(Direct2);

				if((Direct1!=Direct2)&&(Direct3!=Direct2)) continue;
					if(CFPMinutiae::PossibleReachPoint(inImg, nWt, nHt, j,i,(j+m),(i+n), Radius)==0){	// нельзя достигнуть
						outImg[i*nWt+j]=1;
						flag=1;
						break;
					}
		}
			if(flag==1) break;
		}
	}

	
}


// удаление коротких гребней
//  inImg [IN] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
// 	outImg [OUT] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
//	nWt, nHt [IN] - параметры изображения
void CFPMinutiae::DFMRule30(int *inImg, int *outImg, int nWt, int nHt){
	
	int Radius=10;
	int flag;

	for(int i=1; i<nHt-1; i++)
	for(int j=1; j<nWt-1; j++)
	{
		outImg[i*nWt+j]=inImg[i*nWt+j];
		if(inImg[i*nWt+j]<10) continue;

		flag=0;
		for(int n=-1*Radius; n<Radius; n++){
		for(int m=-1*Radius; m<Radius; m++){
		
				if((n==0)&&(m==0)) continue;
					if((i+n<0)||(i+n>=nHt)||(j+m<0)||(j+m>=nWt)) continue;
						if((((m*m)+(n*n))/(Radius*Radius))>1)	continue;	// по желанию можно закоментировать (- область окружности)
							if(inImg[(i+n)*nWt+(j+m)]<10) continue;

							

								if(CFPMinutiae::PossibleReachPoint(inImg, nWt, nHt, j, i,(j+m),(i+n), Radius)==1){	// можно достигнуть
									outImg[i*nWt+j]=1;
									flag=1;
									break;
								}
			}
			if(flag==1) break;
		}
	}

}


// Назначение: функция определяет возможность 
// достижения точки за определенное кол-во шагов.
// Реализация: "волновой" алгоритм.
//  inImg [IN] - массив типов точек, в котором 
// 	элемент 11 - окончание, 22 - разветвление, 1 - гребень
//	nWt, nHt [IN] - параметры изображения
//  X1,Y1 [IN] - координаты первой точки
//  X2,Y2 [IN] - координаты второй точки
// Функция возвращает 0 - если точка достижима
//	1 - если не достижима
int CFPMinutiae::PossibleReachPoint(int *Img, int nWt, int nHt, int X1, int Y1, int X2, int Y2, int Len){
	
	int flag=0;
	int i,j;
	int *delta;
	delta=new int[(2*SizeDelta+3)*(2*SizeDelta+3)];
	memset (delta,0,((2*SizeDelta+3)*(2*SizeDelta+3))*sizeof(int));

	if(((X2>X1+Len)&&(X2<X1-Len))||((Y2>Y1+Len)&&(Y2<Y1-Len))) return flag;
		if(Len>SizeDelta) Len=SizeDelta;


	for(int n=0, i=Y1-Len-1; n<2*SizeDelta+3; n++, i++)
	for(int m=0, j=X1-Len-1; m<2*SizeDelta+3; m++, j++){
	
		if((i<0)||(j<0)||(i>=nHt)||(j>=nWt)){ // за границы изображения
			delta[n*(2*SizeDelta+3)+m]=0;
			continue; 
		}
			if((n==0)||(m==0)||(i==(2*SizeDelta+3)-1)||(j==(2*SizeDelta+3)-1)) 
				delta[n*(2*SizeDelta+3)+m]=0; 
			else {
				delta[n*(2*SizeDelta+3)+m]=Img[i*nWt+j]%10; 
			}
	}


	X2=(Len+1)+(X2-X1);
	Y2=(Len+1)+(Y2-Y1);
	X1=Len+1;
	Y1=Len+1;
	delta[Y1*(2*SizeDelta+3)+X1]=10;

	//поиск
	int m=2*SizeDelta+3;
	for(int n=10; n<Len+10; n++)
	{
		for(i=0; i<m; i++)
		for(j=0; j<m; j++){
			if(delta[i*m+j]==n)
			{
				if((delta[(i-1)*m+(j-1)]<10)&&(delta[(i-1)*m+(j-1)]>0))
				{
					delta[(i-1)*m+(j-1)]=n+1;
					if((X2==(j-1))&&(Y2==(i-1))) flag=1;
				}

				if((delta[(i-1)*m+(j  )]<10)&&(delta[(i-1)*m+(j  )]>0))
				{
					delta[(i-1)*m+(j  )]=n+1;
					if((X2==(j  ))&&(Y2==(i-1))) flag=1;
				}

				if((delta[(i-1)*m+(j+1)]<10)&&(delta[(i-1)*m+(j+1)]>0))
				{
					delta[(i-1)*m+(j+1)]=n+1;
					if((X2==(j+1))&&(Y2==(i-1))) flag=1;
				}

				if((delta[(i  )*m+(j+1)]<10)&&(delta[(i  )*m+(j+1)]>0))
				{
					delta[(i  )*m+(j+1)]=n+1;
					if((X2==(j+1))&&(Y2==(i  ))) flag=1;
				}

				if((delta[(i+1)*m+(j+1)]<10)&&(delta[(i+1)*m+(j+1)]>0))
				{
					delta[(i+1)*m+(j+1)]=n+1;
					if((X2==(j+1))&&(Y2==(i+1))) flag=1;
				}

				if((delta[(i+1)*m+(j  )]<10)&&(delta[(i+1)*m+(j  )]>0))
				{
					delta[(i+1)*m+(j  )]=n+1;
					if((X2==(j  ))&&(Y2==(i+1))) flag=1;
				}

				if((delta[(i+1)*m+(j-1)]<10)&&(delta[(i+1)*m+(j-1)]>0))
				{
					delta[(i+1)*m+(j-1)]=n+1;
					if((X2==(j-1))&&(Y2==(i+1))) flag=1;
				}

				if((delta[(i  )*m+(j-1)]<10)&&(delta[(i  )*m+(j-1)]>0))
				{
					delta[(i  )*m+(j-1)]=n+1;
					if((X2==(j-1))&&(Y2==(i  ))) flag=1;
				}
			}
		}

		if (flag==1) break;
	}

	return flag;
}

