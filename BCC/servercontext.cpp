#include "servercontext.h"

// Конструктор по умолчанию
ServerContext::ServerContext(): _state(Free), _sessionCount(0)
{
  LOG
  Log::write("server context ctor");
  _mutex = new QMutex();
  Log::write("return");
}

ServerContext::~ServerContext()
{
  LOG
  Log::write("server context dtor");
  _mutex->lock();
  _mutex->unlock();
  delete _mutex;
  Log::write("return");
}

// Получить состояние
ServerContext::State ServerContext::getState() const
{
  _mutex->lock();
  State s = _state;
  _mutex->unlock();
  return s;
}

/* Остановить сервер
 *
 */
bool ServerContext::markStopped()
{
  _mutex->lock();
  _state = Stopped;
  _mutex->unlock();
  return true;
}

/* Добавить сессию
 *
 */
bool ServerContext::noticeIncomingConnection()
{
  LOG

  Log::write("notice incoming connection");
  _mutex->lock();
  _sessionCount++;
  if (_sessionCount >= maxSessionCount)
    _state = Busy;
  _mutex->unlock();
  return true;
}

/* Снять сессию
 *
 */
bool ServerContext::noticeClosedConnection()
{
  Log::Tab tab();
  Log::write("notice closed connection");
  _mutex->lock();
  _sessionCount--;
  if (_sessionCount < maxSessionCount)
    _state = Free;
  _mutex->unlock();
  return true;
}
