#ifndef PASSWORDQUERY_H
#define PASSWORDQUERY_H

#include "query.h"

class PasswordQuery : public Query
{
public:
  //�������� ��� ��������� �������������� ������
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _passwordBlock;           ///< ���� ������

public:
  /* �����������
   *
   */
  PasswordQuery();

  /* ������� ������ � ��������� ������� ������
   *
   */
  bool create(Nb::Matrix &password);

  /* �������� ������ ������ �������
   *
   */
  bool get(Nb::Matrix &password);

  /* ��������� ���������� ������� (��� � ����� ������)
   *
   */
  bool isOk();
};

#endif // PASSWORDQUERY_H
