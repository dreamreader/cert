#ifndef SESSIONCONTEXT_H
#define SESSIONCONTEXT_H

#include <QTcpSocket>
#include <QLibrary>
#include "serverdatabase.h"
#include "nb/nb.h"
#include "nb/nbc.h"
#include "nb/nbcc.h"
#include "nb/dispatcher.h"
#include "nb/nbclasses.h"
#include "nb/statistics.h"

static const nbUuid nbUUID_NBCC =
{0x9ac0fe89, 0x596e, 0x4414, { 0x94, 0x36, 0x5, 0x12, 0x2b, 0xd9, 0x4d, 0x34}};

static const nbUuid nbUUID_SIGN_PROCESSOR =
  {0x702cbb42,0x7367,0x48cf,{0xbe,0xd1,0xd7,0x8d,0x17,0x74,0xba,0x52}};
static const nbUuid nbUUID_FINGER_PROCESSOR =
  {0x6b2e0b0b,0xff32,0x4db7,{0xb2,0x2f,0x2e,0x60,0xf6,0xd3,0xc6,0x8c} };

#define   nbhandwrLibPath ("C:/program files/BioCertificationCenter/bin/modules/nbhandwr_base")
#define   nbfingerLibPath ("C:/program files/BioCertificationCenter/bin/modules/nbfinger_base")
#define   nbccLibPath     ("C:/program files/BioCertificationCenter/bin/modules/nbcc")
#define   querySymbol     ("NbQueryComponent")
#define   passwordSize    256

///Класс контекста сессии биометрического удостоверяющего центра
class SessionContext
{
public:
  QTcpSocket&             socket;           ///< сокет клиента
  ServerDatabase*         database;         ///< база данных сервера

  Dispatcher              dispatcher;       ///< диспетчер компонентов
  Nbcc                    nbcc;             ///< НПБК
  Nb::Meta                outMeta;          ///< метаописание одноразового ключа

  QString                 userId;           ///< идентификатор пользователя
  QStringList             containers;   ///< контейнеры пользователя
  QString                 containerId;      ///< идентификатор текущего контейнера
  Nbc*                    container;        ///< контейнер со схемой аутентификации
  QString                 regkey;           ///< ключ регистрации
  QList<Nb::Matrix*>      ownBimParams;     ///< биометрические параметры "Свой" для обучения
  QList<Nb::Matrix*>      allBimParams;     ///< биометрические параметры "Свой" для обучения
  QList<Nb::Matrix*>      testBimParams;    ///< биометрические параметры для тестирования
  Nb::Matrix              outCode;          ///< выходной код
  Statistics              stats;            ///< статистика обучения
  Nb::Data                data;             ///< исходные данные
  Nb::Data                signature;        ///< сформированная подпись

public:
  SessionContext(QTcpSocket &clientSocket);
  ~SessionContext();

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
};

#endif // CLIENTCONTEXT_H
