#ifndef NBQUERYHEADER_H
#define NBQUERYHEADER_H

#include "../nb/nbTypes.h"

typedef uint8_t nbQueryFlags;
typedef uint8_t nbQueryType;

#define nbBT_PROTOCOL       (129)       // ���� ���������

/// ��������� �������
struct nbQueryHeader
{
  uint16_t              qcount;         // 2 �����. ����� ��������� �� ���������� ������� ������
  nbQueryFlags          qflags;         // 1 ����. ����� ������� (�� ����������)
  nbQueryType           qtype;          // 1 ����. ��� �������
};

#endif // NBQUERYHEADER_H
