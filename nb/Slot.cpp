//////////////////////////////////////////////////////////////////////////
//файл:         
//версия:
//дата:
//изменен:
//автор:
//
//цель:         
//
//
//
//////////////////////////////////////////////////////////////////////////
#include "Slot.h"
#include "Scheme.h"

namespace Nb {

  Slot::Slot():
    _s(Null),_r(Null),_bf(BfNone),_bs(BsUntrained){
  }



  Slot::Slot(SlotId id, Meta meta):
    _s(Null),_r(Null),_id(id),_meta(meta),_bf(BfNone),_bs(BsUntrained){
  }



  Slot::Slot(SlotId id, Meta meta, const SlotIds &cids, BlockFlags bf, BlockState bs):
    _s(Null),_r(Null),_id(id),_meta(meta),_bf(bf),_bs(bs),_cids(cids){
  }



  Slot::Slot(SlotId id, Meta meta, const SlotIds &cids, const SlotIds &rids, BlockFlags bf, BlockState bs, Scheme *r):
    _s(Null),_r(Null),_id(id),_meta(meta),_bf(bf),_bs(bs){
    if (cids.size() == rids.size() && cids.size()<128){
      _cids = cids;
      _rids = rids;
    }
    rscheme(r);
  }



  Slot::~Slot(){
    //  _cids и _rids освобождаются автоматически
  }


  void    Slot::ccount(int32_t *n){
    n[0]=0; n[1]=0; n[2]=0; n[3]=0;
    for (int32_t i=0; i<_cids.count(); ++i){
      n[((uint16_t)_cids[i].type())>>14]++;
    }
  }



  void    Slot::rcount(int32_t *n){
    n[0]=0; n[1]=0; n[2]=0; n[3]=0;
    for (int32_t i=0; i<_rids.count(); ++i){
      n[((uint16_t)_rids[i].type())>>14]++;
    }
  }



  SlotIds Slot::cids(SlotType st) const{
    SlotIds ids;
    for (int32_t i=0; i<_cids.count(); ++i){
      if (_cids.at(i).type() == st)
        ids.push_back(_cids.at(i));
    }
    return ids;
  }



  void  Slot::cids(SlotIds *ii, SlotIds *isi, SlotIds *iso, SlotIds *io) const {
    for (int32_t i=0; i<_cids.count(); ++i){
      switch (_cids[i].type()){
      case StIn:      if (ii)   ii->push_back(_cids[i]); break;
      case StSpecIn:  if (isi)  isi->push_back(_cids[i]); break;
      case StSpecOut: if (iso)  iso->push_back(_cids[i]); break;
      default:        if (io)   io->push_back(_cids[i]); break;
      }
    }
    if (io) io->push_back(id());
  }


  bool Slot::copy(const Slot &slot){
    _s    = slot._s;    //При копировании схемы преобразования ссылка слота на s-схему должна изменяться
    _id   = slot._id;
    _meta = slot._meta;
    _bf   = slot._bf;
    _bs   = slot._bs;
    _cids = slot._cids;
    _r    = slot._r;
    _rids = slot._rids;
    return true;
  }



  int32_t Slot::sizeSlot(bool first){
    int32_t sz = 0;
    if (!first) //Первый слот интерпретировать как слот параметра вне зависимости от его фактического типа
    if (_id.type() == StOut){    //дополнительно для слота преобразователя
      if (_id.btype() == BtNbc){  //дополнительно для слота мультипреобразователя
        sz += sizeof(SlotId)*count(); //Добавить размер для r-номеров
      }
      sz += sizeof(SlotId)*count(); //Добавить размер для c-слотов
      sz += sizeof(uint8_t);
      sz += sizeof(BlockState);
      sz += sizeof(BlockFlags);
    }
    //дополнительно для слота параметра
    sz += sizeof(Meta);
    sz += sizeof(SlotId);
    return sz;
  }

  //Загрузка данных слота из потока
  //примечание: не поддерживает схемы с полем count=0xFF
  int32_t Slot::readSlot(SlotId id, QDataStream &from, int32_t remain, bool first){
    int32_t r = 0;
    _id = id;
    if (first) id = SlotId::out;  //Установить для первого слота тип "слот параметра"
    //Считать метаописание
    if ((uint32_t)(remain-r) < sizeof(Meta)) goto err;
    r += from.readRawData((char*)&_meta,sizeof(Meta));
    if (from.status() != QDataStream::Ok) goto err;

    if (id.type() == Nb::StOut){  //Считать параметры преобразователя
      if ((uint32_t)(remain-r) < (sizeof(BlockFlags) + sizeof(BlockState) + sizeof(uint8_t))) goto err;
      r += from.readRawData((char*)&_bf,sizeof(BlockFlags));
      r += from.readRawData((char*)&_bs,sizeof(BlockState));
      uint8_t count = 0;
      r += from.readRawData((char*)&count,sizeof(uint8_t));
      if (from.status() != QDataStream::Ok) goto err;
      if (count>254) return -1;
      if ((remain-r) < (int32_t)((uint32_t)count*sizeof(SlotId))) goto err;

      if (id.btype() == BtNbc){  //Считать список связанных и отображенных слотов
        _cids.reserve(count/2);
        _rids.reserve(count/2);
        for (int32_t i=0; i<(int32_t)count/2; ++i){
          SlotId cid;
          SlotId rid;
          r += from.readRawData((char*)&cid,sizeof(SlotId));
          _cids.push_back(cid);
          r += from.readRawData((char*)&rid,sizeof(SlotId));
          _rids.push_back(rid);
        }
      } else {  //Считать список связанных слотов
        _cids.reserve(count);
        for (int32_t i=0; i<count; ++i){
          SlotId cid;
          r += from.readRawData((char*)&cid,sizeof(SlotId));
          _cids.push_back(cid);
        }
      }
    }
    if (from.status() != QDataStream::Ok) goto err;
    return r;
  err:
    erslt(EcIo);
    return -1;
  }



  //Сохранение данных слота в потоке
  int32_t Slot::writeSlot(QDataStream &to, bool first) const {
    int32_t w = 0;
    //Установить для первого слота тип "слот параметра"
    SlotId id = _id;
    if (first) id=SlotId::out;
    //Считать данные слота параметра
    w += to.writeRawData((char*)&_id,sizeof(SlotId));
    w += to.writeRawData((char*)&_meta,sizeof(Meta));

    if (id.type() == StOut){  //Считать параметры преобразователя
      w += to.writeRawData((char*)&_bf,sizeof(BlockFlags));
      w += to.writeRawData((char*)&_bs,sizeof(BlockState));

      uint8_t count = (uint8_t)(_cids.size() + _rids.size());
      w += to.writeRawData((char*)&count,sizeof(uint8_t));
      if (id.btype() == BtNbc){  //Считать список связанных и отображенных слотов
        for (int32_t i=0; i<_cids.size(); ++i){
          w += to.writeRawData((char*)&_cids[i],sizeof(SlotId));
          w += to.writeRawData((char*)&_rids[i],sizeof(SlotId));
        }
      } else {  //Считать список связанных слотов
        for (int32_t i=0; i<_cids.size(); ++i){
          w += to.writeRawData((char*)&_cids[i],sizeof(SlotId));
        }
      }
    }
    if (to.status() != QDataStream::Ok) goto err;
    return w;
  err:
    erslt(EcIo);
    return -1;
  }



  bool Slot::scheme(Scheme* s){
    if (s==_s) return true;
    _s = s;
    return update() && validate();
  }



  bool Slot::rscheme(Scheme* r){
    if (r==_r) return true;
    if (r && (_id.type() != StOut || _id.btype() != BtNbc)) return erslt(EcCScheme);
    _r = r;
    return rupdate() && rvalidate();
  }



  bool Slot::validate() const{
    switch (_id.type()){
    case StIn:
    case StSpecIn:
    case StSpecOut:     //для вх., сп.вх., сп.вых. параметров
      if (_bf != BfNone) goto err;
      if (_bs != BsUntrained) goto err;
      if (_cids.size()) goto err;
      if (_rids.size()) goto err;
      if (_r) goto err;
      if (_s){
        if (this != _s->at(_id)) goto err; //Проверить наличие слота в схеме
        //Проверить однократное исп. сп.вых. параметра в качестве сп.вых.
        Slots ss;
        _s->used(ss,SlotId::retype(_id,StSpecOut),false);
        if (_id.type() == StSpecOut){ //Проверить для сп.вых. слота
          if (ss.count() >= 2) goto err;
        }
      }
      return true;
    case StOut:   //для преобразователей
      //Проверить, что cids не содержит слотов, используемых как StOut
      for (int32_t i=0; i<_cids.count(); ++i)
        if (_cids[i].type() == StOut) goto err;
      //Проверить, что только сп.вых. параметры исп. в качестве сп.вых.
      if (_s){
        for (int32_t i=0; i<_cids.count(); ++i){
          SlotId cid = _cids[i]; SlotId sid = _s->idOf(cid);
          if (sid == SlotId::out) goto err;
          if (cid.type() == StSpecOut && sid.type() != StSpecOut) goto err;
        }
      }
      if (_id.btype() != BtNbc){  //для невложенного преобразователя
        //Схема и отображенные слоты должны отсутствовать
        if (_r || _rids.count()) goto err;
      } else {                    //для вложенного преобразователя
        //Число cids и rids должно совпадать
        if (_cids.size() != _rids.count()) goto err;
        //Проверить соответствие типов связанных и отображенных слотов
        //согласно их фактическому типу
        for (int32_t i=0; i<_cids.count(); ++i){
          SlotType ct = _cids[i].type(); SlotType rt = _rids[i].type();
          if (rt==StOut) rt = StSpecOut;
          if (ct != rt) goto err;
          //примечание:
          //  тот же слот вых. параметра может исп. в качестве входного и выходного,
          //  поэтому цикличность проверяется только в методе validate s-схемы
        }
      }
    }
    return true;
  err:
    return erslt(EcCScheme);

  }



  bool Slot::rvalidate() const {
    //Проверка нужна только для вложенного преобразователя
    if (_id.type() == StOut && _id.btype() == BtNbc) {
      //Проверить наличие rids слотов в r-схеме и совпадение их описания
      if (_r){
        for (int32_t i=0; i<_rids.count(); ++i){
          SlotId rid = _rids[i]; SlotId sid = _r->idOf(rid);
          //= -> ==
          if (SlotId::out == sid) goto err;
          if (rid != sid) goto err;
        }
      }
      //Проверить совпадение метаописания
      if (_r && _s){
        for (int32_t i=0; i<_cids.count(); ++i){
          if (_s->at(_cids[i])->meta() != _r->at(_rids[i])->meta())
            goto err;
        }
      }
    }
    return true;
  err:
    return erslt(EcCScheme);
  }



  bool Slot::update(){
    return true;
  }



  bool Slot::rupdate(){
    return true;
  }


  Result Slot::rslt() const {
    Scheme* s = scheme();
    return (s)? s->rslt(): Result();
  }



  bool Slot::erslt(ResultCode rc) const {
    Scheme* s = scheme();
    if (s) s->rslt(id(),rc);
    return false;
  }



  bool Slot::rslt(Result r) const {
    Scheme* s = scheme();
    if (s) s->rslt(r);
    return r.succeeded();
  }


#if !defined(QT_NO_DEBUG_STREAM)
  //Вывод информации слота (зависимые схемы не выводятся)
  QDebug Slot::debug(QDebug  dbg) const{
    QDebug d = dbg.nospace();
    d<<"Slot("<<_id<<" "<<_meta<<" "<<_bf<<" "<<_bs;
    if (!_s) d<<" "<<_cids;
    else {
      d<<" [";
      for (int i=0; i<_cids.count(); ++i){
        if (i) d<<" ";
        d<< _s->indexOf(_cids.at(i));
      }
      d<<"]";
    }
    if (!_r) d<<" "<<_rids;
    else {
      d<<" [";
      for (int i=0; i<_rids.count(); ++i){
        if (i) d<<" ";
        d<< _r->indexOf(_rids.at(i));
      }
      d<<"]";
    }
    if (_s) d<<" "<<_s; else d<<" Nil";
    if (_r) d<<" "<<_r; else d<<" Nil";
    return d<<")";
  }

  QDebug operator<<(QDebug  dbg, const Slot &x){
    return x.debug(dbg);
  }

  QDebug operator<<(QDebug  dbg, const Slots &x){
    dbg.nospace() << '[';
    for (int i = 0; i < x.count(); ++i) {
        if (i) dbg << "\n";
        dbg << i<<":"<<*x.at(i);
    }
    dbg << ']';
    return dbg.space();
  }

#endif


}//end namespace Nb

