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

///Класс потока обработки запросов клиента
class SessionThread : public QThread
{
  Q_OBJECT

private:
  SessionContext*       _session;       ///< контекст сессии
  ServerContext* const  _server;        ///< контекст сервера

public:
  SessionThread(QObject* parent, QTcpSocket &socket, ServerContext* const serverContext);

  /* Перегруженная функция - точка входа
   *
   */
  void run();


  /* Запустить обработку
   *
   */
  void processQuery();
};

#endif // SESSIONTHREAD_H
