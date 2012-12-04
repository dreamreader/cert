#include <Qt/qapplication.h>
#include <QLibrary>
#include <QDebug>
#include <QDir>

#include "nb/nb.h"

#include "qtnb/Data.h"
#include "qtnb/Uuid.h"

#define ERROR(s) qDebug () << s;

typedef nbResult (*SIGN_FUNC)(const nbData username, const nbData data, nbData signature);
typedef nbResult (*REGISTER_FUNC)(const nbData username, const nbData key);

using namespace Nb;

void sign (SIGN_FUNC func);
void registr (REGISTER_FUNC func);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SIGN_FUNC sign_dll = NULL;
    REGISTER_FUNC registr_dll = NULL;

    if (argc < 2) ERROR("No parameters set");

    QLibrary lib ("../bioclient/untitled");
    if (!lib.load ()) ERROR(lib.errorString());

    if ((QString)argv[1] == (QString) "sign") {

      sign_dll = (SIGN_FUNC)lib.resolve ("sign");
      if (!sign_dll) ERROR("Cannot resolve symbol \"sign\"")
      sign (sign_dll);

    } else if ((QString)argv[1] == (QString) "registrate") {

      registr_dll = (REGISTER_FUNC)lib.resolve ("registrate");
      if (!registr_dll) ERROR("Cannot resolve symbol \"registrate\"")
      registr (registr_dll);

    } else {

      ERROR ("Wrong parameter")

    }

    return 0;
}

void sign (SIGN_FUNC func)
{

  //Подпись

  Data data(1024), signature(32), username;
  int size = 0;
  username.fromString((QString)"test", size, true);

  qDebug () << "Data to server: " << username << data;
  int errc = func (username, data, signature);
  qDebug () << "errc = " << (Result)errc;
  qDebug () << "Data from server: " << signature;


}

void registr (REGISTER_FUNC func)
{

  //Регистрация

  Data key(32), username;
  int size = 0;
  username.fromString((QString)"test", size, true);

  qDebug () << "Data to server: " << username;
  int errc = func (username, key);
  qDebug () << "errc = " << (Result)errc;
  qDebug () << "Data from server: " << key;

}
