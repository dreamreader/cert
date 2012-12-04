#include "client.h"

static QTcpSocket Client::_socket;

// Запустить сессию
bool Client::start()
{
  return false;
}


// Остановить сессию
bool Client::stop()
{
  return false;
}

// Получить контейнер
nbResult Client::getContainer(QString userId, Nbc &container)
{
  return nbS_OK;
}

// Аутентифицироваться
nbResult Client::authenticate(QString userId, Nb::Data &key, bool &accessGranted)
{
  return nbS_OK;
}

// Аутентифицироваться биометрически с помощью контейнера
nbResult Client::authenticateBio(QString userId, Nb::Data &keyFromBiometrics, bool &accessGranted)
{
  return nbS_OK;
}

// Зарегистрировать биометрический контейнер с указанными данными
nbResult Client::registerContainer(QString userId, Nbc &container, QList<Nb::Matrix*> &ownParams, Statistics& stats)
{
  return nbS_OK;
}

// Протестировать зарегистрированный контейнер
nbResult Client::testContainer(QList<Nb::Matrix*> &testParams)
{
  return nbS_OK;
}

// Подтверить регистрацию контейнера
nbResult Client::confirmContainer()
{
  return nbS_OK;
}

// Подписать данные биометрической меткой
nbResult Client::signData(QString userId, Nb::Data &data, Nb::Data &signature)
{
  return nbS_OK;
}
