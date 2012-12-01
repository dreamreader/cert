#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "../bcc/protocol/log.h"
#include "../bcc/protocol/queries.h"
#include "../bcc/nb/nb.h"
#include "../bcc/nb/nbc.h"

/// Класс клиента биометрического удостоверяющего центра
class Client: public QObject
{
  Q_OBJECT

private:
  QTcpSocket  _socket;        ///< сокет

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
  bool start(QString tcpAddress, int port);

  /* Остановить сессию
   *
   */
  bool stop();


  /* Перечислить контейнеры пользователя
   *
   */
  nbResult enumerateContainers(QString userId, QStringList &containers);

  /* Получить одноразовый контейнер
   *
   */
  nbResult getContainer(QString userId, Nbc &container);



  /* Аутентифицироваться
   *
   */
  nbResult authenticate(QString userId, Nb::Data &key, bool &accessGranted);

  /* Аутентифицироваться биометрически с помощью контейнера
   *
   */
  nbResult authenticateBio(QString userId, Nb::Data &keyFromBiometrics, bool &accessGranted);



  /* Зарегистрировать биометрический контейнер с указанными данными
   * Доступно после любой аутентификации
   *
   */
  nbResult registerContainer(QString userId, Nbc &container, QList<Nb::Matrix*> &ownParams, Statistics& stats);

  /* Протестировать зарегистрированный контейнер
   *
   */
  nbResult testContainer(QList<Nb::Matrix*> &testParams);

  /* Подтверить регистрацию контейнера
   *
   */
  nbResult confirmContainer();



  /* Подписать данные биометрической меткой
   * Доступно после биометрической аутентификации
   *
   */
  nbResult signData(QString userId, Nb::Data &data, Nb::Data &signature);



  /* Отменить операцию
   *
   */
  nbResult cancel();
};

#endif // CLIENT_H
