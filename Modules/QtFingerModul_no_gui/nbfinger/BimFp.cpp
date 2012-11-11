#include "BimFp.h"

//�����������
BimFp::BimFp():_bim(0){

}

BimFp::BimFp(uint16_t width, uint16_t height, uint8_t format, uint8_t action):_bim(0){
  create(width,height,format,action);
}

//����������
BimFp::~BimFp(){
  free();
}

//��������� ������
bool BimFp::create(uint16_t width, uint16_t height, uint8_t format, uint8_t action){
  free();
  if (format != fpfGrey) return false;

  //�������� ������
  uint32_t size = sizeof(nbBimFpHeader) + width*height*sizeof(uint8_t);
  nbBimFpHeader *fp = (nbBimFpHeader*) new uint8_t[size];
  if (!fp) return false;
  fp->width   = width;
  fp->height  = height;
  fp->format  = format;
  fp->action  = action;
  fp->size    = size-sizeof(nbBim);
  //fp->size    = width*height;
  fp->bimType = nbBIM_TYPE_FINGER;
  _bim = fp;
  return true;
}

//������������ ������
bool BimFp::free(){
  if (!empty()) { delete[] _bim; _bim=0; }
  return true;
}

//����������� ��������� ������
bool BimFp::fromBim(const nbBim* bim){
  free();
  if (!bim) return true;

  _bim = (nbBim*)new uint8_t[bim->size+sizeof(nbBim)];
  if (!_bim) return false;
  for (uint32_t i=0; i<(bim->size+sizeof(nbBim)); ++i){
    *((uint8_t*)_bim+i) = *((uint8_t*)bim+i);
  }
  return true;
}

//����������� ����������� �� ������� ����� ����� width*height
bool BimFp::fromInt32(uint16_t width, uint16_t height, const int32_t *image){
  if (!create(width, height, fpfGrey, fpaNone)) return false;

  uint8_t *d = data();
  uint32_t length = width*height;
  for (uint32_t y=0; y<length; ++y, ++d, ++image){
    *d = (uint8_t)*image;
  }
  return true;
}

//����������� ����������� � ������ ����� ����� width*height
bool BimFp::toInt32(int32_t *image){
  if (empty()) return true;

  uint8_t *d = data();
  uint32_t length = header()->width*header()->height;
  for (uint32_t y=0; y<length; ++y, ++d, ++image){
    *image = (int32_t)*d;
  }
  return true;
}

//������������� ��������������� ������
bool BimFp::attach(nbBim *bim){
  free();
  _bim = bim;
  return true;
}

//������������ ��������������� ������
nbBim* BimFp::detach(){
  nbBim *bim = _bim;
  _bim = 0;
  return bim;
}

//������ � ��������� ��������� ������
nbBimFpHeader* BimFp::header() const {
  return (nbBimFpHeader*)_bim;
}

//������ � ����������� ��������� ������
uint8_t* BimFp::data() const {
  if (!empty()) return (uint8_t*)(header()+1);
  return 0;
}

nbBim* BimFp::handle() const {
  return _bim;
}

//���
nbUuid BimFp::type() const {
  if (!empty()) return _bim->bimType;
  return nbUUID_NIL;
}

//������
uint32_t BimFp::size() const {
  if (!empty()) return (_bim->size+sizeof(nbBim));
  return 0;
}

//�������� ������� ������
uint32_t BimFp::empty() const{
  return (_bim == 0);
}
