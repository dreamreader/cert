#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QList>
#include <Windows.h>
#include "sessionthread.h"
#include "servercontext.h"
#include "protocol/log.h"


#define tcpServerAddress      ("localhost")
#define tcpServerPort         (31111)

///Класс сервера биометрического удостоверяющего центра
class Server: public QTcpServer
{
  Q_OBJECT

private:
  QTcpServer*             _serverSocket;
  QList<SessionThread*>   _sessionThreads;
  ServerContext           _serverContext;

public slots:

  /* Перегруженный слот - новое соединение
   *
   */
  void openSession();

  /* Запустить сервер
   *
   */
  bool start();

  /* Остановить сервер
   *
   */
  bool stop();

public:
  Server();
  ~Server();

  bool event(QEvent* e);
};

#endif // SERVER_H
