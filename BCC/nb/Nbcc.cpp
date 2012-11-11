#include "Nbcc.h"
#include "../protocol/log.h"

//Nbcc
//Класс-обертка модуля нейросетевого преобразователя

#ifndef NBCC_H_INCLUDED
#define NBCC_H_INCLUDED

#include "nb.h"
#include "Nbc.h"
#include "Matrix.h"
#include "Statistics.h"

using namespace Nb;

Nbcc::Nbcc ():
    _iface(NULL),_created(false),_trained(false),
    _defended(false),_tested(false),_id(nbUUID_NIL)
{

}

Nbcc::~Nbcc()
{
    reset ();
}

bool Nbcc::create(nbINbcc *iface, Uuid id)
{
    _created = true;
    _iface = iface;
    _id = id;
    return true;
}

bool Nbcc::isCreated()
{
    return _created;
}

bool Nbcc::reset()
{
    _defended = false;
    _tested = false;
    _trained = false;
    if (_iface) {
        _iface->release ();
    _iface = NULL;
    }
    _created = false;
    return true;
}

Uuid Nbcc::getId()
{
    return _id;
}

bool Nbcc::isTrained()
{
    return _trained;
}

bool Nbcc::isDefended()
{
    return _defended;
}

bool Nbcc::isTested()
{
    return _tested;
}

bool Nbcc::extract(Nbc &container, Matrix &code, QList<Matrix *> &params)
{
    Data container_raw;
    int errc;
    container.writeTo(container_raw);

    _iface->reset(nbNS_ALL);

    switch (errc = _iface->importNbc(container_raw))
    {
    case nbS_OK:
        break;
    default:
        Log::write("importNbc failed", errc, true);
        qDebug() << "importNbc failed, errc=" << Result (errc);
        throw QString::fromUtf8("Ошибка установки контейнера");
    }

    Scheme scheme;
    scheme.copy (container.getScheme());

    SlotIds in_slots = scheme.ids(nbSLOT_TYPE_IN);
    for (int i = 0; i != in_slots.size(); i++) {
        switch (errc = _iface->setConstraint(in_slots[i], nbCT_DATA, &(nbMatrix &)(*params[i])))
        {
        case nbS_OK:
          break;
        default:
          Log::write("setConstraint failed", errc, true);
          qDebug() << "setConstraint failed, errc=" << Result (errc);
          throw QString::fromUtf8("Ошибка установки биометрических параметров");
        }
    }
    qDebug () << "before extract";
    switch (errc = _iface->extract(NULL, &(nbMatrix &)code, SlotId::out))
    {
    case nbS_OK:
        break;
    default:
        Log::write("extract failed", errc, true);
        qDebug() << "extract failed, errc=" << Result (errc);
        throw QString::fromUtf8("Ошибка извлечения кода");
    }

    return true;
}

bool Nbcc::learn(Nbc &container, QList<Matrix *> &owns, QList<Matrix *> &foreigns, Matrix &code, Statistics &stats)
{
    int errc;
    Data container_raw;
    container.writeTo(container_raw);

    Scheme scheme;
    scheme.copy (container.getScheme ());

    _iface->reset (nbNS_ALL);
    switch (errc = _iface->importNbc(container_raw))
    {
    case nbS_OK:
        break;
    default:
        Log::write("import failed", errc, true);
        throw QString::fromUtf8("Ошибка установки контейнера");
    }

    SlotIds in_slots = scheme.ids(nbSLOT_TYPE_IN);
    for (int i = 0; i != in_slots.size(); i++) {
        switch (errc = _iface->setConstraint(in_slots[i], nbCT_DATA_ALL, &(nbMatrix &)(*foreigns[i])))
        {
        case nbS_OK:
          break;
        default:
          Log::write("setConstraint ALL failed", errc, true);
          throw QString::fromUtf8("Ошибка установки параметров \"Чужой\" при обучении");
        }
        switch (errc = _iface->setConstraint(in_slots[i], nbCT_DATA_OWN, &(nbMatrix &)(*owns[i])))
        {
        case nbS_OK:
            break;
        default:
          Log::write("setConstraint OWN failed", errc, true);
          throw QString::fromUtf8("Ошибка установки параметров \"Свой\" при обучении");
        }
    }

    switch (errc = _iface->train(NULL, NULL, &(nbMatrix &)code, nbTIMEOUT_INFINITE))
    {
    case nbS_OK:
        break;
    case nbECODE_NO_DATA_OWN:
    case nbECODE_POOR_DIVERGENCE:
    case nbECODE_POOR_QUALITY:
    case nbECODE_POOR_STABILITY:
    case nbECODE_POOR_ORIGINALITY:
    case nbECODE_TOO_MANY_GROSS_ERRORS:
        throw QString::fromUtf8("Плохие данные для обучения");
    default:
        Log::write("train failed", errc, true);
        throw QString::fromUtf8("Ошибка обучения");
    }

    _trained = true;
    _defended = false;

    Matrix stat(1,Meta(1,MfR32,MtContinuous));
    nbTestResult tr;
    memset(&tr,0,sizeof(nbTestResult));

    stat[0][0].r32[0] = 0.0f;

    switch (errc = _iface->test(nbTEST_MODE_E1_BIO, &(nbMatrix&)stat, nbTIMEOUT_INFINITE))
    {
    case nbS_OK:
        break;
    default:
        Log::write("test failed", errc, true);
        throw QString::fromUtf8("Ошибка автоматического тестирования");
    }
    tr.e1Bio = stat[0][0].r32[0];

    switch (errc = _iface->test(nbTEST_MODE_E2_BIO, &(nbMatrix&)stat, nbTIMEOUT_INFINITE))
    {
    case nbS_OK:
        break;
    default:
        Log::write("test failed", errc, true);
        throw QString::fromUtf8("Ошибка автоматического тестирования");
    }
    tr.e2Bio = stat[0][0].r32[0];

    Data temp(1);
    switch (errc = _iface->exportNbc(temp, nbUUID_NIL))
    {
    case nbS_OK:
        break;
    case nbE_INSUFFICIENT_BUFFER:
        break;
    default:
        Log::write("exportNbc failed", errc, true);
        throw QString::fromUtf8("Ошибка выгрузки контейнера (1)");
    }
    uint32_t size = temp.size();
    temp.setWishfulSize(1);
    temp.resize(size);
    switch (errc = _iface->exportNbc(temp, nbUUID_NIL))
    {
    case nbS_OK:
        break;
    default:
        Log::write("exportNbc failed", errc, true);
        throw QString::fromUtf8("Ошибка выгрузки контейнера (2)");
    }

    container.readFrom (temp);

    //Защита контейнера
    SlotIds slot_ids = scheme.ids(StConvertor, BtNeuralConv);
    qDebug () <<slot_ids;
    for (int i = 0; i != slot_ids.size (); i++) {
      switch (errc = _iface->postTrain(slot_ids[i], TmDefault, BsTrained, scheme.at(slot_ids[i])->bflags() | BfProtected, nbTIMEOUT_INFINITE))
      {
      case nbS_OK:
          break;
      default:
          Log::write("postTrain failed", errc, true);
          throw QString::fromUtf8("Ошибка постобучения (2)");
      }
    }

    switch (errc = _iface->test(nbTEST_MODE_E2_BIO, &(nbMatrix&)stat, nbTIMEOUT_INFINITE))
    {
    case nbS_OK:
        break;
    default:
        Log::write("test failed", errc, true);
        throw QString::fromUtf8("Ошибка автоматического тестирования");
    }
    tr.e2Code = stat[0][0].r32[0];
    qDebug () << "e2 posttrain" << tr.e2Code;

    stats.setData(tr);

    Log::write("dtors now...");
    return true;
}

bool Nbcc::load (Uuid id, QString path)
{
    int errc;
    QLibrary lib (path);
    NbQueryComponentPtr query = NULL;

    if (!lib.load ()) {
      lib.unload();
      Log::write(lib.errorString());
      throw QString::fromUtf8 ("Невозможно загрузить библиотеку биометрического преобразователя");
    }

    query = (NbQueryComponentPtr)lib.resolve("NbQueryComponent");
    if (!query) {
      throw QString::fromUtf8 ("Невозможно загрузить интерфейс биометрического преобразователя");
    }
    switch (errc = query(id, nbUUID_INBCC, (void **)&_iface) )
    {
    case nbS_OK:
        break;
    default:
        Log::write("query failed, errc=" + QString::number(Result(errc).code(), 16));
        throw QString::fromUtf8 ("Невозможно загрузить интерфейс библиотеки биометрического преобразователя");
    }
    _created = true;
    return true;
}

#endif
