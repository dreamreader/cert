//////////////////////////////////////////////////////////////////////////
//файл:
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Объявление класса универсального биометрического образа
//              и набора биометрических образов
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_BIM_H
#define _NB_BIM_H

#include <QtCore/QDataStream>

#include "Enums.h"
#include "Uuid.h"

#pragma pack(1)

namespace Nb {

  using namespace  std;

  //Описатель биометрического образа
  typedef nbBim* BimHandle;

  //Биометрический образ
  class Bim {
  public:    
    Bim();
    Bim(const BimHandle handle);  //Копирование образа
    Bim(const Bim& bim);          //Копирование образа
    //Bim(int32_t sizeData);        //Выделение памяти под образ нужного размера
    ~Bim();                       //Освобождение памяти

    Bim& operator=(const Bim& bim);  //Оператор копирования

    bool isNull() const;          //Проверка на наличие памяти
    bool isEmpty() const;         //Проверка на наличие данных (существование)

    int32_t size() const;         //Полный размер для хранения
    int32_t sizeData() const;     //Размер данных
    const Uuid& type() const;     //Тип
    uint8_t* data() const;        //Данные

    BimHandle handle() const;     // Доступ к описателю
    bool attach(BimHandle);       // Присоединение описателя
    BimHandle detach();           // Отсоединение описателя

    bool resize(const Uuid &type, int32_t dataSize); //Создание неинициализированного образа
    void free();                  //Освобождение образа

    bool copy(const BimHandle handle); //Копирование с выделением памяти

    int32_t read(QDataStream &from); //Чтение из потока
    int32_t write(QDataStream &to) const;  //Запись в поток

    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const Bim &x);
    #endif

  protected:
    BimHandle _m;     //Описатель биометрического образа
  };

  typedef QList<Bim> Bims;

  //Класс неупакованного биометрического образа
  //Примечание. Классы неупакованных рукописных образов (BimHandWr), отпечатков пальцев (BimFingerPr) 
  //  и т.д. должны наследоваться от него.
  class BimUnpacked {
    virtual ~BimUnpacked()=0;                       //Виртуальный деструктор

    virtual bool isEmpty() const=0;                 //Проверка на наличие данных

    virtual const Uuid& type() const=0;             //Тип
    virtual int32_t sizePacked()=0;                 //Размер упакованного образа

    virtual bool pack(BimHandle bim)=0;             //Упаковка образа без изменения размера
    virtual bool pack(Bim &bim, bool canResize=false)=0; //Упаковка образа с изменение размера по желанию
    virtual bool unpack(const BimHandle handle)=0;  //Распаковка образа
    virtual bool isSupportedType(Uuid& classType)=0;//Проверка возможности загрузки образа
    virtual const Uuid& baseType()=0;               //Возвращает основной поддерживаемый тип
  };
}

#pragma pack()
#endif //_NB_BIM_H
