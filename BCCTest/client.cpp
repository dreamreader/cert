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
bool Client::start()
{
  LOG

  Log::write("connect to server...");
  QHostAddress addr(tcpServerAddress);
  _socket.connectToHost(QHostAddress::LocalHost, tcpServerPort);
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

// Перечислить контейнеры пользователя
nbResult Client::enumerateContainers(QString userId, QStringList &containers)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить идентификаторы и ключ регистрации
  // ---
  Log::write("<--- WANT_CONTAINERS: enumerate containers");
  EnumerateContainersQuery enumQuery;
  enumQuery.create(userId);
  enumQuery.write(_socket);

  // ------------------------------------------------------------------------------------
  // --- Получить запрос на ввод контейнера
  // ---
  ContainerNamesQuery contQuery;
  contQuery.read(_socket);
  if (!contQuery.isOk() ||
      !contQuery.get(containers))
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  Log::write("---> ANS: container names list");

  return nbS_OK;
}


//Начать регистрацию
nbResult Client::startRegistration(QString userId, QString containerId, QString &regkey)
{
  LOG
  Query query;

  // ------------------------------------------------------------------------------------
  // --- Отправить идентификаторы и ключ регистрации
  // ---
  Log::write("<--- WANT_REGISTER: start register");
  StartRegisterQuery startRegQuery;
  startRegQuery.create(userId, containerId, regkey);
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
  Log::write("---> WAIT_DATA: server waiting for cointainer");

  return nbS_OK;
}


// Зарегистрировать указанный контейнер
nbResult Client::registerContainer(Nbc &container)
{
  LOG
  Query query;

  // ------------------------------------------------------------------------------------
  // --- Отправить контейнер
  // ---
  Log::write("<--- DATA: send container");
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
  Log::write("---> WAIT_DATA: server waiting for own bims");

  return nbS_OK;
}


// Зарегистрировать указанные биометрические данные
nbResult Client::registerData(QList<Nb::Matrix*> &ownParams, QList<Nb::Matrix*> &allParams, Statistics& stats, bool repeat)
{
  LOG
  Query query;

  // ------------------------------------------------------------------------------------
  // --- Отправить образы "Свой"
  // ---
  Log::write("<--- DATA: send own bims");
  BimParametersQuery bimParamQuery;
  bimParamQuery.create(ownParams, repeat);
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


// Протестировать
nbResult Client::testData(QList<Nb::Matrix*> &testParams)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить данные
  // ---
  Log::write("<--- TEST: send bims");
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
    Log::write("---> RESULT: access granted");
  else
    Log::write("---> RESULT: access denied");

  return nbS_OK;
}


// Подтверить регистрацию
nbResult Client::confirmRegistration()
{
  LOG
  Query query;

  // ------------------------------------------------------------------------------------
  // --- Отправить подтверждение
  // ---
  Log::write("<--- OK: confirm data");
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

  return nbS_OK;
}


// Начать процедуру подписывания документа
nbResult Client::startSign(QString userId, QString containerId, Nbc &container)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить идентификаторы
  // ---
  Log::write("<--- WANT_SIGNATURE: start creating signature");
  StartSignatureQuery startSigQuery;
  startSigQuery.create(userId, containerId);
  startSigQuery.write(_socket);

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
  Log::write("---> ANS: container");

  return nbS_OK;
}


// Использовать одноразовый пароль
nbResult Client::usePassword(Nb::Matrix &password, bool &accessGranted)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить одноразовый пароль
  // ---
  Log::write("<--- PASSWORD: one-time password ");
  PasswordQuery passwordQuery;
  passwordQuery.create(password);
  passwordQuery.write(_socket);

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
    Log::write("---> RESULT: access granted");
  else
    Log::write("---> RESULT: access denied");

  return nbS_OK;
}


// Подписать документ
nbResult Client::signDocument(Nb::Data document, Nb::Data signature)
{
  LOG

  // ------------------------------------------------------------------------------------
  // --- Отправить документ
  // ---
  Log::write("<--- DATA: document");
  DocumentQuery documentQuery;
  documentQuery.create(document);
  documentQuery.write(_socket);

  // ------------------------------------------------------------------------------------
  // --- Принять подпись
  // ---
  Log::write("---> DATA: signature");
  documentQuery.clear();
  documentQuery.read(_socket);
  if (!documentQuery.typeIs(Query::Data))
  {
    stop();
    return nbE_FAIL/*nbE_UNEXPECTED_QUERY*/;
  }
  documentQuery.get(signature);
  Log::write("        : " + signature.toString() );

  return nbS_OK;
}


// Слот - открыть сессию
void Client::openSession()
{
  LOG
  Log::write("client connected");
  emit sessionOpened();
}
