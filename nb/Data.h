//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Объявление класса блока данных с дополнительным полем
//              размера по отрицательному смещению блока
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_DATA_H
#define _NB_DATA_H

#include <QtCore/QString>
#include <QtCore/QDataStream>

#include "Enums.h"

#pragma pack(1)

namespace Nb {

using namespace std;

//...........................................................................
//  Блок данных
//...........................................................................
class Data {
  nbData   _m;          //блок данных с размером
  int32_t  _capacity;   //фактическая зарезервированная емкость блока
                        //  (отрицательное значение для предельно допустимой емкости)
public:
  Data();
  Data(int32_t size);
  Data(const nbData data);
  Data(const Data &source);
  Data(const QString &source);
  ~Data();

  inline operator nbData*(){return &_m;}
  inline operator nbData() const { return _m; }
  
  Data& operator=(const Data &d) { ref(d._m); return *this; }
  bool operator==(const Data &d) const { return (0==compare(d)); }
  bool operator!=(const Data &d) const { return (0!=compare(d)); }
      
  inline uint8_t* data() const { return (uint8_t*)_m;}  //Доступ к данным

  bool  attach(nbData data);  //Присоединение ранее отсоединенного блока
  nbData detach();            //Отсоединение блока

  bool isNull() const;      //Проверка наличия буфера
  bool isEmpty() const;     //Проверка буфера и размера
  int32_t size() const;     //Размер блока
  int32_t capacity() const; //Предельная емкость блока

  //Запись в поле размера блока желаемого размера
  //примечание: после использования нельзя вызывать функции изменения размера и копирования,
  //не проверив достаточность размера буфера
  bool     setWishfulSize(int32_t size);

  //Особождение памяти
  void free();
  //Изменение размера блока
  bool resize(int32_t size);
  //Резервирование размера
  bool reserve(int32_t capacity);

  //Формирование ссылки на данные
  //примечание: допускается выполнять любые операции с блоком, кроме увеличения его емкости
  Data ref() const;
  static Data ref(const nbData data, int32_t capacity=0);
  //Формирование ссылки на блок данных как на Data
  //примечание: первые 4 байта массива должны содержать поле размера,
  //            устанавливаемое до вызова функции
  static Data ref(const QByteArray &ba);
  bool isRef() const;
  //Копирование данных
  bool copy(const Data &source);
  //Копирование данных
  bool copy(const uint8_t* src, int32_t srcSize);

  //Чтение из потока
  //  from - поток для чтения
  //  size - число считанных байт
  //  withSize - чтение заголовка (если false, используется значение поля size)
  //возвращает: число считанных байт или -1
  int32_t read(QDataStream &from, bool withSize=true, int32_t size=0);
  //Запись в поток
  //  to        - поток для записи
  //  withSize  - запись в поток с заголовком
  //возвращает: число записанных байт или -1
  int32_t write(QDataStream &to, bool withSize=true) const;

  //Длина получаемой вызовом toString строки в utf-16
  int32_t  length() const;
  //Преобразование в строку utf-16 из блока utf-8
  QString toString() const;
  void toString(wchar_t *dest) const;
  //Преобразование в блок utf-8 из строки utf-16
  //Возвращает необходимый размер строки, если размер буфера недостаточен. Иначе 0
  bool fromString(const QString &source, int32_t &wishfulSize, bool fResize = false);
 
  //Сравнение (0 (c==d), >0 (c>d), <0 (c<d))
  int32_t compare(const Data &d) const;

  static int32_t* sizeByPtr(nbData ptr);

  #if !defined(QT_NO_DEBUG_STREAM)
    QDebug friend operator<<(QDebug  dbg, const Data &x);
  #endif
};

  inline bool Data::isRef() const { return _capacity & 0x80000000; }
  inline int32_t* Data::sizeByPtr(nbData ptr) { return (int32_t*)ptr-1; }

}

#pragma pack()
#endif  //_NB_DATA_H
