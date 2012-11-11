#ifndef SERVERCONTEXT_H
#define SERVERCONTEXT_H

#include <QVector>
#include <QThread>
#include <QMutexLocker>
#include "protocol/log.h"

#define maxSessionCount       (100)


///Класс данных сервера биометрического удостоверяющего центра
class ServerContext
{
public:
  enum State
  {
    Stopped,
    Free,
    Busy
  };

private:
  QMutex*           _mutex;         ///<мьютекс для доступа к данным
  State             _state;         ///<состояние сервера
  unsigned          _sessionCount;  ///<текущее количество открытых сессий

public:
    ServerContext();
    ~ServerContext();

    /* Получить состояние
     *
     */
    ServerContext::State getState() const;


    /* Установить состояние "остановлен"
     *
     */
    bool markStopped();

    /* Добавить сессию
     *
     */
    bool noticeIncomingConnection();

    /* Снять сессию
     *
     */
    bool noticeClosedConnection();
};

#endif // SERVERCONTEXT_H
