#include "server.h"

// Конструктор
Server::Server()
{
  _serverSocket = new QTcpServer(this);
  connect(_serverSocket, SIGNAL(newConnection()),
    this, SLOT(openSession()));
}

Server::~Server()
{
  _serverSocket->close();
  delete _serverSocket;
}

// Перегруженный слот - новое соединение
void Server::openSession()
{
  LOG
  Log::write("incoming connection!");

  _serverContext.noticeIncomingConnection();
  SessionThread* clientThread = new SessionThread(this, *_serverSocket->nextPendingConnection(), &_serverContext);
  _sessionThreads.push_back(clientThread);

  QEventLoop loop;
  QObject::connect(clientThread, SIGNAL(started()), &loop, SLOT(quit()));
  Log::write("starting thread... ", int(clientThread));
  clientThread->start();
  loop.exec();
  Log::write("thread started");

  Log::write("process query ");
  clientThread->processQuery();
}

bool Server::event(QEvent* e)
{
  LOG
  if (e->type() == FinishThreadEvent::FinishThreadEventType)
  {
    Log::write("finish event received");
    SessionThread* clientThread = static_cast<SessionThread*>(static_cast<FinishThreadEvent *>(e)->thread());
    _sessionThreads.removeAt(_sessionThreads.indexOf(clientThread));
    Log::write("wait thread... ");
    clientThread->wait();
    Log::write("thread finished... ", clientThread->isFinished());
    Log::write("delete thread... ", int(clientThread));
    delete clientThread;
    Log::write("closed connection!");
    _serverContext.noticeClosedConnection();
  }
  else
  {
    return QTcpServer::event(e);
  }
  return true;
}

// Запустить сервер
bool Server::start()
{
  LOG
  QHostAddress addr(tcpServerAddress);
  _serverSocket->listen(addr, tcpServerPort);

  Log::write("server is listening now", _serverSocket->isListening());
  return true;
}

// Остановить сервер
bool Server::stop()
{
  LOG
  _serverSocket->close();
  _serverContext.markStopped();

  Log::write("server stopped");
  return true;
}
