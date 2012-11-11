//////////////////////////////////////////////////////////////////////////
//����:         
//������:
//����:
//�������:
//�����:        ������� �.�.
//
//����:         ���������� ����� ��������������, ������� ������
//              ��� ������������ �������� �������������� ���������-���
//              �� ���� � 52633.4, ���� � 52633.7.
//              �������������� ������ ����� �������������� � �������������
//              ������ ��������� (������������) ������.
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_SCHEME_H
#define _NB_SCHEME_H

#include <QtCore/QList>
#include <QtCore/QDataStream>

#include "Enums.h"
#include "Data.h"
#include "SlotId.h"
#include "Slot.h"

namespace Nb {


  //....................................................................................
  //����� ��������������
  //....................................................................................
  class Scheme {
  public:
    //....................................................................................
    //������� ������
    //(��������� ������� ��������� ����� ��������� ��������� �����)
    //....................................................................................
    class Factory {
    public:
      virtual Slot* create(SlotId id) const;  //�������� ����� �� ��� ����
      virtual void  free(Slot* s) const;      //�������� �����
    };

  public:
    Scheme();
    Scheme(const Factory &factory);   //�����������
    virtual ~Scheme();                //����������
    Scheme(Scheme& scheme):_factory(Scheme::defaultFactory()),_posOut(-1) {copy(scheme);}

    const Factory& factory() const;   //������ � ������� ������
    static Factory& defaultFactory(); //������� ������ "�� ���������"

    bool isSet() const;               //�������� ����������� �����

    int32_t count() const;            //����� ������ � �����
    SlotId  idOf(SlotId id) const;    //������ ����� �� ��� ��������������, ��� SlotId::out
    SlotId  idOf(int32_t pos) const;  //����� ����� �� ������� ��� SlotId::out
    int32_t indexOf(SlotId id) const; //����� ����� � ��������� �������, ���������� ��� �������
    int32_t indexOf(SlotId id, int32_t start) const;  //����� ����� � ���������� �������, ���������� ��� �������
    int32_t indexOfLast(SlotType st, BlockType bt=BtExtention) const; //������� ���������� ����� � ��������� �����
    Slot* at(SlotId id) const;        //������ �� ������ (����������� SlotId::out)
    Slot* at(int32_t pos) const;      //������ �� �������

    //������ � ������ ������� ������ �����
    //����������: ����� SlotId::out �������� � ������� id(SlotId::out)
    SlotIds ids() const;
    //������������ ������ ������� ������ � �������� �����
    //����������: ����� SlotId::out �������� � ������� id(SlotId::out)
    //  st  - ��� ����� (��., ��.��., ��.���, ���������������)
    //  bt  - ��� ��������������� (BtExtention �������� ���� �����)
    SlotIds ids(SlotType st, BlockType bt=BtExtention) const;

    //����� ������ �� ������ �������
    //����������: ���� ordered ����������, �� ������� � ids ������������� ������� ������ � �����
    //            ��������� ����� ����������� � ������ ss
    //����������: true, ���� ������� ��� ����� (��� ���������)
    //            false, ���� ��������� ����� �� ���� ������� � ��� ��� ���� ��������� Null �������� Slot*
    bool find(Slots &ss, const SlotIds &ids, bool ordered=false) const;

    //����� ������ � ����������� �� �� ���� ����������
    //����������: ����� ����������� � ��������� ������
    //  si  - �������
    //  ssi - ����������� �������
    //  sso - ����������� ��������
    //  so  - �������� (���������������)
    bool find(Slots *si, Slots *ssi, Slots *sso, Slots *so) const;


    //�������������� ������ � ������� ����������
    //����������: ����������� ����� (Null) ����������� �� ������
    bool order(Slots &ss) const;

    //����� ������, ������������ ����, � ��� ����� � �����������
    //  id    - ����� ����� � ��� ���
    //  multi - ������� "��������" ���������, ������������� ����� �����
    //          ���������� ������������ ������������� ����� ��������� ������
    //����������: ������������� ����� ������ ��������� ������� ��� �������������
    //            (��� ��., ��.��., ��.���., ���. ��������) �� ���������
    //            � ������������� �����
    //            ��������� ����� ����������� � ������ ss
    void used(Slots &ss, SlotId id, bool lazy=true) const;


    //���������� ����� � �������������� ����������� ��������
    //  st  - ��� ����� ��� ��., ��.��., ��.���
    //  bt  - ��� ��������������� ��� ��.��.
    //  meta- ������������
    //  bf  - ����� ���������������
    //  bs  - ��������� ���������������
    //  (ct,ci)  - ���� ��� ��������� ����������: ���� � ���������� ������ ����������
    //  (rt,ri)  - ���� ��� ������������ ����������: ���� � ���������� ������ ����������
    //����������:
    //  ������ ���������� ���������� ���������������, ������� � 1 ��� ��., ��.��., ��.���. ����������
    //  ������ ��� ����� ���������� ���������� ��������������� ��� ��������� ����, ������� � 1
    Scheme& push(SlotType st, Meta meta);
    Scheme& push(BlockType bt, Meta meta, const SlotTypes &ct, const SlotNums &ci, BlockFlags bf = BfNone);
    Scheme& push(Scheme &r, Meta meta, const SlotTypes &ct, const SlotNums &ci, const SlotNums &ri, BlockFlags bf = BfNone);


    //�������� ����� �� ������ � �������
    Scheme& push(const Slot &s);    //���������� ����� � ����� ����� � �������������� ������� �������
    Scheme& out(SlotId id);         //���������� ��������� �����, ���������� � ����� ��������������
    Scheme& out(int32_t pos);       //���������� ��������� ����� �� ������� (������  � 0)

    virtual void free();                      //������������ �����
    virtual bool copy(const Scheme &scheme);  //����������� ����� � �������������� ������� �������

    //�������� ������������ ����� (��������� ��������� � ������������������ �������)
    virtual bool validate();
    virtual bool rvalidate();

    //������ ������ ����� ��� ����������
    int32_t sizeOfScheme() const;
    //������ ����� �������������� �� ������
    virtual int32_t readScheme(QDataStream &from, int32_t remain);
    //������ ����� �������������� � �����
    //  to  - �������� �����
    //  asData  - ������� ������ � ������� Data (� ����� int32_t ����� �����)
    virtual int32_t writeScheme(QDataStream &to, bool withSize = false) const;

    //������ �������� ���������� ��������� ��������
    inline Result rslt() const { return _rslt; }
    //��������� ���� ��������� ��������
    inline bool rslt(Result r){ _rslt = r; return _rslt.succeeded(); }
    inline bool rslt(SlotId id, ResultCode rc){ _rslt = Result::fail(id,rc); return _rslt.succeeded(); }

  #if !defined(QT_NO_DEBUG_STREAM)
    virtual QDebug debug(QDebug  dbg) const;
  #endif

  protected:
    //�������� �� �����������
    bool circled() const;
    //����������
    static void mathQSort(int32_t A[], void** P, int32_t nelms);
    template<class T> static inline void mathSwap(T &x, T &y){ T temp = x; x = y;  y = temp; }
  protected:
    const Scheme::Factory  &_factory; //������� �������
    Slots   _slots;                   //������ ������ �����
    int32_t _posOut;                  //������� ��������� ����� ����� (�� ��������� ��������������� � ��������� ������� �����)
    Result  _rslt;                    //��� ����������
  };

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug operator<<(QDebug  dbg, const Scheme &x);
#endif
}

#endif //_NB_SCHEME_H
