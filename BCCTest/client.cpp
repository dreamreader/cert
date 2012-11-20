#include "client.h"

Client::Client()
{
  connect(&_socket, SIGNAL(connected()),
      this, SLOT(openSession()));
}

Client::~Client()
{
  stop();
}

// Запустить сессию
bool Client::start(QString tcpAddress, int port)
{
  LOG

  Log::write("connect to server...");
  QHostAddress addr(tcpAddress);
  _socket.connectToHost(QHostAddress::LocalHost, port);
  _socket.waitForConnected(-1);
  return _socket.isOpen();
}


// Остановить сессию
bool Client::stop()
{
  LOG

  Log::write("stop session!");
  _socket.close();
  emit sessionClosed();
  return !_socket.isOpen();
}

// Получить контейнер
nbResult Client::getContainer(QString userId, Nbc &container)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить запрос на одноразовый контейнер
  // ---
  Log::write("<--- WantContainer: create one-time biometric container");
  GetContainerQuery getContQuery;
  getContQuery.create(userId);
  getContQuery.write(_socket);

  // ---
  // --- Получить динамически сформированный контейнер
  // ---
  ContainerQuery containerQuery;
  containerQuery.read(_socket);
  if (!containerQuery.isOk())
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  containerQuery.container()->copy(container);
  Log::write("---> Ans: container");

  return nbS_OK;
}

// Аутентифицироваться
nbResult Client::authenticate(QString userId, Nb::Data &key, bool &accessGranted)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить запрос на аутентификацию
  // ---
  Log::write("<--- WantAuth: auth bstro blya!");
  AuthQuery authQuery(false);
  authQuery.create(userId, key);
  authQuery.write(_socket);

  // ------------------------------------------------------------------------------------
  // --- Получить результат аутентификации
  // ---
  ResultQuery resultQuery;
  resultQuery.read(_socket);
  if (!resultQuery.isOk())
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  accessGranted = resultQuery.accessGranted();
  if (accessGranted)
    Log::write("---> Access granted");
  else
    Log::write("---> Access denied");

  return nbS_OK;
}

// Аутентифицироваться биометрически с помощью контейнера
nbResult Client::authenticateBio(QString userId, Nb::Data &keyFromBiometrics, bool &accessGranted)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить запрос на аутентификацию
  // ---
  Log::write("<--- WantAuth: auth bstro blya!");
  AuthQuery authQuery;
  Log::write("key size", keyFromBiometrics.size());
  authQuery.create(userId, keyFromBiometrics);
  authQuery.write(_socket);

  // ------------------------------------------------------------------------------------
  // --- Получить результат аутентификации
  // ---
  ResultQuery resultQuery;
  resultQuery.read(_socket);
  if (!resultQuery.isOk())
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  accessGranted = resultQuery.accessGranted();
  if (accessGranted)
    Log::write("---> Access granted");
  else
    Log::write("---> Access denied");

  return nbS_OK;
}

// Зарегистрировать биометрический контейнер с указанными данными
nbResult Client::registerContainer(QString userId, Nbc &container, QList<Nb::Matrix*> &ownParams, Statistics& stats)
{
  LOG
  Query query;

  // ------------------------------------------------------------------------------------
  // --- Отправить идентификатор пользователя
  // ---
  Log::write("<--- WantRegister: register bstro blya!");
  StartRegisterQuery startRegQuery;
  startRegQuery.create(userId);
  startRegQuery.write(_socket);

  // ------------------------------------------------------------------------------------
  // --- Получить запрос на ввод контейнера
  // ---
  query.clear();
  query.read(_socket);
  if (!query.typeIs(Query::WaitData))
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  Log::write("---> WaitData: server waiting for cointainer");

  // ------------------------------------------------------------------------------------
  // --- Отправить контейнер
  // ---
  Log::write("<--- Data: send container");
  ContainerQuery containerQuery;
  containerQuery.create(container);
  containerQuery.write(_socket);

  // ------------------------------------------------------------------------------------
  // --- Получить запрос на ввод образов "Свой"
  // ---
  query.clear();
  query.read(_socket);
  if (!query.typeIs(Query::WaitData))
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  Log::write("---> WaitData: server waiting for own bims");

  // ------------------------------------------------------------------------------------
  // --- Отправить образы "Свой"
  // ---
  Log::write("<--- Data: send own bims");
  BimParametersQuery bimParamQuery;
  bimParamQuery.create(ownParams);
  bimParamQuery.write(_socket);

  // ------------------------------------------------------------------------------------
  // --- Получить статистику обучения
  // ---
  StatsQuery statsQuery;
  statsQuery.read(_socket);
  if (!statsQuery.isOk())
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  statsQuery.get(stats);
  Log::write("---> STATS: e1  " + QString::number(stats.e1()));
  Log::write("            e2  " + QString::number(stats.e2()));
  Log::write("            e2d " + QString::number(stats.e2d()));

  return nbS_OK;
}

// Протестировать зарегистрированный контейнер
nbResult Client::testContainer(QList<Nb::Matrix*> &testParams)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить данные
  // ---
  Log::write("<--- Test: send bims");
  TestQuery testQuery;
  testQuery.create(testParams);
  testQuery.write(_socket);

  // ------------------------------------------------------------------------------------
  // --- Получить результат теста
  // ---
  ResultQuery resultQuery;
  resultQuery.read(_socket);
  if (!resultQuery.isOk())
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  if (resultQuery.accessGranted())
    Log::write("---> Access granted");
  else
    Log::write("---> Access denied");

  return nbS_OK;
}

// Подтверить регистрацию контейнера
nbResult Client::confirmContainer()
{
  LOG
  Query query;

  // ------------------------------------------------------------------------------------
  // --- Отправить подтверждение
  // ---
  Log::write("<--- Ok: confirm data");
  query.create(Query::Ok);
  query.write(_socket);


  // ------------------------------------------------------------------------------------
  // --- Получить подтверждение
  // ---
  query.clear();
  query.read(_socket);
  if (!query.typeIs(Query::Ok))
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  Log::write("---> Ok: confirm data");

  return nbS_OK;
}

// Подписать данные биометрической меткой
nbResult Client::signData(QString userId, Nb::Data &data, Nb::Data &signature)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить данные
  // ---
  Log::write("<--- Data: document");
  StartSignatureQuery startSigQuery;
  startSigQuery.create(userId, data);
  startSigQuery.write(_socket);

  // ------------------------------------------------------------------------------------
  // --- Принять подпись
  // ---
  Log::write("---> Data: signature");
  DocumentQuery documentQuery;
  documentQuery.read(_socket);
  if (!documentQuery.isOk())
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  documentQuery.get(signature);

  return nbS_OK;
}


// Слот - открыть сессию
void Client::openSession()
{
  LOG
  Log::write("client connected");
  emit sessionOpened();
}
