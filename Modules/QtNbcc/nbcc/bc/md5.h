#pragma once
//////////////////////////////////////////////////////////////////////////
//файл:         md5.h
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Вычисление контрольной суммы и результата необратимого
//              преобразования
//////////////////////////////////////////////////////////////////////////
#ifndef _MD5_H
#define _MD5_H

#include <QtCore/QString>
#include "../nb/nb.h"

#pragma pack(1)

  //Хеш Md5
  class Md5Hash {
  public:
    //Контекст для блочного вычисления хеш-функции
    typedef struct Md5Context_t {
      uint32_t state[4];	  // состояния(ABCD)
      uint32_t count[2];    // число бит, modulo 2^64 (lsb first)
      uint8_t  buffer[64];  // входной буфер
    } Md5Context;	

    enum {
      md5HASH_SIZE = 16,
      md5HASH_BITS = (md5HASH_SIZE*8)
    };
    uint8_t digest[md5HASH_SIZE];
  public:

    Md5Hash();
    Md5Hash(const uint8_t *data, uint32_t size);
    ~Md5Hash();

    //Создание хеша по блоку данных
    void create(const uint8_t *data, uint32_t size);

    //Инициализация контекста во время отложенного вычисления
    void init(Md5Context &ctx);
    //Обновление хеша во время отложенного вычисления
    void update(Md5Context &ctx, const uint8_t *data, uint32_t size);
    //Завершение вычисления хеша
    void final(Md5Context &ctx);

    //Стирание хеша
	  void erase();
	  //Проверка на равенство
	  bool isEqual(const Md5Hash &hash) const;
    //Проверка на ноль
    bool isNil(void) const;

    //Усечение до заданного числа бит
    void trim(uint32_t bitLength);
    void set(const uint8_t *data, uint32_t size);

	  Md5Hash& operator =(const Md5Hash &hash);
	  bool operator ==(const Md5Hash &hash) const;
    bool operator !=(const Md5Hash &hash) const;

    //Преобразование строки в хеш-значение
    bool fromString(const QString &s);
	  //Преобразование хеш-значения в строку
    QString& toString(QString &s) const;
  protected:
    //Обновление хеша во время отложенного вычисления
    void update16(Md5Context &ctx, const uint8_t *data, uint16_t size);
  private:
    static const QString _tblX16;
  };

#pragma pack()

#endif

