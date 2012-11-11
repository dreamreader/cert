//////////////////////////////////////////////////////////////////////////
//����:
//������:
//����:
//�������:
//�����:        ������� �.�.
//
//����:         ���������� ������ �������������� ��������������� ������
//              � ������ �������������� �������
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_BIM_H
#define _NB_BIM_H

#include <QtCore/QDataStream>

#include "Enums.h"
#include "Uuid.h"

#pragma pack(1)

namespace Nb {

  using namespace  std;

  //��������� ��������������� ������
  typedef nbBim* BimHandle;

  //�������������� �����
  class Bim {
  public:    
    Bim();
    Bim(const BimHandle handle);  //����������� ������
    Bim(const Bim& bim);          //����������� ������
    //Bim(int32_t sizeData);        //��������� ������ ��� ����� ������� �������
    ~Bim();                       //������������ ������

    Bim& operator=(const Bim& bim);  //�������� �����������

    bool isNull() const;          //�������� �� ������� ������
    bool isEmpty() const;         //�������� �� ������� ������ (�������������)

    int32_t size() const;         //������ ������ ��� ��������
    int32_t sizeData() const;     //������ ������
    const Uuid& type() const;     //���
    uint8_t* data() const;        //������

    BimHandle handle() const;     // ������ � ���������
    bool attach(BimHandle);       // ������������� ���������
    BimHandle detach();           // ������������ ���������

    bool resize(const Uuid &type, int32_t dataSize); //�������� ��������������������� ������
    void free();                  //������������ ������

    bool copy(const BimHandle handle); //����������� � ���������� ������

    int32_t read(QDataStream &from); //������ �� ������
    int32_t write(QDataStream &to) const;  //������ � �����

    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const Bim &x);
    #endif

  protected:
    BimHandle _m;     //��������� ��������������� ������
  };

  typedef QList<Bim> Bims;

  //����� �������������� ��������������� ������
  //����������. ������ ������������� ���������� ������� (BimHandWr), ���������� ������� (BimFingerPr) 
  //  � �.�. ������ ������������� �� ����.
  class BimUnpacked {
    virtual ~BimUnpacked()=0;                       //����������� ����������

    virtual bool isEmpty() const=0;                 //�������� �� ������� ������

    virtual const Uuid& type() const=0;             //���
    virtual int32_t sizePacked()=0;                 //������ ������������ ������

    virtual bool pack(BimHandle bim)=0;             //�������� ������ ��� ��������� �������
    virtual bool pack(Bim &bim, bool canResize=false)=0; //�������� ������ � ��������� ������� �� �������
    virtual bool unpack(const BimHandle handle)=0;  //���������� ������
    virtual bool isSupportedType(Uuid& classType)=0;//�������� ����������� �������� ������
    virtual const Uuid& baseType()=0;               //���������� �������� �������������� ���
  };
}

#pragma pack()
#endif //_NB_BIM_H
