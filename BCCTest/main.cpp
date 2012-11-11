#include <QtCore/QCoreApplication>
#include <QFile>
#include <QLibrary>
#include <QDataStream>
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

int main(int argc, char *argv[])
{
  LOG

  QCoreApplication a(argc, argv);
  QDataStream stream;
  srand(time(NULL));
  
  Log::open("d:/services/log_client.txt");
  Log::write("log opened!");
  Client client;
  client.start();

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
  bool access;
  nbResult rslt;
  QStringList containers;

  /*qDebug() << containers;

  rslt = client.startRegistration("Kulikov S", "Tablet " + QString::number(rand()), regkey);
  if (nbFAILED(rslt))
    Log::write("startRegistration failed with error", rslt, true);

  rslt = client.registerContainer(container);
  if (nbFAILED(rslt))
    Log::write("registerContainer failed with error", rslt, true);

  QList<Nb::Matrix*> list1;
  list1.push_back(&own);
  QList<Nb::Matrix*> list2;
  list2.push_back(&all);
  rslt = client.registerData(list1, list2, stats);
  if (nbFAILED(rslt))
    Log::write("registerData failed with error", rslt, true);

  list1.clear();
  list1.push_back(&test);
  Log::write("ncols", test.ncols());
  rslt = client.testData(list1);
  if (nbFAILED(rslt))
    Log::write("testData 1 failed with error", rslt, true);
  rslt = client.testData(list1);
  if (nbFAILED(rslt))
    Log::write("testData 2 failed with error", rslt, true);
  rslt = client.testData(list1);
  if (nbFAILED(rslt))
    Log::write("testData 3 failed with error", rslt, true);


  rslt = client.confirmRegistration();
  if (nbFAILED(rslt))
    Log::write("confirmRegistration failed with error", rslt, true);

  //rslt = client.createSignature("Test", "Test", images, result);*/


  /*rslt = client.enumerateContainers("Kulikov S", containers);
  if (nbFAILED(rslt))
    Log::write("enumerateContainers failed with error", rslt, true);*/


  rslt = client.startSign("Kulikov S", "Tablet 2155", container);
  if (nbFAILED(rslt))
    Log::write("startSign failed with error", rslt, true);

  OneTimePassword wrongPwd(256, false, 0x54);
  rslt = client.usePassword(wrongPwd, access);
  if (nbFAILED(rslt))
    Log::write("usePassword failed with error", rslt, true);
  qDebug() << "access granted ? " << access;

  OneTimePassword truePwd(256, false, 0x55);
  rslt = client.usePassword(truePwd, access);
  if (nbFAILED(rslt))
    Log::write("usePassword failed with error", rslt, true);
  qDebug() << "access granted ? " << access;

  client.stop();
  Log::close();
  return a.exec();
}
