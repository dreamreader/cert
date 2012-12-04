#include "dll.h"

//------------------------------------------------------------

Q_DECL_EXPORT nbResult sign(const nbData user_id, const nbData data, nbData signature)
{
  int errc;
  qDebug () << "Now at 'sign' function";

  //--------------------------
  //---Проверяем параметры
  //---
  if (!user_id || !data || !signature) {
    qDebug () << "returning nbE_PARAM\r\n" << "  some parameters are NULL";
    return nbE_PARAM;
  }
  if (((Data)data).size() == 0) {
    qDebug () << "returning nbE_PARAM\r\n" << "  data.size == 0";
    return nbE_PARAM;
  }
  if (((Data)signature).size() < SIGNATURE_SIZE / 8) {
    ((Data)signature).setWishfulSize(SIGNATURE_SIZE / 8);
    qDebug () << "returning nbE_INSUFFICIENT_BUFFER\r\n" << "  data.size < 32\r\n" << "  setting signature.WishfulSize = 32";
    return nbE_INSUFFICIENT_BUFFER;
  }

  try {
    QString user_name;
    user_name = user_id.toString ();


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
    switch (errc = Client::enumerateContainers( ((Data)username).toString(), nbcs))
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
      return nbS_SKIP;
    }

    //--------------------------
    //---Получение контейнера
    //---
    Nbc nbc;
    switch (errc = Client::getContainer(user_name, nbc))
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
      return nbS_CANCEL;
    }

    //--------------------------
    //---Подпись документа
    //---
    switch (errc = Client::signData(user_name, (Data &) data, (Data &) signature))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "failed to signDocument, errc= " << Result (errc);
        throw QString::fromUtf8 ("Невозможно подписать документ");
    }

  } catch (QString e) {
    Client::stop ();
    Gui::error(e);
    return nbE_FAIL;
  }

  Client::stop ();

  return nbS_OK;
}

//------------------------------------------------------------

Q_DECL_EXPORT nbResult registrate(const nbData user_id, const nbData key)
{
  int errc;

  qDebug () << "Now at 'registrate' function";


  //--------------------------
  //---Проверка параметров
  //---
  if (!user_id) {
    qDebug () << "returning nbE_PARAM\r\n" << "  some parameters are NULL";
    return nbE_PARAM;
  }

  try {
    QString user_name;
    user_name = user_id.toString ();


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

    bool granted = false;
    switch (errc = authenticate(user_name, (Data &)key, &granted))
    {
    case nbS_OK:
      break;
    default:
      qDebug() << "failed to authenticate, errc=" << Result (errc);
      throw QString::fromUtf8 ("Невозможно провести криптографическую аутентифкаицию");
    }

    if (!granted) {
        qDebug () << "returning nbS_SKIP\r\n" << "  crypto auth unsuccesful";
        return nbS_SKIP;

    }

    //--------------------------
    //---Получаем список контейнеров
    //---
    QStringList nbcs;
    switch (errc = Client::enumerateContainers( ((Data)username).toString(), nbcs))
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
      switch (errc = Client::getContainer(((Data)username).toString(), nbc))
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
        return nbS_CANCEL;
      }
    }

    //--------------------------
    //---Ввод биометрии
    //-----ввод биометрии
    //-----registerContainer ()
    //-----registerData ()
    //-----confirmRegistration ()
    TrainWizard dlg2 (((Data)username).toString());
    if (dlg2.exec() == QDialog::Rejected) {
      qDebug () << "returning nbS_CANCEL\r\n" << "user cancels the dialog";
      return nbS_CANCEL;
    }

    Client::stop ();

  } catch (QString e) {
    Client::stop ();
    Gui::error(e);
    return nbE_FAIL;
  }

  return nbS_OK;
}
