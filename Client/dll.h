#ifndef DLL_H
#define DLL_H

#include <QLibrary>
#include <QStringList>
#include <Qt/qapplication.h>
#include <QtCore/qglobal.h>

#include "../nb/nb.h"
#include "../nb/Data.h"
#include "../nb/Uuid.h"

#include "interface/gui.h"
#include "interface/ExtractWizard.h"
#include "interface/TrainWizard.h"

#include "client.h"

#define UNTITLEDSHARED_EXPORT Q_DECL_EXPORT

using namespace Nb;

// Коды результатов:

#define E_OK       0            //< успешное завершение
#define E_PARAM   -1            //< неверный параметр
#define E_CANCEL  -3            //< процедура отменена пользователем
#define E_SKIP    -4            //< процедура пропущена (пользователь не зарегистрирован)
#define E_FAIL    -5            //< ошибка



extern "C"
{

  //Установка параметров сервера
  Q_DECL_EXPORT int setup               //< возвращается результат (см. выше)
  (
    const char *server_address,         //< IP-адрес сервера (например, "192.168.0.1")
    const uint16_t server_port          //< порт сервера
  );

  //Подпись
  Q_DECL_EXPORT int sign                //< возвращается результат (см. выше)
  (
    const char *username,               //< имя пользователя (null-terminated UTF-8)
    const char *data,                   //< данные для подписи
    uint32_t data_size,                 //< размер данных для подписи
    uint32_t signature_size,            //< длина подписи
    char *signature                     //< подпись
  );

  //Подпись
  Q_DECL_EXPORT int registrate          //< возвращается результат (см. выше)
  (
    const char *key,                    //< ключ регистрации
    int key_size,                       //< длина ключа регистрации
    const char *username                //< имя пользователя (null-terminated UTF-8)
  );

}


#endif // DLL_H
