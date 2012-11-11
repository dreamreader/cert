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
//.............................................................................

// ������������� ���������� nbIConfigurate
// {D262F70D-062D-4a50-B7C4-3DFD47CBDEE4}
static const nbUuid nbUUID_ICONFIGURATE = 
  {0xd262f70d,0x62d,0x4a50,{0xb7,0xc4,0x3d,0xfd,0x47,0xcb,0xde,0xe4}};

#undef  nbINTERFACE
#define nbINTERFACE nbIConfigurate

nbDECLARE_INTERFACE(nbIConfigurate,nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //�������� ������� ������������
  //  page  - [��] ������������� �������� ������������
  //  opt   - [��/���] ������ ������������
  nbMETHOD(setConfig)(nbUuid page, nbData opt) nbPURE;

  //������ ������� ������������
  //����������: ������������� ������������ ������������ ��� ���������
  //  page  - [��] ������������� �������� ������������
  //  opt   - [��] ������ ������������
  nbMETHOD(getConfig)(nbUuid page, nbData opt) nbPURE;

  //������ ������������ "�� ���������"
  //  page  - [��] ������������� �������� ������������
  //  opt   - [��/���] ������ ������������
  nbMETHOD(getDefaultConfig)(nbUuid page, nbData opt) nbPURE;

  //��������� ������������
  //  handle- [��] ����������� ����, � ������� ������������ ����������
  //  guihandler  - [���] ��������� ����������� ��������� ������������ ����
  //  page  - [��] ������������� �������� ������������
  //  opt   - [���/���] ������ ������������
  nbMETHOD(tuneUp)(nbHandle handle, nbUuid page, nbData opt, nbIEventHandler **guihandler) nbPURE;

  //������������ ������� ������������, ������� ����� ��������� ��� ���������� ����������
  //����������: ������� RaiseEvent ���������� � �����������
  //                (nbUuid*)ParamA - ������������� �������� ������������
  //                (nbData)ParamB  - ������ � ���������� ���� (�����������)
  //  eh  - ���������� ������� nbEVENT_ENUMERATE_ITEM
  //  iid - ������������� ���������� ���������� (�������� nbUUID_NIL ����������� ��� �������� �������� ����������)
  nbMETHOD(enumPages)(nbIEventHandler *eh, nbRefUuid iid) nbPURE;

  //��������� �������� �������� ������������
  //  page  - [��] ��������
  //  name  - [���] ��������� �������� (����� ���� nbNULL)
  //  descr - [���] �������� �������� (����� ���� nbNULL)
  //����������: ���� ���������� ������ ������������ ������������ ������ ������� ������
  nbMETHOD(getTitle)(nbUuid page, nbData name, nbData descr) nbPURE;
};

#endif
