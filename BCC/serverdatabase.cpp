#include "serverdatabase.h"

// Создать базу данных
bool ServerDatabase::create()
{
  // Открыть базу
  QSqlDatabase base;
  base = QSqlDatabase::addDatabase("QSQLITE");
  base.setDatabaseName(databasePath);
  base.setHostName("localhost");
  if (!base.open()) return false;

  QSqlQuery query(base);
  QString queryCode;

  // Создать таблицы базы данных
  queryCode  = "CREATE TABLE IF NOT EXISTS RegistrationKey ";
  queryCode += "( N_Id           INTEGER PRIMARY KEY AUTOINCREMENT, ";
  queryCode += "  userId         VARCHAR(256) NOT NULL, ";
  queryCode += "  regkey         VARCHAR(256) NOT NULL,";
  queryCode += "  UNIQUE (userId, regkey) );";
  if (!query.exec(queryCode)) return false;

  queryCode  = "CREATE TABLE IF NOT EXISTS ContainerName ";
  queryCode += "( N_Id           INTEGER PRIMARY KEY AUTOINCREMENT, ";
  queryCode += "  userId         VARCHAR(256) NOT NULL, ";
  queryCode += "  conId          VARCHAR(256) NOT NULL,";
  queryCode += "  UNIQUE (userId, conId) );";
  if (!query.exec(queryCode)) return false;

  queryCode  = "CREATE TABLE IF NOT EXISTS ContainerData ";
  queryCode += "( D_Id           INTEGER PRIMARY KEY, ";
  queryCode += "  conPath        VARCHAR(256) NOT NULL, ";
  queryCode += "  bimsPath       VARCHAR(256) NOT NULL, ";
  queryCode += "  FOREIGN KEY(D_Id) REFERENCES ContainerName(N_Id) );";
  if (!query.exec(queryCode)) return false;

  return true;
}

// Открыть базу данных
ServerDatabase* ServerDatabase::open()
{
  ServerDatabase* cbase = new ServerDatabase();
  cbase->_base = QSqlDatabase::addDatabase("QSQLITE");
  cbase->_base.setDatabaseName(databasePath);
  cbase->_base.setHostName("localhost");
  if (!cbase->_base.open()) cbase = nbNULL;
  return cbase;
}

// Начать транзакцию
bool ServerDatabase::beginTransaction()
{
  return _base.transaction();
}

// Откатить транзакцию
bool ServerDatabase::rollbackTransaction()
{
  return _base.rollback();
}

// Зафиксировать транзакцию
bool ServerDatabase::commitTransaction()
{
  return _base.commit();
}

// Добавить ключ регистрации пользователю
bool ServerDatabase::addAuthenticationKey(QString userId, QString regkey)
{
  LOG
  Log::write("add regkey call");
  QSqlQuery query(_base);
  QString queryCode;

  queryCode  = "INSERT INTO RegistrationKey VALUES ";
  queryCode += "(NULL, '" + userId + "', '" + regkey +"') ";
  if (!query.exec(queryCode)) goto rollback;
  return true;

rollback:
  Log::write("error: " + query.lastError().text());
  qDebug() << query.lastError();
  return false;
}

// Проверить наличие ключа регистрации у пользователя
bool ServerDatabase::useAuthenticationKey(QString userId, QString regkey, bool &ok)
{
  LOG
  Log::write("check regkey call");
  QSqlQuery query(_base);
  QString queryCode;

  queryCode  = "SELECT COUNT(regkey) FROM RegistrationKey ";
  queryCode += "WHERE userId='" + userId + "' AND regkey='" + regkey + "'";
  if (!query.exec(queryCode)) goto rollback;

  if (!query.first()) goto rollback;
  Log::write("query columns count", query.record().count());
  if (!query.first() || (query.record().count() != 1))  goto rollback;
  ok = (query.value(0) != 0);

  queryCode  = "DELETE FROM RegistrationKey ";
  queryCode += "WHERE userId='" + userId + "' AND regkey='" + regkey + "'";
  if (!query.exec(queryCode)) goto rollback;
  return true;

rollback:
  Log::write("error: " + query.lastError().text());
  qDebug() << query.lastError();
  return false;
}

// Проверить наличие контейнера у пользователя
bool ServerDatabase::containerExists(QString userId, QString containerId, bool &exist)
{
  LOG
  Log::write("check container call");
  QSqlQuery query(_base);
  QString queryCode;

  queryCode  = "SELECT COUNT(conId) FROM ContainerName ";
  queryCode += "WHERE userId='" + userId + "' AND conId='" + containerId + "'";
  if (!query.exec(queryCode)) goto rollback;

  if (!query.first()) goto rollback;
  Log::write("query columns count", query.record().count());
  if (!query.first() || (query.record().count() != 1))  goto rollback;
  exist = (query.value(0) != 0);
  return true;

rollback:
  Log::write("error: " + query.lastError().text());
  qDebug() << query.lastError();
  return false;
}

// Добавить контейнер и сертификаты с указанным идентификатором для указанного пользователя
bool ServerDatabase::addContainer(QString userId, QString containerId, Nb::Container &container, QList<Nb::Matrix*> &bimParams)
{
  LOG
  Log::write("add container call");
  QString containerDirPath = containerPathEx + userId;
  QString containerFilePath = containerDirPath;
  QString bimsDirPath = bimsPathEx + userId;
  QString bimsFilePath = bimsDirPath;

  containerFilePath += "/";
  containerFilePath += containerId;
  containerFilePath += ".nbc";
  bimsFilePath += "/";
  bimsFilePath += containerId;
  bimsFilePath += ".bim";

  QDir dir;
  dir.mkpath(containerDirPath);
  dir.mkpath(bimsDirPath);

  Nb::Block bimsBlock;
  for (int i=0; i<bimParams.size(); i++)
    bimsBlock.push(*bimParams[i]);

  QDataStream stream;
  QFile containerFile(containerFilePath);
  QFile bimsFile(bimsFilePath);
  containerFile.open(QFile::WriteOnly);
  bimsFile.open(QFile::WriteOnly);
  stream.setDevice(&containerFile);
  container.write(stream);
  stream.setDevice(&bimsFile);
  bimsBlock.write(stream);

  QSqlQuery query(_base);
  QString queryCode;

  bool exist;
  containerExists(userId, containerId, exist);
  if (!exist)
  {
    queryCode  = "INSERT INTO ContainerName VALUES ";
    queryCode += "(NULL, '"+userId+"', '"+containerId+"') ";
    if (!query.exec(queryCode)) goto rollback;

    queryCode  = "INSERT INTO ContainerData VALUES ";
    queryCode += "((SELECT N_Id FROM ContainerName WHERE rowid=last_insert_rowid()), ";
    queryCode += "'" + containerFilePath + "', '" + bimsFilePath + "')";
    if (!query.exec(queryCode)) goto rollback;
  }
  return true;

rollback:
  Log::write("error: " + query.lastError().text());
  qDebug() << query.lastError();
  return false;
}

// Удалить указанную схему аутентификации
bool ServerDatabase::deleteContainer(QString userId, QString containerId)
{
  return false;
}

// Получить схему аутентификации
bool ServerDatabase::getContainer(QString userId, QString containerId, Nb::Container &container)
{
  LOG
  Log::write("get container call");
  QDataStream stream;
  QFile containerFile;
  QString containerFilePath;

  QSqlQuery query(_base);
  QString queryCode;

  queryCode  = "SELECT conPath FROM ContainerData ";
  queryCode += "WHERE (D_id IN ";
  queryCode += "(SELECT N_Id FROM ContainerName ";
  queryCode += "WHERE (userId='" + userId + "' AND conId='" + containerId + "')))";
  if (!query.exec(queryCode)) goto rollback;

  if (!query.first()) goto rollback;
  if (!query.first() || (query.record().count() != 1))  goto rollback;
  Log::write("query value(0)", query.value(0).isValid());
  containerFilePath = query.value(0).toString();
  Log::write(containerFilePath);
  containerFile.setFileName(containerFilePath);
  containerFile.open(QFile::ReadOnly);
  stream.setDevice(&containerFile);

  container.read(stream);
  return true;

rollback:
  Log::write(queryCode);
  Log::write("error: " + query.lastError().text());
  qDebug() << query.lastError();
  return false;
}

// Получить биометрические образы "Свой"
bool ServerDatabase::getBims(QString userId, QString containerId, QList<Nb::Matrix*> &bimParams)
{
  LOG
  Log::write("get bims call");
  Nb::Block bimsBlock;
  QDataStream stream;
  QFile bimsFile;
  QString bimsFilePath;
  int pos(0);

  QSqlQuery query(_base);
  QString queryCode;

  queryCode  = "SELECT bimsPath FROM ContainerData ";
  queryCode += "WHERE (D_id IN";
  queryCode += "(SELECT N_Id FROM ContainerName ";
  queryCode += "WHERE (userId='" + userId + "' AND conId='" + containerId + "')))";
  if (!query.exec(queryCode)) goto rollback;

  if (!query.first()) goto rollback;
  if (!query.first() || (query.record().count() != 1))  goto rollback;
  Log::write("query value(0)", query.value(0).isValid());
  bimsFilePath = query.value(0).toString();
  bimsFile.setFileName(bimsFilePath);
  Log::write(bimsFilePath);
  bimsFile.open(QFile::ReadOnly);

  pos = 0;
  stream.setDevice(&bimsFile);
  bimsBlock.read(stream);
  for (int i=0; i<bimParams.size(); i++)
    bimsBlock.pop(pos, *bimParams[i]);
  return true;

rollback:
  Log::write("error: " + query.lastError().text());
  qDebug() << query.lastError();
  return false;
}

// Получить идентификаторы всех контейнеров
bool ServerDatabase::getContainers(QString userId, QStringList& containers)
{
  LOG
  Log::write("get containers call");
  QSqlQuery query(_base);
  QString queryCode;

  queryCode  = "SELECT conId FROM ContainerName ";
  queryCode += "WHERE userId='" + userId + "'";
  if (!query.exec(queryCode)) goto rollback;

  Log::write("query columns count", query.record().count());
  if (query.record().count() != 1) goto rollback;
  containers.clear();

  while (query.next())
  {
    Log::write("push container: " + query.value(0).toString());
    containers.push_back(query.value(0).toString());
  }

  return true;

rollback:
  Log::write("error: " + query.lastError().text());
  qDebug() << query.lastError();
  return false;
}
