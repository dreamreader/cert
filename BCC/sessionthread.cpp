#include "sessionthread.h"

SessionThread::SessionThread(QObject* parent, QTcpSocket &socket, ServerContext* const serverContext): QThread(parent), _server(serverContext)
{
  LOG
  socket.moveToThread(this);
  _session = new SessionContext(socket);
  Log::write("context created");
}

void SessionThread::processQuery()
{
  LOG
  Log::write("enter thread 'run'");

  bool rslt;

  /*
  TODO: if server busy stop thread
   */

  do
  {
      Log::write("enter wait command loop");
      SessionTask task(*_session, *_server);
      _session->beginTransaction();
      rslt = task.start();

      // Возникла ошибка протокола
      if (rslt)
      {
        _session->commitTransaction();
      }
      else
      {
        _session->rollbackTransaction();
        goto finish;
      }

      // Соединение завершено
      if (!rslt)
      {
          //terminate();
          goto finish;
      }
  }
  while (rslt);

finish:
  Log::write("leaving thread 'run'");
  Log::write("raise thread-finish event");
  QCoreApplication::postEvent(parent(), new FinishThreadEvent(this));
  quit();
  return;
}

void SessionThread::run()
{
  exec();
}
