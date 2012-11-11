//////////////////////////////////////////////////////////////////////////
//����:         
//������:
//����:
//�������:
//�����:        ������� �.�.
//
//����:         ���������� �������������� ����� ����� �������������� -
//              ���������, ���������������, ���������� ���������������
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_SLOT_H
#define _NB_SLOT_H

#include <QtCore/QList>
#include <QtCore/QDataStream>

#include "Enums.h"
#include "SlotId.h"
#include "Meta.h"

namespace Nb {

  class Scheme;
  class Slot;
  typedef QList<Slot*> Slots;

  //....................................................................................
  //���� ���������
  //  �. ��������, ������������ ��������, ������������ ���������
  //  �. ��������� ��� ���������������
  //  �. ��������� ��� ���������� ���������������
  //     ����������: ������ ��������� ������ ���������� ��������������� ������ �������������
  //                �� ��� �� ��������, ��� � � �������� ���������������
  //....................................................................................
  class Slot {
  public:
    Slot();
    Slot(SlotId id, Meta meta);    //����������� ����� ��., ��.��., ��.���. ���������
    Slot(SlotId id, Meta meta, const SlotIds &cids, BlockFlags bf = BfNone, BlockState bs = BsUntrained); //����������� ����� ���������������
    Slot(SlotId id, Meta meta, const SlotIds &cids, const SlotIds &rids, BlockFlags bf = BfNone, BlockState bs = BsUntrained, Scheme *r=Null); //����������� ���������� ���������������

    virtual ~Slot();                                  //����������

    inline SlotId  id() const { return _id; }         //�������������
    inline Meta    meta() const { return _meta; }     //������������

    inline Scheme* scheme() const { return _s; }      //����� ��������������, � ������� ������� ����
    inline Scheme* rscheme() const { return _r; }     //����� �������������� ���������� ���������������

    inline BlockFlags bflags() const { return _bf; }  //����� ��� ����� ���������������
    inline BlockState bstate() const { return _bs; }  //��������� ��� ����� ���������������

    inline const SlotIds&  cids() const { return _cids;}     //������ ��������������� ��������� ������
    inline const SlotIds&  rids() const { return _rids;}     //������ ������� ��������� ������
    inline int32_t  count() const { return _cids.count(); }  //����� ��������� (������������) ������
    void     ccount(int32_t *n);                      //����� ��������� ������ �� ����� (������� n : 0=��.,1=��.��.,2=��.���.,3=���.)
    void     rcount(int32_t *n);                      //����� ������������ ������ �� ����� (������� n : 0=��.,1=��.��.,2=��.���.,3=���.)
    SlotIds cids(SlotType st) const;                  //������ ��������������� ��������� ������ �� ���� (��.,��.��.,��.���)
    //������������ ������� ����������� �� ���� (��.,��.��.,��.���) ����� ���������� � ���������
    //����������: io � ����� ������ �������� ����� ������������ �����, ����� ���������� ������
    void    cids(SlotIds *ii, SlotIds *isi, SlotIds *iso, SlotIds *io) const;

    virtual bool copy(const Slot &slot);              //����������� �����
    
    virtual int32_t sizeSlot(bool first=false);       //������ ��� �������� �������� �����
    virtual int32_t readSlot(SlotId id, QDataStream &from, int32_t remain, bool first=false); //������ �������� �� ������ (������������� ����� ����������� �������������� � id)
    virtual int32_t writeSlot(QDataStream &to, bool first=false) const;      //������ �������� ����� � �����

    friend class Scheme;


    //�������� ���������� ����� ���������� ������ (C-�����, ���� ��� �����������)
    virtual bool validate() const;
    //�������� ���������� ����� ������� ������ (R-�����, ���� ��� � S-����� �����������)
    virtual bool rvalidate() const;

    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug virtual debug(QDebug  dbg) const ;
    #endif

  protected:
    //���������� ������ ����� �� ������� C-����� (������ ���� �����������)
    virtual bool update();
    //���������� ������ ����� �� ������� R-����� (������ ���� �����������)
    virtual bool rupdate();

    bool scheme(Scheme* s);   //��������� C-����� ��������������
    bool rscheme(Scheme* r);  //��������� R-����� ��������������

  protected:
    //��������� ���������� ���� ������ �����
    Result rslt() const;
    //��������� ���� ������ (�����)
    //����������: false
    bool erslt(ResultCode rc) const;
    //��������� ���� ���������� (��������, ������� ������� ������)
    //����������: ������� ������������� ������ (false)/������ (true)
    bool rslt(Result r) const;

  private:
    Scheme       *_s;       //������ �� �����
    Scheme       *_r;       //������ �� ��������� �����
  protected:
    SlotId        _id;      //�����
    Meta          _meta;    //������������
    BlockFlags    _bf;      //����� ���������������
    BlockState    _bs;      //��������� ���������������

    SlotIds       _cids;    //������ ������� ��������� ������ (� ���������� ����� ����)
    SlotIds       _rids;    //������ ������� ������������ ������ (���� ��� ����������� � _cids)
  };

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug operator<<(QDebug  dbg, const Slot &x);
  QDebug operator<<(QDebug  dbg, const Slots &x);
#endif
}

#endif //_NB_SLOT_H
