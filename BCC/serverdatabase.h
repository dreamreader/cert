#ifndef CONTAINERBASE_H
#define CONTAINERBASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QVector>
#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include "protocol/log.h"
#include "nb/nb.h"
#include "nb/nbclasses.h"

#define       databasePath      ("C:/program files/BioCertificationCenter/data/bcc.sqlite3.db")
#define       containerPathEx   ("C:/program files/BioCertificationCenter/data/containers/")
#define       bimsPathEx        ("C:/program files/BioCertificationCenter/data/bims/")

///Класс базы данных
class ServerDatabase
{
private:
  QSqlDatabase    _base;          ///<база данных

public:
  /* Создать базу данных
   */
  static bool create();

  /* Открыть соединение с базой данных
   */
  static ServerDatabase* open();

  /* Начать транзакцию
   *
   */
  bool beginTransaction();

  /* Откатить транзакцию
   *
   */
  bool rollbackTransaction();

  /* Зафиксировать транзакцию
   *
   */
  bool commitTransaction();

  /* Добавить ключ регистрации пользователю
   *
   */
  bool addRegisterKey(QString userId, QString regkey);

  /* Проверить наличие ключа регистрации у пользователя
   *
   */
  bool checkRegisterKey(QString userId, QString regkey, bool &exist);

  /* Проверить существование у пользователя заданного контейнера
   *
   */
  bool containerExists(QString userId, QString containerId, bool &exist);

  /* Добавить схему аутентификации пользователю
   *
   */
  bool addContainer(QString userId, QString schemeId, Nb::Container &container, QList<Nb::Matrix*> &bimParams, QString &regkey);

  /* Удалить указанную схему аутентификации
   *
   */
  bool deleteContainer(QString userId, QString containerId);

  /* Получить идентификаторы всех схем аутентификации
   *
   */
  bool getContainers(QString userId, QStringList &containers);

  /* Получить схему аутентификации
   *
   */
  bool getContainer(QString userId, QString schemeId, Nb::Container &container);

  /* Получить биометрические образы "Свой"
   *
   */
  bool getBims(QString userId, QString schemeId, QList<Nb::Matrix*> &bimParams);
};

#endif // CONTAINERBASE_H
