#include "BcNbcc.h"

namespace Bc {

  BcNbcc::BcNbcc(){
    _cRefs.ref();
  }

  BcNbcc::~BcNbcc(){
    _cRefs.deref();
  }

  QAtomicInt BcNbcc::_cRefs = 0;

  //Значение глобального счетчика компонентов
  nbResult BcNbcc::getCoRefs(){
    return _cRefs;
  }

  nbMETHOD_DEF(BcNbcc,queryInterface)(nbTHISc nbRefUuid iid, void **icd){
    if (Uuid(iid) == Uuid(nbUUID_IUNKNOWN) || Uuid(iid) == Uuid(nbUUID_INBCC)){
      *icd = static_cast<nbINbcc*>(this);
      static_cast<nbIUnknown*>(*icd)->retain();
      return Result::success(ScOk);
    }
    return Result::fail(EcNoInterface);
  }



  nbMETHOD_DEF(BcNbcc,retain)(nbTHIS){
    return _iRefs.ref();
  }




  nbMETHOD_DEF(BcNbcc,release)(nbTHIS){
    if (!_iRefs.deref()){
      delete this;
      return nbS_OK;
    }
    return _iRefs;
  }



  nbMETHOD_DEF(BcNbcc,setEventHandler)(nbTHISc nbIEventHandler *handler){
    return Result::fail(EcUnsupportedFunction);
  }



  nbMETHOD_DEF(BcNbcc,setConvScheme)(nbTHISc const nbData cScheme){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    QByteArray ba = QByteArray::fromRawData((const char*)cScheme,Data::ref(cScheme).size());
    QDataStream from(&ba,QIODevice::ReadOnly);
    _scheme.readScheme(from,ba.size());
    return _scheme.rslt();
  }



  nbMETHOD_DEF(BcNbcc,enumConvScheme)(nbTHISc nbRefUuid csid, const nbMatrix *mIn, const nbMatrix *mCode, nbData cScheme){
    return Result::fail(EcUnsupportedFunction);
  }



  nbMETHOD_DEF(BcNbcc,getSlotDescr)(nbTHISc nbSlotID id, nbData slotDescr){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    //Найти слот
    Slot* slot = _scheme.at(SlotId(id));
    if (!slot) return Result::fail(EcSlotId);
    Data d(slotDescr);
    if (d.size() >= slot->sizeSlot(false)){
      //Записать слот, и изменить размер
      QByteArray ba = QByteArray::fromRawData((const char*)d.data(),d.size());
      QDataStream to(&ba,QIODevice::ReadOnly);
      d.resize(slot->writeSlot(to,false));
      return Result::success(ScOk);
    } else {
      //Указать размер
      d.setWishfulSize(slot->sizeSlot(false));
      return Result::fail(EcInsufficientBuffer);
    }
  }



  nbMETHOD_DEF(BcNbcc,setConstraint)(nbTHISc nbSlotID id, nbConstraintType conType, nbMatrix *mCon){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    if (!mCon) return Result::fail(EcPointer);
    _scheme.set(SlotId(id),conType,Matrix::Row(mCon,0),false);
    return _scheme.rslt();
  }



  nbMETHOD_DEF(BcNbcc,getConstraint)(nbTHISc nbSlotID id, nbConstraintType conType, nbMatrix *mCon){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    if (!mCon) return Result::fail(EcPointer);
    Matrix::Rows value = Matrix::fromM(*mCon);
    _scheme.get(SlotId::fromIDs(&id,1),conType,value);
    return _scheme.rslt();
  }



  nbMETHOD_DEF(BcNbcc,extract)(nbTHISc const nbMatrix *mIn, nbMatrix *mOut, const nbSlotID *ids){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    if (!mOut) return Result::fail(EcParam);
    SlotIds idi = _scheme.ids(StIn);
    SlotIds ido = (ids)? SlotId::fromIDs(ids,mOut->nrows) : SlotId::fromIDs((nbSlotID*)&SlotId::out,1);  //Номера вых. слотов

    if (mIn && !_scheme.set(idi,CtAny,Matrix::fromM(*mIn),true)){
      return _scheme.rslt();
    }

    if (!_scheme.emulate(ido,CtAny))
      return _scheme.rslt();

    Matrix::Rows any = Matrix::fromM(*mOut);
    if (!_scheme.get(ido,CtAny,any))
      return _scheme.rslt();

    _scheme.clear(CtAny);
    return Result::success(ScOk);
  }



  nbMETHOD_DEF(BcNbcc,train)(nbTHISc const nbMatrix *mInOwn, const nbMatrix *mInAll, const nbMatrix *mCode, nbTimeout timeout){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    SlotIds idi = _scheme.ids(StIn);
    SlotIds ido = SlotId::fromIDs((nbSlotID*)&SlotId::out,1);

    if (mInOwn && ! _scheme.set(idi,CtOwn,Matrix::fromM(*mInOwn),true))
      return _scheme.rslt();

    if (mInAll && ! _scheme.set(idi,CtAll,Matrix::fromM(*mInAll),true))
      return _scheme.rslt();

    if (mCode && !_scheme.set(ido,CtOwn,Matrix::fromM(*mCode),true))
      return _scheme.rslt();

    _scheme.clear(CtTst);
    _scheme.train();
    return _scheme.rslt();
  }



  nbMETHOD_DEF(BcNbcc,postTrain)(nbTHISc nbSlotID id, nbTrainMode trainMode, nbBlockState blockState, nbBlockFlags blockFlags, nbTimeout timeout){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    BcSlot* slot = (BcSlot*)_scheme.at(SlotId(id));
    if (!slot) return Result::fail(EcSlotId);

    _scheme.clear(CtTst);
    slot->train(trainMode,blockState,blockFlags);
    return _scheme.rslt();
  }



  nbMETHOD_DEF(BcNbcc,indicateGrossErrors)(nbTHISc const nbMatrix *mInOwn, const nbMatrix *mInAll, nbMatrix *mGe){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    //Переустановить заданные входные параметры "Свой"
    if (mInOwn){
      SlotIds ids = _scheme.ids(StIn);
      for (int32_t i=0; i<ids.count(); ++i){
        Matrix::Row own(mInOwn,i);
        if (!_scheme.set(ids[i],CtOwn,own,true)) return _scheme.rslt();
      }
    }
    //Переустановить заданные входные параметры "все Чужие"
    if (mInAll){
      SlotIds ids = _scheme.ids(StIn);
      for (int32_t i=0; i<ids.count(); ++i){
        Matrix::Row all(mInAll,i);
        if (!_scheme.set(ids[i],CtOwn,all,true)) return _scheme.rslt();
      }
    }
    //Подсчитать грубые отклонения для входных параметров
    Matrix::Row ge(mGe,0);
    if (!_scheme.qeval(ge)) return _scheme.rslt();
    return Result::ok;
  }



  nbMETHOD_DEF(BcNbcc,test)(nbTHISc nbTestMode testMode, nbMatrix *mEstimate, nbTimeout timeout){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    //Если результат уже получен, вернуть требуемый
    if (TestMode(testMode)<TmE1Bio || TestMode(testMode)>TmE2Conv) return Result().fail(EcTestMode);

    Matrix::Row est(mEstimate,0);
    if (est.isEmpty()) return Result().fail(EcParam);
    if (!est.meta().count() || est.meta().format() != MfR32) return Result().fail(EcMeta);

    Matrix mTst(1,Meta(9,MfR32,MtCO)); Matrix::Row tst = mTst.row(0);
    if (!_scheme.test(tst)) return _scheme.rslt();

    est.at(0).r32[0] = tst.at(0).r32[TestMode(testMode)];
    return Result::ok;
  }



  nbMETHOD_DEF(BcNbcc,reset)(nbTHISc nbNbccState nbccState){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    _scheme.reset(nbccState);
    return _scheme.rslt();
  }



  nbMETHOD_DEF(BcNbcc,exportNbc)(nbTHISc nbData nbc, nbRefUuid nbcType){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    Container::Factory f;
    Container c(f);
    //Записать в один контейнер
    if (-1 == _scheme.writeParams(c,Null)){
      return _scheme.rslt();
    }

    //Рассчитать размер
    if (c.size() > Data::ref(nbc).size()){
      Data::ref(nbc).setWishfulSize(c.size());
      return Result::fail(EcInsufficientBuffer);
    } else {
      QByteArray ba;
      QDataStream to(&ba,QIODevice::WriteOnly);
      c.write(to,true);
      Data d = Data::ref(ba);
      Data d2= Data::ref(nbc);
      if (!d2.copy(d)) return Result::fail(EcOutOfMemory);
      return Result::ok;
    }
  }

  nbMETHOD_DEF(BcNbcc,importNbc)(nbTHISc const nbData nbc){
    _scheme.rslt(Result::ok); //Сбросить код ошибки
    Container::Factory f;
    Container c(f);
    QByteArray ba = QByteArray::fromRawData((const char*)nbc,Data::ref(nbc).size());
    QDataStream from(&ba,QIODevice::ReadOnly);
    if (-1 == c.read(from,-1)) return Result::fail(EcConsistencyFail); //UPD: (from,-1) <- (from,ba.size())
    _scheme.readParams(c,0);
    return _scheme.rslt();
  }
}


