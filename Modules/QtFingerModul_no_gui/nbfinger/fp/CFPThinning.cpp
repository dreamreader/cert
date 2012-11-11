
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
//#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "CFPThinning.h"

	// ����� �������
const int	Mask1[SIZEMASK]={1,1,1,1,0,1,1,1,1};
const int	Mask2[SIZEMASK]={0,1,0,0,0,1,0,1,0};
const int	Mask3[SIZEMASK]={0,0,0,1,0,1,0,1,0};
const int	Mask4[SIZEMASK]={0,1,0,1,0,1,0,0,0};
const int	Mask5[SIZEMASK]={0,1,0,1,0,0,0,1,0};
const int	Mask6[SIZEMASK]={1,0,0,0,0,1,0,1,0};
const int	Mask7[SIZEMASK]={0,1,0,1,0,0,0,0,1};
const int	Mask8[SIZEMASK]={0,0,1,1,0,0,0,1,0};
const int	Mask9[SIZEMASK]={0,1,0,0,0,1,1,0,0};


	// ������� ���������
const int 	Tabl[SIZETRANSITION]={0,1,2,5,8,7,6,3};

	// �����������
CFPThinning::CFPThinning(){
}
	// ����������
CFPThinning::~CFPThinning(){
}


// �������� �����-���� (���������� ����� �� 1-�� �������) 
// Img [IN] - ������� �������� �����������
// tImg [OUT] - �������� ���������� �������� �����������
// nWt [IN] - ������ �����������
// nHt [IN] - ������ �����������
void CFPThinning::thinning(int *Img, int *tImg,  int nWt, int nHt)
{

int *Region;
Region=new int[SIZEMASK];


int nChange, Term;
bool NoChange;

	NoChange=false;
	while (!NoChange) 
	{
	
		nChange=0;

		// ����������� "1"
		for(int i=1; i<nHt-1; i++)
		for(int j=1; j<nWt-1; j++){


			tImg[i*nWt+j]=Img[i*nWt+j];
			if(Img[i*nWt+j]==0) continue;

			// ���������� ��������
				for(int r=-1; r<=1; r++)
					for(int c=-1; c<=1; c++)
						Region[(r+1)*3+(c+1)]=Img[(i+r)*nWt+(j+c)];
				
			

			// ������� "1"
			Term=CFPThinning::convolutionAdd(Region,Mask1);	// �������
			if((Term<3)||(Term>6)) continue;

			// ������� "2"
			Term=CFPThinning::transition01(Region,Tabl); 
			if(Term!=1) continue;

			// ������� "3"
			Term=CFPThinning::convolutionAdd(Region,Mask2);	
			if(Term==3) continue;

			// ������� "4"
			Term=CFPThinning::convolutionAdd(Region,Mask3);	
			if(Term==3) continue;

			tImg[i*nWt+j]=0; // ������� ������
			nChange++;
		}
	

		
		// ����������� "2"
		for(int i=1; i<nHt-1; i++)
		for(int j=1; j<nWt-1; j++){
		
			Img[i*nWt+j]=tImg[i*nWt+j];
			if(Img[i*nWt+j]==0) continue;
			
				// ���������� ��������
					for(int r=-1; r<=1; r++)
						for(int c=-1; c<=1; c++)
							Region[(r+1)*3+(c+1)]=tImg[(i+r)*nWt+(j+c)];
					

				// ������� "1"
				Term=CFPThinning::convolutionAdd(Region,Mask1); // �������
				if((Term<3)||(Term>6)) continue; // 

				// ������� "2"
				Term=CFPThinning::transition01(Region,Tabl); // ���������� ����� ���������
				if(Term!=1) continue;

				// ������� "3"
				Term=CFPThinning::convolutionAdd(Region,Mask4); 
				if(Term==3) continue;

				// ������� "4"
				Term=CFPThinning::convolutionAdd(Region,Mask5);
				if(Term==3) continue;

				Img[i*nWt+j]=0; // ������� ������
				nChange++;
			}
				
			if (nChange==0) NoChange=true; // ��������� ���
			
	} 

		for(int i=1; i<nHt-1; i++)
		for(int j=1; j<nWt-1; j++){
			if(Img[i*nWt+j]==0) continue;

				// ��������� ������	
					for(int r=-1; r<=1; r++){
						for(int c=-1; c<=1; c++){
								Region[(r+1)*3+(c+1)]=Img[(i+r)*nWt+(j+c)];}
						}

			// ������� "1"
			Region[0]^=1;
			Term=CFPThinning::convolutionAdd(Region,Mask6); // �������
			Region[0]^=1;
			if(Term==3){
				Img[i*nWt+j]=0; // ������� ������
					nChange++;
						continue;
			}

				// ������� "2"
			Region[8]^=1;
			Term=CFPThinning::convolutionAdd(Region,Mask7);
			Region[8]^=1;
			if(Term==3){
				Img[i*nWt+j]=0; // ������� ������
					nChange++;
					continue;
				}

				// ������� "3"
			Region[2]^=1;
			Term=CFPThinning::convolutionAdd(Region,Mask8);
			Region[2]^=1;
			if(Term==3) {
				Img[i*nWt+j]=0; // ������� ������
					nChange++;
					continue;
				}

				// ������� "4"
			Region[6]^=1;
			Term=CFPThinning::convolutionAdd(Region,Mask9);
			Region[6]^=1;
			if(Term==3){
				Img[i*nWt+j]=0; // ������� ������
					nChange++;
					continue;
				}
			}	
			
		for(int i=0; i<nWt*nHt; i++)
			tImg[i]=Img[i];
			
			delete [] Region;

}


// ������� �� �����
// m[IN] - �����
// r[IN] - ������ ���������� ������� �����������
// ������� ���������� �������� �������

inline int CFPThinning::convolutionAdd(int *r, const int *m)
{

/*

		return	(r[0]*m[0])+(r[1]*m[1])+(r[2]*m[2])+\
			(r[3]*m[3])+(r[4]*m[4])+(r[5]*m[5])+\
			(r[6]*m[6])+(r[7]*m[7])+(r[8]*m[8]);
*/		

			int sum=0;
			for(int i=0; i<SIZEMASK; i++)
				sum+=r[i]*m[i];
			
			return sum;	
		
}

// ����������� ����� ��������� 0-1 �� ������� ���������
// r[IN] - ������ ���������� ������� �����������
// t[IN] - ������� ���������
// ������� ����������  ����� ���������
inline int CFPThinning::transition01(int *r,  const int *t)
{
	int n=0;
/*	
	if(r[t[0]]<r[t[1]]) n++; 
	if(r[t[1]]<r[t[2]]) n++; 
	if(r[t[2]]<r[t[3]]) n++;
	if(r[t[3]]<r[t[4]]) n++; 
	if(r[t[4]]<r[t[5]]) n++; 
	if(r[t[5]]<r[t[6]]) n++;
	if(r[t[6]]<r[t[7]]) n++;
	if(r[t[7]]<r[t[0]]) n++;

	return n;
*/
	
	for(int i=0; i<SIZETRANSITION; i++)
		if (r[t[i]] < r[t[(i+1)%8]]) n++;
		
		return n;
	
}
