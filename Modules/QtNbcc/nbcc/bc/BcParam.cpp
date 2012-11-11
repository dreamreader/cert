
#include "NeuralMath.h"
#include "BcParam.h"

namespace Bc {
  using namespace Nb;



  bool BcParam::rrslt() const {
    return scheme()->rslt(id(),ref()->rscheme()->rslt().code());
  }



  bool BcParam::clear(){
    if (isReferred()){
      if (!ref()->clear()) return rrslt();
      else return true;
    }
    return BcSlot::clear();
  }



  bool BcParam::clear(ConstraintType ct){
    if (isReferred()){
      if (!ref()->clear(ct)) return rrslt();
      else return true;
    }
    return BcSlot::clear(ct);
  }



  bool BcParam::set(ConstraintType ct, const Matrix::Row &value){
    if (isReferred()){
      if (!ref()->set(ct,value)) return rrslt();
      else return true;
    }
    return BcSlot::set(ct,value);
  }



  bool BcParam::get(ConstraintType ct, Matrix::Row &value) const {
    if (isReferred()){
      if (!ref()->get(ct,value)) return rrslt();
      return true;
    }
    return BcSlot::get(ct,value);
  }



  int32_t BcParam::actual(ConstraintType ct) const {
    if (isReferred()) return ref()->actual(ct);
    return BcSlot::actual(ct);
  }



  Matrix::Row BcParam::constraint(ConstraintType ct) const {
    if (isReferred()) return ref()->constraint(ct);
    return BcSlot::constraint(ct);
  }



  //..........................................................................................................



  bool BcParam::reset(){
    if (isReferred()) {
      if (!ref()->reset()) return rrslt();
      refm()->sync();
      return true;
    }
    ELOG(1);
    return erslt(EcSlotId); //Слоты параметров не обучаются
  }



  bool BcParam::train(TrainMode mode, BlockState state, BlockFlags flags){
    if (isReferred()) {
      if (!ref()->train(mode,state,flags)) return rrslt();
      refm()->sync();
      return true;
    }
    ELOG(1);
    return erslt(EcSlotId); //Слоты параметров не обучаются
  }



  bool BcParam::emulate(ConstraintType cti, ConstraintType cto, bool recur){
    if (isReferred()){
      if (!ref()->emulate(cti,cto,recur)) return rrslt();
      return true;
    }
    if (constraint(cto).isEmpty()) { ELOG(1); return erslt(EcNoData); }
    return true;
  }



  bool BcParam::qeval(){
    if (isReferred()) {ELOG(1); return false;}        //Не проводить для ссылочных параметров
    if (id().type() != StIn) {ELOG(2); return false;} //Проводить только для вх. параметров

    if (meta().format() != MfR32 || meta().type() != MtC) { ELOG(3); return false; } //Только для непрерывных
    if (actual(CtOwn)<2) return false;     //Если данные не установлены, не выполнять

    Matrix::Row own = constraint(CtOwn);
    Matrix::Row ge  = constraint(CtGe);
    if (!ge.resize(own.count())) { ELOG(5); return erslt(EcOutOfMemory); }

    uint32_t nbetter=0;
    Vbp* better = new Vbp[own.count()];
    if (!better) { ELOG(6); return erslt(EcOutOfMemory);}
    //Определить грубые ошибки
    indicateGeR32(own.data(),own.count(),own.meta().count(),better,nbetter,ge.data());
    delete[] better;
    return true;
  }



  bool BcParam::test(){
    if (isReferred()){
      if (!ref()->test()) return rrslt();
      else return true;
    }
    ELOG(1);
    return erslt(EcSlotId); //Слоты параметров не тестируются
  }

  int32_t BcParam::writeParams(Container &c, const Uuid *ct, const BlockType *bt) const {
    return 0; //Параметры не сохраняются, в том числе и для отображений
  }

  int32_t BcParam::readParams(const Container &c, int32_t pos, const Uuid *ct){
    return 0; //Параметры не сохраняются, в том числе и для отображений
  }

}
