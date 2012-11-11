#ifndef STARTSIGNATUREQUERY_H
#define STARTSIGNATUREQUERY_H

#include "query.h"
#include "../nb/Container.h"
#include "../nb/Matrix.h"

class StartSignatureQuery : public Query
{
public:
  //�������� ��� ��������� �������������� ������
  using Query::create;

public:
  static const unsigned blockCount = 2;

  Query::DataBlock   _userIdBlock;
  Query::DataBlock   _schemeIdBlock;

public:
  /* ����������� �� ���������
   *
   */
  StartSignatureQuery();

  /* ������� ������ � ��������� ������� ������
   *
   */
  bool create(QString &userId, QString &schemeId);

  /* �������� ������ ������ �������
   *
   */
  bool get(QString &userId, QString &schemeId);

  /* ��������� ���������� ������� (��� � ����� ������)
   *
   */
  bool isOk();
};

#endif // STARTSIGNATUREQUERY_H
