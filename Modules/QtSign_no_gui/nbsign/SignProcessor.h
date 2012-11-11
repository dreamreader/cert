/****************************************************************************
** ����:         SignProcessor.h
** ������:
** ����:
** �������:
** �����:
**
** ��������:     ���������� ���������� �������.
****************************************************************************/

#ifndef _SIGN_PROCESSOR_H
#define _SIGN_PROCESSOR_H

#include <qatomic.h>

#include "nb\nb.h"
#include "nbclasses\nbcTypes.h"

//////////////////////////////////////////////////////////////////////////
// ������������� ���������� ����� ���������� �������
// {A9EBF452-CF25-408c-8647-5E3A1D57FF4E}
static const nbUuid nbUUID_SIGN_PROVIDER =
  {0xa9ebf452,0xcf25,0x408c,{0x86,0x47,0x5e,0x3a,0x1d,0x57,0xff,0x4e}};

//////////////////////////////////////////////////////////////////////////
// ������������� ���������� ��������� ���������� �������
// {702CBB42-7367-48cf-BED1-D78D1774BA52}
static const nbUuid nbUUID_SIGN_PROCESSOR =
  {0x702cbb42,0x7367,0x48cf,{0xbe,0xd1,0xd7,0x8d,0x17,0x74,0xba,0x52}};

//////////////////////////////////////////////////////////////////////////
// ��������� ��������� ���������� �������
class SignProcessor: public nbIBimProcessor, nbIConfigurate {
public:
  // �����������
  SignProcessor();
  // ����������
  ~SignProcessor();

  // �������� ����������� �������� �����������
  static nbResult GetCoRefs();

  ///////////////////////nbIUnknown//////////////////////////////////////

  // ������ ��������� ���������� ����������
  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd);
  // ���������� �������� ������ �� ��������� (� ������ ������������ ������ �� ����)
  nbMETHOD(retain)(nbTHIS);
  // ������������ ����������, ����� ���������� �������� ������ �� ����
  nbMETHOD(release)(nbTHIS);

  ///////////////////////nbIBimProcessor///////////////////////////////////

  // ��������� ����������� ������� �� ����������� �������
  nbMETHOD(setEventHandler)(nbTHISc nbIEventHandler *handler);
  // ��������� ���� (�������) �������������� �������
  nbMETHOD(process)(nbTHISc nbMatrix **any, const nbBim **bimBase, uint32_t bimCount, nbMatrix **ind);
  // ��������� �������� �������������� �������
  nbMETHOD(createTemplate)(nbTHISc nbMatrix **own, nbMatrix **pattern, const nbBim **bimBase, uint32_t bimCount);
  // ��������� ���������������  ����� �������
  nbMETHOD(setTemplate)(nbTHISc const nbMatrix *pattern, nbRefUuid bimType);
  // ������ ������� ��������� ��������������� ������ ��������� ����
  nbMETHOD(queryProcessRules)(nbTHISc nbMatrix **pattern, nbRefUuid bimType);
  // ������ ���� "��� �����" ��� "����", �������������� � ���� ������� ���
  nbMETHOD(queryBaseAny)(nbTHISc nbMatrix **any, uint32_t maxCount, nbRefUuid bimType, int32_t userId);
  // ������������ ������ ��-��� ���� "��� �����" ��� ����, ���������� �� ����� ���������
  nbMETHOD(releaseBase)(nbTHISc nbMatrix *any);

  ///////////////////////nbIConfigurate///////////////////////////////////

  // �������� ������� ������������
  nbMETHOD(setConfig)(nbUuid page, nbData opt);
  // ������ ������� ������������
  nbMETHOD(getConfig)(nbUuid page, nbData opt);
  // ������ ������������ "�� ���������"
  nbMETHOD(getDefaultConfig)(nbUuid page, nbData opt);
  // ��������� ������������
  nbMETHOD(tuneUp)(nbHandle handle, nbUuid page, nbData opt, nbIEventHandler **guihandler);
  // ������������ ������� ������������, ������� ����� ��������� ��� ���������� ����������
  nbMETHOD(enumPages)(nbIEventHandler *eh, nbRefUuid iid);
  //��������� �������� �������� ������������
  nbMETHOD(getTitle)(nbUuid page, nbData name, nbData descr);

private:
  QAtomicInt        _iRefs;         // ������� ������ �� ���������� ����������
  static QAtomicInt _cRefs;         // ������� ������ �� ����������

};

#endif  // _SIGN_PROCESSOR_H
