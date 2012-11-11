//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:
//
//цель:
//
//
//
//////////////////////////////////////////////////////////////////////////
#include "Vbp.h"

namespace Nb {

//Функции чтения и записи для работы с элементами из 1,2,4 бит,
//интерпретирующимися как беззнаковые

bool Vbp::attach(nbVbp vec){
  if (v) return false;
  v = vec;
  return true;
}

nbVbp Vbp::detach(){
  nbVbp vec = v;
  v = Null;
  return vec;
}

bool Vbp::create(uint32_t size){
  if (!size) { v= Null; return true; }
  u8 = new uint8_t[size];
  return (u8!=Null);
}

bool Vbp::create(Meta meta){
  return create(meta.size());
}

void Vbp::free(){
  if (v) { 
    delete[] u8; 
    u8 = 0;
  }
}

bool Vbp::isEmpty(){
  return (v==Null);
}

//Получение смещенного вектора
Vbp Vbp::shift(int32_t size) const{
  return (Vbp)(u8 + size);
}


uint32_t Vbp::u1(uint32_t pos) const {
  uint32_t ibyte = pos >> 3;
  uint32_t ibit  = pos & 7;
  return (uint32_t)((i8[ibyte] >> ibit) & 1);
}

void Vbp::u1(uint32_t pos, uint32_t val){
  uint32_t ibyte = pos >> 3;
  uint32_t ibit  = pos & 7;
  i8[ibyte] = (uint8_t)((i8[ibyte] & ~(1 << ibit)) | (val << ibit));
}

uint32_t Vbp::u2(uint32_t pos) const{
  uint32_t ibyte = pos >> 2;
  uint32_t ibit  = pos & 3;
  return (uint32_t)((i8[ibyte] >> ibit) & 3);
}

void Vbp::u2(uint32_t pos, uint32_t val){
  uint32_t ibyte = pos >> 2;
  uint32_t ibit  = pos & 3;
  i8[ibyte] = (uint8_t)((i8[ibyte] & ~(3 << ibit)) | (val << ibit));
}

uint32_t Vbp::u4(uint32_t pos) const{
  uint32_t ibyte = pos >> 1;
  uint32_t ibit  = pos & 1;
  return (uint32_t)((i8[ibyte] >> ibit) & 15);
}

void Vbp::u4(uint32_t pos, uint32_t val){
  uint32_t ibyte = pos >> 1;
  uint32_t ibit  = pos & 1;
  i8[ibyte] = (uint8_t)((i8[ibyte] & ~(15 << ibit)) | (val << ibit));
}

//Функции чтения и записи для работы с элементами из 1,2,4 бит,
//интерпретирующимися как знаковые

//возвращает -1 или 1
int32_t Vbp::i1(uint32_t pos) const {
  uint32_t ibyte = pos >> 3;
  uint32_t ibit  = pos & 7;
  return ((int32_t)((i8[ibyte] >> ibit) & 1))*2-1;
}

//устанавливает >0 и <=0 
void Vbp::i1(uint32_t pos, int32_t val){
  uint32_t ibyte = pos >> 3;
  uint32_t ibit  = pos & 7;
  i8[ibyte] = (uint8_t)((i8[ibyte] & ~(1 << ibit)) | ((val>0) << ibit));
}

//Функции преобразования основных форматов представления

void Vbp::convU1fromR32(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    u1(i,(vec.r32[i]==0.0f)?0:1);
}

void Vbp::convR32fromU1(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    r32[i] = (float)vec.u1(i);
}

void Vbp::convI1fromR32(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    i1(i,(vec.r32[i]>0.0f)?1:-1);
}

void Vbp::convR32fromI1(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    r32[i] = (float)vec.i1(i);
}

void Vbp::convU16fromR32(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    u16[i] = (uint16_t)vec.r32[i];
}

void Vbp::convR32fromU16(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    r32[i] = (float)vec.u16[i];
}

void Vbp::convI16fromR32(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    i16[i] = (int16_t)vec.r32[i];
}

void Vbp::convR32fromI16(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    r32[i] = (float)vec.i16[i];
}

void Vbp::convI32fromR32(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    i32[i] = (int32_t)vec.r32[i];
}

void Vbp::convR32fromI32(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    r32[i] = (float)vec.i32[i];
}

void Vbp::convU32fromR32(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    u32[i] = (uint32_t)vec.r32[i];
}

void Vbp::convR32fromU32(const Vbp &vec, uint32_t count) {
  for (uint32_t i=0; i!=count; ++i)
    r32[i] = (float)vec.u32[i];
}

//Унивесальное преобразование
bool Vbp::conv(const Vbp &vec, uint32_t count, MetaFormat mfTo, MetaFormat mfFrom){
  switch (mfFrom){
    case MfU1: 
      switch (mfTo){
        case MfR32:  convR32fromU1(vec,count); return true;
        default: break;
      }
      break;
    case MfI1:
      switch (mfTo){
        case MfR32:  convR32fromI1(vec,count); return true;
        default: break;
      }
      break;
    case MfR32:
      switch (mfTo){
        case MfU1:   convU1fromR32(vec,count); return true;
        case MfI1:   convI1fromR32(vec,count); return true;
        case MfU16:  convU16fromR32(vec,count); return true;
        case MfI16:  convI16fromR32(vec,count); return true;
        case MfI32:  convI32fromR32(vec,count); return true;
        case MfU32:  convU32fromR32(vec,count); return true;
        default: break;
      }
      break;
    case MfU16:
      switch (mfTo){
        case MfR32:  convR32fromU16(vec,count); return true;
        default: break;
      }
      break;
    case MfI16:
      switch (mfTo){
        case MfR32:  convR32fromI16(vec,count); return true;
        default: break;
      }
      break;
    case MfI32:
      switch (mfTo){
        case MfR32:  convR32fromI32(vec,count); return true;
        default: break;
      }
      break;
    case MfU32:
      switch (mfTo){
        case MfR32:  convR32fromU32(vec,count); return true;
        default: break;
      }
      break;
      default: break;
  }
  //Скопировать, если совпадают
  if (mfTo == mfFrom){
    copy(vec.u8,Meta(count,mfTo,MtAny).size());
  }
  return false;
}

void Vbp::copy(Vbp vbp, uint32_t size){
  for (uint32_t i=0;i<size;++i)
    *(i8+i) =  *(vbp.u8+i);
}

}
