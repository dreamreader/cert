/****************************************************************************
** ����:        psnReadWrite.cpp
** ������:
** ����:
** �������:
** �����:       ������� �.�.
**
** ��������:    ������������ ������ � ������� (������ � ������).
****************************************************************************/

#include "nb\nbIBimProvider.h"
#include "nbclasses\nbcTypes.h"
#include "psnReadWrite.h"
#include "exp\expTypes.h"
#include "..\nbsign\SignProcessor.h"

// ��������� ����� � ��������������� �����������
#define SIG_PARAMS 0x20504942 // const char paramSign[4]	= {'B','I','P',' '};

//////////////////////////////////////////////////////////////////////////
// �������� ���������� �������, ���������� � ��������
nbResult GetBimCount ( const uint8_t data[], uint32_t *maxCount ) {
  if(!data)
    return nbE_POINTER;

  // ������� ��������� �����
  nbBimFileHeader *header = (nbBimFileHeader*)data; // ��������� �� ��������� ������ �������
  if (header->count > *maxCount) {
    *maxCount = header->count;
    return nbE_INSUFFICIENT_BUFFER;
  }
  *maxCount = header->count;

  return nbS_OK;
}

//////////////////////////////////////////////////////////////////////////
//����������: ��������� �������������� ������ �� ����� ��������
//��������:   ������� ���������� � ���� ������ ������ ��� ������.
//            �� ���� ������� ������ ���������� �� nbBim �������� maxCount.
//            ���� maxCount ������ ���������� ������� ���������� � ���� �������, 
//            �� ���������� nbE_OUT_OF_MEMORY � � maxCount ���������� ��������
//            ���������� �������.
//            ������ ��� bimBase ���������� ������ �������.
//            ����� �������� ����� ������ �������.
//���������:
//  data     - [��]     ��������� �� ������, ���������� � ����� �������
//  maxCount - [��\���] ����������� ��������� ���������� �������
//	bimBase	 - [��\���] �������������� ������
//����������: nbS_OK, ���� �� ���������, ���� ��� ������
//
nbResult LoadBimFromResource ( const uint8_t data[], uint32_t *maxCount, nbBim **bimBase ) {
  if (!bimBase || !data)
    return nbE_POINTER;

  // ������� ��������� �����
  nbBimFileHeader *header = (nbBimFileHeader*)data;         // ��������� �� ��������� ������ �������
  nbBim* bimOne = (nbBim*)(data + sizeof(nbBimFileHeader)); // ��������� �� ������ ������� ������
  if (header->count > *maxCount) {
    *maxCount = header->count;
    return nbE_INSUFFICIENT_BUFFER;
  }
  *maxCount = header->count;

  // ������� ��������������� ��� ������, ������� ��� ��� ������
  for (uint32_t i=0; i<header->count; ++i) {
    uint32_t size = sizeof(nbBim) + bimOne->size;  // ������ ������ ������
    bimBase[i] = (nbBim*)new uint8_t[size];
    if (!bimBase[i]) return nbE_OUT_OF_MEMORY;     // !�� �������� ����� ���������� ����� ���������� ������
    memcpy(bimBase[i],bimOne,size);                // ����������� ������
    bimOne = (nbBim*)((uint8_t*)bimOne + size);    // ������� � ���������� ������
  }

  return nbS_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//����������: ��������� ������� �������� �������������� ���������� �� ����� ��������
//��������:   ������� ���������� � ���� ��������� ������� ������ ��� ������.
//            �� ���� ������� ������ ������� �������� ����. ����������.
//            ������ ��� vbpsBase ���������� ������ �������.
//            ����� �������� ����� ������ �������.
//���������:
//  data      - [��]  ��������� �� ������, ���������� � ����� �������
//	vbpsBase  - [���] ������� �������� �������������� ����������
//����������: nbS_OK, ���� �� ���������, ���� ��� ������
nbResult LoadMatrixFromResource ( const uint8_t data[], nbMatrix **vbpsBase ) {
  if (!vbpsBase)
    return nbE_POINTER;

  // ������� ��������� �����
  nbMatrixFileHeader *header = (nbMatrixFileHeader*)data;         // ��������� �� ��������� ������ �������
  if ( header->signature != SIG_PARAMS &&
       header->bimType != nbBIM_TYPE_HANDWR &&
       header->providerCid != nbUUID_SIGN_PROVIDER &&
       header->processorCid != nbUUID_SIGN_PROCESSOR) {
    return nbE_IDENTIFIER;
  }
  // ������� ��������� �����
  //nbBlockHeader *blHeader = (nbBlockHeader*)(data + sizeof(nbMatrixFileHeader));

  // ������� ������
  nbData buffer = (nbData)(data + sizeof(nbMatrixFileHeader) + sizeof(nbBlockHeader));

  // ������������� ������ � ������� ���
  if (!dataToMatrix(buffer,vbpsBase)) {
    return nbE_FAIL;
  }

  return nbS_OK;
}

