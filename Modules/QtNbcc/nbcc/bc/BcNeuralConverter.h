#ifndef BC_CONVERTER_H
#define BC_CONVERTER_H

#include "BcSlot.h"
#include "NeuralMath.h"

namespace Bc {
  using namespace Nb;

  //..................................................................................
  //Слот нейросетевого преобразователя
  //примечание:
  //  используется однослойная нейронная сеть с произвольным числом нейронов
  //  формат входных параметров Meta(x,R32,C)
  //  формат выходного параметра Meta(y,I1,DO), где y кратен 8
  //..................................................................................
  class BcNeuralConverter: public BcSlot {
  public:

    //virtual Matrix::Row constraint(ConstraintType ct) const;
    //virtual bool clear();
    //virtual bool clear(ConstraintType ct);
    //virtual bool set(ConstraintType ct, const Matrix::Row &value);
    //virtual bool get(ConstraintType ct, Matrix::Row &value) const;
    //virtual int32_t actual(ConstraintType ct) const;
    virtual bool reset();
    virtual bool train(TrainMode mode, BlockState state, BlockFlags flags);
    virtual bool emulate(ConstraintType cti=CtAny, ConstraintType cto=CtAny, bool recur=false);
    //virtual bool qeval();
    virtual bool test();

    virtual int32_t writeParams(Container &c, const Uuid *ct=Null, const BlockType *bt=Null) const;
    virtual int32_t readParams(const Container &c, int32_t pos, const Uuid *ct=Null);

    virtual bool validate() const;
    //virtual bool rvalidate() const;
    //virtual bool update();
    //virtual bool rupdate();
    //virtual void sync();

  private:
    Layer _layer;
  };


}


#endif // BC_NEURAL_CONVERTER_H
