#include "sessiontask.h"

// Конструктор
SessionTask::SessionTask(SessionContext &session, const ServerContext &server):
  _session(session), _server(server) {}

// Запустить задачу
bool SessionTask::start()
{
  LOG

  Log::write("start task");
  Query::QueryHeaderBlock header;
  if (!header.read(_session.socket)) return false;

  Log::write("query type", header.getType());
  switch (header.getType())
  {
    case Query::WantAuth:
      Log::write("WantAuth");
      return authenticate(header);
      break;

    case Query::WantAuthBio:
      Log::write("WantAuthBio");
      return authenticateBio(header);
      break;

    case Query::WantContainer:
      Log::write("WantContainer");
      return createContainer(header);
      break;

    case Query::WantSignature:
      Log::write("WantSignature");
      return signData(header);
      break;

    case Query::WantRegister:
      Log::write("WantRegister");
      return registerContainer(header);
      break;

    default:
      Log::write("unknown task type ", header.getType());
      stop(/*неизвестная задача*/);
      break;
  }

  return true;
}

// Проверить состояние сервера
bool SessionTask::checkServer()
{
  return (_server.getState() != ServerContext::Stopped);
}

// Выполнить аутентификацию
bool SessionTask::authenticate(Query::QueryHeaderBlock &header)
{
  LOG

  // Проверить НПБК и базу данных
  if(!_session.nbcc.isCreated() || !_session.database)
  {
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }
  // Начать транзакцию
  _session.beginTransaction();

  // ------------------------------------------------------------------------------------
  // --- Получить код аутентификации
  // ---
  AuthQuery authQuery;
  authQuery.create(header);
  authQuery.read(_session.socket);
  if (!authQuery.isOk())
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  Log::write("---> WantAuth: start authentication, check authentication key");
  authQuery.get(_session.userId, _session.authKey);

  // Проверить наличие кода аутентификации
  _session.authenticated = false;
  _session.database->addAuthenticationKey(_session.userId, _session.authKey.toString());   //TMP: ЧИСТО ДЛЯ ТЕСТА
  _session.database->useAuthenticationKey(_session.userId, _session.authKey.toString(), _session.authenticated);
  Log::write("regkey confirmed ", _session.authenticated);

  // ------------------------------------------------------------------------------------
  // --- Отправить результат
  // ---
  if (_session.authenticated)
    Log::write("<--- Result: access granted");
  else
    Log::write("<--- Result: access denied");
  ResultQuery resultQuery;
  resultQuery.create(_session.authenticated, nbNULL);
  resultQuery.write(_session.socket);

  //Завершить транзакцию
  _session.commitTransaction();
  return true;
}

// Выполнить биометрическую аутентификацию
bool SessionTask::authenticateBio(Query::QueryHeaderBlock &header)
{
  LOG

  // Проверить НПБК и базу данных
  if(!_session.nbcc.isCreated() || !_session.database)
  {
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }
  // Начать транзакцию
  _session.beginTransaction();

  // ------------------------------------------------------------------------------------
  // --- Получить код аутентификации
  // ---
  AuthQuery authQuery;
  authQuery.create(header);
  authQuery.read(_session.socket);
  if (!authQuery.isOk())
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  Log::write("---> WantAuth: start biometric authentication, check one-time key");
  authQuery.get(_session.userId, _session.authBioKey);

  // Проверить наличие одноразового контейнера
  if (!_session.containerCreated)
  {
    Log::write("no one-time container");
    _session.authenticatedBio = false;
    return false;
  }
  else
  {
    _session.authenticatedBio = (_session.containerKey.isEqual(_session.authBioKey));
  }

  // ------------------------------------------------------------------------------------
  // --- Отправить результат
  // ---
  if (_session.authenticatedBio)
  {
    //Сбросить контейнер
    _session.containerCreated = false;
    Log::write("<--- Result: access granted");
  }
  else
    Log::write("<--- Result: access denied");
  ResultQuery resultQuery;
  resultQuery.create(_session.authenticatedBio, nbNULL);
  resultQuery.write(_session.socket);

  //Завершить транзакцию
  _session.commitTransaction();
  return true;
}

// Создать одноразовый контейнер для биометрической аутентификации
bool SessionTask::createContainer(Query::QueryHeaderBlock &header)
{
  LOG

  // Проверить НПБК и базу данных
  if(!_session.nbcc.isCreated() || !_session.database)
  {
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }
  // Начать транзакцию
  _session.beginTransaction();

  // ------------------------------------------------------------------------------------
  // --- Получить идентификатор пользователя
  // ---
  GetContainerQuery getQuery;
  getQuery.create(header);
  getQuery.read(_session.socket);
  if (!getQuery.isOk())
  {
    Log::write("err GetContainerQuery");
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  Log::write("---> WantContainer: start creating one-time container");
  getQuery.get(_session.userId);

  // Загрузить схему и образы из базы данных
  _session.container = new Nbc;
  if (!_session.database->getContainer(_session.userId, "theone", *_session.container))
  {
    Log::write("get container failed");
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }
  SlotIds inSlots = _session.container->getScheme().ids(Nb::StIn);
  for (int i=0; i < inSlots.size(); i++)
  {
    _session.ownBimParams.push_back(new Nb::Matrix);
  }

  if (!_session.database->getBims(_session.userId, "theone", _session.ownBimParams))
  {
    Log::write("get bims failed");
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }

  // Загрузить и сформировать базу "Чужие"
  for (unsigned i = 0; i != inSlots.size(); i++)
  {
    Processor processor;
    Matrix *foreign = new Matrix;
    Nb::Uuids uuids = _session.container->getSlotInfo(inSlots[i]);
    if (uuids.size() < 2)
    {
      Log::write("error get slot info; uuids size", uuids.size());
      stop(/*nbE_INTERNAL_ERROR*/);
      return false;
    }
    if (!_session.dispatcher.getProcessor(uuids[1], processor, nbNULL))
    {
      Log::write("error get processor");
      stop(/*nbE_INTERNAL_ERROR*/);
      return false;
    }
    if (!processor.getForeignBase(*foreign))
    {
      Log::write("error get foreignBase ");
      stop(/*nbE_INTERNAL_ERROR*/);
      return false;
    }
    _session.allBimParams.push_back(foreign);
  }

  // Сформировать контейнер одноразового пароля
  _session.containerKey.create(_session.authMeta.count(), false, 0x55); //TMP: тест
  if (!_session.nbcc.learn(*_session.container, _session.ownBimParams, _session.allBimParams, _session.containerKey, _session.stats))
  {
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }
  _session.containerCreated = true;

  // ------------------------------------------------------------------------------------
  // --- Отправить сформированный контейнер
  // ---
  Log::write("<--- ANS: one-time container");
  if (!checkServer())
  {
    stop(/**/);
    return false;
  }
  ContainerQuery containerQuery;
  containerQuery.create(*_session.container);
  containerQuery.write(_session.socket);

  for (int i=0; i<_session.ownBimParams.size(); i++)
    delete _session.ownBimParams[i];
  for (int i=0; i<_session.allBimParams.size(); i++)
    delete _session.allBimParams[i];

  _session.ownBimParams.clear();
  _session.allBimParams.clear();

  //Завершить транзакцию
  _session.commitTransaction();
  return true;
}

// Запустить задачу регистрации
bool SessionTask::registerContainer(Query::QueryHeaderBlock &header)
{
  LOG
  Query query;

  // Проверить НПБК и базу данных
  if(!_session.nbcc.isCreated() || !_session.database)
  {
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }

  // Начать транзакцию
  _session.beginTransaction();

  // ------------------------------------------------------------------------------------
  // --- Получить желаемые параметры регистрации (в т.ч. схему преобразования)
  // ---
  StartRegisterQuery regQuery;
  regQuery.create(header);
  regQuery.read(_session.socket);
  if (!regQuery.isOk())
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  Log::write("---> WantRegister: start registration, check ids and regkey");
  regQuery.get(_session.userId);

  // Проверить аутентифицированность
  if (!_session.authenticated && !_session.authenticatedBio)
  {
    stop(/*nbE_ACCESS_DENIED*/);
    return false;
  }

  // ------------------------------------------------------------------------------------
  // --- Отправить запрос на ввод контейнера
  // ---
  Log::write("<--- WaitData: wait for container");
  query.create(Query::WaitData);
  query.write(_session.socket);

  // ------------------------------------------------------------------------------------
  // --- Принять контейнер
  // ---
  ContainerQuery containerQuery;
  containerQuery.read(_session.socket);
  if (!containerQuery.isOk())
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  Log::write("---> Data: container");

  _session.container = containerQuery.container();
  SlotIds inSlots = _session.container->getScheme().ids(Nb::StIn);
  for (int i=0; i < inSlots.size(); i++)
  {
    _session.ownBimParams.push_back(new Nb::Matrix);
    _session.testBimParams.push_back(new Nb::Matrix);
  }
  Log::write("own parts", _session.ownBimParams.size());

  // ------------------------------------------------------------------------------------
  // --- Отправить запрос на ввод образов "Свой"
  // ---
  Log::write("<--- WaitData: waiting for own bims");
  if (!checkServer())
  {
    stop(/*nbE_SERVER_STOPPED*/);
    return false;
  }
  query.create(Query::WaitData);
  query.write(_session.socket);

  // ------------------------------------------------------------------------------------
  // --- Получить образы "Свой"
  // ---
  BimParametersQuery bimParametersQuery;
  bimParametersQuery.read(_session.socket);
  if (!bimParametersQuery.isOk() ||
      !bimParametersQuery.get(_session.ownBimParams) )
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  Log::write("---> Data: own bims");

  // Загрузить и сформировать базу "Чужие"
  for (unsigned i = 0; i != inSlots.size(); i++)
  {
    Processor processor;
    Matrix *foreign = new Matrix;
    Nb::Uuids uuids = _session.container->getSlotInfo(inSlots[i]);
    Log::write("uuids size", uuids.size());
    if (uuids.size() < 2)
    {
      stop(/*nbE_INTERNAL_ERROR*/);
      return false;
    }
    if (!_session.dispatcher.getProcessor(uuids[1], processor, nbNULL))
    {
      Log::write("error get processor");
      stop(/*nbE_INTERNAL_ERROR*/);
      return false;
    }
    if (!processor.getForeignBase(*foreign))
    {
      Log::write("error get foreignBase ");
      stop(/*nbE_INTERNAL_ERROR*/);
      return false;
    }
    _session.allBimParams.push_back(foreign);
  }

  // Обучить НПБК
  OneTimePassword password(_session.authMeta.count());

  Log::write("start learning...");
  if (!_session.nbcc.learn(*_session.container, _session.ownBimParams, _session.allBimParams, password, _session.stats))
  {
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }
  Log::write("learning done");

  _session.allBimParams.clear();

  // ------------------------------------------------------------------------------------
  // --- Отправить статистику
  // ---
  Log::write("<--- Stats: send learn statistics");
  if (!checkServer())
  {
    stop(/*nbE_SERVER_STOPPED*/);
    return false;
  }
  StatsQuery statsQuery;
  statsQuery.create(_session.stats);
  statsQuery.write(_session.socket);

  TestQuery testQuery;
  ResultQuery resultQuery;
  bool accessGranted = false;

  header.free();
  header.read(_session.socket);
  while (!header.typeIs(Query::Ok))
  {
    Log::write("not confirmed, query type:", header.getType());
    Log::write("               header count:", header.getCount());
    switch(header.getType())
    {
    // ------------------------------------------------------------------------------------
    // --- Получить запрос на ввод новых образов
    // ---
    case Query::RepeatData:

      _session.ownBimParams.clear();
      bimParametersQuery.clear();
      bimParametersQuery.create(header);
      bimParametersQuery.read(_session.socket);
      if (!bimParametersQuery.isOk() ||
          !bimParametersQuery.get(_session.ownBimParams) )
      {
        stop(/*nbE_UNEXPECTED_QUERY*/);
        return false;
      }
      Log::write("---> RepeatData: own bims");

      // Переобучить НПБК (база чужой сформирована)
      if (!_session.nbcc.learn(*_session.container, _session.ownBimParams, _session.allBimParams, password, _session.stats))
      {
        stop(/*nbE_INTERNAL_ERROR*/);
        return false;
      }

      // ------------------------------------------------------------------------------------
      // --- Отправить статистику
      // ---
      Log::write("<--- Stats: send learn statistics");
      if (!checkServer())
      {
        stop(/*nbE_SERVER_STOPPED*/);
        return false;
      }
      statsQuery.clear();
      statsQuery.create(_session.stats);
      statsQuery.write(_session.socket);
      break;

    // ------------------------------------------------------------------------------------
    // --- Получить запрос на тестирование
    // ---
    case Query::Test:
      testQuery.clear();
      testQuery.create(header);
      testQuery.read(_session.socket);
      if (!testQuery.isOk())
      {
        stop(/*nbE_UNEXPECTED_QUERY*/);
        return false;
      }
      Log::write("---> Test: test bim");

      // Получить отклик
      testQuery.get(_session.testBimParams);
      Log::write("testBimParams size", _session.testBimParams.size());

      for (int i=0; i<_session.testBimParams.size(); i++)
      {
        if (_session.testBimParams[i]->ncols() != 1)
        {
          Log::write("ncols", _session.testBimParams[i]->ncols());
          stop(/*nbE_INTERNAL_ERROR*/);
          return false;
        }
      }

      Log::write("extract now!");
      _session.nbcc.extract(*_session.container, _session.outCode, _session.testBimParams);

      // ------------------------------------------------------------------------------------
      // --- Отправить результат и значения индикаторов
      // ---
      if (!checkServer())
      {
        stop(/*nbE_SERVER_STOPPED*/);
        return false;
      }
      accessGranted = (password == _session.outCode);
      if (accessGranted)
        Log::write("<--- RESULT: access granted");
      else
        Log::write("<--- RESULT: access denied");
      resultQuery.create(accessGranted, &_session.outCode);
      resultQuery.write(_session.socket);
      break;

      default:
      {
        stop(/*nbE_UNEXPECTED_QUERY*/);
        return false;
      }
    }

    header.free();
    header.read(_session.socket);
  }

  // ------------------------------------------------------------------------------------
  // --- Получить подтверждение данных
  // ---
  Log::write("---> Ok: data confirmed");

  // Внести изменения в базу данных
  if (!_session.database->addContainer(_session.userId, "theone", *_session.container, _session.ownBimParams))
  {
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }

  _session.authenticated = false;
  _session.authenticatedBio = false;
  _session.commitTransaction();

  // ------------------------------------------------------------------------------------
  // --- Отправить подтверждение
  // ---
  Log::write("<--- Ok: confirm register");
  query.create(Query::Ok);
  query.write(_session.socket);

  for (int i=0; i<_session.ownBimParams.size(); i++)
    delete _session.ownBimParams[i];
  for (int i=0; i<_session.testBimParams.size(); i++)
    delete _session.testBimParams[i];
  for (int i=0; i<_session.allBimParams.size(); i++)
    delete _session.allBimParams[i];

  _session.ownBimParams.clear();
  _session.testBimParams.clear();
  _session.allBimParams.clear();
  return true;
}

// Запустить задачу подписывания документа
bool SessionTask::signData(Query::QueryHeaderBlock &header)
{
  // ------------------------------------------------------------------------------------
  // --- Принять документ
  // ---
  Log::write("---> WantSignature: start sign document");
  StartSignatureQuery sigQuery;
  sigQuery.create(header);
  sigQuery.read(_session.socket);
  if (!sigQuery.isOk())
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  if (!_session.authenticatedBio)
  {
    stop(/*nbE_ACCESS_DENIED*/);
    return false;
  }
  sigQuery.get(_session.userId, _session.data);

  // Подписать документ
  QString sigStr;
  sigStr  = QString::fromUtf8("Пользователь ")
          + _session.userId
          + QString::fromUtf8(" или какой-то злоумышленник, кто иж разберёт то, сформировал биометрическую метку ")
          + QDateTime::currentDateTime().toString();
  int size(0);
  _session.signature.fromString(sigStr, size, true);

  // ------------------------------------------------------------------------------------
  // --- Отправить документ
  // ---
  if (!checkServer())
  {
    stop(/**/);
    return false;
  }
  DocumentQuery documentQuery;
  Log::write("<--- Data: signature");
  documentQuery.clear();
  documentQuery.create(_session.signature);
  Log::write("data query type: ", documentQuery.type());
  documentQuery.write(_session.socket);

  _session.commitTransaction();
  return true;
}

bool SessionTask::stop(/*nbResult error*/)
{
  LOG
  Log::write("protocol internal error");
  Query query;
  query.create(Query::Err);
  query.write(_session.socket);
  return true;
}
