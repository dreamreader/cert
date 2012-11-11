#ifndef STARTREGISTERQUERY_H
#define STARTREGISTERQUERY_H

#include "query.h"
#include "../nb/Container.h"
#include "../nb/Matrix.h"
#include "../nb/Data.h"
#include "../nb/Scheme.h"

class StartRegisterQuery : public Query
{
public:
  //�������� ��� ��������� �������������� ������
  using Query::create;

public:
  static const unsigned blockCount = 3;

  Query::DataBlock   _userIdBlock;
  Query::DataBlock   _containerIdBlock;
  Query::DataBlock   _regkeyBlock;

public:
  /* ����������� �� ���������
   *
   */
  StartRegisterQuery();

  /* ������� ������ � ��������� ������� ������
   *
   */
  bool create(QString &userId, QString &containerId, QString &regkey);

  /* �������� ������ ������ �������
   *
   */
  bool get(QString &userId, QString &containerId, QString &regkey);

  /* ��������� ���������� ������� (��� � ����� ������)
   *
   */
  bool isOk();
};

#endif // STARTREGISTERQUERY_H
