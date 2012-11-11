//////////////////////////////////////////////////////////////////////////
//����:         nbIUnknown.h
//������:       0.9
//����:         01.01.2010
//�������:
//�����:
//
//����:         �������� ��� ����������� nbIUnknown, ������������� ��������,
//              ���� nbResult, ����� ������, ����� ������,
//              ��������� nbUUID_IUNKNOWN
//////////////////////////////////////////////////////////////////////////

#ifndef _NB_IUNKNOWN_H
#define _NB_IUNKNOWN_H

#include "nbTypes.h"
#include "nbResult.h"

#pragma pack(1)

//.............................................................................
//  ������� �������������� �������� ����������� �� C,C++
//.............................................................................

#if defined(__cplusplus)
  #if defined(interface)
    #undef interface
  #endif
  #define interface struct//__interface

  #define nbPURE =0
  #define nbTHIS  void
  #define nbTHISc

  #define nbMETHOD(method)  virtual nbResult nbCALL method
  #define nbMETHOD_DEF(iclass,method) nbResult nbCALL iclass::method
  
  #define nbDECLARE_INTERFACE0(iface)            interface nbNOVTABLE iface
  #define nbDECLARE_INTERFACE(iface, baseiface)  interface nbNOVTABLE iface : public baseiface
  
#else
  #define interface struct

  #define PURE
  #define THIS  nbINTERFACE *this
  #define THISc nbINTERFACE *this,

  #define nbMETHOD(method)  nbResult (nbCALL *method)
  #define nbMETHOD_DEF(iclass,method) nbResult nbCALL iclass##_##method

  #define nbDECLARE_INTERFACE0(iface)\
    typedef interface iface { \
      struct iface##Vtbl * vtbl; \
    } iface; \
    typedef struct iface##Vtbl iface##Vtbl; \
    struct iface##Vtbl
  #define DECLARE_INTERFACE(iface, baseiface)    nbDECLARE_INTERFACE0(iface)
#endif

//.............................................................................
//  ��� �����������
//.............................................................................

// ������������� ���������� nbIUnknown
// {00000000-0000-0000-C000-000000000046}
static const nbUuid nbUUID_IUNKNOWN = 
  {0x00000000,0x0000,0x0000,{0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x46}};

#undef  nbINTERFACE
#define nbINTERFACE   nbIUnknown

nbDECLARE_INTERFACE0(nbIUnknown){
  
  //������ ��������� ���������� ����������
  //  iid     - [��]    ������ �� ������������� ����������
  //  icd     - [���]   ��������� �� ��������� ���������� ����������
  //����������:  nbS_OK
  //             nbE_NO_INTERFACE
  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  
  //���������� �������� ������ �� ��������� (� ������ ������������ ������ �� ����)
  //����������:  ������������� ��������
  nbMETHOD(retain)(nbTHIS) nbPURE;
  
  //������������ ����������, ����� ���������� �������� ������ �� ����
  //����������: ��������������� ��������
  nbMETHOD(release)(nbTHIS) nbPURE;
};

//.............................................................................
//  ��� ������ ����
//.............................................................................

//��������� �� ������� 'NbQueryComponent'. ������ � �������� ����������
//  cid - [��]  ������������� ����������
//  iid - [��]  ������������� ���
//  icd - [���] ��������� ���
//����������: nbS_OK, nbE_NO_INTERFACE, nbE_NO_COMPONENT, nbE_NO_MODULE, nbE_FAIL
typedef nbResult (nbCALL *NbQueryComponentPtr)(nbRefUuid cid, nbRefUuid iid, void **icd);

#pragma pack()

#endif
