#ifndef SESSIONTHREAD_H
#define SESSIONTHREAD_H

#include <QThread>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QSqlDatabase>
#include "sessiontask.h"
#include "serverdatabase.h"
#include "finishthreadevent.h"
#include "nb/nb.h"

///����� ������ ��������� �������� �������
class SessionThread : public QThread
{
  Q_OBJECT

private:
  SessionContext*       _session;       ///< �������� ������
  ServerContext* const  _server;        ///< �������� �������

public:
  SessionThread(QObject* parent, QTcpSocket &socket, ServerContext* const serverContext);

  /* ������������� ������� - ����� �����
   *
   */
  void run();


  /* ��������� ���������
   *
   */
  void processQuery();
};

#endif // SESSIONTHREAD_H
