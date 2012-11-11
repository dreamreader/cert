//////////////////////////////////////////////////////////////////////////
//����:         nbIComponentDispatcher.h
//������:
//����:
//�������:
//�����:
//
//����:         �������� ��� ��������� �����������
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_ICOMPONENT_DISPATCHER_H
#define _NB_ICOMPONENT_DISPATCHER_H

#include "nbTypes.h"
#include "nbIEventHandler.h"

#pragma pack(1)

//.............................................................................
//  ��� ��������� �����������
//.............................................................................

// ������������� ���������� nbIComponentDispatcher
// {6D5A8E41-158A-4718-A090-DDF4E49BC8B6}
static const nbUuid nbUUID_ICOMPONENT_DISPATCHER = 
  {0x6D5A8E41,0x158a,0x4718,{0xa0,0x90,0xdd,0xf4,0xe4,0x9b,0xc8,0xb6}};

#undef  nbINTERFACE
#define nbINTERFACE   nbIComponentDispatcher

nbDECLARE_INTERFACE(nbIComponentDispatcher,nbIUnknown) {

  //������ ��������� ���������� ����������
  //  iid     - [��]    ������ �� ������������� ����������
  //  icd     - [���]   ��������� �� ��������� ���������� ����������
  //����������: nbS_OK
  //            nbE_NO_INTERFACE
  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;

  //���������� �������� ������ �� ��������� (� ������ ������������ ������ �� ����)
  //����������: ������������� ��������
  nbMETHOD(retain)(nbTHIS) nbPURE;

  //������������ ����������, ����� ���������� �������� ������ �� ����
  //����������: ��������������� ��������  
  nbMETHOD(release)(nbTHIS) nbPURE;

  //����������� ���������� � �������
  //���������:
  //  cid     - [��] ������������� ����������
  //  coInfo  - [��] �������� ����������, ������� ���� � ������ ����������
  //����������: nbS_OK
  //            nbE_OBJECT_ALREADY_EXISTS
  //            nbE_PARAM
  //            nbE_ACCESS_DENIED
  //            nbE_OBJECT_NOT_FOUND
  nbMETHOD(registerComponent)(nbTHISc nbRefUuid cid, const nbData coInfo, const nbData key) nbPURE;

  //������������� ���������� � �������
  //���������:
  //  cid   - [��] ������������� ����������
  //����������: nbS_OK
  //            nbE_PARAM
  //            nbE_NO_COMPONENT
  //            nbE_ACCESS_DENIED
  //            nbE_OBJECT_NOT_FOUND
  nbMETHOD(unregisterComponent)(nbTHISc nbRefUuid cid, const nbData key) nbPURE;
  
  //������ �� �������� � �������� ����������
  //���������:
  //  �id   - [��] ������������� ����������
  //  iid   - [��] ������������� ����������
  //  icd   - [���] ��������� ���
  //����������: nbS_OK
  //            nbE_PARAM
  //            nbE_NO_MODULE
  //            nbE_NO_COMPONENT
  //            nbE_NO_INTERFACE
  //            nbE_INTEGRITY_FAIL
  nbMETHOD(queryComponent)(nbTHISc nbRefUuid cid, nbRefUuid iid, void **icd) nbPURE;
  
  //������ ���������� � ����������
  //  cid     - [��]  ������������� ����������
  //  coInfo  - [���] ��������� �� ����������� �������������� ����
  //����������: nbS_OK
  //            nbE_PARAM
  //            nbE_NO_COMPONENT
  //            nbE_INSUFFICIENT_BUFFER
  nbMETHOD(queryComponentInfo)(nbTHISc nbRefUuid cid, nbData *coInfo) nbPURE;

  //������������ ����������� � �������� �����������, ������������������ � �������
  //��������:   �������� ������� RaiseEvent ����������� �������, ��������� ��� *cid � �������� paramA.
  //            ���� iid==nbUUID_NULL ������������� ��� ������������������ � ������� ����������.
  //            �������� ����������� nbS_OK ���������� �������, �������� nbS_CANCEL ����������.  
  //  iid     - [��] ������������� ��������� ���������� ��� nbUUID_NULL, ���� ����� ����������� ��� ����������
  //  handler - [��] ��������� ��� ���������� ���������
  //����������: nbS_OK
  //            nbE_PARAM
  //            nbE_NO_COMPONENT
  //            nbE_CANCELLED
  nbMETHOD(enumComponents)(nbTHISc nbRefUuid iid, nbIEventHandler *eh) nbPURE;
  
  //��������� ������ ��� ����� ������
  nbMETHOD(allocData)(nbTHISc nbData *data, uint32_t size);
  //������������ ����� ������, ����������� �����������
  nbMETHOD(releaseData)(nbTHISc nbData data);
};

//.............................................................................
//  ���
//.............................................................................

//��������� �� ������� 'NbEnumComponents'. ����������� ���������� � ����
//��������� � ������ ����������� � �������������� ��� �����������.
//����������:
//  � ������� RaiseEvent � �������� ���������� ����������
//    (const nbUuid*) paramA  - ������������� ����������
//    (nbData)        paramB  - ���� ������ � ������� xml (UTF-8) � ����������� � ����������
//  key - [��] ��������������� ���� (������������ ��� ������ �� �������������������� ��������)
//����������: nbS_OK
typedef nbResult (nbCALL *NbEnumComponentsPtr)(nbIEventHandler *eh);

//�������� ����������� �������� ������
//����������: nbS_OK, ���� ������ �� ������������ � ��� ����� ���������
//            ������ �������� ������, ��� ������� ���������� ������
//            (��������� � �������� ������� �������� ������ ���������� 1)
typedef nbResult (nbCALL *NbCanUnloadPtr)();

#pragma pack()

#endif
