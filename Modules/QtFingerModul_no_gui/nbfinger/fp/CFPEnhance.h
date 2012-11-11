//////////////////////////////////////////////////////////////////////////
//����:         CFPEnhance.h
//������:
//����:
//�������:
//�����:
//
//��������:     ���������, ����������� ��������� ��������� ����������� ���
//      		��������� ��� ��������  		     
//              
//////////////////////////////////////////////////////////////////////////
#ifndef CFPENHANCE_H
#define CFPENHANCE_H

class CFPMinutiae;


#define CountPointVector	31 // ����� ������� ��������

typedef struct tag_RidgesVector{
	int x[CountPointVector];
	int y[CountPointVector];

} RidgesVector;

typedef struct tag_PositionVector{
	RidgesVector	xPosition;
	RidgesVector	yPosition;


} PositionVector;

class CFPEnhance
{
public:
	// �����������
	CFPEnhance();
	// ����������
	~CFPEnhance();

friend class CFPMinutiae;
void EnhanceMfs8600(int *Img, float *oImg, int *enhImg, int nWt, int nHt); // ��������� �������� �����������
void EnhanceGauss(int *Img, int *enhImg, int nWt, int nHt); // ������ ������
void ExtractRidges(int *Img, float *oImg,int *bImg, int nWt, int nHt); // 	���������� �������


protected:
float getDirectPixel( float *oImg, int RowPixel, int ColPixel, int nWt, int nHt); // ����������� ���������� ����������� � �����
int getSegmentPixel(int *sImg, int RowPixel, int ColPixel, int nWt, int nHt); // ���������� ��������� �� ����������� ���� � �����
int getNumFromDirect(float Direct); // ���������� ������������� �����������
void Morf2(int *bImg, int nWt, int nHt); //	���������� ��������������� �����������
void GeneratorXYPosition(PositionVector *Position, int nPosition); // ��������� ��������

private:



};
#endif
