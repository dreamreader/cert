//////////////////////////////////////////////////////////////////////////
//����:
//������:
//����:
//�������:
//�����:        ������� �.�.
//
//����:         ������ �������������� ����������
//
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_VBP_H
#define _NB_VBP_H

#include "Meta.h"

#pragma pack(1)

namespace Nb {

//.............................................................................
//  ���
//  - ��������� ���������������� ��������� �� ���� ������ ��� ������ ���������
//    ��������� ����
//  - ��������� �������� � ���, ��������������� � ���� ������ ���
//  ����������: �� ����� �����������, ������� ���������� ������ ������ ���������
//              �������
//.............................................................................
union Vbp {
public:
  //������ ������ � ��������� ����� �������������� ���������
  nbVbp     v;
  int8_t    *i8;
  int16_t   *i16;
  int32_t   *i32;
  int64_t   *i64;
  uint8_t   *u8;
  uint16_t  *u16;
  uint32_t  *u32;
  uint64_t  *u64;
  float     *r32;
  double    *r64;
public:
  Vbp():v(0){}
  Vbp(const nbVbp vec):v(vec){}
  
  //������������� � �������
  bool attach(nbVbp vec);
  nbVbp detach();

  //�������� ������� � ������������� ������
  bool create(uint32_t size);
  bool create(Meta meta);
  void free();

  operator nbVbp() const { return v; }
  Vbp& operator =(const nbVbp& vec){ v=vec; return *this; }
  bool isEmpty();
  
  //��������� ���������� �������
  Vbp shift(int32_t size) const;

  //������� ������ � ������ ��� ������ � ���������� �� 1,2,4 ���,
  //������������������� ��� �����������
  uint32_t u1(uint32_t pos) const;
  void     u1(uint32_t pos, uint32_t val);
  uint32_t u2(uint32_t pos) const;
  void     u2(uint32_t pos, uint32_t val);
  uint32_t u4(uint32_t pos) const;
  void     u4(uint32_t pos, uint32_t val);

  //������� ������ � ������ ��� ������ � ���������� �� 1,2,4 ���,
  //������������������� ��� ��������

  int32_t  i1(uint32_t pos) const;          //���������� -1 � 1
  void     i1(uint32_t pos, int32_t val);   //������������� >0 ��� <=0

  //������� �������������� �������� �������� �������������
  void convU1fromR32(const Vbp &vec, uint32_t count);
  void convR32fromU1(const Vbp &vec, uint32_t count);
  void convI1fromR32(const Vbp &vec, uint32_t count);
  void convR32fromI1(const Vbp &vec, uint32_t count);
  void convU16fromR32(const Vbp &vec, uint32_t count);
  void convR32fromU16(const Vbp &vec, uint32_t count);
  void convI16fromR32(const Vbp &vec, uint32_t count);
  void convR32fromI16(const Vbp &vec, uint32_t count);
  void convI32fromR32(const Vbp &vec, uint32_t count);
  void convR32fromI32(const Vbp &vec, uint32_t count);
  void convU32fromR32(const Vbp &vec, uint32_t count);
  void convR32fromU32(const Vbp &vec, uint32_t count);

  //�������������� � ����� �����
  bool conv(const Vbp &vec, uint32_t count, MetaFormat mfTo, MetaFormat mfFrom);
  //�����������
  void copy(Vbp vbp, uint32_t size);
};

}

#pragma pack()
#endif //_NB_VBP_H
