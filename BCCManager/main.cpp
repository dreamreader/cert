#include <QtCore/QCoreApplication>
#include <Windows.h>
#include <QDir>
#include "../BCC/serverdatabase.h"

#define servicePath       ((WCHAR*)L"c:/program files/BioCertificationCenter/bin/BioCertificationCenter.exe")
#define serviceName       ((WCHAR*)L"BioCertificationCenter")

void echo(char* msg, DWORD err = ERROR_SUCCESS)
{
  printf(msg);
  if (err != ERROR_SUCCESS)
  {
      printf(" (error = %d)", err);
  }
  printf("\n");

}

int installDatabase()
{
  if (!ServerDatabase::create())
  {
    echo("Error: Can't create container database");
    return -1;
  }
  echo("Container database created");
}

int installService()
{
  SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  if(!hSCManager)
  {
    echo("Error: Can't open Service Control Manager");
    return -1;
  }

  SC_HANDLE hService = CreateService(
     hSCManager,
     serviceName,
     serviceName,
     SERVICE_ALL_ACCESS,
     SERVICE_WIN32_OWN_PROCESS,
     SERVICE_DEMAND_START,
     SERVICE_ERROR_NORMAL,
     servicePath,
     NULL, NULL, NULL, NULL, NULL
  );

  if(!hService)
  {
    int err = GetLastError();
    switch(err)
    {
      case ERROR_ACCESS_DENIED:
        echo("Error: ERROR_ACCESS_DENIED");
        break;
      case ERROR_CIRCULAR_DEPENDENCY:
        echo("Error: ERROR_CIRCULAR_DEPENDENCY");
        break;
      case ERROR_DUPLICATE_SERVICE_NAME:
        echo("Error: ERROR_DUPLICATE_SERVICE_NAME");
        break;
      case ERROR_INVALID_HANDLE:
        echo("Error: ERROR_INVALID_HANDLE");
        break;
      case ERROR_INVALID_NAME:
        echo("Error: ERROR_INVALID_NAME");
        break;
      case ERROR_INVALID_PARAMETER:
        echo("Error: ERROR_INVALID_PARAMETER");
        break;
      case ERROR_INVALID_SERVICE_ACCOUNT:
        echo("Error: ERROR_INVALID_SERVICE_ACCOUNT");
        break;
      case ERROR_SERVICE_EXISTS:
        echo("Error: ERROR_SERVICE_EXISTS");
        break;
      default:
        echo("Error: Undefined");
    }
    CloseServiceHandle(hSCManager);
    return -1;
  }
  CloseServiceHandle(hService);
  CloseServiceHandle(hSCManager);
  echo("Service INSTALLED");

  return 0;
}

int removeService()
{
  SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if(!hSCManager)
  {
     echo("Error: Can't open Service Control Manager");
     return -1;
  }
  SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_STOP | DELETE);
  if(!hService)
  {
     echo("Error: Can't remove service");
     CloseServiceHandle(hSCManager);
     return -1;
  }
  DeleteService(hService);
  CloseServiceHandle(hService);
  CloseServiceHandle(hSCManager);
  echo("Service removed!");
  return 0;
}

int startService()
{
  SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_START);
  if(!StartService(hService, 0, NULL))
  {
    CloseServiceHandle(hSCManager);
    echo("Error: Can't start service", GetLastError());
    return -1;
  }
  CloseServiceHandle(hService);
  CloseServiceHandle(hSCManager);

  return 0;
}


int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);

  if(argc != 2)
  {
    echo("no command! available commands:\ninstall_database\ninstall_service\nremove_service\nstart_service");
    return -1;
  }
  else if( strcmp(argv[argc-1], ("install_database")) == 0)
  {
    echo("Installing database...");
    return installDatabase();
  }
  else if( strcmp(argv[argc-1], ("install_service")) == 0)
  {
    echo("Installing service...");
    return installService();
  }
  else if( strcmp(argv[argc-1], ("remove_service")) == 0)
  {
    echo("Removing service ...");
    return removeService();
  }
  else if( strcmp(argv[argc-1], ("start_service")) == 0 )
  {
    echo("Starting service...");
    return startService();
  }
  else
  {
    echo("wrong syntax, available commands:\nremove\ninstall\nstart\n");
  }

  return app.exec();
}
