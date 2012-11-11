#include "BcCondition.h"

namespace Bc {



  BcCondition::BcCondition():_var(VarUnknown){

  }



  bool BcCondition::validate() const {
    if (!BcSlot::validate()) { ELOG(1); return false; }
    if (!scheme()) return true;
    BcSlots si, ssi, sso, so; cons(&si,&ssi,&sso,&so);
    //Проверить число параметров:
    //  у вх. должно быть больше 2
    //  у сп.вх. должно быть равно 0, 1 или вх.
    //  сп.вых. отсутствуют
    //  вых. один
    if (si.count() < 2) { ELOG(2); return erslt(EcCScheme); }
    if (ssi.count() != 0 && ssi.count() != 1  && ssi.count() != si.count()) { ELOG(3); return erslt(EcCScheme); }
    if (sso.count()) { ELOG(4); return erslt(EcCScheme); }
    if (so.count() != 1) { ELOG(5); return erslt(EcCScheme); }
    //Проверить метаописание параметров:
    //  оно должно иметь определенный размер
    //  у вх. должно быть идентичным и совпадать с вых.
    //  у сп.вх. должно соответствовать индикатору (1,I32,DO)
    if (!meta().size()) { ELOG(6); return erslt(EcMeta); }
    for (int32_t i=0; i<si.count(); ++i)
      if (si[i]->meta() != meta()) { ELOG(7); return erslt(EcMeta); }
    for (int32_t i=0; i<ssi.count(); ++i)
      if (ssi[i]->meta() != Meta(0,MfI32,MtDO)) { ELOG(8); return erslt(EcMeta); }
    //Проверить флаги
    //  BfHidden, BfLocked, BfBackwardTrain  поддерживаются
    //  BfProtected пропускается
    //  BfGenerator, BfThrough, BfIterativeTrain  запрещаются
    if (_bf & BlockFlags(BfIterativeTrain | BfGenerated | BfThrough)) { ELOG(8); return erslt(EcBlockFlag); }
    //Состояние проверяется во время загрузки параметров
    if (_bs != BsUntrained && _bs != BsTrained) { ELOG(9); return erslt(EcBlockState); }
    return true;
  }


  bool BcCondition::update(){
    if (!BcSlot::update()) { ELOG(1); return false; }
    //Определить вариант в зависимости от числа входных параметров
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
    //Варианты преобразования
    //  [вх1,(x,x,x)]..[вхN,(x,x,x)] -> [вых,(x,x,x)]
    //  [вх1,(x,x,x)]..[вхN,(x,x,x)][сп.вх,(1,I32,DO)] -> [вых,(x,x,x)]
    //  [вх1,(x,x,x)]..[вхN,(x,x,x)][сп.вх1,(1,I32,DO)]..[сп.вхN,(1,I32,DO)] -> [вых,(x,x,x)]
    if (mode & TmWithoutFlags) flags = _bf;
    if (mode & TmWithoutState) state = _bs;
    if (state != BsTrained) return erslt(EcBlockState);
    //Обработать флаги
    if (flags & BlockFlags(BfIterativeTrain | BfGenerated | BfThrough)) { ELOG(1); return erslt(EcBlockFlag); }
    if (((flags ^ _bf) & BfLocked) || ((flags ^ _bf) & BfHidden)){
      if (state != _bs) { ELOG(2); return false; }
      if (!(mode & TmCheckOnly)) _bf = flags;
      return true;
    }

    int32_t ncols;
    BcSlots si, ssi, so; cons(&si,&ssi,Null,&so);

    if (flags & BfBackwardTrain){
      if (!actual(CtOwn)) return erslt(EcNoDataOwn);  //Проверить выходной параметр
      prepare(si,CtOwn,actual(CtOwn));  //Выделить параметры
      for (int32_t i=0; i<si.count(); ++i){            //Установить входные параметры по входному
        si[i]->constraint(CtOwn).copy(so[0]->constraint(CtOwn));
      }
    } else {
      if (!actualize(so,CtOwn,ncols,true)) return erslt(EcNoDataOwn); //Проверить входные параметры (найти первый установленный)
      prepare(so,CtOwn,ncols);
      so[0]->constraint(CtOwn).copy(si[0]->constraint(CtOwn));        //Установить выходной код по найденному входному
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
    //Актуализировать минимальное и достаточное число входных параметров согласно установленному варианту
    //
    //VarExistence; проверять варианты до первого успешного
    //VarIndex; проверять заданный вариант
    //VarIndicator; проверить все варианты и выбрать среди вычисленных нужный
    //если не один из вариантов не подошел, возвращается ошибка NoData
    switch (_var){
    case VarExistence:  //Проверить все варианты, до первого успешного
      ncols=0;
      for (int32_t i=0; i<si.count(); ++i){
        BcSlot* s = si.at(i);
        if (!s->actual(cti) && (!recur || !s->emulate(cti,cti,recur))){
          if (scheme()->rslt().failed() && scheme()->rslt().code() != ResultCode(EcNoData)) { ELOG(2); return false; }  //Ошибка, помимо отсутствия данных
          else { scheme()->rslt(Result::ok); continue; }                      //Продолжить поиск, сбросив код
        }
        ncols = s->actual(cti);              //Запомнить число примеров
        index = i;
        break;
      }
      if (!ncols) { ELOG(3); return false; } //Вариант не найден
      break;
    case VarIndex:      //Проверить заданный вариант
      {
        Matrix::Row rsi = ssi[0]->constraint(CtAny);
        if (rsi.count() != 1) { ELOG(10); return false; }
        index = rsi.at(0).i32[0];

        s = si[index];
        if (!s->actual(cti) && (!recur || !s->emulate(cti,cti,recur))) { ELOG(4); return false; }
        ncols = s->actual(cti);  //Запомнить число примеров
        if (!ncols) return false; //Вариант не найден
      }
      break;
    case VarIndicator:  //Проверить все варианты, оставив среди них успешные
      ncols=0;
      for (int32_t i=0; i<si.count(); ++i){
        BcSlot* s = si.at(i);   //Вычислить вх. параметр
        if (!s->actual(cti) && (!recur || !s->emulate(cti,cti,recur))){
          if (scheme()->rslt().failed() && scheme()->rslt().code() != ResultCode(EcNoData)) { ELOG(5); return false; }  //Ошибка, помимо отсутствия данных
          else { scheme()->rslt(Result::ok); continue; }                      //Продолжить поиск, сбросив код
        }

        BcSlot* t = ssi.at(i);  //Вычислить соответствующий код индикатора
        if (!t->actual(cti) && (!recur || !t->emulate(cti,cti,recur))){
          if (scheme()->rslt().failed() && scheme()->rslt().code() != ResultCode(EcNoData)) { ELOG(6); return false; }  //Ошибка, помимо отсутствия данных
          else { scheme()->rslt(Result::ok); continue; }                      //Продолжить поиск, сбросив код
        }

        //Проверить число элементов
        if (ncols==0) ncols = s->actual(cti);              //Запомнить число примеров первого слота
        else if (ncols != s->actual(cti)) { ELOG(7); return false; } //Проверить совпадение числа примеров
        //Запомнить вычислимые слоты
        si2.push_back(si[i]);
        ssi2.push_back(ssi[i]);
      }
      if (!ncols) { ELOG(8); return false; } //Вариант не найден
      break;
    default: { ELOG(10); return false;}
    }

    //Подготовить выходной параметр
    if (!prepare(so,cto,ncols)) { ELOG(9); return erslt(EcOutOfMemory); }

    //Скопировать элементы в зависимости от значения индикатора
    switch (_var){
    case VarIndicator: //по значению индикатора (выбирается наименьшее | при равенстве - первый)
      {
        //НЕ ПРОВЕРЕНО
        Matrix::Rows ri; constraints(si2,cti,ri);
        Matrix::Rows rsi; constraints(ssi2,cti,rsi);
        Matrix::Row  ro = so[0]->constraint(cto);

        for (int32_t icol=0; icol<ncols; ++icol){
          //Выбрать пример с существующими данными
          int32_t jrow = 0;
          for (int32_t irow=1; irow<ri.count(); ++irow){
            Matrix::Row mrow = rsi[irow];
            Matrix::Row crow = rsi[jrow];
            if (mrow[icol].i32[0] > crow[icol].i32[0]) jrow = irow; //Запомнить номер параметра с минимальным значением индикатора
          }
          ro.copy1(ri[jrow],icol,icol);
        }
      }
      break;
    case VarIndex:     //по номеру (выбирается установленный или существующий, если есть)
    case VarExistence: //по наличию параметра
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

    //P1 = aver(P1[0],..,P1[i]) средняя среди имеющихся
    //P2 = max(P2[0],..,P2[i]) максимальная среди имеющихся, т.е. самый плохой показатель стойкости

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
    //Записать пустой блок, т.к. хранимых параметров нет
    c.push(Block(BtCondition));
    c.last()->update();
    return 1;
  }



  int32_t BcCondition::readParams(const Container &c, int32_t pos, const Uuid *ct){
    //Считать пустой блок, т.к. хранимых параметров нет
    Block* b = c.at(pos);
    if (!b || b->type() != BtCondition){ erslt(EcBlockType); ELOG(1); return -1; }
    if (!b->isEmpty()) { erslt(EcConsistencyFail); ELOG(2); return -1; }
    return 1;
  }


}
