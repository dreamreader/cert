#define _USE_MATH_DEFINES
#include <math.h>
#include "rndmt.h"
#include "VbpMath.h"
#include "NeuralMath.h"
#include "BcBitStream.h"

namespace Bc {
  using namespace Nb;

  //------------------------------------------------------------------------------------------
  //������� �������� ���
  //------------------------------------------------------------------------------------------

  //���������� ����� ������ ������� (��� ��������� �������)
  //  n       - [��] ������
  //  inp     - [��] ����� ��������
  //  ncols   - [��] ����� ��������
  //  sig     - [���]������ �������� � ������ ��������� ncols
  bool mulsumR32(const Neuron &nr, const Vbp *inp, int32_t ncols, Vbp &sig){
    //��� ������� ������
    for (int32_t i=0; i<ncols; ++i){
      //��������������� ������ ������� �� ������� �������
      float o1 = nr.b;                                             //�������� �������� ���������
      for (int32_t j=0; j<nr.nconns(); ++j){
        o1 += inp[i].r32[nr.conns[j]]*nr.weights[j];
      }
      sig.r32[i] = o1;
    }
    return true;
  }

  //���������� ����� ������ ������� ��� index-����
  bool mulsumN1R32(const Neuron &nr, int32_t index, const Vbp *inp, int32_t ncols, Vbp &sig){
    //��� ������� ������
    for (int32_t i=0; i<ncols; ++i){
      //��������������� ������ ������� �� ������� �������
      float o1 = nr.b;                                             //�������� �������� ���������
      for (int32_t j=0; j<index; ++j){
        o1 += inp[i].r32[nr.conns[j]]*nr.weights[j];
      }
      for (int32_t j=index+1; j<nr.nconns(); ++j){
        o1 += inp[i].r32[nr.conns[j]]*nr.weights[j];
      }
      sig.r32[i] = o1;
    }
    return true;
  }

  //��������� �������
  //��� [-oo; 0] ��������� ������� ������ 0
  //��� (0; +oo] ��������� ������� ������ 1
  //  sig       - [��/���] ������� ��������� ����
  //  nelms     - [��] ����� ��������
  void hardlimR32(Vbp &sig, int32_t nelms){
    for (int32_t i=0; i<nelms; ++i){
      sig.r32[i] = (sig.r32[i]>0.0f)? 1.0f : 0.0f;
    }
  }

  //��������� ������� � ����� �����������
  //��� [-oo; 0] ��������� ������� ������ -1
  //��� (0; +oo] ��������� ������� ������ 1
  //  sig     - [��/���] ������� ��������� ����
  //  nelms   - [��] ����� ��������
  void sign2R32(Vbp &sig, int32_t nelms){
    for (int32_t i=0; i<nelms; ++i){
      sig.r32[i] = (sig.r32[i]>0.0f)? 1.0f : -1.0f;
    }
  }

  //���������� ��������� �������� �������
  float qualR32(const Vbp &sig, int32_t nelms){
    float a = averR32(sig,nelms);
    float d = dispR32(sig,nelms,a);
    return qualR32(a,d);
  }

  float qualI32(const Vbp &sig, int32_t nelms){
    float a = averI32(sig,nelms);
    float d = dispI32(sig,nelms,a);
    return qualI32(a,d);
  }

  //���������� ������������ i-�� ������ ������� �� ���� ���������� [<=0, >0]
  //� �������������� ����� ������ (-1 ��� 1). ���� 0, �� -1.
  //�������� stab � ������ ������ 0, � ������ 1
  void stab2stateColR32(const Vbp *inp, int32_t ncols, int32_t nelms, Vbp &stab, Vbp &sgn){
    for (int32_t i=0; i<nelms; ++i){
      float p0 = 0.0f;
      for (int32_t j=0; j<ncols; ++j)
        if (inp[j].r32[i]<0.0f) p0 += 1;
      p0 /= ncols;
      if (p0>=0.5) sgn.r32[i]=-1.0; else sgn.r32[i]=1.0f;
      if (p0 == 0.0f || p0 == 1.0f)
        //�������� �������� ����� ��������� ���������
        stab.r32[i] = 0.0f;
      else
        //�������� �������� ��� �������� ���������
        stab.r32[i] = (float)fabs(p0*log(p0)/M_LN2 + (1.0-p0)*log(1.0-p0)/M_LN2);
    }
  }

  //�������� ��������� � �������� ��������
  // ��������:
  //    - ��� ������� ���������� �������������� ����������� � ���������
  //			 �������� � ��������� ��������
  //		- ���� �������� �� �����, �� �������� � ���������� �� -3����� �� 3�����.
  // sig    - [��] �������� �������� �������, ������ ����� ���������
  // ncols  - [��] ����� ��������
  // interv - [��] �������� �������� (������ �� ���� �����: ���., ����. �������� ������ �������
  int32_t hittestR32(const Vbp &sig, int32_t nelms){
    int32_t hit = 0;
    float left, right;
    float a,d,q;
    adqR32(sig,nelms,a,d,q);
    left  = a - 3.0f*d + 0.01f;//a - 3.0f*d + 0.01f;
    right = a + 3.0f*d - 0.01f;//a + 3.0f*d - 0.01f;
    //��������� ��� ���� �������� ��������
    for ( int32_t i=0; i<nelms; ++i){
      if (/*(sign2(left)==sign2(right)) && */(sig.r32[i] > left) && (sig.r32[i] < right))
        ++hit;
    }
    return hit;
  }

  //���������� ����� ���������� ������
  int32_t sigtestR32(const Vbp &sig, int32_t nelms, float bit){
    int32_t hit=0;
    for ( int32_t i=0; i<nelms; ++i){
      if (sign2(sig.r32[i]) == sign2(bit)) ++hit;
    }
    return hit;
  }

  //������������� ������ ���� ������������ ���� ��������� ���� �� ���������� �������.
  //����������:
  //  - ������������ ��������� ������� ��������� sign2R32 [-1,1].
  //  l     - [��] ���� ��������� ����
  //  ncols - [��] ����� ��������
  //  inp   - [��] ����� �������� ������� ��������
  //  sig   - [���]����� �������� �������� �������� ����
  //  defended - [��] ������������ ������������� �� ���������� �����
  bool simR32(const Neurons &l, int32_t ncols, const Vbp *inp, Vbp *sig, bool defended){

    Matrix mtxSigCol(2,Meta(ncols,MfR32,MtContinuous));
    Vbp sig1 = mtxSigCol.at(0,0); //  ������� ������ ������� �� ���� ��������
    Vbp sig0 = mtxSigCol.at(0,1); //  ���������� ������� ������ �������

    //�������� ���������� ��� �������� ���� �����
    //���������� ��������� ������� � 1
    for (int32_t i=0; i<ncols; ++i) sig0.r32[i] = 1.0f;
    //��� ������� �������
    for (int32_t i=0; i<l.size(); ++i){
      //��������� �����
      mulsumR32(l[i],inp,ncols,sig1);
      //������������ ��������� �������
      sign2R32(sig1,ncols);
      //����������� ���������, ���� ���� ����������� �������
      if (defended){
        for (int32_t j=0; j<ncols; ++j)
          sig[j].r32[i] = sig1.r32[j] = sig1.r32[j] * sig0.r32[j];
        //�������� ������� �������
        Vbp tmp = sig1; sig1 = sig0; sig0 = tmp;
      } else {
        for (int32_t j=0; j<ncols; ++j)
          sig[j].r32[i] = sig1.r32[j];
      }
    }
    return true;
  }

  bool simR32I1(const Neurons &l, int32_t ncols, const Vbp *inp, Vbp *sig, bool defended){

    Matrix mtxSigCol(2,Meta(ncols,MfR32,MtContinuous));
    Vbp sig1 = mtxSigCol.at(0,0); //  ������� ������ ������� �� ���� ��������
    Vbp sig0 = mtxSigCol.at(0,1); //  ���������� ������� ������ �������

    //�������� ���������� ��� �������� ���� �����
    //���������� ��������� ������� � 1
    for (int32_t i=0; i<ncols; ++i) sig0.r32[i] = 1.0f;
    //��� ������� �������
    for (int32_t i=0; i<l.size(); ++i){
      //��������� �����
      mulsumR32(l[i],inp,ncols,sig1);
      //������������ ��������� �������
      sign2R32(sig1,ncols);
      //����������� ���������, ���� ���� ����������� �������
      if (defended){
        for (int32_t j=0; j<ncols; ++j){
          sig1.r32[j] *= sig0.r32[j];
          sig[j].i1(i,sig1.r32[j]>0.0f);
        }
        //�������� ������� ������ �������� (����� �� ����������)
        Vbp tmp = sig1; sig1 = sig0; sig0 = tmp;
      } else {
        for (int32_t j=0; j<ncols; ++j)
          sig[j].i1(i,sig1.r32[j]>0.0f);
      }
    }
    return true;
  }

  //------------------------------------------------------------------------------------------
  //��������� ������ ����������
  //------------------------------------------------------------------------------------------

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
                              Vbp *better, uint32_t &nbetter, Vbp *ge){

    if (nown<2) return false;

    //1. ��������� ������ �������� ��-������� ��� ���� �������
    //1.1-1.2 ���������� �������������� �������� � ��������� ������� ��������
    Matrix mAdq(3,Meta(nelms,MfR32,MtContinuous));
    Vbp &a = mAdq.at(0,0);
    Vbp &d = mAdq.at(0,1);
    Vbp &q = mAdq.at(0,2);
    adqColR32(own,nown,nelms,a,d,q);

    //1.3 ��������� ��-������� �� �������
    Matrix mChi2(1,Meta(nown,MfR32,MtContinuous));
    Vbp &chi2 = mChi2.at(0,0);  //������ �������� ��-�������
    for (uint32_t i=0; i<nown; ++i){
      chi2.r32[i] = 0.0f;
      for (uint32_t j=0; j<nelms; ++j){
        float x;
        if (fabs(d.r32[j])<0.00001f)
          if (fabs(a.r32[j]-own[i].r32[j])<0.00001) x=0.0f; //������ ����������
          else                                      x=32.f; //������� ����������
        else
          x = (a.r32[j]-own[i].r32[j])/d.r32[j];          //�� �������
        chi2.r32[i] += x*x;
      }
      chi2.r32[i] /= nelms;
    }

    //���� ����� ������ ge, �������� ����������� �������� ��� ������� ������
    if (ge){
      for (uint32_t i=0; i<nown; ++i){
        ge[i].r32[0] = chi2.r32[i];
      }
    }

    //2. ������������� ������
    //��������� better
    memcpy(better,own,nown*sizeof(Vbp));
    //2.1 ������������� ��������� �� ������� � ������������ �� ���������� chi2
    mathQSort(chi2.r32,(void**)better,nown);
    //2.2 ���������� ������ �������� �������
    uint32_t iHalf = (uint32_t)ceill((float)nown/2);
    //2.3 ��������� ���������� ����� ������ � ��������� ���������� � ��������������� �������� ��-2
    float chi2Dist = chi2.r32[iHalf] - chi2.r32[0];
    //2.4 ���� ������ ������ ��������, ����� ������ ������� (�� ��������� ����� � ������ �������)
    float chi2Max = chi2.r32[iHalf+1] + chi2Dist;
    //2.5 ��������� ��� �������, �� ��������� ��-������� ������ chi2Max
    nbetter = iHalf+1;
    while ((nbetter < nown) && (chi2.r32[nbetter] < chi2Max)){
      ++nbetter;
    }
    //������ ������� ���������� (������� 0..nbetter-1)

    //����������������� ge, ����� ������� ������ �������� � 0..1
    if (ge){
      for (uint32_t i=0; i<nown; ++i){
        ge[i].r32[0] /= chi2Max;
      }
    }
    return true;
  }

  //������ ��������, ������ �����������, ������ ��������
  struct Qes {
    float q;    //��������
    int32_t e;  //������ �����������
    int32_t s;  //������ ��������
  };


  bool  Range::intersect(const Range &range) const {
    return intersect(*this,range).valid();
  }

  //��������� ����������� ����������
  Range Range::intersect(const Range &a, const Range &b){
    Range c;
    if (a.left>b.right)     c.left = max(a.right,b.right);//��� �����������
    else if (a.left>=b.left)c.left = a.left;
    else                    c.left = b.left;
    if (a.right<b.left)         c.right = min(a.left,b.left);//��� �����������
    else if (a.right<=b.right)  c.right = a.right;
    else                        c.right = b.right;
    return c;
  }


//------------------------------------------------------------------------------------------------
//����� ��������� �������� �������
//------------------------------------------------------------------------------------------------

  //����������:
  //  ��� ������������ (float) ����� ����� ���� �����
  //  y0 = y0 + x; y1 = y0 - x; (y0 != y1) true
  //  ��-�� ������ ����������, ������� ����������� �� �������� ��������� �� ������ ��������

  //���������� ��������� � ����������� ������ ������ sigtest
  //  nr  - ������������� ������
  //  iw  - ����� ����������� ����
  //  own, nown, bit - ������� "����", �� �����, ��������� ���
  //  left  - ����� �������
  //  right - ������ �������
  //����������: ����������� ����� ������ � �������� ���������
  int32_t trainFindSHit(Neuron &nr, int32_t index, const Vbp *own, int32_t nown, float bit, Range &range){
    Matrix mOut0(1,Meta(nown,MfR32,MtContinuous)); Vbp &out0 = mOut0.at(0,0);  //������ �������� �������
    Matrix mOut(1,Meta(nown,MfR32,MtContinuous)); Vbp &out = mOut.at(0,0);  //������ �������� �������
    Matrix mK(1,Meta(nown+2,MfR32,MtContinuous)); Vbp &k = mK.at(0,0);      //������ ����� �������� (+2 �������)
    Matrix mErr(1,Meta(nown+2,MfI32,MtContinuous)); Vbp &err = mErr.at(0,0);//������ ������������� � �������� � ������ ���������

    //����� ������������� ����� ����� �������� ��� ������� �������, ������ w[i] �� 0
    mulsumN1R32(nr,index,own,nown,out0);
    for (int32_t i=0; i<nown; ++i)  //��� ������� ������� ����� �����, � ������� ���������� ����� �����
      k.r32[i+1] = - out0.r32[i]/own[i].r32[nr.conns[index]];

    //������������� �� �� ����������� � �������� ��������� (����� ����� ������ weiD)
    mathQSort(k.r32+1,nown);
    k.r32[0] = k.r32[1] - 1000.0f;
    k.r32[nown+1] = k.r32[nown] + 1000.0f;

    //� ������ �� nown+1 ��������� ����� ����������� ����� �����, � ������ � ����� ������
    //-oo--------<k0>-----<k1>-----0-----<kn>------ +oo
    //��������� ����� ������ � ������ ���������, ������ ����������
    int32_t  ehit=nown+1;
    for (int32_t i=0; i<nown+1;++i){
      float wk = k.r32[i]/2.0f + k.r32[i+1]/2.0f;  //������� �������� ���� ������ i-�� ���������
      for (int32_t j=0; j<nown;++j){
        out.r32[j] = out0.r32[j] + wk*own[j].r32[nr.conns[index]];
      }

      err.i32[i] =  nown - sigtestR32(out,nown,bit);
      if (err.i32[i] <= ehit) ehit = err.i32[i];
    }

    //������������ ����� ��������, ����� ����������� �������� ���������� � ���������� �������
    int32_t imin;
    int32_t imax;
    for (imin=0; imin<nown+1; ++imin) if (err.i32[imin]==ehit) break;
    for (imax=imin+1; imax<nown+1; ++imax) if (err.i32[imax]!=ehit) break;

    range.left  = (imin==0)?k.r32[imin]-1000.0f:k.r32[imin];
    range.right = (imax==(nown+1))?k.r32[imax-1+1]+1000.0f:k.r32[imax-1+1];

    return ehit;
  }


  //������������ �������� � �������� ���������
  //����������:
  //  ���������������� ������������, ��� � �������� ��������� (left, right) � ����� � ��� �� sigtest
  //  �������� hittest ����� �������� ��������� ���. ������� ���� �� �������� ������� hittest,
  //  ������������ ��������� ��������� ����� � �������� ��������� (�������� �������)
  int32_t trainMinimizeEHit(RndMT &g, Neuron &nr, int32_t index, const Vbp *own, int32_t nown, Range range, float &optW){
    Matrix mOut0(1,Meta(nown,MfR32,MtContinuous)); Vbp &out0 = mOut0.at(0,0);  //������ �������� �������
    Matrix mOut(1,Meta(nown,MfR32,MtContinuous)); Vbp &out = mOut.at(0,0);  //������ �������� �������

    mulsumN1R32(nr,index,own,nown,out0);

    int32_t ehit=nown+1; //������������ ����� ������ (����������� ���)
    optW=range.left;

    //���������� ��������� ��������, �������������� ������ hit
    int32_t i = (range.left != range.right)?256:1;
    for (;i>0;){//��������� �������� ��������, �� ������� ����� � ������ �������
      float wk = g.rand(range.left,range.right);
      if (wk == range.left) { i--; continue; } //�������� ������ ������������
      //����� �������� out � ������ index-�� ����
      for (int32_t j=0; j<nown;++j){
        out.r32[j] = out0.r32[j] + wk*own[j].r32[nr.conns[index]];
      }

      //��������� ������
      int32_t ecur = nown - hittestR32(out,nown);
      if (ecur < ehit){
        ehit = ecur;
        optW  = wk;
        if (optW>0.0f) range.right = optW; else range.left = optW; //�������� ������� ��������� � ������� ���������� � 0 �������� (wei ������ ������ left,right)
        i = i >> 1;
        //if (ecur == 0) break; //����������� �������� �������
      } else if (ecur == ehit && (fabs(optW)>fabs(wk))){  //������ �������� - ���. ������ ����
        ehit = ecur;
        optW  = wk;
        if (optW>0.0f) range.right = optW; else range.left = optW;
        i = i >> 1; //������� ����� ��������� ������� ������� � 2 ����
      } else {
        i--;
      }
    }
    return ehit;
  }



  //���������� ��������� �������� ��� n ������ ������
  Range calcWRange(const Vbp &out, uint32_t nout){
    float a = averR32(out,nout);
    float d = dispR32(out,nout,a);
    a = fabs(a);
    return Range(-a-d-1.0f,a+d+1.0f);
  }

  //���������� ����������� ��������� ��������, ������ �������������, ������ ��������
  Qes calcWQes(const Vbp &out, int32_t nout, float bit){
    Qes qes;
    qes.q = qualR32(out,nout);
    qes.e = nout - hittestR32(out,nout);
    qes.s = nout - sigtestR32(out,nout,bit);
    return qes;
  }



  //������ ������������ ��-�� ������ ���������� mulsumR32 �������� ������ ��������� ��� ������ �������� ����
  //inline void updateOutByW(Neuron &nr, int32_t index, const Vbp *own, uint32_t nown, Vbp &out, float curW, float predW){
  //  for (int j=0; j<nown; ++j)                      //����������� �������� ��������� �� ���� ��������
  //    out.r32[j] += own[j].r32[nr.conns[index]]*(curW-predW);
  //}

  //��������� �������� (��� ����� ����� � Q � ������� �������) � ������� � ��������� ���������
  //  n - ����� ����������� �����
  //  index - ����� �������������� �������
  //  own - �������
  //  nown - ����� ��������
  //  out - ������������ � ������� mulsum ������� ��� n ������� ����� �� nown ��������
  //  range - ��������, � ������� ����������� ��������
  //  targetQ  - ��������� ��������
  //  pred - ������� ���������� �������� � ������
  //  optW - (���) ����������� �������� ����
  //  opt - ���������� ���������� �������� � ������
  //����������: ������� ��������� �������� ��������
  bool optimizeWByOutQualRand(RndMT &g, Neuron &nr, int32_t index, const Vbp *own, int32_t nown, float bit,  Range range, float targetQ, const Qes &pred, float &optW, Qes &opt, Vbp &out0, Vbp &out){
    //���������� ����� ��� index-����
    mulsumN1R32(nr,index,own,nown,out0);

    bool changed = false;
    opt  = pred;
    optW = nr.weights[index];

    for (int32_t i=5; i>0; --i){//����� ������� ������� = 5 (���� �������)
      float wk = g.rand(range.left,range.right);    //������������� ���
      //����� �������� out � ������ index-�� ����
      for (int32_t i=0; i<nown;++i){
        out.r32[i] = out0.r32[i] + wk*own[i].r32[nr.conns[index]];
      }

      Qes cur = calcWQes(out,nown,bit);                                        //������� ���������� �������� ��������

      if ((cur.e+cur.s) <= (pred.e+pred.s)){ //��������� ���������� ������
        if (opt.q<= targetQ){ //�������� ����� ����������� � ����� ��������� (��� ��������� q ������������ �������� ����� �������� � 0 ��������)
          if ((cur.q > opt.q) || ((cur.q == opt.q) && (fabs(wk)<fabs(optW)))){
            opt = cur;
            optW = wk;
            changed = true;
          }
        } else {  //�������� ����� �������, �� �� ������ targetQ (��� ��������� q ������������ �������� ����� �������� � 0 ��������)
          if (cur.q > targetQ){
            if ((cur.q < opt.q) || ((cur.q == opt.q) && (fabs(wk)<fabs(optW)))){
              opt = cur;
              optW = wk;
              changed = true;
            }
          }
        }
      }
    }
    return changed;
  }


  //����������� ����� ������ ������������� � ��������
  //  opt   - [���] �������� �������� � ������
  //  optW  - [���] �������� ����
  //  index - [���] ����� ����
  //����������: ������ ���������� �������������� ��������
  void optimizeWByEs(RndMT &g, Neuron &nr, int32_t n, const Vbp *own, int32_t nown, float bit, const Range &goodRng, float &optW, int32_t &index, Qes &opt){
    opt.s = nown; opt.e = nown;
    optW  = 0.0f;
    index = -1;
    int32_t i=0;
    for (; i<n; i++){
      Qes   cur; cur.q = opt.q;
      Range range;
      float curW;
      //����� ����������� ������ ��������
      cur.s = trainFindSHit(nr,i,own,nown,bit,range);
      //����������, ������ ���� ����� ������ �� �����������
      if (cur.s > opt.s) continue;
      cur.e = trainMinimizeEHit(g,nr,i,own,nown,range,curW);
      //����������, ������ ���� ����� ������ ����������� � ������� �������� ������
      if (index>=0){
        if (cur.s == opt.s){
            if (cur.e > opt.e) continue;
            if ((cur.e == opt.e) && (fabs(curW) < fabs(optW))) continue;
        }
      }
      opt   = cur;
      optW  = curW;
      index = i;
      if (goodRng.in(curW) && !opt.s && !opt.e) break;  //�������� �����, ���� ������ ������� � ��������� goodRange
    }
  }


  //��������� ����� �� ��������� ���� � �������� ��������
  //  k - ����� ������ ��������������� ����� (��������� ����������)
  //  a - ��. ���. ��������
  //  q - ��. ��������
  //  bit - ��������� ������ �������
  void setWByInQual(Neuron &nr, int32_t k, const Vbp &a, const Vbp &q, float bit){
    int32_t n = nr.nconns();
    nr.b = 0.0f;
    for (int32_t i=0; i<k; ++i){
      float sgn  = sign2(a.r32[nr.conns[i]]*bit);
      nr.weights[i] = sgn*q.r32[nr.conns[i]];
    }
    for (int32_t i=k; i<n; ++i){
      nr.weights[i]=0.0f;
    }
  }


  //�������� �������� ������� 1, � �������� ����� ��������
  //���������: ������� ��������� � ������ �������� ��� �������� �������, �� � P2 �������
  //��� ������ ���������� ��������������� ��������, � ���� �� ���� � ��.���.�������� sign(a*bit)
  //�.�. �������� � �������� ����������� ������� ��� ������������������ ��������.
  //(�������� �������� ����� �������� ������, �� ����� ����� ������� �������� � ���������� �������)

  //�������� �������� ������� 2
  //����������: ������ ��������� ��� ��������� ��������, �� �������� � 50% ��� ������ �������� �������
  //(��� ����� ����������, � ���. ������ �������� � �������������, � ������������ ��������)
  //
  //��������� �� ����� � �������� "����" ��� �������
  //�� ���� ���������� �������� �������� ��������� ����� = +-(|aver|+disp+1.0f)
  //������� ��������� �� k �������� �� n (1 ������ �� �����������, � ��� 2 � ����� ����������� �� ������� �������)
  //  ������������ ������ �� ��������, ������ ��� ������� ��� �� ����� ��������, �� ���������� ������ e-hit � s-hit
  //    (��������� �������� � k-�� ������� � ��������� � ������, ���� �� ������� ��������� �� �������� - ��������� ��������)
  //  ������������ ������ � ��������� ������� �������� �� ���� �������������
  //    (���� �� ������� ��������� � ��������, ��������� ����� ��������)
  //�������� �������������� ������� � ����� ������
  //
  //!���� �� ������� ��������� ���������:
  //  �.����������� �������� �� n ������ ��� ����������� �� ��������
  //  �.������������ ������ ������� ����� ����������
  //
  Qes trainQFastOwn2(RndMT &g, bool lazy, Neuron &nr, const Vbp *own, int32_t nown, const Vbp &a, const Vbp &q, float bit, Vbp &out0, Vbp &out){
    int32_t maxN = nr.nconns();
    Qes best = {-1.0f,nown+1,nown+1};            //��������� ������ ��������
    //���� �������� �������, �� �������� ��������� � 1 �������
    //���� ������, �� ����� ����������� ��� �������, �.�. i=n-1
    int32_t n = (lazy)?1:maxN;
    for (;n<=maxN;++n){  //���� ������� ��������� �� �������� ������ n ��������
      //��������� �� ��������
      setWByInQual(nr,n,a,q,bit);

      //������������� �� ������������� ����������� �������� ����
      uint32_t imax;
      float m = amaxR32(Vbp(nr.weights.data()),nr.nconns(),imax);
      if (m!=0.0f)
      for (int32_t i=0; i<nr.nconns(); ++i)
        nr.weights[i] /= m;

      //���������� �������� � ������
      mulsumR32(nr,own,nown,out);
      Qes cur = calcWQes(out,nown,bit);
      Range goodRng = calcWRange(out,nown);
      best = cur;

      //��������� ����������� �������� � �������������� ��� �������� ����� ������
      bool stop = false;
      while (!stop){
        stop = true;
        for (int32_t index=1;index<n; ++index){//������ �� ������� ����
          float curW;
          Qes qes;
          if (optimizeWByOutQualRand(g,nr,index,own,nown,bit,goodRng,4.5f,cur,curW,qes, out0, out)){
            nr.weights[index] = curW;                                   //���������
            cur = qes;
            best = cur;
            if (!cur.s && !cur.e && (best.q<5.0f)) { stop = true; break; }
            stop = false;
          }
        }
      }

      if (cur.s || cur.e )
      for (int32_t i=0; i<n; ++i){ //���� ������ n-������� (����� ����������, ���� �� ����� ���������)
        float curW;
        int32_t index;
        optimizeWByEs(g,nr,n,own,nown,bit,goodRng,curW,index,cur);    //��������
        nr.weights[index] = curW;                                   //���������
        cur.q = qualR32(out,nown);
        best = cur;
        if (!cur.s && !cur.e) break;  //���� ��� ������ ���������, ����� �����
      }

      if (!cur.s && !cur.e ){
        if (cur.q>5.0f && n<maxN) continue; //����������� ������� ��������, �������� ����� ������
        if (cur.q>4.0f) break;              //�������� � ��������� - ����� � �������
        continue;  //����� - �������� ��������, �������� ����� ������
      }
    }
    return best;
  }

  /*Qes trainQFastOwn2(RndMT &g, bool minUsing, Neuron &nr, const Vbp *own, int32_t nown, const Vbp &a, const Vbp &q, float bit, Vbp &out0, Vbp &out){
    int32_t maxN = nr.nconns();
    Matrix mWBest(1,Meta(maxN,MfR32,MtC)); Vbp &wb = mWBest.at(0,0);  //������ ����, ��� ������� Qes(q>4.0, e=0, s=0)
    Qes best = {-1.0f,nown+1,nown+1};                                 //��������� ������ ��������

    //������ � ����� �������, ���������� �� ������, ���� �� ������� �������, ����� ��� ������� ���������
    //���� ���������� ������ �������, ������� �� ���.
    //����� ����������� ������� ����� l � r (l+r)/2, ������, ��� ��� r ������� �������.
    bool has1 = false;  //������� 1-� ������� (���� ��� ������ ���� ���������, ����������� ������������ �������)
    int32_t l=(minUsing)?1:maxN;
    int32_t r=(minUsing)?1:maxN;
    //����������� ������ �������, ���������� �����.
    //��������� ������ �������, �������� � �����.
    do {
      //������� ������� ����� ������������ ������
      if (!has1)  n = r;
      else        n = (r+l)/2;

      //.................................................

      //��������� �� ��������
      setWByInQual(nr,n,a,q,bit);

      //������������� �� ������������� ����������� �������� ����
      uint32_t imax;
      float m = amaxR32(Vbp(nr.weights.data()),nr.nconns(),imax);
      if (m!=0.0f)
      for (int32_t i=0; i<nr.nconns(); ++i)
        nr.weights[i] /= m;

      //���������� �������� � ������
      mulsumR32(nr,own,nown,out);
      Qes cur = calcWQes(out,nown,bit);
      Range goodRng = calcWRange(out,nown);

      //��������� ����������� �������� � �������������� ��� �������� ����� ������
      bool stop = false;
      while (!stop){
        stop = true;
        for (int32_t index=1;index<n; ++index){//������ �� ������� ����
          float curW;
          Qes qes;
          if (optimizeWByOutQualRand(g,nr,index,own,nown,bit,goodRng,4.5f,cur,curW,qes, out0, out)){
            nr.weights[index] = curW;                                   //���������
            cur = qes;
            if (!cur.s && !cur.e && (cur.q<5.0f)) { stop = true; break; }
            stop = false;
          }
        }
      }

      //��������� ��������� ������
      if (cur.s || cur.e )
      for (int32_t i=0; i<n; ++i){ //���� ������ n-������� (����� ����������, ���� �� ����� ���������)
        float curW;
        int32_t index;
        optimizeWByEs(g,nr,n,own,nown,bit,goodRng,curW,index,cur);    //��������
        nr.weights[index] = curW;                                   //���������
        cur.q = qualR32(out,nown);
        if (!cur.s && !cur.e) break;  //���� ��� ������ ���������, ����� �����
      }

      if (!cur.s && !cur.e ){
        if (cur.q>5.0f && n<maxN) continue; //����������� ������� ��������, �������� ����� ������
        if (cur.q>4.0f) break;              //�������� � ��������� - ����� � �������
        continue;  //����� - �������� ��������, �������� ����� ������
      }

      //..................................................

      if (!has1){
        //��������� �������: �������� (q � ���������, e=0, s=0), ������� (q �����, e=0, s=0), ������������������ (q �����, e=�����, s=����� ������ �� ���������)
        //
        if (!cur.s && !cur.e){
          if (cur.q>4.0f && cur.q<5.0f){  //�������� �������
            has1 = true;
            best = cur;
            //..��������� ���������� �����
          } else {//������� ������� (����� ���������� � best)
            //���� ����� ���������, ����� ���������, ����� ������� ��������, ��������� � 2 ����
          }
        } else {  //������������������ ������� (����� ���������� � best)

        }


        if (good){ //������������ ������� ������� n=r
          has1 = true;
          //..��������� ���������� �����
        } else {
          l = r;
          r = min(maxN,r*2); //��������� � ������� ����������
        }
      } else {
        if (good){  //���� �� ����������� ������� �������
          r = n;
          //..��������� ���������� �����
        } else {
          l = n;
        }
      }

    } while (l!=r);

    //������������, ���� ������� ������, ����� �������� ������� ���� (������ �� ��������� n=r)




    for (;n<=maxN;++n){  //���� ������� ��������� �� �������� ������ n ��������

    }
    return best;
  }*/

  //�������� (��������� ����� ��������) ���� �� �������� �������� � �������������� ������ ���������
  //  own   - ������� �������
  //  nown  - ����� ��������
  //  ncols - ����� ��������� � ����� �������
  //  out   - �������� ���
  void trainQualityR32(Neurons &l, const Vbp *own, uint32_t nown, uint32_t nelms, const Vbp &sig, bool lazy){
    //�������� ������ ��� ������� ��������
    Matrix mOut0(1,Meta(nown,MfR32,MtC)); Vbp &out0 = mOut0.at(0,0);  //������ �������� ������� ��� index-�� ����
    Matrix mOut(1,Meta(nown,MfR32,MtC)); Vbp &out = mOut.at(0,0);    //������ �������� �������

    //�������� ������ ��� a,d,q
    Matrix m(3,Meta(nelms,MfR32,MtContinuous));
    Vbp &a = m.at(0,0); //�������������� �������� "����"
    Vbp &d = m.at(0,1); //��������� "����"
    Vbp &q = m.at(0,2); //�������� "����"

    adqColR32(own,nown,nelms,a,d,q);

    //Uuid uid = (Uuid)Uuid::createUuid();
    //uint32_t *s = (uint32_t*)uid.data();
    //RndMT rg(s[0] ^ s[1] ^ s[2] ^ s[3]); //���������������� ��������� ��������� �����
    RndMT g(0);

    //��������� �������� ��� ������� �������
    for (int32_t i=0; i<l.size(); ++i){
      trainQFastOwn2(g,lazy,l[i],own,nown,a,q,sig.r32[i],out0,out);
    }
  }

  // ������������� ����� �������� ������ ������� ����
  // � ������ �������� ��������� ��� ��������� ���.��������
  // nl   - [in/out] ����, ���� �������� ������������
  // cor  - [in]     �������������� ������
  void modifyWeights(Neuron &nr, float cor){
    for (int32_t i = 0; i < nr.nconns(); ++i)
      nr.weights[i] /= fabs(cor);
  }

  //���������� �������� �������� �� ������ ��������� "��� �����"
  void modifyB(Neurons &l, const Vbp *all, uint32_t nall){
    Matrix msig1(1,Meta(nall,MfR32,MtContinuous)); //������� ������ �������
    Vbp &sig1 = msig1.at(0,0);
    //��������� ����� ��� ������� ������� ��� ������� �� ���� "��� �����"
    for (int32_t i=0; i<l.size(); ++i){
      //�������� ���������
      mulsumR32(l[i],all,nall,sig1);
      //��������� �������� ���������� ������������ ��������� "��� �����"
      l[i].b = averR32(sig1,nall);
    }
  }

  //�������� ���� � ������������ �������
  bool trainR32Continuous(  Neurons &l,
                          const Vbp *own, int32_t nown,
                          //const Vbp *tst, uint32_t ntst,
                          const Vbp *all, int32_t nall,
                          int32_t nelms,
                          const Vbp &sig, bool lazy){
    if (nown<2 || nall<2 || nelms<2) return false;

    //�������
    trainQualityR32(l,own,nown, nelms,sig,lazy);

    //��������� ������� �� ��������� � ��������
    Matrix msig1(1,Meta(nown,MfR32,MtC)); //������� ������ �������
    Vbp &sig1 = msig1.at(0,0);
    for (int32_t i=0; i<l.size(); ++i){
      //��������� ������������� �� ������� "����"
      mulsumR32(l[i],own,nown,sig1);
      //���������� ��������� � ��������� �������� ����� �� ��������� ��������
      modifyWeights(l[i],dispR32(sig1,nown,averR32(sig1,nown)));
      //�������� ��������� ������������� �� ������� "����"
      mulsumR32(l[i],own,nown,sig1);
      //���������� ���. �������� � ��������� �������� ����� �� ��������� ��������
      modifyWeights(l[i],averR32(sig1,nown));
    }
    //�������� �������� �������� ��������
    //modifyB(l,all,nall);
    return true;
  }


  //���������� ����������� ��������� ���� �� ����� (�������)
  //  � ����������� �� ��� ��������������� �����
  //  ��������� �������� ������� ������������� �������� ����
  //  l - ��������� ����
  //  sig - ����������� ������ ���� ��� "������"
  bool trainR32Protected(Neurons &l, const Vbp &sig){
    //�������� �������� ���� ����� ������� �������
    float cur = sig.r32[0];
    for (int32_t j=1; j<l.size(); ++j){
      for (int32_t iw=0; iw<l[j].nconns(); ++iw)
        l[j].weights[iw] *= cur;
      cur = sig.r32[j];
    }
    return true;
  }

  //------------------------------------------------------------------------------------------
  //������� ���� ��������� ����
  //------------------------------------------------------------------------------------------

  //�������� ����������� �����
  //  bitPerW - ��������������� ��������� ��� �� ���
  //  originOfCoord - � �������������� �������� ���� "0" ��� ��� ����
  //����������: ����� �������
  bool Neuron::digitize(uint8_t bitPerW, bool originOfCoord){
    if (bitPerW<1 || bitPerW>32) return false;
    if (bitPerW == 32) return true;
    int32_t nw = weights.count();
    if (!nw) return true;
    QVector<float> &w = weights;
    uint32_t nrange = 1<<(bitPerW-1); //����� ���������� � �������� ���������

    //����� ������������ wmax
    float wmax=0.0f;
    for (int i=0; i<nw; ++i){
      float wa = fabs(w[i]);
      if (wa>wmax) wmax = wa;
    }
    //����������� � |w|=1
    for (int i=0; i<nw; ++i)
      w[i] /= wmax;
    //��������� ���������� � ���������� ���������
    if (originOfCoord){ //� �����
      for (int i=0; i<nw; ++i) w[i] = roundNearest(w[i]*(nrange-1));
    } else {            //��� ����
      for (int i=0; i<nw; ++i) w[i] = sign(w[i])*ceil(fabs(w[i])*nrange);
    }
    return true;
  }

  //���������� ������ (� �� �����) �� �����������
  bool Neuron::csort(){
    int32_t nc = conns.count();
    int32_t nw = weights.count();
    if (nc == nw){
      if (nc == 0) return true;
      mathQSort(conns.data(),weights.data(),nc);
    } else {
      if (nw != 0) return false;
      mathQSort(conns.data(),nc);
    }
    return true;
  }

  //����������� ������� ����� (� �� ������) � �����
  void Neuron::zeroback(bool andTrim){
    if (conns.count() != weights.count()) return;
    if (!weights.count()) return;

    //���������� ������� ���� � �����
    int nw = weights.count();
    int i0; //����� ������� �������� ���� (�.�. ����� ��������� ���������)
    for (i0 = 0; i0<nw; ++i0) //����� ������ ������� ���
      if (weights[i0] == 0.0f) break;
    if (i0>nw) return;  //��� ���� ���������
    for (int ix=i0+1;ix<nw;++ix){
      if (weights[ix]!=0.0f) {  //������� ��� ���������
        mathSwap(weights[ix],weights[i0]);
        mathSwap(conns[ix],conns[i0]);
        i0++;
      } else {                  //������� ��� �������
        continue;
      }
    }
    //������ ��� ������������� ��
    if (andTrim){
      conns.resize(i0); // ��������: ����� ���������� ������, ����� ����������� ����. ����� ������, � ����� - ����������
      weights.resize(i0);
    } else {
      if ((conns.count()-i0) > 1){
        mathQSort(conns.data()+i0,weights.data()+i0,conns.count()-i0);
      }
    }
  }

  //�������� ������� ����� � ����� ������
  int32_t Neuron::wnotzeros() const{
    //���������� ����� ������� ����� � ����� ������
    int notz = weights.count()-1;
    for (;notz>=0;--notz){
      if (weights[notz]!=0.0f) break;
    }
    notz++;//����� ��������� ���������
    return notz;
  }

  int32_t Neuron::cencoding(uint8_t &bitPerC, bool distance, int32_t nmax){
    if (!conns.count()) return 0;
    //���������� ����������� ����� ��� ���������, ����� 0x00
    if (bitPerC<0 || bitPerC>16) return -1;
    if (bitPerC == 0){
      int32_t len=-1;
      for (int i=2;i<=16;++i){
        uint8_t bpc = i;
        int32_t cur = cencoding(bpc,distance, nmax);
        if (cur<len || len==-1){ len = cur; bitPerC = i; }
      }
      return len;
    } else {
      if (distance){
        int32_t range = (1<<bitPerC) - 1; //������ ���� ������ �� 2, �.�. [0..range-2, range-1 ������ ���]
        int32_t cost=0;
        int32_t pred=0;
        for (int32_t i=0; i<conns.count(); ++i){
          int32_t delta = conns[i] - pred; pred = conns[i];
          if (delta<0) delta += nmax;
          cost += (delta / range + 1)*bitPerC;//����������� "���������" ����������� ����� ����� � �����
        }
        return cost;
      } else {
        int32_t range = 1<<bitPerC;
        for (int32_t i=0; i<conns.count(); ++i)
          if (conns[i]>=range) return -1; //������ ���� ������ �� 1, �.�. ������� � [0..range-1]
        return conns.count()*bitPerC;
      }
    }
  }


  bool Neuron::cencode(QDataStream &to, int32_t nc, uint8_t bitPerC, bool distance, int32_t nmax) const {
    BitStream to2(to,BitStream::LittleEndian);
    if (nc > conns.count()) return false;     //������ ���������� ����� ������
    if (bitPerC<2 || bitPerC>16) return false;//������ ��������

    if (distance){
      int32_t range = (1<<bitPerC) - 1;
      int32_t pred = 0;
      for (int32_t i=0; i<nc; ++i){  //��� ���� �����
        int32_t delta = conns[i] - pred; pred = conns[i];
        if (delta<0) delta += nmax;
        for (int32_t j=delta/range; j>0; --j){
          if (!to2.writeBits(range,bitPerC)) return false; //������������ �������� ��� �������������
        }
        if (!to2.writeBits(delta % range,bitPerC)) return false; //������������ �������� ��� �������������
      }
    } else {
      for (int32_t i=0; i<nc; ++i){
        if (!to2.writeBits(conns[i],bitPerC)) return false;
      }
    }
    if (!to2.writeAlign()) return false;
    return true;
  }


  bool Neuron::cdecode(QDataStream &from, int32_t nc, uint8_t bitPerC, bool distance, int32_t nmax){
    BitStream from2(from,BitStream::LittleEndian);
    if (nc > conns.count()) return false;     //������ ���������� ����� ������
    if (bitPerC<2 || bitPerC>16) return false;//������ ��������

    if (distance){
      int32_t range = (1<<bitPerC) - 1;
      int32_t pred = 0;
      for (int32_t i=0; i<nc; ++i){  //��� ���� �����
        int32_t delta=0;
        int32_t x;
        do {
          if (!from2.readBits((uint32_t&)x,bitPerC)) return false;
          delta += x;
        } while (x == range); //����������: �������� ���. �������� ������ ������������
        conns[i]=pred+delta;  //!!!!������ ����� �� ������� ������������ ������������
        if (conns[i]>nmax) conns[i]-=nmax;
        pred = conns[i];
      }
    } else {
      for (int32_t i=0; i<nc; ++i){
        if (!from2.readBits((uint32_t&)conns[i],bitPerC)) return false;
      }
    }
    from2.readAlign();
    return true;
  }

  bool Neuron::wencode(QDataStream &to, int32_t nw, uint8_t bitPerW, bool originOfCoord) const {
    if (nw > conns.count() || nw > weights.count()) return false;
    if (bitPerW<1 || bitPerW>32) return false;
    //��� ������ 32 ������ �� ������������
    //����������: �������������� ������ ���� ��������� �������� digitize
    if (bitPerW==32){
      if (-1 == to.writeRawData((const char*)weights.data(),sizeof(float)*nw))
        return false;
      return true;
    }

    BitStream to2(to,BitStream::LittleEndian);
    if (originOfCoord){
      for (int32_t i=0; i<nw; ++i){
        int32_t x = weights[i];
        if (!to2.writeBits((uint32_t)x,bitPerW))
          return false;
      }
    } else {
      //����������: ����� �������� � ������ �����, ������ 1 � ���������� ������� ��� � ������������ � �������� ������
      //            ��� ���� ������� �������� ����� �� �����������
      for (int32_t i=0; i<nw; ++i){
        int32_t x = weights[i];
        if (x>0) x -= 1;//������������ ����������� �������� �������� (���� x=0, ��� �������� ����� �������� ��� �������������� �� 1)
        if (!to2.writeBits((uint32_t)x,bitPerW))
          return false;
      }
    }
    if (!to2.writeAlign()) return false;
    return true;
  }

  bool Neuron::wdecode(QDataStream &from, int32_t nw, uint8_t bitPerW, bool originOfCoord){
    if (nw > weights.count()) { ELOG(1); return false; }
    if (bitPerW<1 || bitPerW>32) { ELOG(2); return false;}
    //��� ������ 32 ������ �� ������������
    //����������: �������������� ������ ���� ��������� �������� digitize
    if (bitPerW==32){
      if (-1 == from.readRawData((char*)weights.data(),sizeof(float)*nw)){
        ELOG(3); return false;
      }
      return true;
    }

    BitStream from2(from,BitStream::LittleEndian);
    if (originOfCoord){
      for (int32_t i=0; i<nw; ++i){
        int32_t x;
        if (!from2.readBits((uint32_t&)x,bitPerW)) { ELOG(4); return false; }
        x <<= 32-bitPerW; x >>= 32-bitPerW; //������������ ������� ��������� ����
        weights[i] = x;
      }
    } else {
      //����������: ����� �������� � ������ �����, ������ 1 � ���������� ������� ��� � ������������ � �������� ������
      //            ��� ���� ������� �������� ����� �� �����������
      for (int32_t i=0; i<nw; ++i){
        int32_t x;
        if (!from2.readBits((uint32_t&)x,bitPerW)){
          ELOG(5); return false;
        }
        x <<= 32-bitPerW; x >>= 32-bitPerW; //������������ ������� ��������� ����
        //���������� �� �������� ����, ������������� ��� ������������� �����
        if (!(x & (1<<(bitPerW-1))))
          x +=1;
        weights[i] = x;
      }
    }
    from2.readAlign();
    return true;
  }

  Layer::Layer(){
    _lf = 0x00;
    _cda= CdaRandom;
    _bpc= 16;
    _bpw= 32;
    _nin=0;
    _nconn = 0;
    _salt = Uuid::nil;
  }

  void Layer::clear(){
    _lf = 0x00;
    _cda= CdaRandom;
    _bpc= 16;
    _bpw= 32;
    _nin=0;
    _nconn = 0;
    _salt = Uuid::nil;
    _n.clear();
  }

  //���������������� ����
  bool Layer::config(const Uuid &id, int32_t nin, int32_t nout, int32_t nconn, LayerFlags lf,  ConnDistribAlg cda,
                      uint8_t bpc, bool fdc,
                      uint8_t bpw, bool fooc){
    //�������� ���� � ������� �������
    clear();
    _n.resize(nout);
    _nin = nin;
    _nconn = nconn;
    _lf = lf;
    _cda  = cda;
    _bpc = bpc | ((uint8_t)fdc<<7);
    _bpw = bpw | ((uint8_t)fooc<<7);
    //������������� �����
    switch (cda){
    case CdaRandom:   //����������: ��� ������������ � ������ Random ������������ ������������ ��������� ��������,
                      //�� ����������� ��� ������� ������

                      //_salt = id; cgenerate(nin,nconn,_salt); break;
                      _salt = (Uuid)Uuid::createUuid(); cgenerate(nin,nconn,_salt); break;//��������
    case CdaFixTable: _salt = Uuid::nil; cgenerate(nin,nconn,_salt);  break; //�� ������������� �������
    case CdaFixAlg:   _salt = id; cgenerate(nin,nconn,_salt); break; //�� �������������� ���������
    default:          return false;//�� ��������������
    }
    return true;
  }


  //����������� �������� ����� �������� ���� � ������������ � ��������� �������
  bool Layer::optimize(){
    //���������� ����� ������ ����� (������ ������ ��� ���� ��������)
    if (!(_bpw & 0x7F)) _bpw = 32;
    //���������������� ����, ���� ����������
    for (int i=0; i<_n.count(); ++i){
      if (!_n[i].digitize(_bpw & 0x7F,(_bpw & 0x80) !=0)) return false;
    }

    //���� ����� ����������� � ����������� ������ � ������ Random
    if (_cda == CdaRandom){
      //������������� ����� �� �������
      for (int i=0; i<_n.count(); ++i){
        if (!_n[i].csort()) return false;
        _n[i].zeroback((_lf & LfCOwn)!=0);//������� ����� ������, ���� ����� ������ �������������
      }
      //��������� ������� ����� � ����� ������, ���� ���� ��� ����� �������������
      //����������: ����� ������� ������, ���� ����� �������������
      //            ����� ������� �� ����� ������, ���� ���� �������������

      //������ ����� ���������� ����������� ������ �����������, ���� �� �� ����� ����
      if (0==(_bpc &0x7F)){ //�������������� ������ ����������� ������ �� �����
        if (_lf & LfCOwn){  //���� ��� ������� �������
          for (int i=0; i<_n.count(); ++i){
            uint8_t b=0;
            if (-1 == _n[i].cencoding(b,(_bpc & 0x80) != 0,_nin)) return false;
            _n[i].bpc = b | (_bpc & 0x80);
          }
        } else {  //����� ��� ���� ��������
          uint8_t bmax=0;
          //��������� ��� ���� � ������� ����� ��� ������������ ����� �����������,
          //����� ����� ������ ������ ��� ���� �����������
          for (int i=0; i<_n.count(); ++i){
            uint8_t b=0;
            if (-1 == _n[i].cencoding(b,(_bpc & 0x80) != 0,_nin)) return false;
            if (b > bmax) bmax = b;
          }
          _bpc = bmax | (_bpc & 0x80);
        }
      } else {
        if (_lf & LfCOwn){ //���������� ������ ��� ���� ��������
          for (int i=0; i<_n.count(); ++i){
            uint8_t b = _bpc & 0x7F;
            if (-1 == _n[i].cencoding(b,(_bpc & 0x80) != 0,_nin)) return false;  //���������
            _n[i].bpc = _bpc;
          }
        } else {  //������������ ��� �����
          for (int i=0; i<_n.count(); ++i){
            uint8_t b = _bpc & 0x7F;
            if (-1 == _n[i].cencoding(b,(_bpc & 0x80) != 0,_nin)) return false;  //���������
          }
        }
      }
    }
    return true;
  }


  bool Layer::cgenerate(int32_t imax, int32_t cmax, const Uuid& salt){
    //���������������� ��������� (����� ������� � ����������������� ���-��������)
    uint32_t *s = ((uint32_t*)salt.data());
    RndMT gen(s[0] ^ s[1] ^ s[2] ^ s[3]);
    Matrix m(1,Meta(imax,MfU32,MtDO)); Vbp& v = m.at(0,0);
    //�������� ������ ��� ������ ��������� ��������
    for (int32_t i=0; i<_n.count(); ++i){
      Neuron &n = _n[i];
      n.conns.resize(cmax);
      gen.rand(imax,cmax,(unsigned long*)v.u32);
      ((Vbp)n.conns.data()).copy(v,sizeof(int32_t)*cmax);
    }
    return true;
  }


  //���������� ������������� ������� ��������, ������� � ������, ����������������� �����
  bool Layer::cline(int32_t imax, int32_t cmax){
    for (int32_t i=0; i<_n.count(); ++i){
      Neuron &n = _n[i];
      n.conns.resize(cmax);
      int32_t start = (int32_t)(i*(float)imax/(float)_n.count()) % imax;
      for (int32_t j=0; j<cmax; ++j)
        n.conns[j] = (start + j) % imax;
    }
    return true;
  }


  bool Layer::train(const Matrix::Row &own, const Matrix::Row &all, const Matrix::Row &out, WeightTrainAlg wta){
    //������� ������� 1xR32 ��� ��������� ���� � �������������� ��� ����
    Matrix m(1,Meta(out.meta().count(),MfR32,MtC));
    Matrix::Row sig = m.row(0);
    if (!sig.convert(out)) return false;
    //�������� ����
    for (int32_t i=0; i<_n.count(); ++i)
      _n[i].weights.resize(_n[i].conns.count());

    //������������� ����� �������� �������������� ������
    if (!trainR32Continuous(_n,own.data(),own.count(),all.data(),all.count(),own.meta().count(),sig.at(0),wta==WtaMinAndSuff)){
      //������� ����
      for (int32_t i=0; i<_n.count(); ++i)
        _n[i].weights.clear();
      return false;
    }
    return true;
  }



  bool Layer::protect(const Matrix::Row &out){
    Matrix m(1,Meta(out.meta().count(),MfR32,MtC));
    Matrix::Row sig = m.row(0);
    if (!sig.convert(out)) return false;
    if (!trainR32Protected(_n,sig.at(0))) return false;
    //���������� ���� ����������� ������
    _lf |= LfProt;
    return true;
  }



  bool  Layer::sim(const Matrix::Row &in, Matrix::Row &out, bool defended){
    //Matrix m(out.count(),Meta(out.meta().count(),MfR32,MtDO));    //�������� ������ ��� �������� ������
    //Matrix::Row sig = m.row(0);
    //if (!simR32(_n,sig.count(),in.data(),sig.data(),defended)){    //������������
    //  return false;
    //}
    //return out.convert(sig);                                    //������������� � ������� ������������������
    return simR32I1(_n,out.count(),in.data(),out.data(),defended);
  }


  //���������� ������������ ������ ������� ���� �� ���� "����"
  bool Layer::testP1(const Matrix::Row &own, const Matrix::Row &out, float &p1){
    Matrix mOut2(own.count(),out.meta());       Matrix::Row out2 = mOut2.row(0);

    if (!sim(own,out2,_lf & Layer::LfProt)) return false;

    int32_t ehit=0;
    for (int32_t i=0; i<own.count(); ++i){
      uint32_t h = hammU1(out2.at(i),out.at(0),out.meta().count());
      if (h) ehit++;
    }
    p1 = (float)(ehit/(float)own.count());
    return true;
  }


  //���������� �� ���������������� ���� "��� �����" �� ������ ������������� ��� ��������
  bool Layer::testP2(const Matrix::Row &all, const Matrix::Row &out, float &p2){
    Matrix mOut2(all.count(),out.meta());       Matrix::Row out2 = mOut2.row(0);
    Matrix mHam(1,Meta(all.count(),MfU32,MtD)); Matrix::Row ham  = mHam.row(0);

    if (!sim(all,out2,_lf & Layer::LfProt)) return false;

    for (int32_t i=0; i<all.count(); ++i){
      ham.at(0).u32[i] = hammU1(out2.at(i),out.at(0),out.meta().count());
    }
    float Q = qualI32(ham.at(0),ham.meta().count());
    p2 = pow(10,-(0.87f+0.221f*Q*Q));
    return true;
  }




  bool  Layer::write(QDataStream &pub, QDataStream &pri) const {
    //����::=[nin:2][nout:2][����� lf:1]
    //            [�������� "��� �����"][��������� "��� �����]
    //            [cda:1][ncmax:1][bpc|fdc:1][����� �������]{nout}
    //            [nwmax:1][bpw|fooc:1][���� �������]{nout}
    //            [������ ��������]
    //            [��� ������]
    //  lf �����::= [cfix|wfix|mdinc|binc||binc|prot:1]
    //  ����� ��������::== �) {[nc:1][bpc|fdc]:1,���� cfix=0}[..�����..],���� cda=������ | �) [uuid],���� cda=FixAlg | �) {nil}, ���� cda=FixTable
    //  ���� ��������::=={[nw:1 �� ���=0][[bpw|fooc:1],���� wfix=0}[..����..]
    //  ��� ������::=[��������=0x00]
    //  ������ ��������::=[bpw|fooc:1][bmax:4][.. �������� ..]{nout}  //�� ��������������
    //  �������� ::=[bpw|fooc:1][mmax:4][.. �������� ..]{nin}         //�� ��������������
    //  ���������::=[bpw|fooc:1][dmax:4]{nin}                         //�� ��������������
    //
    //nin   - ����� ������ �������� (2 �����)
    //nout  - ����� ������� (2 �����)
    //ncmax - ������������ ����� ������ (2 �����) �����
    //nwmax - ������������ ����� ������ (2 �����) �����
    //nw    - ����� ����� i-�� �������
    //n�    - ����� ������ � ������� i-��
    //
    //bpc - ��� �� ����� (��� ������� ������� ��� ��� ���� ����) (�� 0x0F)
    //fdc - ������� ������������� ������������� ����������� (��� ������� ������� ��� ��� ���� ����)
    //bpw - ��� �� ��� (��� ������� ������� ��� ��� ���� ����) (�� 0x1F)
    //fooc- ������� ������������� "0" (��� ������� ������� ��� ��� ���� ����)

    if (!_n.count()) { ELOG(1); return false;}
    //������ ���������
    pub<<(uint16_t)_nin;        //����� ������
    pub<<(uint16_t)_n.count();  //����� �������
    pub<<(uint8_t)_lf;          //�����, ������������ ��������� ��������� ������ ����

    //������ ���������� ����������
    if (_lf & LfMdInc) { ELOG(2); return false; }

    //������ ������
    pub<<(uint8_t)_cda;         //����� ��������� ������
    pub<<(uint8_t)_nconn;       //����� ������ �� ������ (������������)
    pub<<(uint8_t)_bpc;         //����� ����������� ������ (�����)
    //qDebug()<<"WRITE CON bpc="<<_bpc<<" nc="<<_nconn;
    switch (_cda){
    case CdaFixTable: //�� ������� ����������
      break;
    case CdaFixAlg:   //����������� ������ ������������� ���������, ��������, 16 ���� (128 ���)
      if (-1 == pri.writeRawData((const char*)_salt.data(),16)) { ELOG(3); return false; }
      break;
    case CdaRandom:   //����������� ������ ������� ������
      for (int32_t i=0; i<_n.count(); ++i){
        const Neuron &n = _n[i];
        uint8_t nc1, bpc1;
        if (_lf & LfCOwn){
          nc1  = n.conns.count();
           bpc1 = n.bpc;
          pri<<nc1;   //����� ������ �������
          pri<<bpc1;  //����� ����������� �����
        } else {
          nc1 = _nconn; bpc1= _bpc;
        }
        //qDebug()<<"WRITE CON bpc1="<<bpc1<<" nc1="<<nc1;
        //qDebug()<<n.conns;
        if (!n.cencode(pri,nc1,bpc1 & 0x7F,(bpc1 & 0x80) != 0,_nin)) { ELOG(4); return false; }
      }
      break;
    default: return false;
    }

    //������ �����
    //qDebug()<<"WRITE WEI bpw="<<_bpw<<" nw="<<_nconn;
    pub<<(uint8_t)_nconn;//���������������
    pub<<(uint8_t)_bpw; //����� ����������� ����� (�����)
    if (_lf & LfWOwn){  //����������� ����������� ������ � �������������� �������� ������
      for (int32_t i=0; i<_n.count(); ++i){
        const Neuron &n = _n[i];
        int32_t nw1 = n.wnotzeros();  //���������� ����� �������
        pub<<(uint8_t)nw1;  //����� ����� �������
        //qDebug()<<"WRITE WEI1 bpw1="<<_bpw<<" nw1="<<n.weights.count();
        if (!n.wencode(pub,nw1,_bpw & 0x7F, (_bpw & 0x80) != 0)) { ELOG(5); return false; }
      }
    } else {  //������������� ����� ������, � ������ ������ ������
      for (int32_t i=0; i<_n.count(); ++i){
        const Neuron &n = _n[i];
        uint8_t nw = (_lf & LfCOwn)?(uint8_t)n.conns.count():(uint8_t)_nconn;
        //qDebug()<<"WRITE WEI2 bpw1="<<_bpw<<" nw1="<<nw;
        if (!n.wencode(pub,nw,_bpw & 0x7F, (_bpw & 0x80) != 0)) { ELOG(6); return false; }
      }
    }

    //������ ���������� ��������
    if (_lf & LfBInc) { ELOG(7); return false; }

    //������ ��������� ������
    if (_lf & LfProt){
      pub<<(uint8_t)0x00; //�������� �� ���������
    }
    return true;
  }


  bool  Layer::read(QDataStream &pub, QDataStream &pri){
    clear();
    uint16_t nin;
    uint16_t nout;
    LayerFlags lf;
    ConnDistribAlg cda;
    uint8_t nconn;
    uint8_t bpc;
    uint8_t bpw;
    Uuid id;

    //������� ���������
    pub>>nin;  _nin = nin;  //����� ������
    pub>>nout;              //����� �������
    _n.resize(nout);        //���������� ����� ��������
    pub>>(uint8_t&)lf;   _lf = lf;    //�����, ������������ ��������� ��������� ������ ����

    //qDebug()<<"READ nin="<<nin<<" nout="<<nout<<" lf="<<lf;

    //��������� ���������������� �����
    if (lf & ~LayerFlags(LfCOwn | LfWOwn | LfProt)) { ELOG(1); return false; }

    //�������� ��� ��������� ������ �� ��������� ���������
    pub>>(uint8_t&)cda;   _cda = cda;    //����� ��������� ������
    pub>>nconn; _nconn = nconn;//����� ������ �� ������ (������������)
    pub>>bpc;   _bpc = bpc;    //����� ����������� ������ (�����)

    //qDebug()<<"READ cda="<<cda<<" nconn="<<nconn<<" lf="<<_lf<<" bpc="<<bpc;

    switch (cda){
    case CdaFixTable: //������������ ������� ������
      if (!cgenerate(nin,nconn,Uuid::nil)) { ELOG(2); return false; }
      break;
    case CdaFixAlg:   //������� ������������� ��� �������������� ������� ������
      if (-1 == pri.readRawData((char*)id.data(),16)) { ELOG(3); return false; }
      //qDebug()<<"READ id="<<id;
      _salt = id;
      if (!cgenerate(nin,nconn,id)) { ELOG(4); return false; }
      break;
    case CdaRandom:   //������� ������ ������� ������
      for (int32_t i=0; i<nout; ++i){
        Neuron &n = _n[i];
        uint8_t nc1, bpc1;
        if (lf & LfCOwn){
          pri>>nc1;   //����� ������ ������ �������
          pri>>bpc1;  //����� ����������� ����� ������ �������
        } else {
          nc1 = nconn; bpc1= bpc;
        }
        n.conns.resize(nc1);  //������� ����� ������� � ����������� ��
        n.bpc = bpc1;
        //qDebug()<<"READ CON bpc1="<<bpc1<<" nc="<<nc1;
        if (!n.cdecode(pri,nc1,bpc1 & 0x7F,(bpc1 & 0x80) != 0,_nin)) { ELOG(5); return false; }
      }
      break;
    default: { ELOG(6); return false; }
    }
    //qDebug()<<"READ CONNS SUCCESS";

    //�������� �����
    uint8_t nweights;
    pub>>nweights;        //���������������
    pub>>bpw; _bpw = bpw; //����� ����������� ����� (�����)
    //qDebug()<<"READ WEI bpw="<<bpw<<" nw="<<nweights;
    if (lf & LfWOwn){  //����������� ����������� ������ � �������������� �������� ������
      for (int32_t i=0; i<_n.count(); ++i){
        uint8_t nw1;
        Neuron &n = _n[i];
        pub>>nw1;  //����� ����� �������
        //qDebug()<<"READ WEI1 bpw1="<<bpw<<" nw1="<<nw1<<" nc1="<<n.conns.count();
        //������� ���� � �������� ��
        n.weights.resize(n.conns.count());
        for (int32_t j=0; j<n.weights.count(); ++j)
            n.weights[j]=0.0f;
        if (!n.wdecode(pub,nw1,bpw & 0x7F, (bpw & 0x80) != 0)) { ELOG(7); return false; }
      }
    } else {  //������������� ����� ������, � ������ ������ ������
      for (int32_t i=0; i<_n.count(); ++i){
        Neuron &n = _n[i];
        uint8_t nw1  = n.conns.count();
        uint8_t bpw1 = bpw;
        //qDebug()<<"READ WEI2 bpw1="<<bpw1<<" nw1="<<nw1;
        n.weights.resize(nw1);  //������� ����
        if (!n.wdecode(pub,nw1,bpw1 & 0x7F, (bpw1 & 0x80) != 0)) { ELOG(8); return false; }
      }
    }
    //�.�. ����� ����� ����� ���� ������ ����� ������, ��������� �������� �������� ������� ������
    for (int32_t i=0; i<_n.count(); ++i){
      Neuron &n = _n[i];
      if (n.weights.count() > n.conns.count()) n.conns.resize(n.weights.count());
    }


    //qDebug()<<"READ WEIGHTS SUCCESS";
    //�������� ��������� ������
    if (lf & LfProt){
      uint8_t alg;
      pub>>alg;
      if (alg != 0) { ELOG(9); return false;}
    }
    return true;
  }



#if !defined(QT_NO_DEBUG_STREAM)

  //������ �������
  QDebug operator<<(QDebug  dbg, const Neuron &x){
    QDebug d = dbg.nospace();
    d<<"Neuron(count="<<x.conns.count()<<" b="<<x.b<<" bpc="<<x.bpc;
    if (x.conns.count()){
      d<<"\n[";
      for (int i=0; i<x.conns.count(); ++i){
        if (i) d<<" ";
        d<<x.conns.at(i);
      }
      d<<"]";
    }
    if (x.weights.count()){
      d<<"\n[";
      for (int i=0; i<x.weights.count(); ++i){
        if (i) d<<" ";
        d<<x.weights.at(i);
      }
      d<<"]";
    }
    return d<<")";
  }


  QDebug operator<<(QDebug  dbg, const Layer::ConnDistribAlg &x){
    QDebug d = dbg.nospace();
    static QList<Layer::ConnDistribAlg> k;
    static QList<char*> v;
    if (k.isEmpty()){
      k<<Layer::CdaRandom<<Layer::CdaFixTable<<Layer::CdaFixAlg<<Layer::CdaFixByQual<<Layer::CdaFixByCorr<<Layer::CdaDynByQual<<Layer::CdaDynByCorr;
      v<<"CdaRandom"<<"CdaFixTable"<<"CdaFixAlg"<<"CdaFixByQual"<<"CdaFixByCorr"<<"CdaDynByQual"<<"CdaDynByCorr";
    }
    int i = k.indexOf(x);
    if (i>=0) d<<v.at(i); else d<<"Cda"<<(uint8_t)x;
    return d;
  }


  QDebug operator<<(QDebug  dbg, const Layer::WeightTrainAlg &x){
    QDebug d = dbg.nospace();
    static QList<Layer::WeightTrainAlg> k;
    static QList<char*> v;
    if (k.isEmpty()){
      k<<Layer::WtaFullUsing<<Layer::WtaMinAndSuff;
      v<<"WtaFullUsing"<<"WtaMinAndSuff";
    }
    int i = k.indexOf(x);
    if (i>=0) d<<v.at(i); else d<<"Wta"<<(uint8_t)x;
    return d;
  }

  QDebug operator<<(QDebug  dbg, const Layer::LayerFlags &x){
    QDebug d = dbg.nospace();
    static QList<Layer::LayerFlagsEnum> k;
    static QList<char*> v;
    if (k.isEmpty()){
      k<<Layer::LfCOwn<<Layer::LfWOwn<<Layer::LfBInc<<Layer::LfMdInc<<Layer::LfProt;
      v<<"LfCOwn"<<"LfWOwn"<<"LfBInc"<<"LfMdInc"<<"LfProt";
    }

    d<<"{";
    bool once=false;
    for (int i=0; i<k.count(); ++i){
      if (x & k[i]){
        if (once) d<<"|"; else once = true;
        d<< v[i];
      }
    }
    d<<"}";
    return d;
  }

  //������ ����
  QDebug operator<<(QDebug  dbg, const Layer &x){
    QDebug d = dbg.nospace();
    d<<"Layer(out="<<x._n.count()<<" lf="<<x._lf<<" cda="<<x._cda<<" in="<<x._nin<<" con="<<x._nconn<<" salt"<<x._salt<<" bpc="<<x._bpc<<" bpw="<<x._bpw;
    if (x._n.count()){
      d<<"\n";
      for (int i=0; i<min(x._n.count(),5); ++i){
        d<<i<<":"<<x._n.at(i)<<"\n";
      }
    }
    return d<<")";
  }
#endif


} //end namespace Bc
