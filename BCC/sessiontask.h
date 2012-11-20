#ifndef SESSIONTASK_H
#define SESSIONTASK_H

#include <QDataStream>
#include <QDateTime>
#include "protocol/Queries.h"
#include "sessioncontext.h"
#include "servercontext.h"
#include "onetimepassword.h"
#include "nb/nb.h"
#include "nb/nbclasses.h"
#include "nb/nbc.h"
#include "nb/nbcc.h"
#include "nb/dispatcher.h"
#include "nb/processor.h"


///Абстрактный класс задачи
/*
 *  Инкапсулирует протокол обмена сервера и клиента
 *  Динамически типизируется по типу запроса
 */
class SessionTask
{
private:
  SessionContext          &_session;      ///<данные клиента
  const ServerContext     &_server;       ///<контекст сервера

public:
  /* Конструктор - создать задачу неопределённого типа
   *
   */
  SessionTask(SessionContext &session, const ServerContext &server);

  /* Запустить задачу
   *
   */
  bool start();

  /* Проверить состояние сервера
   *
   *  \return true если продолжение операции протокола возможно
   */
  bool checkServer();

private:
  /* Выполнить аутентификацию
   *
   *  \return false если возникла ошибка протокола
   */
  bool authenticate(Query::QueryHeaderBlock &header);

  /* Выполнить биометрическую аутентификацию
   *
   *  \return false если возникла ошибка протокола
   */
  bool authenticateBio(Query::QueryHeaderBlock &header);

  /* Создать одноразовый контейнер для биометрической аутентификации
   *
   *  \return false если возникла ошибка протокола
   */
  bool createContainer(Query::QueryHeaderBlock &header);

  /* Запустить задачу регистрации
   *
   *  \return false если возникла ошибка протокола
   */
  bool registerContainer(Query::QueryHeaderBlock &header);

  /* Запустить задачу подписывания документа
   *
   *  \return false если возникла ошибка протокола
   */
  bool signData(Query::QueryHeaderBlock &header);

  /* Остановить выполнение задачи
   *
   */
  bool stop(/*nbResult error*/);
};

#endif // SESSIONTASK_H
