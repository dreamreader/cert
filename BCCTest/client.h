#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "../bcc/protocol/log.h"
#include "../bcc/protocol/queries.h"
#include "../bcc/nb/nb.h"
#include "../bcc/nb/nbc.h"

#define tcpServerAddress      ("localhost")
#define tcpServerPort         (31111)

/// Класс клиента биометрического удостоверяющего центра
class Client: public QObject
{
  Q_OBJECT

private:
  QTcpSocket  _socket;      ///< сокет

public slots:
  void openSession();

signals:
  void sessionOpened();
  void sessionClosed();

public:
  Client();
  ~Client();

  /* Запустить сессию
   *
   */
  bool start();

  /* Остановить сессию
   *
   */
  bool stop();



  /* Перечислить контейнеры пользователя
   *
   */
  nbResult enumerateContainers(QString userId, QStringList &containers);



  /* Начать регистрацию
   *
   */
  nbResult startRegistration(QString userId, QString containerId, QString &regkey);

  /* Зарегистрировать указанный контейнер
   *
   */
  nbResult registerContainer(Nbc &container);

  /* Зарегистрировать указанные биометрические данные
   *
   */
  nbResult registerData(QList<Nb::Matrix*> &ownParams, QList<Nb::Matrix*> &allParams, Statistics& stats, bool repeat = false);

  /* Протестировать
   *
   */
  nbResult testData(QList<Nb::Matrix*> &testParams);

  /* Подтверить регистрацию
   *
   */
  nbResult confirmRegistration();



  /* Начать процедуру подписывания документа
   *
   */
  nbResult startSign(QString userId, QString containerId, Nbc &container);

  /* Использовать одноразовый пароль
   *
   */
  nbResult usePassword(Nb::Matrix &password, bool &accessGranted);

  /* Подписать документ
   *
   */
  nbResult signDocument(Nb::Data document, Nb::Data signature);



  /* Отменить операцию
   *
   */
  nbResult cancel();
};

#endif // CLIENT_H
