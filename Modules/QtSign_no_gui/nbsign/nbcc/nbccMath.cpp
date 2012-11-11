/****************************************************************************
** файл:        nbccMath.cpp
** версия:
** дата:        15.03.2010
** изменен:
** автор:       Майоров А.В., Захаров О.С.
**
** описание:    Математический модуль для рассчета нейронных сетей.
****************************************************************************/

#include "nbccMath.h"

/****************************************************************************
** Математические преобразования с числами
****************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//назначение: округлить с заданной точностью
//параметры:
//  val	   - [вх] округляемое значение
//  digits - [вх] количество цифр после нуля
//
float roundUp ( float val, int32_t digits ) {
  return floor (val * pow ( 10.0f, digits ) + 0.5f ) / pow ( 10.0f, digits );
}

/****************************************************************************
** Статистические функции для вещественных векторов значений
****************************************************************************/

//назначение: вычисление среднего значения по вектору
//параметры:
//  w       - [вх] набор векторов коэффициентов
//  nelms   - [вх] число элементов вектора
//
float averR32(const Vbp &w, uint32_t nelms){
  float aver = 0.0f;
  for (uint32_t ie=0; ie<nelms; ++ie){
    aver += w.r32[ie];
  }
  return (aver/nelms);
}

//назначение: вычисление дисперсии по вектору
//параметры:
//  w       - [вх] набор векторов коэффициентов
//  nelms   - [вх] число элементов вектора
//  aver    - [вх] расчетное математическое ожидание этого же столбца
//
float dispR32(const Vbp &w, uint32_t nelms, float aver){
  float disp = 0.0f;
  for (uint32_t ie=0; ie<nelms; ++ie){
    float d = w.r32[ie] - aver;
    disp += d*d;
  }
  return sqrt(disp/nelms);
}

//назначение: вычисление качества
//параметры:
//  aver    - [вх] расчетное математическое ожидание
//  disp    - [вх] расчетное качество
//
float qualR32(float aver, float disp){
  if (disp == 0.0f)  
    if (aver == 0.0f) return 0.0f;
    else              return 16.0f;          //максимальное значение качества
  else return fabs(aver)/disp;
}

//назначение: вычисление математического ожидания, дисперсии и
//            качества для столбца набора векторов
void adqR32(const Vbp &w, uint32_t nelms, float &a, float &d, float &q){
  a = averR32(w,nelms);
  d = dispR32(w,nelms,a);
  q = qualR32(a,d);
}

//назначение: вычисление среднего значения по столбцу
//параметры:
//  w       - [вх] набор векторов коэффициентов
//  ncols   - [вх] число примеров
//  ielm    - [вх] индекс элемента, для которого рассчитывается значение
float averColR32(const Vbp *w, uint32_t ncols, uint32_t ielm){
  float aver = 0.0f;
  for (uint32_t ic=0; ic<ncols; ++ic){
    aver += w[ic].r32[ielm];
  }
  return (aver/ncols);
}

//назначение: вычисление дисперсии по столбцу
//параметры:
//  w       - [вх] набор векторов коэффициентов
//  ncols   - [вх] число примеров
//  ielm    - [вх] индекс элемента, для которого рассчитывается значение
//  aver    - [вх] расчетное математическое ожидание этого же столбца
float dispColR32(const Vbp *w, uint32_t ncols, uint32_t ielm, float aver){
  float disp = 0.0f;
  for (uint32_t ic=0; ic<ncols; ++ic){
    float d = w[ic].r32[ielm] - aver;
    disp += d*d;
  }
  return sqrt(disp/ncols);
}

// Поиск минимального значения целочисленного вектора
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

// Поиск максимального значения целочисленного вектора
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

//назначение: вычисление стабильности, уникальности и качества
//            для соответствующих элементов векторов
//параметры:
//  aown      - [вх] математическое ожидание "Свой"
//  down      - [вх] дисперсия "Свой"
//  aall      - [вх] математическое ожидание "Все чужие"
//  dall      - [вх] дисперсия "Все чужие"
//  ncols     - [вх] число элементов
//  s         - [вых] стабильность
//  o         - [вых] уникальность
//  q         - [вых] качество
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
** Статистические функции для целых векторов значений
****************************************************************************/

//назначение: вычисление среднего значения по вектору бит
//параметры:
//  w         - [вх] набор векторов коэффициентов
//  ncols     - [вх] число векторов коэффициентов
//  ielm      - [вх] номер элемента вектора
float averColU1(const Vbp *w, uint32_t ncols, uint32_t ielm){
  uint32_t aver = 0;
  for (uint32_t ic=0; ic<ncols; ++ic){
    aver += w[ic].u1(ielm);
  }
  return ((float)aver/ncols);
}


//назначение: вычисление среднего значения по вектору
//параметры:
//  w         - [вх] вектор коэффициентов
//  nelms     - [вх] число элементов вектора
float averI32(const Vbp &w, uint32_t nelms){
  float aver = 0.0f;
  for (uint32_t ie=0; ie<nelms; ++ie){
    aver += w.i32[ie];
  }
  return (aver/nelms);
}

//назначение: вычисление дисперсии по вектору
//параметры:
//  w         - [вх] набор векторов коэффициентов
//  nelms     - [вх] число элементов вектора
//  aver      - [вх] расчетное математическое ожидание этого же столбца
float dispI32(const Vbp &w, uint32_t nelms, float aver){
  float disp = 0.0f;
  for (uint32_t ie=0; ie<nelms; ++ie){
    float d = w.i32[ie] - aver;
    disp += d*d;
  }
  return sqrt(disp/nelms);
}

//назначение: вычисление качества
//параметры:
//  aver      - [вх] расчетное математическое ожидание
//  disp      - [вх] расчетная дисперсия
//возвращает: качество
float qualI32(float aver, float disp){
  if (disp == 0.0f)
    if (aver == 0.0f) return 0.0f;
    else              return 16.0f;          //максимальное значение качества
  else               return fabs(aver)/disp;
}

// Поиск минимального значения целочисленного вектора
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

// Поиск максимального значения целочисленного вектора
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

// Подсчет числа нулей в векторе
uint32_t cumNilI32(const Vbp &w, uint32_t nelms){
  uint32_t cum = 0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    if (w.i32[ie] == 0) ++cum;
  }
  return cum;
}

// Вычисление характеристик для вектора целочисленных значений
void adqI32(const Vbp &w, uint32_t nelms, float &a, float &d, float &q){
  a = averI32(w,nelms);
  d = dispI32(w,nelms,a);
  q = qualI32(a,d);
}

// Подсчет отличий между векторами бит
uint32_t diffsU1(const Vbp &w, const Vbp &w2, uint32_t nelms){
  uint32_t cum=0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    cum += (w.u1(ie)!=w2.u1(ie));
  }
  return cum;
}

// Учет совпадений по знаку
uint32_t diffsSignR32(const Vbp &w, float s, uint32_t nelms){
  uint32_t cum=0;
  for (uint32_t ie=0; ie<nelms; ++ie){
    if (((w.r32[ie]<=0.0f) && (s<=0.0f)) || ((w.r32[ie]>0.0f) && (s>0.0f)))
      cum++;
  }
  return cum;
}

//назначение: вычисление значения плотности вероятности нормального распределения для заданного X
//параметры:
//  x    - [вх] значение для которого вычисляется плотность вероятности
//  aver - [вх] математическое ожидание распределения
//  disp - [вх] стандартное отклонение
//возвращает: вычисленное значение плотности вероятности со знаком, относительно 0
double probNormDistrX(double x, double aver, double disp){
  if (disp == 0.0) return 0.0;
  double deg = -1.0*pow((x-aver),2)/(2*pow(disp,2));
  double res = pow(M_E, deg) / (disp*pow(2*M_PI,0.5));
  return res;
}

//назначение: вычисление вероятности (со знаком) распределения
//            относительно начала координат
//параметры:
// aver - [вх] математическое ожидание распределения
// disp - [вх] стандартное отклонение
// a    - [вх] левая граница интервала
// b    - [вх] правая граница интервала
//возвращает: значение вероятности
float probNormR32(float aver, float disp, float a, float b){
  double step = 0.01;        // шаг интегрирования
  uint32_t n = 0;            // количество интервалов
  n = (uint32_t)roundUp((float)((b-a)/step),0);
  double x = a;              // значение X
  double h = (b-a)/n;        // ширина столбика
  double s = 0.0f;           // общая площадь
  for (uint32_t i = 1; i < n; i++){
    x += step;
    s += probNormDistrX(x-(h/2.0),aver, disp);
  }
  s += probNormDistrX(b-(h/2.0),aver, disp);
  s *= h;
  // Найти большую часть интеграла и установить знак по математическому ожиданию
  if (s<0.5) s = 1.0-s;
  s *= (aver>=0)?1.0:-1.0;
  return (float)s;
}

void probNormR32(const Vbp &aver, const Vbp &disp, uint32_t nelms, float a, float b, Vbp &prob){
  for (uint32_t ie=0; ie<nelms; ++ie){
    prob.r32[ie] = probNormR32(aver.r32[ie],disp.r32[ie],a,b);
  }
}
