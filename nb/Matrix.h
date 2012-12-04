//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Объявление класса матрицы векторов биометрических
//              параметров
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_MATRIX_H
#define _NB_MATRIX_H

#include <QtCore/QDataStream>
#include <QtCore/QList>

#include "Enums.h"
#include "Meta.h"
#include "Vbp.h"

#pragma pack(1)

namespace Nb {

//...........................................................................
//  Матрица ВБП
//...........................................................................
class Matrix {
public:

  class Row;
  typedef QList<Matrix::Row> Rows;

  enum UniteType {
    UtByVbp,    //объединение/разделение данных (1,2,3)...(1,2,3)
    //  метаописания rows, кроме count, и число примеров должны совпадать
    UtByIndex,  //объединение/разделение по номерам rows 1,2,3,...,1,2,3
    //  метаописания rows и число примеров должны совпадать
    UtByOrder   //объединение/разделение по порядку rows 1,1,1,...,3,3,3
    //  метаописания rows и число примеров должны совпадать
  };
  //Тип представления матрицы
  enum MatrixType {
    MtEmpty     = 0x00, //все поля 0
    MtTemplate  = 0x01, //мета не ноль
    MtFull      = 0x02, //мета и данные не ноль
    MtGuid      = 0x03, //идентификатор интерфейса (зарезервирован)
    MtPointer   = 0x04  //указатель на интерфейс (зарезервирован)
  };

  //Ссылка на вектор матрицы
  //примечания:
  //  1. ссылаемая матрица не должна освобождаться, пока имеются ссылки на нее
  //  2. осторожно следует использовать ссылки на разные столбцы одной матрицы,
  //      в режиме "изменение размера", "изменение метаописания"
  //  3. операции "чтения/записи" данных требуют явного вызова функций изменения размера или метаописания
  class Row {
  public:
    Row();                                      //Конструктор пустой ссылки
    Row(const Row &row);                        //Конструктор копирования ссылки
    Row(Matrix &m, int32_t irow);               //Конструктор ссылки на матрицу
    Row(const nbMatrix *m, int32_t irow);       //Конструктор ссылки на неизменяемую матрицу

    Vbp*    operator*() const;

    Vbp*    data() const;                       //Данные
    Meta    meta() const;                       //Метаописание
    int32_t count() const;                      //Число примеров

    Vbp    at(int32_t icol) const;              //Доступ по номеру примера
    Vbp&   at(int32_t icol);                    //Доступ по номеру примера

    Vbp operator [](int32_t icol) const;        //Доступ по номеру примера
    Vbp& operator [](int32_t icol);             //Доступ по номеру примера

    Row& operator=(const Row& row);             //Оператор присваивания
    Row ref() const;                            //Оператор создания ссылки

    bool isNull() const;
    bool isEmpty() const;   //Проверка наличия данных
    bool isLocked() const;   //Проверка ссылочности (доступ в режиме "чтение/запись" данных)

    //Выделение памяти (выделение производится под все примеры)

    bool resize(int32_t ncols);                 //Выделение памяти по числу примеров
    bool resize(int32_t ncols, Meta meta);      //Выделение памяти по числу примеров и метаописанию

    void clear();         //Очищение ссылки (данные не удаляются)
    void tideUp();        //Обнуление данных

    //Копирование всех примеров (row.count <= count)
    //примечание:
    //  выделение памяти не проводится
    //  метаописания и число примеров должны совпадать
    bool copy(const Row &row);
    //Копирование одного элемента
    bool copy1(const Row &row, int32_t ifrom, int32_t ito);
    //Копирование части примеров
    //  ifrom, ito, icount  - с какого номера источника, с какого номера в приемник, сколько
    bool copy(const Row &row, int32_t ifrom, int32_t ito, int32_t icount);
    //Копирование части примеров со сдвигом по байтам
    //  ifrom, ito, icount  - с какого номера источника, с какого номера в приемник, сколько
    //  sfrom, sto, ssize   - с какого байта источника, в какой байт приемника, какой длины
    bool copyb(const Row &row, int32_t ifrom, int32_t ito, int32_t icount, int32_t sfrom, int32_t sto, int32_t ssize);
    //Копирование части примеров со сдвигом по числу элементов
    //  ifrom, ito, icount  - с какого номера источника, с какого номера в приемник, сколько
    //  efrom, eto, esize   - с какого элемента источника, в какой элемент приемника, сколько элементов
    //bool copy(const Row &row, int32_t ifrom, int32_t ito, int32_t icount, int32_t efrom, int32_t eto, int32_t ecount);


    //Копирование с поэлементной конвертацией формата представления
    //примечание:
    //  выделение памяти не проводится
    //  метаописания и число примеров должно совпадать
    bool convert(const Row &row);
    //bool convert(const Row &row, int32_t ifrom, int32_t ito, int32_t icount, int32_t efrom, int32_t eto, int32_t ecount);

    //Объединение векторов
    //примечание: выделение памяти должно проводиться в методе resize(..., Unite ut)
    //            проверки на корректность не производится
    bool unite(const Rows &rows, UniteType ut);
    //Разеделение по векторам
    //примечание: выделение памяти у rows должно проводиться отдельно в зависимости от типа разделения
    //            проверки на корректность параметров не производится
    bool divide(Rows &rows, UniteType ut) const;

    //Определить параметры объединения rows
    //  umeta   [вых] - общее метаописание
    //  ucount  [вых] - общий размер
    static bool metaForUnite(const Rows &rows, UniteType ut, int32_t &ucount, Meta &umeta);

    //Преобразование в форму неизменяемой матрицы с одним столбцом
    const nbMatrix toM() const;

#if !defined(QT_NO_DEBUG_STREAM)
    QDebug friend operator<<(QDebug  dbg, const Row &x);
#endif

  protected:
    Matrix* _m;   //матрица с данными
    int32_t _irow;//номер строки (если >=0, Matrix доступна в режиме "чтение", "запись", "изменение"
    //        если <0, то в режиме "чтение", "запись" без изменения размера и метаописания
    //
    friend class Matrix;
  private:
    inline int32_t irow() const { return (_irow & ~0x80000000); } //Снятие блокировки флага
  };


public:

  Matrix();                                                 //Конструктор матрицы MtEmpty
  Matrix(int32_t nrows, int32_t ncols, const Meta *meta);   //Конструктор полной матрицы
  Matrix(int32_t ncols, Meta meta);                         //Конструктор матрицы с одним вектором
  Matrix(const Rows &rows);

  ~Matrix();

  inline operator nbMatrix&() {return _m;}
  inline operator const nbMatrix&() const {return _m;}
  inline Vbp* operator[](int32_t irow) const { return (Vbp*)_m.data[irow]; }

  Row     row(int32_t irow) const; //Получение ссылки на часть
  Rows    rows() const;            //Разделение на ссылки

  Vbp     at(int32_t irow, int32_t icol) const;  //Доступ к ВБП
  Vbp&    at(int32_t irow, int32_t icol);        //Доступ к ВБП

  Vbp**   data() const;
  int32_t ncols() const;            //Число примеров
  int32_t ncap() const;             //Зарезервированное число примеров
  int32_t nrows() const;            //Число частей примеров
  Meta*   meta() const;             //Метаописания матрицы
  Meta    meta(int32_t irow) const; //Метаописание части

  MatrixType type() const;          //Тип матрицы
  bool    isEmpty() const;          //Проверка пустоты матрицы и частей
  bool    isRow(int32_t irow) const;//Проверка наличия данных в части

  //Установка метаописания частей
  bool remeta(int32_t nrows, const Meta *meta);
  bool remeta(const Rows &rows);
  bool remeta(Meta meta);
  //Изменение метаописания части (установка, если отсутствует)
  bool remeta1(int32_t irow, Meta meta);

  //Выделение памяти под примеры
  bool resize(int32_t ncols);
  bool reserve(int32_t capacity);

  //Освобождение данных без удаления метаописаний (ncols=0)
  void empty();
  //Полное освобождение памяти, включая метаописания и зарезервированную память
  void free();

  //Копирование данных примеров без перевыделения памяти
  //примечание: число примеров в каждой части должно быть не больше установленного
  //            память не перевыделяется
  bool copy(const Rows &rows);
  bool copy(const Matrix &m);
  //Конвертация данных примеров без перевыделения памяти
  //примечание: число примеров в каждой части должно быть не больше установленного
  //            память не перевыделяется
  bool convert(const Rows &rows);
  bool convert(const Matrix &m);


  //Запись матрицы в поток
  //возращает: число записанных байт или -1
  int32_t write(QDataStream &to) const;
  //Чтение матрицы из потока
  //возращает: число записанных байт или -1
  int32_t read(QDataStream &from);

  //Создание вектора ссылок по матрице
  static Rows fromM(const nbMatrix &m);

protected:
  //Выделение памяти под один вектор примеров
  bool reserve1(Vbp** d, int32_t nfrom, int32_t nto, Meta mfrom, Meta mto);
  //Освобождение памяти из-под одного вектора примеров
  void free1(Vbp** d, int32_t nfrom);

#if !defined(QT_NO_DEBUG_STREAM)
  QDebug friend operator<<(QDebug  dbg, const Matrix &x);
#endif

private:
  nbMatrix _m;        //матрица
  int32_t  _ncap;     //зарезервированное число примеров
};
}

#pragma pack()
#endif //_NB_MATRIX_H
