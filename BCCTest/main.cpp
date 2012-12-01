#include <QtCore/QCoreApplication>
#include <QFile>
#include <QLibrary>
#include <QDataStream>
#include <QTextCodec>
#include <QStringList>
#include <time.h>
#include "client.h"
#include "schemeconstructor.h"
#include "../bcc/protocol/log.h"
#include "../bcc/protocol/query.h"
#include "../bcc/onetimepassword.h"
#include "../bcc/nb/nbtypes.h"
#include "../bcc/nb/nbclasses.h"
#include "../bcc/nb/bimbase.h"
#include "../bcc/nb/processor.h"

static const nbUuid nbccUUID_NBCC =
  {0x35b8c4f9,0xda6a,0x4568,{0x8d,0x8d,0x40,0x55,0x89,0x7e,0x1c,0x1f}};

static const nbUuid nbUUID_MODULESIGN =
  {0xa9ebf452,0xcf25,0x408c,{0x86,0x47,0x5e,0x3a,0x1d,0x57,0xff,0x4e}};

#define tcpServerAddress      ("localhost")
#define tcpServerPort         (31111)

int main(int argc, char *argv[])
{
  LOG

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM866"));
  QCoreApplication a(argc, argv);
  QDataStream stream;
  srand(time(NULL));
  
  Log::open("d:/services/log_client.txt");
  Log::write("log opened!");
  Client client;
  client.start(tcpServerAddress, tcpServerPort);

  // Запихиваем схему в контейнер
  Nbc container  = SchemeConstructor::construct(0);
  // Вводим ключ регистрации
  QString regkey = "test_regkey_223456";

  // Вводим биометрические образы
  Nb::Matrix own;
  Nb::Matrix all;
  QFile ownFile("d:/services/bip/own_0_10.bip");
  QFile allFile("d:/services/bip/all_balanced_hw.bip");

  ownFile.open(QFile::ReadOnly);
  stream.setDevice(&ownFile);
  own.read(stream);
  allFile.open(QFile::ReadOnly);
  stream.setDevice(&allFile);
  all.read(stream);

  Nb::Matrix test(1,own.row(0).meta());
  for (int i=0; i<test.meta()[0].count(); i++)
    test.at(0, 0).r32[i] = own.at(0, 0).r32[i];

  Statistics stats;
  nbResult rslt;
  QStringList containers;
  bool accessGranted;
  Nb::Data key;
  int32_t s;
  key.fromString("test key", s, true);
  QList<Nb::Matrix*> list1;
  list1.push_back(&own);
  OneTimePassword wrongM(256, false, 0x54);
  OneTimePassword trueM(256, false, 0x55);
  Nb::Data wrongPwd(256);
  Nb::Data truePwd(256);
  wrongPwd.copy( ((Nb::Matrix)wrongM).at(0, 0).u8, 256 );
  truePwd.copy( ((Nb::Matrix)trueM).at(0, 0).u8, 256 );
  Nb::Data doc(10000);
  Nb::Data sig;

  Log::write("\nEnum containers\n");
  rslt = client.enumerateContainers("Kulikov S", containers);

  Log::write("\nAuthenticate\n");
  rslt = client.authenticate("Kulikov S", key, accessGranted);

  Log::write("\nRegister\n");
  rslt = client.registerContainer("Kulikov S", container, list1, stats);

  list1.clear();
  list1.push_back(&test);
  Log::write("\nTest 1\n");
  rslt = client.testContainer(list1);
  Log::write("\nTest 2\n");
  rslt = client.testContainer(list1);
  Log::write("\nTest 3\n");
  rslt = client.testContainer(list1);

  Log::write("\nConfirm\n");
  rslt = client.confirmContainer();


  Log::write("\nGet Container\n");
  rslt = client.getContainer("Kulikov S", container);
  qDebug() << container.size();

  Log::write("\nTry authenticate\n");
  rslt = client.authenticateBio("Kulikov S", wrongPwd, accessGranted);

  Log::write("\nTry authenticate\n");
  rslt = client.authenticateBio("Kulikov S", truePwd, accessGranted);

  Log::write("\nCreate signature\n");
  rslt = client.signData("Kulikov S", doc, sig);
  Log::write("sig: " + sig.toString());

  client.stop();
  Log::close();
  return a.exec();
}
