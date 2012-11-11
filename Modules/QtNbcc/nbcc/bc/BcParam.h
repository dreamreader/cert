#ifndef BC_PARAM_H
#define BC_PARAM_H

#include "BcSlot.h"

namespace Bc {
  using namespace Nb;

  //................................................................................................
  //Слот параметра
  //примечание:
  //  основные функции доступа к данным реализованы в BcSlot
  //  основные функции проверки корректности реализованы в Slot
  //  в классе реализуется поддержка перенаправления запросов на ссылаемые слоты
  //  и индикация грубых отклонений
  //
  //
  //РЕАЛИЗОВАТЬ:
  //  индикацию грубых отклонений
  //  обновление состояния связанных преобразователей после обучения вложенных слотов
  //................................................................................................
  class BcParam : public BcSlot {
  public:
    virtual Matrix::Row constraint(ConstraintType ct) const;
    virtual bool clear();
    virtual bool clear(ConstraintType ct);
    virtual bool set(ConstraintType ct, const Matrix::Row &value);
    virtual bool get(ConstraintType ct, Matrix::Row &value) const;
    virtual int32_t actual(ConstraintType ct) const;

    virtual bool reset();
    virtual bool train(TrainMode mode, BlockState state, BlockFlags flags);
    virtual bool emulate(ConstraintType cti=CtAny, ConstraintType cto=CtAny, bool recur=false);
    virtual bool qeval();
    virtual bool test();

    virtual int32_t writeParams(Container &c, const Uuid *ct=Null, const BlockType *bt=Null) const;
    virtual int32_t readParams(const Container &c, int32_t pos, const Uuid *ct=Null);

    //virtual bool validate() const;
    //virtual bool rvalidate() const;
    //virtual bool update();
    //virtual bool rupdate();
    //virtual void sync();
  protected:
    //Установка кода ошибки собственной схемы в соответствии с кодом ошибки связанной схемы
    bool rrslt() const ;
  };

}//end namespace Bc

#endif // BC_PARAM_H
