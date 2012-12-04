#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include "../protocol/log.h"
#include "../protocol/queries.h"
#include "../nb/nb.h"
#include "../model/Nbc.h"

#define tcpServerAddress      ("localhost")
#define tcpServerPort         (31111)

/// Класс клиента биометрического удостоверяющего центра
class Client: public QObject
{
  Q_OBJECT

private:
  static QTcpSocket  _socket;        ///< сокет

public:

  /* Запустить сессию
   *
   */
  static bool start(QString address = tcpServerAddress, int port = tcpServerPort);

  /* Остановить сессию
   *
   */
  static bool stop();

  /* Получить контейнер
   *
   */
  static nbResult enumerateContainers(QString userId, QStringList &containers);

  /* Получить контейнер
   *
   */
  static nbResult getContainer(QString userId, Nbc &container);



  /* Аутентифицироваться
   *
   */
  static nbResult authenticate(QString userId, Nb::Data &key, bool &accessGranted);

  /* Аутентифицироваться биометрически с помощью контейнера
   *
   */
  static nbResult authenticateBio(QString userId, Nb::Data &keyFromBiometrics, bool &accessGranted);



  /* Зарегистрировать биометрический контейнер с указанными данными
   * Доступно после любой аутентификации
   *
   */
  static nbResult registerContainer(QString userId, Nbc &container, QList<Nb::Matrix*> &ownParams, Statistics& stats);

  /* Протестировать зарегистрированный контейнер
   *
   */
  static nbResult testContainer(QList<Nb::Matrix*> &testParams);

  /* Подтверить регистрацию контейнера
   *
   */
  static nbResult confirmContainer();



  /* Подписать данные биометрической меткой
   * Доступно после биометрической аутентификации
   *
   */
  static nbResult signData(QString userId, Nb::Data &data, Nb::Data &signature);



  /* Отменить операцию
   *
   */
  static nbResult cancel();
};

#endif // CLIENT_H
