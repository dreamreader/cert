//////////////////////////////////////////////////////////////////////////
//����:
//������:
//����:
//�������:
//�����:        ������� �.�.
//
//����:         ���������� ������ ����� ������ � �������������� �����
//              ������� �� �������������� �������� �����
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_DATA_H
#define _NB_DATA_H

#include <QtCore/QString>
#include <QtCore/QDataStream>

#include "Enums.h"

#pragma pack(1)

namespace Nb {

using namespace std;

//...........................................................................
//  ���� ������
//...........................................................................
class Data {
  nbData   _m;          //���� ������ � ��������
  int32_t  _capacity;   //����������� ����������������� ������� �����
                        //  (������������� �������� ��� ��������� ���������� �������)
public:
  Data();
  Data(int32_t size);
  Data(const nbData data);
  Data(const Data &source);
  Data(const QString &source);
  ~Data();

  inline operator nbData*(){return &_m;}
  inline operator nbData() const { return _m; }
  
  Data& operator=(const Data &d) { ref(d._m); return *this; }
  bool operator==(const Data &d) const { return (0==compare(d)); }
  bool operator!=(const Data &d) const { return (0!=compare(d)); }
      
  inline uint8_t* data() const { return (uint8_t*)_m;}  //������ � ������

  bool  attach(nbData data);  //������������� ����� �������������� �����
  nbData detach();            //������������ �����

  bool isNull() const;      //�������� ������� ������
  bool isEmpty() const;     //�������� ������ � �������
  int32_t size() const;     //������ �����
  int32_t capacity() const; //���������� ������� �����

  //������ � ���� ������� ����� ��������� �������
  //����������: ����� ������������� ������ �������� ������� ��������� ������� � �����������,
  //�� �������� ������������� ������� ������
  bool     setWishfulSize(int32_t size);

  //����������� ������
  void free();
  //��������� ������� �����
  bool resize(int32_t size);
  //�������������� �������
  bool reserve(int32_t capacity);

  //������������ ������ �� ������
  //����������: ����������� ��������� ����� �������� � ������, ����� ���������� ��� �������
  Data ref() const;
  static Data ref(const nbData data, int32_t capacity=0);
  //������������ ������ �� ���� ������ ��� �� Data
  //����������: ������ 4 ����� ������� ������ ��������� ���� �������,
  //            ��������������� �� ������ �������
  static Data ref(const QByteArray &ba);
  bool isRef() const;
  //����������� ������
  bool copy(const Data &source);
  //����������� ������
  bool copy(const uint8_t* src, int32_t srcSize);

  //������ �� ������
  //  from - ����� ��� ������
  //  size - ����� ��������� ����
  //  withSize - ������ ��������� (���� false, ������������ �������� ���� size)
  //����������: ����� ��������� ���� ��� -1
  int32_t read(QDataStream &from, bool withSize=true, int32_t size=0);
  //������ � �����
  //  to        - ����� ��� ������
  //  withSize  - ������ � ����� � ����������
  //����������: ����� ���������� ���� ��� -1
  int32_t write(QDataStream &to, bool withSize=true) const;

  //����� ���������� ������� toString ������ � utf-16
  int32_t  length() const;
  //�������������� � ������ utf-16 �� ����� utf-8
  QString toString() const;
  void toString(wchar_t *dest) const;
  //�������������� � ���� utf-8 �� ������ utf-16
  //���������� ����������� ������ ������, ���� ������ ������ ������������. ����� 0
  bool fromString(const QString &source, int32_t &wishfulSize, bool fResize = false);
 
  //��������� (0 (c==d), >0 (c>d), <0 (c<d))
  int32_t compare(const Data &d) const;

  static int32_t* sizeByPtr(nbData ptr);

  #if !defined(QT_NO_DEBUG_STREAM)
    QDebug friend operator<<(QDebug  dbg, const Data &x);
  #endif
};

  inline bool Data::isRef() const { return _capacity & 0x80000000; }
  inline int32_t* Data::sizeByPtr(nbData ptr) { return (int32_t*)ptr-1; }

}

#pragma pack()
#endif  //_NB_DATA_H
