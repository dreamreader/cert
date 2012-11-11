#include "RowMath.h"
#include "BcNeuralConverter.h"

namespace Bc {
  using namespace Nb;



  bool BcNeuralConverter::validate() const{
    if (!BcSlot::validate()) return false;
    //Проверить флаги
    //  BfHidden, BfLocked, BfProtected поддерживаются
    //  BfIterativeTrain, BfBackwardTrain, BfGenerator, BfThrough  запрещаются
    if (_bf & BlockFlags(BfIterativeTrain | BfBackwardTrain | BfGenerated | BfThrough)) { ELOG(1); return erslt(EcBlockFlag); }
    //Состояние проверяется во время загрузки параметров
    if (_bf != BsUntrained && _bf != BsTrained) { ELOG(2); return erslt(EcBlockState); }

    if (!scheme()) return true;
    BcSlots si, ssi, sso, so; cons(&si,&ssi,&sso,&so);
    //Проверить число параметров: вх.=1, сп.вх.=0,сп.вых.=0,вых.=1
    //ЧИСЛО ВХОДОВ БОЛЬШЕ 1 ПОКА НЕ ПОДДЕРЖИВАЕТСЯ
    if (si.count()!=1 || ssi.count() || sso.count() || so.count()!=1) { ELOG(3); return erslt(EcCScheme); }
    //Проверить шаблон метаописания вх. параметров Meta(x,R32,C)
    //и выходного кода Meta(y,I1,DO) и его кратность 8 битам
    if (!meta().subrange(Meta(0,MfI1,MtDO))) { ELOG(4); return erslt(EcMeta); }
    if (meta().count() % 8) { ELOG(5); return erslt(EcMeta); }
    for (int32_t i=0; i<si.count(); ++i){
      Meta m2 = si[i]->meta();
      if (!m2.subrange(Meta(0,MfR32,MtC))) { ELOG(6); return erslt(EcMeta); }
    }
    return true;
  }



  bool BcNeuralConverter::reset(){
    BcSlot::reset();
    _layer.clear();
    return true;
  }



  bool BcNeuralConverter::train(TrainMode mode, BlockState state, BlockFlags flags){
    //Обработать режим обучения: TmWithoutFlags, TmWithoutState, TmCheckOnly
    if (mode & TmWithoutFlags) flags = _bf;
    if (mode & TmWithoutState) state = _bs;
    //Обработать состояние: BsUntrained, BsTrained, BsPartially
    if (state != BsTrained) { ELOG(1); return false; }
    //Обработать флаги
    if (flags & BlockFlags(BfIterativeTrain | BfBackwardTrain | BfGenerated | BfThrough)) { ELOG(3); return false; }
    if (((flags ^ _bf) & BfLocked) || ((flags ^ _bf) & BfHidden)){
      if (state != _bs) { ELOG(2); return false; }
      if (!(mode & TmCheckOnly)) _bf = flags;
      return true;
    }

    int32_t ncols;
    BcSlots si, so; cons(&si,Null,Null,&so);
    if (si.count()!=1) { ELOG(7); return false; }

    //Проверить входные и выходные параметры и число примеров
    if (!actualize(si,CtOwn,ncols,true) || ncols<2 || ncols>64) { ELOG(4); return false; }
    if (!actualize(si,CtAll,ncols,true) || ncols<2) { ELOG(5); return false; }
    if (actual(CtOwn)!=1) { ELOG(6); return false; }

    //Необходимо переобучение
    if (state == BsTrained){
      Matrix::Row key = so[0]->constraint(CtOwn);
      //Переобучить, если идентичны
      if (flags == _bf){
        //Объединить примеры, если
        Matrix::Row own = si[0]->constraint(CtOwn);
        Matrix::Row all = si[0]->constraint(CtAll);

        if (!_layer.config(Uuid::nil,own.meta().count(),key.meta().count(),32,0,Layer::CdaRandom,16,false,32,true)) { ELOG(13); return false; }  //Сконфигурировать нейронную сеть (хранить без сжатия)
        if (!_layer.train(own,all,key,Layer::WtaFullUsing/*WtaMinAndSuff*/)) { ELOG(14); erslt(EcNoDataOwn); return false; }   //Обучить нейронную сеть
        //Протестировать в открытом режиме
        test();
      }
      //Защитить, если защищен
      if ((flags ^ _bf) == BfProtected){
        if (!_layer.protect(key))
          { ELOG(15); return false; }
        //Протестировать в защищенном режиме
        test();
      }
    }
    _bf = flags;
    _bs = state;
    return true;
  }



  bool BcNeuralConverter::emulate(ConstraintType cti, ConstraintType cto, bool recur){
    if (actual(cto)) return true;
    if (bstate() == BsUntrained) { ELOG(1); return false; }

    int32_t ncols;
    BcSlots si, so; cons(&si,Null,Null,&so);
    if (!actualize(si,cti,ncols,recur)) { ELOG(2); return false; }

    if (!prepare(so,cto,ncols)) { ELOG(3); return false; }

    Matrix::Row in  = si[0]->constraint(cti);
    Matrix::Row out = so[0]->constraint(cto);
    if (!_layer.sim(in,out,_bf & BfProtected)) { ELOG(7); return false; }
    return true;
  }



  bool BcNeuralConverter::test(){
    if (actual(CtTst)) return true;

    BcSlots si, so; cons(&si,Null,Null,&so);
    Matrix::Row key = so[0]->constraint(CtOwn);
    Matrix::Row own = si[0]->constraint(CtOwn);
    Matrix::Row all = si[0]->constraint(CtAll);

    if (key.isEmpty()) { ELOG(1); return false; }

    //Протестировать в открытом режиме на базах "Свой" и "все Чужие"
    Matrix::Row tst = constraint(CtTst);
    if (!tst.resize(1)) { ELOG(2); return false; }
    for (int32_t i=0; i<9; ++i)
      tst.at(0).r32[i] = 0.0f;

    if (!own.isEmpty()){
      _layer.testP1(own,key,tst.at(0).r32[1]); //Вычислить ошибку первого рода на "Свой"
    }

    if (!all.isEmpty()){
      _layer.testP2(all,key,tst.at(0).r32[2]); //Вычислить ошибку второго рода на "все Чужие"

      tst.at(0).r32[3] = tst.at(0).r32[2];
      tst.at(0).r32[4] = tst.at(0).r32[2];
      tst.at(0).r32[5] = tst.at(0).r32[2];
      tst.at(0).r32[6] = tst.at(0).r32[2];
      tst.at(0).r32[7] = tst.at(0).r32[2];
      tst.at(0).r32[8] = tst.at(0).r32[2];
    }
    return true;
  }



  int32_t BcNeuralConverter::writeParams(Container &c, const Uuid *ct, const BlockType *bt) const {
    if (!(bstate() & BsTrained)) return 0;

    c.push(Block(BtNeuralConv));
    Block *b = c.last();
    QDataStream to(&b->asByteArray(),QIODevice::WriteOnly);
    if (!_layer.write(to,to)) { c.pop(1); ELOG(1); return -1; }  //Записать параметры
    b->update();  //Обновить размер блока
    return 1;
  }


  int32_t BcNeuralConverter::readParams(const Container &c, int32_t pos, const Uuid *ct){
    Block *b = c.at(pos);
    if (!b) return -1;
    QDataStream from(&b->asByteArray(),QIODevice::ReadOnly);
    if (!_layer.read(from,from)) { ELOG(1); return -1; }
    return 1;
  }

}
