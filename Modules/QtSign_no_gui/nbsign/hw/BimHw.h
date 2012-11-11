/****************************************************************************
** файл:         BimHw.h
** версия:
** дата:
** изменен:
** автор:
**
** описание:     Определение биометрического образа (БО), форматов его
**               хранения, а также специфичных типов данных и структур.
****************************************************************************/

#ifndef _BIM_HW_H
#define _BIM_HW_H

#include "..\nb\nb.h"

#pragma pack(1)

/****************************************************************************
**
** ТЕХНОЛОГИЯ Рукописный образ
**
****************************************************************************/

// Уникальный идентификатор типа БО для рукописного образа
// {C42A41D2-0118-4ef9-B71C-F3B27D1A980D}
static const nbUuid nbBIM_TYPE_HANDWR = 
{0xc42a41d2,0x0118,0x4ef9,{0xb7,0x1c,0xf3,0xb2,0x7d,0x1a,0x98,0xd}};

// Точка рукописного образа с 4-координатами
typedef struct nbPointHw_t {
  uint16_t x;           //  координата x
  uint16_t y;           //  координата y
  uint16_t p;           //  давление (0, используется как ограничитель, maxP/2 как нормальное, если давление не задано)
  uint16_t t;           //  время (относительное в мс)
} nbPointHw;

// Заголовок рукописного образа
struct nbBimHwHeader: public nbBim {
  uint16_t num;         //  количество точек (координат)
  uint16_t maxX;        //  максимальная координата X
  uint16_t maxY;        //  максимальная координата Y
  uint16_t maxP;        //  максимальное значение давления [0..maxP]
  uint16_t dpiX;        //  разрешение по X (видимая)
  uint16_t dpiY;        //  разрешение по Y (видимая)
  uint16_t phrase[32];  //  предполагаемая фраза
  //  массив точек рукописного образа (nbPointHw[num])
  //  ...
  //  ... 
};

// Состояние устройства ввода рукописных образов (мышь или планшет)
enum SignDeviceState {
  sdsNone     = 0,      // начальное состояние
  sdsMouse    = 1,      // работать в режиме эмулятора мышки
  sdsPen      = 2,      // работать в режиме графического планшета
  sdsNoTablet = 3       // не найден планшет
};

// Флаги настроек
#define SO_FLAG_HIDE_IMG   (0x0001)  // скрывать образ
#define SO_FLAG_CAPTURE    (0x0002)  // захватывать планшет в области ввода
#define SO_FLAG_MOUSE_ONLY (0x0004)  // использовать мышь (планшет не определяется)
#define SO_MAX_SERVICE_NAME 64

// Настройки поля ввода рукописных образов
typedef struct nbSignOptions_t {
  int       flags;                        // флаги SO_FLAG_XXX
  wchar_t   szName[SO_MAX_SERVICE_NAME];  // имя службы планшета
  inline bool isCapture(){ return ((flags & SO_FLAG_CAPTURE) == SO_FLAG_CAPTURE); }
  inline bool isMouseOnly(){ return ((flags & SO_FLAG_MOUSE_ONLY) == SO_FLAG_MOUSE_ONLY); }
  inline bool isHideImg(){ return ((flags & SO_FLAG_HIDE_IMG) == SO_FLAG_HIDE_IMG); }
} nbCanvasOptions;

// Настройки планшета
typedef struct nbTabletOptions_t {
  uint16_t maxX;    // максимальное разрешение планшета по ширине
  uint16_t maxY;    // максимальное разрешение планшета по высоте
  uint16_t maxP;    // максимальное значение давления [0..maxP]
  uint16_t scrW;    // ширина экрана
  uint16_t scrH;    // высота экрана
  uint16_t ppiX;    // разрешение по X (видимая) (число пикселей в логическом дюйме)
  uint16_t ppiY;    // разрешение по Y (видимая) (число пикселей в логическом дюйме)
} nbTabletOptions;


// Рукописный образ
class BimHw {
public:
  // Конструктор
  BimHw();
  BimHw(uint16_t num, nbTabletOptions params);
  // Деструктор
  ~BimHw();

  // Выделение памяти
  bool create(uint16_t num, nbTabletOptions params);

  // Освобождение памяти
  bool free();

  // Копирование рукописного образа
  bool fromBim(const nbBim* bim);

  // Присоединение биометрического образа
  bool attach(nbBim *bim);
  // Отсоединение биометрического образа
  nbBim* detach();

  // Доступ к заголовку рукописного образа
  nbBimHwHeader* header() const;
  // Доступ к точкам рукописного образа
  uint8_t* data() const;

  nbBim* handle() const;

  // Тип
  nbUuid type() const;
  // Размер
  uint32_t size() const;
  // Проверка наличия образа
  uint32_t empty() const;

private:
  nbBim* _bim;
};

#pragma pack()

#endif  // _BIM_HW_H





