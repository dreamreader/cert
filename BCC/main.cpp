#include <QtCore/QCoreApplication>
#include <Windows.h>
#include "server.h"
#include "protocol/log.h"

#define serviceName         ((WCHAR*)L"Биометрический удостоверяющий центр")
#define stop                "stop"

Server*               gServer;          //сервер биометрического удостоверяющего центра
SERVICE_STATUS        gStatus;          //статус службы
SERVICE_STATUS_HANDLE gStatusHandle;    //

// Обработка запросов сервис-менеджера
void serviceControl(DWORD request)
{
  LOG

  Log::write("service control request", (int)request);
  bool res = false;
  switch(request)
  {
    case SERVICE_CONTROL_STOP:
      Log::write("invoke stop");
      res = QMetaObject::invokeMethod(gServer, stop, Qt::BlockingQueuedConnection);
      Log::write("stop invoked", res);

      gStatus.dwWin32ExitCode = 0;
      gStatus.dwCurrentState = SERVICE_STOPPED;
      SetServiceStatus (gStatusHandle, &gStatus);
      QCoreApplication::quit();
      return;

    case SERVICE_CONTROL_SHUTDOWN:
      Log::write("invoke stop");
      res = QMetaObject::invokeMethod(gServer, stop, Qt::BlockingQueuedConnection);
      Log::write("stop invoked", res);

      gStatus.dwWin32ExitCode = 0;
      gStatus.dwCurrentState = SERVICE_STOPPED;
      SetServiceStatus (gStatusHandle, &gStatus);
      QCoreApplication::quit();
      return;

    default:
      break;
  }

  SetServiceStatus (gStatusHandle, &gStatus);
  return;
}

void debugMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
      txt = QString("Debug: %1").arg(msg);
      break;
    case QtWarningMsg:
      txt = QString("Warning: %1").arg(msg);
      break;
    case QtCriticalMsg:
      txt = QString("Critical: %1").arg(msg);
      break;
    case QtFatalMsg:
      txt = QString("Fatal: %1").arg(msg);
      abort();
    }
    Log::write(txt);
}

// Точка входа для сервис-менеджера
DWORD serviceProcess(int argc, char* argv[])
{
  LOG

  QCoreApplication app(argc, argv);

  gServer = new Server();

  gStatus.dwServiceType       = SERVICE_WIN32_OWN_PROCESS;
  gStatus.dwCurrentState      = SERVICE_START_PENDING;
  gStatus.dwControlsAccepted  = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
  gStatus.dwWin32ExitCode     = 0;
  gStatus.dwServiceSpecificExitCode = 0;
  gStatus.dwCheckPoint        = 0;
  gStatus.dwWaitHint          = 0;

  LPWSTR name = serviceName;
  gStatusHandle = RegisterServiceCtrlHandler(name, (LPHANDLER_FUNCTION)serviceControl);
  if (gStatusHandle == (SERVICE_STATUS_HANDLE)0)
  {
    return 1;
  }

  gServer->moveToThread(app.thread());
  bool res = gServer->start();
  if (!res)
  {
    gStatus.dwCurrentState    = SERVICE_STOPPED;
    gStatus.dwWin32ExitCode   = -1;
    SetServiceStatus(gStatusHandle, &gStatus);
    return 2;
  }

  gStatus.dwCurrentState = SERVICE_RUNNING;
  SetServiceStatus (gStatusHandle, &gStatus);

  Log::write("main thread id", (int)QThread::currentThread());

  app.exec();
  delete gServer;
  return 0;
}


int main(int argc, char* argv[])
{
  Log::open("d:/services/log_server.txt");
  Log::write("enter main");

  SERVICE_TABLE_ENTRY ServiceTable[2];
  LPWSTR name = serviceName;
  ServiceTable[0].lpServiceName = name;
  ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)serviceProcess;
  ServiceTable[1].lpServiceName = NULL;
  ServiceTable[1].lpServiceProc = NULL;

  StartServiceCtrlDispatcher(ServiceTable);

  Log::write("service return");
}

