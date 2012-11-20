#include "sessioncontext.h"

// Конструктор
SessionContext::SessionContext(QTcpSocket &clientSocket):
  socket(clientSocket),
  authMeta(passwordSize, Nb::MfI1, Nb::MtDiscreteOwn),
  outCode(1, authMeta)
{
  LOG

  database = ServerDatabase::open();

  try
  {
    nbcc.load(nbUUID_NBCC, nbccLibPath);
    Log::write("nbcc loaded");
  }
  catch (QString str)
  {
    Log::write("ERROR loading nbcc dll:\n" + str);
  }

  try
  {
    dispatcher.loadModule(nbhandwrLibPath);
    Log::write("handwr loaded");
  }
  catch (QString str)
  {
    Log::write("ERROR loading handwr dll:\n" + str);
  }

  try
  {
    dispatcher.loadModule(nbfingerLibPath);
    Log::write("finger loaded");
  }
  catch (QString str)
  {
    Log::write("ERROR loading finger dll:\n" + str);
  }
}

SessionContext::~SessionContext()
{
  delete database;
}

// Начать транзакцию
bool SessionContext::beginTransaction()
{
  database->beginTransaction();
  return true;
}

// Откатить транзакцию
bool SessionContext::rollbackTransaction()
{
  database->rollbackTransaction();
  return true;
}

// Зафиксировать транзакцию
bool SessionContext::commitTransaction()
{
  database->commitTransaction();
  return true;
}
