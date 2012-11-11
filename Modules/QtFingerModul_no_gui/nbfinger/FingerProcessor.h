//////////////////////////////////////////////////////////////////////////
//����:         FingerProcessor.h
//������:
//����:
//�������:
//�����:
//
//��������:     ���������� ��������� ������
//////////////////////////////////////////////////////////////////////////

#ifndef _FINGER_PROCESSOR_H
#define _FINGER_PROCESSOR_H

#include <qatomic.h>

#include "nb/nb.h"
#include "nbclasses/nbcTypes.h"

//////////////////////////////////////////////////////////////////////////
// ������������� ���������� ��������� ������������ ������� ��������� ������
//  {6B2E0B0B-FF32-4DB7-B22F-2E60F6D3C68C}
static const nbUuid nbUUID_FINGER_PROCESSOR = 
  {0x6b2e0b0b,0xff32,0x4db7,{0xb2,0x2f,0x2e,0x60,0xf6,0xd3,0xc6,0x8c} };


//////////////////////////////////////////////////////////////////////////
// ��������� ��������� ������������ ������� ��������� ������
class FingerProcessor: public nbIBimProcessor, nbIConfigurate {
public:
	// �����������
  FingerProcessor();
	// ����������
	~FingerProcessor();

  //�������� ����������� �������� �����������
  static nbResult getCoRefs();

  ///////////////////////nbIUnknown//////////////////////////////////////

  //������ ��������� ���������� ����������
  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd);
  //���������� �������� ������ �� ��������� (� ������ ������������ ������ �� ����)
  nbMETHOD(retain)(nbTHIS);
  //������������ ����������, ����� ���������� �������� ������ �� ����
  nbMETHOD(release)(nbTHIS);

  ///////////////////////nbIBimProcessor///////////////////////////////////

  //��������� ����������� ������� �� ����������� �������
  nbMETHOD(setEventHandler)(nbTHISc nbIEventHandler *handler);
  //��������� ���� (�������) �������������� �������
  nbMETHOD(process)(nbTHISc nbMatrix **baseAny, const nbBim **bimBase, uint32_t bimCount, nbMatrix **ind);

  //������ ������� ��������� ��������������� ������ ��������� ����
  nbMETHOD(queryProcessRules)(nbTHISc nbMatrix **meta, nbRefUuid bimType);


  //������ ���� "��� �����" ��� "����", �������������� � ���� ������� ���
  nbMETHOD(queryBaseAny)(nbTHISc nbMatrix **baseAny, uint32_t maxCount, nbRefUuid bimType, int32_t userId);
  //������������ ������ ��-��� ���� "��� �����" ��� ����, ���������� �� ����� ���������
  nbMETHOD(releaseBase)(nbTHISc nbMatrix *baseAny);

  //��������� �������� �������������� �������
  nbMETHOD(createTemplate)(nbTHISc nbMatrix **baseOwn, nbMatrix **pattern, const nbBim **bimBase, uint32_t bimCount);

  //��������� ���������������  ����� ������� ��������� �������
  nbMETHOD(setTemplate)(nbTHISc const nbMatrix *pattern, nbRefUuid bimType);

  ///////////////////////nbIConfigurate///////////////////////////////////

  //�������� ������� ������������
  nbMETHOD(setConfig)(nbUuid page, nbData opt);
  //������ ������� ������������
  nbMETHOD(getConfig)(nbUuid page, nbData opt);
  //������ ������������ "�� ���������"
  nbMETHOD(getDefaultConfig)(nbUuid page, nbData opt);
  //��������� ������������
  nbMETHOD(tuneUp)(nbHandle handle, nbUuid page, nbData opt, nbIEventHandler **guihandler);
  //������������ ������� ������������, ������� ����� ��������� ��� ���������� ����������
  nbMETHOD(enumPages)(nbIEventHandler *eh, nbRefUuid iid);
  //��������� �������� �������� ������������
  nbMETHOD(getTitle)(nbUuid page, nbData name, nbData descr);

private:

  nbHandle          _module;
  QAtomicInt        _iRefs; // ������� ������ �� ���������� ����������
  static QAtomicInt _cRefs; // ������� ������ �� ����������
  NbMatrix          _pattern;
};

#endif	// FINGER_PROCESSOR_H
