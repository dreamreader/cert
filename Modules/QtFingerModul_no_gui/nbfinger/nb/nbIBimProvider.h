///////////////////////////////////////////////////////////////////////////////
//����:         nbIBimProvider.h
//������:       
//����:         
//�������:
//�����:
//
//����:         �������� ��� ��������� �������������� ������� nbBimProvider,
//              ���� nbPurpose, ���� nbBim, ����������� �������,
//              ��������� nbUUID_BIM_PROVIDER
///////////////////////////////////////////////////////////////////////////////
#ifndef _NB_IBIMPROVIDER_H
#define _NB_IBIMPROVIDER_H

#include "nbTypes.h"
#include "nbIEventHandler.h"

#pragma pack(1)

//.............................................................................

//�������������� �����
//  ���������� ��������� ������� �� ����������, 
//  �� ������ 2 ���� ������ ���� � �� ��.
typedef struct nbBim_t {
  uint32_t  size;                     // ����� ����� (� ������) ��������������� ������
                                      // (��� ����� ����� ��������� nbBim)
  nbUuid    bimType;                  // ��� ��������������� ������
  //  ������
} nbBim;

//.............................................................................
//
//������������ �����, ��������� �������������� ������
//
// *.bim
//
//  signature sess_id puporse multipart (parttype (if0x0000)(provider_cid count bim[count]))[multipart]
//
//  signature        = "BIM" = 0x204d4942;
//  sess_id          = nbUuid ������������� ������
//  purpose          = uint8_t ���� ����� �������
//  multipart        = uint8_t ����� ������ � �����
//  parttype         = uint16_t ��� ����� (0x0000) ��� �������������� �������
//    provider_cid     = nbUuid ������������� ����������
//    count            = uint32_t (����� �������������� �������)
//    ����� ��������������� ���� �� ������ ���� "����" �������������� �������
//    bim[count]
//    ...
//  parttype
//  ...
//.............................................................................

//��������� ����� �������������� �������
typedef struct nbBimFileHeader_t {
  uint32_t  signature;  // ��������� BIM 
  nbUuid    sess_id;    // ������������� �����, �� ����� ������� �������� ������
  nbPurpose purpose;    // ���� ����� �������
  uint8_t   multipart;  // ����� ������
  //������������ ������ �����
  uint16_t  parttype;   // ��� ����� (����� 0x0000 ��� ��������������� ������)
  nbUuid    providerCid;// ������������� ����������
  uint32_t  count;      // ����� ������� �������
} nbBimFileHeader;


typedef int32_t nbBimInputMode;

#define nbBIM_HIDDEN              (0x00000001)  // ����� �������� ����� ������
#define nbBIM_AUTO_CORRECTION     (0x00000002)  // � ���������� ��������� ������ (����� ���������� ����������� �� ���������)
#define nbBIM_FILTER_BAD_QUALITY  (0x00000003)  // �� ���������� ������ � �������� ������ ��������� (��������)

//.............................................................................

//  �������

//�������: ������ ����� ������
//  IBimProvider* sender
//  nbBim* paramA - (�������) ��������� �� �������, � ������� ������������ �����
//  nbData paramB - (�������) nbNULL
//����������: nbS_OK, nbS_CANCEL
#define nbEVENT_BIM_STARTED             (0x00000100)

//�������: �������������� ����� ������������� (���������)
//  IBimProvider* sender
//  nbBim* paramA - (�������) ��������� �� �������, � ������� ������������ �����
//  int32_t paramB- (�������) ��������� ��������� ��� ������ 0..max(int32_t)
//����������: nbS_OK, nbS_CANCEL
#define nbEVENT_BIM_PROGRESS            (0x00000101)

//�������: ����� ����� ������, ����� ������ ���������
//  IBimProvider* sender
//  nbBim* paramA - (�������) ��������� �� �������, � ������� ������������ �����
//  nbData paramB - (�������) nbNULL
//����������: nbS_OK, nbS_CANCEL
#define nbEVENT_BIM_READY               (0x00000103)

//�������: ���� ������ ������� ����� (��� �� �������)
//  IBimProvider* sender
//  nbBim* paramA - (�������) nbNULL
//  nbData paramB - (�������) nbNULL
//����������: nbS_OK, nbS_CANCEL
#define nbEVENT_BIM_CANCEL              (0x00000104)

//.............................................................................
//  ��� ��������� �������������� �������
//.............................................................................

// ������������� ���������� nbIBimProvider
// {4077DC01-8AB9-4322-B37E-26C2A4A27A2C}
static const nbUuid nbUUID_IBIM_PROVIDER = 
  {0x4077DC01,0x8ab9,0x4322,{0xb3,0x7e,0x26,0xc2,0xa4,0xa2,0x7a,0x2c}};

#undef  nbINTERFACE
#define nbINTERFACE nbIBimProvider

nbDECLARE_INTERFACE(nbIBimProvider,nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //��������� ����������� ������� �� ��������������� ����������
  //  handler   - [��] ��������� ��� ���������� ���������
  //����������: nbS_OK ��� ��� ������
  nbMETHOD(setEventHandler)(nbTHISc nbIEventHandler *handler) nbPURE;
  
  //������������� ����������
  //  handle  - [��] ��������� ���������� (0, �� ���������; -1 ��� ����������)
  nbMETHOD(connectToDevice)(nbTHISc nbHandle handle) nbPURE;
  
  //������������� ������������ ����
  //  handle      - [��] ��������� ������������ ���� (0, �� ���������; -1, ��� ����������)
  //  guihandler  - [���/���] ��������� ����������� ��������� ������������ ����
  nbMETHOD(connectToWindow)(nbTHISc nbHandle handle, nbIEventHandler **guihandler) nbPURE;

  //��������� ��������������� ����������
  //���������:
  //  evt     - [��]      ������� ����������������
  //  paramA  - [��/���]  ������ �������� 
  //  paramB  - [��/���]  ������ ��������
  //����������: nbS_OK (�������), nbS_CANCEL (���������) ��� ��� ������
  nbMETHOD(control)(nbTHISc nbEvent evt, const nbData paramA, nbData paramB) nbPURE;

  //����� ��������������� ������
  //����������: ���������� ������� ������� ����� (� �������� �������)
  //            ���� ����� ��������, ���� ���������� ������
  //����������: nbS_OK ��� ��� ������
  nbMETHOD(resetBim)(nbTHIS) nbPURE;

  //��������� ��������������� ������
  //  bim   - [��] �������������� ����� (0, ��� ������ ������)
  nbMETHOD(setBim)(nbTHISc const nbBim *bim, nbPurpose purpose) nbPURE;

  //��������� ������ ����� �������������� �������
  //  mode  - [��/���] ����� ������ �����������. � mode ������������ ����������� �������� ������
  //              (� ������ ������ ������, ������������ ������ ������, �������� ��� ������� �� ��������������)
  //  mask  - [��] ��������� ����� ��� ������, � ������� ����� ���������������/������������
  //����������: nbS_OK ��� ��� ������
  nbMETHOD(changeInputMode)(nbTHISc nbBimInputMode *mode, nbBimInputMode mask) nbPURE;
    
  //����������� ������ ��������������� ����������
  //����������: ������� ������ ���������� ����������, �� ������ ���������� �������� ����������
  //  bim       - [���] ����������� �������������� �����
  //                    (������ ���������� ����������� � ����� ����������, ���� �� ����� ������� WaitForBimReady)
  //  bimType   - [��]  ��� ��������������� ������
  //  purpose   - [��]  ���� ��������� ��������������� ������
  //����������:  nbS_OK ��� ��� ������
  nbMETHOD(captureBim)(nbTHISc nbBim **bim, nbRefUuid bimType, nbPurpose purpose) nbPURE;

  //�������� ���������� ����� ������ (��� ���������� �������� ��� �����)
  //  bim       - [��]  ��� �� ��������� �� ����������� �������������� �����, ��� ��� ������� � ������� CaptureBim
  //  timeout   - [��]  ����� ��������
  nbMETHOD(waitForBimReady)(nbTHISc nbBim **bim, nbTimeout timeout) nbPURE;
    
  //������������ ���������� ��� ����� ��� ������ ������
  //  bim       - [��] ��������� �� �������������� �����, ������ ��� ������� ���� �������� �����������
  //����������: nbS_OK ��� ��� ������
  nbMETHOD(releaseBim)(nbTHISc nbBim *bim) nbPURE;
};

#pragma pack()

#endif
