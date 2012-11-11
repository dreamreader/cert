#ifndef SCHEMEQUERY_H
#define SCHEMEQUERY_H

#include "query.h"

class SchemeQuery : public Query
{
public:
  //�������� ��� ��������� �������������� ������
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _schemeBlock;           ///< ���� ����� ��������������

public:
  /* �����������
   *
   */
  SchemeQuery();

  /* ������� ������ � ��������� ������� ������
   *
   */
  bool create(Nb::Scheme &scheme);

  /* �������� ������ ������ �������
   *
   */
  bool get(Nb::Scheme &scheme);

  /* ��������� ���������� ������� (��� � ����� ������)
   *
   */
  bool isOk();
};

#endif // SCHEMEQUERY_H
