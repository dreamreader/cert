#define _USE_MATH_DEFINES
#include <math.h>
#include "VbpMath.h"

namespace Bc {
  using namespace Nb;

  //Округление с заданной точностью
  // x -  [in] округляемое значение
  // digits - [in] количество цифр после нуля
  float roundUp( float x, int32_t digits ){
    return floor(x*pow(10.0f,digits)+0.5f)/pow(10.0f,digits);
  }


  //Вычисление корреляции между двумя столбцами векторов
  //  w     - [вх] первый набор векторов вещественных значений
  //  w2    - [вх] второй набор векторов вещественных значений
  //  ncols - [вх] число примеров
  //  ie    - [вх] номер столбца первого набора
  //  ie2   - [вх] номер столбца второго набора
  float corrColR32( const Vbp *w, const Vbp *w2, uint32_t ncols, uint32_t ie, uint32_t ie2){
    //Найти первых два статистических момента
    float aver = averColR32(w,ncols,ie);
    float aver2= averColR32(w2,ncols,ie2);
    float disp = dispColR32(w,ncols,ie,aver);
    float disp2= dispColR32(w2,ncols,ie2,aver2);

    //Вычислить корреляцию
    float corr = 0.0f;
    for(uint32_t ic=0; ic<ncols; ++ic){
      corr += ((aver - w[ic].r32[ie])*(aver2 - w2[ic].r32[ie2]))/(disp*disp2);
    }
    return corr/ncols;
  }

  //Вычисление перекрестной корреляции между примерами по столбцам
  //  Вычисление перекрестной корреляции осуществляется путем построения
  //  расчета таблицы вычисления корреляции "каждого с каждым" вектора
  //  подаваемого на вход набора векторов вещественных значений
  //  С целью экономии ресурсов рассчитываются только
  //  левый верхний треугольник корреляции матрицы (ncols*ncols)
  //  w       - [вх] набор векторов вещественных значений
  //  ncols   - [вх] число примеров в наборе
  //  nelms   - [вх] число элементов в наборе
  //  corr    - [вых] вектор вещественных значений корреляции из ncols*(ncols-1)/2 значений
  void corrColCrossR32(const Vbp *w, uint32_t ncols, uint32_t nelms, Vbp &corr){
    uint32_t c=0;
    //Пройти по всем столбцам (1,2..nelms)
    for (uint32_t ie=0; ie<nelms; ++ie){
      //Пройти по всем столбцам (i+1,i+2..nelms)
      for (uint32_t jc=ie+1; jc<nelms; jc++){
        corr.r32[c] = corrColR32(w,w,ncols,ie,jc);
        ++c;
      }
    }
  }

  // Быстрая сортировка массива
  // A        - [in/out] массив элементов, который нужно упорядочить
  // nelms    - [in]	   количество элементов
  void mathQSort(float A[], uint32_t nelms){
    uint32_t i = 0;				    //увеличиваемый индекс
    uint32_t j = nelms-1;		  //уменьшаемый индекс
    float x = A[nelms >> 1];	//средний элемент (по значению)

    do {
      while (A[i] < x) {i++;}
      while (x < A[j]) {j--;}
      if (i <= j) { mathSwap(A[i],A[j]); i++; j--; }
    } while (i <= j);

    if (0 < j)			mathQSort(&A[0], j + 1);
    if (i < nelms-1)	mathQSort(&A[i], nelms - i);
  }

  // Быстрая сортировка массива
  // A        - [in/out] массив элементов, который нужно упорядочить
  // nelms    - [in]	   количество элементов
  void mathQSort(int32_t A[], uint32_t nelms){
    uint32_t i = 0;				    //увеличиваемый индекс
    uint32_t j = nelms-1;		  //уменьшаемый индекс
    float x = A[nelms >> 1];	//средний элемент (по значению)

    do {
      while (A[i] < x) {i++;}
      while (x < A[j]) {j--;}
      if (i <= j) { mathSwap(A[i],A[j]); i++; j--; }
    } while (i <= j);

    if (0 < j)			mathQSort(&A[0], j + 1);
    if (i < nelms-1)	mathQSort(&A[i], nelms - i);
  }

  // Быстрая сортировка массива по возрастанию
  // A    - [in/out]	массив элементов, который нужно упорядочить
  // P		- [in]		  массив указателей, который связан с А и сортируется вместе с ним, т.е. сортируем только А, а P меняем так же
  // nelms- [in]		  количество элементов
  void mathQSort(float A[], void* P[], uint32_t nelms)
  {
    uint32_t i = 0;				    //увеличиваемый индекс
    uint32_t j = nelms-1;		  //уменьшаемый индекс
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

  // Быстрая сортировка массива по возрастанию
  // A    - [in/out]	массив элементов, который нужно упорядочить
  // P		- [in]		  массив указателей, который связан с А и сортируется вместе с ним, т.е. сортируем только А, а P меняем так же
  // nelms- [in]		  количество элементов
  void mathQSort(int32_t A[], float P[], uint32_t nelms)
  {
    uint32_t i = 0;				    //увеличиваемый индекс
    uint32_t j = nelms-1;		  //уменьшаемый индекс
    int32_t x = A[nelms >> 1];	//средний элемент (по значению)

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


  // Вычисление стойкости к атакам подбора
  float qual2Stab (float qual){
    return 0.87f+0.221f*pow(qual,2);		// формула АИ (точнее чем моя)
  }

  //.............................................................................
  //  Статистические функции для вещественных векторов значений
  //.............................................................................

  //Вычисление среднего значения по вектору
  //  w       - [вх] набор векторов коэффициентов
  //  nelms   - [вх] число элементов вектора
  float averR32(const Vbp &w, uint32_t nelms){
    float aver = 0.0f;
    for (uint32_t ie=0; ie<nelms; ++ie){
      aver += w.r32[ie];
    }
    return (aver/nelms);
  }

  //Вычисление дисперсии по вектору
  //  w       - [вх] набор векторов коэффициентов
  //  ncols   - [вх] число элементов вектора
  //  aver    - [вх] расчетное математическое ожидание этого же столбца
  float dispR32(const Vbp &w, uint32_t nelms, float aver){
    float disp = 0.0f;
    for (uint32_t ie=0; ie<nelms; ++ie){
      float d = w.r32[ie] - aver;
      disp += d*d;
    }
    return sqrt(disp/nelms);
  }

  //Вычисление качества
  //  aver    - [вх] расчетное математическое ожидание
  //  disp    - [вх] расчетное качество
  float qualR32(float aver, float disp){
    if (disp == 0.0f)
      if (aver == 0.0f) return 0.0f;
      else              return 16.0f;          //максимальное значение качества
    else return fabs(aver)/disp;
    //if (disp <= 0.00001f)
    //  if (fabs(aver) <= 0.00001f)  return 0.0f;
    //  else                        return 32.0f;  //максимальное значение качества
    //else if (fabs(aver) <= 0.1f)   return 0.0f;
    //else return fabs(aver)/disp;
  }

  //Вычисление математического ожидания, дисперсии и качества для столбца набора векторов
  void adqR32(const Vbp &w, uint32_t nelms, float &a, float &d, float &q){
    a = averR32(w,nelms);
    d = dispR32(w,nelms,a);
    q = qualR32(a,d);
  }

  //Вычисление среднего значения по столбцу
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

  //Вычисление дисперсии по столбцу
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

  //Вычисление математического ожидания, дисперсии и качества для столбца набора векторов
  void adqColR32(const Vbp *w, uint32_t ncols, uint32_t ielm, float &a, float &d, float &q){
    a = averColR32(w,ncols,ielm);
    d = dispColR32(w,ncols,ielm,a);
    q = qualR32(a,d);
  }


  //Вычисление математического ожидания, дисперсии и качества для каждого примера входного параметра
  //  w       - [вх] набор примеров, представленных векторами вещественных чисел
  //  ncols   - [вх] число примеров
  //  nelms   - [вх] число элементов вектора
  //  a,d,q   - [вых] статистические характеристики (r32[nelms])
  void adqColR32(const Vbp *w, uint32_t ncols, uint32_t nelms, Vbp &a, Vbp &d, Vbp &q){
    for (uint32_t ie=0; ie<nelms; ++ie){
      a.r32[ie] = averColR32(w,ncols,ie);
    }
    if (d.isEmpty()) return;
    for (uint32_t ie=0; ie<nelms; ++ie){
      d.r32[ie] = dispColR32(w,ncols,ie,a.r32[ie]);
    }
    if (q.isEmpty()) return;
    for (uint32_t ie=0; ie<nelms; ++ie){
      q.r32[ie] = qualR32(a.r32[ie],d.r32[ie]);
    }
  }

  //Поиск минимального значения целочисленного вектора
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

  //Поиск максимального значения целочисленного вектора
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

  float amaxR32(const Vbp &w, uint32_t nelms, uint32_t &imax){
    float x = fabs(w.r32[0]);
    imax = 0;
    for (uint32_t ie=0; ie<nelms; ++ie){
      if (x < fabs(w.r32[ie])){
        imax = ie;
        x = fabs(w.r32[ie]);
      }
    }
    return x;
  }

  //Вычисление стабильности, уникальности и качества для соответствующих
  //элементов векторов
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

  //Вычисление стабильности, уникальности и качества через среднее и дисперсию Своих и Чужих
  //void soqColR32(float aown, float down, float aall, float dall, float &s, float &o, float &q){
  //  s = dall/down;
  //  o = fabs(aall-aown)/dall;
  //  q = fabs(aall-aown)/(dall+down);
  //}

  //.............................................................................
  //  Статистические функции для целых векторов значений
  //.............................................................................

  //Вычисление среднего значения по вектору бит
  //  w      - [вх] набор векторов коэффициентов
  //  ncols  - [вх] число векторов коэффициентов
  //  ielm   - [вх] номер элемента вектора
  float averColU1(const Vbp *w, uint32_t ncols, uint32_t ielm){
    uint32_t aver = 0;
    for (uint32_t ic=0; ic<ncols; ++ic){
      aver += w[ic].u1(ielm);
    }
    return ((float)aver/ncols);
  }


  //Вычисление среднего значения по вектору
  //  w      - [вх] вектор коэффициентов
  //  nelms  - [вх] число элементов вектора
  float averI32(const Vbp &w, uint32_t nelms){
    float aver = 0.0f;
    for (uint32_t ie=0; ie<nelms; ++ie){
      aver += w.i32[ie];
    }
    return (aver/nelms);
  }

  //Вычисление дисперсии по вектору
  //  w      - [вх] набор векторов коэффициентов
  //  nelms  - [вх] число элементов вектора
  //  aver   - [вх] расчетное математическое ожидание этого же столбца
  float dispI32(const Vbp &w, uint32_t nelms, float aver){
    float disp = 0.0f;
    for (uint32_t ie=0; ie<nelms; ++ie){
      float d = w.i32[ie] - aver;
      disp += d*d;
    }
    return sqrt(disp/nelms);
  }

  //Вычисление качества
  //  aver   - [вх] расчетное математическое ожидание
  //  disp   - [вх] расчетное качество
  float qualI32(float aver, float disp){
    if (disp == 0.0f)
      if (aver == 0.0f) return 0.0f;
      else              return 16.0f;          //максимальное значение качества
    else               return fabs(aver)/disp;
  }

  //Поиск минимального значения целочисленного вектора
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

  //Поиск максимального значения целочисленного вектора
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

  //Подсчет числа нулей в векторе
  uint32_t cumNilI32(const Vbp &w, uint32_t nelms){
    uint32_t cum = 0;
    for (uint32_t ie=0; ie<nelms; ++ie){
      if (w.i32[ie] == 0) ++cum;
    }
    return cum;
  }

  //Вычисление характеристик для вектора целочисленных значений
  void adqI32(const Vbp &w, uint32_t nelms, float &a, float &d, float &q){
    a = averI32(w,nelms);
    d = dispI32(w,nelms,a);
    q = qualI32(a,d);
  }

  //Подсчет отличий между векторами бит
  uint32_t hammU1(const Vbp &w, const Vbp &w2, uint32_t nelms){
    uint32_t cum=0;
    for (uint32_t ie=0; ie<nelms; ++ie){
      cum += (w.u1(ie)!=w2.u1(ie));
    }
    return cum;
  }

  //Подсчет отличий между векторами бит
  uint32_t hammR32(const Vbp &w, const Vbp &w2, uint32_t nelms){
    uint32_t cum=0;
    for (uint32_t ie=0; ie<nelms; ++ie){
      cum += (sign2(w.r32[ie])!=sign2(w2.r32[ie]));
    }
    return cum;
  }

  //Учет совпадений по знаку
  uint32_t diffsSignR32(const Vbp &w, float s, uint32_t nelms){
    uint32_t cum=0;
    for (uint32_t ie=0; ie<nelms; ++ie){
      if (((w.r32[ie]<=0.0f) && (s<=0.0f)) || ((w.r32[ie]>0.0f) && (s>0.0f)))
        cum++;
    }
    return cum;
  }


  //Предобработка обучающих примеров
  //  После работы функции может измениться количество образов и коэффициенты,
  //  т.к. выбросим "плохие примеры"
  //  В результирующем векторе коэффициентов coefs образы упорядочены следующим образом:
  //  чем ближе образ к среднему, тем ближе он к началу, т.е. вначале идут образы из
  //  центра области Свой, а последний - с края области Свой.
  //
  //  w         - [вх] набор векторов r32[ncols][nelms]
  //  ncols     - [вх] число векторов w
  //  nelms     - [вх] число элементов в одном векторе w
  //  wbetter   - [вых] набор нулевых векторов, в который будет занесены
  //                    указатели на отсортированные по возрастанию ge вектора w.
  //                    r32[ncols] = null
  //              (память должна быть выделена только под массив указателей)
  //  nbetter   - [вых] число "хороших" векторов
  //  ge        - [вых] одиночный вектор, с ранжирующими значениями (r32[nelms])
  bool geR32(const Vbp *w, uint32_t ncols, uint32_t nelms, Vbp *wbetter, uint32_t &nbetter, Vbp &ge){
    //_ASSERT(w); _ASSERT(ncols>1); _ASSERT(wbetter); _ASSERT(nelms);

    return false;

    //1. Вычислить вектор значений хи-квадрат для всех образов
    //1.1-1.2 Рассчитать математическое ожидание и дисперсию каждого элемента

    Meta meta[4] = {Meta(nelms,MfR32,MtC),Meta(nelms,MfR32,MtC),Meta(nelms,MfR32,MtC),Meta(nelms,MfR32,MtC)};
    Matrix m(3,1,meta);
    Vbp a = m.at(0,0); //вектор мат. ожидания
    Vbp d = m.at(1,0); //вектор дисперсии
    Vbp q = m.at(2,0); //вектор качества

    adqColR32(w,ncols,nelms,a,d,q);

    //1.3 Вычислить хи-квадрат по формуле

    Vbp chi2 = m.at(3,0); //вектор значений хи-квадрат

    for (uint32_t ic=0; ic<ncols; ++ic){
      chi2.r32[ic] = 0.0f;
      for (uint32_t ie=0; ie<nelms; ++ie){
        float d2 = (a.r32[ie]-w[ic].r32[ie])/d.r32[ie];
        chi2.r32[ic] += d2*d2;
      }
      chi2.r32[ic] /= nelms;
    }

    //Если задан вектор ge, записать оцениваемый параметр для каждого образа
    for (uint32_t ic=0; ic<ncols; ++ic){
      ge.r32[ic] = chi2.r32[ic];
    }

    //2. Отфильтровать образы
    //Заполнить wbetter
    memcpy(wbetter,w,ncols*sizeof(Vbp));
    //2.1 Отсортировать указатели на примеры в соответствии со значениями chi2
    mathQSort(chi2.r32,(void**)wbetter,ncols);
    //2.2 Определить первую половину образов
    uint32_t ihalf = (uint32_t)ceill((float)ncols/2);
    //2.3 Вычислить расстояние между первым и последним элементами в отсортированной половине хи-2
    float chi2dist = chi2.r32[ihalf] - chi2.r32[0];
    //2.4 Зная ширину первой половины, найти правую границу (по равенству левой и правой половин)
    float chi2max = chi2.r32[ihalf+1] + chi2dist;

    //2.5 Отбросить все примеры, со значением хи-квадрат больше chi2max
    nbetter= ihalf+1;
    while ((nbetter < ncols) && (chi2.r32[nbetter] < chi2max)){
      ++nbetter;
    }
    //Лучшие примеры определены (индексы 0..nbetter-1)

    //Промасштабировать ge, чтобы хорошие образы попадали в 0..1
    for (uint32_t ic=0; ic<ncols; ++ic){
      ge.r32[ic] /= chi2max;
    }
  }

  // Вычисление значения плотности вероятности нормального распределения для заданного X
  // x    - [in] значение для которого вычисляется плотность вероятности
  // aver - [in] математическое ожидание распределения
  // disp - [in] стандартное отклонение
  // возвращает вычисленное значение плотности вероятности со знаком, относительно 0
  double probNormDistrX(double x, double aver, double disp){
    if (disp == 0.0) return 0.0;
    double deg = -1.0*pow((x-aver),2)/(2*pow(disp,2));
    double res = pow(M_E, deg) / (disp*pow(2*M_PI,0.5));
    return res;
  }

  // Вычисление вероятности (со знаком) распределения относительно начала координат
  // aver - [in] математическое ожидание распределения
  // disp - [in] стандартное отклонение
  // a    - [in] левая граница интервала
  // b    - [in] правая граница интервала
  // возвращает: значение вероятности
  float probNormR32(float aver, float disp, float a, float b){
    double step = 0.01;       // шаг интегрирования
    uint32_t n = 0;           // количество интервалов
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
    //Найти большую часть интеграла и установить знак по математическому ожиданию
    if (s<0.5) s = 1.0-s;
    s *= (aver>=0)?1.0:-1.0;
    return (float)s;
  }

  void probNormR32(const Vbp &aver, const Vbp &disp, uint32_t nelms, float a, float b, Vbp &prob){
    for (uint32_t ie=0; ie<nelms; ++ie){
      prob.r32[ie] = probNormR32(aver.r32[ie],disp.r32[ie],a,b);
    }
  }

}//end namespace Bc

