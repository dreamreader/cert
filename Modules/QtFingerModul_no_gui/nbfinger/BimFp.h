#ifndef _BIM_FP_H
#define _BIM_FP_H

#include "nb/nb.h"

#pragma pack(1)

//Уникальный идентификатор типа БО для отпечатка пальца
// {EFAD1EB7-29F3-4c69-8689-EA2C938D1119}
static const nbUuid nbBIM_TYPE_FINGER = 
{0xefad1eb7,0x29f3,0x4c69,{0x86,0x89,0xea,0x2c,0x93,0x8d,0x11,0x19}};

// Заголовок образа "Отпечаток пальца"
struct nbBimFpHeader: public nbBim {
  uint16_t  width;           // ширина изображения  (x)
  uint16_t  height;          // высота изображения  (y)
  uint8_t   format;          // 1 - монохром; 8 - серый; 24 - RGB (реализован только 8)
  uint8_t   action;          // операция с образом
  //  массив точек образа (uint8_t[width*heigth])
  //  ...
  //  ... 
};

enum FingerprintFormat {
  fpfMono = 1,
  fpfGrey = 8,
  fpfRgb  = 24
};

//Перечисление для действий, которые необходимо выполнить пользователю,
//для ввода хорошего образа
enum FingerprintAction {
  fpaNone             = 0x00, //нет команд
  fpaLeft             = 0x01, //сдвинуть влево
  fpaRight            = 0x02, //сдвинуть вправо
  fpaUp               = 0x04, //сдвинуть вверх
  fpaDown             = 0x08, //сдвинуть вниз
  fpaClockwise        = 0x10, //повернуть по часовой
  fpaAnticlockwise    = 0x20, //повернуть против часовой
  fpaIncreasePressure = 0x40, //усилить давление
  fpaRelievePressure  = 0x80  //ослабить давление
};

//Состояние устройства ввода отпечатка пальца
enum FingerDeviceState {
  fdsNone,              //начальное состояние
  fdsWaiting,           //ожидание ввода образа (устройство занято)
  fdsWaitingBetter,     //ожидание ввода лучшего образа
  fdsLoaded,            //загружен образ
  fdsError              //ошибка
};


//Изображение отпечатка пальца
class BimFp {
public:
  //Конструктор
  BimFp();
  BimFp(uint16_t width, uint16_t height, uint8_t format, uint8_t action);
  //Деструктор
  ~BimFp();
  
  //Выделение памяти
  bool create(uint16_t width, uint16_t height, uint8_t format, uint8_t action);
  //Освобождение памяти
  bool free();
  
  //Копирование отпечатка пальца
  bool fromBim(const nbBim* bim);

  //Копирование изображения из массива целых чисел width*height
  bool fromInt32(uint16_t width, uint16_t height, const int32_t *image);
  //Копирование изображения в массив целых чисел width*height
  bool toInt32(int32_t *image);

  //Присоединение биометрического образа
  bool attach(nbBim *bim);
  //Отсоединение биометрического образа
  nbBim* detach();

  //Доступ к заголовку отпечатка пальца
  nbBimFpHeader* header() const;
  //Доступ к изображению отпечатка пальца
  uint8_t* data() const;

  nbBim* handle() const;

  //Тип
  nbUuid type() const;
  //Размер
  uint32_t size() const;
  //Проверка наличия образа
  uint32_t empty() const;

private:
  nbBim* _bim;
};

#pragma pack()

#endif