/****************************************************************************
** ����:         BimHw.h
** ������:
** ����:
** �������:
** �����:
**
** ��������:     ����������� ��������������� ������ (��), �������� ���
**               ��������, � ����� ����������� ����� ������ � ��������.
****************************************************************************/

#ifndef _BIM_HW_H
#define _BIM_HW_H

#include "..\nb\nb.h"

#pragma pack(1)

/****************************************************************************
**
** ���������� ���������� �����
**
****************************************************************************/

// ���������� ������������� ���� �� ��� ����������� ������
// {C42A41D2-0118-4ef9-B71C-F3B27D1A980D}
static const nbUuid nbBIM_TYPE_HANDWR = 
{0xc42a41d2,0x0118,0x4ef9,{0xb7,0x1c,0xf3,0xb2,0x7d,0x1a,0x98,0xd}};

// ����� ����������� ������ � 4-������������
typedef struct nbPointHw_t {
  uint16_t x;           //  ���������� x
  uint16_t y;           //  ���������� y
  uint16_t p;           //  �������� (0, ������������ ��� ������������, maxP/2 ��� ����������, ���� �������� �� ������)
  uint16_t t;           //  ����� (������������� � ��)
} nbPointHw;

// ��������� ����������� ������
struct nbBimHwHeader: public nbBim {
  uint16_t num;         //  ���������� ����� (���������)
  uint16_t maxX;        //  ������������ ���������� X
  uint16_t maxY;        //  ������������ ���������� Y
  uint16_t maxP;        //  ������������ �������� �������� [0..maxP]
  uint16_t dpiX;        //  ���������� �� X (�������)
  uint16_t dpiY;        //  ���������� �� Y (�������)
  uint16_t phrase[32];  //  �������������� �����
  //  ������ ����� ����������� ������ (nbPointHw[num])
  //  ...
  //  ... 
};

// ��������� ���������� ����� ���������� ������� (���� ��� �������)
enum SignDeviceState {
  sdsNone     = 0,      // ��������� ���������
  sdsMouse    = 1,      // �������� � ������ ��������� �����
  sdsPen      = 2,      // �������� � ������ ������������ ��������
  sdsNoTablet = 3       // �� ������ �������
};

// ����� ��������
#define SO_FLAG_HIDE_IMG   (0x0001)  // �������� �����
#define SO_FLAG_CAPTURE    (0x0002)  // ����������� ������� � ������� �����
#define SO_FLAG_MOUSE_ONLY (0x0004)  // ������������ ���� (������� �� ������������)
#define SO_MAX_SERVICE_NAME 64

// ��������� ���� ����� ���������� �������
typedef struct nbSignOptions_t {
  int       flags;                        // ����� SO_FLAG_XXX
  wchar_t   szName[SO_MAX_SERVICE_NAME];  // ��� ������ ��������
  inline bool isCapture(){ return ((flags & SO_FLAG_CAPTURE) == SO_FLAG_CAPTURE); }
  inline bool isMouseOnly(){ return ((flags & SO_FLAG_MOUSE_ONLY) == SO_FLAG_MOUSE_ONLY); }
  inline bool isHideImg(){ return ((flags & SO_FLAG_HIDE_IMG) == SO_FLAG_HIDE_IMG); }
} nbCanvasOptions;

// ��������� ��������
typedef struct nbTabletOptions_t {
  uint16_t maxX;    // ������������ ���������� �������� �� ������
  uint16_t maxY;    // ������������ ���������� �������� �� ������
  uint16_t maxP;    // ������������ �������� �������� [0..maxP]
  uint16_t scrW;    // ������ ������
  uint16_t scrH;    // ������ ������
  uint16_t ppiX;    // ���������� �� X (�������) (����� �������� � ���������� �����)
  uint16_t ppiY;    // ���������� �� Y (�������) (����� �������� � ���������� �����)
} nbTabletOptions;


// ���������� �����
class BimHw {
public:
  // �����������
  BimHw();
  BimHw(uint16_t num, nbTabletOptions params);
  // ����������
  ~BimHw();

  // ��������� ������
  bool create(uint16_t num, nbTabletOptions params);

  // ������������ ������
  bool free();

  // ����������� ����������� ������
  bool fromBim(const nbBim* bim);

  // ������������� ��������������� ������
  bool attach(nbBim *bim);
  // ������������ ��������������� ������
  nbBim* detach();

  // ������ � ��������� ����������� ������
  nbBimHwHeader* header() const;
  // ������ � ������ ����������� ������
  uint8_t* data() const;

  nbBim* handle() const;

  // ���
  nbUuid type() const;
  // ������
  uint32_t size() const;
  // �������� ������� ������
  uint32_t empty() const;

private:
  nbBim* _bim;
};

#pragma pack()

#endif  // _BIM_HW_H





