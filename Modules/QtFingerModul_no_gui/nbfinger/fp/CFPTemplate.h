//////////////////////////////////////////////////////////////////////////
//����:         CFPTemplate.h
//������:
//����:
//�������:
//�����:
//
//��������:     ���������, ������������ ������ ����. ������ ��
//              
//              
//////////////////////////////////////////////////////////////////////////

#include "CFPMinutiae.h"
#include "FPDefine.h"


#ifndef CFPTEMPLATE_H
#define CFPTEMPLATE_H



typedef struct tag_FPShift{
	int dX;		// ���������
	int	dY;		// ���������
	float dR;	// �������
} FPShift;

typedef struct tag_FPRegion{
	int x;			// ���������
	int y;			// ���������
	int r;			// ������
	int direct;		// �����������
	float percent;	// �����������
} FPRegion;

typedef struct tag_FPPointF{
	int type;		// ��� �������
	int x;			// ���������
	int y;			// ���������
	float direct;   // �����������
} FPPointF;



class CFPTemplate
{

public:
	// �����������
	CFPTemplate();
	// ����������
	~CFPTemplate();

	int getTemplate ( FPPoint *allMinutiae, int *numMinutiae, int imgCount, int *rightImg, FPPoint *bestMinutiae, int *numBestMinutiae, FPPoint *templateMinutiae );
	void MatchingSpeed(FPPoint *tMinutiae, int tLen, FPPoint *sMinutiae, int sLen, float *res, float *dX, float *dY, float *dR);
	void AffineTransform(FPPoint *minutiae, int len, float dX, float dY, float dR);

protected:
	
	int FindAllTriangle(int num, int *triangle, int mode);
	int Sum(int *vector, int size);
	FPShift* MatchingSpeedTriangle(FPPoint *tMinutiae, int tLen, FPPoint *sMinutiae, int sLen, int *numShift);
	int FindBestMatching(int fpNum, float *Cmp, FPShift *Offset, FPShift *OffsetBest);
	int FindMinutiaeWin(FPPoint *minutiae, int len, FPPoint *newMinutiae, int mode);
	int FindRegionOnes(FPPoint *minutiae, int len, int fpNum, FPRegion *ones, int mode);
	int FindRegionStitching(FPRegion *region, int size, int num, FPRegion *stitch);
	int FindRegionWinThr(FPRegion *region, int num, float threshold, FPRegion *newRegion, int mode);
	int GenGrid(FPPoint *minutiae, int len, int fpNum, FPRegion *grid, int mode);
	


};

#endif  

