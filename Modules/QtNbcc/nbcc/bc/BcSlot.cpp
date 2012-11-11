
#include "BcScheme.h"
#include "BcSlot.h"

namespace Bc {

  BcSlot::BcSlot(){
  }

  BcSlot::~BcSlot(){
  }



  bool BcSlot::update(){
    _data[0].remeta(Meta(1,MfR32,MtCO));  //  CtGe  1 значение индикатора на пример
    _data[1].remeta(Meta(9,MfR32,MtCO));  //  CtTst 8 режимов тестирования + нулевой элемент
    _data[2].remeta(meta());              //  CtAny
    _data[3].remeta(meta());              //  CtOwn
    _data[4].remeta(meta());              //  CtAll

    //Сбросить данные связанных слотов
    _cons.clear();
    //Сформировать список связанных слотов
    if (!scheme() || !_cids.count()) return true;
    if (!scheme()->find((Slots&)_cons,_cids,false))
      return erslt(EcCScheme);
    return true;
  }



  bool BcSlot::rupdate(){
    //Сбросить отображенные слоты
    _refs.clear();
    //Сформировать отображенных слотов, если схема определена
    if (!rscheme() || !_rids.count()) return true;
    if (!rscheme()->find((Slots&)_refs,_rids,false))
      return erslt(EcCScheme);
    return true;
  }



  void BcSlot::sync(){
  }



  bool BcSlot::clear(){
    return  constraint(CtGe).resize(0) &&
            constraint(CtTst).resize(0) &&
            constraint(CtAny).resize(0) &&
            constraint(CtOwn).resize(0) &&
            constraint(CtAll).resize(0);
  }


  Matrix::Row BcSlot::constraint(ConstraintType ct) const {
    if ((int16_t)ct <= CtAll)
      return _data[(int16_t)ct+1].row(0);
    erslt(EcConstraint);
    ELOG(1);
    return Matrix::Row();
  }



  bool BcSlot::clear(ConstraintType ct){
    return constraint(ct).resize(0);
  }



  bool BcSlot::set(ConstraintType ct, const Matrix::Row &value){
    Matrix::Row c = constraint(ct);
    if (c.isNull()) { ELOG(1); return erslt(EcConstraint); }
    if (value.isEmpty()) //Очистить для нулевого параметра
      return clear(ct);
    //Установить для ненулевого
    if (c.meta() != value.meta()) { ELOG(2); return erslt(EcMeta); }
    if (!c.resize(value.count())) { ELOG(3); return erslt(EcOutOfMemory); }
    if (!c.copy(value)) { ELOG(4); return erslt(EcNoData); }
    return true;
  }



  bool BcSlot::get(ConstraintType ct, Matrix::Row &value) const {
    Matrix::Row c = constraint(ct);
    if (c.isNull()) { ELOG(1); return erslt(EcConstraint); }
    if (c.meta() != value.meta()) { ELOG(2); return erslt(EcMeta); }
    if (!value.copy(c)) { ELOG(3); return erslt(EcNoData); }
    return true;
  }



  int32_t BcSlot::actual(ConstraintType ct) const {
    return constraint(ct).count();
  }



  bool BcSlot::reset(){
    _bs = BsUntrained;
    return true;
  }



  bool BcSlot::qeval(){
    return false;
  }


  //.....................................................................................


  bool BcSlot::actualize(BcSlots &cons, ConstraintType ct, int32_t &ncols, bool emul){
    ncols=0;
    for (int32_t i=0; i<cons.count(); ++i){
      BcSlot* s = cons.at(i);
      if (!s->actual(ct) && (!emul || !s->emulate(ct,ct,true))) { ELOG(1); return false; }
      if (ncols==0) ncols = s->actual(ct);                        //Запомнить число примеров первого слота
      else if (ncols != s->actual(ct)) { ELOG(2); return false; } //Проверить совпадение числа примеров
    }
    return true;
  }



  bool BcSlot::prepare(BcSlots &cons, ConstraintType ct, int32_t ncols){
    for (int32_t i=0; i<cons.count(); ++i){
      if (!cons.at(i)->constraint(ct).resize(ncols))
        return false;
    }
    return true;
  }



  void BcSlot::cons(BcSlots *si, BcSlots *ssi, BcSlots *sso, BcSlots *so) const {
    for (int32_t i=0; i<_cons.count(); ++i){
      SlotId id = _cids.at(i);
      BcSlot* s = _cons.at(i);
      switch (id.type()){
      case StIn:      if (si) si->push_back(s);   break;
      case StSpecIn:  if (ssi) ssi->push_back(s); break;
      case StSpecOut: if (sso) sso->push_back(s); break;
      default: break;
      }
    }
    if (so) so->push_back((BcSlot*)this);
  }



  Matrix::Rows& BcSlot::constraints(const BcSlots &ss, ConstraintType ct, Matrix::Rows &rr){
    for (int32_t i=0; i<ss.count(); ++i)
      rr.push_back(ss.at(i)->constraint(ct));
    return rr;
  }

}
