//////////////////////////////////////////////////////////////////////////
//файл:         CFPTemplate.cpp
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Компонент, определяющий шаблон биом. образа ОП
//              
//              
//////////////////////////////////////////////////////////////////////////


#include "CFPTemplate.h"
#include "FPDefine.h"
#include <memory.h>
#include <math.h>
#include <cstdlib>


	// Конструктор
CFPTemplate::CFPTemplate(){
}
	// Деструктор
CFPTemplate::~CFPTemplate(){
}




// Функция сравнения отпечатков пальцев по минуциям
// tMinutiae [IN] - вектор минуций первого отпечатка
// sMinutiae [IN] - вектор минуций второго отпечатка
// tLen [IN] - 
// sLen [IN] - длины векторов минуций первого и второго отпечатка
// res [OUT] - коэффициент совпадения [0..1]
// dX [OUT] - смещение по Х
// dY [OUT] - смещение по Y
// dR [OUT] - поворот

void CFPTemplate::MatchingSpeed(FPPoint *tMinutiae, int tLen, FPPoint *sMinutiae, int sLen, float *res, float *dX, float *dY, float *dR)
{
	int h, m;
	float dx, dy, tX, tY, dr, gg, r;

	*res=0; *dX=0; *dY=0; *dR=0;
	if((tLen == 0)||(sLen == 0)) return;

	FPPointF *minutiae1 = new FPPointF [tLen];
	FPPointF *minutiae2 = new FPPointF [sLen];
	FPPointF *minutiae3 = new FPPointF [tLen];
	
	
	// Kosty "техн."
	for(int i=0; i<sLen; i++){
		minutiae2[i].x=sMinutiae[i].x;
		minutiae2[i].y=sMinutiae[i].y;
		minutiae2[i].type=sMinutiae[i].type;
		minutiae2[i].direct=((sMinutiae[i].direct%360)*0.01745);
	}

	for(int i=0; i<tLen; i++){
		minutiae1[i].x=tMinutiae[i].x;
		minutiae1[i].y=tMinutiae[i].y;
		minutiae1[i].type=tMinutiae[i].type;
		minutiae1[i].direct=((tMinutiae[i].direct%360)*0.01745);
	}

	//Поиск приблизительного совмещения
	h=0;
	for(int i=0; i<sLen; i++)
	for(int j=0; j<tLen; j++){

		//Вычисление смещения по "X" и "Y" и поворот
	    dr=minutiae2[i].direct-minutiae1[j].direct;

			if (dr >= 2*F_PI)	dr=dr-2*F_PI;
		    if (dr < 0)		dr=dr+2*F_PI;

	    tX=(float)(minutiae1[j].x-160)*cos(dr)-(float)(minutiae1[j].y-240)*sin(dr)+160;
		tY=(float)(minutiae1[j].x-160)*sin(dr)+(float)(minutiae1[j].y-240)*cos(dr)+240;

		dx=minutiae2[i].x-tX;
		dy=minutiae2[i].y-tY;

		//Сдвиг и поворот шаблона 
		//Результат сохраняется во временном векторе
		for( int ii=0; ii<tLen; ii++)
		{
					
			minutiae3[ii].x=(int)((float)(minutiae1[ii].x-160)*cos(dr)-(float)(minutiae1[ii].y-240)*sin(dr)+dx+160);
			minutiae3[ii].y=(int)((float)(minutiae1[ii].x-160)*sin(dr)+(float)(minutiae1[ii].y-240)*cos(dr)+dy+240);
        
			gg=minutiae1[ii].direct+dr;
			if (gg >= 2*F_PI)	gg=gg-2*F_PI; 
			if (gg < 0)		gg=gg+2*F_PI; 
			minutiae3[ii].direct=gg;
		}

		//Поиск растояний между всеми минуциями со схожим направлением
	    m=0;     
		for(int ii=0; ii<sLen; ii++)
		for(int jj=0; jj<tLen; jj++)
		{
			//вычисление растояния между минуциями
			r=sqrt((float)(minutiae2[ii].x-minutiae3[jj].x)*(minutiae2[ii].x-minutiae3[jj].x)+\
		    			   (minutiae2[ii].y-minutiae3[jj].y)*(minutiae2[ii].y-minutiae3[jj].y));
						
			//вычисление разницы направления минуций
			gg=minutiae2[ii].direct-minutiae3[jj].direct;

			//Угол должен быть [0:2pi]
			if(gg>=2*F_PI)	gg=gg-2*F_PI;
            if(gg<0)		gg=gg+2*F_PI;

			//Совпала минуция?
			if((r<7)&&((gg<F_PI/8)||(gg>(2*F_PI-F_PI/8))))
			{
				m++;
				minutiae3[jj].x=9000;
				minutiae3[jj].y=9000;
				break;
			}
		}

		//Появилось лучьшее совпадение?
		if(m > h)
		{
	        h=m;
			*dX=dx;
			*dY=dy;
	        *dR=dr;
		}
	}

	//Результат
	if(sLen < tLen)	
	{
		dx=(float)sLen/tLen;
		dy=(float)h/sLen;
	} else
	{
		dx=(float)tLen/sLen;
		dy=(float)h/tLen;
	}
	*res=dx*dy;
	
	delete [] minutiae1;
	delete [] minutiae2;
	delete [] minutiae3;
}

// Функция поиска всех образованных треугольников
int CFPTemplate::FindAllTriangle(int num, int *triangle, int mode)
{
	int n=0, i1, i2, i3;
	for(i1=   0; i1<num-2; i1++)
	for(i2=i1+1; i2<num-1; i2++)
	for(i3=i2+1; i3<num-0; i3++)
	{
		if(mode != 0) {triangle[n*3+0]=i1;	triangle[n*3+1]=i2; triangle[n*3+2]=i3;}
		n++;
	}
	return n;
}

// Функция нахождения суммы элементов вектора
int CFPTemplate::Sum(int *vector, int size)
{
	int n=0;
	for(int i=0; i<size; i++) n+=vector[i];
	return n;
}

// Функция опредления наилучших смещений двух отпечатков
// tMinutiae [IN] - вектор минуций первого отпечатка
// sMinutiae [IN] - вектор минуций второго отпечатка
// tLen [IN] - 
// sLen [IN] - длины векторов минуций первого и второго отпечатка
// numShift [OUT] - число определеных смещений
// возвращает указатель на элемент типа FPShift

FPShift* CFPTemplate::MatchingSpeedTriangle(FPPoint *tMinutiae, int tLen, FPPoint *sMinutiae, int sLen, int *numShift)
{
	int ii, jj, i1, j1, nn, kk;
	float dx, dy, tX, tY, dr, gg, r;
	
        int *cmpTable=0, *cmp2=0, *Cmp=0, *CMP=0;	// вектор сопоставления минуций
	int Sum1, Sum2;
	int Step, End, sizeX, LenCMP;

	if((tLen == 0)||(sLen <= 0)) return NULL;
	
	FPPointF *minutiae1 = new FPPointF [tLen];
	FPPointF *minutiae2 = new FPPointF [sLen];
	FPPointF *minutiae3 = new FPPointF [tLen];

	// Kosty "техн."
	for(ii=0; ii<sLen; ii++){
		minutiae2[ii].x=sMinutiae[ii].x;
		minutiae2[ii].y=sMinutiae[ii].y;
		minutiae2[ii].type=sMinutiae[ii].type;
		minutiae2[ii].direct=((sMinutiae[ii].direct%360)*0.01745);
	}

	for(jj=0; jj<tLen; jj++){
		minutiae1[jj].x=tMinutiae[jj].x;
		minutiae1[jj].y=tMinutiae[jj].y;
		minutiae1[jj].type=tMinutiae[jj].type;
		minutiae1[jj].direct=((tMinutiae[jj].direct%360)*0.01745);
	}

	//Поиск приблизительного совмещения
	LenCMP=0;
	cmpTable = new int [tLen*sLen];
	for(ii=0; ii<sLen; ii++)
	for(jj=0; jj<tLen; jj++){

		//Вычисление смещения по "X" и "Y" и поворот
	    dr=minutiae2[ii].direct-minutiae1[jj].direct;

			if (dr >= 2*F_PI)	dr=dr-2*F_PI;
		    if (dr < 0)		dr=dr+2*F_PI;

	    tX=(float)(minutiae1[jj].x-160)*cos(dr)-(float)(minutiae1[jj].y-240)*sin(dr)+160;
		tY=(float)(minutiae1[jj].x-160)*sin(dr)+(float)(minutiae1[jj].y-240)*cos(dr)+240;

		dx=minutiae2[ii].x-tX;
		dy=minutiae2[ii].y-tY;

		//Сдвиг и поворот шаблона 
		//Результат сохраняется во временном векторе
		for(i1=0; i1<tLen; i1++){
					
			minutiae3[i1].x=(int)((float)(minutiae1[i1].x-160)*cos(dr)-(float)(minutiae1[i1].y-240)*sin(dr)+dx+160);
			minutiae3[i1].y=(int)((float)(minutiae1[i1].x-160)*sin(dr)+(float)(minutiae1[i1].y-240)*cos(dr)+dy+240);
        
			gg=minutiae1[i1].direct+dr;
			if (gg >= 2*F_PI)	gg=gg-2*F_PI; 
			if (gg < 0)		gg=gg+2*F_PI; 
			minutiae3[i1].direct=gg;
		}

		//Поиск растояний между всеми минуциями со схожим направлением
		memset(cmpTable,0,sLen*tLen*sizeof(int));
		for(i1=0; i1<sLen; i1++)
		for(j1=0; j1<tLen; j1++){
			//вычисление растояния между минуциями
			r=sqrt((float)(minutiae2[i1].x-minutiae3[j1].x)*(minutiae2[i1].x-minutiae3[j1].x)+\
		    			   (minutiae2[i1].y-minutiae3[j1].y)*(minutiae2[i1].y-minutiae3[j1].y));
						
			//вычисление разницы направления минуций
			gg=minutiae2[i1].direct-minutiae3[j1].direct;

			//Угол должен быть [0:2pi]
			if(gg>=2*F_PI)	gg=gg-2*F_PI;
            if(gg<0)		gg=gg+2*F_PI;

			//Совпала минуция?
			if((r<12)&&((gg<F_PI/8)||(gg>(2*F_PI-F_PI/8))))
				cmpTable[i1*tLen+j1]=1;
		}

		//Есть ли смысл выполнять дальше действия?
		i1=0;
		Sum1=1;
		for(j1=0; j1<sLen; j1++){
			Sum2=Sum(cmpTable+j1*tLen,tLen);
			if (Sum2 != 0){
				i1++;
				Sum1*=Sum2;
			}
		}
		if(i1 < 3) continue;

		//Все возможные варианты (совмещения) 
		cmp2 = new int [Sum1*sLen];
		memset(cmp2,-1,Sum1*sLen*sizeof(int));
		
		End=1;
		for(i1=0; i1<sLen; i1++){
			Step=1;
			sizeX=End;
			for(j1=0; j1<tLen; j1++){
				//Нужно ли выполнять дальше действия?
				if(cmpTable[i1*tLen+j1] == 0) continue;

				if(Step > 1){
					for(nn=1; nn<=sizeX; nn++)
					for(kk=0; kk<sLen; kk++)
						cmp2[(End+(nn-1))*sLen+kk]=cmp2[(nn-1)*sLen+kk];
					End=End+sizeX;
				}

				for(nn=End-sizeX; nn<End; nn++)
					cmp2[nn*sLen+i1]=j1;
				
				Step++;
			}
		}

		//Добавление новых вариантов совмещения
		CMP = new int [(LenCMP+Sum1)*sLen];

		for(i1=0; i1<LenCMP; i1++)
		for(j1=0; j1<sLen; j1++)
			CMP[i1*sLen+j1]=Cmp[i1*sLen+j1];

		for(i1=LenCMP; i1<LenCMP+Sum1; i1++)
		for(j1=0; j1<sLen; j1++)
			CMP[i1*sLen+j1]=cmp2[(i1-LenCMP)*sLen+j1];

                if(cmp2 !=0) {delete [] cmp2; cmp2=0;}

                if(Cmp  !=0)
		{
			delete [] Cmp;  
                        Cmp=0;
		}

		Cmp=CMP;
		LenCMP+=Sum1;
	}

        if(cmpTable !=0) {delete [] cmpTable; cmpTable=0;}

	//Сранение треугольников
	numShift[0]=0;
        FPShift *Shift=0;
	Shift = new FPShift [LenCMP];

	int flag;
	int triNumT, triNum, j2, j3;
	int *Triangle_template=NULL, *Triangle=NULL;

	int rr1, rr2, rr3, tt1, tt2, tt3;
	float edge_r1, edge_r2, edge_r3, edge_t1, edge_t2, edge_t3;
	float delta_adge_1, delta_adge_2, delta_adge_3;
	float Min;
	float cosX, sinY;
	float dx1, dx2, dx3, dy1, dy2, dy3;

	triNumT=FindAllTriangle(sLen, NULL, 0);
	Triangle_template = new int [triNumT*3];
	Triangle = new int [triNumT*3];
	FindAllTriangle(sLen, Triangle_template, 1);
	
	for(i1=0; i1<LenCMP; i1++){
		//Опр. какие треугольники нужно сравнить
		for(j1=0;  j1<triNumT; j1++){
			Triangle[j1*3+0]=Cmp[i1*sLen+Triangle_template[j1*3+0]];
			Triangle[j1*3+1]=Cmp[i1*sLen+Triangle_template[j1*3+1]];
			Triangle[j1*3+2]=Cmp[i1*sLen+Triangle_template[j1*3+2]];
		}

		triNum=triNumT;

		for(j1=triNumT-1;  j1>=0; j1--)	{
			flag=0;
			for(j2=0; j2<3; j2++){
				if((flag == 1) || (Triangle[j1*3+j2] == -1)) {flag=1;	break;}

				for(j3=0; j3<3; j3++){
					//Одна и таже вершина?
					if(j2 == j3) continue;

					if((Triangle[j1*3+j2] == Triangle[j1*3+j3]) || (Triangle[j1*3+j3] == -1)) {flag=1; break;}
				}
			}

			if(flag == 1){
				//удаление
				for(j2=j1+1; j2<triNum; j2++){
					Triangle[(j2-1)*3+0]=Triangle[j2*3+0];
					Triangle[(j2-1)*3+1]=Triangle[j2*3+1];
					Triangle[(j2-1)*3+2]=Triangle[j2*3+2];
				}
				triNum--;
			}else{
				Triangle[j1*3+0]=Triangle_template[j1*3+0];
				Triangle[j1*3+1]=Triangle_template[j1*3+1];
				Triangle[j1*3+2]=Triangle_template[j1*3+2];
			}
		}

		//...
		if(triNum <= 0) continue;

		//цикл по всем возможным треугольникам
		Min=90000;
		for(j1=0;  j1<triNum; j1++){
			//номер вершины
			rr1=Triangle[j1*3+0];
			rr2=Triangle[j1*3+1];
			rr3=Triangle[j1*3+2];

			tt1=Cmp[i1*sLen+rr1];
		    tt2=Cmp[i1*sLen+rr2];
			tt3=Cmp[i1*sLen+rr3];

			//Длина ребра
	        edge_r1=sqrt((float)((minutiae2[rr1].y-minutiae2[rr2].y)*(minutiae2[rr1].y-minutiae2[rr2].y)+\
								  (minutiae2[rr1].x-minutiae2[rr2].x)*(minutiae2[rr1].x-minutiae2[rr2].x)));
	        edge_r2=sqrt((float)((minutiae2[rr1].y-minutiae2[rr3].y)*(minutiae2[rr1].y-minutiae2[rr3].y)+\
								  (minutiae2[rr1].x-minutiae2[rr3].x)*(minutiae2[rr1].x-minutiae2[rr3].x)));
	        edge_r3=sqrt((float)((minutiae2[rr2].y-minutiae2[rr3].y)*(minutiae2[rr2].y-minutiae2[rr3].y)+\
								  (minutiae2[rr2].x-minutiae2[rr3].x)*(minutiae2[rr2].x-minutiae2[rr3].x)));

	        edge_t1=sqrt((float)((minutiae1[tt1].y-minutiae1[tt2].y)*(minutiae1[tt1].y-minutiae1[tt2].y)+\
								  (minutiae1[tt1].x-minutiae1[tt2].x)*(minutiae1[tt1].x-minutiae1[tt2].x)));
	        edge_t2=sqrt((float)((minutiae1[tt1].y-minutiae1[tt3].y)*(minutiae1[tt1].y-minutiae1[tt3].y)+\
								  (minutiae1[tt1].x-minutiae1[tt3].x)*(minutiae1[tt1].x-minutiae1[tt3].x)));
	        edge_t3=sqrt((float)((minutiae1[tt2].y-minutiae1[tt3].y)*(minutiae1[tt2].y-minutiae1[tt3].y)+\
								  (minutiae1[tt2].x-minutiae1[tt3].x)*(minutiae1[tt2].x-minutiae1[tt3].x)));

			//Опр. разницы в длинее рёбер
	        delta_adge_1=abs(edge_r1-edge_t1);
		    delta_adge_2=abs(edge_r2-edge_t2);
			delta_adge_3=abs(edge_r3-edge_t3);

			Sum1=delta_adge_1+delta_adge_2+delta_adge_3;

			if(Sum1 < Min) {Min=Sum1; j2=j1;}
 		}

		//номер вершины
		rr1=Triangle[j2*3+0];
		rr2=Triangle[j2*3+1];
		rr3=Triangle[j2*3+2];

		tt1=Cmp[i1*sLen+rr1];
	    tt2=Cmp[i1*sLen+rr2];
		tt3=Cmp[i1*sLen+rr3];

		//Опр. поворота (точнее)
		cosX=cos(minutiae1[tt1].direct-minutiae2[rr1].direct)+\
			 cos(minutiae1[tt2].direct-minutiae2[rr2].direct)+\
			 cos(minutiae1[tt3].direct-minutiae2[rr3].direct);
		sinY=sin(minutiae1[tt1].direct-minutiae2[rr1].direct)+\
			 sin(minutiae1[tt2].direct-minutiae2[rr2].direct)+\
			 sin(minutiae1[tt3].direct-minutiae2[rr3].direct);

		dr=atan2(sinY,cosX);
		if(dr < 0) dr=dr+2*F_PI;

		//Опр. смещения (точнее)
		dx1=(float)(minutiae2[rr1].x-160)*cos(dr)-(float)(minutiae2[rr1].y-240)*sin(dr)+160;
		dx2=(float)(minutiae2[rr2].x-160)*cos(dr)-(float)(minutiae2[rr2].y-240)*sin(dr)+160;
		dx3=(float)(minutiae2[rr3].x-160)*cos(dr)-(float)(minutiae2[rr3].y-240)*sin(dr)+160;

		dy1=(float)(minutiae2[rr1].x-160)*sin(dr)+(float)(minutiae2[rr1].y-240)*cos(dr)+240;
		dy2=(float)(minutiae2[rr2].x-160)*sin(dr)+(float)(minutiae2[rr2].y-240)*cos(dr)+240;
		dy3=(float)(minutiae2[rr3].x-160)*sin(dr)+(float)(minutiae2[rr3].y-240)*cos(dr)+240;


		dx=(-dx1-dx2-dx3+(float)minutiae1[tt1].x+(float)minutiae1[tt2].x+(float)minutiae1[tt3].x)/3;
		dy=(-dy1-dy2-dy3+(float)minutiae1[tt1].y+(float)minutiae1[tt2].y+(float)minutiae1[tt3].y)/3;


		Shift[numShift[0]].dX=(int)dx;
		Shift[numShift[0]].dY=(int)dy;
		Shift[numShift[0]].dR=dr;

		numShift[0]=numShift[0]+1;
	}

	
	delete [] Cmp;
	delete [] Triangle;
	delete [] Triangle_template;
	delete [] minutiae1;
	delete [] minutiae2;
	delete [] minutiae3;
	return Shift;
}

// Функция выполняющая Афинные преобразования над вектором минуций отпечатка
// minutiae [IN/OUT] -  вектор минуций отпечатка
// len [IN] - длинна вектора или число точек
// dX [IN] - смещение по Х
// dY [IN] - смещение по Y
// dR [IN] - поворот

void CFPTemplate::AffineTransform(FPPoint *minutiae, int len, float dX, float dY, float dR)
{
	float x,y,angle;	
	//Цикл по всем минуциям
	for (int i=0; i<len; i++){
		x=(float)(minutiae[i].x-160)*cos(dR)-(float)(minutiae[i].y-240)*sin(dR)+dX+160+0.5;
		y=(float)(minutiae[i].x-160)*sin(dR)+(float)(minutiae[i].y-240)*cos(dR)+dY+240+0.5;
		minutiae[i].x=(int)x;
		minutiae[i].y=(int)y;
		//Угол должен быть [0:360]
		angle=minutiae[i].direct+(int)(dR/0.01745);
		
		if (angle >= 360)	angle=angle-360; 
		if (angle < 0)		angle=angle+360; 
		minutiae[i].direct=angle;
	}
}
// Функция нахождения наилучшего совмещения всех ОП
// fpNum [IN] - количество отпечатков
// Cmp [IN] - таблица коэффициентов совпадения отпечатков.
// Offset [IN] - таблица смещений отпечатков
// OffsetBest [OUT] - вектор наилучших смещений
// Возвращает 0 при успешном завершении

int CFPTemplate::FindBestMatching(int fpNum, float *Cmp, FPShift *Offset, FPShift *OffsetBest)
{
	int ii,jj,nn,iii,jjj,Num;
	int *Chain;
	float Max;

	//Праверка аргументов
	if((fpNum<=0) || (Cmp==NULL) || (Offset==NULL) || (OffsetBest==NULL))
		return 1;

	//Для точного поиска елементы таблицы должны удовлетворять условию 
	//Cmp(i,j)==Cmp(j,i), в противном случае нельзя гарантировать наилучшее
	//совмещение.
	for(ii=0; ii<fpNum; ii++)
	for(jj=0; jj<fpNum; jj++){

	    //Найлучший результат сравнения
		if(Cmp[ii*fpNum+jj] > Cmp[jj*fpNum+ii])  //Почему ">"?
		{
			Cmp[ii*fpNum+jj]=Cmp[jj*fpNum+ii];
	        Offset[ii*fpNum+jj].dX=-1*Offset[jj*fpNum+ii].dX;
		    Offset[ii*fpNum+jj].dY=-1*Offset[jj*fpNum+ii].dY;
			Offset[ii*fpNum+jj].dR=-1*Offset[jj*fpNum+ii].dR;
		}else{
		    Cmp[jj*fpNum+ii]=Cmp[ii*fpNum+jj];
			Offset[jj*fpNum+ii].dX=-1*Offset[ii*fpNum+jj].dX;
			Offset[jj*fpNum+ii].dY=-1*Offset[ii*fpNum+jj].dY;
			Offset[jj*fpNum+ii].dR=-1*Offset[ii*fpNum+jj].dR;
		}
	}

	//Начальные значения
	Chain = new int [fpNum];
	memset(Chain,0,fpNum*sizeof(int));
	memset(OffsetBest,0,fpNum*sizeof(FPShift));

	//Все ОП будут смещены относительно 1-ого ОП
	Num=0;
	Chain[Num]=0;
	OffsetBest[Num].dX=0;
	OffsetBest[Num].dY=0;
	OffsetBest[Num].dR=0;

	//Поиск наилучшего совмещения всех ОП
	while (Num < fpNum-1) //Цикл до тех пор, пока не будут смещены все ОП
	{
	    //Поиск максимального значения коэффициента совпадения
	    Max=-1;
		for(nn=0; nn<=Num; nn++) //Цикл по ОП, которые УЖЕ смещены
		{
	        ii=Chain[nn];
			for(jj=0; jj<fpNum; jj++){
			    //Коэффициенты совпадения, стоящие на диагонали не рассматриваются
				if (ii == jj) continue;
            
			    //Есть ли новое максимальное значение коэффициента совпадения?
				if (Max < Cmp[ii*fpNum+jj]){
	                Max=Cmp[ii*fpNum+jj];
		            iii=ii;
			        jjj=jj;
				}
			}
		}

	    //Наследующем проходе эти коэффициенты не расматриваются
	    for(nn=0; nn<=Num; nn++){
		    ii=Chain[nn];
			Cmp[ii*fpNum+jjj]=-1;
			Cmp[jjj*fpNum+ii]=-1;
		}

	    //Заполнение вектора смещений ОП
	    OffsetBest[jjj].dX=OffsetBest[iii].dX+Offset[jjj*fpNum+iii].dX;
		OffsetBest[jjj].dY=OffsetBest[iii].dY+Offset[jjj*fpNum+iii].dY;
		OffsetBest[jjj].dR=OffsetBest[iii].dR+Offset[jjj*fpNum+iii].dR;

	    //Еще один ОП добавлен в вектор смещений
	    Num++;
		Chain[Num]=jjj;

	}

	delete [] Chain;
	return 0;
}


// Функция выбирает точки входящие в окно 320х480 после смещения центра тяжести
// к центральной точке 160; 240
// minutiae [IN/OUT] -  вектор минуций отпечатка, смещенный к центру
// newMinutiae [OUT] -  новый вектор минуций, входящих в окно 320х480, только в режиме mode != 0
// len [IN] - длинна вектора или число точек
// mode [IN] - режим работы функции

int CFPTemplate::FindMinutiaeWin(FPPoint *minutiae, int len, FPPoint *newMinutiae, int mode)
{
	int SumX, SumY, PowerX, PowerY, dX, dY;
	//Определение энергии(центра тяжести) минуций
	SumX=0; SumY=0;
	for( int i=0; i<len; i++ ){
		SumX=SumX+minutiae[i].x;
		SumY=SumY+minutiae[i].y;
	}

	PowerX=SumX/len;
	PowerY=SumY/len;

	//Смещение
	dX=160-PowerX;
	dY=240-PowerY;

	//Поиск минуций которые попадают в окно(320x480)
	int j=0;
	for( int i=0; i<len; i++ ){
		minutiae[i].x=minutiae[i].x+dX;
		minutiae[i].y=minutiae[i].y+dY;
    
	    if((minutiae[i].x < 0)||(minutiae[i].x >= 320)||(minutiae[i].y < 0)||(minutiae[i].y >= 480))  continue; 
			if (mode != 0){
				newMinutiae[j].x=minutiae[i].x;
					newMinutiae[j].y=minutiae[i].y;
						newMinutiae[j].direct=minutiae[i].direct;
			}
				j++;
	}
	return j;
}



// Поиск единичных областей
// minutiae [IN] - вектор минуций биометрического образа
// len [IN] - длинна вектора или число точек
// ones [OUT] - единичные зоны
// mode [IN] - режим работы функции
//		если mode=0 функция возвращает число зон
//		если mode!=0 функция возвращает число зон, вектор областей находится в переменной ones
// 		функция возвращает 0, при нулевом векторе минуций
int CFPTemplate::FindRegionOnes(FPPoint *minutiae, int len, int fpNum, FPRegion *ones, int mode)
{
	int New, Num_Ones, *Chain, N, dX, dY, x_min, y_min, x_max, y_max;
	float r, angle, cosX, sinY;

	if((len <= 0) || (minutiae == NULL)) return 0;

	Num_Ones=1; //Номер области
	Chain = new int [2*len]; //Последовательность минуций, который "УЖЕ" включины в области
	memset(Chain,0,(2*len*sizeof(int)));

	//Поиск с 1-ой минуции (соответственно номер области =1)
	Chain[0*2+0]=Num_Ones;    //Номер области
	Chain[0*2+1]=0;           //Статус (указывает на то что, поиск вокруг минуции был выполнен)

	//Поиск областей
	New=1;
	while(New != 0)
	{
		//Поиск минуций принадлежащих области с номером "Num_Ones"
		while(New != 0)
		{ 
			for( int i=0; i<len; i++ ){
	            //Принадлежит ли минуция области с номером "Num_Ones"?
		        if (Chain[i*2+0] != Num_Ones) continue;

	            //Был ли "УЖЕ" поиск вокруг этой минуции?
		        if (Chain[i*2+1] == 1) continue;

				for( int j=0; j<len; j++ ){
					//Принадлежит ли эта минуции "УЖЕ" какой нибудь области?
					if (Chain[j*2+0] != 0) continue;

					//вычисление растояния между минуциями
					r=sqrt((float)(minutiae[i].x-minutiae[j].x)*(minutiae[i].x-minutiae[j].x)+\
								   (minutiae[i].y-minutiae[j].y)*(minutiae[i].y-minutiae[j].y));
						
					//вычисление разницы направления минуций
					angle=minutiae[i].direct-minutiae[j].direct;

					//Угол должен быть [0:360]
					if(angle >= 360)	angle=angle-360;
			        if(angle <  0)		angle+=360;

	                //Совпала минуция?
					if((r < 7) && ((angle < 25) || (angle > (360-25)))){
			            Chain[j*2+0]=Num_Ones;
				        New=New+1;
					}
				}

	            //Поиск вокруг минуции выполнен!
		        Chain[i*2+1]=1;
			    New=New-1;
			}
		}
	    
		//Есть ли еще области?
		New=0; 
		for( int i=0; i<len; i++){
			if (Chain[i*2+0] == 0){
				New=1;
				Num_Ones++;
				Chain[i*2+0]=Num_Ones;
				break;
			}
		}
	}

	//Какой режим?
	if((mode == 0) || (ones==NULL)){ 
		delete [] Chain; 
			return Num_Ones; 
	}

	//Поиск координат и вероятностей областей
	for( int i=0; i<Num_Ones; i++ ){
	    N=0; //Кол-во минуций принадлижащих "i" области
	    cosX=0;	sinY=0;
		dX=0; dY=0;
		x_min=9000;	y_min=9000;	x_max=-9000; y_max=-9000;
		
		//Цикл по минуциям
		for( int j=0; j<len; j++ ){ 
	        //Принадлежит минуция "i" области?
		    if (i != Chain[j*2+0]-1) continue;
    
		    //Подсчет кол-ва минуций в "i" области
	        N++;
        
		    //Направление
	        sinY=sinY+sin((float)(minutiae[j].direct)*0.01745);
		    cosX=cosX+cos((float)(minutiae[j].direct)*0.01745);
        
	        //Координаты
		    dX=dX+minutiae[j].x;
			dY=dY+minutiae[j].y;

	        //Поиск мин. мах. значений "x" и "y" области
		    if (x_min > minutiae[j].x) x_min=minutiae[j].x;
            if (y_min > minutiae[j].y) y_min=minutiae[j].y;
		    if (x_max < minutiae[j].x) x_max=minutiae[j].x;
		    if (y_max < minutiae[j].y) y_max=minutiae[j].y;
		}

	    //Направление [0:360]
	    angle = (int)(atan2(sinY,cosX)/0.01745);
		if(angle <  0)	angle+=360;
		
		ones[i].percent=(float)N/fpNum;
		ones[i].x=dX/N;
	    ones[i].y=dY/N;
		ones[i].direct=angle;
		ones[i].r=1+sqrt((float)(x_max-x_min)*(x_max-x_min)+(y_max-y_min)*(y_max-y_min))/2;
	}
	
	delete [] Chain;
	return Num_Ones;
}



// Поиск областей для совмещения ОП. Поиск областей идет по принципу
// выбора "num" областей с найбольшим коэффициентом "выбора" равным
// "region.percent/region.r".
// region [IN] - вектор единичных областей
// size [IN] - размер
// num [IN] - количество областей для совмещения
// stitch [IN] - вектор найденных областей
// взвращет false если вектор едничных областей содержит меньше 
// элементов, чем необходимо найти
// при выполнении условия возвращает true

int CFPTemplate::FindRegionStitching(FPRegion *region, int size, int num, FPRegion *stitch)
{
	float *Coefficient, max;

	if (size < num) return 1;
	
	//Коэффициент "выбора"
	Coefficient = new float [size];
	
	for ( int i=0; i<size; i++ )
	    Coefficient[i]=region[i].percent;
	
	//Поиск "num" областей
	int n;
	for( int i=0; i<num; i++ ){ 
		max=0;
		//Поиск максимального коэффициента
	    for( int j=0; j<size; j++ )
			if (max < Coefficient[j]){
				max=Coefficient[j];
				n=j;  //номер коэффициента
			}
		stitch[i].x=region[n].x;
		stitch[i].y=region[n].y;
		stitch[i].r=region[n].r;
		stitch[i].direct=region[n].direct;
		stitch[i].percent=region[n].percent;
		Coefficient[n]=-1;
	}
	
	delete [] Coefficient;

	return 0;
}


// Поиск областей которые попадают в окно(192x192) полностью, 
// а также имеют заданную вероятность.
// region [IN] - вектор областей
// num [IN] - число областей
// threshold [IN] - порог
// newRegion [OUT] - вектор новых областей, попадающих в окно, в режиме mode != 0
// mode [IN] - режим работы функции
// в режиме mode=0 функция возвращает число областей удовлетворяющих условию

int CFPTemplate::FindRegionWinThr(FPRegion *region, int num, float threshold, FPRegion *newRegion, int mode)
{
	//Предварительно
	int j=0;
	for(int i=0; i<num; i++) 
	{
	    //Какая вероятность?
		if ((region[i].percent < threshold) || (region[i].percent > 1))
			continue;

	    //Попадает ли область в окно полностью?
		if((region[i].x-region[i].r < 0) || (region[i].x+region[i].r >= 320)||\
		   (region[i].y-region[i].r < 0) || (region[i].y+region[i].r >= 480))
		    continue;	
		if(mode != 0)
		{
			newRegion[j].x=region[i].x;
			newRegion[j].y=region[i].y;
			newRegion[j].r=region[i].r;
			newRegion[j].direct=region[i].direct;
			newRegion[j].percent=region[i].percent;
		}
		j++;
	}

	return j;
}


// Генерация сетки размером 20х30
// Память выделяется вызывающей стороной
// minutiae [IN] - вектор минуций
// len [IN] - число минуций
// fpNum [IN] - число отпечатков
// grid [OUT] - получаемая сетка
// mode [IN] - режим
//    если mode = 0  функция возвращает число элементов
//    если mode != 0  функция возвращает фактическое число элементов

int CFPTemplate::GenGrid(FPPoint *minutiae, int len, int fpNum, FPRegion *grid, int mode)
{
	int n;
	int x_begin=20, x_end=0, y_begin=30, y_end=0;
	
	FPRegion *gridTemp = new FPRegion [20*30];


	//Сетка (инициализация)
	for( int i=0; i<30; i++ )
	for( int j=0; j<20; j++ ){
	    gridTemp[i*20+j].x=j*16;
		gridTemp[i*20+j].y=i*16;
		gridTemp[i*20+j].percent=0;
		gridTemp[i*20+j].r=16;
		gridTemp[i*20+j].direct=0;
		n=0;
	    for(int ii=0; ii<len; ii++){
		    if(minutiae[ii].x <  gridTemp[i*20+j].x)	continue;
	        if(minutiae[ii].x >= gridTemp[i*20+j].x+16)	continue;
	        if(minutiae[ii].y <  gridTemp[i*20+j].y)	continue;
	        if(minutiae[ii].y >= gridTemp[i*20+j].y+16)	continue;
			n++;
		}
		gridTemp[i*20+j].percent=(double)n/fpNum;
		
		if(gridTemp[i*20+j].percent == 0) continue;

	    if(x_begin > j)	x_begin=j;
		if(y_begin > i)	y_begin=i;
		if(x_end < j)	x_end=j;
		if(y_end < i)	y_end=i;
	}

	n=(x_end-x_begin+1)*(y_end-y_begin+1);
	
	if(mode == 0){
		delete [] gridTemp;
			return n;
	}

	//Результат
	n=0;
	for(int i=0; i<30; i++)
	for(int j=0; j<20; j++){
	    if(j < x_begin) continue;
		if(i < y_begin) continue;
		if(j > x_end) continue;
		if(i > y_end) continue;

	    grid[n].x=gridTemp[i*20+j].x;
		grid[n].y=gridTemp[i*20+j].y;
		grid[n].percent=gridTemp[i*20+j].percent;
		grid[n].r=gridTemp[i*20+j].r;
		grid[n].direct=gridTemp[i*20+j].direct;

		n++;
	}
	
	delete [] gridTemp;
	return n;
}

// Получение шаблона отпечатков пальцев
// allMinutiae [IN] - все минуции примеров, участвующих в обучении
// numMinutiae [IN] - число минуций у каждого примера
// imgCount [IN] - число образов
// rightImg [OUT] - вектор правильных примеров, на которых следует обучить НС
// bestMinutiae [OUT] - минуции примеров, на которых следует обучить НС
// numBestMinutiae [OUT] - число минуций у каждого примера
// templateMinutiae [OUT] - шаблон, состоящий из 5 минуций
// Функция возвращает 0, если шаблон из 5 точек получен
// Функция возвращает 1, если шаблон из 5 точек не получен
int CFPTemplate::getTemplate ( FPPoint *allMinutiae, int *numMinutiae, int imgCount, int *rightImg, FPPoint *bestMinutiae, int *numBestMinutiae, FPPoint *templateMinutiae )
{
	float *cmpTable  =new float [imgCount*imgCount];
	FPShift *offsetTable = new FPShift [imgCount*imgCount];
	int allNumberMinutiae = 0;
	
	float res;
	float dX;
	float dY;
	float dR;
	int	  tNum;					// количество координат
	int	  sNum;	
	int tShift=0;
	int sShift=0;
	
	//Сравнение векторов минуций отпечатков пальцев между собой
	for (int i=0; i<imgCount; i++){
		tNum = numMinutiae[i];	
		sShift = 0;
			for (int j=0; j<=i; j++){
				sNum = numMinutiae[j];
				if (i != j)
					MatchingSpeed(allMinutiae+tShift,tNum,allMinutiae+sShift,sNum,&res,&dX,&dY,&dR);

				else{
					res=1; dX=0; dY=0; dR=0;
				}
				if(res < 0.3){
					res=0;
				}
				//Таблица коэффициентов совпадения ОП
				cmpTable[i*imgCount+j]=res;
				cmpTable[j*imgCount+i]=res;

				//Таблица коэффициентов для аффинных преобразования
				offsetTable[i*imgCount+j].dX=dX;
				offsetTable[i*imgCount+j].dY=dY;
				offsetTable[i*imgCount+j].dR=dR;
				offsetTable[j*imgCount+i].dX=dX*(-1);
				offsetTable[j*imgCount+i].dY=dY*(-1);
				offsetTable[j*imgCount+i].dR=dR*(-1);

				sShift += numMinutiae[j];

			}

		tShift += numMinutiae[i];

	}
	
	allNumberMinutiae = sShift;
	
	int indexImg;
	int sum;
	//Поиск отпечатков подходящих для "обучения"
	for(int i=0, max=0; i<imgCount; i++){
		indexImg = 0;
		sum=0;
		for(int j=0; j<imgCount; j++)
			if(cmpTable[i*imgCount+j] != 0) sum++;
		if(max < sum){
			max=sum;
			indexImg=i;
		}
	}

	// Создание маски отпечатков подходящих для "обучения"	
	for(int j=0; j<imgCount; j++){
		if(cmpTable[indexImg*imgCount+j] == 0)
			rightImg[j]=0; // отпечаток не подходит для обучения
		else rightImg[j]=1;  // отпечаток подходит для обучения
	}
	
	
	
	// Окончательные таблицы сравнения и совмещения ОП подходящих для "обучения"
	// Таблица обрезается лишь в том случае, когда есть нулевые коэффициенты
	int newImgCount = imgCount;
	for(int n = imgCount-1; n>=0; n--){
		if(cmpTable[indexImg*imgCount+n] == 0){
			//смещение столбца
			for(int i=0; i<imgCount; i++) {
				for(int j=n; j<imgCount-1; j++) {
					cmpTable[i*imgCount+j]=cmpTable[i*imgCount+j+1];
					offsetTable[i*imgCount+j].dX=offsetTable[i*imgCount+j+1].dX;
					offsetTable[i*imgCount+j].dY=offsetTable[i*imgCount+j+1].dY;
					offsetTable[i*imgCount+j].dR=offsetTable[i*imgCount+j+1].dR;

				}

				cmpTable[i*imgCount+imgCount-1]=0;
				offsetTable[i*imgCount+imgCount-1].dX=0;
				offsetTable[i*imgCount+imgCount-1].dY=0;
				offsetTable[i*imgCount+imgCount-1].dR=0;

			}

			//смещение строки
			for(int i=0; i<imgCount; i++) {
				for(int j=n; j<imgCount-1; j++) {	
					cmpTable[j*imgCount+i]=cmpTable[(j+1)*imgCount+i];
					offsetTable[j*imgCount+i].dX=offsetTable[(j+1)*imgCount+i].dX;
					offsetTable[j*imgCount+i].dY=offsetTable[(j+1)*imgCount+i].dY;
					offsetTable[j*imgCount+i].dR=offsetTable[(j+1)*imgCount+i].dR;
				}

				cmpTable[(imgCount-1)*imgCount+i]=0;
				offsetTable[(imgCount-1)*imgCount+i].dX=0;
				offsetTable[(imgCount-1)*imgCount+i].dY=0;
				offsetTable[(imgCount-1)*imgCount+i].dR=0;

			}

			newImgCount--;


		}
	}
	
	int nnn=0;
	for(int i=0; i<newImgCount; i++)
		for(int j=0; j<newImgCount; j++){
			cmpTable[nnn]=cmpTable[i*imgCount+j];
			offsetTable[nnn].dX=offsetTable[i*imgCount+j].dX;
			offsetTable[nnn].dY=offsetTable[i*imgCount+j].dY;
			offsetTable[nnn].dR=offsetTable[i*imgCount+j].dR;
			nnn++;
		}
	
	
	FPShift *offsetBestTable = new FPShift [newImgCount];

	//Поиск наилучшего совмещения всех ОП
	if (FindBestMatching(newImgCount, cmpTable, offsetTable, offsetBestTable)) return 1;
	
	// Оставшиеся вектора минуций перемещаем в вектор лучших минуций
	tShift = 0;
	sShift = 0;
	for (int i=0, ii=0; i<imgCount; i++){
		if (rightImg[i] != 0){
			numBestMinutiae[ii] = numMinutiae[i];
				ii++;
					for (int j=sShift; j<numMinutiae[i]+sShift; j++){
						bestMinutiae[tShift] = allMinutiae[j];
						tShift++;
					}	
		}
		sShift += numMinutiae[i];
	}
	
	tNum = tShift; // оставшиеся минуции
	
	//Выделение памяти для переменных
	FPPoint *tMinutiae = new FPPoint [tNum];	// массив с минуциями
	FPPoint *sMinutiae = new FPPoint [tNum];	// массив с минуциями
	FPRegion *ones = new FPRegion [tNum];
	FPRegion *onesNew = new FPRegion [tNum];
	FPRegion *grid = new FPRegion [20*30];
	int nGrid;
	
	//Загрузка минуций всех правильных образов и их смещение
	tNum=0;
	sNum=0;
	sShift=0;
	for(int i=0; i<newImgCount; i++){ 
		// Лучшее cмещение
		dX=offsetBestTable[i].dX;
		dY=offsetBestTable[i].dY;
		dR=offsetBestTable[i].dR;	
		//Аффинные преобразования
		AffineTransform(bestMinutiae+sShift, numBestMinutiae[i], dX, dY, dR);
		sShift += numBestMinutiae[i];
	}
	
	sNum = sShift;
	FPPoint *centerBestMinutiae = new FPPoint [sNum];

	for (int i=0; i<sNum; i++ )
		centerBestMinutiae[i] = bestMinutiae[i];
	
	// *** Определение энергии(центра тяжести) минуций ***
	// *** Для вычитания из координат точек шаблона ***
	int sumX=0; 
	int sumY=0;
	int powerX=0;
	int powerY=0;

	for(int i=0; i<sNum; i++){
		sumX=sumX+centerBestMinutiae[i].x;
		sumY=sumY+centerBestMinutiae[i].y;
	}

	powerX = 160 - sumX/sNum;
	powerY = 240 - sumY/sNum;
		

	tNum=FindMinutiaeWin(centerBestMinutiae, sNum, sMinutiae, 1);	
	nGrid = GenGrid(sMinutiae, tNum, newImgCount, grid, 1);
	sNum = FindRegionOnes(sMinutiae, tNum, newImgCount, ones, 1);
	tNum = FindRegionWinThr(ones, sNum, 0.5, onesNew, 1);
	FindRegionStitching(onesNew, tNum, tNum, ones);
	
	if(tNum < 5) {
	
		delete [] tMinutiae;
		delete [] sMinutiae;
		delete [] grid;
		delete [] offsetBestTable;
		delete [] offsetTable;
		delete [] cmpTable;
		delete [] centerBestMinutiae;
		
			return 1; // необходимые точки для шаблона не найдены
	}

	// Результат - пять точек для совмещения
	for(int i=0; i<5; i++){
		templateMinutiae[i].x=ones[i].x - powerX; // ***
		templateMinutiae[i].y=ones[i].y - powerY; // ***
		templateMinutiae[i].direct=ones[i].direct;
	}
	
	delete [] tMinutiae;
	delete [] sMinutiae;
	delete [] grid;
	delete [] offsetBestTable;
	delete [] offsetTable;
	delete [] cmpTable;
	delete [] ones;
	delete [] onesNew;
	delete [] centerBestMinutiae;
	
	
	
	return 0;


}

