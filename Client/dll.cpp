#include "dll.h"






//Установка параметров сервера
Q_DECL_EXPORT int setup
(
  const char *server_address,
  const uint16_t server_port
)
{
  Client::setup (QString::fromUtf8 (server_address), server_port);
  return E_OK;
}


//Подпись
Q_DECL_EXPORT int sign                //< возвращается результат (см. выше)
(
  const char *username,               //< (вх) имя пользователя (null-terminated UTF-8)
  const char *data,                   //< (вх) данные для подписи
  uint32_t data_size,                 //< (вх) размер данных для подписи
  uint32_t signature_size,            //< (вх) длина подписи
  char *signature                     //< (вых) подпись
)
{
  int errc;
  qDebug () << "Now at 'sign' function";

  //--------------------------
  //---Проверяем параметры
  //---
  if ( !signature || !data || !username ) {
    qDebug () << "returning nbE_PARAM\r\n" << "  some parameters are NULL";
    return E_PARAM;
  }

  if ( (signature_size == 0) || (data_size == 0) || (strlen (username) == 0) ) {
    qDebug () << "returning nbE_PARAM\r\n" << "  some parameters are empty";
    return E_PARAM;
  }

  try {
    QString qusername = QString::fromUtf8(username);
    qDebug () << "username = " << qusername;

    //--------------------------
    //---Соединяемся с сервером
    //---
    switch (errc = Client::start())
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "failed to start, errc= " << Result (errc);
        throw QString::fromUtf8 ("Невозможно соединиться с сервером");
    }

    //--------------------------
    //---Получаем список контейнеров
    //---
    QStringList nbcs;
    switch (errc = Client::enumerateContainers(qusername, nbcs))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "failed to enumerateContainers, errc= " << Result (errc);
        throw QString::fromUtf8 ("Невозможно перечислить контейнеры пользователя");
    }
    if (nbcs.size () < 1) {
      qDebug () << "returning nbS_CANCEL\r\n" << "  username is not register on server";
      Client::stop ();
      return E_SKIP;
    }

    //--------------------------
    //---Получение контейнера
    //---
    Nbc nbc;
    switch (errc = Client::getContainer(username, nbc))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "failed to getContainer, errc= " << Result (errc);
        throw QString::fromUtf8 ("Невозможно получить контейнер пользователя");
    }

    //--------------------------
    //---Биометрическая аутентификация
    //-----преобразование биометрии в код
    //-----authenticateBio ()
    ExtractWizard dlg (nbc);
    if (dlg.exec() == QDialog::Rejected) {
      qDebug () << "returning nbS_CANCEL\r\n" << "  user cancels the dialog";
      Client::stop ();
      return E_CANCEL;
    }

    //--------------------------
    //---Подпись документа
    //---
    Data data, sig;
    data.copy (data, data_size);
    switch (errc = Client::signData(username, data, sig))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "failed to signDocument, errc= " << Result (errc);
        throw QString::fromUtf8 ("Невозможно подписать документ");
    }
    memcpy (signature, (nbData)sig, signature_size);

  } catch (QString e) {
    Client::stop ();
    Gui::error(e);
    return E_FAIL;
  }

  Client::stop ();
  return E_OK;
}

//------------------------------------------------------------

Q_DECL_EXPORT int registrate          //< возвращается результат (см. выше)
(
  const char *key,                    //< (вх) ключ регистрации
  int key_size,                       //< (вх) длина ключа регистрации
  const char *username                //< (вх) имя пользователя (null-terminated UTF-8)
)
{
  int errc;
  qDebug () << "Now at 'registrate' function";


  //--------------------------
  //---Проверка параметров
  //---
  if (!key || !username) {
    qDebug () << "returning nbE_PARAM\r\n" << "  some parameters are NULL";
    return nbE_PARAM;
  }

  if ( (key_size == 0) || (strlen (username) == 0) ) {
    qDebug () << "returning nbE_PARAM\r\n" << "  some parameters are empty";
    return nbE_PARAM;
  }

  try {
    QString qusername = QString::fromUtf8(username);


    //--------------------------
    //---Соединяемся с сервером
    //---
    switch (errc = Client::start())
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "failed to start, errc=" << Result (errc);
        throw QString::fromUtf8 ("Невозможно соединиться с сервером");
    }


    //--------------------------
    //---Аутентификация
    //---
    Data key;
    key.copy (key, key_size);
    bool granted = false;
    switch (errc = Client::authenticate(qusername, (Data &)key, granted))
    {
    case nbS_OK:
      break;
    default:
      qDebug() << "failed to authenticate, errc=" << Result (errc);
      throw QString::fromUtf8 ("Невозможно провести криптографическую аутентифкаицию");
    }

    if (!granted) {
        qDebug () << "returning nbS_SKIP\r\n" << "  crypto auth unsuccesful";
        return E_SKIP;

    }

    //--------------------------
    //---Получаем список контейнеров
    //---
    QStringList nbcs;
    switch (errc = Client::enumerateContainers(qusername, nbcs))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "failed to enumerateContainers, errc=" << Result (errc);
        throw QString::fromUtf8 ("Невозможно перечислить контейнеры пользователя");
    }

    if (nbcs.size () >= 1) {

      //--------------------------
      //---Биометрическая аутентификация
      //---

      Nbc nbc;
      switch (errc = Client::getContainer(username, nbc))
      {
      case nbS_OK:
          break;
      default:
          qDebug() << "failed to getContainer, errc=" << Result (errc);
          throw QString::fromUtf8 ("Невозможно провести авторизацию пользователя на сервере");
      }

      //--------------------------
      //---Биометрическая аутентификация
      //-----преобразование биометрии в код
      //-----authenticateBio ()
      ExtractWizard dlg1 (nbc);
      if (dlg1.exec() == QDialog::Rejected) {
        qDebug () << "returning nbS_CANCEL\r\n" << "  user cancels the dialog";
        Client::stop ();
        return E_CANCEL;
      }
    }

    //--------------------------
    //---Ввод биометрии
    //-----ввод биометрии
    //-----registerContainer ()
    //-----confirmContainer ()
    TrainWizard dlg2 (username);
    if (dlg2.exec() == QDialog::Rejected) {
      qDebug () << "returning nbS_CANCEL\r\n" << "user cancels the dialog";
      return E_CANCEL;
    }

    Client::stop ();

  } catch (QString e) {
    Client::stop ();
    Gui::error(e);
    return E_FAIL;
  }

  return E_OK;
}
