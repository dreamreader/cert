 ///////////////////////////////////////////////////////////////////////////////
//файл:         nbcTypes.cpp
//версия:       1.0
//дата:
//изменен:
//автор:
//
//цель:         Основные типы данных НПБК
///////////////////////////////////////////////////////////////////////////////

#include <quuid.h>
#include "nbcTypes.h"

const NbSlotID NbSlotID::out = NbSlotID(nbSLOT_OUT);

//...........................................................................
//  Уникальный универсальный идентификатор
//...........................................................................

bool operator ==(const nbUuid &id1, const nbUuid &id2){
  return (0==memcmp(&id1,&id2,sizeof(nbUuid)));
}
bool operator !=(const nbUuid &id1, const nbUuid &id2){
  return (0!=memcmp(&id1,&id2,sizeof(nbUuid)));
}

NbUuid::NbUuid(){ erase(); }
NbUuid::NbUuid(const nbUuid &id){ memcpy(this,&id,sizeof(nbUuid)); }

//Генерация идентификатора, связанного с вычислительной машиной
bool NbUuid::generate(){
  QUuid id = QUuid::createUuid();
  d1 = id.data1;
  d2 = id.data2;
  d3 = id.data3;
  memcpy(d4,id.data4,8*sizeof(char));
  return true;
}

//Генерация нулевого идентификатора
void NbUuid::erase(){
  *this = nbUUID_NIL;
}

//Проверка на нулевое значение
bool NbUuid::nil() const {
  return (0==memcmp(this,&nbUUID_NIL,sizeof(nbUuid)));
}

//Преобразование в строку
QString NbUuid::toString() const {
  QUuid id(d1,d2,d3,d4[0],d4[1],d4[2],d4[3],d4[4],d4[5],d4[6],d4[7]);
  return id.toString();
}

//Преобразование из строки
bool NbUuid::fromString(const QString &s){
  QUuid id(s);
  d1 = id.data1;
  d2 = id.data2;
  d3 = id.data3;
  memcpy(d4,id.data4,8*sizeof(char));
  return id.isNull();
}
    
//.............................................................................
//  Блок данных
//.............................................................................

NbData::NbData():_data(nbNULL),_capacity(0),_isAttached(false){
}

NbData::NbData(uint32_t size):_data(nbNULL),_capacity(0),_isAttached(false){
  resize(size);
}

NbData::NbData(const nbData data, bool isAttached):_data(nbNULL),_capacity(0),_isAttached(false){
  if (isAttached)   attach(data);
  else              copyFrom(data);
}

NbData::NbData(const NbData &source):_data(nbNULL),_capacity(0),_isAttached(false){
  copyFrom(source);
}

NbData::NbData(const QString &s):_data(nbNULL),_capacity(0),_isAttached(false){
  fromString(s);
}

NbData::~NbData(){
  free();
}

//Присоединение блока
//  - нельзя использовать resize для присоединенного блока
void NbData::attach(nbData data) {
  free();
  _data       = data;
  _capacity   = getWishfulSize();
  _isAttached = true;
}

//Отсоединение блока
nbData NbData::detach(){
  nbData data = _data;
  _data       = nbNULL;
  _capacity   = 0;
  _isAttached = false;
  return data; 
}

//Копирование блока данных из nbData
void NbData::copyFrom(const nbData data){
  NbData source(data,true);
  resize(source.size());
  if (empty()) return;
  memcpy(_data,(nbData)source,source.size());
}

//Копирование блока данных из массива определенной длины
void NbData::copyFrom(const void* data, uint32_t size){
  resize(size);
  if (empty()) return;
  memcpy(_data,data,size);
}

//Копирование блока данных в массив
bool NbData::copyTo(void* data, uint32_t size) const{
  if (!data) return false;
  if (size > this->size()) return false;
  memcpy(data,_data,size);
  return true;
}
  
bool NbData::empty() const {
  if (_data == nbNULL) return true;
  return (size()==0);
}

//Освобождение блока данных
void NbData::free(){
  if (!_isAttached && _data){
    delete[] ((uint8_t*)_data-sizeof(uint32_t));
  }
  _data       = nbNULL;
  _capacity   = 0;
  _isAttached = false;
}

//Освобождение блока данных, даже если он присоединен
void NbData::freeForced(){
  _isAttached = false;
  free();
}

//Изменение размера блока
//  - для присоединенных блоков размер может изменяться только в сторону уменьшения
bool NbData::resize(uint32_t size){
  if (_capacity < size)
    if (!reserve(size)) return false;
  setWishfulSize(size);
  setDataTerminal(size);
  return true;
}

//Установка в поле "размер" желательного размера блока без фактического изменения размера
void NbData::setWishfulSize(uint32_t size){
  if (_data)
    *(uint32_t*)(_data-sizeof(uint32_t)) = size;
}

//Размер блока
uint32_t NbData::size() const {
  if (!_data) return 0;
  return *(uint32_t*)(_data-sizeof(uint32_t));
}

//Получение желаемого размера строки
uint32_t NbData::getWishfulSize() const {
  return size();
}

//Изменение емкости
bool NbData::reserve(uint32_t capacity){
  if (_capacity >= capacity) return true;
  if (_isAttached) return false;
  free();
  _data = new uint8_t[sizeof(uint32_t) + capacity + sizeof(uint16_t)];
  if (!_data) return false;
  _data += sizeof(uint32_t);
  setWishfulSize(0);
  setDataTerminal(capacity);
  _capacity = capacity;
  return true;
}

//Сравнение (0 (c==d), >0 (c>d), <0 (c<d))
int32_t NbData::compare(const NbData &d) const {
  //Сравнить по длине
  if (_data == d._data) return 0;
  //Сравнить по указателю
  int32_t s = (int32_t)size() - (int32_t)d.size();
  if (s != 0 || !size()) return s;
  //Сравнить по одинаковому блоку
  return memcmp(_data,d._data,min(size(),d.size()));
}

//Преобразование данных, представляющих строку utf-8, в строку utf-16
QString NbData::toString() const {
  if (empty()) return QString();
  return QString::fromUtf8((const char*)_data,size());
}

//Преобразование данных из строки utf-16 в строку utf-8
bool NbData::fromString(const QString &s){
  QByteArray ba = s.toUtf8();
  if (!resize(ba.size())) return false;
  memcpy(_data,ba.data(),ba.size());
  return true;
}

//Определение текущей длины строки в символах utf-8
uint32_t NbData::getStringLength() const {
  if (empty()) return 0;
  return QString::fromUtf8((const char*)_data,size()).size();
}

//.............................................................................
NbBlockHeader::NbBlockHeader(nbBlockHeader bh){
  blockType = bh.blockType;
  sizeHi    = bh.sizeHi;
  sizeLo    = bh.sizeLo;
}

uint32_t NbBlockHeader::bodySize() const{
  return ((uint32_t)sizeHi<<16) + (uint32_t)sizeLo;
}

void NbBlockHeader::bodySize(uint32_t size){
  sizeHi = (uint8_t)(size>>16);
  sizeLo = (uint16_t)(size);
}

nbBlockType NbBlockHeader::type() const {
  return blockType;
}

void NbBlockHeader::type(nbBlockType bt){
  blockType = bt;
}

//.............................................................................
//  Метаинформация
//.............................................................................

NbMeta::NbMeta(){
  count = 0;
  format= nbMF_ANY;
  type  = nbMT_ANY;
}

NbMeta::NbMeta(const nbMeta &mi){
  (nbMeta&)*this = mi;
}

NbMeta::NbMeta(uint32_t count, nbMetaFormat format, nbMetaType type){
  this->count = count;
  this->format= format;
  this->type  = type;
}

//Проверка, является ли метаописание подмножеством range
bool NbMeta::subrange(const NbMeta &range) const {
  if (((count == range.count)  || range.anyCount()) &&
      ((format == range.format)|| range.anyFormat()) &&
      ((type == range.type)    || range.anyType()))
    return true;
  return false;
}

//Проверка содержит ли метаописание диапазон
bool NbMeta::ranged() const {
  return (anyFormat() || anyType() || anyCount());
}

//Метаописание переменное по типу
bool NbMeta::anyType() const {
  return type == nbMT_ANY;
}

//Метаописание переменное по числу элементов
bool NbMeta::anyCount() const {
  return count == nbNULL;
}

//Метаописание переменное по формату
bool NbMeta::anyFormat() const {
  return format == nbMF_ANY;
}

//Сравнение
bool NbMeta::equal(const NbMeta &mi) const {
  return ((count == mi.count) &&
          (format== mi.format) &&
          (type  == mi.type));
}

//Метаинформация не определена
bool NbMeta::any() const {
  return equal(NbMeta(0,nbMF_ANY,nbMT_ANY));
}

const NbMeta NbMeta::nil(){
  static NbMeta meta(0,nbMF_ANY,nbMF_ANY);
  return meta;
}

//Длина элемента ВБП в битах
uint32_t NbMeta::sizeItem() const {
  //Таблица, позволяющая выполнить переход от размера элемента в бита к размеру
  static const uint8_t sizes[16*8] = {
  //0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    0, 1, 2, 4, 8,16,32,64, 0, 0, 0, 0, 0, 0, 0, 0, // 0
    0, 1, 2, 4, 8,16,32,64, 0, 0, 0, 0, 0, 0, 0, 0, // 16
    0, 0, 0, 0, 0,16,32,64,80, 0, 0, 0, 0, 0, 0, 0, // 32
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 112
  };
  //(должна выполняться проверка на размер 0)
  return sizes[format&0x7F];
}

//Длина ВБП в байтах с выравниванием по байту
uint32_t NbMeta::size() const {
  uint32_t bits = sizeItem()*count;
  return ((bits>>3) + (0 != (bits & 7)));
}

//...........................................................................
//  Вектор биометрических параметров (ВБП)
//...........................................................................

//Функции чтения и записи для работы с элементами из 1,2,4 бит,
//интерпретирующимися как беззнаковые

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
bool Vbp::conv(const Vbp &vec, uint32_t count, nbMetaFormat mfTo, nbMetaFormat mfFrom){
  switch (mfFrom){
    case nbMF_U1: 
      switch (mfTo){
        case nbMF_R32:  convR32fromU1(vec,count); return true;
      }
      break;
    case nbMF_I1:
      switch (mfTo){
        case nbMF_R32:  convR32fromI1(vec,count); return true;
      }
      break;
    case nbMF_R32:
      switch (mfTo){
        case nbMF_U1:   convU1fromR32(vec,count); return true;
        case nbMF_I1:   convI1fromR32(vec,count); return true;
        case nbMF_U16:  convU16fromR32(vec,count); return true;
        case nbMF_I16:  convI16fromR32(vec,count); return true;
        case nbMF_I32:  convI32fromR32(vec,count); return true;
        case nbMF_U32:  convU32fromR32(vec,count); return true;
      }
      break;
    case nbMF_U16:
      switch (mfTo){
        case nbMF_R32:  convR32fromU16(vec,count); return true;
      }
      break;
    case nbMF_I16:
      switch (mfTo){
        case nbMF_R32:  convR32fromI16(vec,count); return true;
      }
      break;
    case nbMF_I32:
      switch (mfTo){
        case nbMF_R32:  convR32fromI32(vec,count); return true;
      }
      break;
    case nbMF_U32:
      switch (mfTo){
        case nbMF_R32:  convR32fromU32(vec,count); return true;
      }
      break;
  }
  //Скопировать, если совпадают
  if (mfTo == mfFrom){
    copy(vec.u8,NbMeta(count,mfTo,nbMT_ANY).size());
  }
  return false;
}

void Vbp::copy(uint8_t *data, uint32_t size){
  memcpy(i8,data,size);
}

//...........................................................................
// Матрица ВБП
//...........................................................................

//Изменение размеров, если это возможно (т.е. матрица определена классом NbMatrix)
bool NbMatrix::Row::resize(uint32_t ncols, NbMeta meta){
  //Если матрица, не связана со столбцом, связать заново
  if (!_handle) {
    NbMatrix *m = new NbMatrix(ncols,meta);
    if (!m) return false;
    _ref  = m; _irow = 0; _handle = (nbHandle)m;
    return true;
  } else {
    NbMatrix *m = (NbMatrix*)_handle;
    if (!m) return false;
    //не разрешено во время изменения размера изменять и метаописание
    if (m->meta(0) != meta) return false;
    return m->resizeData(ncols);
  }
}

//Принужденное освобождение связанной матрицы
void NbMatrix::Row::free(){
  if (_handle) delete (NbMatrix*)_handle;
  _ref    = nbNULL;
  _irow   = 0;
  _handle = nbNULL;
}

//Заполнение вектора нулями
void NbMatrix::Row::tideup(){
  if (!_ref) return;

  //Выполнить для всех примеров столбца
  uint32_t ncols = count();
  uint32_t vsize = meta().size();
  for (uint32_t icol=0; icol<ncols; ++icol){
    Vbp &v = (Vbp&)_ref->data[_irow][icol];
    memset(v.u8,0xAA,vsize);  //первое обнуление 10101010
    memset(v.u8,0x55,vsize);  //второе обнуление 01010101
    memset(v.u8,0x00,vsize);  //третье обнуление 00000000
  }
}

NbMatrix::NbMatrix(){
  nbMatrix::nrows = 0;
  nbMatrix::ncols = 0;
  nbMatrix::meta  = nbNULL;
  nbMatrix::data  = nbNULL;
  _capacity = 0;
}

//Конструктор полной матрицы
NbMatrix::NbMatrix(uint32_t nrows, uint32_t ncols, const NbMeta *meta){
  nbMatrix::nrows = 0;
  nbMatrix::ncols = 0;
  nbMatrix::meta  = nbNULL;
  nbMatrix::data  = nbNULL;
  _capacity = 0;

  if (!resizeMeta(nrows,meta)) return;
  resizeData(ncols);
}

//Конструктор для вектора
NbMatrix::NbMatrix(uint32_t ncols, const NbMeta meta){
  nbMatrix::nrows = 0;
  nbMatrix::ncols = 0;
  nbMatrix::meta  = nbNULL;
  nbMatrix::data  = nbNULL;
  _capacity = 0;
  if (!resizeMeta(1,&meta)) return;
  resizeData(ncols);
}

//Конструктор с конвертацией формата
//(формат устанавливается для всех вложенных векторов)
NbMatrix::NbMatrix(const NbMatrix &m, nbMetaFormat f){
  nbMatrix::nrows = 0;
  nbMatrix::ncols = 0;
  nbMatrix::meta  = nbNULL;
  nbMatrix::data  = nbNULL;
  _capacity = 0;

  if (!m.nrows()) return;
  //Выделить память под метаописания
  vector<NbMeta> vecMeta(m.nrows());
  //Скопировать измененные метаописания
  for (uint32_t i=0; i<m.nrows(); ++i){
    vecMeta[i] = m.meta(i);
    vecMeta[i].format = f;
  }
  if (!resizeMeta(m.nrows(),&vecMeta[0])) return;
  if (!resizeData(m.ncols())) return;
  //Скопировать данные, преобразовав к новому формату
  convert(m);
}

//Конструктор с конвертацией формата
NbMatrix::NbMatrix(const NbMatrix::Row &r, nbMetaFormat f){
  nbMatrix::nrows = 0;
  nbMatrix::ncols = 0;
  nbMatrix::meta  = nbNULL;
  nbMatrix::data  = nbNULL;
  _capacity = 0;

  NbMeta newMeta = r.meta();
  newMeta.format = f;

  if (!resizeMeta(1,&newMeta)) return;
  if (!resizeData(r.count())) return;
  //Скопировать данные, преобразовав к новому формату
  if (!row(0).convert(r)) return;
}


NbMatrix::~NbMatrix(){
  freeMeta();
}

//Максимальная емкость
uint32_t NbMatrix::capacity() const {
  return _capacity;
}

bool NbMatrix::valid() const {
  return (nbMatrix::nrows <= 0x0000FFFF);
};

bool NbMatrix::empty() const {
  return (nbMatrix::nrows == 0 && nbMatrix::ncols == 0);
}

bool NbMatrix::pattern() const {
  return (nbMatrix::meta != nbNULL);
}

bool NbMatrix::full() const {
  return (nbMatrix::nrows != 0 && nbMatrix::ncols != 0);
}

NbMatrix::Row NbMatrix::row(uint32_t irow) const{
  return NbMatrix::Row(this,irow,(nbHandle)this);
}

Vbp NbMatrix::at(uint32_t irow, uint32_t icol) const {
  return (Vbp)nbMatrix::data[irow][icol];
}

Vbp&   NbMatrix::at(uint32_t irow, uint32_t icol){
  return (Vbp&)nbMatrix::data[irow][icol];
}

Vbp** NbMatrix::data() const {
  return (Vbp**)nbMatrix::data;
}

uint32_t NbMatrix::ncols() const {
  return nbMatrix::ncols;
}

uint32_t NbMatrix::nrows() const {
  return nbMatrix::nrows;
}

NbMeta* NbMatrix::meta() const {
  return (NbMeta*)nbMatrix::meta;
}

NbMeta  NbMatrix::meta(uint32_t irow) const {
  return (NbMeta)nbMatrix::meta[irow];
}

//Выделение памяти под метаописание
bool NbMatrix::resizeMeta(uint32_t nrows, const NbMeta *meta){
  freeMeta();
  if (nrows > 0x0000FFFF) return false;
  //Выделить память под метаописания
  if (!nrows) return true;
  nbMatrix::meta  = new nbMeta[nrows];
  if (!nbMatrix::meta) return false;
  nbMatrix::nrows = nrows;
  memcpy(nbMatrix::meta,meta,sizeof(nbMeta)*nrows);
  return true;
}

//Выделение памяти под данные
bool NbMatrix::resizeData(uint32_t ncols){
  //Проверить необходимость изменения размера
  if (ncols<=capacity()){
    nbMatrix::ncols = ncols;
    return true;
  }
  freeData();
  if (!ncols || !nbMatrix::nrows) return true;
  //В двойном цикле выделить память под вектора
  nbMatrix::data = new nbVbp*[nbMatrix::nrows];
  if (!nbMatrix::data) return false;
  memset(nbMatrix::data,0,sizeof(nbVbp*)*nbMatrix::nrows);
  _capacity = ncols;
  nbMatrix::ncols = ncols;
  for (uint32_t irow=0; irow<nbMatrix::nrows; ++irow){
    nbMatrix::data[irow] = new nbVbp[ncols];
    if (!nbMatrix::data[irow]) return false;
    memset(nbMatrix::data[irow],0,sizeof(nbVbp)*ncols);
    if (!resizeVbp(irow, nbMatrix::meta[irow])) return false;
  }
  return true;
}

//Перевыделение памяти под строку при изменении метаданных 
//(память под метаданные и данные должна быть выделена ранее)
bool NbMatrix::resizeVbp(uint32_t irow, NbMeta meta){
  if (!nbMatrix::ncols || !nbMatrix::nrows) return true;
  freeVbp(irow);
  nbMatrix::meta[irow] = meta;
  uint32_t vsize = meta.size();
  if (vsize)
  for (uint32_t icol=0; icol<nbMatrix::ncols; ++icol){
    nbMatrix::data[irow][icol] = (nbVbp)new uint8_t[vsize];
    if (!nbMatrix::data[irow][icol]) return false;
  }
  return true;
}

void NbMatrix::free(){
  freeMeta();
}

void NbMatrix::freeMeta(){
  if (!nbMatrix::nrows) return;
  freeData();
  delete[] nbMatrix::meta;
  nbMatrix::meta = nbNULL;
  nbMatrix::nrows= 0;
}

void NbMatrix::freeData(){
  if (!nbMatrix::ncols) return;
  for (uint32_t irow=0; irow<nbMatrix::nrows; ++irow){
    freeVbp(irow);
    delete[] nbMatrix::data[irow];
  }
  delete[] nbMatrix::data;
  nbMatrix::data = nbNULL;
  nbMatrix::ncols= 0;
  _capacity = 0;
}

void NbMatrix::freeVbp(uint32_t irow){
  if (!nbMatrix::ncols || !nbMatrix::nrows) return;
  for (uint32_t icol=0; icol<nbMatrix::ncols; ++icol){
    if (nbMatrix::data[irow][icol]) {
      delete[] (uint8_t*)nbMatrix::data[irow][icol];
      nbMatrix::data[irow][icol] = nbNULL;
    }
  }    
}

bool NbMatrix::copyVbp(uint32_t irow, const Row &row){
  //Проверить размеры
  if (!nbMatrix::nrows || nbMatrix::ncols!=row.count()){
    //Не совпадает размер
    return false; //throw nbE(nbF_DEF,nbECODE_UNBOUND_DATA);
  }
  if (!nbMatrix::ncols) return true;
  //Проверить наличие записей ВБП
  if ((row[0].v==nbNULL) != (nbMatrix::data[irow][0] == nbNULL)){
    return false; //throw nbE(nbF_DEF,nbECODE_UNBOUND_DATA);
  }

  uint32_t vsize = meta(irow).size();
  if (vsize)
  for (uint32_t icol=0; icol<nbMatrix::ncols; ++icol){
    memcpy(nbMatrix::data[irow][icol],row[icol].v,vsize);
  }
  return true;
}

bool NbMatrix::copyVbp(const nbMatrix &m){
  if ((void*)this == (void*)&m)
    return true;
  //Проверить размеры (если не совпадают, плохо)
  if ((nbMatrix::nrows!=m.nrows) || (nbMatrix::ncols != m.ncols)){
    return false; //throw nbE(nbF_DEF,nbECODE_UNBOUND_DATA);
  }
  //Проверить метаданные
  for (uint32_t irow=0; irow<m.nrows; ++irow){
    if ((NbMeta)nbMatrix::meta[irow] != (NbMeta)m.meta[irow])
      return false;//throw nbE(nbF_DEF,nbECODE_UNBOUND_DATA);
  }
  //Скопировать данные, если заданы
  for (uint32_t irow=0; irow<nbMatrix::nrows; ++irow){
    uint32_t vsize = ((NbMeta)nbMatrix::meta[irow]).size();
    if (vsize)
    for (uint32_t icol=0; icol<nbMatrix::ncols; ++icol){
      memcpy(nbMatrix::data[irow][icol],m.data[irow][icol],vsize);
    }
  }
  return true;
}


bool NbMatrix::copy(const nbMatrix &m){
  if ((nbMatrix*)this == &m)
    return true;
  if (!resizeMeta(m.nrows,(NbMeta*)m.meta)) return false;
  if (!resizeData(m.ncols)) return false;
  return copyVbp(m);
}

//Конвертация матрицы из одного формата в другой
//(число векторов и число элементов вектора должны совпадать)
bool NbMatrix::convert(const NbMatrix &m){
  if (this == &m)
    return true;
  if (m.nrows() != nrows()) return false;//throw nbE(nbF_DEF,nbECODE_META);
  //Для каждого вектора
  for (uint32_t irow=0; irow<nrows(); ++irow){
    row(irow).convert(m.row(irow));
  }
  return true;
}

//Копирование вектора с данными (формат представления значения не имеет, проверятся лишь равенство по размеру)
bool NbMatrix::Row::copy(const NbMatrix::Row &row){
  //Выйти, если источник и приемник совпадают
  if (row._handle == _handle && row._irow==_irow) 
    return true;

  uint32_t vsize = meta().size();
  uint32_t nrows = count();
  //Проверить размер вектора
  if (meta().count != row.meta().count) return false;//throw nbE(nbF_DEF,nbECODE_META);
  if (vsize != row.meta().size()) return false;//throw nbE(nbF_DEF,nbECODE_META);
  if (nrows != row.count()) return false;//throw nbE(nbF_DEF,nbECODE_META);

  if (vsize)
  for (uint32_t j=0; j<nrows; ++j){
    memcpy(at(j).u8,row.at(j).u8,vsize);
  }
  return true;
}

//Преобразование данных из одного формата в другой
//(преобразование заменяется копированием, если нет специальной функции преобразования)
bool NbMatrix::Row::convert(const NbMatrix::Row &row){
  //Выйти, если источник и приемник совпадают
  if (row._handle == _handle && row._irow==_irow) 
    return true;
  //Проверить совпадение числа элементов и числа примеров в строке
  uint32_t ncols = count();
  uint32_t ecols = meta().count;

  if ((ncols != row.count()) || (ecols != row.meta().count))
    return false;//throw nbE(nbF_DEF,nbECODE_META);
  
  //Выполнить преобразование для каждого элемента каждого примера вектора
  //(для преобразований, который отличаются только интерпретацией, использовать копирование)
  nbMetaFormat from = row.meta().format;
  nbMetaFormat to = meta().format;

  switch (from){
    case nbMF_U1: 
      switch (to){
        case nbMF_R32:  for (uint32_t j=0; j<ncols; ++j) at(j).convR32fromU1(row.at(j),ecols); return true;
      }
      break;
    case nbMF_I1:
      switch (to){
        case nbMF_R32:  for (uint32_t j=0; j<ncols; ++j) at(j).convR32fromI1(row.at(j),ecols); return true;
      }
      break;
    case nbMF_R32:
      switch (to){
        case nbMF_U1:   for (uint32_t j=0; j<ncols; ++j) at(j).convU1fromR32(row.at(j),ecols); return true;
        case nbMF_I1:   for (uint32_t j=0; j<ncols; ++j) at(j).convI1fromR32(row.at(j),ecols); return true;
        case nbMF_U16:  for (uint32_t j=0; j<ncols; ++j) at(j).convU16fromR32(row.at(j),ecols); return true;
        case nbMF_I16:  for (uint32_t j=0; j<ncols; ++j) at(j).convI16fromR32(row.at(j),ecols); return true;
        case nbMF_I32:  for (uint32_t j=0; j<ncols; ++j) at(j).convI32fromR32(row.at(j),ecols); return true;
        case nbMF_U32:  for (uint32_t j=0; j<ncols; ++j) at(j).convU32fromR32(row.at(j),ecols); return true;
      }
      break;
    case nbMF_U16:
      switch (to){
        case nbMF_R32:  for (uint32_t j=0; j<ncols; ++j) at(j).convR32fromU16(row.at(j),ecols); return true;
      }
      break;
    case nbMF_I16:
      switch (to){
        case nbMF_R32:  for (uint32_t j=0; j<ncols; ++j) at(j).convR32fromI16(row.at(j),ecols); return true;
      }
      break;
    case nbMF_I32:
      switch (to){
        case nbMF_R32:  for (uint32_t j=0; j<ncols; ++j) at(j).convR32fromI32(row.at(j),ecols); return true;
      }
      break;
    case nbMF_U32:
      switch (to){
        case nbMF_R32:  for (uint32_t j=0; j<ncols; ++j) at(j).convR32fromU32(row.at(j),ecols); return true;
      }
      break;
  }
  //Скопировать, если форматы идентичны по размеру
  return copy(row);
}
