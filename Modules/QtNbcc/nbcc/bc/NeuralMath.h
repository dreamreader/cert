#ifndef NEURAL_MATH_H
#define NEURAL_MATH_H

#include <QtCore/QVector>
#include "../nb/nbc.h"

namespace Bc {
  using namespace Nb;


  struct Range {
    Range():left(0.0f),right(0.0f){}
    Range(float l, float r):left(l),right(r){}

    float left;  //����� (����������� �������)
    float right; //������ ������� (������������)

    bool in(float x) const;
    bool inl(float x) const;
    bool inr(float x) const;
    bool inlr(float x) const;

    bool valid() const;
    bool  intersect(const Range &range) const;  //�������� ����������� ���� ����������
    static Range intersect(const Range &a, const Range &b);//����������� ����������. ������������ ����������� valid()
    float dist() const;
    float dist(float x) const;  //���������� �� ����� ��� ��������� (��� 0.0f)
    //bool unite(range,range) - ����������� ������ �������� ��� false, ���� �� ����� ����� ����� ��� �����������
    //swap()
    //bool exclude(range) - ���������� ����� ����� (���� ����������� 2 ������������, ����� ��������
    //
  };

  inline bool Range::in(float x) const { return (left>x && x<right); }
  inline bool Range::inl(float x) const { return (left>=x && x<right); }
  inline bool Range::inr(float x) const { return (left>x && x<=right); }
  inline bool Range::inlr(float x) const { return (left>=x && x<=right); }
  inline float Range::dist() const { return right - left; }
  inline float Range::dist(float x) const {
    if (x <=left) return left - x;
    else if (x>=right) return x - right;
    return 0.0f;
  }
  inline bool Range::valid() const { return left>=right; }

  //-----------------------------------------------------------------------------------
  //������
  //����������: �������� ������������� ����� � �������� ������������� ����
  //-----------------------------------------------------------------------------------
  struct Neuron {
    float b;                //�������� (������������ ������������)
    QVector<float> weights; //������ �����
    QVector<int32_t> conns; //������ ������

    uint8_t bpc;  //��� �� ����� + ���� �����������

    Neuron():b(0.0f),bpc(16){}
    int32_t nconns() const;

    void clear(){
      b=0.0f;
      weights.clear();
      conns.clear();
    }

    bool isEmpty() const;
    bool isTrained() const;

    //���������� ������ (� �� �����) �� �����������
    //����������: ����� ����� ������ ���� ����� ����� ������ ��� 0
    bool csort();
    //����������� ������� ����� (� �� ������) � �����
    //����������: ��� ���� ������������������ ������� ������ ����������� ��� ��������� ���������
    void zeroback(bool andTrim);
    //����� ��������� ��������� � ����� ������
    int32_t wnotzeros() const;

    //�������� ����������� �����
    //  bitPerW - ��������������� ��������� ��� �� ���
    //  originOfCoord - � �������������� �������� ���� "0" ��� ��� ����
    //        ���� true,  �� ������������ ������� (.., {-2}, -1, 0, 1, {2}, ..) n=2^bitPerW-1
    //        ���� false, �� ������������ ������� (.., -2, -1, 1, 2, ..) n=2^bitPerW
    //����������: ���� ������� w=0 ��� originOfCoord=false,
    //            �� �� ������ ������������ � ��� ������ ���� ������� � ������� �����������
    //            (��� ����� �������� � ��������� � �����, �������� ����� ��� �� 1 � �� originOfCoord=true)
    //����������: ����� �������
    bool digitize(uint8_t bitPerW = 32, bool originOfCoord=true);

    //����������� ��������� � ����������� ������ ��� �� ����� (�� 2 �� 16) ��� �������� ��������� ���������
    //  bitPerC   - ����������� ��������� ��� �� ����� (��������������� � ���������� ��������� �������� ��� �� ��������)
    //����������:
    //  -1 � ������ ������������� ���������
    //  0, ���� ��������� ����� ���������
    //  x ���, ���� ����� �� ������ ��������� � bitPerC
    int32_t cencoding(uint8_t &bitPerC, bool distance, int32_t wmax);

    //����������� ������ � �����
    bool cencode(QDataStream &to, int32_t nc, uint8_t bitPerC, bool distance, int32_t wmax) const;
    //������������� ������ �� ������
    bool cdecode(QDataStream &from, int32_t nc, uint8_t bitPerC, bool distance, int32_t wmax);

    //����������� ����� � �����
    //����������: �������������� ������ ���� ��������� �������� digitize
    bool wencode(QDataStream &to, int32_t nw, uint8_t bitPerW, bool originOfCoord) const;
    //������������� ������ �� ������
    bool wdecode(QDataStream &from, int32_t nw, uint8_t bitPerW, bool originOfCoord);


    #if !defined(QT_NO_DEBUG_STREAM)
    friend QDebug operator<<(QDebug  dbg, const Neuron &x);
    #endif
  };

  inline int32_t Neuron::nconns() const { return conns.count(); }
  inline bool Neuron::isEmpty() const { return conns.isEmpty(); }
  inline bool Neuron::isTrained() const { return !weights.isEmpty(); }

  typedef QVector<Neuron> Neurons;

  //-----------------------------------------------------------------------------------
  //���� ������������� ��������� ���� ��������������� ���������-���
  //-----------------------------------------------------------------------------------
  class Layer {
  public:
    enum LayerFlagsEnum {
      LfCOwn  = 0x01, //����������� ����� ������ � ��������
      LfWOwn  = 0x02, //����������� ����� ����� � �������� (���� LfWOwn=false, ������������ ����� �������)
      //0x03
      LfBInc  = 0x04, //�������� ������� �������� (����� ���������� ��������, �.�. 4*416 ���� ��� 0, �.�. �������� ���������)
      LfMdInc = 0x08, //�������� ������� �������� "��� �����" (����� ���������� ��������, �.�. 4*416 ����)
      //        � ������� ��������� "��� �����" (����� ���������� ��������, �.�. 4*416 ����)
      LfProt  = 0x80  //�������� ���������� �� ��������� ������ ����
    };
    typedef packed_flags<LayerFlagsEnum,uint8_t>    LayerFlags;

    //������ ������������� ������ (��������)
    enum ConnDistribAlgEnum {
      CdaRandom=0,        //��������� ������� (���������� ����� ������) Uuid::generateUuid
      CdaFixTable=1,      //������������� ������� (���� ��� ����) Uuid::nil
      CdaFixAlg=2,        //������������� �������� �������������, �������� � ������� Uuid
      CdaFixByQual=3,     //����������� ������������� �� ��. �������� (� ���������� "����")
      CdaFixByCorr=4,     //����������� ������������� �� �������� � ���������� "��� �����" � ������������� ������
      CdaDynByQual=5,     //������������ �� ���. �������� � ��. ���������� (� ����������� �����������)
      CdaDynByCorr=6      //������������ �� ���. �������� � ��. ���������� (� ����������� �����������)
    };
    typedef packed_enum<ConnDistribAlgEnum,uint8_t>    ConnDistribAlg;

    enum WeightTrainAlgEnum {
      WtaFullUsing = 0,   //������ ������������� ������
      WtaMinAndSuff = 1   //������������� ������ �����, �������������� ����������� ���������
    };
    typedef packed_enum<WeightTrainAlgEnum,uint8_t>   WeightTrainAlg;


    #if !defined(QT_NO_DEBUG_STREAM)
    friend QDebug operator<<(QDebug  dbg, const ConnDistribAlg &x);
    friend QDebug operator<<(QDebug  dbg, const WeightTrainAlg &x);
    friend QDebug operator<<(QDebug  dbg, const LayerFlags &x);
    #endif

    Layer();

    void clear();

    //������� ������� ����������
    bool isEmpty(){ return _n.isEmpty(); }
    bool isTrained(){ return !_n.isEmpty() && _n.at(0).isTrained(); }

    //������� ���������� ������� �����
    // nin  - ����� ������
    // nout - ����� ��������
    // nconn- ����� ������ �������
    // lf   - ����� ����
    // cda  - ����� ��������� ������
    // bpc  - ��� �� ����� (0x00 - ����� ������������ ��� ���� ��� �������������)
    // fdc  - ����������� ������������
    // bpw  - ��� �� ��� (2..32)
    // fooc - ������������� "0" ��� �����������
    // id   - ����� ��������� ��������� ������ (��� ������ FixAlg)
    bool config(const Uuid &id, int32_t nin, int32_t nout, int32_t nconn=24, LayerFlags lf=0x00,  ConnDistribAlg cda=CdaFixTable,
                      uint8_t bpc=16, bool fdc=false,
                      uint8_t bpw=32, bool fooc=false);

    //����������� �������� ����� �������� ���� � ������������ � ��������� �������
    bool optimize();

    //�������� ���� �� ����������� ���������� ��� ������ �������
    //  own - ������� "����", n ~ 8-32, Meta(a,MfR32,MtC)
    //  all - ������� "��� �����", m ~ 100, Meta(a,MfR32,MtC)
    //  out - ������ ��������� ����, 1, Meta(b,MfI1,MtDO), b<=a
    //bool train(const Matrix::Row &own, const Matrix::Row &all, const Matrix::Row &out, WeightTrainAlg wta);
    bool train(const Matrix::Row &own, const Matrix::Row &all, const Matrix::Row &out, WeightTrainAlg wta);


    //�������� ���� �� ����������� ���������� ���������� ��������
    //����������:
    //  1.�������� ����������� ���������� 1 �������, �� ����������� ����, ��� ������������ �������� ��������� ������ �� ��������
    //    ��� ���������� � ����� � �������������� ��������� �� ����, � � ����� ������ ������������ ������� ���������
    //  2.�� ������ ����� ����� ������ ������������ (�������� � ������� &-���������) ����� ��.�������
    //    � ��������� ���������� ����������
    //  3.��� ���������� �������� ��� i-� �����, ��������� � k-� ��.��������, out �������������� � �������� ������������� ��������,
    //    �.�. � ���������� ���������� ��� �����, ���������� � ������� !=k ��������� ��� ���� ���������� ��������.
    //  4.
    //bool train(const Matrix::Rows &owns, const Matrix::Rows &alls, const Matrix::Row &out, WeightTrainAlg wta);


    //���������� ������� ������������ ������ ������� ���� �� ���� "����"
    bool testP1(const Matrix::Row &own, const Matrix::Row &out, float &p1);

    //���������� ������� ����������� ������ ������� ���� �� ���������������� ���� "��� �����" �� ������ ������������� ��� ��������
    bool testP2(const Matrix::Row &all, const Matrix::Row &out, float &p2);

    //������ ���������� �������������� � ������� ����������� ���������� ��������� ����
    //  out - ������ �����, 1, Meta(b,MfI1,MtDO)
    bool protect(const Matrix::Row &out);

    //�������� ���� ��� ����������� ����������
    //  in - ������� "�����", n, Meta(a,MfR32,MtC)
    //  out - ������ ��������� ����, n, Meta(b,MfI1,MtDO)
    bool sim(const Matrix::Row &in, Matrix::Row &out, bool defended);

    //������ ���������� ���� �� ������
    //  pub - �������� ��������� (��� ���������, ���� pri �� �����)
    //  pri - �������� ��������� (�������� � ������������)
    //����������: � �������� pri ����� ������� ��� �� �����
    bool read(QDataStream &pub, QDataStream &pri);
    //������ ���������� ���� � �����
    //  pub - �������� ��������� (��� ���������, ���� pri �� �����)
    //  pri - �������� ��������� (�������� � ������������)
    bool write(QDataStream &pub, QDataStream &pri) const;

  #if !defined(QT_NO_DEBUG_STREAM)
  friend QDebug operator<<(QDebug  dbg, const Layer &x);
  #endif

  protected:
    //��������� � ������� ������� ������ � ������������ � ������������� ����������
    //imax  - ������������ ����� ������
    //cmax  - ����� ������ � ������ �������
    //salt  - �����, ������������ ��� ��������� ������������� ����������
    bool cgenerate(int32_t imax, int32_t cmax, const Uuid& salt);
    //���������� ������������� ������� ��������, ������� � ������, ��������������� �����
    bool cline(int32_t imax, int32_t cmax);

  protected:
    Neurons         _n;     //������� nout=_n.count()

    LayerFlags      _lf;    //����� (�������������� ����������� ��� ����������� �� �������)
    ConnDistribAlg  _cda;   //����� ��������� ������

    int32_t         _nin;   //����� ������
    int32_t         _nconn; //����� ������

    Uuid            _salt;  //�������������

    uint8_t _bpc;  //��� �� ����� + ���� �����������
    uint8_t _bpw;  //��� �� ��� + ���� �����������
  };


  //-----------------------------------------------------------------------------------
  //�������������� �������
  //-----------------------------------------------------------------------------------

  //��������� �������
  //��� [-oo; 0] ��������� ������� ������ 0
  //��� (0; +oo] ��������� ������� ������ 1
  //  sig       - [��/���] ������� ��������� ����
  //  nelms     - [��] ����� ��������
  void hardlimR32(Vbp &sig, int32_t nelms);

  //��������� ������� � ����� �����������
  //��� [-oo; 0] ��������� ������� ������ -1
  //��� (0; +oo] ��������� ������� ������ 1
  //  o       - [��/���] ������� ��������� ����
  //  nelms   - [��] ����� ��������
  void sign2R32(Vbp &sig, int32_t nelms);

  //���������� ��������� �������� �������
  float qualR32(const Vbp &sig, int32_t nelms);

  //���������� ������������ i-�� ������ ������� �� ���� ���������� [<=0, >0]
  //� �������������� ����� ������ (-1 ��� 1). ���� 0, �� -1.
  void stab2stateColR32(const Vbp *inp, int32_t ncols, int32_t nelms, Vbp &stab, Vbp &sgn);

  //�������� ��������� � �������� ��������
  // ��������:
  //    - ��� ������� ���������� �������������� ����������� � ���������
  //			 �������� � ��������� ��������
  //		- ���� �������� �� �����, �� �������� � ���������� �� -3����� �� 3�����.
  // sig    - [��] �������� �������� �������, ������ ����� ���������
  // ncols  - [��] ����� ��������
  // interv - [��] �������� �������� (������ �� ���� �����: ���., ����. �������� ������ �������
  int32_t hittestR32(const Vbp &sig, int32_t nelms);

  int32_t sigtestR32(const Vbp &sig, int32_t nelms, float bit);

  //���������� ����� ������ ������� (��� ��������� �������)
  //  n       - [��] ������
  //  inp     - [��] ����� ��������
  //  ncols   - [��] ����� ��������
  //  sig     - [���]������ �������� � ������ ��������� ncols
  bool mulsumR32(const Neuron &nr, const Vbp *inp, int32_t ncols, Vbp &sig);


  //������������� ������ ���� ��� �� ���������� (n,R32,C) � ���������� (m,R32,C)
  //����������:
  //  - ������������ ��������� ������� ��������� sign2R32 [-1,1].
  //  l     - [��] ���� ��������� ����
  //  ncols - [��] ����� ��������
  //  nelms - [��] ����� ��������� �� ������� �������
  //  inp   - [��] ����� �������� ������� ��������
  //  sig   - [���]����� �������� �������� �������� ����
  //  defended - [��] ������������ ������������� �� ���������� �����
  bool simR32(const Neurons &l, int32_t ncols, const Vbp *inp, Vbp *sig, bool defended);


  //������������� ������ ���� ��� �� ���������� (n,R32,C) � ���������� (m,I1,DC)
  //  l     - [��] ���� ��������� ����
  //  ncols - [��] ����� ��������
  //  nelms - [��] ����� ��������� �� ������� �������
  //  inp   - [��] ����� �������� ������� ��������
  //  sig   - [���]����� �������� �������� �������� ����
  //  defended - [��] ������������ ������������� �� ���������� �����
  bool simR32I1(const Neurons &l, int32_t ncols, const Vbp *inp, Vbp *sig, bool defended);

  //��������� ������� ������ �������� ����������� ����������
  //���������:
  //	l  - [��/���] ����
  //  nelms  - [��] ����� ��������� �� ������� ������� ����. ���������� (������� �������������)
  bool generateConnR32Continuous(Neurons &l, uint32_t nelms);

  //��������� ������� ������ �������� ���������� ����������
  //	l  - [��/���] ����
  //  nelms  - [��] ����� ��������� �� ������� ������� ����. ���������� (������� �������������)
  bool generateConnR32Discrete(Neurons &l, uint32_t nelms);

  //��������� ��������� ���� (������ ������ ���� ��������)
  bool generateR32Sig(Vbp &sig, uint32_t nelms);

  //���������� �������� ������ ��������� �������� (��� ����������� ���)
  //  ����� ������ ������� ����� ���������� ���������� ������� � ������������,
  //  �.�. �������� "������ �������"
  //  � �������������� ������� ������������� coefs ������ ����������� ��������� �������:
  //  ��� ����� ����� � ��������, ��� ����� �� � ������, �.�. ������� ���� ������ ��
  //  ������ ������� ����, � ��������� - � ���� ������� ����.
  //
  //  own      - [��] ����� �������� ���������� "����", n
  //  nown     - [��] ����� �������� ���������� "����"
  //  nelms    - [��] ����� ��������� � ����� ������� ����������
  //  better   - [���] ����� ���������� �� "�������" ������� ���������� "����"
  //              (������ ������ ���� �������� ������ ��� ������ ����������)
  //  nbetter  - [���] ����� ���������� "�������" ��������
  //  ge       - [���] n �������� �� ������ ��������, � ������ ������� (�� 0 �� 1 �������, ������ 1 ������)
  bool indicateGeR32(  const Vbp *own, uint32_t nown,  uint32_t nelms,
                              Vbp *better, uint32_t &nbetter, Vbp *ge = nbNULL);


  //�������� ������� ���� ��� (����� ������ ������� ������� = 5% �� ����� ������)
  //����������:
  //  - �������� ����������� � ��������� ������� �����, � ������� ������ � ������
  //    ��� ��� ������ ���������� �������
  //  - ������������� ����� ������������ �������� �������� �� �������� �������������� �����������
  //    ����������
  //	l       - [��/���] ����
  //  own     - [��] ��������� ������� ������������� "����"
  //  nown    - [��] ����� ��������� �������� "����"
  //  all     - [��] ��������� ������� ������������� "��� �����"
  //  nall    - [��] ����� ��������� �������� "��� �����"
  //  nelms   - [��] ����� ��������� � ������ ������� �������
  //  sig     - [��] ������ ��������� ���� (����� ��������� ����� ����� �������� ���������� ����)
  bool trainR32Continuous(  Neurons &l,
                            const Vbp *own, uint32_t nown,
                            const Vbp *all, uint32_t nall,
                            uint32_t nelms,
                            const Vbp &sig);

  //�������� ������� ���� ��� (����� ������ ������� ������� = 50% �� ����� ������)
  //����������:
  //  - �������� ����������� � ��������� ������� �����, � ������� ������ � ������
  //    ��� ��� ������ ���������� �������
  //  - �� ������ ������� �������� ����������� ������� �������� �������� � ����� �����������
  //	l       - [��/���] ����
  //  own     - [��] ��������� ������� ������������� "����"
  //  nown    - [��] ����� ��������� �������� "����"
  //  all     - [��] ��������� ������� ������������� "��� �����"
  //  nall    - [��] ����� ��������� �������� "��� �����"
  //  nelms   - [��] ����� ��������� � ������ ������� �������
  //  sig     - [��] ������ ��������� ���� (����� ��������� ����� ����� �������� ���������� ����)
  bool trainR32Discrete(   Neurons &l,
                           const Vbp *own, uint32_t nown,
                           const Vbp *all, uint32_t nall,
                           uint32_t nelms,
                           const Vbp &sig);

  //������� ���� ��� � ���������� �����
  //  ����������� ��������� ����� ������� ������� �� ������ ����������� ������� (����)
  bool trainR32Protected( Neurons &l, const Vbp &sig);


}



#endif // BC_MATH_NEURAL_H
