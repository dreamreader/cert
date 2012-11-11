
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
      //case BtNbc: return Null;  //Вложенные преобразователи не поддерживаются
      case BtConnector:   return new BcConnector();
      case BtCondition:   return new BcCondition();
      case BtNeuralConv:  return new BcNeuralConverter();
      default: break;     //Другие типы преобразователей не поддерживаются
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



  //Установка данных слотов
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


  //Чтение данных слотов
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
    if (stt & SsScheme){ //сброс схемы
      //Удалить схему и все параметры
      free();
      return true;
    }
    Slots ss;
    if (stt & SsTrained){  //cброс параметров обучения и результатов тестирования
      //Выполнить для преобразователей
      find(Null,Null,Null,&ss);
      for (int32_t i=0; i<ss.count(); ++i){
        ((BcSlot*)ss[i])->reset();
        ((BcSlot*)ss[i])->clear(CtTst);
      }
    }
    if (stt & SsInAny || stt & SsInOwn){  //сброс вх. и сп.вх. параметров (имеет смысл по отношению к схеме целиком)
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
    if (stt & SsOutAny || stt & SsOutOwn){ //сброс результатов вычислений (связанные параметры сбрасываются схемой)
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
    Slots ss;   find(ss,ids(StConvertor), true);  //Найти все слоты преобразования
    BcSlots st; st.reserve(ss.count());
    //Исключить из схемы все заблокированные слоты
    for (int32_t i=0; i<ss.count(); ++i)
      if (!(ss[i]->bflags() & BfHidden)) st.push_back((BcSlot*)ss[i]);
    //Сбросить состояние обучаемых слотов (для мультипреобразователя вызовет сброс состояния вложенных слотов)
    for (int32_t i=0; i<st.count(); ++i)
      st[i]->reset();
    //Двигаться с двух сторон по списку, обучая с одной или другой стороны
    int32_t l = 0, r = st.count()-1;
    Result err;
    do {
      //Обучить ближайший к началу/концу преобразователь
      if (st[l]->train(TmWithoutFlags,BsTrained,0)){
        ++l;
      } else {
        err = rslt();
        if (l!=r && st[r]->train(TmWithoutFlags,BsTrained,0)){
          --r;
        } else {
          //Вернуть первый неудачный результат
          rslt(err);
          return false;
        }
      }
    } while (l<=r);
    sync();
    return true;
  }



  bool BcScheme::test(Matrix::Row &tr){
    //TmE1Bio успешность, вычисляется на поданных примерах "Свой"
    //TmE2Bio стойкость "Все чужие" для контейнера в двс, =TmE2BioCorrNoise + разм.ош.
    //TmE2BioHumanCompromised стойкость при компрометации одного примера, =0 (нет блока аудита)
    //TmE2BioCompromised стойкость при компрометации одного вектора, =0 (нет блока аудита)
    //TmE2Code стойкость, =длина вых.кода
    //TmE2BioWhiteNoise стойкость при атаке белым шумом, =длина вх.
    //TmE2BioCorrNoise стойкость при атаке коррелированным шумом (ожидаемая по "Все чужие")
    //TmE2Conv стойкость для контейнера вне двс, =TmE2BioCorrNoise + индикаторы + разм.ош.

    //Найти все слоты
    Slots ss; if (!find(ss,ids(StConvertor),true)) { ELOG(1); return rslt(0,EcCScheme); }
    Slots ss2; ss2.reserve(ss.count());
    //Переупорядочить слоты, разместив индикаторы непосредственно за связанными слотами
    for (int32_t i=0; i<ss.count(); ++i){
        Slot* s = ss[i];
        if (s->id().btype() == BtSoftIndicator || s->id().btype() == BtHardIndicator){  //Переместить индикаторы сразу за зависимыми слотами
          int32_t idx = ss.indexOf(at(s->cids()[0])); //Найти позицию зависимого слота
          if (idx<0) { ELOG(2); return rslt(0,EcCScheme); } //Обработать ошибку схемы преобразования
          ss2.insert(idx+1,s);                        //Добавить непосредственно за найденным слотом
        } else {
          ss2.push_back(s);                           //Добавить в нормальном режиме
      }
    }

    //Протестировать слоты в указанном порядке
    for (int32_t i=0; i<ss2.count(); ++i){
      BcSlot* s = (BcSlot*)ss2[i];
      if (!s->test()) { ELOG(3); return false; }
    }

    //Считать результат с результирующего слота
    BcSlot* s = (BcSlot*)at(SlotId::out);
    if (!s->get(CtTst,tr)) { ELOG(4); return false; } //Получить результат и обработать ошибку выделения памяти

    return true;
  }


  bool BcScheme::qeval(Matrix::Row &ge){
    //Рейтинг качества примеров должен изменяться от 0 (лучший) до X (произвольный), I32
    if (ge.meta().count() != 1) return rslt(0,EcMeta);
    if (ge.meta().format() != MfR32) return rslt(0,EcMeta);
    //Сформировать список входных параметров
    Slots ss; find(&ss,Null,Null,Null);

    //Получить оценку ошибок для каждого вх. параметра, для которого установлен вектор CtOwn
    //и добавить параметры, для которых выполнен расчет в список вычисленных
    Matrix::Rows ges;   //значения ge для вычисленных параметров
    QList<float> mids;  //средние значения ge
    for (int32_t i=0; i<ss.count(); ++i){
      BcSlot* s = (BcSlot*)ss[i];
      if (!s->qeval()) continue;
      Matrix::Row ge1 = s->constraint(CtGe);
      mids.append(averColR32(ge1.data(),ge1.count(),0));  //Рассчитать среднее по примерам
      ges.append(ge1);
    }

    for (int32_t i=0; i<ge.count(); ++i){   //Обнулить
      ge.at(i).r32[0] = 0.0f;
    }

    for (int32_t j=0; j<ges.count(); ++j){  //Добавить ges
      Matrix::Row gej = ges[j];
      int32_t i;
      for (i=0; i<min(gej.count(),ge.count()); ++i) //Сложить с gej
        ge.at(i).r32[0] += gej.at(i).r32[0];
      for (; i<ge.count(); ++i) //Добавить среднее для отсутствующих примеров
        ge.at(i).r32[0] += mids[j];
    }

    if (ges.count())
    for (int32_t i=0; i<ge.count(); ++i){   //Пронормировать, чтобы сохранить интервал 0..1
      ge.at(i).r32[0] /= ges.count();
    }

    //Стереть все значения ge входных параметров
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
    int32_t ct=0, nt=0, pt=0;   //число обученных преобразователей, вложенных преобразователей, частично обученных
    _sf = _sf & SfValidated;  //Сбросить флаги обучения

    Slots ss; find(Null,Null,Null,&ss);
    if (!ss.count()) return;

    for (int32_t i=0; i<ss.count(); ++i){
      bool nested   = (ss[i]->id().btype() == BtNbc);
      if (nested)
        ((BcSlot*)ss[i])->sync();  //Синхронизировать вложенный преобразователь
      BlockState bs = ss[i]->bstate();
      if (bs == BsUntrained){ //необучен
        if (ct) break;  //Уже можно маркировать как SfPartially и SfConv1
      } else if (bs == BsPartially){  //обучен (или частично для вложенных)
        if (nested) { pt++; if (ct) break; } //Уже можно маркировать как SfPartially и SfConv1
        else ct++;
      } else {  //обучен с некоторым состоянием
        if (nested) nt++;
        else ct++;
      }
    }
    if (ct)                   _sf |= SfConv1;   //Хотя бы 1 преобразователь обучен
    if (ct+nt == ss.count())  _sf |= SfTrained;  //Все обучены полностью
    else if (ct+nt+pt)        _sf |= SfPartially;//Обучены частично
  }



  // {30413CB1-53A7-4b62-9F01-0E430E7E3EF8}
  const Uuid BcScheme::_containerType = Uuid(0x30413cb1, 0x53a7, 0x4b62, 0x9f, 0x1, 0xe, 0x43, 0xe, 0x7e, 0x3e, 0xf8);



  int32_t BcScheme::writeParams(Container &c, const Uuid *ct) const{
    //Определить поддерживаемый тип контейнера
    if (ct && (!ct->isNull() || *ct != _containerType)) { ELOG(1); return -1; }
    //Записать заголовок контейнера и сохранить позицию
    c.push(BlockContHead(_containerType,0));
    int32_t hpos = c.count()-1;
    BlockContHead*  h = (BlockContHead*)c.at(hpos);
    SchemeFlags sf = state();
    //if (sf & SfValidated){
      //Записать схему преобразования
      c.push(Block(BtCScheme));
      Block* s = c.last();
      s->push(*this);
      if (s->isEmpty()){
        c.pop(c.count()-hpos);
        return -1;
      }
      bool ist = sf & SfConv1;  //Если хотя бы 1 простой преобразователь обучен, записать все параметры преобразователей
      Slots ss; find(Null,Null,Null,&ss);
      for (int32_t i=0; i<ss.count(); ++i){
        if (((Slot*)ss[i])->id().btype() == BtNbc || ist)
          if (((BcSlot*)ss[i])->writeParams(c,&_containerType,Null)==-1){ //Записать параметры
            c.pop(c.count()-hpos);                              //Откатить запись
            ELOG(2);
            return -1;
          }

      }
    //}
    if (_info.count())
      c.push(_info);                      //Сохранить информационные блоки в конце контейнера
    h->update(c.all(),hpos,c.count());  //Обновить размер контейнера
    return c.count()-hpos;              //Вычислить число записанных блоков
  }

  //Чтение контейнера
  //возвращает: число считанных блоков или -1
  int32_t BcScheme::readParams(const Container &c, int32_t pos){
    rslt(Result::ok);
    Slots   ss;       //список загруженных слотов преобразования схемы, имеющий параметры
    int32_t islot=-1; //номер слота преобразования из схемы, для которого загружаются параметры
    Container::Factory f;
    Container info(f);
    //Найти номер блока (вложенного) контейнера следующего за последним
    int32_t epos = c.findEnd(pos);
    if (epos == pos) { ELOG(1); goto err; }

    //Загрузить все следующие за заголовком контейнера блоки
    for (int32_t ipos=pos+1; ipos<epos;){
      BlockType btype = c.at(ipos)->type();
      if (btype < BtAppData){ //БЛОК (ВЛОЖЕННОГО) ПРЕОБРАЗОВАНИЯ
        if (islot == -1) { ELOG(2); goto err; } //Нет соответствующего блоку слота

        //правила загрузки: если в схеме есть обученный преобразователь, ему должен соответствовать блок
        //                  если в схеме нет обученного преобразователя, ему не должен соответствовать блок
        //                  если есть вложенный преобразователь, ему всегда должен соответствовать блок вне зависимости от обучения
        while (islot<ss.count()){
          Slot *s = ss[islot];
          if (s->bstate() != BsUntrained || s->id().btype()==BtNbc) break;  //Найти первый обученный или вложенный преобразователь
          islot++;
        }
        if (islot>=ss.count()) { ELOG(8); rslt(EcBlockType); goto err; }
        //Загрузить контейнер, если номер слота реальный
        BcSlot *bs = (BcSlot*)ss[islot];
        bs->clear();  //Сбросить данные
        int32_t cnt = bs->readParams(c,ipos,&_containerType);  //Считать параметры (ИСПОЛЬЗОВАТЬ ТИП БЛОКА ИЗ ЗАГОЛОВКА)
        if (cnt == -1) { ELOG(5); goto err; }
        ipos += cnt;  //Пропустить загруженные блоки
        islot++;      //Перейти к следующему блоку схемы
      } else {  //БЛОК ИНФОРМАЦИОННЫЙ
        int32_t sh=0;
        switch (btype){
        case BtCScheme: //БЛОК СХЕМЫ ПРЕОБРАЗОВАНИЯ
          //Если схема загружена, вернуть ошибку
          if (count()) { ELOG(6); goto err; }
          c.at(ipos)->pop(sh,*this);  //Загрузить схему: выполняется повторный сброс всех полей схемы
          if (!count()) { ELOG(7); goto err; }
          find(Null,Null,Null,&ss);   //Найти и установить слоты преобразования, в которые будут загружаться параметры
          islot = 0;
          break;
        case BtExtention://БЛОК РАСШИРЕНИЯ
          //Не обрабатывается
          ELOG(8);
          goto err;
        default:  //ДРУГИЕ БЛОКИ ИНФОРМАЦИОННЫЕ
          //Скопировать для дальнейшего использования
          info.push(*c.at(ipos));
        }
        ipos++;
      }
    }
    //Скопировать информационные блоки
    _info.push(info);
    //Синхронизировать состояние
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
