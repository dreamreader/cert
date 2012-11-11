//////////////////////////////////////////////////////////////////////////
//����:
//������:
//����:
//�������:
//�����:        ������� �.�.
//
//����:         ���������� �������������� �����
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_SLOT_ID
#define _NB_SLOT_ID

#include <QtCore/QList>

#include "Enums.h"

namespace Nb {

  class SlotId;
  typedef QList<SlotId> SlotIds;

//...........................................................................
//  ������������� �����
//...........................................................................
  class SlotId {
    nbSlotID _m;
  public:
    SlotId();
    SlotId(nbSlotID id);
    SlotId(SlotType st, uint16_t index);
    SlotId(SlotId id, SlotType st);

    inline operator nbSlotID*(){return &_m;}
    inline operator nbSlotID() const { return _m; }

    inline bool operator ==(const SlotId &id) const { return _m == id._m; }
    inline bool operator !=(const SlotId &id) const { return _m != id._m; }

    SlotType  type() const;
    uint16_t  index() const;
    uint8_t   bindex() const;
    BlockType btype() const;

    //�������� ����
    SlotId static out;
    //������� ���� � �������� �������
    SlotId static prm(SlotType st, uint16_t i);
    //������� ���� � �������� �������
    SlotId static in(uint16_t i);
    //���� ��������������
    SlotId static conv(BlockType bt, uint8_t i);
    //���� �������������� �� ���������
    SlotId static convDef(BlockType bt);
    //����������� ������� ����
    SlotId static sin(uint16_t i);
    //����������� �������� ����
    SlotId static sout(uint16_t i);

    //������� ��������� ����
    SlotId as(SlotType st);
    static SlotId as(SlotId id, SlotType st);
    SlotId retype(SlotType st);
    static SlotId retype(SlotId id, SlotType st);

    static SlotIds fromIDs(const nbSlotID *id, int32_t count);

    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const SlotId &x);
    #endif
  };


#if !defined(QT_NO_DEBUG_STREAM)
  QDebug operator<<(QDebug  dbg, const SlotIds &x);
#endif

}

#endif
