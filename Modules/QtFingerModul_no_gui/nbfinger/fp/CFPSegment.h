//////////////////////////////////////////////////////////////////////////
//����:         CFPSegment.h
//������:
//����:
//�������:
//�����:
//
//��������:     ���������, ������������ ������� ������������ ��     		   
//              
//////////////////////////////////////////////////////////////////////////
#ifndef CFPSEGMENT_H
#define CFPSEGMENT_H

class CFPSegment
{
public:
	// �����������
	CFPSegment();
	// ����������
	~CFPSegment();
	
float getSegment(int *Img, int *sImg, int nWt, int nHt); // ����������� �������� ������������ ������

protected:
void Morphology(int *sImg, int nWt, int nHt); // ���������� ��������

private:

};

#endif