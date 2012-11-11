#ifndef BC_SCHEME_H
#define BC_SCHEME_H

#include "../nb/nbc.h"
#include "../nb/Scheme.h"
#include "BcSlot.h"

namespace Bc {

  //...............................................................................
  //Схема преобразования
  //...............................................................................
  class BcScheme: public Scheme {
  public:
    BcScheme():Scheme(BcScheme::defaultFactory()), _info(_infof){}

    class BcFactory: public Scheme::Factory {
    public:
      virtual Slot* create(SlotId id) const;  //Создание слота по его типу
      virtual void  free(Slot* s) const;      //Удаление слота
    };

    static Factory& defaultFactory();

    //Освобождение схемы
    virtual void free();

    //Очищение параметров
    bool clear(ConstraintType ct);
    bool clear(const SlotIds &ids, ConstraintType ct);
    //Установка параметров в слоты
    //  skipEmpty - пустые параметры не устанавливаются, иначе сбрасываются соответствующие ограничения
    //возвращает: false, если хотя бы 1 параметр не может быть установлен
    bool set(SlotId id,  ConstraintType ct, const Matrix::Row &value, bool skipEmpty=true);
    bool set(const SlotIds &ids, ConstraintType ct, const Matrix::Rows &values, bool skipEmpty=true);
    //Чтение параметров (без перевыделения памяти)
    //возвращает: false, если хотя бы 1 параметр не может быть считан
    bool get(SlotId id, ConstraintType ct, Matrix::Row &any);
    bool get(const SlotIds &ids, ConstraintType ct, Matrix::Rows &any);
    //Число примеров в слоте
    int32_t actual(SlotId id, ConstraintType ct);

    //Сброс состояния элементво схемы
    bool reset(SecureState stt = SsAll);

    //Эмуляция с целью получения значений слотов
    bool emulate(SlotId, ConstraintType ct);
    bool emulate(const SlotIds &ids, ConstraintType ct);
    //Обучение
    bool train();
    //Тестирование
    bool test(Matrix::Row &tr);
    //Оценка качества входных примеров для установленных входных параметров
    //формат вектора [nown]R32[1] = 0..1 и больше
    bool qeval(Matrix::Row &ge);

    //Запись в контейнер (возвращает число записанных блоков)
    //возвращает: число записанных блоков или -1
    int32_t writeParams(Container &c, const Uuid *ct=Null) const;

    //Чтение контейнера
    //возвращает: число считанных блоков или -1
    int32_t readParams(const Container &c, int32_t pos);

    //Чтение описания слота (и связанных с ним)
    bool descript(SlotId id, QDataStream &to, bool recur=true);

    //Проверка корректности схемы (связность элементов и непротиворечивость номеров)
    virtual bool validate();

    //Синхронизация состояния схемы по результатам обучения или сброса состояния
    virtual void sync();

    enum SchemeFlagsEnum {
      SfTrained     = 0x0100, //все преобразователи обучены
      SfPartially   = 0x0200, //преобразователи обучены частично
      SfConv1       = 0x0400, //хотя бы 1 простой преобразователь обучен
      SfValidated   = 0x0800  //схема проверена
    };
    typedef packed_flags<SchemeFlagsEnum,uint16_t> SchemeFlags;

    inline SchemeFlags state() const { return _sf; }

  protected:
    SchemeFlags         _sf;
    Container::Factory  _infof;         //фабрика классов для контейнера
    Container           _info;          //набор информационных блоков загруженного контейнера (размещаются в конце блока)
    static const Uuid   _containerType; //поддерживаемый тип контейнера
  };

}


#endif // BC_SCHEME_H
