#include "BcCondition.h"

namespace Bc {



  BcCondition::BcCondition():_var(VarUnknown){

  }



  bool BcCondition::validate() const {
    if (!BcSlot::validate()) { ELOG(1); return false; }
    if (!scheme()) return true;
    BcSlots si, ssi, sso, so; cons(&si,&ssi,&sso,&so);
    //��������� ����� ����������:
    //  � ��. ������ ���� ������ 2
    //  � ��.��. ������ ���� ����� 0, 1 ��� ��.
    //  ��.���. �����������
    //  ���. ����
    if (si.count() < 2) { ELOG(2); return erslt(EcCScheme); }
    if (ssi.count() != 0 && ssi.count() != 1  && ssi.count() != si.count()) { ELOG(3); return erslt(EcCScheme); }
    if (sso.count()) { ELOG(4); return erslt(EcCScheme); }
    if (so.count() != 1) { ELOG(5); return erslt(EcCScheme); }
    //��������� ������������ ����������:
    //  ��� ������ ����� ������������ ������
    //  � ��. ������ ���� ���������� � ��������� � ���.
    //  � ��.��. ������ ��������������� ���������� (1,I32,DO)
    if (!meta().size()) { ELOG(6); return erslt(EcMeta); }
    for (int32_t i=0; i<si.count(); ++i)
      if (si[i]->meta() != meta()) { ELOG(7); return erslt(EcMeta); }
    for (int32_t i=0; i<ssi.count(); ++i)
      if (ssi[i]->meta() != Meta(0,MfI32,MtDO)) { ELOG(8); return erslt(EcMeta); }
    //��������� �����
    //  BfHidden, BfLocked, BfBackwardTrain  ��������������
    //  BfProtected ������������
    //  BfGenerator, BfThrough, BfIterativeTrain  �����������
    if (_bf & BlockFlags(BfIterativeTrain | BfGenerated | BfThrough)) { ELOG(8); return erslt(EcBlockFlag); }
    //��������� ����������� �� ����� �������� ����������
    if (_bs != BsUntrained && _bs != BsTrained) { ELOG(9); return erslt(EcBlockState); }
    return true;
  }


  bool BcCondition::update(){
    if (!BcSlot::update()) { ELOG(1); return false; }
    //���������� ������� � ����������� �� ����� ������� ����������
    int32_t cnt = cids(StSpecIn).count();
    switch (cnt){
    case 0:  _var = VarExistence; break;
    case 1:  _var = VarIndex; break;
    default: _var = VarIndicator;
    }
    return true;
  }


  bool BcCondition::reset(){
    return BcSlot::reset();
  }


  bool BcCondition::train(TrainMode mode, BlockState state, BlockFlags flags){
    //�������� ��������������
    //  [��1,(x,x,x)]..[��N,(x,x,x)] -> [���,(x,x,x)]
    //  [��1,(x,x,x)]..[��N,(x,x,x)][��.��,(1,I32,DO)] -> [���,(x,x,x)]
    //  [��1,(x,x,x)]..[��N,(x,x,x)][��.��1,(1,I32,DO)]..[��.��N,(1,I32,DO)] -> [���,(x,x,x)]
    if (mode & TmWithoutFlags) flags = _bf;
    if (mode & TmWithoutState) state = _bs;
    if (state != BsTrained) return erslt(EcBlockState);
    //���������� �����
    if (flags & BlockFlags(BfIterativeTrain | BfGenerated | BfThrough)) { ELOG(1); return erslt(EcBlockFlag); }
    if (((flags ^ _bf) & BfLocked) || ((flags ^ _bf) & BfHidden)){
      if (state != _bs) { ELOG(2); return false; }
      if (!(mode & TmCheckOnly)) _bf = flags;
      return true;
    }

    int32_t ncols;
    BcSlots si, ssi, so; cons(&si,&ssi,Null,&so);

    if (flags & BfBackwardTrain){
      if (!actual(CtOwn)) return erslt(EcNoDataOwn);  //��������� �������� ��������
      prepare(si,CtOwn,actual(CtOwn));  //�������� ���������
      for (int32_t i=0; i<si.count(); ++i){            //���������� ������� ��������� �� ��������
        si[i]->constraint(CtOwn).copy(so[0]->constraint(CtOwn));
      }
    } else {
      if (!actualize(so,CtOwn,ncols,true)) return erslt(EcNoDataOwn); //��������� ������� ��������� (����� ������ �������������)
      prepare(so,CtOwn,ncols);
      so[0]->constraint(CtOwn).copy(si[0]->constraint(CtOwn));        //���������� �������� ��� �� ���������� ��������
    }
    _bf = flags;
    _bs = state;
    return true;
  }



  bool BcCondition::emulate(ConstraintType cti, ConstraintType cto, bool recur){
    if (actual(cto)) return true;
    if (bstate() == BsUntrained) { ELOG(1); return false; }

    int32_t index=0;
    int32_t ncols;
    BcSlots si, ssi, so; cons(&si,&ssi,Null,&so);
    BcSlot *s;
    BcSlots si2, ssi2;
    //��������������� ����������� � ����������� ����� ������� ���������� �������� �������������� ��������
    //
    //VarExistence; ��������� �������� �� ������� ���������
    //VarIndex; ��������� �������� �������
    //VarIndicator; ��������� ��� �������� � ������� ����� ����������� ������
    //���� �� ���� �� ��������� �� �������, ������������ ������ NoData
    switch (_var){
    case VarExistence:  //��������� ��� ��������, �� ������� ���������
      ncols=0;
      for (int32_t i=0; i<si.count(); ++i){
        BcSlot* s = si.at(i);
        if (!s->actual(cti) && (!recur || !s->emulate(cti,cti,recur))){
          if (scheme()->rslt().failed() && scheme()->rslt().code() != ResultCode(EcNoData)) { ELOG(2); return false; }  //������, ������ ���������� ������
          else { scheme()->rslt(Result::ok); continue; }                      //���������� �����, ������� ���
        }
        ncols = s->actual(cti);              //��������� ����� ��������
        index = i;
        break;
      }
      if (!ncols) { ELOG(3); return false; } //������� �� ������
      break;
    case VarIndex:      //��������� �������� �������
      {
        Matrix::Row rsi = ssi[0]->constraint(CtAny);
        if (rsi.count() != 1) { ELOG(10); return false; }
        index = rsi.at(0).i32[0];

        s = si[index];
        if (!s->actual(cti) && (!recur || !s->emulate(cti,cti,recur))) { ELOG(4); return false; }
        ncols = s->actual(cti);  //��������� ����� ��������
        if (!ncols) return false; //������� �� ������
      }
      break;
    case VarIndicator:  //��������� ��� ��������, ������� ����� ��� ��������
      ncols=0;
      for (int32_t i=0; i<si.count(); ++i){
        BcSlot* s = si.at(i);   //��������� ��. ��������
        if (!s->actual(cti) && (!recur || !s->emulate(cti,cti,recur))){
          if (scheme()->rslt().failed() && scheme()->rslt().code() != ResultCode(EcNoData)) { ELOG(5); return false; }  //������, ������ ���������� ������
          else { scheme()->rslt(Result::ok); continue; }                      //���������� �����, ������� ���
        }

        BcSlot* t = ssi.at(i);  //��������� ��������������� ��� ����������
        if (!t->actual(cti) && (!recur || !t->emulate(cti,cti,recur))){
          if (scheme()->rslt().failed() && scheme()->rslt().code() != ResultCode(EcNoData)) { ELOG(6); return false; }  //������, ������ ���������� ������
          else { scheme()->rslt(Result::ok); continue; }                      //���������� �����, ������� ���
        }

        //��������� ����� ���������
        if (ncols==0) ncols = s->actual(cti);              //��������� ����� �������� ������� �����
        else if (ncols != s->actual(cti)) { ELOG(7); return false; } //��������� ���������� ����� ��������
        //��������� ���������� �����
        si2.push_back(si[i]);
        ssi2.push_back(ssi[i]);
      }
      if (!ncols) { ELOG(8); return false; } //������� �� ������
      break;
    default: { ELOG(10); return false;}
    }

    //����������� �������� ��������
    if (!prepare(so,cto,ncols)) { ELOG(9); return erslt(EcOutOfMemory); }

    //����������� �������� � ����������� �� �������� ����������
    switch (_var){
    case VarIndicator: //�� �������� ���������� (���������� ���������� | ��� ��������� - ������)
      {
        //�� ���������
        Matrix::Rows ri; constraints(si2,cti,ri);
        Matrix::Rows rsi; constraints(ssi2,cti,rsi);
        Matrix::Row  ro = so[0]->constraint(cto);

        for (int32_t icol=0; icol<ncols; ++icol){
          //������� ������ � ������������� �������
          int32_t jrow = 0;
          for (int32_t irow=1; irow<ri.count(); ++irow){
            Matrix::Row mrow = rsi[irow];
            Matrix::Row crow = rsi[jrow];
            if (mrow[icol].i32[0] > crow[icol].i32[0]) jrow = irow; //��������� ����� ��������� � ����������� ��������� ����������
          }
          ro.copy1(ri[jrow],icol,icol);
        }
      }
      break;
    case VarIndex:     //�� ������ (���������� ������������� ��� ������������, ���� ����)
    case VarExistence: //�� ������� ���������
    {
        Matrix::Row  ri = si[index]->constraint(cti);
        Matrix::Row  ro = so[0]->constraint(cto);
        ro.copy(ri);
      }
      break;
    default:
      return false;
    }
    return true;
  }



  bool BcCondition::test(){
    if (actual(CtTst)) return true;
    BcSlots si; cons(&si,Null,Null,Null);

    Matrix::Row tst = constraint(CtTst);
    if (!tst.resize(1)) { ELOG(1); return false; }
    for (int32_t i=0; i<9; ++i)
      tst.at(0).r32[i] = 0.0f;

    //P1 = aver(P1[0],..,P1[i]) ������� ����� ���������
    //P2 = max(P2[0],..,P2[i]) ������������ ����� ���������, �.�. ����� ������ ���������� ���������

    for (int32_t i=0; i<si.count(); ++i){
      Matrix::Row tst1 = si[i]->constraint(CtTst);
      if (!tst1.count()) return false;

      tst.at(0).r32[TmE1] += tst1.at(0).r32[TmE1];
      if (!i) tst.at(0).r32[TmE2] = tst1.at(0).r32[TmE2];
      else    tst.at(0).r32[TmE2] = max(tst.at(0).r32[TmE2],tst1.at(0).r32[TmE2]);
    }

    for (int32_t i=3; i<9; ++i)
      tst.at(0).r32[i] = tst.at(0).r32[TmE2];

    return true;
  }



  int32_t BcCondition::writeParams(Container &c, const Uuid *ct, const BlockType *bt) const {
    if (bstate() == BsUntrained) return 0;
    //�������� ������ ����, �.�. �������� ���������� ���
    c.push(Block(BtCondition));
    c.last()->update();
    return 1;
  }



  int32_t BcCondition::readParams(const Container &c, int32_t pos, const Uuid *ct){
    //������� ������ ����, �.�. �������� ���������� ���
    Block* b = c.at(pos);
    if (!b || b->type() != BtCondition){ erslt(EcBlockType); ELOG(1); return -1; }
    if (!b->isEmpty()) { erslt(EcConsistencyFail); ELOG(2); return -1; }
    return 1;
  }


}
