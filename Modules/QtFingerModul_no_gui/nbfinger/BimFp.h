#ifndef _BIM_FP_H
#define _BIM_FP_H

#include "nb/nb.h"

#pragma pack(1)

//���������� ������������� ���� �� ��� ��������� ������
// {EFAD1EB7-29F3-4c69-8689-EA2C938D1119}
static const nbUuid nbBIM_TYPE_FINGER = 
{0xefad1eb7,0x29f3,0x4c69,{0x86,0x89,0xea,0x2c,0x93,0x8d,0x11,0x19}};

// ��������� ������ "��������� ������"
struct nbBimFpHeader: public nbBim {
  uint16_t  width;           // ������ �����������  (x)
  uint16_t  height;          // ������ �����������  (y)
  uint8_t   format;          // 1 - ��������; 8 - �����; 24 - RGB (���������� ������ 8)
  uint8_t   action;          // �������� � �������
  //  ������ ����� ������ (uint8_t[width*heigth])
  //  ...
  //  ... 
};

enum FingerprintFormat {
  fpfMono = 1,
  fpfGrey = 8,
  fpfRgb  = 24
};

//������������ ��� ��������, ������� ���������� ��������� ������������,
//��� ����� �������� ������
enum FingerprintAction {
  fpaNone             = 0x00, //��� ������
  fpaLeft             = 0x01, //�������� �����
  fpaRight            = 0x02, //�������� ������
  fpaUp               = 0x04, //�������� �����
  fpaDown             = 0x08, //�������� ����
  fpaClockwise        = 0x10, //��������� �� �������
  fpaAnticlockwise    = 0x20, //��������� ������ �������
  fpaIncreasePressure = 0x40, //������� ��������
  fpaRelievePressure  = 0x80  //�������� ��������
};

//��������� ���������� ����� ��������� ������
enum FingerDeviceState {
  fdsNone,              //��������� ���������
  fdsWaiting,           //�������� ����� ������ (���������� ������)
  fdsWaitingBetter,     //�������� ����� ������� ������
  fdsLoaded,            //�������� �����
  fdsError              //������
};


//����������� ��������� ������
class BimFp {
public:
  //�����������
  BimFp();
  BimFp(uint16_t width, uint16_t height, uint8_t format, uint8_t action);
  //����������
  ~BimFp();
  
  //��������� ������
  bool create(uint16_t width, uint16_t height, uint8_t format, uint8_t action);
  //������������ ������
  bool free();
  
  //����������� ��������� ������
  bool fromBim(const nbBim* bim);

  //����������� ����������� �� ������� ����� ����� width*height
  bool fromInt32(uint16_t width, uint16_t height, const int32_t *image);
  //����������� ����������� � ������ ����� ����� width*height
  bool toInt32(int32_t *image);

  //������������� ��������������� ������
  bool attach(nbBim *bim);
  //������������ ��������������� ������
  nbBim* detach();

  //������ � ��������� ��������� ������
  nbBimFpHeader* header() const;
  //������ � ����������� ��������� ������
  uint8_t* data() const;

  nbBim* handle() const;

  //���
  nbUuid type() const;
  //������
  uint32_t size() const;
  //�������� ������� ������
  uint32_t empty() const;

private:
  nbBim* _bim;
};

#pragma pack()

#endif