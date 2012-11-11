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
    case Query::WantRegister:
      Log::write("WantRegister");
      return startRegister(header);
      break;

    case Query::WantSignature:
      Log::write("WantSignature");
      return startSignature(header);
      break;

    case Query::WantContainers:
      return enumerateContainers(header);
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

// Перечислить имена контейнеров для пользователя
bool SessionTask::enumerateContainers(Query::QueryHeaderBlock &header)
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
  // --- Получить имя пользователя
  // ---
  EnumerateContainersQuery enumQuery;
  enumQuery.create(header);
  enumQuery.read(_session.socket);
  if (!enumQuery.isOk())
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  Log::write("---> WANT_CONTAINERS: enumerate containers");
  enumQuery.get(_session.userId);

  // Получить список имён контейнеров
  _session.database->getContainers(_session.userId, _session.containers);

  // ------------------------------------------------------------------------------------
  // --- Отправить список имён контейнеров
  // ---
  Log::write("<--- ANS: send container names");
  if (!checkServer())
  {
    stop(/*nbE_SERVER_STOPPED*/);
    return false;
  }
  ContainerNamesQuery contQuery;
  contQuery.create(_session.containers);
  contQuery.write(_session.socket);
}


// Запустить обработку задачи: регистрация
bool SessionTask::startRegister(Query::QueryHeaderBlock &header)
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
  StartRegisterQuery startRegQuery;
  startRegQuery.create(header);
  startRegQuery.read(_session.socket);
  if (!startRegQuery.isOk())
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  Log::write("---> WANT_REGISTER: start registration, check ids and regkey");
  startRegQuery.get(_session.userId, _session.containerId, _session.regkey);

  // Проверить наличие ключа регистрации
  bool exist(false);
  _session.database->addRegisterKey(_session.userId, _session.regkey);   //TMP: ЧИСТО ДЛЯ ТЕСТА
  _session.database->checkRegisterKey(_session.userId, _session.regkey, exist);
  Log::write("regkey confirmed ", exist);
  if (!exist)
  {
    stop(/*nbE_INVALID_REGISTRATION_KEY*/);
    return false;
  }

  // Проверить уникальность имени контейнера
  _session.database->containerExists(_session.userId, _session.containerId, exist);
  Log::write("container not exists ", !exist);
  if (exist)
  {
    stop(/*nbE_CONTAINER_ALREADY_EXISTS*/);
    return false;
  }

  // ------------------------------------------------------------------------------------
  // --- Отправить подтверждение
  // ---
  Log::write("<--- WAIT_DATA: confirm ids and regkey");
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
  Log::write("---> DATA: container");

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
  Log::write("<--- WAIT_DATA: waiting for own bims");
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
  Log::write("---> DATA: own bims");

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
  OneTimePassword password(_session.outMeta.count());

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
  Log::write("<--- STATS: send learn statistics");
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
      Log::write("---> DATA: own bims");

      // Переобучить НПБК (база чужой сформирована)
      if (!_session.nbcc.learn(*_session.container, _session.ownBimParams, _session.allBimParams, password, _session.stats))
      {
        stop(/*nbE_INTERNAL_ERROR*/);
        return false;
      }

      // ------------------------------------------------------------------------------------
      // --- Отправить статистику
      // ---
      Log::write("<--- STATS: send learn statistics");
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
      Log::write("               header count:", header.getCount());
      testQuery.clear();
      Log::write("   after clear header count:", header.getCount());
      testQuery.create(header);
      testQuery.read(_session.socket);
      if (!testQuery.isOk())
      {
        stop(/*nbE_UNEXPECTED_QUERY*/);
        return false;
      }
      Log::write("---> TEST: test bim");

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
      resultQuery.create(accessGranted, _session.outCode);
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
  Log::write("---> OK: data confirmed");

  // Внести изменения в базу данных
  if (!_session.database->addContainer(_session.userId, _session.containerId, *_session.container, _session.ownBimParams, _session.regkey))
  {
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }
  _session.commitTransaction();

  // ------------------------------------------------------------------------------------
  // --- Отправить подтверждение
  // ---
  Log::write("<--- OK: confirm register");
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



// Запустить обработку задачи: подписывание документа
bool SessionTask::startSignature(Query::QueryHeaderBlock &header)
{
  LOG
  Query query;
  Log::write("startSignature");

  // Проверить НПБК и базу данных
  if(!_session.nbcc.isCreated() || !_session.database)
  {
    Log::write("nccc || database error");
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }

  // Начать транзакцию
  _session.beginTransaction();

  // ------------------------------------------------------------------------------------
  // --- Получить идентификаторы пользователя и схемы
  // ---
  StartSignatureQuery startSigQuery;
  startSigQuery.create(header);
  Log::write("start read startSigQuery");
  startSigQuery.read(_session.socket);
  if (!startSigQuery.isOk())
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  Log::write("---> WANT_SIGNATURE: start creating signature");
  startSigQuery.get(_session.userId, _session.containerId);

  // Загрузить схему и образы из базы данных
  _session.container = new Nbc;
  if (!_session.database->getContainer(_session.userId, _session.containerId, *_session.container))
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

  if (!_session.database->getBims(_session.userId, _session.containerId, _session.ownBimParams))
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

  // Сформировать контейнер одноразового пароля
  OneTimePassword password(_session.outMeta.count(), false, 0x55);
  if (!_session.nbcc.learn(*_session.container, _session.ownBimParams, _session.allBimParams, password, _session.stats))
  {
    stop(/*nbE_INTERNAL_ERROR*/);
    return false;
  }

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

  bool accessGranted = false;
  header.free();
  header.read(_session.socket);
  while (header.typeIs(Query::Password) && !accessGranted)    //TODO: количество попыток добавить
  {
    // ------------------------------------------------------------------------------------
    // --- Получить пароль
    // ---
    PasswordQuery passwordQuery;
    passwordQuery.create(header);
    passwordQuery.read(_session.socket);
    if (!passwordQuery.isOk())
    {
      stop(/*nbE_UNEXPECTED_QUERY*/);
      return false;
    }
    Log::write("---> PASSWORD: one-time password");
    passwordQuery.get(_session.outCode);

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
    ResultQuery resultQuery;
    resultQuery.create(accessGranted, _session.outCode);
    resultQuery.write(_session.socket);

    header.free();
    header.read(_session.socket);
  }

  // ------------------------------------------------------------------------------------
  // --- Принять документ
  // ---
  Log::write("---> DATA: document");
  DocumentQuery documentQuery;
  documentQuery.create(header);
  documentQuery.read(_session.socket);
  if (!documentQuery.isOk())
  {
    stop(/*nbE_UNEXPECTED_QUERY*/);
    return false;
  }
  if (!accessGranted)
  {
    stop(/*nbE_ACCESS_DENIED*/);
    return false;
  }
  documentQuery.get(_session.data);

  // Подписать документ
  QString sigStr;
  sigStr  = QString::fromUtf8("Пользователь ")
          + _session.userId
          + QString::fromUtf8("аутентифицировался с помощью схемы \"")
          + _session.containerId
          + QString::fromUtf8("\" ");
          //+ QDateTime::currentDateTime.toString();
  int size(0);
  _session.signature.fromString(sigStr, size, false);

  // ------------------------------------------------------------------------------------
  // --- Отправить документ
  // ---
  if (!checkServer())
  {
    stop(/**/);
    return false;
  }
  Log::write("<--- DATA: signed document");
  documentQuery.clear();
  documentQuery.create(_session.signature);
  query.write(_session.socket);

  _session.commitTransaction();

  for (int i=0; i<_session.ownBimParams.size(); i++)
    delete _session.ownBimParams[i];
  for (int i=0; i<_session.allBimParams.size(); i++)
    delete _session.allBimParams[i];

  _session.ownBimParams.clear();
  _session.allBimParams.clear();

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
