#pragma once
//////////////////////////////////////////////////////////////////////////
//����:         md5.h
//������:
//����:
//�������:
//�����:
//
//��������:     ���������� ����������� ����� � ���������� ������������
//              ��������������
//////////////////////////////////////////////////////////////////////////
#ifndef _MD5_H
#define _MD5_H

#include <QtCore/QString>
#include "../nb/nb.h"

#pragma pack(1)

  //��� Md5
  class Md5Hash {
  public:
    //�������� ��� �������� ���������� ���-�������
    typedef struct Md5Context_t {
      uint32_t state[4];	  // ���������(ABCD)
      uint32_t count[2];    // ����� ���, modulo 2^64 (lsb first)
      uint8_t  buffer[64];  // ������� �����
    } Md5Context;	

    enum {
      md5HASH_SIZE = 16,
      md5HASH_BITS = (md5HASH_SIZE*8)
    };
    uint8_t digest[md5HASH_SIZE];
  public:

    Md5Hash();
    Md5Hash(const uint8_t *data, uint32_t size);
    ~Md5Hash();

    //�������� ���� �� ����� ������
    void create(const uint8_t *data, uint32_t size);

    //������������� ��������� �� ����� ����������� ����������
    void init(Md5Context &ctx);
    //���������� ���� �� ����� ����������� ����������
    void update(Md5Context &ctx, const uint8_t *data, uint32_t size);
    //���������� ���������� ����
    void final(Md5Context &ctx);

    //�������� ����
	  void erase();
	  //�������� �� ���������
	  bool isEqual(const Md5Hash &hash) const;
    //�������� �� ����
    bool isNil(void) const;

    //�������� �� ��������� ����� ���
    void trim(uint32_t bitLength);
    void set(const uint8_t *data, uint32_t size);

	  Md5Hash& operator =(const Md5Hash &hash);
	  bool operator ==(const Md5Hash &hash) const;
    bool operator !=(const Md5Hash &hash) const;

    //�������������� ������ � ���-��������
    bool fromString(const QString &s);
	  //�������������� ���-�������� � ������
    QString& toString(QString &s) const;
  protected:
    //���������� ���� �� ����� ����������� ����������
    void update16(Md5Context &ctx, const uint8_t *data, uint16_t size);
  private:
    static const QString _tblX16;
  };

#pragma pack()

#endif

