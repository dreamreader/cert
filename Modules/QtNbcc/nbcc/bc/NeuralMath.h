#ifndef NEURAL_MATH_H
#define NEURAL_MATH_H

#include <QtCore/QVector>
#include "../nb/nbc.h"

namespace Bc {
  using namespace Nb;


  struct Range {
    Range():left(0.0f),right(0.0f){}
    Range(float l, float r):left(l),right(r){}

    float left;  //левая (минимальная граница)
    float right; //правая граница (максимальная)

    bool in(float x) const;
    bool inl(float x) const;
    bool inr(float x) const;
    bool inlr(float x) const;

    bool valid() const;
    bool  intersect(const Range &range) const;  //Проверка пересечения двух интервалов
    static Range intersect(const Range &a, const Range &b);//Пересечение интервалов. Корректность проверяется valid()
    float dist() const;
    float dist(float x) const;  //Расстояние до дочки вне интервала (или 0.0f)
    //bool unite(range,range) - формируется единый интервал или false, если не имеют общей части для объединения
    //swap()
    //bool exclude(range) - исключение общей части (если формируется 2 подынтервала, нужно сообщить
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
  //Нейрон
  //примечание: отдельно настраиваются связи и отдельно настраиваются веса
  //-----------------------------------------------------------------------------------
  struct Neuron {
    float b;                //смещение (использовать нежелательно)
    QVector<float> weights; //вектор весов
    QVector<int32_t> conns; //вектор связей

    uint8_t bpc;  //бит на связь + флаг кодирования

    Neuron():b(0.0f),bpc(16){}
    int32_t nconns() const;

    void clear(){
      b=0.0f;
      weights.clear();
      conns.clear();
    }

    bool isEmpty() const;
    bool isTrained() const;

    //Сортировка связей (и их весов) по возрастанию
    //примечание: число весов должно быть равно числу связей или 0
    bool csort();
    //Перемещение нулевых весов (и их связей) в конец
    //примечание: при этом последовательность номеров связей сохраняется для ненулевых элементов
    void zeroback(bool andTrim);
    //Число ненулевых элементов в конце списка
    int32_t wnotzeros() const;

    //Снижение разрядности весов
    //  bitPerW - устанавливаемое отношение бит на вес
    //  originOfCoord - с использованием значения веса "0" или без него
    //        если true,  то используется вариант (.., {-2}, -1, 0, 1, {2}, ..) n=2^bitPerW-1
    //        если false, то используется вариант (.., -2, -1, 1, 2, ..) n=2^bitPerW
    //примечание: если имеются w=0 для originOfCoord=false,
    //            то их нельзя закодировать и они должны быть удалены к моменту кодирования
    //            (или можно привести к диапазону с нулем, увеличив число бит на 1 и на originOfCoord=true)
    //возвращает: длину цепочки
    bool digitize(uint8_t bitPerW = 32, bool originOfCoord=true);

    //Определение кодировки с минимальным числом бит на связь (от 2 до 16) или проверка поддержки кодировки
    //  bitPerC   - проверяемое отношение бит на связь (устанавливается в минимально возможное значение или не меняется)
    //возвращает:
    //  -1 в случае невозможности кодировки
    //  0, если допустима любая кодировка
    //  x бит, если любая не меньше указанной в bitPerC
    int32_t cencoding(uint8_t &bitPerC, bool distance, int32_t wmax);

    //Кодирование связей в поток
    bool cencode(QDataStream &to, int32_t nc, uint8_t bitPerC, bool distance, int32_t wmax) const;
    //Декодирование связей из потока
    bool cdecode(QDataStream &from, int32_t nc, uint8_t bitPerC, bool distance, int32_t wmax);

    //Кодирование весов в поток
    //примечание: предварительно должна быть выполнена операция digitize
    bool wencode(QDataStream &to, int32_t nw, uint8_t bitPerW, bool originOfCoord) const;
    //Декодирование связей из потока
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
  //Слой искусственной нейронной сети преобразователя биометрия-код
  //-----------------------------------------------------------------------------------
  class Layer {
  public:
    enum LayerFlagsEnum {
      LfCOwn  = 0x01, //собственное число связей у нейронов
      LfWOwn  = 0x02, //собственное число весов у нейронов (если LfWOwn=false, используются связи нейрона)
      //0x03
      LfBInc  = 0x04, //включена таблица поправок (нужны абсолютные значения, т.е. 4*416 байт или 0, т.к. поправки маленькие)
      LfMdInc = 0x08, //включена таблица смещений "все Чужие" (нужны абсолютные значения, т.е. 4*416 байт)
      //        и таблица дисперсий "все Чужие" (нужны абсолютные значения, т.е. 4*416 байт)
      LfProt  = 0x80  //включена информация об алгоритме защиты сети
    };
    typedef packed_flags<LayerFlagsEnum,uint8_t>    LayerFlags;

    //Режимы распределения связей (обучения)
    enum ConnDistribAlgEnum {
      CdaRandom=0,        //случайным образом (одинаковое число связей) Uuid::generateUuid
      CdaFixTable=1,      //фиксированная таблица (одна для всех) Uuid::nil
      CdaFixAlg=2,        //фиксированный алгоритм распределения, задается с помощью Uuid
      CdaFixByQual=3,     //статическое распределение по вх. качеству (и корреляции "Свой")
      CdaFixByCorr=4,     //статическое распределение по качеству и корреляции "Все чужие" с фиксированным числом
      CdaDynByQual=5,     //динамическое по вых. качеству и вх. корреляции (с выполнением оптимизации)
      CdaDynByCorr=6      //динамическое по вых. качеству и вх. корреляции (с выполнением оптимизации)
    };
    typedef packed_enum<ConnDistribAlgEnum,uint8_t>    ConnDistribAlg;

    enum WeightTrainAlgEnum {
      WtaFullUsing = 0,   //полное использование связей
      WtaMinAndSuff = 1   //настраиваются первые связи, обеспечивающий необходимый результат
    };
    typedef packed_enum<WeightTrainAlgEnum,uint8_t>   WeightTrainAlg;


    #if !defined(QT_NO_DEBUG_STREAM)
    friend QDebug operator<<(QDebug  dbg, const ConnDistribAlg &x);
    friend QDebug operator<<(QDebug  dbg, const WeightTrainAlg &x);
    friend QDebug operator<<(QDebug  dbg, const LayerFlags &x);
    #endif

    Layer();

    void clear();

    //Признак наличия параметров
    bool isEmpty(){ return _n.isEmpty(); }
    bool isTrained(){ return !_n.isEmpty() && _n.at(0).isTrained(); }

    //Функция начального задания весов
    // nin  - число входов
    // nout - число нейронов
    // nconn- число связей нейрона
    // lf   - флаги слоя
    // cda  - режим генерации связей
    // bpc  - бит на связь (0x00 - поиск оптимального для всех или индивидуально)
    // fdc  - кодирование расстояниями
    // bpw  - бит на вес (2..32)
    // fooc - использование "0" при кодировании
    // id   - номер алгоритма генерации связей (для режима FixAlg)
    bool config(const Uuid &id, int32_t nin, int32_t nout, int32_t nconn=24, LayerFlags lf=0x00,  ConnDistribAlg cda=CdaFixTable,
                      uint8_t bpc=16, bool fdc=false,
                      uint8_t bpw=32, bool fooc=false);

    //Оптимизация хранения после обучения сети в соответствии с заданными флагами
    bool optimize();

    //Обучение слоя на непрерывных параметрах для одного вектора
    //  own - примеры "Свой", n ~ 8-32, Meta(a,MfR32,MtC)
    //  all - примеры "Все чужие", m ~ 100, Meta(a,MfR32,MtC)
    //  out - пример выходного кода, 1, Meta(b,MfI1,MtDO), b<=a
    //bool train(const Matrix::Row &own, const Matrix::Row &all, const Matrix::Row &out, WeightTrainAlg wta);
    bool train(const Matrix::Row &own, const Matrix::Row &all, const Matrix::Row &out, WeightTrainAlg wta);


    //Обучение слоя на непрерывных параметрах нескольких векторов
    //примечания:
    //  1.обучение выполняется аналогично 1 вектору, за исключением того, что используется сквозная нумерация связей по векторам
    //    при сохранении в поток и восстановление состояния из него, а в общем случае используется частная адресация
    //  2.по номеру связи можно быстро восстановить (например с помощью &-оператора) номер вх.вектора
    //    и выполнять сортировку аналогично
    //  3.при подстройке примеров для i-й связи, связанной с k-м вх.вектором, out рассчитывается в гипотезе независимости векторов,
    //    т.е. с суммарными значениями для весов, связанными с другими !=k векторами для всех комбинаций векторов.
    //  4.
    //bool train(const Matrix::Rows &owns, const Matrix::Rows &alls, const Matrix::Row &out, WeightTrainAlg wta);


    //Вычисление степени вероятностей ошибок первого рода по базе "Свой"
    bool testP1(const Matrix::Row &own, const Matrix::Row &out, float &p1);

    //Вычисление степени вероятности ошибок второго рода по сбалансированной базе "все Чужие" на основе распределений мер Хэмминга
    bool testP2(const Matrix::Row &all, const Matrix::Row &out, float &p2);

    //Защита параметров преобразования с помощью цепочечного связывания выходного кода
    //  out - пример ключа, 1, Meta(b,MfI1,MtDO)
    bool protect(const Matrix::Row &out);

    //Эмуляция слоя для непрерывных параметров
    //  in - примеры "Любой", n, Meta(a,MfR32,MtC)
    //  out - пример выходного кода, n, Meta(b,MfI1,MtDO)
    bool sim(const Matrix::Row &in, Matrix::Row &out, bool defended);

    //Чтение параметров слоя из потока
    //  pub - открытые параметры (все параметры, если pri не задан)
    //  pri - закрытые параметры (хранимые у пользователя)
    //примечание: в качестве pri можно указать тот же поток
    bool read(QDataStream &pub, QDataStream &pri);
    //Запись параметров слоя в поток
    //  pub - закрытые параметры (все параметры, если pri не задан)
    //  pri - закрытые параметры (хранимые у пользователя)
    bool write(QDataStream &pub, QDataStream &pri) const;

  #if !defined(QT_NO_DEBUG_STREAM)
  friend QDebug operator<<(QDebug  dbg, const Layer &x);
  #endif

  protected:
    //Генерация у каждого нейрона связей в соответствии с установленным алгоритмом
    //imax  - максимальное число входов
    //cmax  - число входов у одного нейрона
    //salt  - число, используемое для начальной инициализации параметров
    bool cgenerate(int32_t imax, int32_t cmax, const Uuid& salt);
    //Заполнение возрастающими сериями значений, начиная с номера, пропорционально длине
    bool cline(int32_t imax, int32_t cmax);

  protected:
    Neurons         _n;     //нейроны nout=_n.count()

    LayerFlags      _lf;    //флаги (индивидуальное кодирование или кодирование по образцу)
    ConnDistribAlg  _cda;   //режим генерации связей

    int32_t         _nin;   //число входов
    int32_t         _nconn; //число связей

    Uuid            _salt;  //идентификатор

    uint8_t _bpc;  //бит на связь + флаг кодирования
    uint8_t _bpw;  //бит на вес + флаг кодирования
  };


  //-----------------------------------------------------------------------------------
  //Математические функции
  //-----------------------------------------------------------------------------------

  //Пороговая функция
  //для [-oo; 0] пороговая функция выдает 0
  //для (0; +oo] пороговая функция выдает 1
  //  sig       - [вх/вых] отклики нейронной сети
  //  nelms     - [вх] число откликов
  void hardlimR32(Vbp &sig, int32_t nelms);

  //Пороговая функция с двумя состояниями
  //для [-oo; 0] пороговая функция выдает -1
  //для (0; +oo] пороговая функция выдает 1
  //  o       - [вх/вых] отклики нейронной сети
  //  nelms   - [вх] число откликов
  void sign2R32(Vbp &sig, int32_t nelms);

  //Вычисление выходного качества нейрона
  float qualR32(const Vbp &sig, int32_t nelms);

  //Вычисление стабильности i-го выхода нейрона по двум состояниям [<=0, >0]
  //и преобладающего знака выхода (-1 или 1). Если 0, то -1.
  void stab2stateColR32(const Vbp *inp, int32_t ncols, int32_t nelms, Vbp &stab, Vbp &sgn);

  //Проверка попадания в заданный интервал
  // описание:
  //    - для выходов сумматоров рассчитывается матожидание и дисперсия
  //			 попавших в требуемый диапазон
  //		- если диапазон не задан, то работаем с диапазоном от -3сигма до 3сигма.
  // sig    - [вх] выходные значения нейрона, снятые путем симуляции
  // ncols  - [вх] число векторов
  // interv - [вх] диапазон значений (массив из двух чисел: мин., макс. значения выхода нейрона
  int32_t hittestR32(const Vbp &sig, int32_t nelms);

  int32_t sigtestR32(const Vbp &sig, int32_t nelms, float bit);

  //Вычисление суммы выхода нейрона (без пороговой функции)
  //  n       - [вх] нейрон
  //  inp     - [вх] набор примеров
  //  ncols   - [вх] число примеров
  //  sig     - [вых]вектор откликов с числом элементов ncols
  bool mulsumR32(const Neuron &nr, const Vbp *inp, int32_t ncols, Vbp &sig);


  //Моделирование работы слоя ИНС на параметрах (n,R32,C) и результате (m,R32,C)
  //примечание:
  //  - используется пороговая функция активации sign2R32 [-1,1].
  //  l     - [вх] слой нейронной сети
  //  ncols - [вх] число примеров
  //  nelms - [вх] число элементов во входном векторе
  //  inp   - [вх] набор векторов входных сигналов
  //  sig   - [вых]набор векторов выходных откликов слоя
  //  defended - [вх] используется перемешивание на предыдущих битах
  bool simR32(const Neurons &l, int32_t ncols, const Vbp *inp, Vbp *sig, bool defended);


  //Моделирование работы слоя ИНС на параметрах (n,R32,C) и результате (m,I1,DC)
  //  l     - [вх] слой нейронной сети
  //  ncols - [вх] число примеров
  //  nelms - [вх] число элементов во входном векторе
  //  inp   - [вх] набор векторов входных сигналов
  //  sig   - [вых]набор векторов выходных откликов слоя
  //  defended - [вх] используется перемешивание на предыдущих битах
  bool simR32I1(const Neurons &l, int32_t ncols, const Vbp *inp, Vbp *sig, bool defended);

  //Генерация таблицы связей нейронов непрерывных параметров
  //параметры:
  //	l  - [вх/вых] слой
  //  nelms  - [вх] число элементов во входном векторе биом. параметров (векторе коэффициентов)
  bool generateConnR32Continuous(Neurons &l, uint32_t nelms);

  //Генерация таблицы связей нейронов дискретных параметров
  //	l  - [вх/вых] слой
  //  nelms  - [вх] число элементов во входном векторе биом. параметров (векторе коэффициентов)
  bool generateConnR32Discrete(Neurons &l, uint32_t nelms);

  //Генерация выходного кода (память должна быть выделена)
  bool generateR32Sig(Vbp &sig, uint32_t nelms);

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
                              Vbp *better, uint32_t &nbetter, Vbp *ge = nbNULL);


  //Обучение первого слоя ИНС (число связей каждого нейрона = 5% от числа входов)
  //примечание:
  //  - обучение заключается в настройке таблицы весов, а таблицы связей и память
  //    под них должна выделяться заранее
  //  - дополнительно может выставляться диапазон качества по которому осуществляется загрубление
  //    результата
  //	l       - [вх/вых] слой
  //  own     - [вх] обучающие вектора коэффициентов "Свой"
  //  nown    - [вх] число обучающих векторов "Свой"
  //  all     - [вх] обучающие вектора коэффициентов "Все чужие"
  //  nall    - [вх] число обучающих векторов "Все чужие"
  //  nelms   - [вх] число элементов в каждом входном векторе
  //  sig     - [вх] вектор выходного кода (число элементов равно числу нейронов последнего слоя)
  bool trainR32Continuous(  Neurons &l,
                            const Vbp *own, uint32_t nown,
                            const Vbp *all, uint32_t nall,
                            uint32_t nelms,
                            const Vbp &sig);

  //Обучение второго слоя ИНС (число связей каждого нейрона = 50% от числа входов)
  //примечание:
  //  - обучение заключается в настройке таблицы весов, а таблицы связей и память
  //    под них должна выделяться заранее
  //  - на основе входных примеров вычисляется входное качество примеров а затем усредняется
  //	l       - [вх/вых] слой
  //  own     - [вх] обучающие вектора коэффициентов "Свой"
  //  nown    - [вх] число обучающих векторов "Свой"
  //  all     - [вх] обучающие вектора коэффициентов "Все чужие"
  //  nall    - [вх] число обучающих векторов "Все чужие"
  //  nelms   - [вх] число элементов в каждом входном векторе
  //  sig     - [вх] вектор выходного кода (число элементов равно числу нейронов последнего слоя)
  bool trainR32Discrete(   Neurons &l,
                           const Vbp *own, uint32_t nown,
                           const Vbp *all, uint32_t nall,
                           uint32_t nelms,
                           const Vbp &sig);

  //Перевод слоя ИНС в защищенный режим
  //  выполняется умножение весов каждого нейрона на отклик предыдущего нейрона (кода)
  bool trainR32Protected( Neurons &l, const Vbp &sig);


}



#endif // BC_MATH_NEURAL_H
