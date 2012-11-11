
#include "VbpMath.h"
#include "BcSlot.h"
#include "BcScheme.h"

#include "BcParam.h"
#include "BcNeuralConverter.h"
#include "BcCondition.h"
#include "BcConnector.h"

namespace Bc {
  using namespace Nb;


  Slot* BcScheme::BcFactory::create(SlotId id) const {
    switch (id.type()){
    case StIn:      return new BcParam();
    case StSpecIn:  return new BcParam();
    case StSpecOut: return new BcParam();
    default:
      switch (id.btype()){
      //case BtNbc: return Null;  //��������� ��������������� �� ��������������
      case BtConnector:   return new BcConnector();
      case BtCondition:   return new BcCondition();
      case BtNeuralConv:  return new BcNeuralConverter();
      default: break;     //������ ���� ���������������� �� ��������������
      }
    }
    return Null;
  }

  void  BcScheme::BcFactory::free(Slot* s) const{
    delete s;
  }

  Scheme::Factory& BcScheme::defaultFactory(){
    static BcScheme::BcFactory def;
    return def;
  }


  void BcScheme::free(){
    _sf = 0;
    _info.free();
    Scheme::free();
  }



  bool BcScheme::clear(ConstraintType ct){
    for (int32_t i=0; i<_slots.count(); ++i){
      BcSlot* s = (BcSlot*)_slots[i];
      if (!s->clear(ct)) return false;
    }
    return true;
  }



  bool BcScheme::clear(const SlotIds &ids, ConstraintType ct){
    Slots ss;
    if (!find(ss,ids,false)) return rslt(0,EcSlotId);
    for (int32_t i=0; i<ss.count();++i){
      BcSlot* s = (BcSlot*)_slots[i];
      if (!s->clear(ct)) return false;
    }
    return true;
  }



  //��������� ������ ������
  bool BcScheme::set(SlotId id, ConstraintType ct, const Matrix::Row &value, bool skipEmpty){
    BcSlot *s = (BcSlot*)at(id);
    if (!s) return rslt(0,EcSlotId);
    if (skipEmpty && value.isEmpty()) return true;
    if (!s->set(ct,value)) return false;
    return true;
  }

  bool BcScheme::set(const SlotIds &ids, ConstraintType ct, const Matrix::Rows &values, bool skipEmpty){
    Slots ss;
    if (ids.count() != values.count()) return rslt(0,EcParam);
    if (!find(ss,ids,false))        return rslt(0,EcSlotId);

    for (int32_t i=0; i<ss.count();++i){
      if (!ss[i]) continue;
      if (skipEmpty && values[i].isEmpty()) continue;
      if (!((BcSlot*)ss[i])->set(ct,values.at(i))) return false;
    }
    return true;
  }


  //������ ������ ������
  bool BcScheme::get(SlotId id, ConstraintType ct, Matrix::Row &value){
    BcSlot *s = (BcSlot*)at(id);
    if (!s) return rslt(0,EcSlotId);
    if (!value.isEmpty())
      if (!s->get(ct,value)) return false;
    return true;
  }

  bool BcScheme::get(const SlotIds &ids, ConstraintType ct, Matrix::Rows &values){
    Slots ss;
    if (ids.count() != values.count()) return rslt(0,EcParam);
    if (!find(ss,ids,false))        return rslt(0,EcSlotId);

    for (int32_t i=0; i<ss.count();++i){
      if (!ss[i] || values[i].isEmpty()) continue;
      if (!((BcSlot*)ss[i])->get(ct,values[i]))
      {
        return false;
      }
    }
    return true;
  }


  int32_t BcScheme::actual(SlotId id, ConstraintType ct){
    BcSlot *s = (BcSlot*)at(id);
    if (!s) return rslt(0,EcSlotId);
    return s->actual(ct);
  }


  bool BcScheme::reset(SecureState stt){
    rslt(Result::ok);
    if (stt & SsScheme){ //����� �����
      //������� ����� � ��� ���������
      free();
      return true;
    }
    Slots ss;
    if (stt & SsTrained){  //c���� ���������� �������� � ����������� ������������
      //��������� ��� ����������������
      find(Null,Null,Null,&ss);
      for (int32_t i=0; i<ss.count(); ++i){
        ((BcSlot*)ss[i])->reset();
        ((BcSlot*)ss[i])->clear(CtTst);
      }
    }
    if (stt & SsInAny || stt & SsInOwn){  //����� ��. � ��.��. ���������� (����� ����� �� ��������� � ����� �������)
      find(&ss,&ss,Null,Null);
      if (stt & SsInAny)
        for (int32_t i=0; i<ss.count(); ++i)
          ((BcSlot*)ss[i])->clear(CtAny);
      if (stt & SsInOwn)
        for (int32_t i=0; i<ss.count(); ++i){
          ((BcSlot*)ss[i])->clear(CtAll);
          ((BcSlot*)ss[i])->clear(CtOwn);
        }
    }
    if (stt & SsOutAny || stt & SsOutOwn){ //����� ����������� ���������� (��������� ��������� ������������ ������)
      find(Null,Null,&ss,&ss);
      if (stt & SsOutAny)
        for (int32_t i=0; i<ss.count(); ++i)
          ((BcSlot*)ss[i])->clear(CtAny);
      if (stt & SsOutOwn)
        for (int32_t i=0; i<ss.count(); ++i){
          ((BcSlot*)ss[i])->clear(CtAll);
          ((BcSlot*)ss[i])->clear(CtOwn);
        }
    }
    sync();
    return true;
  }

  bool BcScheme::emulate(SlotId id, ConstraintType ct){
    Slot *s;
    s = at(id);
    if (!s) return rslt(0,EcSlotId);
    if (!((BcSlot*)s)->emulate(ct,ct,true)) return false;
    return true;
  }

  bool BcScheme::emulate(const SlotIds &ids, ConstraintType ct){
    Slots ss;
    if (!find(ss,ids,false)) return rslt(0,EcSlotId);

    for (int32_t i=0; i<ss.count();++i){
      if (!ss[i]) continue;
      if (!((BcSlot*)ss[i])->emulate(ct,ct,true)) return false;
    }
    return true;
  }



  bool BcScheme::train(){
    Slots ss;   find(ss,ids(StConvertor), true);  //����� ��� ����� ��������������
    BcSlots st; st.reserve(ss.count());
    //��������� �� ����� ��� ��������������� �����
    for (int32_t i=0; i<ss.count(); ++i)
      if (!(ss[i]->bflags() & BfHidden)) st.push_back((BcSlot*)ss[i]);
    //�������� ��������� ��������� ������ (��� ��������������������� ������� ����� ��������� ��������� ������)
    for (int32_t i=0; i<st.count(); ++i)
      st[i]->reset();
    //��������� � ���� ������ �� ������, ������ � ����� ��� ������ �������
    int32_t l = 0, r = st.count()-1;
    Result err;
    do {
      //������� ��������� � ������/����� ���������������
      if (st[l]->train(TmWithoutFlags,BsTrained,0)){
        ++l;
      } else {
        err = rslt();
        if (l!=r && st[r]->train(TmWithoutFlags,BsTrained,0)){
          --r;
        } else {
          //������� ������ ��������� ���������
          rslt(err);
          return false;
        }
      }
    } while (l<=r);
    sync();
    return true;
  }



  bool BcScheme::test(Matrix::Row &tr){
    //TmE1Bio ����������, ����������� �� �������� �������� "����"
    //TmE2Bio ��������� "��� �����" ��� ���������� � ���, =TmE2BioCorrNoise + ����.��.
    //TmE2BioHumanCompromised ��������� ��� ������������� ������ �������, =0 (��� ����� ������)
    //TmE2BioCompromised ��������� ��� ������������� ������ �������, =0 (��� ����� ������)
    //TmE2Code ���������, =����� ���.����
    //TmE2BioWhiteNoise ��������� ��� ����� ����� �����, =����� ��.
    //TmE2BioCorrNoise ��������� ��� ����� ��������������� ����� (��������� �� "��� �����")
    //TmE2Conv ��������� ��� ���������� ��� ���, =TmE2BioCorrNoise + ���������� + ����.��.

    //����� ��� �����
    Slots ss; if (!find(ss,ids(StConvertor),true)) { ELOG(1); return rslt(0,EcCScheme); }
    Slots ss2; ss2.reserve(ss.count());
    //��������������� �����, ��������� ���������� ��������������� �� ���������� �������
    for (int32_t i=0; i<ss.count(); ++i){
        Slot* s = ss[i];
        if (s->id().btype() == BtSoftIndicator || s->id().btype() == BtHardIndicator){  //����������� ���������� ����� �� ���������� �������
          int32_t idx = ss.indexOf(at(s->cids()[0])); //����� ������� ���������� �����
          if (idx<0) { ELOG(2); return rslt(0,EcCScheme); } //���������� ������ ����� ��������������
          ss2.insert(idx+1,s);                        //�������� ��������������� �� ��������� ������
        } else {
          ss2.push_back(s);                           //�������� � ���������� ������
      }
    }

    //�������������� ����� � ��������� �������
    for (int32_t i=0; i<ss2.count(); ++i){
      BcSlot* s = (BcSlot*)ss2[i];
      if (!s->test()) { ELOG(3); return false; }
    }

    //������� ��������� � ��������������� �����
    BcSlot* s = (BcSlot*)at(SlotId::out);
    if (!s->get(CtTst,tr)) { ELOG(4); return false; } //�������� ��������� � ���������� ������ ��������� ������

    return true;
  }


  bool BcScheme::qeval(Matrix::Row &ge){
    //������� �������� �������� ������ ���������� �� 0 (������) �� X (������������), I32
    if (ge.meta().count() != 1) return rslt(0,EcMeta);
    if (ge.meta().format() != MfR32) return rslt(0,EcMeta);
    //������������ ������ ������� ����������
    Slots ss; find(&ss,Null,Null,Null);

    //�������� ������ ������ ��� ������� ��. ���������, ��� �������� ���������� ������ CtOwn
    //� �������� ���������, ��� ������� �������� ������ � ������ �����������
    Matrix::Rows ges;   //�������� ge ��� ����������� ����������
    QList<float> mids;  //������� �������� ge
    for (int32_t i=0; i<ss.count(); ++i){
      BcSlot* s = (BcSlot*)ss[i];
      if (!s->qeval()) continue;
      Matrix::Row ge1 = s->constraint(CtGe);
      mids.append(averColR32(ge1.data(),ge1.count(),0));  //���������� ������� �� ��������
      ges.append(ge1);
    }

    for (int32_t i=0; i<ge.count(); ++i){   //��������
      ge.at(i).r32[0] = 0.0f;
    }

    for (int32_t j=0; j<ges.count(); ++j){  //�������� ges
      Matrix::Row gej = ges[j];
      int32_t i;
      for (i=0; i<min(gej.count(),ge.count()); ++i) //������� � gej
        ge.at(i).r32[0] += gej.at(i).r32[0];
      for (; i<ge.count(); ++i) //�������� ������� ��� ������������� ��������
        ge.at(i).r32[0] += mids[j];
    }

    if (ges.count())
    for (int32_t i=0; i<ge.count(); ++i){   //��������������, ����� ��������� �������� 0..1
      ge.at(i).r32[0] /= ges.count();
    }

    //������� ��� �������� ge ������� ����������
    for (int32_t i=0; i<ss.count(); ++i){
      BcSlot* s = (BcSlot*)ss[i];
      s->clear(CtGe);
    }
    return true;
  }




  bool BcScheme::validate(){
    bool ok = Scheme::validate();
    if (ok) _sf |= SfValidated; else _sf &= ~SfValidated;
    return ok;
  }



  void BcScheme::sync(){
    int32_t ct=0, nt=0, pt=0;   //����� ��������� ����������������, ��������� ����������������, �������� ���������
    _sf = _sf & SfValidated;  //�������� ����� ��������

    Slots ss; find(Null,Null,Null,&ss);
    if (!ss.count()) return;

    for (int32_t i=0; i<ss.count(); ++i){
      bool nested   = (ss[i]->id().btype() == BtNbc);
      if (nested)
        ((BcSlot*)ss[i])->sync();  //���������������� ��������� ���������������
      BlockState bs = ss[i]->bstate();
      if (bs == BsUntrained){ //��������
        if (ct) break;  //��� ����� ����������� ��� SfPartially � SfConv1
      } else if (bs == BsPartially){  //������ (��� �������� ��� ���������)
        if (nested) { pt++; if (ct) break; } //��� ����� ����������� ��� SfPartially � SfConv1
        else ct++;
      } else {  //������ � ��������� ����������
        if (nested) nt++;
        else ct++;
      }
    }
    if (ct)                   _sf |= SfConv1;   //���� �� 1 ��������������� ������
    if (ct+nt == ss.count())  _sf |= SfTrained;  //��� ������� ���������
    else if (ct+nt+pt)        _sf |= SfPartially;//������� ��������
  }



  // {30413CB1-53A7-4b62-9F01-0E430E7E3EF8}
  const Uuid BcScheme::_containerType = Uuid(0x30413cb1, 0x53a7, 0x4b62, 0x9f, 0x1, 0xe, 0x43, 0xe, 0x7e, 0x3e, 0xf8);



  int32_t BcScheme::writeParams(Container &c, const Uuid *ct) const{
    //���������� �������������� ��� ����������
    if (ct && (!ct->isNull() || *ct != _containerType)) { ELOG(1); return -1; }
    //�������� ��������� ���������� � ��������� �������
    c.push(BlockContHead(_containerType,0));
    int32_t hpos = c.count()-1;
    BlockContHead*  h = (BlockContHead*)c.at(hpos);
    SchemeFlags sf = state();
    //if (sf & SfValidated){
      //�������� ����� ��������������
      c.push(Block(BtCScheme));
      Block* s = c.last();
      s->push(*this);
      if (s->isEmpty()){
        c.pop(c.count()-hpos);
        return -1;
      }
      bool ist = sf & SfConv1;  //���� ���� �� 1 ������� ��������������� ������, �������� ��� ��������� ����������������
      Slots ss; find(Null,Null,Null,&ss);
      for (int32_t i=0; i<ss.count(); ++i){
        if (((Slot*)ss[i])->id().btype() == BtNbc || ist)
          if (((BcSlot*)ss[i])->writeParams(c,&_containerType,Null)==-1){ //�������� ���������
            c.pop(c.count()-hpos);                              //�������� ������
            ELOG(2);
            return -1;
          }

      }
    //}
    if (_info.count())
      c.push(_info);                      //��������� �������������� ����� � ����� ����������
    h->update(c.all(),hpos,c.count());  //�������� ������ ����������
    return c.count()-hpos;              //��������� ����� ���������� ������
  }

  //������ ����������
  //����������: ����� ��������� ������ ��� -1
  int32_t BcScheme::readParams(const Container &c, int32_t pos){
    rslt(Result::ok);
    Slots   ss;       //������ ����������� ������ �������������� �����, ������� ���������
    int32_t islot=-1; //����� ����� �������������� �� �����, ��� �������� ����������� ���������
    Container::Factory f;
    Container info(f);
    //����� ����� ����� (����������) ���������� ���������� �� ���������
    int32_t epos = c.findEnd(pos);
    if (epos == pos) { ELOG(1); goto err; }

    //��������� ��� ��������� �� ���������� ���������� �����
    for (int32_t ipos=pos+1; ipos<epos;){
      BlockType btype = c.at(ipos)->type();
      if (btype < BtAppData){ //���� (����������) ��������������
        if (islot == -1) { ELOG(2); goto err; } //��� ���������������� ����� �����

        //������� ��������: ���� � ����� ���� ��������� ���������������, ��� ������ ��������������� ����
        //                  ���� � ����� ��� ���������� ���������������, ��� �� ������ ��������������� ����
        //                  ���� ���� ��������� ���������������, ��� ������ ������ ��������������� ���� ��� ����������� �� ��������
        while (islot<ss.count()){
          Slot *s = ss[islot];
          if (s->bstate() != BsUntrained || s->id().btype()==BtNbc) break;  //����� ������ ��������� ��� ��������� ���������������
          islot++;
        }
        if (islot>=ss.count()) { ELOG(8); rslt(EcBlockType); goto err; }
        //��������� ���������, ���� ����� ����� ��������
        BcSlot *bs = (BcSlot*)ss[islot];
        bs->clear();  //�������� ������
        int32_t cnt = bs->readParams(c,ipos,&_containerType);  //������� ��������� (������������ ��� ����� �� ���������)
        if (cnt == -1) { ELOG(5); goto err; }
        ipos += cnt;  //���������� ����������� �����
        islot++;      //������� � ���������� ����� �����
      } else {  //���� ��������������
        int32_t sh=0;
        switch (btype){
        case BtCScheme: //���� ����� ��������������
          //���� ����� ���������, ������� ������
          if (count()) { ELOG(6); goto err; }
          c.at(ipos)->pop(sh,*this);  //��������� �����: ����������� ��������� ����� ���� ����� �����
          if (!count()) { ELOG(7); goto err; }
          find(Null,Null,Null,&ss);   //����� � ���������� ����� ��������������, � ������� ����� ����������� ���������
          islot = 0;
          break;
        case BtExtention://���� ����������
          //�� ��������������
          ELOG(8);
          goto err;
        default:  //������ ����� ��������������
          //����������� ��� ����������� �������������
          info.push(*c.at(ipos));
        }
        ipos++;
      }
    }
    //����������� �������������� �����
    _info.push(info);
    //���������������� ���������
    sync();
    return epos-pos;
  err:
    free();
    if (rslt().succeeded()) rslt(0,EcConsistencyFail);
    return -1;
  }



  bool BcScheme::descript(const SlotId id, QDataStream &to, bool recur){
    int32_t pos = indexOf(id);
    if (pos==-1) return rslt(0,EcSlotId);
    BcSlot* s = (BcSlot*)at(pos);
    s->writeSlot(to);
    if (recur){
      while (s->isReferred() && (s->id().type()==StSpecOut)){
        s = s->ref();
        s->writeSlot(to);
      }
    }
    return true;
  }


}
