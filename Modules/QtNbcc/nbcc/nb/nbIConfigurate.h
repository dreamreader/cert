//////////////////////////////////////////////////////////////////////////
//����:         nbIConfigurate.h
//������:
//����:
//�������:
//�����:
//
//����:       ��������� ������������ ����������.
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_ICONFIGURATE_H
#define _NB_ICONFIGURATE_H

#include "nbTypes.h"
#include "nbIEventHandler.h"

//.............................................................................
//  ��� ���������������� ����������
//  ����������:
//    ������ ������� �������� ����������� ������������������
//    ��� ���������� ���������� ������������ ����� ������� � ����� ������
//    ��� �������� ���������� ������������ ����� ����� �������
//.............................................................................

// ������������� ���������� nbIConfigurate
// {D262F70D-062D-4a50-B7C4-3DFD47CBDEE5}
static const nbUuid nbUUID_ICONFIGURATE = 
  {0xd262f70d,0x62d,0x4a50,{0xb7,0xc4,0x3d,0xfd,0x47,0xcb,0xde,0xe5}};

#undef  nbINTERFACE
#define nbINTERFACE nbIConfigurate

nbDECLARE_INTERFACE(nbIConfigurate,nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //�������� ������� ������������
  //  page  - [��] ������������� �������� ������������
  //  opt   - [��/���] ������ ������������
  nbMETHOD(setConfig)(int32_t page, nbData opt) nbPURE;

  //������ ������� ������������
  //����������: ������������� ������������ ������������ ��� ���������
  //  page  - [��] ������������� �������� ������������
  //  opt   - [��] ������ ������������
  nbMETHOD(getConfig)(int32_t page, nbData opt) nbPURE;

  //������ ������������ "�� ���������"
  //  page  - [��] ������������� �������� ������������
  //  opt   - [��/���] ������ ������������
  nbMETHOD(getDefaultConfig)(int32_t page, nbData opt) nbPURE;

  //��������� ������������
  //  handle- [��] ����������� ����, � ������� ������������ ����������
  //  page  - [��] ������������� �������� ������������
  //  opt   - [���/���] ������ ������������
  nbMETHOD(tuneUp)(nbHandle handle, int32_t page, nbData opt) nbPURE;

  //������������ ������� ������������
  //  iid   - ������������� ���������� ���������� (�������� nbUUID_NIL ���������� ����� ����� �������)
  //  count - ����� �������
  //  start - ����� ������ �������� ��� ����������
  nbMETHOD(getPageCount)(nbUuid *iid, int32_t *count, int32_t *start) nbPURE;

  //��������� �������� �������� ������������
  //  page  - [��] ��������
  //  name  - [���] ��������� �������� (����� ���� nbNULL)(�� ������ 128 ����)
  //  descr - [���] �������� �������� (����� ���� nbNULL)(�� ������ 1024 ����)
  //����������: ���� ���������� ������ ������������ ������������ ������ ������� ������
  nbMETHOD(getTitle)(int32_t page, nbData name, nbData descr) nbPURE;
};

#endif
