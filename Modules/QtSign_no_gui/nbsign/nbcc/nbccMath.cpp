/****************************************************************************
** ����:        nbccMath.cpp
** ������:
** ����:        15.03.2010
** �������:
** �����:       ������� �.�., ������� �.�.
**
** ��������:    �������������� ������ ��� �������� ��������� �����.
****************************************************************************/

#include "nbccMath.h"

/****************************************************************************
** �������������� �������������� � �������
****************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//����������: ��������� � �������� ���������
//���������:
//  val	   - [��] ����������� ��������
//  digits - [��] ���������� ���� ����� ����
//
float roundUp ( float val, int32_t digits ) {
  return floor (val * pow ( 10.0f, digits ) + 0.5f ) / pow ( 10.0f, digits );
}

/****************************************************************************
** �������������� ������� ��� ������������ �������� ��������
****************************************************************************/

//����������: ���������� �������� �������� �� �������
//���������:
//  w       - [��] ����� �������� �������������
//  nelms   - [��] ����� ��������� �������
//
float averR32(const Vbp &w, uint32_t nelms){
  float aver = 0.0f;
  for (uint32_t ie=0; ie<nelms; ++ie){
    aver += w.r32[ie];
  }
  return (aver/nelms);
}

//����������: ���������� ��������� �� �������
//���������:
//  w       - [��] ����� �������� �������������
//  nelms   - [��] ����� ��������� �������
//  aver    - [��] ��������� �������������� �������� ����� �� �������
//
float dispR32(const Vbp &w, uint32_t nelms, float aver){
  float disp = 0.0f;
  for (uint32_t ie=0; ie<nelms; ++ie){
    float d = w.r32[ie] - aver;
    disp += d*d;
  }
  return sqrt(disp/nelms);
}

//����������: ���������� ��������
//���������:
//  aver    - [��] ��������� �������������� ��������
//  disp    - [��] ��������� ��������
//
float qualR32(float aver, float disp){
  if (disp == 0.0f)  
    if (aver == 0.0f) return 0.0f;
    else              return 16.0f;          //������������ �������� ��������
  else return fabs(aver)/disp;
}

//����������: ���������� ��������������� ��������, ��������� �
//            �������� ��� ������� ������ ��������
void adqR32(const Vbp &w, uint32_t nelms, float &a, float &d, float &q){
  a = averR32(w,nelms);
  d = dispR32(w,nelms,a);
  q = qualR32(a,d);
}

//����������: ���������� �������� �������� �� �������
//���������:
//  w       - [��] ����� �������� �������������
//  ncols   - [��] ����� ��������
//  ielm    - [��] ������ ��������, ��� �������� �������������� ��������
float averColR32(const Vbp *w, uint32_t ncols, uint32_t ielm){
  float aver = 0.0f;
  for (uint32_t ic=0; ic<ncols; ++ic){
    aver += w[ic].r32[ielm];
  }
  return (aver/ncols);
}

//����������: ���������� ��������� �� �������
//���������:
//  w       - [��] ����� �������� �������������
//  ncols   - [��] ����� ��������
//  ielm    - [��] ������ ��������, ��� �������� �������������� ��������
//  aver    - [��] ��������� �������������� �������� ����� �� �������
float dispColR32(const Vbp *w, uint32_t ncols, uint32_t ielm, float aver){
  float disp = 0.0f;
  for (uint32_t ic=0; ic<ncols; ++ic){
    float d = w[ic].r32[ielm] - aver;
    disp += d*d;
  }
  return sqrt(disp/ncols);
}

// ����� ������������ �������� �������������� �������
float minR32(const Vbp &w, uint32_t nelms, uint32_t &imin){
  float x = w.r32[0];
  imin = 0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    if (x > w.r32[ie]){
      imin = ie;
      x = w.r32[ie];
    }
  }
  return x;
}

// ����� ������������� �������� �������������� �������
float maxR32(const Vbp &w, uint32_t nelms, uint32_t &imax){
  float x = w.r32[0];
  imax = 0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    if (x < w.r32[ie]){
      imax = ie;
      x = w.r32[ie];
    }
  }
  return x;
}

//����������: ���������� ������������, ������������ � ��������
//            ��� ��������������� ��������� ��������
//���������:
//  aown      - [��] �������������� �������� "����"
//  down      - [��] ��������� "����"
//  aall      - [��] �������������� �������� "��� �����"
//  dall      - [��] ��������� "��� �����"
//  ncols     - [��] ����� ���������
//  s         - [���] ������������
//  o         - [���] ������������
//  q         - [���] ��������
void soqColR32( const Vbp &aown, const Vbp &down, const Vbp &aall, const Vbp & dall, 
               uint32_t ncols, float &s , float &o, float &q){
  uint32_t n = ncols;
  s = 0.0f; o = 0.0f; q = 0.0f;
  for (uint32_t i=0; i<ncols; ++i){
    if (down.r32[i] == 0.0f) {n--; continue; }
    if (dall.r32[i] == 0.0f) {n--; continue; }
    s += dall.r32[i]/down.r32[i];
    o += fabs(aall.r32[i] - aown.r32[i])/dall.r32[i];
    q += fabs(aall.r32[i] - aown.r32[i])/(dall.r32[i] + down.r32[i]);
  }
  s /= n; o /= n; q /= n;
}

/****************************************************************************
** �������������� ������� ��� ����� �������� ��������
****************************************************************************/

//����������: ���������� �������� �������� �� ������� ���
//���������:
//  w         - [��] ����� �������� �������������
//  ncols     - [��] ����� �������� �������������
//  ielm      - [��] ����� �������� �������
float averColU1(const Vbp *w, uint32_t ncols, uint32_t ielm){
  uint32_t aver = 0;
  for (uint32_t ic=0; ic<ncols; ++ic){
    aver += w[ic].u1(ielm);
  }
  return ((float)aver/ncols);
}


//����������: ���������� �������� �������� �� �������
//���������:
//  w         - [��] ������ �������������
//  nelms     - [��] ����� ��������� �������
float averI32(const Vbp &w, uint32_t nelms){
  float aver = 0.0f;
  for (uint32_t ie=0; ie<nelms; ++ie){
    aver += w.i32[ie];
  }
  return (aver/nelms);
}

//����������: ���������� ��������� �� �������
//���������:
//  w         - [��] ����� �������� �������������
//  nelms     - [��] ����� ��������� �������
//  aver      - [��] ��������� �������������� �������� ����� �� �������
float dispI32(const Vbp &w, uint32_t nelms, float aver){
  float disp = 0.0f;
  for (uint32_t ie=0; ie<nelms; ++ie){
    float d = w.i32[ie] - aver;
    disp += d*d;
  }
  return sqrt(disp/nelms);
}

//����������: ���������� ��������
//���������:
//  aver      - [��] ��������� �������������� ��������
//  disp      - [��] ��������� ���������
//����������: ��������
float qualI32(float aver, float disp){
  if (disp == 0.0f)
    if (aver == 0.0f) return 0.0f;
    else              return 16.0f;          //������������ �������� ��������
  else               return fabs(aver)/disp;
}

// ����� ������������ �������� �������������� �������
int32_t minI32(const Vbp &w, uint32_t nelms, uint32_t &imin){
  int32_t x = w.i32[0];
  imin = 0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    if (x > w.i32[ie]){
      imin = ie;
      x = w.i32[ie];
    }
  }
  return x;
}

// ����� ������������� �������� �������������� �������
int32_t maxI32(const Vbp &w, uint32_t nelms, uint32_t &imax){
  int32_t x = w.i32[0];
  imax = 0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    if (x < w.i32[ie]){
      imax = ie;
      x = w.i32[ie];
    }
  }
  return x;
}

// ������� ����� ����� � �������
uint32_t cumNilI32(const Vbp &w, uint32_t nelms){
  uint32_t cum = 0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    if (w.i32[ie] == 0) ++cum;
  }
  return cum;
}

// ���������� ������������� ��� ������� ������������� ��������
void adqI32(const Vbp &w, uint32_t nelms, float &a, float &d, float &q){
  a = averI32(w,nelms);
  d = dispI32(w,nelms,a);
  q = qualI32(a,d);
}

// ������� ������� ����� ��������� ���
uint32_t diffsU1(const Vbp &w, const Vbp &w2, uint32_t nelms){
  uint32_t cum=0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    cum += (w.u1(ie)!=w2.u1(ie));
  }
  return cum;
}

// ���� ���������� �� �����
uint32_t diffsSignR32(const Vbp &w, float s, uint32_t nelms){
  uint32_t cum=0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    if (((w.r32[ie]<=0.0f) && (s<=0.0f)) || ((w.r32[ie]>0.0f) && (s>0.0f)))
      cum++;
  }
  return cum;
}

//����������: ���������� �������� ��������� ����������� ����������� ������������� ��� ��������� X
//���������:
//  x    - [��] �������� ��� �������� ����������� ��������� �����������
//  aver - [��] �������������� �������� �������������
//  disp - [��] ����������� ����������
//����������: ����������� �������� ��������� ����������� �� ������, ������������ 0
double probNormDistrX(double x, double aver, double disp){
  if (disp == 0.0) return 0.0;
  double deg = -1.0*pow((x-aver),2)/(2*pow(disp,2));
  double res = pow(M_E, deg) / (disp*pow(2*M_PI,0.5));
  return res;
}

//����������: ���������� ����������� (�� ������) �������������
//            ������������ ������ ���������
//���������:
// aver - [��] �������������� �������� �������������
// disp - [��] ����������� ����������
// a    - [��] ����� ������� ���������
// b    - [��] ������ ������� ���������
//����������: �������� �����������
float probNormR32(float aver, float disp, float a, float b){
  double step = 0.01;        // ��� ��������������
  uint32_t n = 0;            // ���������� ����������
  n = (uint32_t)roundUp((float)((b-a)/step),0);
  double x = a;              // �������� X
  double h = (b-a)/n;        // ������ ��������
  double s = 0.0f;           // ����� �������
  for (uint32_t i = 1; i < n; i++){
    x += step;
    s += probNormDistrX(x-(h/2.0),aver, disp);
  }
  s += probNormDistrX(b-(h/2.0),aver, disp);
  s *= h;
  // ����� ������� ����� ��������� � ���������� ���� �� ��������������� ��������
  if (s<0.5) s = 1.0-s;
  s *= (aver>=0)?1.0:-1.0;
  return (float)s;
}

void probNormR32(const Vbp &aver, const Vbp &disp, uint32_t nelms, float a, float b, Vbp &prob){
  for (uint32_t ie=0; ie<nelms; ++ie){
    prob.r32[ie] = probNormR32(aver.r32[ie],disp.r32[ie],a,b);
  }
}
