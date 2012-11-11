/****************************************************************************
** файл:        nbcTypes.h
** версия:      1.0
** дата:
** изменен:
** автор:       Майоров А.В.
**
** описание:    Классы для основных типов данных НПБК.
****************************************************************************/

#ifndef _NBC_TYPES_H
#define _NBC_TYPES_H

#include <qstring.h>
#include <qvector.h>

#include "..\nb\nb.h"

#pragma pack(1)

using namespace std;

// Тело блока безопасности (один из вариантов)
typedef struct nbCrcBody_t {
  int16_t   zone;     // число контроллируемых блоков и их расположение относительно блока безопасности
  uint16_t  owner;    // собственник = 0
  uint16_t  format;   // формат = 0
  uint8_t   crc[16];  // контрольная сумма
} nbCrcBody;

//...........................................................................
//  Уникальный универсальный идентификатор
//...........................................................................

bool operator ==(const nbUuid &id1, const nbUuid &id2);
bool operator !=(const nbUuid &id1, const nbUuid &id2);

class NbUuid:public nbUuid {
public:
  NbUuid();
  NbUuid(const nbUuid &id);
  
  // Генерация идентификатора, связанного с вычислительной машиной
  bool generate();
  // Генерация нулевого идентификатора
  void erase();
  // Проверка на нулевое значение
  bool nil() const;

  // Преобразование в строку
  QString toString() const;
  // Преобразование из строки
  bool fromString(const QString &s);
};

// Вектор идентификаторов
typedef QVector<NbUuid> NbUuids;

//...........................................................................
//  Блок данных
//...........................................................................
class NbData {
public:
  // Конструктор
  NbData();
  NbData(uint32_t size);
  NbData(const nbData data, bool isAttached=true);
  NbData(const NbData &source);
  NbData(const QString &s);
  // Деструктор
  virtual ~NbData();

  operator nbData() const { return _data; }
  
  NbData& operator =(const NbData &source) { copyFrom((nbData)source); return *this; }

  bool operator==(const NbData &d) const { return (0==compare(d)); }
  bool operator!=(const NbData &d) const { return (0!=compare(d)); }
      
  // Присоединение блока
  //  - нельзя использовать resize для присоединенного блока
  void attach(nbData data);
  // Отсоединение блока
  nbData detach();
  
  // Копирование блока данных из nbData
  void copyFrom(const nbData data);
  // Копирование блока данных из массива определенной длины
  void copyFrom(const void* data, uint32_t size);
  // Копирование блока данных в массив
  bool copyTo(void* data, uint32_t size) const;
 
  bool empty() const;
  // Освобождение блока данных
  void free();
  // Освобождение блока данных, даже если он присоединен
  // (корректно работает, если присоединенный блок был отсоединен от nbData функцией detach)
  void freeForced();
  
  // Изменение размера блока
  //  - для присоединенных блоков размер может изменяться только в сторону уменьшения
  bool resize(uint32_t size);
  // Установка в поле "размер" желательного размера блока без фактического изменения размера
  void setWishfulSize(uint32_t size);
  
  // Размер блока
  uint32_t size() const;

  // Преобразование данных, представляющих строку utf-8, в строку utf-16
  QString toString() const;

  // Преобразование данных из строки utf-16 в строку utf-8
  bool fromString(const QString &s);
  // Определение текущей длины строки в символах wchar
  uint32_t getStringLength() const;
  
  // Получение желаемого размера строки
  uint32_t getWishfulSize() const;
  // Емкость
  uint32_t capacity() const { return _capacity; }
  // Изменение емкости
  bool reserve(uint32_t capacity);

  // Сравнение (0 (c==d), >0 (c>d), <0 (c<d))
  int32_t compare(const NbData &d) const;

protected:

  // Установка терминала в указанную позицию
  void setDataTerminal(uint32_t pos){ if (_data) { _data[pos] = 0; _data[pos+1] = 0; } }

  nbData    _data;        // прикрепленная строка
  uint32_t  _capacity;    // емкость буфера в байтах (без учета длины строки и терминальных нулей)
  bool      _isAttached;
};

//.............................................................................
//  Метаинформация ВБП
//.............................................................................
class NbMeta: public nbMeta {
public:
  NbMeta();
  NbMeta(const nbMeta &mi);
  NbMeta(uint32_t count, nbMetaFormat format, nbMetaType type);

  bool operator==(const NbMeta &mi) const { return equal(mi); }
  bool operator!=(const NbMeta &mi) const { return !equal(mi); }
  //Проверка на равенство
  bool equal(const NbMeta &mi) const;
  //Проверка, является ли метаописание подмножеством range
  bool subrange(const NbMeta &range) const;
  //Проверка содержит ли метаописание диапазон
  bool ranged() const;
  //Метаописание переменное по типу
  bool anyType() const;
  //Метаописание переменное по числу элементов
  bool anyCount() const;
  //Метаописание переменное по формату
  bool anyFormat() const;
  //Проверка на неопределенное значение
  bool any() const;
  
  static const NbMeta nil();
  
  //Длина элемента ВБП в битах
  uint32_t sizeItem() const;
  //Длина ВБП в байтах с выравниванием по байту
  uint32_t size() const;
};  

typedef QVector<NbMeta> NbMetas;

//.............................................................................
struct NbBlockHeader: public nbBlockHeader {
  NbBlockHeader(nbBlockHeader bh);
  uint32_t    bodySize() const;
  void        bodySize(uint32_t size);
  nbBlockType type() const;
  void        type(nbBlockType bt);
};


//.............................................................................
//  ВБП
//  - позволяет интерпретировать указатель на блок данных как массив элементов
//    заданного типа
//  - позволяет работать с ВБП, представленными в виде набора бит
//.............................................................................
union Vbp {
public:
  //Прямой доступ к элементам через типизированный указатель
  nbVbp     v;
  int8_t    *i8;
  int16_t   *i16;
  int32_t   *i32;
  int64_t   *i64;
  uint8_t   *u8;
  uint16_t  *u16;
  uint32_t  *u32;
  uint64_t  *u64;
  float     *r32;
  double    *r64;
public:
  Vbp():v(nbNULL){}
  Vbp(const nbVbp &vec):v(vec){}
  inline bool create(uint32_t size) { free(); u8 = new uint8_t[size]; if (!u8) return false; }
  inline void free(){ if (u8) { delete[] u8; u8 = nbNULL; }}

  operator nbVbp() const { return v; }
  Vbp& operator =(const nbVbp& vec){ v=vec; return *this; }
  bool empty(){ return (v==nbNULL); }
  static Vbp null(){ static Vbp vec(nbNULL); return vec; }
  
  Vbp shift(int32_t size) const { return (Vbp)((uint8_t*)v + size); }
  //Функции чтения и записи для работы с элементами из 1,2,4 бит,
  //интерпретирующимися как беззнаковые

  uint32_t u1(uint32_t pos) const;
  void     u1(uint32_t pos, uint32_t val);
  uint32_t u2(uint32_t pos) const;
  void     u2(uint32_t pos, uint32_t val);
  uint32_t u4(uint32_t pos) const;
  void     u4(uint32_t pos, uint32_t val);

  //Функции чтения и записи для работы с элементами из 1,2,4 бит,
  //интерпретирующимися как знаковые

  int32_t  i1(uint32_t pos) const;          //возвращает -1 и 1
  void     i1(uint32_t pos, int32_t val);   //устанавливает >0 или <=0

  //Функции преобразования основных форматов представления

  void convU1fromR32(const Vbp &vec, uint32_t count);
  void convR32fromU1(const Vbp &vec, uint32_t count);
  void convI1fromR32(const Vbp &vec, uint32_t count);
  void convR32fromI1(const Vbp &vec, uint32_t count);
  void convU16fromR32(const Vbp &vec, uint32_t count);
  void convR32fromU16(const Vbp &vec, uint32_t count);
  void convI16fromR32(const Vbp &vec, uint32_t count);
  void convR32fromI16(const Vbp &vec, uint32_t count);
  void convI32fromR32(const Vbp &vec, uint32_t count);
  void convR32fromI32(const Vbp &vec, uint32_t count);
  void convU32fromR32(const Vbp &vec, uint32_t count);
  void convR32fromU32(const Vbp &vec, uint32_t count);

  bool conv(const Vbp &vec, uint32_t count, nbMetaFormat mfTo, nbMetaFormat mfFrom);
  void copy(uint8_t *data, uint32_t size);
};

//...........................................................................
//  Матрица ВБП
//...........................................................................
class NbMatrix: public nbMatrix {
public:
  //Ссылка на вектор матрицы
  class Row {
  public:
    Row():
      _ref(nbNULL),_irow(0),_handle(nbNULL){}
    Row(const Row &row):
      _ref(row._ref),_irow(row._irow),_handle(row._handle){}
    Row(const nbMatrix *m, uint32_t irow, nbHandle handle=nbNULL):
      _ref(m),_irow(irow),_handle(handle){}
    //Конструктор с выделением внутренней памяти (память должна освобождаться)
    Row(uint32_t ncols, NbMeta meta):_ref(nbNULL),_irow(0),_handle(nbNULL){
      NbMatrix *m = new NbMatrix(ncols,meta);
      _ref  = m; _irow = 0; _handle = (nbHandle)m;
    }
    //Изменение размеров, если это возможно (т.е. матрица определена классом NbMatrix)
    bool resize(uint32_t ncols, NbMeta meta);
    //Освобождение связанной матрицы
    void free();

    Vbp operator [](uint32_t icol) const { return (Vbp)_ref->data[_irow][icol]; }
    Vbp& operator [](uint32_t icol) { return (Vbp&)_ref->data[_irow][icol]; }
    Vbp* operator*() const { return (Vbp*)_ref->data[_irow]; }
    bool empty() const { return (!_ref); }

    Vbp    at(uint32_t icol) const { return ((Vbp**)_ref->data)[_irow][icol]; }
    Vbp&   at(uint32_t icol) { return ((Vbp**)_ref->data)[_irow][icol]; }

    Vbp* data() const { return (Vbp*)_ref->data[_irow]; }
    NbMeta meta() const { return (NbMeta)_ref->meta[_irow]; }
    uint32_t count() const { return _ref->ncols; }
    const nbMatrix* ref() const { return _ref; }

    nbHandle  handle() const { return _handle; }
    void clear(){ _ref=nbNULL; _irow=0; _handle=nbNULL; }
    
    bool copy(const Row &row);
    bool convert(const Row &row);

    //Заполнение вектора нулями
    void tideup();

  protected:
    const nbMatrix *_ref;     // ссылка на матрицу
    uint32_t  _irow;          // номер строки
    nbHandle  _handle;  // вспомогательный описатель (например аллокатор)
  };

public:
  
  NbMatrix();
  //Конструктор полной матрицы
  NbMatrix(uint32_t nrows, uint32_t ncols, const NbMeta *meta);
  //Конструктор для вектора
  NbMatrix(uint32_t ncols, const NbMeta meta);
  //Конструктор с конвертацией формата
  NbMatrix(const NbMatrix &m, nbMetaFormat f);
  //Конструктор с конвертацией формата
  NbMatrix(const NbMatrix::Row &r, nbMetaFormat f);

  ~NbMatrix();

  Vbp* operator[](uint32_t irow) const { return (Vbp*)nbMatrix::data[irow]; }

  Row    row(uint32_t irow) const;
  Vbp    at(uint32_t irow, uint32_t icol) const;
  Vbp&   at(uint32_t irow, uint32_t icol);

  Vbp** data() const;
  uint32_t ncols() const;
  uint32_t nrows() const;
  NbMeta* meta() const;
  NbMeta  meta(uint32_t irow) const;
  //Максимальная емкость
  uint32_t capacity() const;

  bool valid() const;    //Проверка поддержки типа матрицы (nrows < 65536)
  bool empty() const;    //Проверка пустоты
  bool pattern() const;  //Проверка наличия шаблона
  bool full() const;     //Проверка на полную матрицу

  //Выделение памяти под метаописание
  bool resizeMeta(uint32_t nrows, const NbMeta *meta);
  //Выделение памяти под данные
  bool resizeData(uint32_t ncols);
  //Перевыделение памяти под строку при изменении метаданных 
  //(память под метаданные и данные должна быть выделена ранее)
  bool resizeVbp(uint32_t irow, NbMeta meta);
  //Освобождение матрицы (аналогична freeMeta)
  void free();
  //Освобождение метаописаний (и данных)
  void freeMeta();
  //Освобождение данных
  void freeData();

  bool copyVbp(uint32_t irow, const Row& v); //Копирование по ссылке на вектор (память не перевыделяется)
  bool copyVbp(const nbMatrix &m);           //Копирование матрицы (память не перевыделяется)

  bool copy(const nbMatrix &m);  //  Копирование матрицы

  //Конвертация матрицы из одного формата в другой
  //(число векторов и число элементов вектора должны совпадать)
  bool convert(const NbMatrix &m);

protected:
  void freeVbp(uint32_t irow);
private:
  uint32_t _capacity;
};

//...........................................................................
//  Идентификатор слота
//...........................................................................
class NbSlotID {
public:
  NbSlotID():_id(nbSLOT_OUT){}
  NbSlotID(nbSlotID id):_id(id){}
  operator nbSlotID() const { return _id; }

  uint16_t    type() const   { return _id & nbSLOT_MASK_TYPE; }
  uint16_t    index() const  { return _id & nbSLOT_MASK_INDEX; }
  uint8_t     bindex() const { return (uint8_t)(_id & nbSLOT_MASK_INDEX);}
  nbBlockType btype() const  { return (nbBlockType)((_id & nbSLOT_MASK_BTYPE)>>7); }
  //Быстрое формирование номера
  NbSlotID inline static in(uint16_t i) { return NbSlotID(nbSLOT_IN(i)); }
  NbSlotID inline static conv(nbBlockType bt, uint8_t i) { return NbSlotID(nbSLOT_CONV(bt,i)); }
  NbSlotID inline static convDef(nbBlockType bt){ return NbSlotID(nbSLOT_CONV_DEFAULT(bt)); }
  NbSlotID inline static specIn(uint16_t i){ return NbSlotID(nbSLOT_SPEC_IN(i)); }
  NbSlotID inline static specOut(uint16_t i){ return NbSlotID(nbSLOT_SPEC_OUT(i)); }
  static const NbSlotID out;

  //Быстрое изменение типа
  NbSlotID inline static retype(NbSlotID id, uint16_t st){ return NbSlotID(st | id.index()); }
protected:
  nbSlotID _id;
};

typedef QVector<NbSlotID> NbSlotIDs;

#pragma pack()
#endif
