//////////////////////////////////////////////////////////////////////////
//����:         nbResult.h
//������:
//����:
//�������:
//�����:
//
//����:         �������� ���� nbResult, ������� ����� ������ � ������
//////////////////////////////////////////////////////////////////////////

#ifndef _NB_RESULT_H
#define _NB_RESULT_H

#include "nbTypes.h"

#pragma pack(1)

//.............................................................................

//�������� ����������
typedef int32_t         nbResult;

#define nbRESULT_S_MASK                      (0x80000000)    //  ����� �������� ������/������
#define nbRESULT_F_MASK                      (0x7FFF0000)    //  ����� ����������
#define nbRESULT_ECODE_MASK                  (0x0000FFFF)    //  ����� ���� ������

//�������� �� �������� ��������� ���������� �������
#define nbSUCCEEDED(r)  ((nbResult)(r)>=0)
//�������� �� �������� ������
#define nbFAILED(r)     ((nbResult)(r)<0)
//������������ �������� ������
#define nbE(f,ecode)     ((nbResult)(nbRESULT_S_MASK | ((uint32_t)(f)<<16) | (uint16_t)(ecode)))
//������������ �������� ������
#define nbS(scode)       ((nbResult)(~nbRESULT_S_MASK & (uint32_t)scode))

//  �������� ��������� ���������� �������

#define nbS_OK                              (0x00000000)//  �������.
#define nbS_CANCEL                          (0x00000001)//  ��������.
#define nbS_SKIP                            (0x00000002)//  ���������.

//  ���� ���������

#define nbF_SYS                             (0x0000)    //  ���������� ��������� ����� ������ (0x0000-0x07FF)
#define nbF_DEF                             (0x2000)    //  ����� ���� ���������� (0x2000)
#define nbF_SUB(x)                          (0x4000 | (uint16_t)(x))//  ���� ��������� ���������� (0x4000-0x7FFF)
                                                        //  (��� ���������� ���� �������� ������ ����� ����� � ������� 14 �����)
// ���� ������

#define nbECODE_FAIL                        (0x0001)    //  ����� ����.
#define nbECODE_POINTER                     (0x0002)    //  ������������ ��������� ��� ����� �� �������, ������������ ��� ���������.
#define nbECODE_PARAM                       (0x0003)    //  ����������� �������� ��������� �������.
#define nbECODE_IDENTIFIER                  (0x0004)    //  ������������ ������������� (���� ��� ������ ��������, ������������ � �������� ��������������).
#define nbECODE_INTEGRITY_FAIL              (0x0005)    //  �������� ����������� ������/����������.
#define nbECODE_CONSISTENCY_FAIL            (0x0006)    //  ����������� (���������) ������ ��� ����� ������ ������.
#define nbECODE_OUT_OF_MEMORY               (0x0007)    //  ������ ��������� ������ (��� ��������� ����� ������ ���������� ��������).
#define nbECODE_INSUFFICIENT_BUFFER         (0x0008)    //  ������������� ������ ������ (��� �������� ����� ������ ���������� ��������).
#define nbECODE_ACCESS_DENIED               (0x0009)    //  ������ �������� (��������� ��������� ��)
#define nbECODE_NO_MODULE                   (0x000A)    //  �� ������ ������.
#define nbECODE_NO_COMPONENT                (0x000B)    //  �� ������ ���������.
#define nbECODE_NO_INTERFACE                (0x000C)    //  �� ������ ���������.
#define nbECODE_OBJECT_NOT_FOUND            (0x000D)    //  ������ (����, ����, ...) �� ������
#define nbECODE_OBJECT_ALREADY_EXISTS       (0x000E)    //  ������ ��� ���������� (����, ���������, ...)
#define nbECODE_TIMEOUT_EXPIRED             (0x000F)    //  �������� �� ��������� �� ������� ��������� �������.
#define nbECODE_CANCELLED                   (0x0010)    //  �������� �� ���������, ��� ��� ���� �������� �����.
#define nbECODE_INCORRECT_VERSION           (0x0011)    //  ������������ ������.
#define nbECODE_IO                          (0x0012)    //  ������ �����/������.
#define nbECODE_UNSUPPORTED_FUNCTION        (0x0013)    //  ���������������� �������.

#define nbECODE_MATRIX_NOTATION             (0x0014)    //  ���������������� ������ ������������� ������� (������� ���������� ������������� ��� ��������� �� ���������)
#define nbECODE_UNBOUND_INDEX               (0x0015)    //  ������ ������� �� ������� ����������  ��������
#define nbECODE_UNBOUND_DATA                (0x0016)    //  �������� ������ �� ������������� ���������� ������������.
#define nbECODE_META                        (0x0017)    //  ������ ������ ���������� �� ����������.
#define nbECODE_META_PATTERN                (0x0018)    //  ������������ ������ ������������ (������������ nrows ��� �������� ����� meta)
#define nbECODE_META_NIL                    (0x0019)    //  ������ ������������ ����������� (���� count ����� 0).
                                                        //  (����� �������������� ��� ������ �������� �����)

#define nbECODE_NBCC_STATE                  (0x0020)    //  ������������ (�����������) ��������� ���� �� ��������� ��������� ��������.
#define nbECODE_SLOT_ID                     (0x0021)    //  ������������� ����� ���������� (���� ����������� � �����).
#define nbECODE_UNSUPPORTED_TEST_MODE       (0x0022)    //  ���������������� ����� ������������.
#define nbECODE_UNSUPPORTED_TRAIN_MODE      (0x0023)    //  ���������������� ����� ��������.
#define nbECODE_UNSUPPORTED_CONSTRAINT      (0x0024)    //  ���������������� ��������.
#define nbECODE_UNSUPPORTED_PARAM           (0x0025)    //  ���������������� ��������.
#define nbECODE_UNSUPPORTED_CSCHEME         (0x0026)    //  ���������������� ��� ������������ ����� ��������.
#define nbECODE_UNSUPPORTED_BLOCK_TYPE      (0x0027)    //  ���������������� ��� �����.
#define nbECODE_NUMERATION_RULE             (0x0028)    //  ������ ������ ������� ��������� (������).

#define nbECODE_NO_DATA                     (0x002A)    //  �� ������ ������ ��������� ��� ����� �������� �� ������������� ����������.
#define nbECODE_NO_DATA_OWN                 (0x002B)    //  �� ������ ��� ������������ �������� "����"
#define nbECODE_NO_DATA_ALL                 (0x002C)    //  �� ������ ��� ������������ �������� "�����".

#define nbECODE_BLOCK_STATE                 (0x0030)    //  ������������ ��������� �����.
#define nbECODE_BLOCK_FLAG                  (0x0031)    //  ���������������� ���� (��� ����������) �����.
#define nbECODE_UNATTAINABLE_BLOCK_STATE    (0x0032)    //  ������������ ��������� ����� �� ��������.
#define nbECODE_PARAM_IN_NUMBER             (0x0033)    //  ������������ ����� ������� ���������� (������).
#define nbECODE_PARAM_SPEC_IN_NUMBER        (0x0034)    //  ������������ ����� ����������� ������� ���������� (������).
#define nbECODE_PARAM_SPEC_OUT_NUMBER       (0x0035)    //  ������������ ����� ����������� �������� ���������� (������).
#define nbECODE_POOR_DIVERGENCE             (0x0037)    //  ������ ���������� ������ ��� �������� ����������.
#define nbECODE_POOR_QUALITY                (0x0038)    //  ������ �������� �������.
#define nbECODE_POOR_STABILITY              (0x0039)    //  ������ ������������ �������.
#define nbECODE_POOR_ORIGINALITY            (0x003A)    //  ������ ������������.
#define nbECODE_TOO_MANY_GROSS_ERRORS       (0x003B)    //  ������� ����� ������ ������ ������.

#define nbECODE_OBJECT_STATE                (0x003D)    //  ������������ ��������� �������
#define nbECODE_NO_DEVICE                   (0x003E)    //  �� ������� ����������

//.............................................................................

#define nbE_FAIL                        nbE(nbF_DEF,nbECODE_FAIL)
#define nbE_POINTER                     nbE(nbF_DEF,nbECODE_POINTER)
#define nbE_PARAM                       nbE(nbF_DEF,nbECODE_PARAM)
#define nbE_IDENTIFIER                  nbE(nbF_DEF,nbECODE_IDENTIFIER)
#define nbE_INTEGRITY_FAIL              nbE(nbF_DEF,nbECODE_INTEGRITY_FAIL)
#define nbE_CONSISTENCY_FAIL            nbE(nbF_DEF,nbECODE_CONSISTENCY_FAIL)
#define nbE_OUT_OF_MEMORY               nbE(nbF_DEF,nbECODE_OUT_OF_MEMORY)
#define nbE_INSUFFICIENT_BUFFER         nbE(nbF_DEF,nbECODE_INSUFFICIENT_BUFFER)
#define nbE_NO_MODULE                   nbE(nbF_DEF,nbECODE_NO_MODULE)
#define nbE_NO_COMPONENT                nbE(nbF_DEF,nbECODE_NO_COMPONENT)
#define nbE_NO_INTERFACE                nbE(nbF_DEF,nbECODE_NO_INTERFACE)
#define nbE_ACCESS_DENIED               nbE(nbF_DEF,nbECODE_ACCESS_DENIED)
#define nbE_OBJECT_NOT_FOUND            nbE(nbF_DEF,nbECODE_OBJECT_NOT_FOUND)
#define nbE_OBJECT_ALREADY_EXISTS       nbE(nbF_DEF,nbECODE_OBJECT_ALREADY_EXISTS)
#define nbE_TIMEOUT_EXPIRED             nbE(nbF_DEF,nbECODE_TIMEOUT_EXPIRED)
#define nbE_CANCELLED                   nbE(nbF_DEF,nbECODE_CANCELLED)
#define nbE_UNSUPPORTED_VERSION         nbE(nbF_DEF,nbECODE_INCORRECT_VERSION)
#define nbE_IO                          nbE(nbF_DEF,nbECODE_IO)
#define nbE_UNSUPPORTED_FUNCTION        nbE(nbF_DEF,nbECODE_UNSUPPORTED_FUNCTION)

#define nbE_MATRIX_NOTATION             nbE(nbF_DEF,nbECODE_MATRIX_NOTATION)
#define nbE_UNBOUND_INDEX               nbE(nbF_DEF,nbECODE_UNBOUND_INDEX)
#define nbE_UNBOUND_DATA                nbE(nbF_DEF,nbECODE_UNBOUND_DATA)
#define nbE_META                        nbE(nbF_DEF,nbECODE_META)
#define nbE_META_PATTERN                nbE(nbF_DEF,nbECODE_META_PATTERN)
#define nbE_META_NIL                    nbE(nbF_DEF,nbECODE_META_NIL)

#define nbE_INVALID_NBCC_STATE          nbE(nbF_DEF,nbECODE_NBCC_STATE)
#define nbE_INVALID_SLOT                nbE(nbF_DEF,nbECODE_SLOT_ID)
#define nbE_UNSUPPORTED_TEST_MODE       nbE(nbF_DEF,nbECODE_UNSUPPORTED_TEST_MODE)
#define nbE_UNSUPPORTED_TRAIN_MODE      nbE(nbF_DEF,nbECODE_UNSUPPORTED_TRAIN_MODE)
#define nbE_UNSUPPORTED_CONSTRAINT      nbE(nbF_DEF,nbECODE_UNSUPPORTED_CONSTRAINT)
#define nbE_UNSUPPORTED_PARAM           nbE(nbF_DEF,nbECODE_UNSUPPORTED_PARAM)
#define nbE_UNSUPPORTED_CSCHEME         nbE(nbF_DEF,nbECODE_UNSUPPORTED_CSCHEME)
#define nbE_UNSUPPORTED_BLOCK_TYPE      nbE(nbF_DEF,nbECODE_UNSUPPORTED_BLOCK_TYPE)
#define nbE_NUMERATION_RULE             nbE(nbF_DEF,nbECODE_NUMERATION_RULE)

#define nbE_NO_DATA                     nbE(nbF_DEF,nbECODE_NO_DATA)
#define nbE_NO_DATA_OWN                 nbE(nbF_DEF,nbECODE_NO_DATA_OWN)
#define nbE_NO_DATA_ALL                 nbE(nbF_DEF,nbECODE_NO_DATA_ALL)

#define nbE_BLOCK_STATE                 nbE(nbF_DEF,nbECODE_BLOCK_STATE)
#define nbE_BLOCK_FLAG                  nbE(nbF_DEF,nbECODE_BLOCK_FLAG)
#define nbE_UNATTAINABLE_BLOCK_STATE    nbE(nbF_DEF,nbECODE_UNATTAINABLE_BLOCK_STATE)
#define nbE_PARAM_IN_NUMBER             nbE(nbF_DEF,nbECODE_PARAM_IN_NUMBER)
#define nbE_PARAM_SPEC_IN_NUMBER        nbE(nbF_DEF,nbECODE_PARAM_SPEC_IN_NUMBER)
#define nbE_PARAM_SPEC_OUT_NUMBER       nbE(nbF_DEF,nbECODE_PARAM_SPEC_OUT_NUMBER)
#define nbE_POOR_DIVERGENCE             nbE(nbF_DEF,nbECODE_POOR_DIVERGENCE)
#define nbE_POOR_QUALITY                nbE(nbF_DEF,nbECODE_POOR_QUALITY)
#define nbE_POOR_STABILITY              nbE(nbF_DEF,nbECODE_POOR_STABILITY)
#define nbE_POOR_ORIGINALITY            nbE(nbF_DEF,nbECODE_POOR_ORIGINALITY)
#define nbE_TOO_MANY_GROSS_ERRORS       nbE(nbF_DEF,nbECODE_TOO_MANY_GROSS_ERRORS)

#define nbE_OBJECT_STATE                nbE(nbF_DEF,nbECODE_OBJECT_STATE)
#define nbE_NO_DEVICE                   nbE(nbF_DEF,nbECODE_NO_DEVICE)

#pragma pack()
#endif

