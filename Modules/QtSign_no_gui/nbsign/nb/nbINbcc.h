 ///////////////////////////////////////////////////////////////////////////////
//����:         nbINbcc.h
//������:
//����:
//�������:
//�����:
//
//����:         �������� ��� ����
///////////////////////////////////////////////////////////////////////////////
#ifndef _NB_INBCC_H
#define _NB_INBCC_H

#include "nbTypes.h"
#include "nbResult.h"
#include "nbIUnknown.h"

//.............................................................................

//  ������� ��� ����

//�������: ��������
//  nbSlotID  paramA - (�������) ���������������� ����� �������� �����.
//  int32_t paramB - (�������) ���������������� ��� ������� �������� �� 0 �� 2^31 (������������ �������� �������� ����������� ��������).
//                    ����� �����, �������� ������ ���������.
//����������: nbS_OK, nbS_DENY
#define nbEVENT_NBCC_PROGRESS              (0x00000401)    //  ��������� (��������)

//.............................................................................
//  ��� ����
//.............................................................................

// ������������� ���������� nbINbcc
// {A3213E60-57B3-4643-84E4-11CAB9E81FFE}
static const nbUuid nbUUID_INBCC = 
  {0xa3213e60,0x57b3,0x4643,{0x84,0xe4,0x11,0xca,0xb9,0xe8,0x1f,0xfe}};

#undef  nbINTERFACE
#define nbINTERFACE   nbINbcc

nbDECLARE_INTERFACE(nbINbcc, nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //��������� ����������� ������� ����
  //  handler     - [��]  ��������� ��� ���������� ������� (���� nbNULL, ���������� ������������)
  //����������: nbS_OK
  //            nbECODE_UNSUPPORTED_PARAM
  nbMETHOD(setEventHandler)(nbTHISc nbIEventHandler *handler) nbPURE;

  //��������� ����� ��������������
  //  cscheme     - [��]  ����� �������������� � �������� ������� ��� nbNULL,
  //                      ���� ������ �������������� ����� "�� ���������"
  //����������: nbS_OK
  //            nbECODE_OUT_OF_MEMORY
  //            nbECODE_CONSISTENCY_FAIL
  //            nbECODE_UNSUPPORTED_CSCHEME
  //            nbECODE_UNSUPPORTED_BLOCK_TYPE
  //            nbECODE_META
  //            nbECODE_PARAM_IN_NUMBER
  //            nbECODE_PARAM_SPEC_IN_NUMBER
  //            nbECODE_PARAM_SPEC_OUT_NUMBER
  //            nbECODE_BLOCK_FLAG
  //            nbECODE_BLOCK_STATE
  //            nbECODE_NUMERATION_RULE
  nbMETHOD(setConvScheme)(nbTHISc const nbData cScheme) nbPURE;

  //������������ ��������� ���� ���� �������������� � �� ��������� � �������/�������� ����������
  //  csid        - [��]  ���������� ������������� ����� ��������������.
  //                      ���� �������� �������������� ������ 65536, ������������� ���������� ���������� ����� ����� ��������������,
  //                      ��������� ����. �������� nbUUID_NIL ������ ����� �� ���������.
  //  mIn         - [��/����] ������ ��� ������� ������� �������������� ����������
  //  mCode       - [��/����] ������ ��������� ����.
  //  convScheme  - [���] ����� ��������������
  //����������: nbS_OK
  //            nbECODE_IDENTIFIER
  //            nbECODE_UNBOUND_INDEX
  //            nbECODE_INSUFFICIENT_BUFFER
  //            nbECODE_META
  //            nbECODE_PARAM_IN_NUMBER
  //            nbECODE_PARAM_OUT_NUMBER
  nbMETHOD(enumConvScheme)(nbTHISc nbRefUuid csid, const nbMatrix *mIn, const nbMatrix *mCode, nbData cScheme) nbPURE;

  //��������� �������� ����� �� ������ ������ � ������� ����� ��������������
  //  id        - [��] ����� ����� � �����
  //  slotDescr - [���/����] �������� ����� (��� ����� ����� ��������������).
  //                  ���� ����� nbNULL, ������ �� ����������.
  //                  ���� ������ ������������, ������������ ��������� ������ ������
  //����������: ����� ���������� ����� �� ����� (������ �������� ������)
  //            nbECODE_INVALID_NBCC_STATE
  //            nbECODE_INSUFFICIENT_BUFFER
  nbMETHOD(getSlotDescr)(nbTHISc nbSlotID id, nbData slotDescr) nbPURE;

  //��������� ��������� ��� �������� ��� ��� ������/������
  //  id          - [��]  ����� ����� ����.
  //  conType     - [��]  ��� �����������.
  //  mCon        - [��]  �������� �����������.
  //                  ���� ����� NIL, ��������� (��� �������� ���������) ����������� �� ���������.
  //                  ���� ������������ ����� META_NIL, �������� ��������� �� ������ ����������� ��� ��������������.
  //����������: nbS_OK
  //            nbECODE_ACCESS_DENIED
  //            nbECODE_INVALID_NBCC_STATE
  //            nbECODE_INVALID_SLOT
  //            nbECODE_UNSUPPORTED_CONSTRAINT
  //            nbECODE_UNSUPPORTED_PARAM
  //            nbECODE_NO_DATA
  //            nbECODE_UNBOUND_DATA
  //            nbECODE_MATRIX_NOTATION
  //            nbECODE_META
  nbMETHOD(setConstraint)(nbTHISc nbSlotID id, nbConstraintType conType, nbMatrix *mCon) nbPURE;
  
  //������ �������� ��� ��������� ��� ���������
  //  ���� �������� ������ ������������, ������������ ��� ������������, � ����� ����������� ���� ncols
  //����������:
  //  1. ������ ����������� ������������ ��������� ������� ���������.
  //  2. ������ ��������� ������������ ������
  nbMETHOD(getConstraint)(nbTHISc nbSlotID id, nbConstraintType conType, nbMatrix *mCon) nbPURE;

  //���������� ����-������� �� ���������� ����.
  //(������ �������������� ��� ������� ����� �������� � ����� ��������)
  //(������� �������������� �������������)
  //  ��������������� ������ ���� �������������� �������� ����� �������� ���
  //  ������ ������������ ��������������� ����������.
  //  ��������� ������ ���������� ���������� ��������.
  //
  //  mIn       - [��]  ������� ��� �������������� �������. ���� �� ����� ��������� ��������,
  //                      �� ������� �������� ����� ����� ������ ��������� ����� �������� �����.
  //  mOut      - [���] ������� ����� �������, ���������� 1 ��� ����� ������ ���� �������.
  //  ids       - [��/����] ������ ������, � ������� ��������� ��������.
  //                    �������� nbNULL ���������� ���� ���� ������.
  //����������: nbS_OK
  //            ��. ���� ������ SetConstraint
  //            nbECODE_NBCC_INSUFFICIENT_BASE_ANY
  //            nbECODE_WRONG_IN_NUMBER
  //            nbECODE_WRONG_OUT_NUMBER
  nbMETHOD(extract)(nbTHISc const nbMatrix *mIn, nbMatrix *mOut, const nbSlotID *ids) nbPURE;

  //��������
  //  mInOwn      - [��]  ������� ��� "����".
  //  mInAll      - [��/����] ������� ��� "��� �����". ���� �� �����, ������������ ���������� �������� �������������
  //                    ������� "��� �����" ��� �������� �������������� ����������� ���������.
  //  mOutCode    - [��]  ������� ���� �������. ������ ��������� ������ 1 ������ ���� �������.
  //  timeout     - [��] ������������ ����� �������� ���������� ��������. ���� ����� �������� ������ ���������� ��������, �������� �����������.
  //����������: nbS_OK
  //            ��. ���� ������ SetParam
  //            ��. ���� ������ PostTrain
  nbMETHOD(train)(nbTHISc const nbMatrix *mInOwn, const nbMatrix *mInAll, const nbMatrix *mCode, nbTimeout timeout) nbPURE;

  //���������� ���������� ����� �������������� (��� ��������� ������)
  //  ������������ ��� ������, ��� ������� ����� ���� nbBLOCK_FLAG_POST_TRAIN
  //  id          - [��]  ���� ����� ���������
  //  trainMode   - [��]  ����� ����������
  //  blockState  - [��]  ��������� �� ��������� �����, �� �������� ����������� ���������� (��� � ������� �������, ��� � � �������)
  //  blockFlags  - [��]  ��������� �� ����� ���������������/������������ ����� �����
  //  timeout     - [��]  ������������ ����� �������� ���������� ��������. ���� ����� �������� ������ ���������� ��������, �������� �����������.
  //����������: ����� �������� ���������
  //            nbECODE_TIMEOUT_EXPIRED
  //            nbECODE_CANCELLED
  //            nbECODE_ACCESS_DENIED
  //            nbECODE_INVALID_NBCC_STATE
  //            nbECODE_INVALID_SLOT
  //
  //            nbECODE_BLOCK_STATE
  //            nbECODE_BLOCK_FLAG
  //            nbECODE_UNATTAINABLE_BLOCK_STATE
  //            nbECODE_UNSUPPORTED_TRAIN_MODE
  //
  //            nbECODE_NO_DATA
  //            nbECODE_NO_DATA_OWN
  //            nbECODE_NO_DATA_ALL
  //            nbECODE_POOR_DIVERGENCE
  //            nbECODE_POOR_QUALITY
  //            nbECODE_POOR_STABILITY
  //            nbECODE_POOR_ORIGINALITY
  //            nbECODE_TOO_MANY_GROSS_ERRORS
  nbMETHOD(postTrain)(nbTHISc nbSlotID id, nbTrainMode trainMode, nbBlockState blockState, nbBlockFlags blockFlags, nbTimeout timeout) nbPURE;
  
  //��������� ������ ������ ������� ���������� 
  //(��������� ���������� �������� ������ ������� ����� �� ������������)
  //  ������������ ����� ������� ������� Learn ��� ��������� �������� "������", "��������������" �������.
  //  mInOwn     - [��]  ������� ��� "����".
  //  mInAll     - [��]  ������� ��� "��� �����".
  //  mGe        - [���] �������, ���������� ������ �����������, ������������ ���������������
  //                      ���������� �������� �� �������� ��������. ����� ��������� �������
  //                      ����� ����� �������� ������� ��� "����". ���������� ����� ����
  //                      ������������ � ����� �����. �������� [0.0 .. 1.0] ��� [0 .. 100] �������� 
  //                      ���������� ������, � �������� ������ 1.0 ��� 100 - ������ ������������
  //                      ������ ��� ������������ � ����� ����� ��������������.
  //����������: nbS_OK
  nbMETHOD(indicateGrossErrors)(nbTHISc const nbMatrix *mInOwn, const nbMatrix *mInAll, nbMatrix *mGe) nbPURE;

  //�������� ������������ ���������� ����
  //(����� ���������� ��������� ����������)
  //  ��������� �������������� ���������-��� � �������� �������������� ���������� � �����������.
  //  testMode    - [��] ����� ������������
  //  mEstimate   - [���] ������� ������ ��������� (� ������������ ��� ����� �������)
  //  timeout     - [��] ������������ �����, � ������� �������� ����������� ������
  //����������: nbS_OK
  //            nbECODE_NBCC_STATE
  //            nbECODE_CANCELLED
  //            nbECODE_UNSUPPORTED_TEST_MODE
  nbMETHOD(test)(nbTHISc nbTestMode testMode, nbMatrix *mEstimate, nbTimeout timeout) nbPURE;

  //����� ������� ��������������� ���������-���
  //  flags       - [��]  ����� ������������ ������ ����, ������������ ��������� "���"
  //����������: ����������� ��������� ����� ������ (������������ �����, ������� ����� ���� ��������)
  //            nbECODE_FAIL
  //            nbECODE_INVALID_NBCC_STATE
  nbMETHOD(reset)(nbTHISc nbNbccState nbccState) nbPURE;

  //������� ��������������� ���������� ��� ����� � �������� �������
  //  nbc         - [���] ������������ �������������� ��������� ��� ����
  //  nbcType     - [��] ��������� ������ ��� (�� ��������� nbUUID_NIL)
  //����������: nbS_OK
  //            nbECODE_INSUFFICIENT_BUFFER
  //            nbECODE_INVALID_NBCC_STATE
  //            nbECODE_INCORRECT_VERSION
  nbMETHOD(exportNbc)(nbTHISc nbData nbc, nbRefUuid nbcType) nbPURE;
  
  //������ ��������������� ���������� ��� ����� � �������� �������
  //  nbc         - [��]  ������������ �������������� ��������� ��� ����
  //����������: nbS_OK
  //            ��. ���� ������ SetConnScheme
  //            nbECODE_OUT_OF_MEMORY
  //            nbECODE_INCORRECT_VERSION
  //            nbECODE_CONSISTENCY_FAIL
  //            nbECODE_DATA
  nbMETHOD(importNbc)(nbTHISc const nbData nbc) nbPURE;

};

#endif
