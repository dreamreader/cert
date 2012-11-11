
#ifndef CFUNCTIONAL_H
#define CFUNCTIONAL_H

#include "CFPMinutiae.h"


// ����� �������� ����������� ��� ���������� �������������

class CFunctional
{

public:
	// �����������
	CFunctional();
	// ����������
	~CFunctional();
	
  float	otsGoriz( FPPoint *points, int nam, int sv, int sn );   // �������������� �������
  float	otsVertik( FPPoint *points, int nam, int sv, int sn );  // ������������ �������
  float otsGoriz2( FPPoint *points, int nam, int sv, int sn ); // ���� �����������
  float otsVertik2( FPPoint *points, int nam, int sv, int sn ); // ���� �����������

  void DiskretCell (FPPoint *pt, int numberMinutiae, int xSizeCell, int ySizeCell, int nWt, int nHt, int *d);

/*void CFunctional::Interpolation(nbPointHw *Point, int NumberPoint, nbPointHw *IPoint); // ������������ �������
int CFunctional::SdvigVnoll(nbPointHw *Point, nbPointHw *IPoint); // ����� ������� � ����*/

  //��������������� ������� �� 512 ���������
  // pt       - [��]  ������� �� ������������� ����� ���������
  // ptCount  - [��]  ����� ��������� � �������
  // pt2      - [���] �������������� ������� �� 512 ���������
  // shift    - [��] �������� ����� � ��������� pt � pt2
  void scaleFpTo512ShiftX(const float *pt,  int ptCount, int shift, int *pt2);
  
  // �������������� �����-������� 
  // ���������� 256 �������������
  float* WelshAdamar( FPPoint *points, int nam);
 
  


};

#endif
