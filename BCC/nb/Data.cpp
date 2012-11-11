//////////////////////////////////////////////////////////////////////////
//файл:
//верси€:
//дата:
//изменен:
//автор:
//
//цель:
//
//
//
//////////////////////////////////////////////////////////////////////////
#include "Data.h"

namespace Nb {

  Data::Data():_m(Null),_capacity(0){
  }

  Data::Data(int32_t size):_m(Null),_capacity(0){
    resize(size);
  }

  Data::Data(const nbData data):_m(Null),_capacity(0){
    copy(data,*sizeByPtr(data));
  }

  Data::Data(const Data &source):_m(Null),_capacity(0){
    if (source._capacity>=0)  //копирование данных
      copy(source);
    else {                    //копирование ссылки
      _m = source._m;
      _capacity = source._capacity;
    }
  }

  Data::Data(const QString &source):_m(Null),_capacity(0){
    int32_t wishfulSize=0;
    fromString(source,wishfulSize,true);
  }

  Data::~Data(){
    free();
  }

  //ѕрисоединение блока
  bool Data::attach(nbData data) {
    if (!isNull()) return false;
    _m        = data;
    _capacity = size();
    return true;
  }

  //ќтсоединение блока
  nbData Data::detach(){
    nbData data = _m;
    _m = Null;
    _capacity   = 0;
    return data; 
  }

  bool Data::isNull() const {
    return (_m == Null);
  }
  bool Data::isEmpty() const {
    return (size() != 0);
  }

  //–азмер блока
  int32_t Data::size() const {
    if (!_m) return 0;
    return *sizeByPtr(_m);
  }

  int32_t Data::capacity() const {
    return _capacity & ~0x80000000;
  }

  //”становка в поле "размер" желательного размера блока без фактического изменени€ размера
  bool Data::setWishfulSize(int32_t size){
    if (!_m) return false;
    *sizeByPtr(_m) = size;
    return true;
  }

  //ќсвобождение блока данных
  void Data::free(){
    if (_m && !isRef()){  //ќсвободить блок, если не ссылка
      delete[] sizeByPtr(_m);
      _m        = Null;
      _capacity = 0;
    }
  }

  //»зменение размера блока
  //  - дл€ присоединенных блоков размер может измен€тьс€ только в сторону уменьшени€
  bool Data::resize(int32_t size){
    if (size > this->size())
      if (!reserve(size)) return false;
    setWishfulSize(size);
    return true;
  }

  //»зменение емкости
  bool Data::reserve(int32_t capacity){
    if (this->capacity() >= capacity) return true;  //ѕроверить превышение зарезервированной (максимально допустимой) емкости
    if (isRef()) return false;                      //ѕри превышении предела, вернуть ошибку
    free();
    _m = new uint8_t[sizeof(int32_t) + capacity]; //ѕеревыделить пам€ть
    if (!_m) return false;
    _m += sizeof(int32_t);
    setWishfulSize(0);
    _capacity = capacity;
    return true;
  }



  Data Data::ref() const {
    return Data::ref(_m,_capacity);
  }



  Data Data::ref(const nbData data, int32_t capacity){
    Data d; d._m = data;
    if (!data)          d._capacity = 0;
    else if (capacity)  d._capacity = capacity;
    else                d._capacity = *sizeByPtr(data);
    d._capacity |= 0x80000000;
    return d;
  }



  Data Data::ref(const QByteArray &ba){
    return ref((const nbData)(ba.constData()) + sizeof(int32_t),ba.capacity()-sizeof(int32_t));
  }



  bool Data::copy(const Data &source){
    return copy(source,source.size());
  }



  bool Data::copy(const uint8_t* src, int32_t srcSize){
    if (!resize(srcSize)) return false;
    //—копировать данные
    uint8_t *dst = _m;
    for (int32_t i=0; i<srcSize; ++i, ++dst, ++src)
      *dst = *src;
    return true;
  }


  //„тение из потока
  //  from - поток дл€ чтени€
  //  size - число считанных байт
  //  withSize - чтение заголовка (если false, используетс€ значение пол€ size)
  //возвращает: число считанных байт или -1
  int32_t Data::read(QDataStream &from,  bool withSize, int32_t size){
    int32_t r=0;
    //—читать размер данных, если требуетс€
    if (withSize)
      r += from.readRawData((char*)&size,sizeof(int32_t));
    if (from.status() != QDataStream::Ok || !resize(size)) return -1;
    //—читать данные
    r += from.readRawData((char*)_m,size);
    if (from.status() != QDataStream::Ok) return -1;
    return r;
  }

  //«апись в поток
  //  to        - поток дл€ записи
  //  withSize  - запись в поток с заголовком
  //возвращает: число записанных байт или -1
  int32_t Data::write(QDataStream &to, bool withSize) const {
    int32_t w=0;
    //«аписать пустой блок
    if (isNull()){
      w += to.writeRawData((char*)&w, sizeof(int32_t));
    } else {
      w += to.writeRawData((char*)(_m - ((withSize) ? sizeof(int32_t):0)),*sizeByPtr(_m) + ((withSize) ? sizeof(int32_t):0));
    }
    if (to.status() != QDataStream::Ok) return -1;
    return w;
  }


  //ќпределение текущей длины строки в символах utf-8
  int32_t Data::length() const {
    return QString::fromUtf8((const char*)_m,size()).size();
  }


  //ѕреобразование данных, представл€ющих строку utf-8, в строку utf-16
  QString Data::toString() const {
    return QString::fromUtf8((const char*)_m,size());
  }

  //ѕреобразование в строку utf-16
  void Data::toString(wchar_t *dest) const {
    QString s = QString::fromUtf8((const char*)_m,size());
    if (s.size()) memcpy(dest,s.data(),sizeof(wchar_t)*s.size());
    else         *dest = 0;
  }


  //ѕреобразование данных из строки utf-16 в строку utf-8
  bool Data::fromString(const QString &source, int32_t &wishfulSize, bool fResize){
    QByteArray ba = source.toUtf8();
    wishfulSize = ba.size();
    if (wishfulSize > size() && !fResize) return false;
    if (!resize(wishfulSize)) return false;
    memcpy(_m,ba.data(),wishfulSize);
    return true;
  }

  //—равнение (0 (c==d), >0 (c>d), <0 (c<d))
  int32_t Data::compare(const Data &d) const {
    //—равнить по указателю
    if (_m == d._m) return 0;
    //—равнить по одинаковому блоку
    int32_t r = memcmp(_m,d._m,min(size(),d.size()));
    if (r != 0) return r;
    //—равнить по длине
    int32_t s = (int32_t)size() - (int32_t)d.size();
    return s;
  }

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug operator<<(QDebug  dbg, const Data &x){
    //(size data..end)
    if (x.isNull()){
      return dbg.nospace()<<"Data(Nil)";
    } else {
      QDebug d = dbg.nospace();
      d<<"Data("<<(void*)x._m<<": "<<x.size()<<" "<<(x.isRef()?'!':' ')<<x.capacity()<<" ";
      //«аписать данные, если их достаточно дл€ вывода
      if (x.size()){
        d<<"["<<AsHex(x._m,8,' ',x.size())<<"]";
      }
      d<<")";
      return d;
    }
  }

#endif

}//end namespace Nb
