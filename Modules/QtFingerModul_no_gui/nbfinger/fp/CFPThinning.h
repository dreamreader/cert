
//////////////////////////////////////////////////////////////////////////
//����:         CFPEnhance.h
//������:
//����:
//�������:
//�����:
//
//��������:     ���������, ����������� ��������� ����� � ����������� �� 1 �������
//        		      
//              
//////////////////////////////////////////////////////////////////////////
#ifndef _CFPTHINNING_H
#define _CFPTHINNING_H

#include "FPDefine.h"

#define SIZEMASK		9 		// ������ ����� ��� �������� �������
#define SIZETRANSITION	8 		// ����� ���������

class CFPThinning
{
public:
	// �����������
	CFPThinning();
	// ����������
	~CFPThinning();
	
  void thinning(int *Img, int *tImg, int nWt, int nHt);  	// ������� ��������� ���������� ����� �� 1-�� �������

protected:
  int convolutionAdd(int *r, const int *m);                 // ������� �� �����
  int transition01(int *r,  const int *t);                 // ����������� ����� ��������� 0-1




private:

};

#endif
