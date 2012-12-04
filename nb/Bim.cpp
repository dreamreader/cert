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
#include "Bim.h"

namespace Nb {

//Конструктор по умолчанию
Bim::Bim():_m(Null){
}

//Конструктор копирования
Bim::Bim(const Bim &bim):_m(Null){
  copy(bim.handle());
}

//Конструктор распаковки
Bim::Bim(const BimHandle handle):_m(Null){
  copy(handle);
}

//Деструктор
Bim::~Bim(){
  free();
}

Bim& Bim::operator=(const Bim& bim){
  copy(bim.handle());
  return *this;
}

bool Bim::isNull() const {
  return (_m == Null);
}

bool Bim::isEmpty() const {
  return (_m == Null || (int32_t)_m->size == Nil);
}

//Полный размер биометрического образа
int32_t  Bim::size() const {
  if (isNull()) return Nil;
  return (sizeof(nbBim) + _m->size);
}

//Размер данных
int32_t  Bim::sizeData() const {
  if (isNull()) return Nil;
  return _m->size;
}

//Тип
const Uuid&  Bim::type() const {
  if (isNull()) return Uuid::nil;
  return *(const Uuid*)&_m->bimType;
}

//Данные
uint8_t* Bim::data() const {
  if (isNull()) return Null;
  return (uint8_t*)(_m+1);
}


//Доступ к описателю
BimHandle Bim::handle() const {
  return _m;
}

//Присоединение
bool Bim::attach(BimHandle handle){
  if (!isNull()) return false;
  _m = handle;
  return true;
}

//Отсоединение
BimHandle Bim::detach(){
  BimHandle handle = _m;
  _m = Null;
  return handle;
}

//Выделение памяти
bool Bim::resize(const Uuid &type, int32_t dataSize){
  free();
  _m = (BimHandle)new uint8_t[sizeof(nbBim) + dataSize];
  if (!_m) return false;
  _m->bimType = type;
  _m->size    = dataSize;
  return true;
}

//Освобождение памяти
void Bim::free(){
  if (_m) { delete[] _m; _m = Null; }
}

//Копирование
bool Bim::copy(const BimHandle handle){
  if (_m == handle) return true;
  free();
  if (!handle) return true;
  //Скопировать образ
  _m = (BimHandle)new uint8_t[sizeof(nbBim) + handle->size];
  memcpy(_m,handle,sizeof(nbBim) + handle->size);
  return true;
}

//Запись образа в поток
int32_t Bim::write(QDataStream &to) const {
  if (isNull()) return -1;
  int32_t w = to.writeRawData((const char*)_m,size());
  if (to.status() != QDataStream::Ok) return -1;
  return w;
}

//Чтение из потока
int32_t Bim::read(QDataStream &from){
  nbBim header = {0, Uuid::nil};
  int32_t r=0;
  r += from.readRawData((char*)&header,sizeof(nbBim));
  if (!resize(header.bimType,header.size)) return -1;
  r += from.readRawData((char*)(_m+1),_m->size);
  if (from.status() != QDataStream::Ok) {free(); return -1; }
  return r;
}

#if !defined(QT_NO_DEBUG_STREAM)
QDebug operator<<(QDebug  dbg, const Bim &x){
  if (x.isNull()){
    return dbg.nospace()<<"(Nil)";
  } else {
    QDebug d = dbg.nospace();
    d <<"Bim("<<x._m<<": "<<x._m->size;
    if (x.size()>4){
      uint8_t *v = (uint8_t*)(x._m+1);
      QString s(" [%1%2%3%4..] ");
      QChar z = QLatin1Char('0');
      d << s.arg(v[0],2,16,z).arg(v[1],2,16,z).arg(v[2],2,16,z).arg(v[3],2,16,z).toAscii().data();
    }
    d <<" "<<Uuid(x._m->bimType)<<")";
    return d;
  }
}

QDebug operator<<(QDebug  dbg, const Bims &x){
  dbg.nospace() << '[';
  for (int i = 0; i < x.count(); ++i) {
    if (i) dbg << "\n";
    dbg << i<<":"<<x.at(i);
  }
  dbg << ']';
  return dbg.space();
}
#endif

}
