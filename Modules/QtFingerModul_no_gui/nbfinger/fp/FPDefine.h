//FPDifine.h: ���������� ��������� ��� ��������� �������

#ifndef FP_DEFINE_H
#define FP_DEFINE_H

#define _USE_MATH_DEFINES
#include <math.h>

#undef    F_PI
#define   F_PI  ((float)M_PI)

#define   F_PI_180  (F_PI/180.0f)
#define	  F_EPS			0.00000000000000022204f	// - �������� -> 0

//-----------------------------------------------------------------------------

#define		fpWt		  320	// ������ ����������� (����� �����)
#define		fpHt			480	// ������ ����������� (����� �����)
#define   fpHWt     (fpWt*fpHt) // ������ ����������� (����� �����)

#define		fpBlkNum16 16	// ������ ����� ���� ����������
#define   fpBlkSize  40  // ������ ����� �����������


#define   fpHBlk16  (fpHt/fpBlkNum16)
#define   fpWBlk16  (fpWt/fpBlkNum16)
#define   fpHWBlk16 (fpHBlk16*fpWBlk16) // ����� ������ � �����������

#define   fpCpNum 256     // ����� ����������� ���������� (������������� �����-�������)
#define   fpDpNum (fpHWt/(fpBlkSize*fpBlkSize)) // ����� ���������� ����������

#define   fpOverlap   40  // ������ ������� ����������
#define   fpMtsNum    300 // ������������ ����� �������������� �������
#define   fpResThr    0.09 // ������������ ����� �������������� �������

//-----------------------------------------------------------------------------

#define		fpBlkNum12  12	                            // ������ ����� 12
#define		fpOverlap6  6	                              // ������ ���������� 6
#define		fpHBlk12   (uint32_t)((fpHt-2*fpOverlap6)/fpBlkNum12)	//39
#define		fpWBlk12	  (uint32_t)((fpWt-2*fpOverlap6)/fpBlkNum12)	//25
#define		fpBlk12R(r) (uint32_t)((r-fpOverlap6)/fpBlkNum12)
#define		fpBlk12C(c) (uint32_t)((c-fpOverlap6)/fpBlkNum12)

// ������� �� ������ � ������� ����
#define RadianToAngle(r) (r/0.01745)
// ������� � �������� ����
#define AngleToRadian(r) (r*0.01745)

#endif //FP_DEFINE_H
