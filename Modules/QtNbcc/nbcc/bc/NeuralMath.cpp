#define _USE_MATH_DEFINES
#include <math.h>
#include "rndmt.h"
#include "VbpMath.h"
#include "NeuralMath.h"
#include "BcBitStream.h"

namespace Bc {
  using namespace Nb;

  //------------------------------------------------------------------------------------------
  //Функции эмуляции ИНС
  //------------------------------------------------------------------------------------------

  //Вычисление суммы выхода нейрона (без пороговой функции)
  //  n       - [вх] нейрон
  //  inp     - [вх] набор примеров
  //  ncols   - [вх] число примеров
  //  sig     - [вых]вектор откликов с числом элементов ncols
  bool mulsumR32(const Neuron &nr, const Vbp *inp, int32_t ncols, Vbp &sig){
    //Для каждого образа
    for (int32_t i=0; i<ncols; ++i){
      //Промоделировать работу нейрона на входном векторе
      float o1 = nr.b;                                             //Добавить значение подставки
      for (int32_t j=0; j<nr.nconns(); ++j){
        o1 += inp[i].r32[nr.conns[j]]*nr.weights[j];
      }
      sig.r32[i] = o1;
    }
    return true;
  }

  //Вычисление суммы выхода нейрона без index-веса
  bool mulsumN1R32(const Neuron &nr, int32_t index, const Vbp *inp, int32_t ncols, Vbp &sig){
    //Для каждого образа
    for (int32_t i=0; i<ncols; ++i){
      //Промоделировать работу нейрона на входном векторе
      float o1 = nr.b;                                             //Добавить значение подставки
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

  //Пороговая функция
  //для [-oo; 0] пороговая функция выдает 0
  //для (0; +oo] пороговая функция выдает 1
  //  sig       - [вх/вых] отклики нейронной сети
  //  nelms     - [вх] число откликов
  void hardlimR32(Vbp &sig, int32_t nelms){
    for (int32_t i=0; i<nelms; ++i){
      sig.r32[i] = (sig.r32[i]>0.0f)? 1.0f : 0.0f;
    }
  }

  //Пороговая функция с двумя состояниями
  //для [-oo; 0] пороговая функция выдает -1
  //для (0; +oo] пороговая функция выдает 1
  //  sig     - [вх/вых] отклики нейронной сети
  //  nelms   - [вх] число откликов
  void sign2R32(Vbp &sig, int32_t nelms){
    for (int32_t i=0; i<nelms; ++i){
      sig.r32[i] = (sig.r32[i]>0.0f)? 1.0f : -1.0f;
    }
  }

  //Вычисление выходного качества нейрона
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

  //Вычисление стабильности i-го выхода нейрона по двум состояниям [<=0, >0]
  //и преобладающего знака выхода (-1 или 1). Если 0, то -1.
  //значения stab в лучшем случае 0, в худшем 1
  void stab2stateColR32(const Vbp *inp, int32_t ncols, int32_t nelms, Vbp &stab, Vbp &sgn){
    for (int32_t i=0; i<nelms; ++i){
      float p0 = 0.0f;
      for (int32_t j=0; j<ncols; ++j)
        if (inp[j].r32[i]<0.0f) p0 += 1;
      p0 /= ncols;
      if (p0>=0.5) sgn.r32[i]=-1.0; else sgn.r32[i]=1.0f;
      if (p0 == 0.0f || p0 == 1.0f)
        //значения выходных битов абсолютно стабильны
        stab.r32[i] = 0.0f;
      else
        //значения выходных бит частично стабильны
        stab.r32[i] = (float)fabs(p0*log(p0)/M_LN2 + (1.0-p0)*log(1.0-p0)/M_LN2);
    }
  }

  //Проверка попадания в заданный интервал
  // описание:
  //    - для выходов сумматоров рассчитывается матожидание и дисперсия
  //			 попавших в требуемый диапазон
  //		- если диапазон не задан, то работаем с диапазоном от -3сигма до 3сигма.
  // sig    - [вх] выходные значения нейрона, снятые путем симуляции
  // ncols  - [вх] число векторов
  // interv - [вх] диапазон значений (массив из двух чисел: мин., макс. значения выхода нейрона
  int32_t hittestR32(const Vbp &sig, int32_t nelms){
    int32_t hit = 0;
    float left, right;
    float a,d,q;
    adqR32(sig,nelms,a,d,q);
    left  = a - 3.0f*d + 0.01f;//a - 3.0f*d + 0.01f;
    right = a + 3.0f*d - 0.01f;//a + 3.0f*d - 0.01f;
    //Проверить для всех выходных значений
    for ( int32_t i=0; i<nelms; ++i){
      if (/*(sign2(left)==sign2(right)) && */(sig.r32[i] > left) && (sig.r32[i] < right))
        ++hit;
    }
    return hit;
  }

  //Определяет число совпадений знаков
  int32_t sigtestR32(const Vbp &sig, int32_t nelms, float bit){
    int32_t hit=0;
    for ( int32_t i=0; i<nelms; ++i){
      if (sign2(sig.r32[i]) == sign2(bit)) ++hit;
    }
    return hit;
  }

  //Моделирование работы слоя непрерывного слоя нейронной сети на нескольких образах.
  //примечание:
  //  - используется пороговая функция активации sign2R32 [-1,1].
  //  l     - [вх] слой нейронной сети
  //  ncols - [вх] число примеров
  //  inp   - [вх] набор векторов входных сигналов
  //  sig   - [вых]набор векторов выходных откликов слоя
  //  defended - [вх] используется перемешивание на предыдущих битах
  bool simR32(const Neurons &l, int32_t ncols, const Vbp *inp, Vbp *sig, bool defended){

    Matrix mtxSigCol(2,Meta(ncols,MfR32,MtContinuous));
    Vbp sig1 = mtxSigCol.at(0,0); //  отклики одного нейрона по всем примерам
    Vbp sig0 = mtxSigCol.at(0,1); //  предыдущие отклики одного нейрона

    //АЛГОРИТМ вычисления при инверсии всех весов
    //Установить начальные отклики в 1
    for (int32_t i=0; i<ncols; ++i) sig0.r32[i] = 1.0f;
    //Для каждого нейрона
    for (int32_t i=0; i<l.size(); ++i){
      //Вычислить сумму
      mulsumR32(l[i],inp,ncols,sig1);
      //Использовать пороговую функцию
      sign2R32(sig1,ncols);
      //Скопировать результат, учтя знак предыдущего нейрона
      if (defended){
        for (int32_t j=0; j<ncols; ++j)
          sig[j].r32[i] = sig1.r32[j] = sig1.r32[j] * sig0.r32[j];
        //Обменять местами матрицы
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
    Vbp sig1 = mtxSigCol.at(0,0); //  отклики одного нейрона по всем примерам
    Vbp sig0 = mtxSigCol.at(0,1); //  предыдущие отклики одного нейрона

    //АЛГОРИТМ вычисления при инверсии всех весов
    //Установить начальные отклики в 1
    for (int32_t i=0; i<ncols; ++i) sig0.r32[i] = 1.0f;
    //Для каждого нейрона
    for (int32_t i=0; i<l.size(); ++i){
      //Вычислить сумму
      mulsumR32(l[i],inp,ncols,sig1);
      //Использовать пороговую функцию
      sign2R32(sig1,ncols);
      //Скопировать результат, учтя знак предыдущего нейрона
      if (defended){
        for (int32_t j=0; j<ncols; ++j){
          sig1.r32[j] *= sig0.r32[j];
          sig[j].i1(i,sig1.r32[j]>0.0f);
        }
        //Обменять местами адреса векторов (чтобы не копировать)
        Vbp tmp = sig1; sig1 = sig0; sig0 = tmp;
      } else {
        for (int32_t j=0; j<ncols; ++j)
          sig[j].i1(i,sig1.r32[j]>0.0f);
      }
    }
    return true;
  }

  //------------------------------------------------------------------------------------------
  //ИНДИКАЦИЯ ГРУБЫХ ОТКЛОНЕНИЙ
  //------------------------------------------------------------------------------------------

  //Фильтрация заведомо плохих обучающих примеров (для непрерывных ВБП)
  //  После работы функции может измениться количество образов и коэффициенты,
  //  т.к. выбросим "плохие примеры"
  //  В результирующем векторе коэффициентов coefs образы упорядочены следующим образом:
  //  чем ближе образ к среднему, тем ближе он к началу, т.е. вначале идут образы из
  //  центра области Свой, а последний - с края области Свой.
  //
  //  own      - [вх] набор векторов параметров "Свой", n
  //  nown     - [вх] число векторов параметров "Свой"
  //  nelms    - [вх] число элементов в одном векторе параметров
  //  better   - [вых] набор указателей на "хорошие" векторы параметров "Свой"
  //              (память должна быть выделена только под массив указателей)
  //  nbetter  - [вых] число оставшихся "хороших" векторов
  //  ge       - [вых] n векторов из одного значения, с рангом образов (от 0 до 1 хорошие, больше 1 плохие)
  bool indicateGeR32(  const Vbp *own, uint32_t nown,  uint32_t nelms,
                              Vbp *better, uint32_t &nbetter, Vbp *ge){

    if (nown<2) return false;

    //1. Вычислить вектор значений хи-квадрат для всех образов
    //1.1-1.2 Рассчитать математическое ожидание и дисперсию каждого элемента
    Matrix mAdq(3,Meta(nelms,MfR32,MtContinuous));
    Vbp &a = mAdq.at(0,0);
    Vbp &d = mAdq.at(0,1);
    Vbp &q = mAdq.at(0,2);
    adqColR32(own,nown,nelms,a,d,q);

    //1.3 Вычислить хи-квадрат по формуле
    Matrix mChi2(1,Meta(nown,MfR32,MtContinuous));
    Vbp &chi2 = mChi2.at(0,0);  //вектор значений хи-квадрат
    for (uint32_t i=0; i<nown; ++i){
      chi2.r32[i] = 0.0f;
      for (uint32_t j=0; j<nelms; ++j){
        float x;
        if (fabs(d.r32[j])<0.00001f)
          if (fabs(a.r32[j]-own[i].r32[j])<0.00001) x=0.0f; //плохое расстояние
          else                                      x=32.f; //хорошее расстояние
        else
          x = (a.r32[j]-own[i].r32[j])/d.r32[j];          //по формуле
        chi2.r32[i] += x*x;
      }
      chi2.r32[i] /= nelms;
    }

    //Если задан вектор ge, записать оцениваемый параметр для каждого образа
    if (ge){
      for (uint32_t i=0; i<nown; ++i){
        ge[i].r32[0] = chi2.r32[i];
      }
    }

    //2. Отфильтровать образы
    //Заполнить better
    memcpy(better,own,nown*sizeof(Vbp));
    //2.1 Отсортировать указатели на примеры в соответствии со значениями chi2
    mathQSort(chi2.r32,(void**)better,nown);
    //2.2 Определить первую половину образов
    uint32_t iHalf = (uint32_t)ceill((float)nown/2);
    //2.3 Вычислить расстояние между первым и последним элементами в отсортированной половине хи-2
    float chi2Dist = chi2.r32[iHalf] - chi2.r32[0];
    //2.4 Зная ширину первой половины, найти правую границу (по равенству левой и правой половин)
    float chi2Max = chi2.r32[iHalf+1] + chi2Dist;
    //2.5 Отбросить все примеры, со значением хи-квадрат больше chi2Max
    nbetter = iHalf+1;
    while ((nbetter < nown) && (chi2.r32[nbetter] < chi2Max)){
      ++nbetter;
    }
    //Лучшие примеры определены (индексы 0..nbetter-1)

    //Промасштабировать ge, чтобы хорошие образы попадали в 0..1
    if (ge){
      for (uint32_t i=0; i<nown; ++i){
        ge[i].r32[0] /= chi2Max;
      }
    }
    return true;
  }

  //Оценки качества, ошибок приближения, ошибок эмуляции
  struct Qes {
    float q;    //качество
    int32_t e;  //ошибки приближения
    int32_t s;  //ошибки эмуляции
  };


  bool  Range::intersect(const Range &range) const {
    return intersect(*this,range).valid();
  }

  //Формирует пересечение интервалов
  Range Range::intersect(const Range &a, const Range &b){
    Range c;
    if (a.left>b.right)     c.left = max(a.right,b.right);//Нет пересечений
    else if (a.left>=b.left)c.left = a.left;
    else                    c.left = b.left;
    if (a.right<b.left)         c.right = min(a.left,b.left);//Нет пересечений
    else if (a.right<=b.right)  c.right = a.right;
    else                        c.right = b.right;
    return c;
  }


//------------------------------------------------------------------------------------------------
//НОВЫЕ ПРОЦЕДУРЫ ОБУЧЕНИЯ НЕЙРОНА
//------------------------------------------------------------------------------------------------

  //примечание:
  //  Для вещественных (float) чисел может быть такое
  //  y0 = y0 + x; y1 = y0 - x; (y0 != y1) true
  //  из-за ошибок округления, поэтому оптимизация на границах диапазона не всегда возможна

  //Нахождение диапазона с минимальным числом ошибок sigtest
  //  nr  - настраиваемый нейрон
  //  iw  - номер изменяемого веса
  //  own, nown, bit - примеры "Свой", их число, требуемый бит
  //  left  - левая граница
  //  right - правая граница
  //возвращает: минимальное число ошибок и значения диапазона
  int32_t trainFindSHit(Neuron &nr, int32_t index, const Vbp *own, int32_t nown, float bit, Range &range){
    Matrix mOut0(1,Meta(nown,MfR32,MtContinuous)); Vbp &out0 = mOut0.at(0,0);  //вектор откликов нейрона
    Matrix mOut(1,Meta(nown,MfR32,MtContinuous)); Vbp &out = mOut.at(0,0);  //вектор откликов нейрона
    Matrix mK(1,Meta(nown+2,MfR32,MtContinuous)); Vbp &k = mK.at(0,0);      //вектор точек перехода (+2 крайних)
    Matrix mErr(1,Meta(nown+2,MfI32,MtContinuous)); Vbp &err = mErr.at(0,0);//ошибки распределения и значений в каждом интервале

    //Найти экстремальные точки смены значения для каждого примера, приняв w[i] за 0
    mulsumN1R32(nr,index,own,nown,out0);
    for (int32_t i=0; i<nown; ++i)  //Для каждого примера найти точку, в которой происходит смена знака
      k.r32[i+1] = - out0.r32[i]/own[i].r32[nr.conns[index]];

    //Отсортировать их по возрастанию и добавить граничные (лучше сразу учесть weiD)
    mathQSort(k.r32+1,nown);
    k.r32[0] = k.r32[1] - 1000.0f;
    k.r32[nown+1] = k.r32[nown] + 1000.0f;

    //В каждом из nown+1 интервале будет происходить смена знака, а значит и число ошибок
    //-oo--------<k0>-----<k1>-----0-----<kn>------ +oo
    //Проверить число ошибок в каждом интервале, выбрав наименьшее
    int32_t  ehit=nown+1;
    for (int32_t i=0; i<nown+1;++i){
      float wk = k.r32[i]/2.0f + k.r32[i+1]/2.0f;  //Выбрать значение веса внутри i-го интервала
      for (int32_t j=0; j<nown;++j){
        out.r32[j] = out0.r32[j] + wk*own[j].r32[nr.conns[index]];
      }

      err.i32[i] =  nown - sigtestR32(out,nown,bit);
      if (err.i32[i] <= ehit) ehit = err.i32[i];
    }

    //Сформировать общий интервал, путем объединения соседних интервалов с одинаковой ошибкой
    int32_t imin;
    int32_t imax;
    for (imin=0; imin<nown+1; ++imin) if (err.i32[imin]==ehit) break;
    for (imax=imin+1; imax<nown+1; ++imax) if (err.i32[imax]!=ehit) break;

    range.left  = (imin==0)?k.r32[imin]-1000.0f:k.r32[imin];
    range.right = (imax==(nown+1))?k.r32[imax-1+1]+1000.0f:k.r32[imax-1+1];

    return ehit;
  }


  //Тестирование качества в заданном диапазоне
  //примечание:
  //  Экспериментально подтверждено, что в пределах диапазона (left, right) с одним и тем же sigtest
  //  значение hittest может меняться несколько раз. Поэтому один из способов снизить hittest,
  //  использовать генератор случайных чисел в пределах диапазона (исключив границы)
  int32_t trainMinimizeEHit(RndMT &g, Neuron &nr, int32_t index, const Vbp *own, int32_t nown, Range range, float &optW){
    Matrix mOut0(1,Meta(nown,MfR32,MtContinuous)); Vbp &out0 = mOut0.at(0,0);  //вектор откликов нейрона
    Matrix mOut(1,Meta(nown,MfR32,MtContinuous)); Vbp &out = mOut.at(0,0);  //вектор откликов нейрона

    mulsumN1R32(nr,index,own,nown,out0);

    int32_t ehit=nown+1; //Максимальное число ошибок (оптимальный вес)
    optW=range.left;

    //Подставляя случайные значения, минимизировать ошибку hit
    int32_t i = (range.left != range.right)?256:1;
    for (;i>0;){//Выполнить просмотр значений, не включая левую и правую границы
      float wk = g.rand(range.left,range.right);
      if (wk == range.left) { i--; continue; } //значения границ пропускаются
      //Найти значение out с учетом index-го веса
      for (int32_t j=0; j<nown;++j){
        out.r32[j] = out0.r32[j] + wk*own[j].r32[nr.conns[index]];
      }

      //Проверить ошибку
      int32_t ecur = nown - hittestR32(out,nown);
      if (ecur < ehit){
        ehit = ecur;
        optW  = wk;
        if (optW>0.0f) range.right = optW; else range.left = optW; //Сдвинуть границы генерации в сторону ближайшего к 0 значения (wei всегда внутри left,right)
        i = i >> 1;
        //if (ecur == 0) break; //Оптимальное значение найдено
      } else if (ecur == ehit && (fabs(optW)>fabs(wk))){  //Второй критерий - мин. модуля веса
        ehit = ecur;
        optW  = wk;
        if (optW>0.0f) range.right = optW; else range.left = optW;
        i = i >> 1; //Снизить число следующий попыток подбора в 2 раза
      } else {
        i--;
      }
    }
    return ehit;
  }



  //Вычисление диапазона значений для n первых входов
  Range calcWRange(const Vbp &out, uint32_t nout){
    float a = averR32(out,nout);
    float d = dispR32(out,nout,a);
    a = fabs(a);
    return Range(-a-d-1.0f,a+d+1.0f);
  }

  //Вычисление показателей выходного качества, ошибок аппроксимации, ошибок эмуляции
  Qes calcWQes(const Vbp &out, int32_t nout, float bit){
    Qes qes;
    qes.q = qualR32(out,nout);
    qes.e = nout - hittestR32(out,nout);
    qes.s = nout - sigtestR32(out,nout,bit);
    return qes;
  }



  //НЕЛЬЗЯ ИСПОЛЬЗОВАТЬ ИЗ-ЗА ОШИБОК ОКРУГЛЕНИЯ mulsumR32 Пересчет выхода сумматора для нового значения веса
  //inline void updateOutByW(Neuron &nr, int32_t index, const Vbp *own, uint32_t nown, Vbp &out, float curW, float predW){
  //  for (int j=0; j<nown; ++j)                      //Довычислить значения сумматора на всех примерах
  //    out.r32[j] += own[j].r32[nr.conns[index]]*(curW-predW);
  //}

  //Установка качества (как можно ближе к Q с большей стороны) с помощью в выбранном диапазоне
  //  n - число учитываемых весов
  //  index - номер настраиваемого нейрона
  //  own - примеры
  //  nown - число примеров
  //  out - рассчитанная с помощью mulsum матрица для n текущих весов на nown примерах
  //  range - диапазон, в котором подбираются значения
  //  targetQ  - требуемое качество
  //  pred - текущие показатели качества и ошибок
  //  optW - (вых) оптимальное значение веса
  //  opt - полученные показатели качества и ошибок
  //возвращает: признак улучшения текущего качества
  bool optimizeWByOutQualRand(RndMT &g, Neuron &nr, int32_t index, const Vbp *own, int32_t nown, float bit,  Range range, float targetQ, const Qes &pred, float &optW, Qes &opt, Vbp &out0, Vbp &out){
    //Рассчитать сумму без index-веса
    mulsumN1R32(nr,index,own,nown,out0);

    bool changed = false;
    opt  = pred;
    optW = nr.weights[index];

    for (int32_t i=5; i>0; --i){//Число попыток подбора = 5 (цикл вынесен)
      float wk = g.rand(range.left,range.right);    //Сгенерировать вес
      //Найти значение out с учетом index-го веса
      for (int32_t i=0; i<nown;++i){
        out.r32[i] = out0.r32[i] + wk*own[i].r32[nr.conns[index]];
      }

      Qes cur = calcWQes(out,nown,bit);                                        //Оценить показатели качества обучения

      if ((cur.e+cur.s) <= (pred.e+pred.s)){ //Проверить увеличение ошибок
        if (opt.q<= targetQ){ //Качество нужно увеличивать в любом диапазоне (при равенстве q предпочтение отдается более близкому к 0 значению)
          if ((cur.q > opt.q) || ((cur.q == opt.q) && (fabs(wk)<fabs(optW)))){
            opt = cur;
            optW = wk;
            changed = true;
          }
        } else {  //Качество нужно снижать, но не меньше targetQ (при равенстве q предпочтение отдается более близкому к 0 значению)
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


  //Минимизация числа ошибок аппроксимации и эмуляции
  //  opt   - [вых] значение качества и ошибок
  //  optW  - [вых] значение веса
  //  index - [вых] номер веса
  //возвращает: индекс последнего просмотренного элемента
  void optimizeWByEs(RndMT &g, Neuron &nr, int32_t n, const Vbp *own, int32_t nown, float bit, const Range &goodRng, float &optW, int32_t &index, Qes &opt){
    opt.s = nown; opt.e = nown;
    optW  = 0.0f;
    index = -1;
    int32_t i=0;
    for (; i<n; i++){
      Qes   cur; cur.q = opt.q;
      Range range;
      float curW;
      //Найти минимальную ошибку примеров
      cur.s = trainFindSHit(nr,i,own,nown,bit,range);
      //Продолжить, только если число ошибок не увеличилось
      if (cur.s > opt.s) continue;
      cur.e = trainMinimizeEHit(g,nr,i,own,nown,range,curW);
      //Продолжить, только если число ошибок сократилось и текущий диапазон меньше
      if (index>=0){
        if (cur.s == opt.s){
            if (cur.e > opt.e) continue;
            if ((cur.e == opt.e) && (fabs(curW) < fabs(optW))) continue;
        }
      }
      opt   = cur;
      optW  = curW;
      index = i;
      if (goodRng.in(curW) && !opt.s && !opt.e) break;  //Прервать поиск, если лучшее решение в диапазоне goodRange
    }
  }


  //Установка весов по выходному биту и входному качеству
  //  k - число первых устанавливаемых весов (остальные обнуляются)
  //  a - вх. мат. ожидание
  //  q - вх. качество
  //  bit - требуемый отклик нейрона
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


  //АЛГОРИТМ ОБУЧЕНИЯ НЕЙРОНА 1, С КОТОРОГО НУЖНО НАЧИНАТЬ
  //РЕЗУЛЬТАТ: отлично обучается и хорошо работает для тестовых выборок, но и P2 большая
  //вес просто выбирается пропорционально качеству, а знак по биту и вх.мат.ожиданию sign(a*bit)
  //т.е. работает в гипотезе независимых законов для высококачественных примеров.
  //(выходное качество можно получить больше, но тогда будет сильнее привязка к конкретной выборке)

  //АЛГОРИТМ ОБУЧЕНИЯ НЕЙРОНА 2
  //РЕЗУЛЬТАТЫ: хорошо разрешает для имеющихся примеров, но работает с 50% для другой тестовой выборки
  //(без учета корреляции, с мин. ошибок эмуляции и аппроксимации, с приближением качества)
  //
  //настроить по знаку и качеству "Свой" все нейроны
  //по всем определить диапазон значений генерации весов = +-(|aver|+disp+1.0f)
  //пробуем настроить по k нейронам из n (1 нейрон не настраиваем, а для 2 и более настраиваем со второго нейрона)
  //  генерировать нейрон по качеству, снижая или повышая его до нужно величины, не увеличивая ошибки e-hit и s-hit
  //    (генерацию начинаем с k-го нейрона и двигаемся к началу, если не удалось вписаться по качеству - расширяем диапазон)
  //  минимизируем ошибки в диапазоне текущих значений по всем настраиваемым
  //    (если не удалось вписаться в диапазон, расширяем число нейронов)
  //обнулить неиспользуемые нейроны в конце списка
  //
  //!если не удалось полностью настроить:
  //  а.попробовать настроку на n связях без ограничений на диапазон
  //  б.восстановить лучший вариант среди полученных
  //
  Qes trainQFastOwn2(RndMT &g, bool lazy, Neuron &nr, const Vbp *own, int32_t nown, const Vbp &a, const Vbp &q, float bit, Vbp &out0, Vbp &out){
    int32_t maxN = nr.nconns();
    Qes best = {-1.0f,nown+1,nown+1};            //Вычислить лучшее качество
    //Если алгоритм ленивый, то пытаемся настроить с 1 нейрона
    //если жадный, то сразу настраиваем все нейроны, т.е. i=n-1
    int32_t n = (lazy)?1:maxN;
    for (;n<=maxN;++n){  //Цикл попыток настройки по качеству первых n нейронов
      //Настроить по качеству
      setWByInQual(nr,n,a,q,bit);

      //Нормализовать по максимальному абсолютному значению веса
      uint32_t imax;
      float m = amaxR32(Vbp(nr.weights.data()),nr.nconns(),imax);
      if (m!=0.0f)
      for (int32_t i=0; i<nr.nconns(); ++i)
        nr.weights[i] /= m;

      //Рассчитать качество и ошибки
      mulsumR32(nr,own,nown,out);
      Qes cur = calcWQes(out,nown,bit);
      Range goodRng = calcWRange(out,nown);
      best = cur;

      //Выполнить приближение качества к рекомендуемому без снижения числа ошибок
      bool stop = false;
      while (!stop){
        stop = true;
        for (int32_t index=1;index<n; ++index){//Начать со второго веса
          float curW;
          Qes qes;
          if (optimizeWByOutQualRand(g,nr,index,own,nown,bit,goodRng,4.5f,cur,curW,qes, out0, out)){
            nr.weights[index] = curW;                                   //Применить
            cur = qes;
            best = cur;
            if (!cur.s && !cur.e && (best.q<5.0f)) { stop = true; break; }
            stop = false;
          }
        }
      }

      if (cur.s || cur.e )
      for (int32_t i=0; i<n; ++i){ //Цикл поиска n-решений (можно продолжать, пока не будет улучшений)
        float curW;
        int32_t index;
        optimizeWByEs(g,nr,n,own,nown,bit,goodRng,curW,index,cur);    //Улучшить
        nr.weights[index] = curW;                                   //Применить
        cur.q = qualR32(out,nown);
        best = cur;
        if (!cur.s && !cur.e) break;  //Если все ошибки устранены, можно выйти
      }

      if (!cur.s && !cur.e ){
        if (cur.q>5.0f && n<maxN) continue; //Попробовать снизить качество, увеличив число связей
        if (cur.q>4.0f) break;              //Качество в диапазоне - выйти с успехом
        continue;  //иначе - повысить качество, увеличив число связей
      }
    }
    return best;
  }

  /*Qes trainQFastOwn2(RndMT &g, bool minUsing, Neuron &nr, const Vbp *own, int32_t nown, const Vbp &a, const Vbp &q, float bit, Vbp &out0, Vbp &out){
    int32_t maxN = nr.nconns();
    Matrix mWBest(1,Meta(maxN,MfR32,MtC)); Vbp &wb = mWBest.at(0,0);  //Лучшие веса, для которых Qes(q>4.0, e=0, s=0)
    Qes best = {-1.0f,nown+1,nown+1};                                 //Вычислить лучшее качество

    //Пройти с левой границы, увеличивая до правой, пока не удается обучить, чтобы все примеры проходили
    //если достигнута правая граница, обучить на ней.
    //Найти оптимальное решение между l и r (l+r)/2, считая, что для r имеется решение.
    bool has1 = false;  //Найдено 1-е решение (если все должны быть заполнены, проверяется максимальное решение)
    int32_t l=(minUsing)?1:maxN;
    int32_t r=(minUsing)?1:maxN;
    //Увеличивать правую границу, подтягивая левую.
    //Именьшать правую границу, двигаясь к левой.
    do {
      //Выбрать текущее число используемых связей
      if (!has1)  n = r;
      else        n = (r+l)/2;

      //.................................................

      //Настроить по качеству
      setWByInQual(nr,n,a,q,bit);

      //Нормализовать по максимальному абсолютному значению веса
      uint32_t imax;
      float m = amaxR32(Vbp(nr.weights.data()),nr.nconns(),imax);
      if (m!=0.0f)
      for (int32_t i=0; i<nr.nconns(); ++i)
        nr.weights[i] /= m;

      //Рассчитать качество и ошибки
      mulsumR32(nr,own,nown,out);
      Qes cur = calcWQes(out,nown,bit);
      Range goodRng = calcWRange(out,nown);

      //Выполнить приближение качества к рекомендуемому без снижения числа ошибок
      bool stop = false;
      while (!stop){
        stop = true;
        for (int32_t index=1;index<n; ++index){//Начать со второго веса
          float curW;
          Qes qes;
          if (optimizeWByOutQualRand(g,nr,index,own,nown,bit,goodRng,4.5f,cur,curW,qes, out0, out)){
            nr.weights[index] = curW;                                   //Применить
            cur = qes;
            if (!cur.s && !cur.e && (cur.q<5.0f)) { stop = true; break; }
            stop = false;
          }
        }
      }

      //Выполнить коррекцию ошибок
      if (cur.s || cur.e )
      for (int32_t i=0; i<n; ++i){ //Цикл поиска n-решений (можно продолжать, пока не будет улучшений)
        float curW;
        int32_t index;
        optimizeWByEs(g,nr,n,own,nown,bit,goodRng,curW,index,cur);    //Улучшить
        nr.weights[index] = curW;                                   //Применить
        cur.q = qualR32(out,nown);
        if (!cur.s && !cur.e) break;  //Если все ошибки устранены, можно выйти
      }

      if (!cur.s && !cur.e ){
        if (cur.q>5.0f && n<maxN) continue; //Попробовать снизить качество, увеличив число связей
        if (cur.q>4.0f) break;              //Качество в диапазоне - выйти с успехом
        continue;  //иначе - повысить качество, увеличив число связей
      }

      //..................................................

      if (!has1){
        //Проверить решение: отличное (q в диапазоне, e=0, s=0), хорошее (q любое, e=0, s=0), удовлетворительное (q любое, e=любое, s=любое лучшее из возможных)
        //
        if (!cur.s && !cur.e){
          if (cur.q>4.0f && cur.q<5.0f){  //отличное решение
            has1 = true;
            best = cur;
            //..сохранить комбинацию весов
          } else {//хорошее решение (нужно сравнивать с best)
            //Если можно увеличить, число элементов, чтобы снизить качество, увеличить в 2 раза
          }
        } else {  //удовлетворительное решение (нужно сравнивать с best)

        }


        if (good){ //Максимальное решение найдено n=r
          has1 = true;
          //..сохранить комбинацию весов
        } else {
          l = r;
          r = min(maxN,r*2); //Двигаться в сторону увеличения
        }
      } else {
        if (good){  //Одно из минимальных решений найдено
          r = n;
          //..сохранить комбинацию весов
        } else {
          l = n;
        }
      }

    } while (l!=r);

    //Восстановить, если найдено лучшее, иначе оставить текущие веса (лучшие из найденных n=r)




    for (;n<=maxN;++n){  //Цикл попыток настройки по качеству первых n нейронов

    }
    return best;
  }*/

  //Обучение (настройка весов нейронов) слоя по входному качеству с регулированием границ диапазона
  //  own   - входные примеры
  //  nown  - число примеров
  //  ncols - число элементов в одном векторе
  //  out   - выходной код
  void trainQualityR32(Neurons &l, const Vbp *own, uint32_t nown, uint32_t nelms, const Vbp &sig, bool lazy){
    //Выделить память под матрицы откликов
    Matrix mOut0(1,Meta(nown,MfR32,MtC)); Vbp &out0 = mOut0.at(0,0);  //вектор откликов нейрона без index-го веса
    Matrix mOut(1,Meta(nown,MfR32,MtC)); Vbp &out = mOut.at(0,0);    //вектор откликов нейрона

    //Выделить память для a,d,q
    Matrix m(3,Meta(nelms,MfR32,MtContinuous));
    Vbp &a = m.at(0,0); //математическое ожидание "Свой"
    Vbp &d = m.at(0,1); //дисперсия "Свой"
    Vbp &q = m.at(0,2); //качество "Свой"

    adqColR32(own,nown,nelms,a,d,q);

    //Uuid uid = (Uuid)Uuid::createUuid();
    //uint32_t *s = (uint32_t*)uid.data();
    //RndMT rg(s[0] ^ s[1] ^ s[2] ^ s[3]); //Инициализировать генератор случайных чисел
    RndMT g(0);

    //Выполнить обучение для каждого нейрона
    for (int32_t i=0; i<l.size(); ++i){
      trainQFastOwn2(g,lazy,l[i],own,nown,a,q,sig.r32[i],out0,out);
    }
  }

  // Корректировка весов нейронов одного нейрона сети
  // с учетом выходной дисперсии или выходного мат.ожидания
  // nl   - [in/out] слой, веса которого корректируют
  // cor  - [in]     корректирующий массив
  void modifyWeights(Neuron &nr, float cor){
    for (int32_t i = 0; i < nr.nconns(); ++i)
      nr.weights[i] /= fabs(cor);
  }

  //Вычисление смещения нейронов на основе статистик "Все чужие"
  void modifyB(Neurons &l, const Vbp *all, uint32_t nall){
    Matrix msig1(1,Meta(nall,MfR32,MtContinuous)); //отклики одного нейрона
    Vbp &sig1 = msig1.at(0,0);
    //Вычислить сдвиг для каждого нейрона как среднее на базе "Все чужие"
    for (int32_t i=0; i<l.size(); ++i){
      //Провести симуляцию
      mulsumR32(l[i],all,nall,sig1);
      //Вычислить смещение результата относительно множества "Все чужие"
      l[i].b = averR32(sig1,nall);
    }
  }

  //Обучение слоя с непрерывными данными
  bool trainR32Continuous(  Neurons &l,
                          const Vbp *own, int32_t nown,
                          //const Vbp *tst, uint32_t ntst,
                          const Vbp *all, int32_t nall,
                          int32_t nelms,
                          const Vbp &sig, bool lazy){
    if (nown<2 || nall<2 || nelms<2) return false;

    //Обучить
    trainQualityR32(l,own,nown, nelms,sig,lazy);

    //Дообучить нейроны по дисперсии и ожиданию
    Matrix msig1(1,Meta(nown,MfR32,MtC)); //отклики одного нейрона
    Vbp &sig1 = msig1.at(0,0);
    for (int32_t i=0; i<l.size(); ++i){
      //Выполнить моделирование на образах "Свой"
      mulsumR32(l[i],own,nown,sig1);
      //Рассчитать дисперсию и выполнить поправку весов на найденное значение
      modifyWeights(l[i],dispR32(sig1,nown,averR32(sig1,nown)));
      //Повторно выполнить моделирование на образах "Свой"
      mulsumR32(l[i],own,nown,sig1);
      //Рассчитать мат. ожидание и выполнить поправку весов на найденное значение
      modifyWeights(l[i],averR32(sig1,nown));
    }
    //Оставить смещения нейронов нулевыми
    //modifyB(l,all,nall);
    return true;
  }


  //Дообучение однослойной нейронной сети на ключе (мешалка)
  //  В зависимости от бит перемешивающего ключа
  //  выполнить инверсию весовых коэффициентов нейронов слоя
  //  l - нейронная сеть
  //  sig - действующий отклик слоя для "Своего"
  bool trainR32Protected(Neurons &l, const Vbp &sig){
    //АЛГОРИТМ инверсии всех весов каждого нейрона
    float cur = sig.r32[0];
    for (int32_t j=1; j<l.size(); ++j){
      for (int32_t iw=0; iw<l[j].nconns(); ++iw)
        l[j].weights[iw] *= cur;
      cur = sig.r32[j];
    }
    return true;
  }

  //------------------------------------------------------------------------------------------
  //Функции слоя нейронной сети
  //------------------------------------------------------------------------------------------

  //Снижение разрядности весов
  //  bitPerW - устанавливаемое отношение бит на вес
  //  originOfCoord - с использованием значения веса "0" или без него
  //возвращает: длину цепочки
  bool Neuron::digitize(uint8_t bitPerW, bool originOfCoord){
    if (bitPerW<1 || bitPerW>32) return false;
    if (bitPerW == 32) return true;
    int32_t nw = weights.count();
    if (!nw) return true;
    QVector<float> &w = weights;
    uint32_t nrange = 1<<(bitPerW-1); //Число комбинаций в половине интервала

    //Найти максимальное wmax
    float wmax=0.0f;
    for (int i=0; i<nw; ++i){
      float wa = fabs(w[i]);
      if (wa>wmax) wmax = wa;
    }
    //Нормировать к |w|=1
    for (int i=0; i<nw; ++i)
      w[i] /= wmax;
    //Выполнить приведение к требуемому диапазону
    if (originOfCoord){ //с нулем
      for (int i=0; i<nw; ++i) w[i] = roundNearest(w[i]*(nrange-1));
    } else {            //без нуля
      for (int i=0; i<nw; ++i) w[i] = sign(w[i])*ceil(fabs(w[i])*nrange);
    }
    return true;
  }

  //Сортировка связей (и их весов) по возрастанию
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

  //Перемещение нулевых весов (и их связей) в конец
  void Neuron::zeroback(bool andTrim){
    if (conns.count() != weights.count()) return;
    if (!weights.count()) return;

    //Вытолкнуть нулевые веса в конец
    int nw = weights.count();
    int i0; //Номер первого нулевого веса (т.е. число ненулевых элементов)
    for (i0 = 0; i0<nw; ++i0) //Найти первый нулевой вес
      if (weights[i0] == 0.0f) break;
    if (i0>nw) return;  //Все веса ненулевые
    for (int ix=i0+1;ix<nw;++ix){
      if (weights[ix]!=0.0f) {  //Текущий вес ненулевой
        mathSwap(weights[ix],weights[i0]);
        mathSwap(conns[ix],conns[i0]);
        i0++;
      } else {                  //Текущий вес нулевой
        continue;
      }
    }
    //Отсечь или отсортировать их
    if (andTrim){
      conns.resize(i0); // ВНИМАНИЕ: может возникнуть ошибка, когда сохраняется фикс. число связей, а весов - переменное
      weights.resize(i0);
    } else {
      if ((conns.count()-i0) > 1){
        mathQSort(conns.data()+i0,weights.data()+i0,conns.count()-i0);
      }
    }
  }

  //Удаление нелувых весов в конце списка
  int32_t Neuron::wnotzeros() const{
    //Определить число нулевых весов в конце списка
    int notz = weights.count()-1;
    for (;notz>=0;--notz){
      if (weights[notz]!=0.0f) break;
    }
    notz++;//число ненулевых элементов
    return notz;
  }

  int32_t Neuron::cencoding(uint8_t &bitPerC, bool distance, int32_t nmax){
    if (!conns.count()) return 0;
    //возвращает минимальную длину или кодировку, иначе 0x00
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
        int32_t range = (1<<bitPerC) - 1; //должно быть меньше на 2, т.к. [0..range-2, range-1 резерв для]
        int32_t cost=0;
        int32_t pred=0;
        for (int32_t i=0; i<conns.count(); ++i){
          int32_t delta = conns[i] - pred; pred = conns[i];
          if (delta<0) delta += nmax;
          cost += (delta / range + 1)*bitPerC;//Определение "стоимости" кодирования одной связи в битах
        }
        return cost;
      } else {
        int32_t range = 1<<bitPerC;
        for (int32_t i=0; i<conns.count(); ++i)
          if (conns[i]>=range) return -1; //должно быть меньше на 1, т.к. считаем с [0..range-1]
        return conns.count()*bitPerC;
      }
    }
  }


  bool Neuron::cencode(QDataStream &to, int32_t nc, uint8_t bitPerC, bool distance, int32_t nmax) const {
    BitStream to2(to,BitStream::LittleEndian);
    if (nc > conns.count()) return false;     //Ошибка превышения числа связей
    if (bitPerC<2 || bitPerC>16) return false;//Ошибка значения

    if (distance){
      int32_t range = (1<<bitPerC) - 1;
      int32_t pred = 0;
      for (int32_t i=0; i<nc; ++i){  //Для всех весов
        int32_t delta = conns[i] - pred; pred = conns[i];
        if (delta<0) delta += nmax;
        for (int32_t j=delta/range; j>0; --j){
          if (!to2.writeBits(range,bitPerC)) return false; //Максимальное значение для декодирования
        }
        if (!to2.writeBits(delta % range,bitPerC)) return false; //Максимальное значение для декодирования
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
    if (nc > conns.count()) return false;     //Ошибка превышения числа связей
    if (bitPerC<2 || bitPerC>16) return false;//Ошибка значения

    if (distance){
      int32_t range = (1<<bitPerC) - 1;
      int32_t pred = 0;
      for (int32_t i=0; i<nc; ++i){  //Для всех весов
        int32_t delta=0;
        int32_t x;
        do {
          if (!from2.readBits((uint32_t&)x,bitPerC)) return false;
          delta += x;
        } while (x == range); //примечание: добавить доп. проверку против зацикливания
        conns[i]=pred+delta;  //!!!!должно каким то образом кодироваться переполнение
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
    //Для режима 32 сжатие не используется
    //примечание: предварительно должна быть выполнена операция digitize
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
      //примечание: Нужно привести к целому числу, отнять 1 и установить старший бит в соответствии с исходным знаком
      //            При этом нулевые значения весов не допускаются
      for (int32_t i=0; i<nw; ++i){
        int32_t x = weights[i];
        if (x>0) x -= 1;//Сформировать непрерывный диапазон значений (если x=0, это значение будет заменено при восстановлении на 1)
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
    //Для режима 32 сжатие не используется
    //примечание: предварительно должна быть выполнена операция digitize
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
        x <<= 32-bitPerW; x >>= 32-bitPerW; //восстановить старшие единичные биты
        weights[i] = x;
      }
    } else {
      //примечание: Нужно привести к целому числу, отнять 1 и установить старший бит в соответствии с исходным знаком
      //            При этом нулевые значения весов не допускаются
      for (int32_t i=0; i<nw; ++i){
        int32_t x;
        if (!from2.readBits((uint32_t&)x,bitPerW)){
          ELOG(5); return false;
        }
        x <<= 32-bitPerW; x >>= 32-bitPerW; //восстановить старшие единичные биты
        //Определить по старшему биту, положительное или отрицательное число
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

  //Конфигурирование сети
  bool Layer::config(const Uuid &id, int32_t nin, int32_t nout, int32_t nconn, LayerFlags lf,  ConnDistribAlg cda,
                      uint8_t bpc, bool fdc,
                      uint8_t bpw, bool fooc){
    //Очистить сеть и создать нейроны
    clear();
    _n.resize(nout);
    _nin = nin;
    _nconn = nconn;
    _lf = lf;
    _cda  = cda;
    _bpc = bpc | ((uint8_t)fdc<<7);
    _bpw = bpw | ((uint8_t)fooc<<7);
    //Сгенерировать связи
    switch (cda){
    case CdaRandom:   //ПРИМЕЧАНИЕ: Для тестирования в случае Random используется передаваемое начальное значение,
                      //но сохраняется вся таблица связей

                      //_salt = id; cgenerate(nin,nconn,_salt); break;
                      _salt = (Uuid)Uuid::createUuid(); cgenerate(nin,nconn,_salt); break;//случайно
    case CdaFixTable: _salt = Uuid::nil; cgenerate(nin,nconn,_salt);  break; //по фиксированной таблице
    case CdaFixAlg:   _salt = id; cgenerate(nin,nconn,_salt); break; //по фиксированному алгоритму
    default:          return false;//не поддерживаются
    }
    return true;
  }


  //Оптимизация хранения после обучения сети в соответствии с заданными флагами
  bool Layer::optimize(){
    //Установить режим сжатия весов (всегда единый для всех нейронов)
    if (!(_bpw & 0x7F)) _bpw = 32;
    //Дискретизировать веса, если необходимо
    for (int i=0; i<_n.count(); ++i){
      if (!_n[i].digitize(_bpw & 0x7F,(_bpw & 0x80) !=0)) return false;
    }

    //Веса можно настраивать и сортировать только в режиме Random
    if (_cda == CdaRandom){
      //Отсортировать связи по номерам
      for (int i=0; i<_n.count(); ++i){
        if (!_n[i].csort()) return false;
        _n[i].zeroback((_lf & LfCOwn)!=0);//Удалить можно только, если число связей нефиксировано
      }
      //Перенести нулевые связи в конце списка, если веса или связи индивидуальны
      //примечание: можно удалить сейчас, если связи индивидуальны
      //            можно удалить во время записи, если веса индивидуальны

      //Теперь можно определить эффективный способ кодирования, если он не задан явно
      if (0==(_bpc &0x7F)){ //предпочитаемый способ кодирования связей не задан
        if (_lf & LfCOwn){  //свой для каждого нейрона
          for (int i=0; i<_n.count(); ++i){
            uint8_t b=0;
            if (-1 == _n[i].cencoding(b,(_bpc & 0x80) != 0,_nin)) return false;
            _n[i].bpc = b | (_bpc & 0x80);
          }
        } else {  //общий для всех нейронов
          uint8_t bmax=0;
          //Проверить для всех и выбрать среди них максимальный среди минимальных,
          //чтобы любой вектор связей мог быть закодирован
          for (int i=0; i<_n.count(); ++i){
            uint8_t b=0;
            if (-1 == _n[i].cencoding(b,(_bpc & 0x80) != 0,_nin)) return false;
            if (b > bmax) bmax = b;
          }
          _bpc = bmax | (_bpc & 0x80);
        }
      } else {
        if (_lf & LfCOwn){ //установить способ для всех нейронов
          for (int i=0; i<_n.count(); ++i){
            uint8_t b = _bpc & 0x7F;
            if (-1 == _n[i].cencoding(b,(_bpc & 0x80) != 0,_nin)) return false;  //Проверить
            _n[i].bpc = _bpc;
          }
        } else {  //использовать как общий
          for (int i=0; i<_n.count(); ++i){
            uint8_t b = _bpc & 0x7F;
            if (-1 == _n[i].cencoding(b,(_bpc & 0x80) != 0,_nin)) return false;  //Проверить
          }
        }
      }
    }
    return true;
  }


  bool Layer::cgenerate(int32_t imax, int32_t cmax, const Uuid& salt){
    //Инициализировать генератор (лучше вариант с криптографической хэш-функцией)
    uint32_t *s = ((uint32_t*)salt.data());
    RndMT gen(s[0] ^ s[1] ^ s[2] ^ s[3]);
    Matrix m(1,Meta(imax,MfU32,MtDO)); Vbp& v = m.at(0,0);
    //Выделить память под массив случайных индексов
    for (int32_t i=0; i<_n.count(); ++i){
      Neuron &n = _n[i];
      n.conns.resize(cmax);
      gen.rand(imax,cmax,(unsigned long*)v.u32);
      ((Vbp)n.conns.data()).copy(v,sizeof(int32_t)*cmax);
    }
    return true;
  }


  //Заполнение возрастающими сериями значений, начиная с номера, пропорционального длине
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
    //Создать матрицу 1xR32 для выходного кода и конвертировать его туда
    Matrix m(1,Meta(out.meta().count(),MfR32,MtC));
    Matrix::Row sig = m.row(0);
    if (!sig.convert(out)) return false;
    //Добавить веса
    for (int32_t i=0; i<_n.count(); ++i)
      _n[i].weights.resize(_n[i].conns.count());

    //Сгенерировать связи согласно установленному режиму
    if (!trainR32Continuous(_n,own.data(),own.count(),all.data(),all.count(),own.meta().count(),sig.at(0),wta==WtaMinAndSuff)){
      //Удалить веса
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
    //Установить флаг защищенного режима
    _lf |= LfProt;
    return true;
  }



  bool  Layer::sim(const Matrix::Row &in, Matrix::Row &out, bool defended){
    //Matrix m(out.count(),Meta(out.meta().count(),MfR32,MtDO));    //Выделить память под выходной вектор
    //Matrix::Row sig = m.row(0);
    //if (!simR32(_n,sig.count(),in.data(),sig.data(),defended)){    //Симулировать
    //  return false;
    //}
    //return out.convert(sig);                                    //Преобразовать в битовую последовательность
    return simR32I1(_n,out.count(),in.data(),out.data(),defended);
  }


  //Вычисление вероятностей ошибок первого рода по базе "Свой"
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


  //Вычисление по сбалансированной базе "все Чужие" на основе распределений мер Хэмминга
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
    //слой::=[nin:2][nout:2][флаги lf:1]
    //            [смещения "все Чужие"][дисперсия "все Чужие]
    //            [cda:1][ncmax:1][bpc|fdc:1][связи нейрона]{nout}
    //            [nwmax:1][bpw|fooc:1][веса нейрона]{nout}
    //            [вектор поправок]
    //            [алг защиты]
    //  lf флаги::= [cfix|wfix|mdinc|binc||binc|prot:1]
    //  связи нейронов::== а) {[nc:1][bpc|fdc]:1,если cfix=0}[..связи..],если cda=другие | б) [uuid],если cda=FixAlg | а) {nil}, если cda=FixTable
    //  веса нейронов::=={[nw:1 не исп=0][[bpw|fooc:1],если wfix=0}[..веса..]
    //  алг защиты::=[алгоритм=0x00]
    //  вектор поправок::=[bpw|fooc:1][bmax:4][.. смещения ..]{nout}  //НЕ ПОДДЕРЖИВАЕТСЯ
    //  смещения ::=[bpw|fooc:1][mmax:4][.. смещения ..]{nin}         //НЕ ПОДДЕРЖИВАЕТСЯ
    //  дисперсия::=[bpw|fooc:1][dmax:4]{nin}                         //НЕ ПОДДЕРЖИВАЕТСЯ
    //
    //nin   - число входов нейронов (2 байта)
    //nout  - число выходов (2 байта)
    //ncmax - максимальное число связей (2 байта) общее
    //nwmax - максимальное число связей (2 байта) общее
    //nw    - число весов i-го нейрона
    //nс    - число связей и нейрона i-го
    //
    //bpc - бит на связь (для каждого нейрона или для всей сети) (до 0x0F)
    //fdc - признак использования динамического кодирования (для каждого нейрона или для всей сети)
    //bpw - бит на вес (для каждого нейрона или для всей сети) (до 0x1F)
    //fooc- признак использования "0" (для каждого нейрона или для всей сети)

    if (!_n.count()) { ELOG(1); return false;}
    //Запись заголовка
    pub<<(uint16_t)_nin;        //число входов
    pub<<(uint16_t)_n.count();  //число выходов
    pub<<(uint8_t)_lf;          //флаги, определяющие включение отдельных блоков слоя

    //Запись параметров нормировки
    if (_lf & LfMdInc) { ELOG(2); return false; }

    //Запись связей
    pub<<(uint8_t)_cda;         //режим генерации связей
    pub<<(uint8_t)_nconn;       //число связей на нейрон (максимальное)
    pub<<(uint8_t)_bpc;         //режим кодирования связей (общий)
    //qDebug()<<"WRITE CON bpc="<<_bpc<<" nc="<<_nconn;
    switch (_cda){
    case CdaFixTable: //не требует сохранения
      break;
    case CdaFixAlg:   //сохраняется только идентификатор алгоритма, например, 16 байт (128 бит)
      if (-1 == pri.writeRawData((const char*)_salt.data(),16)) { ELOG(3); return false; }
      break;
    case CdaRandom:   //сохраняется полная таблица связей
      for (int32_t i=0; i<_n.count(); ++i){
        const Neuron &n = _n[i];
        uint8_t nc1, bpc1;
        if (_lf & LfCOwn){
          nc1  = n.conns.count();
           bpc1 = n.bpc;
          pri<<nc1;   //число связей нейрона
          pri<<bpc1;  //режим кодирования весов
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

    //Запись весов
    //qDebug()<<"WRITE WEI bpw="<<_bpw<<" nw="<<_nconn;
    pub<<(uint8_t)_nconn;//зарезервировано
    pub<<(uint8_t)_bpw; //режим кодирования весов (общий)
    if (_lf & LfWOwn){  //кодирование собственной длиной и индивидуальным форматом сжатия
      for (int32_t i=0; i<_n.count(); ++i){
        const Neuron &n = _n[i];
        int32_t nw1 = n.wnotzeros();  //Определить число нулевых
        pub<<(uint8_t)nw1;  //число весов нейрона
        //qDebug()<<"WRITE WEI1 bpw1="<<_bpw<<" nw1="<<n.weights.count();
        if (!n.wencode(pub,nw1,_bpw & 0x7F, (_bpw & 0x80) != 0)) { ELOG(5); return false; }
      }
    } else {  //использование число связей, и единый формат сжатия
      for (int32_t i=0; i<_n.count(); ++i){
        const Neuron &n = _n[i];
        uint8_t nw = (_lf & LfCOwn)?(uint8_t)n.conns.count():(uint8_t)_nconn;
        //qDebug()<<"WRITE WEI2 bpw1="<<_bpw<<" nw1="<<nw;
        if (!n.wencode(pub,nw,_bpw & 0x7F, (_bpw & 0x80) != 0)) { ELOG(6); return false; }
      }
    }

    //Запись параметров смещений
    if (_lf & LfBInc) { ELOG(7); return false; }

    //Запись алгоритма защиты
    if (_lf & LfProt){
      pub<<(uint8_t)0x00; //Алгоритм по умолчанию
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

    //Считать заголовок
    pub>>nin;  _nin = nin;  //число входов
    pub>>nout;              //число выходов
    _n.resize(nout);        //Установить число нейронов
    pub>>(uint8_t&)lf;   _lf = lf;    //флаги, определяющие включение отдельных блоков слоя

    //qDebug()<<"READ nin="<<nin<<" nout="<<nout<<" lf="<<lf;

    //Приверить неподдерживаемые флаги
    if (lf & ~LayerFlags(LfCOwn | LfWOwn | LfProt)) { ELOG(1); return false; }

    //Загрузка или генерация связей по заданному алгоритму
    pub>>(uint8_t&)cda;   _cda = cda;    //режим генерации связей
    pub>>nconn; _nconn = nconn;//число связей на нейрон (максимальное)
    pub>>bpc;   _bpc = bpc;    //режим кодирования связей (общий)

    //qDebug()<<"READ cda="<<cda<<" nconn="<<nconn<<" lf="<<_lf<<" bpc="<<bpc;

    switch (cda){
    case CdaFixTable: //Восстановить таблицу связей
      if (!cgenerate(nin,nconn,Uuid::nil)) { ELOG(2); return false; }
      break;
    case CdaFixAlg:   //Считать идентификатор для восстановления таблицы связей
      if (-1 == pri.readRawData((char*)id.data(),16)) { ELOG(3); return false; }
      //qDebug()<<"READ id="<<id;
      _salt = id;
      if (!cgenerate(nin,nconn,id)) { ELOG(4); return false; }
      break;
    case CdaRandom:   //Считать полную таблицу связей
      for (int32_t i=0; i<nout; ++i){
        Neuron &n = _n[i];
        uint8_t nc1, bpc1;
        if (lf & LfCOwn){
          pri>>nc1;   //число связей одного нейрона
          pri>>bpc1;  //режим кодирования весов одного нейрона
        } else {
          nc1 = nconn; bpc1= bpc;
        }
        n.conns.resize(nc1);  //Создать связи нейрона и распаковать их
        n.bpc = bpc1;
        //qDebug()<<"READ CON bpc1="<<bpc1<<" nc="<<nc1;
        if (!n.cdecode(pri,nc1,bpc1 & 0x7F,(bpc1 & 0x80) != 0,_nin)) { ELOG(5); return false; }
      }
      break;
    default: { ELOG(6); return false; }
    }
    //qDebug()<<"READ CONNS SUCCESS";

    //Загрузка весов
    uint8_t nweights;
    pub>>nweights;        //зарезервировано
    pub>>bpw; _bpw = bpw; //режим кодирования весов (общий)
    //qDebug()<<"READ WEI bpw="<<bpw<<" nw="<<nweights;
    if (lf & LfWOwn){  //кодирование собственной длиной и индивидуальным форматом сжатия
      for (int32_t i=0; i<_n.count(); ++i){
        uint8_t nw1;
        Neuron &n = _n[i];
        pub>>nw1;  //число весов нейрона
        //qDebug()<<"READ WEI1 bpw1="<<bpw<<" nw1="<<nw1<<" nc1="<<n.conns.count();
        //Создать веса и обнулить их
        n.weights.resize(n.conns.count());
        for (int32_t j=0; j<n.weights.count(); ++j)
            n.weights[j]=0.0f;
        if (!n.wdecode(pub,nw1,bpw & 0x7F, (bpw & 0x80) != 0)) { ELOG(7); return false; }
      }
    } else {  //использование число связей, и единый формат сжатия
      for (int32_t i=0; i<_n.count(); ++i){
        Neuron &n = _n[i];
        uint8_t nw1  = n.conns.count();
        uint8_t bpw1 = bpw;
        //qDebug()<<"READ WEI2 bpw1="<<bpw1<<" nw1="<<nw1;
        n.weights.resize(nw1);  //Создать веса
        if (!n.wdecode(pub,nw1,bpw1 & 0x7F, (bpw1 & 0x80) != 0)) { ELOG(8); return false; }
      }
    }
    //Т.к. число весов может быть меньше числа связей, выполнить сведение размеров таблицы связей
    for (int32_t i=0; i<_n.count(); ++i){
      Neuron &n = _n[i];
      if (n.weights.count() > n.conns.count()) n.conns.resize(n.weights.count());
    }


    //qDebug()<<"READ WEIGHTS SUCCESS";
    //Загрузка алгоритма защиты
    if (lf & LfProt){
      uint8_t alg;
      pub>>alg;
      if (alg != 0) { ELOG(9); return false;}
    }
    return true;
  }



#if !defined(QT_NO_DEBUG_STREAM)

  //Данные нейрона
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

  //Данные слоя
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
