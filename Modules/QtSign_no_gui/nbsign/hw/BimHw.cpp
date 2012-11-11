/****************************************************************************
** ����:         BimHw.cpp
** ������:
** ����:
** �������:
** �����:
**
** ��������:     ����������� ��������������� ������ (��), �������� ���
**               ��������, � ����� ����������� ����� ������ � ��������.
****************************************************************************/

#include <string.h>
#include "BimHw.h"

// �����������
BimHw::BimHw():_bim(nbNULL){

}
BimHw::BimHw(uint16_t num, nbTabletOptions params):_bim(nbNULL) {
  create(num,params);
}

// ����������
BimHw::~BimHw(){
  free();
}

//����������: ��������� ������
//���������:
//  num    - [��] ���������� �����
//  params - [��] ��������� ��������
//
bool BimHw::create(uint16_t num, nbTabletOptions params) {
  free();

  // �������� ������
  uint32_t size = sizeof(nbBimHwHeader)+ num*sizeof(nbPointHw); // ������ ��������� + � + Y + P + T

  nbBimHwHeader *img = (nbBimHwHeader*) new uint8_t[size];
  if (!img) return false;

  img->num     = num;
  img->maxX    = params.maxX;
  img->maxY    = params.maxY;
  img->maxP    = params.maxP;
  img->dpiX    = (uint16_t)(params.ppiX*params.maxX/params.scrW);
  img->dpiY    = (uint16_t)(params.ppiY*params.maxY/params.scrH);
  memset(img->phrase, 0, sizeof(img->phrase));
  img->size    = size-sizeof(nbBim);
  img->bimType = nbBIM_TYPE_HANDWR;

  _bim = img;
  return true;
}

// ������������ ������
bool BimHw::free(){
  if (!empty()) { delete[] _bim; _bim = nbNULL; }
  return true;
}

// ����������� ����������� ������
bool BimHw::fromBim(const nbBim* bim){
  free();
  if (!bim) return true;

  _bim = (nbBim*)new uint8_t[bim->size+sizeof(nbBim)];
  if (!_bim) return false;
  for (uint32_t i=0; i<(bim->size+sizeof(nbBim)); ++i){
    *((uint8_t*)_bim+i) = *((uint8_t*)bim+i);
  }
  return true;
}

// ������������� ��������������� ������
bool BimHw::attach(nbBim *bim) {
  free();
  _bim = bim;
  return true;
}

// ������������ ��������������� ������
nbBim* BimHw::detach() {
  nbBim *bim = _bim;
  _bim = nbNULL;
  return bim;
}

// ������ � ��������� ����������� ������
nbBimHwHeader* BimHw::header() const {
  return (nbBimHwHeader*)_bim;
}

// ������ � ������ ����������� ������
uint8_t* BimHw::data() const {
  if (!empty()) return (uint8_t*)(header()+1);
  return nbNULL;
}

// ������ � ����������� ������
nbBim* BimHw::handle() const {
  return _bim;
}

// ���
nbUuid BimHw::type() const {
  if (!empty()) return _bim->bimType;
  return nbUUID_NIL;
}

// ������
uint32_t BimHw::size() const {
  if (!empty()) return (_bim->size+sizeof(nbBim));
  return nbNIL;
}

// �������� ������� ������
uint32_t BimHw::empty() const {
  return (_bim == nbNULL);
}
