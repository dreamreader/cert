//Nbcc
//Класс-обертка модуля нейросетевого преобразователя

#include "Nbcc.h"

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
          qDebug() << "setConstraint failed, errc=" << Result (errc);
          throw QString::fromUtf8("Ошибка установки биометрических параметров");
        }
    }

    switch (errc = _iface->extract(NULL, &(nbMatrix &)code, SlotId::out))
    {
    case nbS_OK:
        break;
    default:
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
        qDebug() << "importNbc failed, errc=" << Result (errc);
        throw QString::fromUtf8("Ошибка установки контейнера");
    }

    SlotIds in_slots = scheme.ids(nbSLOT_TYPE_IN);
    for (int i = 0; i != in_slots.size(); i++) {
        switch (errc = _iface->setConstraint(in_slots[i], nbCT_DATA_ALL, &(nbMatrix &)(*foreigns[i])))
        {
        case nbS_OK:
          break;
        default:
          qDebug() << "setConstraint failed, errc=" << Result (errc);
          throw QString::fromUtf8("Ошибка установки параметров \"Чужой\" при обучении");
        }
        switch (errc = _iface->setConstraint(in_slots[i], nbCT_DATA_OWN, &(nbMatrix &)(*owns[i])))
        {
        case nbS_OK:
            break;
        default:
          qDebug() << "setConstraint failed, errc=" << Result (errc);
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
        qDebug() << "train failed, errc=" << Result (errc);
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
        qDebug() << "test failed, errc=" << Result (errc);
        throw QString::fromUtf8("Ошибка автоматического тестирования");
    }
    tr.e1Bio = stat[0][0].r32[0];

    switch (errc = _iface->test(nbTEST_MODE_E2_BIO, &(nbMatrix&)stat, nbTIMEOUT_INFINITE))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "test failed, errc=" << Result (errc);
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
        qDebug() << "exportNbc failed, errc=" << Result (errc);
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
        qDebug() << "exportNbc failed, errc=" << Result (errc);
        throw QString::fromUtf8("Ошибка выгрузки контейнера (2)");
    }

    container.readFrom (temp);

    //Защита контейнера
    SlotIds slot_ids = scheme.ids(StConvertor, BtNeuralConv);
    for (int i = 0; i != slot_ids.size (); i++) {
      switch (errc = _iface->postTrain(slot_ids[i], TmDefault, BsTrained, scheme.at(slot_ids[i])->bflags() | BfProtected, nbTIMEOUT_INFINITE))
      {
      case nbS_OK:
          break;
      default:
          qDebug() << "postTrain failed, errc=" << Result (errc);
          throw QString::fromUtf8("Ошибка постобучения (2)");
      }
    }

    switch (errc = _iface->test(nbTEST_MODE_E2_BIO, &(nbMatrix&)stat, nbTIMEOUT_INFINITE))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "test failed, errc=" << Result (errc);
        throw QString::fromUtf8("Ошибка автоматического тестирования");
    }
    tr.e2Code = stat[0][0].r32[0];

    stats = tr;

    return true;
}

bool Nbcc::load (Uuid id, QString path, ModuleInfo info)
{
  int errc;
    QLibrary lib (path);
    typedef nbResult nbCALL (*QUERY_FUNC)(nbTHISc nbRefUuid iid, void **icd);
    NbQueryComponentPtr query = NULL;
    if (!lib.load ()) {
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
        qDebug() << "query failed, errc=" << Result (errc);
        throw QString::fromUtf8 ("Невозможно загрузить интерфейс библиотеки биометрического преобразователя");
    }
    _created = true;
    _id = id;
    _info = info;

    return true;
}
