#ifndef BC_CONDITION_H
#define BC_CONDITION_H

#include "BcSlot.h"

namespace Bc {
  using namespace Nb;

  //..................................................................................
  //���� ��������� ���������������
  //����������:
  //  ��������� ��������� ���. ��������� � ���� �� ��. ���������� �� �������
  //  1. ������� ��. 2. ������ 3. �������� ����������
  //..................................................................................
  class BcCondition: public BcSlot {
  public:
    BcCondition();

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
    virtual bool update();
    //virtual bool rupdate();
    //virtual void sync();

  private:

    enum Variant {
      VarUnknown,
      VarExistence, //����� �� �������
      VarIndex,     //����� �� �������
      VarIndicator  //����� �� �������� ����������
    } _var; //������� ����������

  };


}


#endif // BC_CONDITIONAL_CONVERTER_H
