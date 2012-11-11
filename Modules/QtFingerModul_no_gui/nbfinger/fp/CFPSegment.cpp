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

#include <memory.h>
#include "FPDefine.h"
#include "CFPSegment.h"


//�����������
CFPSegment::CFPSegment(){

}

//����������
CFPSegment::~CFPSegment(){

}

//	����������� �������� ������������ ������
//  Img [IN] - �������� ����������� ��������� ������
// 	sImg [OUT] - ����������� �������� ��: 1 - ������� ����������� �����, 
//  0 - ���������� ����������� �����
//	nWt, nHt [IN] - ��������� �����������
//  ������� ���������� ������� ����������� ��  �� �������

float CFPSegment::getSegment(int *Img, int *sImg, int nWt, int nHt)
{

	double Sum;
	double Percent=0;
	int xBLK=int(nWt/fpBlkNum16); // ����� ������ �� X
	int yBLK=int(nHt/fpBlkNum16); // ����� ������ �� Y
	

	for(int i=0; i<yBLK; i++)
	for(int j=0; j<xBLK; j++)
	{
		Sum=0;
		for(int ii=i*fpBlkNum16; ii<i*fpBlkNum16+fpBlkNum16; ii++)
		for(int jj=j*fpBlkNum16; jj<j*fpBlkNum16+fpBlkNum16; jj++)
		{
			Sum+=Img[ii*nWt+jj];
		}
		Sum/=fpBlkNum16*fpBlkNum16;

		if((Sum > 50) && (Sum < 220))
		{
			sImg[i*xBLK+j]=1; 
			Percent+=1;
		}
		else
			sImg[i*xBLK+j]=0;
	}

	CFPSegment::Morphology(sImg, nWt, nHt);

	return Percent/((xBLK*yBLK)/100);
}

//	������ ��������
// 	sImg [IN/OUT] - ����������� �������� ��
//	nWt, nHt [IN] - ��������� �����������
void CFPSegment::Morphology(int *sImg, int nWt, int nHt)
{

	int xBLK=int(nWt/fpBlkNum16); // ����� ������ �� X
	int yBLK=int(nHt/fpBlkNum16); // ����� ������ �� Y
	int sum;
	int *SegmentImg=new int[xBLK*yBLK];
	memset(SegmentImg,0,xBLK*yBLK*sizeof(int));
	
	// ������ �������� ������
	for(int i=1; i<yBLK-1; i++)
	for(int j=1; j<xBLK-1; j++)
	{
		sum =sImg[(i-1)*xBLK+(j-1)];
		sum+=sImg[(i-1)*xBLK+(j  )];
		sum+=sImg[(i-1)*xBLK+(j+1)];
		sum+=sImg[(i  )*xBLK+(j+1)];
		sum+=sImg[(i+1)*xBLK+(j+1)];
		sum+=sImg[(i+1)*xBLK+(j  )];
		sum+=sImg[(i+1)*xBLK+(j-1)];
		sum+=sImg[(i  )*xBLK+(j-1)];

		if(sum>=6) SegmentImg[i*xBLK+j]=1; else SegmentImg[i*xBLK+j]=0;
	}

	for(int i=0; i<xBLK*yBLK; i++) sImg[i]=SegmentImg[i];
	
	delete [] SegmentImg; 
}

