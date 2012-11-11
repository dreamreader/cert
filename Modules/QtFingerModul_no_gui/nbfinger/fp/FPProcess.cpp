
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


//�������������� ����������� ��������� ������ � 2 ������� ����������:
//����������� � ����������
//  img    - [��] 256-������� ����� ����������� nWt*nHt [320 * 480]
//  �param - [���] ������ ����������� ���������� [416]
//  dparam - [���] ������ ���������� ���������� [96], ����������� 1 � 0
//����������: ������� �������� ��������� �����������
bool fp_process(int img[], FPPoint minutiae[], int &numberMinutiae, float cparam[], int dparam[]){

  //�������� ������ ��� ��������� �����������
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

  //��������� �������� ��������������
  for (int32_t i=0; i<fpHWt; ++i){
    img[i] = 255 - img[i];
  }

  sObject.getSegment(img,sImg,fpWt,fpHt);                           // ���������� ������� ������������ ������, �.� ������� ������������ �������
  orObject.getOrientation(img,fpWt,fpHt, oImg);                     // ���������� ���� ����������
  //eObject.EnhanceGauss(img,enhImg,fpWt,fpHt);	                      // ��������� �������������� ������
  eObject.EnhanceMfs8600(img,oImg,enhImg,fpWt,fpHt);             // �������� �������� �����������
  eObject.ExtractRidges(enhImg,oImg,binImg,fpWt,fpHt);              // ������� ������ � ������������
  tObject.thinning(binImg,tImg,fpWt,fpHt);                          // ��������� ����������� �����
  numberMinutiae = mObject.getMinutiae(tImg,sImg,oImg,fpWt,fpHt,minutiaes); // ����� ������ �����

  for(int i=0; i<fpHWt; i++){
    if (tImg[i]>1) tImg[i]=1;
  }

  for(int i=0; i<numberMinutiae; i++){
      minutiae[i].x = minutiaes[i].x;
      minutiae[i].y = minutiaes[i].y;
      minutiae[i].direct = minutiaes[i].direct%360;
  }

  //�������� ���������� ��
  coeffs = func.WelshAdamar(minutiae,numberMinutiae);
  for (int i=0; i<fpCpNum; i++){
	cparam[i] = coeffs[i];
  }

  //�������� ���������� ����������
  func.DiskretCell(minutiae,numberMinutiae,fpBlkSize,fpBlkSize,fpWt,fpHt,dparam);


  if (numberMinutiae == 0) return true;

  return false;
}
