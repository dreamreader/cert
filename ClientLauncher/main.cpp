#include <Qt/qapplication.h>
#include <QLibrary>
#include <QDebug>
#include <QDir>

#include "../nb/nb.h"

#include "../nb/Data.h"
#include "../nb/Uuid.h"

#define ERROR(s) qDebug () << s;



typedef int (*SETUP_FUNC)(const char *server_address, const uint16_t server_port);
typedef int (*SIGN_FUNC)(const char *username, const char *data, uint32_t data_size, uint32_t signature_size, char *signature);
typedef int (*REGISTER_FUNC)(const char *key, int key_size, const char *username);

using namespace Nb;

void sign (SIGN_FUNC func);
void registr (REGISTER_FUNC func);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SIGN_FUNC sign_dll = NULL;
    REGISTER_FUNC registr_dll = NULL;
    SETUP_FUNC setup_dll = NULL;



    if (argc < 2) ERROR("No parameters set");

    QLibrary lib ("../bioclient/untitled");
    if (!lib.load ()) ERROR(lib.errorString());

    setup_dll = (SIGN_FUNC)lib.resolve ("setup");
    if (!sign_dll) ERROR("Cannot resolve symbol \"setup\"")

    Data key(32), username;
    int size = 0;
    qDebug () << "calling \"setup\"";
    int errc = setup_dll ("192.166.0.1", 3030);
    qDebug () << "errc = " << (Result)errc;




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
  qDebug () << "calling \"sign\"";
  int errc = func (username, data, 1024, 32, signature);
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
  qDebug () << "calling \"registr\"";
  int errc = func (key, 32, username);
  qDebug () << "errc = " << (Result)errc;
  qDebug () << "Data from server: " << key;

}
