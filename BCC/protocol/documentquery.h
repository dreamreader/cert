#ifndef DOCUMENTQUERY_H
#define DOCUMENTQUERY_H

#include "query.h"

class DocumentQuery : public Query
{
public:
  //�������� ��� ��������� �������������� ������
  using Query::create;

public:
  static const unsigned blockCount = 1;

  Query::DataBlock   _documentBlock;           ///< ���� ���������

public:
  /* �����������
   *
   */
  DocumentQuery();

  /* ������� ������ � ��������� ������� ������
   *
   */
  bool create(Nb::Data &document);

  /* �������� ������ ������ �������
   *
   */
  bool get(Nb::Data &document);

  /* ��������� ���������� ������� (��� � ����� ������)
   *
   */
  bool isOk();
};

#endif // DOCUMENTQUERY_H
