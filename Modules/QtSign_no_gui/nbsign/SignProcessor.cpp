/****************************************************************************
** ����:         SignProcessor.cpp
** ������:
** ����:
** �������:
** �����:
**
** ��������:     ���������� ���������� �������.
****************************************************************************/

#include <qresource.h>
#include <QFile>
#include <QTextStream>
#include "hw\psnConversionSFN.h"
#include "hw\BimHw.h"
#include "SignProcessor.h"
#include "psnReadWrite.h"

#pragma warning(disable:4100) // ������������ ������ unreferenced formal parameter

QAtomicInt SignProcessor::_cRefs(0); // ������� ������ �� ����������

// �����������
SignProcessor::SignProcessor() {
  _cRefs.ref();
}

// ����������
SignProcessor::~SignProcessor() {
  _cRefs.deref();
}

// �������� ����������� �������� �����������
nbResult SignProcessor::GetCoRefs() {
  return (nbResult)_cRefs;
}

///////////////////////nbIUnknown//////////////////////////////////////

// ������ ��������� ���������� ����������
nbMETHOD_DEF(SignProcessor,queryInterface)(nbRefUuid iid, void **icd) {
  if ( iid == nbUUID_IUNKNOWN )
    *icd = static_cast<nbIBimProcessor*>(this);
  else if ( iid == nbUUID_IBIM_PROCESSOR )
    *icd = static_cast<nbIBimProcessor*>(this);
  else if ( iid == nbUUID_ICONFIGURATE )
    *icd = static_cast<nbIConfigurate*>(this);
  else {
    *icd = nbNULL;
    return nbE_NO_INTERFACE;
  }
  static_cast<nbIUnknown*>(*icd)->retain();
  return nbS_OK;
}

// ���������� �������� ������ �� ��������� (� ������ ������������ ������ �� ����)
nbMETHOD_DEF(SignProcessor,retain)(nbTHIS) {
  return (nbResult)_iRefs.ref();
}

// ������������ ����������, ����� ���������� �������� ������ �� ����
nbMETHOD_DEF(SignProcessor,release)(nbTHIS) {
  if (!_iRefs.deref()){
    delete this;
    return nbS_OK;
  }
  return _iRefs;
}

///////////////////////nbIBimProcessor///////////////////////////////////

// ��������� ����������� ������� �� ����������� �������
nbMETHOD_DEF(SignProcessor,setEventHandler)(nbTHISc nbIEventHandler *handler) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// ��������� ���� (�������) �������������� �������
nbMETHOD_DEF(SignProcessor,process)(nbTHISc nbMatrix **any, const nbBim **bimBase, uint32_t bimCount, nbMatrix **ind) {
  nbResult rslt = nbS_OK;
  // ��������� ������� ���������
/*qDebug() << "step 1. bimCount = " << QString::number(bimCount);*/
  if ( !any || !bimBase || bimCount == 0) return nbE_PARAM;
  for (uint32_t i = 0; i < bimCount; ++i){
    if (bimBase[i]->bimType != nbBIM_TYPE_HANDWR) return nbE_IDENTIFIER;
  }

  // �������� ������ ��� ������� �������� �������������� ����������
  rslt = queryProcessRules(any,nbBIM_TYPE_HANDWR);
  if (nbFAILED(rslt)) return rslt;
  NbMatrix *m = *(NbMatrix**)(any);
  m->resizeData(bimCount);

  // ��������� �����������
  rslt = NcCalcImagesCoef( bimBase, *m );

//////////////////////////////////////////////////////////////////////////
// ��������
/*
  // ��������� ������������ � ���-����
  nbMatrix *vbps = *baseAny;
  float *ptr;
  QFile file("coeffs_log.txt");
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    for ( uint32_t i = 0; i < vbps->ncols; i++ ) {
      ptr = (float*)vbps->data[0][i];
      for ( uint32_t j = 0; j < vbps->meta->count; j++ ) {
        out << ptr[j] << "\n";
      }
    }
    out << "\n";
  }
  file.close();
*/
// ��������
//////////////////////////////////////////////////////////////////////////

  return rslt;
}
// ��������� �������� �������������� �������
nbMETHOD_DEF(SignProcessor, createTemplate)(nbTHISc nbMatrix **baseOwn, nbMatrix **pattern, const nbBim **bimBase, uint32_t bimCount) {
  *pattern = nbNULL;
  return process(baseOwn, bimBase, bimCount, nbNULL);
}

// ��������� ���������������  ����� �������
nbMETHOD_DEF(SignProcessor, setTemplate)(nbTHISc const nbMatrix *pattern, nbRefUuid bimType) {
  if ( pattern ) return nbS_SKIP;
  if ( bimType != nbBIM_TYPE_HANDWR ) return nbE_IDENTIFIER;
  return nbS_OK;
}

// ������ ������� ��������� ��������������� ������ ��������� ����
nbMETHOD_DEF(SignProcessor,queryProcessRules)(nbTHISc nbMatrix **pattern, nbRefUuid bimType) {
  // ��������� ������� ���������
  if ( !pattern ) return nbE_PARAM;
  if ( bimType != nbBIM_TYPE_HANDWR ) return nbE_IDENTIFIER;

  // ������� ������� ������� ��� ���������� ����������
  NbMeta meta(psnFFT_COEF_COUNT + psnP_COEF_COUNT, nbMF_R32, nbMT_CONTINUOUS);
  *pattern = new NbMatrix(1,0,&meta);
  if (!*pattern) return nbE_OUT_OF_MEMORY;

  return nbS_OK;
}

//////////////////////////////////////////////////////////////////////////
// ������ ���� "��� �����" ��� "����", �������������� � ���� ������� ���
nbMETHOD_DEF(SignProcessor,queryBaseAny)(nbTHISc nbMatrix **any, uint32_t maxCount, nbRefUuid bimType, int32_t userId) {
  // ��������� ��� ��������������� ������ � ������������
  if (!any || maxCount == 0) return nbE_PARAM;
  if (bimType != nbBIM_TYPE_HANDWR) return nbE_IDENTIFIER;
  if (userId != 0) return nbE_PARAM;

  nbResult rslt = nbS_OK;

// ������� 1. ���������� ���� ��������� � ���������� ������� ���
/*
  QResource resBimBase(":/base/simgbase");

  // ������� "�������" ���� "������ ���� ��� �����"
  rslt = GetBimCount(resBimBase.data(),&maxCount);
  if (nbFAILED(rslt)) {
    return rslt;
  }
  nbBim **bimBase = (nbBim**)new nbHandle[maxCount];
  if (!bimBase) return nbE_OUT_OF_MEMORY;
  
  rslt = LoadBimFromResource(resBimBase.data(),&maxCount, bimBase);
  if (nbSUCCEEDED(rslt)){
    rslt = Process(baseAny, (const nbBim**)bimBase, maxCount);
    // ���������� ������, ���������� ��� ������
    for (uint32_t i = 0; i < maxCount; ++i) {
      BimHw img;
      img.attach(bimBase[i]);
      img.free();
    }
  }
  delete [] bimBase;
*/

// ������� 2. ���������� ������� ���
  QResource resBase(":/base/sprmbase"); // ���� "������ ���� ��� �����"
  rslt = LoadMatrixFromResource(resBase.data(),any);

  return rslt;
}

// ������������ ������ ��-��� ���� "��� �����" ��� ����, ���������� �� ����� ���������
nbMETHOD_DEF(SignProcessor,releaseBase)(nbTHISc nbMatrix *any) {
  if (any) {
    delete (NbMatrix*)any;
  }
  return nbS_OK;
}

///////////////////////nbIConfigurate///////////////////////////////////

// �������� ������� ������������
nbMETHOD_DEF(SignProcessor,setConfig)(nbUuid page, nbData opt) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// ������ ������� ������������
nbMETHOD_DEF(SignProcessor,getConfig)(nbUuid page, nbData opt) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// ������ ������������ "�� ���������"
nbMETHOD_DEF(SignProcessor,getDefaultConfig)(nbUuid page, nbData opt) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// ��������� ������������
nbMETHOD_DEF(SignProcessor,tuneUp)(nbHandle handle, nbUuid page, nbData opt, nbIEventHandler **guihandler) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// ������������ ������� ������������, ������� ����� ��������� ��� ���������� ����������
nbMETHOD_DEF(SignProcessor,enumPages)(nbIEventHandler *eh, nbRefUuid iid) {
  return nbS_OK;
}

// ��������� �������� �������� ������������
nbMETHOD_DEF(SignProcessor,getTitle)(nbUuid page, nbData name, nbData descr) {
  return nbS_OK;
}
