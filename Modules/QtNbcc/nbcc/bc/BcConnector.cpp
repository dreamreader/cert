#include "math.h"
#include "RowMath.h"
#include "BcConnector.h"

namespace Bc {



  BcConnector::BcConnector():_var(VarUnknown){
  }



  bool BcConnector::validate() const{
    if (!BcSlot::validate()) { ELOG(1); return false; }
    if (!scheme()) return true;
    BcSlots s2si, iso, io; cons(&s2si,&s2si,&iso,&io);
    //Проверить число параметров:
    //  вх. + сп.вх. > 1
    //  сп.вых. отсутствуют
    //  вых. один
    if (s2si.count() < 2) { ELOG(2); return erslt(EcCScheme); }
    if (iso.count()) { ELOG(3); return erslt(EcCScheme); }
    if (io.count() != 1) { ELOG(4); return erslt(EcCScheme); }
    //Проверить шаблон метаописания параметров Meta(x,I1,DO)
    //и кратность длины 8 битам
    if (!meta().subrange(Meta(0,MfI1,MtDO))) { ELOG(5); return erslt(EcMeta); }
    if (meta().size() > 255) { ELOG(10); return erslt(EcMeta); } //ограничение из-за формата хранения длины 1 байтом
    if (meta().count() % 8) { ELOG(6); return erslt(EcMeta); }
    for (int32_t i=0; i<s2si.count(); ++i){
      Meta m2 = s2si[i]->meta();
      if (!m2.subrange(Meta(0,MfI1,MtDO))) { ELOG(7); return erslt(EcMeta); }
      if (m2.count() % 8) { ELOG(8); return erslt(EcMeta); }
    }
    //Проверить флаги
    //  BfHidden, BfLocked, BfBackwardTrain, BfGenerator, BfThrough поддерживаются
    //  BfProtected пропускается
    //  BfIterativeTrain  запрещаются
    if (_bf & BfIterativeTrain) { ELOG(9); return erslt(EcBlockFlag); }
    //Состояние проверяется во время загрузки параметров
    if (_bs != BsUntrained && _bs != BsTrained) { ELOG(10); return erslt(EcBlockState); }
    return true;
  }



  bool BcConnector::update(){
    if (!BcSlot::update()) return false;
    if (!scheme()) return true;

    //Определить вариант в зависимости от числа входных параметров:
    //найти сумму длин параметров и сравнить ее
    BcSlots s2si; cons(&s2si,&s2si,Null,Null);
    int32_t sum=0;
    for (int32_t i=0; i<s2si.count(); ++i)
      sum += s2si[i]->meta().count();
    if (sum == (int32_t)meta().count()) _var = VarComponentCode;
    else                                _var = VarConnectedCode;

    //Вычислить метаописание соли и дополнения в зависимости от длины параметров
    _salt.remeta(Meta(16*8,MfI1,MtDO));
    _xor.remeta(Meta(meta()));

    return true;
  }



  bool BcConnector::reset(){
    BcSlot::reset();
    _salt.row(0).clear();
    _xor.row(0).clear();
    return true;
  }

  bool BcConnector::train(TrainMode mode, BlockState state, BlockFlags flags){
    //Варианты преобразования
    //[вх1,(a,I1,DO)]..[сп.вхN,(c,I1,DO)] -> вх1||..||сп.вхN -> [вых,(a+..+c,I1,DO)]
    //[вх1,(a,I1,DO)]..[сп.вхN,(a,I1,DO)] -> Md5(вх1||..||сп.вхN,prmS) + prmA -> [вых,(z,I1,DO)], z != a+..+c
    //  во втором случае эффективная длина выходного кода не больше длины хэша
    if (mode & TmWithoutFlags) flags = _bf;
    if (mode & TmWithoutState) state = _bs;
    if (state != BsTrained) { ELOG(1); return erslt(EcBlockState); }
    //Обработать флаги
    if (flags & BfIterativeTrain) { ELOG(2); return erslt(EcBlockFlag); }
    if (((flags ^ _bf) & BfLocked) || ((flags ^ _bf) & BfHidden)){
      if (state != _bs) { ELOG(3); return erslt(EcBlockFlag); }
      if (!(mode & TmCheckOnly)) _bf = flags;
      return true;
    }

    int32_t ncols;
    BcSlots si, so; cons(&si,&si,Null,&so); //слоты параметров
    Matrix::Rows ris; constraints(si,CtOwn,ris);    //примеры входных и сп.входных параметров
    Matrix::Row  ro = so[0]->constraint(CtOwn);     //примеры выходного параметра

    //В зависимости от варианта преобразования выполнить
    switch (_var){
    case VarConnectedCode: //связанный код
      {
        //Определить входные и выходные параметры в зависимости от флагов
        if (flags & BfBackwardTrain){
          //Проверить выходной код (он должен быть задан)
          if (!actual(CtOwn)) { ELOG(4); return erslt(EcNoDataOwn); }
          //Проверить флаги и входные параметры
          if (flags & BfGenerated){ //Сгенерировать входные параметры случайным образом
            if (!prepare(si,CtOwn,1)) { ELOG(5); return erslt(EcNoDataOwn); }
            if (!RowMath::gen(ris))   { ELOG(6); return erslt(EcNoDataOwn); }
          } else {
            //Использовать имеющиеся значения
          }
          //Проверить наличие
          if (!actualize(si,CtOwn,ncols,true) || ncols != 1) { ELOG(7); return erslt(EcNoDataOwn); }
        } else {
          //Проверить выходные коды (они должны быть заданы заранее)
          if (!actualize(si,CtOwn,ncols,true) || ncols != 1) { ELOG(8); return erslt(EcNoDataOwn); }
          //Проверить флаги и выходной код
          if (flags & BfGenerated){
            if (!prepare(so,CtOwn,1)) { ELOG(9); return erslt(EcNoDataOwn); }
            if (!RowMath::gen(ro)) { ELOG(10); return erslt(EcFail); }
          } else {
            //Использовать имеющееся значение
          }
          //Проверить наличие
          if (!actual(CtOwn)) { ELOG(11); return erslt(EcNoDataOwn); }
        }
        //Выполнить связывание входных и выходных кодов с помощью хэширования (как и для CodeTransformer)
        Matrix::Row salt  = _salt.row(0); salt.resize(1); RowMath::gen(salt);
        Matrix::Row xr    = _xor.row(0); xr.resize(1);
        if (!RowMath::hash(ris,xr,&ro,&salt)) { ELOG(12); return erslt(EcFail); }
      }
      break;
    case VarComponentCode: //составной код
      //Определить входные и выходные параметры в зависимости от флагов
      if (flags & BfBackwardTrain){
        //Входные коды находятся по выходному коду (флаги не нужны, код должен быть установлен)
        if (!actual(CtOwn)) { ELOG(13); return erslt(EcNoDataOwn); }
        if (!prepare(si,CtOwn,1)) { ELOG(14); return erslt(EcNoDataOwn); }
        if (!RowMath::uncat(ro,ris)) { ELOG(15); return erslt(EcFail); }
      } else {
        //Выходной код находится по выходному коду (флаги не нужны, коды должны быть установлены)
        if (!actualize(si,CtOwn,ncols,true)) { ELOG(16); return erslt(EcNoDataOwn); }
        if (!prepare(so,CtOwn,ncols)) { ELOG(17); return erslt(EcNoDataOwn); }
        if (!RowMath::cat(ris,ro)) { ELOG(18); return erslt(EcFail); }
      }
      break;
    default: break;
    }
    _bs = BsTrained;
    _bf = flags;
    return true;
  }



  bool BcConnector::emulate(ConstraintType cti, ConstraintType cto, bool recur){
    if (actual(cto)) return true;
    if (bstate() == BsUntrained) { ELOG(1); return erslt(EcBlockState); }

    int32_t ncols;
    BcSlots si, so; cons(&si,&si,Null,&so);

    if (!actualize(si,cti,ncols,recur)) { ELOG(2); return erslt(EcNoData); }
    if (!prepare(so,cto,ncols)) { ELOG(3); return erslt(EcNoData); }

    Matrix::Rows ris;  constraints(si,cti,ris);
    Matrix::Row  ro  = so[0]->constraint(cto);
    switch (_var){
    case VarConnectedCode://связанный код
      {
        Matrix::Row salt  = _salt.row(0);
        Matrix::Row xr    = _xor.row(0);
        if (!RowMath::hash(ris,ro,&xr,&salt)) { ELOG(4); return erslt(EcFail); }
      }
      break;
    case VarComponentCode://составной код
      if (!RowMath::cat(ris,ro)) { ELOG(5); return erslt(EcFail); }
      break;
    default: return false;
    }
    return true;
  }



  bool BcConnector::test(){
    if (actual(CtTst)) return true;

    BcSlots si; cons(&si,&si,Null,Null);

    Matrix::Row tst = constraint(CtTst);
    if (!tst.resize(1)) { ELOG(1); return false; }
    for (int32_t i=0; i<9; ++i)
      tst.at(0).r32[i] = 0.0f;

    //P1 = (1 - (1-P1[0])*(1-P1[1])*..*(1-P1[i]))
    //P2 = P2[0]*P2[1]*..*P2[i] но не больше длины выходного ключа и длины хеша

    tst.at(0).r32[TmE1] = 1.0f;
    tst.at(0).r32[TmE2] = 1.0f;
    for (int32_t i=0; i<si.count(); ++i){
      Matrix::Row tst1 = si[i]->constraint(CtTst);
      if (!tst1.count()) return false;

      tst.at(0).r32[TmE1] *= (1.0f - tst1.at(0).r32[TmE1]);
      tst.at(0).r32[TmE2] *= tst1.at(0).r32[TmE2];
    }
    tst.at(0).r32[TmE1] = 1.0f - tst.at(0).r32[TmE1];

    float p2outMax = 1.0f/pow(2.0f,(float)meta().count());        //Максимальная стойкость кода
    float p2hashMax= 1.0f/pow(2.0f,(float)Md5Hash::md5HASH_BITS); //Максимальная стойкость хеша

    tst.at(0).r32[TmE2] = max(max(tst.at(0).r32[TmE2],p2outMax),p2hashMax); //Максимальная P2 среди минимальных

    for (int32_t i=3; i<9; ++i)
      tst.at(0).r32[i] = tst.at(0).r32[TmE2];

    return true;
  }



  int32_t BcConnector::writeParams(Container &c, const Uuid *ct, const BlockType *bt) const{
    if (!(bstate() & BsTrained)) return 0;
    c.push(Block(BtConnector));
    Block *b = c.last();
    uint8_t ssize, xsize;
    switch (_var){
    case VarConnectedCode:
      //примечание: Длина выходного кода ограничена длиной 255*8 бит
      ssize = (uint8_t)_salt.meta(0).size();
      xsize = (uint8_t)_xor.meta(0).size();
      (*b).push((int8_t)ssize);
      (*b).push(_salt.row(0).at(0).u8,ssize);
      (*b).push((int8_t)xsize);
      (*b).push(_xor.row(0).at(0).u8,xsize);
      break;
    case VarComponentCode:
      break;
    default: c.pop(1); ELOG(1); return -1;
    }
    b->update();  //Обновить размер блока
    return 1;
  }



  int32_t BcConnector::readParams(const Container &c, int32_t pos, const Uuid *ct){
    Block *b = c.at(pos);
    if (!b) { ELOG(1); return -1; }
    int32_t p=0;
    uint8_t ssize=0, xsize=0;
    switch (_var){
    case VarConnectedCode:
      (*b).pop(p,(int8_t&)ssize);
      if (!_salt.remeta(Meta(ssize*8,MfI1,MtDO))) { ELOG(2); erslt(EcMeta); return -1; }
      if (!_salt.resize(1)) { ELOG(3); erslt(EcOutOfMemory); return -1; }
      (*b).pop(p,_salt.row(0).at(0).u8,ssize);

      (*b).pop(p,(int8_t&)xsize);
      if (!_xor.remeta(Meta(xsize*8,MfI1,MtDO)))  { ELOG(4); erslt(EcMeta); return -1; }
      if (!_xor.resize(1)) { ELOG(5); erslt(EcOutOfMemory); return -1; }
      (*b).pop(p,_xor.row(0).at(0).u8,xsize);
      break;
    case VarComponentCode:
      break;
    default: ELOG(2); return -1;
    }
    return 1;
  }


}
