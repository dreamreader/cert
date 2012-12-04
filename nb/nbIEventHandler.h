///////////////////////////////////////////////////////////////////////////////
//����:         nbIEventHandler.h
//������:       0.9
//����:         01.01.2010
//�������:
//�����:
//
//����:         �������� ��� ���������� ������� nbIEventHandler,
//              ���� nbEvent, ��������� nbUUID_IEVENT_HANDLER
///////////////////////////////////////////////////////////////////////////////

#ifndef _NB_IEVENTHANDLER_H
#define _NB_IEVENTHANDLER_H

#include "nbTypes.h"
#include "nbIUnknown.h"

//!.............................................................................

//�������
//  ������������ ��� ����������� � ��������, ������������ ������ �����������,
//  ������ �����������. ������ ��������� ����� ���������� ���� ����� �������.
//  ������ ������� [0x00000000..0x0000FFFF] ���������������.
typedef int32_t  nbEvent;

//�������: ������ ������������
//  int32_t paramA - (�������) ���������������� ��� ���������� ����� ������� ��� 0.
//  void*   paramB - (�������) ���������������� ��� ��������� �� ������������� ������.
//                    ��� ������� ������ ���� �������� �� ������ ��������� ����������� ���������.
//  ����������: nbS_OK, nbS_CANCEL
#define nbEVENT_ENUMERATE_ITEM                      (0x00000001)  //������ ������������

//�������: ��������� ������������ ���� (��������� ������� �� �� � ��������� ����������)
//  void* paramA  - (��������)  ��������� �� ������ ���������� Callback 
//                              ������� ������� ��������� ���������
//  void* paramB  - () ��������������
#define nbEVENT_WINDOW_MESSAGE                      (0x00000002)  //��������� �������

//.............................................................................
//  ��� ���������� �������
//.............................................................................

// ������������� ���������� nbIEventHandler
// {585A9237-8D9D-4edb-AB24-8F41C9A22AC6}
static const nbUuid nbUUID_IEVENTHANDLER = 
  {0x585A9237,0x8D9D,0x4edb,{0xab,0x24,0x8f,0x41,0xc9,0xa2,0x2a,0xc6}};

#undef  nbINTERFACE
#define nbINTERFACE nbIEventHandler

nbDECLARE_INTERFACE(nbIEventHandler, nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //����� �������
  //  evt     - [��]      �������
  //  sender  - [��]      �������� �������
  //  paramA  - [��/���]  �������� A (������ �������)
  //  paramB  - [��/���]  �������� B (������ ��������)
  nbMETHOD(handleEvent)(nbTHISc nbEvent evt, nbHandle sender, const nbData paramA, nbData paramB) nbPURE;
};

#endif
