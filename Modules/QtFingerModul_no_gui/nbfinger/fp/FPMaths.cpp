//////////////////////////////////////////////////////////////////////////
//файл:         FPMaths.cpp
//версия:
//дата:
//изменен:
//автор:				
//
//описание:     Модуль, поддерживающий функции обработки полученного изображения
//////////////////////////////////////////////////////////////////////////

#include <qstring.h>
#include "FPDefine.h"
#include "FPMaths.h"

//////////////////////////////////////////////////////////////////////////
// Класс, поддерживающий вычисления для определения качества образа

// Конструктор
FPMaths::FPMaths() { }
// Деструктор
FPMaths::~FPMaths() { }


//назначение: определить "хороший" образ или "плохой"
//параметры:
//	img - [out] изображение отпечатка пальца
//возвращает true если образ "хороший", иначе - false 
FingerprintAction FPMaths::estimateUserAction(uint8_t img[], uint8_t mask[])
{
  uint32_t lt, rt, lb, rb;  //число 1 в квадрантах
  float plt, prt, plb, prb; //часть изображения в квадрантах
	
  //Выделить маску изображения, состоящую из 0(пусто) и 1 (есть)
	fp_Segment(img, mask);

  quadrants(mask, &lt, &rt, &lb, &rb);

  //Проверить процент заполненности изображения
  if ((float)(lt+rt+lb+rb)/fpHWt < 0.35f) return fpaIncreasePressure;

  plt = (float)lt/(lt+rt+lb+rb);
  prt = (float)rt/(lt+rt+lb+rb);
  plb = (float)lb/(lt+rt+lb+rb);
  prb = (float)rb/(lt+rt+lb+rb);

  //Проверить вращение
  if (((plt+prb) - (plb+prt)) > 0.1f) return fpaClockwise;
  if (((plb+prt) - (plt+prb)) > 0.1f) return fpaClockwise;

  //Проверить сдвиги
  if (((plt+plb) - (prt+prb)) > 0.2f) return fpaRight;
  if (((prt+prb) - (plt+plb)) > 0.2f) return fpaLeft;

  if (((plb+prb) - (plt+prt)) > 0.3f) return fpaUp;
  if (((plt+prt) - (plb+prb)) > 0.3f) return fpaDown;

	return fpaNone;
}

//Подсчет числа точек в каждом из квадрантов
void FPMaths::quadrants(uint8_t mask[], uint32_t *lt, uint32_t *rt, uint32_t *lb, uint32_t *rb){
  const uint32_t fpHt2 = fpHt/2;
  const uint32_t fpWt2 = fpWt/2;
  uint32_t x,y;

  uint32_t q00=0,q01=0,q10=0,q11=0;

  //Подсчет в левом верхнем квадранте
  for (y=0; y<fpHt2; y++)
    for (x=0; x<fpWt2; x++){
      q00 += mask[y*fpWt+x];
      q01 += mask[y*fpWt+x+fpWt2];
      q10 += mask[(y+fpHt2)*fpWt+x];
      q11 += mask[(y+fpHt2)*fpWt+x+fpWt2];
    }
  *lt = q00; *rt=q01; *lb=q10; *rb=q11;
}


// Что-то делает и что-то возвращает
// img	- [in]
// sImg - [out]
// возвращает процент области отпечатка пальца на всём изображении
void FPMaths::fp_Segment(uint8_t img[], uint8_t sImg[])
{
	for(uint32_t i = 0; i < fpHWt; i++) {
		sImg[i] = img[i];
	}

	//Основная часть алгоритма
	for(uint32_t i = 0; i < 4 ; i++) {
		imerode(sImg, 5);
	}

	im2bw(sImg, 1, 1);
	bwareaopen(sImg, (uint32_t)(0.15f*fpHWt), 8);
	imerode(sImg, 7);
	bwfill(sImg);
	bwareaopen(sImg, (uint32_t)(0.15f*fpHWt), 8);

	//Дополнение к алгоритму

	//справа
	for(uint32_t i = 0; i < fpHt; i++) {
		for(uint32_t j = 0; j <= fpOverlap6; j++) {
			sImg[i*fpWt + j] = 0;
		}
	}

	//слева
	for(uint32_t i = 0; i < fpHt; i++) {
		for(uint32_t j = fpWBlk12 * fpBlkNum12 + fpOverlap6; j < fpWt; j++) {
			sImg[i*fpWt + j] = 0;
		}
	}

	//сверху
	for(uint32_t i = 0; i < fpWt; i++) {
		for(uint32_t j = 0; j <= fpOverlap6; j++) {
			sImg[j*fpWt + i] = 0;
		}
	}

	//снизу
	for(uint32_t i = 0; i < fpWt; i++) {
		for(uint32_t j = fpHBlk12 * fpBlkNum12 + fpOverlap6; j < fpHt; j++) {
			sImg[j*fpWt + i] = 0;
		}
	}
}

// Что-то делает и ничего не возвращает
void FPMaths::imerode(uint8_t img[], int32_t SE)
{
	uint8_t value, *img2;
	int32_t d_i, d_j, p_i, p_j, iBegin, iEnd, iOffSet;

	img2 = new uint8_t[fpHWt];
	for(uint32_t i = 0; i < fpHWt; i++){
		img2[i] = img[i];
	}
	memset(img, 255, sizeof(uint8_t)*fpHWt);

	iEnd = SE / 2;
	iBegin = (-1) * iEnd;

	for(uint32_t i = 0; i < fpHt; i++)
	{
		for(uint32_t j = 0; j < fpWt; j++)
		{
			value = img2[i*fpWt+j];

			for(d_i = iBegin; d_i <= iEnd; d_i++)
			{
				p_i = i + d_i;
				if((p_i < 0) || (p_i >= fpHt))
					continue;

				for(d_j = iBegin; d_j <= iEnd; d_j++)
				{
					p_j = j + d_j;
					if((p_j < 0) || (p_j >= fpWt))
						continue;

					//imerode
					iOffSet = p_i*fpWt+p_j;

					if (img[iOffSet] <= value)
						continue;

					if(img2[iOffSet] > value)
						img[iOffSet] = value;
					else
						img[iOffSet] = img2[iOffSet];
				}
			}
		}
	}

	delete [] img2;
}


// Что-то делает и ничего не возвращает
void FPMaths::im2bw(uint8_t img[], uint8_t scale, uint8_t inv)
{
	int32_t sign;
	uint8_t th;

	if(scale == 0) scale = 1;
	if(inv > 0)	inv = 1;
	sign = 2*inv-1;

	th = graythresh(img);

	for(uint32_t i = 0; i < fpHWt; i++)
	{
		if(img[i] >= th)
			img[i] = (scale*inv) - (scale*sign);
		else
			img[i] = (scale*inv) - 0;
	}
}

// Что-то делает и ничего не возвращает
void FPMaths::bwareaopen(uint8_t img[], uint32_t th, uint32_t conn)
{
	int32_t dR8[8] = {-1, -1, -1,  0,  0,  1,  1,  1};
	int32_t dC8[8] = {-1,  0,  1, -1,  1, -1,  0,  1};
	int32_t dR4[4] = {-1,  0,  0,  1};
	int32_t dC4[4] = { 0, -1,  1,  0};
	int32_t *dR;
	int32_t *dC;

	int32_t  offset;
	uint32_t begin, end, *Segment;

	Segment = new uint32_t[fpHWt];

	//Подготовка к выполнению алгоритма
	if(conn <= 6) {
		conn = 4;
		dR = dR4;
		dC = dC4;
	} else {
		conn = 8;
		dR = dR8;
		dC = dC8;
	}

	//края изображения - фон
	for(uint32_t i = 0; i < fpHt; i++) {
		img[i*fpWt] = 0;
		img[i*fpWt+(fpWt-1)] = 0;
	}

	for(uint32_t i = 0; i < fpWt; i++) {
		img[i] = 0;
		img[(fpHt-1)*fpWt+i] = 0;
	}

	//цикл по всем эл.
	for(uint32_t i = 0; i < fpHWt; i++)
	{
		//поиск элемента со значением "1" (т.е. эл. еще не принадлежащего
		//ни к одному найденному сегменту)
		if(img[i] != 1)
			continue;

		img[i] = 2; // - первый элемент нового сегмента "2"
		Segment[0] = i;
		begin = 0;
		end = 0;

		//выполняется пока не будут найденны все эл. связанные с img(i)
		while(begin <= end)
		{
			//Опр. есть ли в окрестности другие эл.
			for(uint32_t j = 0; j < conn; j++)
			{
				offset = Segment[begin] + dR[j]*fpWt + dC[j];
				if(img[offset] == 1)
				{
					end++;
					img[offset] = 2;
					Segment[end] = offset;
				}
			}
			begin++;
		}

		//Если найденный сегмент меньше порога "Th", тогда его удаляем
		if(end+1 < th)
			for(uint32_t j = 0; j <= end; j++)
				img[Segment[j]] = 0;
	}

	//Готово
	for(uint32_t i = 0; i < fpHWt; i++) {
		img[i] /= 2;
	}

	delete [] Segment;
}

// Что-то делает и ничего не возвращает
void FPMaths::bwfill(uint8_t img[])
{
	int32_t dR[4] = {-1,  0,  0,  1};
	int32_t dC[4] = { 0, -1,  1,  0};
	int32_t offset;
	uint32_t begin, end, *Segment;

	Segment = new uint32_t[fpHWt];

	//Подготовка к выполнению алгоритма
	//края изображения - фон
	for(uint32_t i = 0; i < fpHt; i++)
	{
		img[i*fpWt+0] = 255;
		img[i*fpWt+(fpWt-1)] = 255;

		img[i*fpWt+1] = 0;
		img[i*fpWt+(fpWt-2)] = 0;
	}

	for(uint32_t i = 0; i < fpWt; i++)
	{
		img[i+0] = 255;
		img[(fpHt-1)*fpWt+i] = 255;

		if((i == 0) || (i == fpWt))
			continue;

		img[i+1] = 0;
		img[(fpHt-2)*fpWt+i] = 0;
	}

	//Начало выполнения алгоритма
	img[fpWt+1] = 255; // - первый элемент нового сегмента "2"
	Segment[0] = fpWt+1;
	begin = 0;
	end = 0;

	//выполняется пока не будут найденны все эл. связанные с img[fpWt+1]
	while(begin <= end)
	{
		//Опр. есть ли в окрестности другие эл.
		for(uint32_t j = 0; j < 4; j++)
		{
			offset = Segment[begin] + dR[j]*fpWt + dC[j];
			if(img[offset] == 0)
			{
				end++;
				img[offset] = 255;
				Segment[end] = offset;
			}
		}

		begin++;
	}

	//Готово
	for(uint32_t i = 0; i < fpHWt; i++){
		if(img[i] == 255)
			img[i] = 0;
		else
			img[i] = 1;
	}

	delete [] Segment;
}

// Что-то делает и ничего не возвращает
uint8_t FPMaths::graythresh(uint8_t img[])
{
	const uint32_t razmer = 256;
	float fP[razmer], fW[razmer], fM[razmer], sigma_b[razmer], val, mt, maxVal;

	val = float( 1 / (fpHWt) );
	memset(fP, 0, sizeof(float)*razmer);
	memset(fW, 0, sizeof(float)*razmer);
	memset(fM, 0, sizeof(float)*razmer);

	for(uint32_t i = 0; i < fpHWt; i++) {
			fP[img[i]] += 1;
	}

	fP[0] /= (float)(fpHWt);
	fW[0] = (float)(fP[0] + F_EPS);
	fM[0] = fP[0];
	for(uint32_t i = 1; i < razmer; i++) 
	{
		fP[i] /= (float)(fpHWt);
		fW[i] = fW[i-1] + fP[i];
		fM[i] = fM[i-1] + fP[i] * (i*2);
	}

	uint8_t th = 0;
	maxVal = 0;
	mt = fM[razmer-1];
	for(uint32_t i = 0; i < razmer; i++) 
	{
		sigma_b[i] = mt * fW[i] - fM[i];
		sigma_b[i] *= sigma_b[i];
		sigma_b[i] /= fW[i] * (1 - fW[i]);
		if(sigma_b[i] > maxVal)
		{
			maxVal = sigma_b[i];
			th = i;
		}
	}

	return th;
}

