
#include <memory.h>
#include "../nb/nb.h"
#include "FPDefine.h"
#include "CFPOrientation.h"
#include "CFPEnhance.h"
#include "CFPThinning.h"
#include "CFPMinutiae.h"
#include "CFPSegment.h"
#include "CFunctional.h"
#include "FPFft.h"


//Преобразование изображения отпечатка пальца в 2 вектора параметров:
//непрерывный и дискретный
//  img    - [вх] 256-цветное серое изображение nWt*nHt [320 * 480]
//  сparam - [вых] вектор непрерывных параметров [416]
//  dparam - [вых] вектор дискретных параметров [96], принимающих 1 и 0
//Возвращает: признак успешной обработки изображения
bool fp_process(int img[], FPPoint minutiae[], int &numberMinutiae, float cparam[], int dparam[]){

  //Выделить буферы для обработки изображений
  CFPOrientation orObject;
  CFPEnhance eObject;
  CFPThinning tObject;
  CFPMinutiae mObject;
  CFPSegment sObject;
  CFunctional func;
  FPPoint minutiaes[fpMtsNum];
  float *coeffs;



  static float oImg[fpHWBlk16];
  static int   sImg[fpHWBlk16];
  static int   enhImg[fpHWt];
  static int   binImg[fpHWt];
  static int   tImg[fpHWt];
  static float otsG[fpHt-fpOverlap];
  static float otsV[fpWt-fpOverlap];

  memset(sImg,0,sizeof(sImg));
  memset(oImg,0,sizeof(oImg));
  memset(enhImg,0,sizeof(enhImg));
  memset(binImg,0,sizeof(binImg));
  memset(tImg,0,sizeof(tImg));
  memset(otsG,0,sizeof(otsG));
  memset(otsV,0,sizeof(otsV));

  //Выполнить основные преобразования
  for (int32_t i=0; i<fpHWt; ++i){
    img[i] = 255 - img[i];
  }

  sObject.getSegment(img,sImg,fpWt,fpHt);                           // Определить сегмент приложенного пальца, т.е наличие папиллярного рисунка
  orObject.getOrientation(img,fpWt,fpHt, oImg);                     // Определить поля ориентаций
  //eObject.EnhanceGauss(img,enhImg,fpWt,fpHt);	                      // Выполнить преобразование Гаусса
  eObject.EnhanceMfs8600(img,oImg,enhImg,fpWt,fpHt);             // Улучшить качество изображения
  eObject.ExtractRidges(enhImg,oImg,binImg,fpWt,fpHt);              // Извлечь гребни и бинаризовать
  tObject.thinning(binImg,tImg,fpWt,fpHt);                          // Утоньшить папиллярные линии
  numberMinutiae = mObject.getMinutiae(tImg,sImg,oImg,fpWt,fpHt,minutiaes); // Найти особые точки

  for(int i=0; i<fpHWt; i++){
    if (tImg[i]>1) tImg[i]=1;
  }

  for(int i=0; i<numberMinutiae; i++){
      minutiae[i].x = minutiaes[i].x;
      minutiae[i].y = minutiaes[i].y;
      minutiae[i].direct = minutiaes[i].direct%360;
  }

  //Получить функционал УА
  coeffs = func.WelshAdamar(minutiae,numberMinutiae);
  for (int i=0; i<fpCpNum; i++){
	cparam[i] = coeffs[i];
  }

  //Получить дискретный функционал
  func.DiskretCell(minutiae,numberMinutiae,fpBlkSize,fpBlkSize,fpWt,fpHt,dparam);


  if (numberMinutiae == 0) return true;

  return false;
}
