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
#include <QtCore/QVector>
#include "Scheme.h"

namespace Nb {

  //...........................................................................
  //Фабрика слотов
  //...........................................................................
  Slot* Scheme::Factory::create(SlotId id) const {
    return new Slot(id,Meta());
  }



  void Scheme::Factory::free(Slot *s) const {
    if (s) delete s;
  }



  //...........................................................................
  //Схема
  //...........................................................................
  Scheme::Scheme():_factory(Scheme::defaultFactory()),_posOut(-1){

  }



  Scheme::Scheme(const Scheme::Factory &factory):_factory(factory),_posOut(-1){
  }



  Scheme::~Scheme(){
    //free();
  }



  const Scheme::Factory& Scheme::factory() const {
    return _factory;
  }

  Scheme::Factory& Scheme::defaultFactory(){
    static Scheme::Factory def;
    return def;
  }

  bool Scheme::isSet() const {
    return (_slots.count() && _posOut >= 0);
  }



  int32_t Scheme::count() const{
    return _slots.size();
  }




  int32_t Scheme::indexOf(SlotId id) const{
    return indexOf(id,0);
  }



  int32_t Scheme::indexOf(SlotId id, int32_t start) const {
    if (id==SlotId::out){
      if (_posOut<0) return _slots.count()-1;
      else return _posOut;
    }
    for (int32_t i=start; i<_slots.count(); ++i){
      if (id.index()==_slots.at(i)->id().index())
        return i;
    }
    return -1;//Номер не найден
  }



  int32_t Scheme::indexOfLast(SlotType st, BlockType bt) const {
    for (int32_t i=_slots.count()-1; i>=0; --i){
      SlotId id = _slots[i]->id();
      if (id.type() == st && (bt==Nb::BtExtention || id.btype()==bt))
        return i;
    }
    return -1;
  }



  Slot* Scheme::at(SlotId id) const {
    int32_t i = indexOf(id);
    if (i<0) { ELOG2(1,Scheme::at); return Null; }
    return _slots.at(i);
  }




  Slot* Scheme::at(int32_t pos) const{
    if (pos<0 || pos>=_slots.count()) { ELOG2(1,Scheme::at); return Null; }
    return _slots.at(pos);
  }




  SlotId Scheme::idOf(SlotId id) const {
    Slot *s = at(id);
    if (s) return s->id();
    return SlotId::out;
  }

  SlotId Scheme::idOf(int32_t pos) const {
    if (pos >= 0 && pos<count())
      return _slots.at(pos)->id();
    return SlotId::out;
  }

  SlotIds Scheme::ids() const {
    SlotIds ids;
    for (int32_t i=0; i<_slots.count(); ++i)
      ids.push_back(_slots.at(i)->id());
    return ids;
  }



  SlotIds Scheme::ids(SlotType st, BlockType bt) const {
    SlotIds ids;    
    for (int32_t i=0; i<_slots.count(); ++i){
      SlotId id = _slots.at(i)->id();
      if (id.type() == st && (bt==Nb::BtExtention || id.btype()==bt))
        ids.push_back(id);
    }
    return ids;
  }



  bool Scheme::find(Slots &ss, const SlotIds &ids, bool ordered) const{
    int32_t start=0;
    int32_t col=0;
    for (int32_t i=0; i<ids.count(); ++i){
      int32_t cur = indexOf(ids.at(i),start);
      if (cur<0) ss.push_back(Null);  //Добавить пропуск
      else {
        if (ordered) start = cur;       //Сохранить позицию начала поиска, если список упорядочен
        ss.push_back(_slots.at(cur)); //Сохранить слот
        col++;
      }
    }
    return (col==ids.count());  //Установить результата в зависимости от нахождения всех слотов
  }



  bool Scheme::find(Slots *si, Slots *ssi, Slots *sso, Slots *so) const {
    for (int32_t i=0; i<_slots.count(); ++i){
      Slot* s = _slots[i];
      if (!s) ELOG(1);
      switch (s->id().type()){
      case Nb::StIn:      if (si)   si->push_back(s);  break;
      case Nb::StSpecIn:  if (ssi)  ssi->push_back(s); break;
      case Nb::StSpecOut: if (sso)  sso->push_back(s); break;
      case Nb::StOut:     if (so)   so->push_back(s);  break;
      }
    }
    return true;
  }

  bool Scheme::order(Slots &ss) const{
    QVector<Slot*> vs(ss.count());    vs.resize(0);
    QVector<int32_t> idx(ss.count()); idx.resize(0);
    for (int32_t i=0; i<ss.count(); ++i){         //Сформировать списки слотов и позиций без Null слотов
      Slot* s = ss[i];
      if (!s) continue;                           //Если Null-слот, пропустить
      int32_t pos = indexOf(s->id());             //Найти номер слота в схеме
      if (pos < 0 ) { ELOG(1); return false;}     //Если слот не найден, ошибка
      idx.push_back(pos);
      vs.push_back(s);
    }
    mathQSort(idx.data(),(void**)vs.data(),vs.count());   //Отсортировать список слотов по списку индексов
    ss.clear();                                   //Очистить список
    for (int32_t i=0; i<vs.count(); ++i)
      ss.push_back(vs[i]);                        //Добавить списки
    return true;
  }



  void Scheme::used(Slots &ss, SlotId id, bool lazy) const{
    //Добавить те слоты, у которых в списке связанных слотов есть id
    for (int32_t i=0; i<_slots.count(); ++i){
      Slot *s = _slots[i];
      if (s->id().type() != StConvertor) continue;  //Проверять только слоты преобразователей
      const SlotIds &cids = s->cids();
      for (int32_t j=0; j<cids.count(); ++j){
        if (cids[j]==id){
          ss.push_back(s);
          if (lazy) break;
        }
      }
    }
  }


  Scheme& Scheme::push(SlotType st, Meta meta){
    if (st == StOut) { ELOG(1); rslt(0,EcCScheme); return *this;}  //Ошибка, добавление не выполняется
    //Найти максимальный номер и увеличить на 1 (для -1 установить в 1)
    int32_t pos = max(max(indexOfLast(StIn),indexOfLast(StSpecIn)),indexOfLast(StSpecOut));
    if (pos==-1) pos = 1; else pos = _slots[pos]->id().index()+1;
    return push(Slot(SlotId::prm(st,pos),meta));
  }

  Scheme& Scheme::push(BlockType bt, Meta meta, const SlotTypes &ct, const SlotNums &ci, BlockFlags bf){
    if (bt == BtNbc) { ELOG(1); rslt(0,EcCScheme); return *this;}  //Ошибка, не для вложенного преобразователя
    if (ct.count() != ci.count()) {ELOG(2); rslt(0,EcCScheme); return *this;}
    //Найти максимальный индекс для этого типа элемента
    int32_t pos = indexOfLast(StConvertor,bt);
    if (pos==-1) pos = 1; else pos = _slots[pos]->id().bindex()+1;
    //Сформировать список связанных слотов
    SlotIds cids;
    for (int32_t i=0; i<ct.count(); ++i){
      if (ct[i] == StOut) { ELOG(3); rslt(0,EcCScheme); return *this;} //Ошибка, только вх., сп.вх., сп.вых.
      SlotId id = idOf(ci[i]);
      if (id == SlotId::out) {ELOG(4); rslt(0,EcCScheme); return *this;}  //Ошибка, слот не найден
      cids.append(id.as(ct[i]));
    }
    return push(Slot(SlotId::conv(bt,pos),meta,cids,bf));
  }

  Scheme& Scheme::push(Scheme &r, Meta meta, const SlotTypes &ct, const SlotNums &ci, const SlotNums &ri, BlockFlags bf){
    if  (ct.count() != ci.count() || ci.count() != ri.count()) { ELOG(1); rslt(0,EcCScheme); return *this;}  //Ошибка, число элементов
    //Найти максимальный индекс для этого типа элемента
    int32_t pos = indexOfLast(StConvertor,BtNbc);
    if (pos==-1) pos = 1; else pos = _slots[pos]->id().bindex()+1;
    //Сформировать список связанных слотов
    SlotIds cids;
    for (int32_t i=0; i<ct.count(); ++i){
      if (ct[i] == StOut) { ELOG(3); rslt(0,EcCScheme); return *this; }    //Ошибка, только вх., сп.вх., сп.вых.
      SlotId id = idOf(ci[i]);
      if (id == SlotId::out) { ELOG(4); rslt(0,EcCScheme); return *this; } //Ошибка, слот не найден
      cids.append(id.as(ct[i]));
    }
    //Сформировать список отображенных слотов
    SlotIds rids;
    for (int32_t i=0; i<ri.count(); ++i){
      SlotId id = r.idOf(ri[i]);
      if (id == SlotId::out) { ELOG(6); rslt(0,EcCScheme); return *this; } //Ошибка, слот не найден
      rids.append(id);
    }
    return push(Slot(SlotId::conv(BtNbc,pos),meta,cids,rids,bf,BsUntrained,&r));
  }



  Scheme& Scheme::push(const Slot &s){
    if (_posOut>=0) return *this;  //Запретить дальнейшее изменение схемы
    Slot *s2 = _factory.create(s.id());
    if (!s2) { ELOG(1); rslt(0,EcOutOfMemory);  return *this; }
    if (!s2->copy(s)) { ELOG(2); rslt(0,EcParam); return *this; }
    _slots.push_back(s2); //Добавить в схему
    s2->scheme(this);    //Обновить S-ссылку
    return *this;
  }



  Scheme& Scheme::out(SlotId id){
    if (_posOut>=0) return *this;  //Запретить дальнейшее изменение схемы
    _posOut = indexOf(id);
    return *this;
  }

  Scheme& Scheme::out(int32_t pos){
    if (_posOut>=0 || pos<0 || pos>=count()) return *this;  //Запретить дальнейшее изменение схемы
    _posOut = pos;
    return *this;
  }

  void Scheme::free(){ 
    for (int32_t i=0; i<_slots.size(); ++i)
      _factory.free(_slots[i]);
    _slots.clear();
    _posOut=-1;
  }



  bool Scheme::copy(const Scheme &scheme){
    free();
    for (int32_t i=0; i<scheme._slots.count(); ++i)
      push(*(scheme._slots[i]));
    _posOut = scheme._posOut;
    return rslt().succeeded();
  }



  bool Scheme::validate() {
    rslt(Result::ok);
    if (_slots.count()==0) return true;
    //Проверить наличие выходного слота
    if (!at(_posOut)) { ELOG(1); return rslt(0,EcCScheme);}
    //Проверить отсутствие дублирования слотов
    for (int32_t i=0; i<_slots.count(); ++i)
      if (indexOf(_slots[i]->id(),i+1)>=0)
        { ELOG(2); return rslt(0,EcCScheme);}
    //Проверить корректность параметров слота в соответствии с его типом
    for (int32_t i=0; i<_slots.count(); ++i)
      if (!_slots.at(i)->validate())
        { ELOG(3); return false;}
    //Проверить отсутствие циклов использования параметров
    if (circled()){
      ELOG(4); return rslt(0,EcCScheme);
    }
    return true;
  }

  bool Scheme::rvalidate() {
    rslt(Result::ok);
    //Проверить корректность параметров слота в соответствии с его типом
    for (int32_t i=0; i<_slots.count(); ++i)
      if (!_slots.at(i)->rvalidate())
        { ELOG(1); return false; }
    return true;
  }

  int32_t Scheme::sizeOfScheme() const{
    if (!count()) return 0;
    if (_posOut<0) return 0;
    int32_t d = 0;
    //Определить размер первого слота
    Slot *s = _slots.at(_posOut);
    d = s->sizeSlot(true);
    //Определить размер всех остальных слотов
    for (int32_t i=0; i<_slots.count(); ++i)
      d += _slots.at(i)->sizeSlot(false);
    return d;
  }
  


  int32_t Scheme::writeScheme(QDataStream &to, bool withSize) const {
    if (!count()) { ELOG(1); return -1;}
    if (_posOut<0) { ELOG(2); return -1; }
    int32_t w=0, wi=0;
    if (withSize) {
      int32_t sz = sizeOfScheme();
      if (-1 == to.writeRawData((char*)&sz,sizeof(int32_t))) { ELOG(3); return -1;}
    }
    if (-1 == (wi = at(_posOut)->writeSlot(to,true))) { ELOG(4); return -1;}  //Записать заголовок выходного слота
    w += wi;
    for (int32_t i=0; i<_slots.count(); ++i){ //Записать все слоты схемы
      if (-1 == (wi = _slots[i]->writeSlot(to,false))) { ELOG(5); return -1;}
      w += wi;
    }
    return w;
  }



  int32_t Scheme::readScheme(QDataStream &from, int32_t remain){
    rslt(Result::ok);
    int32_t r=remain;
    int32_t ri=0;
    SlotId id, id0=SlotId::out;
    Meta meta0;
    Slot *s = Null;
    free();
    if ((uint32_t)r<sizeof(SlotId)) { ELOG(1); goto err;}
    do {
      from.readRawData((char*)&id,sizeof(SlotId));
      if (from.status() != QDataStream::Ok) { ELOG(2); goto err;}
      r -= sizeof(SlotId);
      if (!(s = _factory.create(id))) goto err;                                   //Создать слот
      if (-1 == (ri = s->readSlot(id,from,r,id0 == SlotId::out))) { ELOG(3); goto err;} //Считать параметры
      r -= ri;
      //Установить схему для слота
      //примечание: вложенные схемы устанавливаются извне
      if (id0 == SlotId::out){ //фиктивный 0-й слот схемы
        id0 = s->id();
        if (id0 == SlotId::out) { ELOG(4); rslt(0,EcCScheme); return -1;} //Автоопределение последнего слота не поддерживается
        meta0 = s->meta();
        _factory.free(s);
      } else {
        _slots.push_back(s);
        if (!s->scheme(this)) { ELOG(5); return -1;}
      }
    } while (r > 0);
    if (r < 0) { ELOG(6); goto err; } //Проверить полноту чтения схемы
    _posOut = indexOf(id0);   //Установить номер выходного слота и проверить достоверность id0,meta0
    if (_posOut == -1) { ELOG(7); goto err;}
    if (at(_posOut)->meta() != meta0) { ELOG(8); goto err;}
    return remain;
  err:
    free();
    rslt(0,EcIo);
    return -1;
  }


  bool Scheme::readScheme(Data &from)
  {
      QByteArray buf;
      QDataStream stream(&buf, QIODevice::ReadWrite);
      from.write(stream, true);
      stream.device()->seek(0);
      this->readScheme (stream, from.size());
      return true;
  }

  bool Scheme::writeScheme(Data &to)
  {
      QByteArray buf;
      QDataStream stream(&buf, QIODevice::ReadWrite);
      this->writeScheme (stream, true);
      stream.device()->seek(0);
      to.read(stream, true);
      return true;
  }


  bool Scheme::circled() const {
    //Алгоритм:
    //у всех преобразователей для связанных вх. и сп.вх. параметров
    //проверить, что:
    //  а. позиция объявления слота раньше позиции использующего его слота
    //  б. для сп.вых.: он используется в качестве сп.вых. раньше текущего использования

    for (int32_t i1=0; i1<count(); ++i1){
      Slot *c = _slots.at(i1);
      if (c->id().type() != StConvertor) continue;  //Проверять только для преобразователей
      SlotIds cc; c->cids(&cc,&cc,Null,Null);   //Получить все его вх. и сп.вх. параметры
      for (int32_t j=0; j<cc.count(); ++j){
        int32_t i2= indexOf(cc.at(j));
        if (i2<0) { ELOG(1); return true; }         //Ошибка, слот не найден
        if (i2>=i1) return true;                    //Цикл, т.к. слот используется раньше объявления
                                                    //а для вых. - раньше вычисления
        SlotId sid = _slots[i2]->id();              //Абсолютный номер слота
        if (sid.type() == StSpecOut){
          Slots ss; used(ss,sid,true);              //Найти первое использование
          if (!ss.count()) return true;
          int32_t i3 = indexOf(ss.at(0)->id());
          if (i3>=i1) return true;           //Цикл, т.к. сп.вых. параметр использован раньше вычисления его значения
        }
      }
    }
    return false;
  }




  // Быстрая сортировка массива по возрастанию
  // A    - [in/out]	массив элементов, который нужно упорядочить
  // P		- [in]		  массив указателей, который связан с А и сортируется вместе с ним, т.е. сортируем только А, а P меняем так же
  // nelms- [in]		  количество элементов
  void Scheme::mathQSort(int32_t A[], void** P, int32_t nelms){
    int32_t i = 0;				    //увеличиваемый индекс
    int32_t j = nelms-1;		  //уменьшаемый индекс
    float x = A[nelms >> 1];	//средний элемент (по значению)

    do {
      while (A[i] < x) {i++;}
      while (x < A[j]) {j--;}
      if (i <= j) {
        mathSwap(A[i],A[j]);
        mathSwap(P[i],P[j]);
        i++; j--;
      }
    } while (i <= j);

    if (0 < j)			mathQSort(&A[0], &P[0], j + 1);
    if (i < nelms-1)	mathQSort(&A[i], &P[i], nelms - i);
  }

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug Scheme::debug(QDebug  dbg) const {
    QDebug d = dbg.nospace();
    d<<"Scheme("<<this<<" count:"<<_slots.count()<<" out:"<<_posOut<<" rslt:"<<_rslt<<"\n";
    return d<<_slots<<")";
  }

  QDebug operator<<(QDebug  dbg, const Scheme &x){
    return x.debug(dbg);
  }

#endif

}
