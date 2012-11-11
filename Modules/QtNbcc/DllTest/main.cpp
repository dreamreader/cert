#include <QtCore/QCoreApplication>
#include <QLibrary>
#include <QFile>
#include <QTextCodec>

#include "../nbcc/nb/nb.h"
#include "../nbcc/nb/nbc.h"

#define asHex(c)   (QString::number(Nb::Result(c).code(), 16))

static const nbUuid nbUUID_NBCC =
{ 0x9ac0fe89, 0x596e, 0x4414, { 0x94, 0x36, 0x5, 0x12, 0x2b, 0xd9, 0x4d, 0x34 } };

QLibrary    *lib;
nbINbcc     *nbcc;
Nb::Matrix  *key;
Nb::Matrix  *own1;
Nb::Matrix  *own2;
Nb::Matrix  *test1;
Nb::Matrix  *test2;
Nb::Matrix  *all;
Nb::Matrix  *estimate;
Nb::Matrix  *out;
Nb::Scheme  s;
Nb::Data    nbcData(1000000);

void testNbcc();
bool loadNbcc();
bool unloadNbcc();
bool loadMatrix();
bool loadData();
bool freeData();
bool testSchemes();
bool setScheme();
bool createByIndex(Nb::Scheme &s, int index);
bool train();
bool exportNbc();
bool reset();
bool importNbc();
bool extract();

bool testQStringData()
{
  QString string = QString::fromUtf8("Спать хочется :(");
  Nb::Data data;
  Nb::Data data2;
  int size;

  Nb::Block b;
  b.type(131);
  qDebug() << data.fromString(string, size, true);
  qDebug() << size;

  b.push(data, true);
  qDebug() << b;
  int pos(0);

  Nb::Container::Factory factory;
  Nb::Container c(factory);

  c.push(b);
  Nb::Block *b2 = c.at(0);

  b2->pop(pos, data2, true);

  qDebug() << data2.toString();
}

int main(int argc, char *argv[])
{
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM866"));
  QCoreApplication a(argc, argv);
  testNbcc();
  //testQStringData();

  return a.exec();
}

void testNbcc()
{
  loadNbcc();
  loadData();
  //testSchemes();
  setScheme();
  train();
  exportNbc();
  reset();
  importNbc();
  extract();

  freeData();
  unloadNbcc();
}

bool loadNbcc()
{
  lib = new QLibrary("D:/work/_Bcc/TEST/QtNbcc-build-desktop/nbcc/debug/nbcc");

  qDebug() << "\nload dll "
           << lib->load();

  NbQueryComponentPtr entry = (NbQueryComponentPtr)lib->resolve("NbQueryComponent");
  qDebug() << "resolve entry "
           << (entry != NULL);

  qDebug() << "query component: result code "
           << asHex(entry(nbUUID_NBCC, nbUUID_INBCC, (void**)&nbcc));
}


bool unloadNbcc()
{
  qDebug() << "\nrelease component: result code "
           << asHex(nbcc->release());
}

bool loadData()
{
  qDebug() << "\nload data";

  key = new Nb::Matrix(1,Nb::Meta(256,Nb::MfI1,Nb::MtDO));
  Nb::Vbp& k = key->at(0,0);
  for (int32_t i=0; i<key->meta(0).count()/8; ++i)
    k.u8[i] = 0x55;

  own1 = new Nb::Matrix();
  own2 = new Nb::Matrix();
  test1 = new Nb::Matrix();
  test2 = new Nb::Matrix();
  all = new Nb::Matrix();

  QDataStream stream;
  /*QFile own1File("bip/finger1own.bip");
  QFile own2File("bip/finger1own.bip");
  QFile test1File("bip/finger1own.bip");
  QFile test2File("bip/own_1_20.bip");
  QFile allFile("bip/finger1all.bip");*/

  QFile own1File("bip/finger1own.bip");
  QFile test1File("bip/finger1OwnTest.bip");
  QFile allFile("bip/finger1all.bip");

  /*QFile own1File("bip/maxim_2_18.bip");
  QFile test1File("bip/maxim_2_19.bip");
  QFile allFile("bip/finger1all.bip");*/

  /*QFile own1File("bip/own_1_10.bip");
  QFile test1File("bip/own_1_20.bip");
  QFile allFile("bip/all_balanced_hw.bip");*/

  if (!own1File.open(QFile::ReadOnly))
    qDebug() << "own1 bims not found";
  /*if (!own2File.open(QFile::ReadOnly))
    qDebug() << "own2 bims not found";*/
  if (!test1File.open(QFile::ReadOnly))
    qDebug() << "test1 bims not found";
  /*if (!test2File.open(QFile::ReadOnly))
    qDebug() << "test2 bims not found";*/
  if (!allFile.open(QFile::ReadOnly))
    qDebug() << "all bims not found";

  stream.setDevice(&own1File);
  qDebug() << "read own1 bims: bytes "
           << own1->read(stream);
  /*stream.setDevice(&own2File);
  qDebug() << "read own2 bims: bytes "
           << own2->read(stream);*/
  stream.setDevice(&test1File);
  qDebug() << "read test1 bims: bytes "
           << test1->read(stream);
  /*stream.setDevice(&test2File);
  qDebug() << "read test2 bims: bytes "
           << test2->read(stream);*/
  stream.setDevice(&allFile);
  qDebug() << "read all bims: bytes "
           << all->read(stream);

}

bool freeData()
{
  delete key;
  delete own1;
  delete own2;
  delete test1;
  delete test2;
  delete all;
}

bool createByIndex(Nb::Scheme &s, int index){
  using namespace Nb;

  Meta metaHw(416,MfR32,MtC);
  Meta metaFp(256,MfR32,MtC);
  Meta metaKey(256,MfI1,MtDO);

  s.free();
  switch (index){
  case 0://Подпись
    s.push(StIn,        metaHw)
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)
     .out(1);
    break;
  case 1://Палец
    s.push(StIn,        metaFp)
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)
     .out(1);
    break;
  case 2://Подпись и подпись
    s.push(StIn,        metaHw) //0
     .push(StIn,        metaHw) //1
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
     .push(BtConnector, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain|BfGenerated) //4
     .out(4);
    break;
  case 3://Палец и палец
    s.push(StIn,        metaFp) //0
     .push(StIn,        metaFp) //1
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
     .push(BtConnector, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain|BfGenerated) //4
     .out(4);
    break;
  case 4://Подпись и палец
    s.push(StIn,        metaHw) //0
     .push(StIn,        metaFp) //1
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
     .push(BtConnector, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain|BfGenerated) //4
     .out(4);
    break;
  case 5://Подпись или палец
    s.push(StIn,        metaHw) //0
     .push(StIn,        metaFp) //1
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<0)  //2
     .push(BtNeuralConv,metaKey,  SlotTypes()<<StIn,SlotNums()<<1)  //3
     .push(BtCondition, metaKey,  SlotTypes()<<StIn<<StIn,SlotNums()<<2<<3,BfBackwardTrain) //4
     .out(4);
    break;
   default:
    return false;
   }
 return true;
}

bool testSchemes()
{
  Nb::Scheme s;

  for (int i=0; i<=5; i++)
  {
    createByIndex(s, i);

    qDebug() << "\ncase " << i << " scheme:\n"
             << s;

    Nb::Container::Factory factory;
    Nb::Container c(factory);

    Nb::Block block;
    block.type(Nb::BtCScheme);
    block.push(s);

    Nb::Block testBlock1;
    testBlock1.type(133);
    testBlock1.push(nbUUID_IBIM_PROVIDER);
    testBlock1.push(0x4523423);
    Nb::Block testBlock2;
    testBlock2.type(133);
    testBlock2.push(*all);

    c.push(Nb::BlockContHead(Nb::Uuid::nil,0));
    c.push(testBlock1);
    c.push(testBlock2);
    c.push(block);
    ((Nb::BlockContHead*)c.at(0))->update(c.all(),0,4);
    qDebug()<<c;

    QByteArray ba;
    QDataStream stream(&ba, QIODevice::ReadWrite);
    Nb::Data data;
    c.write(stream, true);
    stream.device()->seek(0);
    data.read(stream, true);

    qDebug() << "import Nbc: result code "
             << asHex(nbcc->importNbc(data));

    qDebug() << "reset constraints: result code "
             << asHex(nbcc->reset(Nb::SsAll));
  }
}

bool setScheme()
{
  //Nb::Meta metaHw(416,Nb::MfR32,Nb::MtC);
  //Nb::Meta metaFp(256,Nb::MfR32,Nb::MtC);
  Nb::Meta metaKey(256,Nb::MfI1,Nb::MtDO);
  s.free();
  s.push(Nb::StIn,        own1->meta(0))
   .push(Nb::BtNeuralConv,metaKey,  Nb::SlotTypes()<<Nb::StIn,Nb::SlotNums()<<0)
   .out(1);

  QByteArray ba;
  QDataStream stream(&ba, QIODevice::ReadWrite);
  Nb::Data data;
  s.writeScheme(stream, true);
  stream.device()->seek(0);
  data.read(stream, true);

  qDebug() << "\nscheme:\n"
           << s;

  qDebug() << "set scheme: result code "
           << asHex(nbcc->setConvScheme(data));

  QFile schemeFile("my_scheme.dat");
  schemeFile.open(QFile::WriteOnly);
  QDataStream stream2(&schemeFile);
  s.writeScheme(stream2, false);
}


bool train()
{
  qDebug() << "\nset input 1 'own': result code "
           << asHex(nbcc->setConstraint(s.idOf(0), Nb::CtOwn, &((nbMatrix&)*own1)));
  qDebug() << "set unput 1 'all': result code "
           << asHex(nbcc->setConstraint(s.idOf(0), Nb::CtAll, &((nbMatrix&)*all)));

  /*qDebug() << "set input 2 'own': result code "
           << asHex(nbcc->setConstraint(s.idOf(1), Nb::CtOwn, &((nbMatrix&)*own2)));
  qDebug() << "set input 2 'all': result code "
           << asHex(nbcc->setConstraint(s.idOf(1), Nb::CtAll, &((nbMatrix&)*all)));*/

  qDebug() << "set out 'target': result code "
           << asHex(nbcc->setConstraint(Nb::SlotId::out, Nb::CtOwn, &((nbMatrix&)*key)));

  qDebug() << "";
  qDebug() << "train: result code"
           << asHex(nbcc->train(nbNULL, nbNULL, nbNULL, Nb::ToInfinite));

  estimate = new Nb::Matrix(1,Nb::Meta(1,Nb::MfR32,Nb::MtCO));
  qDebug() << "\nget p1: result code "
           << asHex(nbcc->test(Nb::TmE1, &((nbMatrix&)*estimate), 0));
  qDebug() << "p1 "
           << estimate->at(0, 0).r32[0];
  qDebug() << "get p2: result code "
           << asHex(nbcc->test(Nb::TmE2, &((nbMatrix&)*estimate), 0));
  qDebug() << "p2 "
           << estimate->at(0, 0).r32[0];
}

bool exportNbc()
{
  qDebug() << "\nexport Nbc: result code "
           << asHex(nbcc->exportNbc(nbcData, nbUUID_NBCC));
}

bool reset()
{
  qDebug() << "\nreset: result code "
           << asHex(nbcc->reset(Nb::SsAll));
}

bool importNbc()
{
  qDebug() << "\nimport Nbc: result code "
           << asHex(nbcc->importNbc(nbcData));
}

bool extract()
{


  // FINGER

  qDebug() << "\n\ntest  1[ owns ]";
  qDebug() << "set input 1 'test own': result code "
           << asHex(nbcc->setConstraint(s.idOf(0), Nb::CtAny, &((nbMatrix&)*test1)));

  out = new Nb::Matrix(test1->ncols(),Nb::Meta(256,Nb::MfI1,Nb::MtDO));
  qDebug() << "extract: result code"
           << asHex(nbcc->extract(nbNULL, &((nbMatrix&)*out), nbNULL));
  QString str1;
  for (unsigned example = 0; example < out->ncols(); example++)
  {
    unsigned ham(0);
    for (unsigned bit = 0; bit < 256; bit++)
    {
      ham += (key->at(0, 0).i1(bit) != out->at(0, example).i1(bit));
    }
    str1 += QString::number(ham) + " ";
  }
  delete out;
  qDebug() << str1;


  qDebug() << "\nreset constraints: result code "
           << asHex(nbcc->reset(Nb::SsInAny | Nb::SsOutAny));

  qDebug() << "\ntest  1[ all ]";
  qDebug() << "set input 1 'all': result code "
           << asHex(nbcc->setConstraint(s.idOf(0), Nb::CtAny, &((nbMatrix&)*all)));

  out = new Nb::Matrix(all->ncols(),Nb::Meta(256,Nb::MfI1,Nb::MtDO));
  qDebug() << "extract: result code"
           << asHex(nbcc->extract(nbNULL, &((nbMatrix&)*out), nbNULL));
  str1.clear();
  for (unsigned example = 0; example < out->ncols(); example++)
  {
    unsigned ham(0);
    for (unsigned bit = 0; bit < 256; bit++)
    {
      ham += (key->at(0, 0).i1(bit) != out->at(0, example).i1(bit));
    }
    str1 += QString::number(ham) + " ";
  }
  delete out;
  qDebug() << str1;

  //FINGER


  return true;


  qDebug() << "\n\ntest  1[ owns ]  2[ empty ]";
  qDebug() << "set input 1 'test own': result code "
           << asHex(nbcc->setConstraint(s.idOf(0), Nb::CtAny, &((nbMatrix&)*test1)));

  out = new Nb::Matrix(test1->ncols(),Nb::Meta(256,Nb::MfI1,Nb::MtDO));
  qDebug() << "extract: result code"
           << asHex(nbcc->extract(nbNULL, &((nbMatrix&)*out), nbNULL));
  QString str;
  for (unsigned example = 0; example < out->ncols(); example++)
  {
    unsigned ham(0);
    for (unsigned bit = 0; bit < 256; bit++)
    {
      ham += (key->at(0, 0).i1(bit) != out->at(0, example).i1(bit));
    }
    str += QString::number(ham) + " ";
  }
  delete out;
  qDebug() << str;

  qDebug() << "\nreset constraints: result code "
           << asHex(nbcc->reset(Nb::SsInAny | Nb::SsOutAny));


  qDebug() << "\ntest  1[ empty ]  2[ owns ]";
  qDebug() << "set input 2 'test own': result code "
           << asHex(nbcc->setConstraint(s.idOf(1), Nb::CtAny, &((nbMatrix&)*test2)));

  out = new Nb::Matrix(test1->ncols(),Nb::Meta(256,Nb::MfI1,Nb::MtDO));
  qDebug() << "extract: result code"
           << asHex(nbcc->extract(nbNULL, &((nbMatrix&)*out), nbNULL));
  str.clear();
  for (unsigned example = 0; example < out->ncols(); example++)
  {
    unsigned ham(0);
    for (unsigned bit = 0; bit < 256; bit++)
    {
      ham += (key->at(0, 0).i1(bit) != out->at(0, example).i1(bit));
    }
    str += QString::number(ham) + " ";
  }
  delete out;
  qDebug() << str;


  qDebug() << "\nreset constraints: result code "
           << asHex(nbcc->reset(Nb::SsInAny | Nb::SsOutAny));


  qDebug() << "\ntest  1[ all ]  2[ empty ]";
  qDebug() << "set input 1 'all': result code "
           << asHex(nbcc->setConstraint(s.idOf(0), Nb::CtAny, &((nbMatrix&)*test2)));

  out = new Nb::Matrix(test1->ncols(),Nb::Meta(256,Nb::MfI1,Nb::MtDO));
  qDebug() << "extract: result code"
           << asHex(nbcc->extract(nbNULL, &((nbMatrix&)*out), nbNULL));
  str.clear();
  for (unsigned example = 0; example < out->ncols(); example++)
  {
    unsigned ham(0);
    for (unsigned bit = 0; bit < 256; bit++)
    {
      ham += (key->at(0, 0).i1(bit) != out->at(0, example).i1(bit));
    }
    str += QString::number(ham) + " ";
  }
  delete out;
  qDebug() << str;


  qDebug() << "\nreset constraints: result code "
           << asHex(nbcc->reset(Nb::SsInAny | Nb::SsOutAny));


  qDebug() << "\ntest  1[ empty ]  2[ all ]";
  qDebug() << "set input 2 'all': result code "
           << asHex(nbcc->setConstraint(s.idOf(1), Nb::CtAny, &((nbMatrix&)*test1)));

  out = new Nb::Matrix(test1->ncols(),Nb::Meta(256,Nb::MfI1,Nb::MtDO));
  qDebug() << "extract: result code"
           << asHex(nbcc->extract(nbNULL, &((nbMatrix&)*out), nbNULL));
  str.clear();
  for (unsigned example = 0; example < out->ncols(); example++)
  {
    unsigned ham(0);
    for (unsigned bit = 0; bit < 256; bit++)
    {
      ham += (key->at(0, 0).i1(bit) != out->at(0, example).i1(bit));
    }
    str += QString::number(ham) + " ";
  }
  delete out;
  qDebug() << str;


  qDebug() << "\nreset constraints: result code "
           << asHex(nbcc->reset(Nb::SsInAny | Nb::SsOutAny));


  qDebug() << "\ntest  1[ own ]  2[ own ]";
  qDebug() << "set input 1 'test own': result code "
           << asHex(nbcc->setConstraint(s.idOf(0), Nb::CtAny, &((nbMatrix&)*test1)));
  qDebug() << "set input 2 'test own': result code "
           << asHex(nbcc->setConstraint(s.idOf(1), Nb::CtAny, &((nbMatrix&)*test2)));

  out = new Nb::Matrix(test1->ncols(),Nb::Meta(256,Nb::MfI1,Nb::MtDO));
  qDebug() << "extract: result code"
           << asHex(nbcc->extract(nbNULL, &((nbMatrix&)*out), nbNULL));
  str.clear();
  for (unsigned example = 0; example < out->ncols(); example++)
  {
    unsigned ham(0);
    for (unsigned bit = 0; bit < 256; bit++)
    {
      ham += (key->at(0, 0).i1(bit) != out->at(0, example).i1(bit));
    }
    str += QString::number(ham) + " ";
  }
  delete out;
  qDebug() << str;


  qDebug() << "\nreset constraints: result code "
           << asHex(nbcc->reset(Nb::SsInAny | Nb::SsOutAny));


  qDebug() << "\ntest  1[ all ]  2[ all ]";
  qDebug() << "set input 1 'all': result code "
           << asHex(nbcc->setConstraint(s.idOf(0), Nb::CtAny, &((nbMatrix&)*test2)));
  qDebug() << "set input 2 'all': result code "
           << asHex(nbcc->setConstraint(s.idOf(1), Nb::CtAny, &((nbMatrix&)*test1)));

  out = new Nb::Matrix(test1->ncols(),Nb::Meta(256,Nb::MfI1,Nb::MtDO));
  qDebug() << "extract: result code"
           << asHex(nbcc->extract(nbNULL, &((nbMatrix&)*out), nbNULL));
  str.clear();
  for (unsigned example = 0; example < out->ncols(); example++)
  {
    unsigned ham(0);
    for (unsigned bit = 0; bit < 256; bit++)
    {
      ham += (key->at(0, 0).i1(bit) != out->at(0, example).i1(bit));
    }
    str += QString::number(ham) + " ";
  }
  delete out;
  qDebug() << str;
}
