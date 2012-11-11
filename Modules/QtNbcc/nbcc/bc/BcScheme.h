#ifndef BC_SCHEME_H
#define BC_SCHEME_H

#include "../nb/nbc.h"
#include "../nb/Scheme.h"
#include "BcSlot.h"

namespace Bc {

  //...............................................................................
  //����� ��������������
  //...............................................................................
  class BcScheme: public Scheme {
  public:
    BcScheme():Scheme(BcScheme::defaultFactory()), _info(_infof){}

    class BcFactory: public Scheme::Factory {
    public:
      virtual Slot* create(SlotId id) const;  //�������� ����� �� ��� ����
      virtual void  free(Slot* s) const;      //�������� �����
    };

    static Factory& defaultFactory();

    //������������ �����
    virtual void free();

    //�������� ����������
    bool clear(ConstraintType ct);
    bool clear(const SlotIds &ids, ConstraintType ct);
    //��������� ���������� � �����
    //  skipEmpty - ������ ��������� �� ���������������, ����� ������������ ��������������� �����������
    //����������: false, ���� ���� �� 1 �������� �� ����� ���� ����������
    bool set(SlotId id,  ConstraintType ct, const Matrix::Row &value, bool skipEmpty=true);
    bool set(const SlotIds &ids, ConstraintType ct, const Matrix::Rows &values, bool skipEmpty=true);
    //������ ���������� (��� ������������� ������)
    //����������: false, ���� ���� �� 1 �������� �� ����� ���� ������
    bool get(SlotId id, ConstraintType ct, Matrix::Row &any);
    bool get(const SlotIds &ids, ConstraintType ct, Matrix::Rows &any);
    //����� �������� � �����
    int32_t actual(SlotId id, ConstraintType ct);

    //����� ��������� ��������� �����
    bool reset(SecureState stt = SsAll);

    //�������� � ����� ��������� �������� ������
    bool emulate(SlotId, ConstraintType ct);
    bool emulate(const SlotIds &ids, ConstraintType ct);
    //��������
    bool train();
    //������������
    bool test(Matrix::Row &tr);
    //������ �������� ������� �������� ��� ������������� ������� ����������
    //������ ������� [nown]R32[1] = 0..1 � ������
    bool qeval(Matrix::Row &ge);

    //������ � ��������� (���������� ����� ���������� ������)
    //����������: ����� ���������� ������ ��� -1
    int32_t writeParams(Container &c, const Uuid *ct=Null) const;

    //������ ����������
    //����������: ����� ��������� ������ ��� -1
    int32_t readParams(const Container &c, int32_t pos);

    //������ �������� ����� (� ��������� � ���)
    bool descript(SlotId id, QDataStream &to, bool recur=true);

    //�������� ������������ ����� (��������� ��������� � ������������������ �������)
    virtual bool validate();

    //������������� ��������� ����� �� ����������� �������� ��� ������ ���������
    virtual void sync();

    enum SchemeFlagsEnum {
      SfTrained     = 0x0100, //��� ��������������� �������
      SfPartially   = 0x0200, //��������������� ������� ��������
      SfConv1       = 0x0400, //���� �� 1 ������� ��������������� ������
      SfValidated   = 0x0800  //����� ���������
    };
    typedef packed_flags<SchemeFlagsEnum,uint16_t> SchemeFlags;

    inline SchemeFlags state() const { return _sf; }

  protected:
    SchemeFlags         _sf;
    Container::Factory  _infof;         //������� ������� ��� ����������
    Container           _info;          //����� �������������� ������ ������������ ���������� (����������� � ����� �����)
    static const Uuid   _containerType; //�������������� ��� ����������
  };

}


#endif // BC_SCHEME_H
