//////////////////////////////////////////////////////////////////////////
//����:         CFPOrientation.h
//������:
//����:
//�������:
//�����:
//
//��������:     ���������, ������������ ���� ���������� �� ������������ ��
//        		���������� ������      
//              
//////////////////////////////////////////////////////////////////////////
#ifndef CFPORIENTATION_H
#define CFPORIENTATION_H

class CFPOrientation
{
public:
	// �����������
	CFPOrientation();
	// ����������
	~CFPOrientation();



void getOrientation(int *Img, int nWt, int nHt, float *oImg);	// ������ ���� ���������� �����������

protected:
int getSobelGradX(int *Reg); // ���������� ������� � ����� ������ �� X
int getSobelGradY(int *Reg); // ���������� ������� � ����� ������ �� Y
void LowPassFilterDirect(int sizeWND, float *oImg, int nWt, int nHt); // �� ���������� ���� ����������



private:

};
#endif