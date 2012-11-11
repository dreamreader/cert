//////////////////////////////////////////////////////////////////////////
//����:
//������:
//����:
//�������:
//�����:
//
//����:
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
    if (source._capacity>=0)  //����������� ������
      copy(source);
    else {                    //����������� ������
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

  //������������� �����
  bool Data::attach(nbData data) {
    if (!isNull()) return false;
    _m        = data;
    _capacity = size();
    return true;
  }

  //������������ �����
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

  //������ �����
  int32_t Data::size() const {
    if (!_m) return 0;
    return *sizeByPtr(_m);
  }

  int32_t Data::capacity() const {
    return _capacity & ~0x80000000;
  }

  //��������� � ���� "������" ������������ ������� ����� ��� ������������ ��������� �������
  bool Data::setWishfulSize(int32_t size){
    if (!_m) return false;
    *sizeByPtr(_m) = size;
    return true;
  }

  //������������ ����� ������
  void Data::free(){
    if (_m && !isRef()){  //���������� ����, ���� �� ������
      delete[] sizeByPtr(_m);
      _m        = Null;
      _capacity = 0;
    }
  }

  //��������� ������� �����
  //  - ��� �������������� ������ ������ ����� ���������� ������ � ������� ����������
  bool Data::resize(int32_t size){
    if (size > this->size())
      if (!reserve(size)) return false;
    setWishfulSize(size);
    return true;
  }

  //��������� �������
  bool Data::reserve(int32_t capacity){
    if (this->capacity() >= capacity) return true;  //��������� ���������� ����������������� (����������� ����������) �������
    if (isRef()) return false;                      //��� ���������� �������, ������� ������
    free();
    _m = new uint8_t[sizeof(int32_t) + capacity]; //������������ ������
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
    //����������� ������
    uint8_t *dst = _m;
    for (int32_t i=0; i<srcSize; ++i, ++dst, ++src)
      *dst = *src;
    return true;
  }


  //������ �� ������
  //  from - ����� ��� ������
  //  size - ����� ��������� ����
  //  withSize - ������ ��������� (���� false, ������������ �������� ���� size)
  //����������: ����� ��������� ���� ��� -1
  int32_t Data::read(QDataStream &from,  bool withSize, int32_t size){
    int32_t r=0;
    //������� ������ ������, ���� ���������
    if (withSize)
      r += from.readRawData((char*)&size,sizeof(int32_t));
    if (from.status() != QDataStream::Ok || !resize(size)) return -1;
    //������� ������
    r += from.readRawData((char*)_m,size);
    if (from.status() != QDataStream::Ok) return -1;
    return r;
  }

  //������ � �����
  //  to        - ����� ��� ������
  //  withSize  - ������ � ����� � ����������
  //����������: ����� ���������� ���� ��� -1
  int32_t Data::write(QDataStream &to, bool withSize) const {
    int32_t w=0;
    //�������� ������ ����
    if (isNull()){
      w += to.writeRawData((char*)&w, sizeof(int32_t));
    } else {
      w += to.writeRawData((char*)(_m - ((withSize) ? sizeof(int32_t):0)),*sizeByPtr(_m) + ((withSize) ? sizeof(int32_t):0));
    }
    if (to.status() != QDataStream::Ok) return -1;
    return w;
  }


  //����������� ������� ����� ������ � �������� utf-8
  int32_t Data::length() const {
    return QString::fromUtf8((const char*)_m,size()).size();
  }


  //�������������� ������, �������������� ������ utf-8, � ������ utf-16
  QString Data::toString() const {
    return QString::fromUtf8((const char*)_m,size());
  }

  //�������������� � ������ utf-16
  void Data::toString(wchar_t *dest) const {
    QString s = QString::fromUtf8((const char*)_m,size());
    if (s.size()) memcpy(dest,s.data(),sizeof(wchar_t)*s.size());
    else         *dest = 0;
  }


  //�������������� ������ �� ������ utf-16 � ������ utf-8
  bool Data::fromString(const QString &source, int32_t &wishfulSize, bool fResize){
    QByteArray ba = source.toUtf8();
    wishfulSize = ba.size();
    if (wishfulSize > size() && !fResize) return false;
    if (!resize(wishfulSize)) return false;
    memcpy(_m,ba.data(),wishfulSize);
    return true;
  }

  //��������� (0 (c==d), >0 (c>d), <0 (c<d))
  int32_t Data::compare(const Data &d) const {
    //�������� �� ���������
    if (_m == d._m) return 0;
    //�������� �� ����������� �����
    int32_t r = memcmp(_m,d._m,min(size(),d.size()));
    if (r != 0) return r;
    //�������� �� �����
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
      //�������� ������, ���� �� ���������� ��� ������
      if (x.size()){
        d<<"["<<AsHex(x._m,8,' ',x.size())<<"]";
      }
      d<<")";
      return d;
    }
  }

#endif

}//end namespace Nb
