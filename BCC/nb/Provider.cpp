#include "Provider.h"


nbResult nbCALL Provider::queryInterface (nbTHISc nbRefUuid iid, void **icd)
{
  (*icd) = (nbIEventHandler *)this;
	return nbS_OK;
}
nbResult nbCALL Provider::retain(nbTHISc)
{
	return 1;
}
nbResult nbCALL Provider::release(nbTHISc)
{
	return 0;
}

nbResult nbCALL Provider::handleEvent(nbTHISc nbEvent evt, nbHandle sender, const nbData paramA, nbData paramB)
{
	return nbS_OK;
}

Provider::Provider ():
  _iface(NULL),_created(false),_attached(false),_purprose(nbPURPOSE_UNKNOWN),
  _id(nbUUID_NIL),_bim(NULL), _bims(NULL)
{
}

Provider::~Provider()
{
	reset ();
}

bool Provider::isCreated ()
{
	return _created;
}

bool Provider::create (nbIBimProvider *iface, Uuid id)
{
	_iface = iface;
    _id = id;
	_created = true;
    return true;
}

bool Provider::attachDevice (int32_t deviceId)
{
    int errc;
    if (!_created) {
        return false;
    }
    switch (errc = _iface->connectToDevice((nbHandle)deviceId))
    {
    case nbS_OK:
        break;
    case nbE_NO_MODULE:
        qDebug() << "connectToDevice failed, errc=" << Result (errc);
        throw QString::fromUtf8 ("Невозможно загрузить драйвер");
        break;
    case nbE_NO_DEVICE:
        qDebug() << "connectToDevice failed, errc=" << Result (errc);
        throw QString::fromUtf8 ("Не установлен драйвер или устройство не подключено");
        break;
    default:
        qDebug() << "connectToDevice failed, errc=" << Result (errc);
        throw QString::fromUtf8 ("Невозможно присоединить устройство ввода");
    }

    return true;
}


bool Provider::reset ()
{
	if (_iface) {
        if (_bim){
            _iface->resetBim();
            _bim = NULL;
        }
        _iface->release();
        _iface = NULL;
	}
	_created = false;
    _bims = NULL;
	return true;
}

void Provider::setTestPurprose (){
  _purprose = nbPURPOSE_AUDIT;
}
void Provider::setAuthenticatePurprose(){
  _purprose = nbPURPOSE_AUTHENTICATE;
}
void Provider::setTrainPurprose(){
  _purprose = nbPURPOSE_IDENTIFY;
}

bool Provider::canAttachWindow (){
    return true;
}

bool Provider::attachWindow (QWidget* window){
    int errc;
    if (!_created || !window) {
        return false;
    }
	_window = window;
  //Присоединить графическое окно
    switch (errc = _iface->connectToWindow(window, NULL))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "connectToWindow failed, errc=" << Result (errc);
        throw QString::fromUtf8 ("Невозможно присоединить окно");
    }

	_attached = true;
    //Обновить окно
    updateWindow();
	return true;
}

bool Provider::detachWindow()
{
    int errc;
    if (!_created || !_attached) {
        return false;
    }
    //Выполнить первичный захват биометрического образа
    switch (errc = _iface->connectToWindow(NULL, NULL))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "connectToWindow failed, errc=" << Result (errc);
        throw QString::fromUtf8 ("Невозможно отсоединить окно");
    }
  _attached = false;
  return true;
}

bool Provider::isAttached ()
{
	return _attached;
}

bool Provider::updateWindow()
{
    return false;
}

bool Provider::clearWindow(){
    int errc;
    if (!_created) {
        return false;
    }

    switch (errc = _iface->captureBim(&_bim,_info.bim_type,_purprose))
    {
    case nbS_OK:
        break;
    default:;
        qDebug() << "captureBim failed, errc=" << Result (errc);
    }
    return true;
}

//Захват устройства для чтения образа
bool Provider::startCapturing(BimBase *bims)
{
    int errc;
    if (!_created || (bims == NULL)) {
        return false;
    }
    _bims = bims;

    //Выполнить первичный захват биометрического образа

    switch (errc = _iface->captureBim(&_bim,_info.bim_type,_purprose))
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "captureBim failed, errc=" << Result (errc);
        throw QString::fromUtf8 ("Невозможно выполнить захват биометрического образа");
    }

    return true;
}

bool Provider::capture ()
{
  int errc;
  if (!_created) {
      return false;
  }
  //Считать биометрический образ (если он введен или не введен)
  switch (errc = _iface->waitForBimReady(&_bim, PROVIDER_TIMEOUT)) {
    case nbS_OK:
      break;
    case nbECODE_TIMEOUT_EXPIRED:
      return false;
    default:
      qDebug() << "waitForBimReady failed, errc=" << Result (errc);
      throw (QString::fromUtf8 ("Невозможно считать биометрический образ"));
  }

  //Добавить образ в список
  _bims->insert(_bims->size(),_bim,0.0f);

  //Освободить временный буфер
  _iface->releaseBim(_bim);
  _bim = NULL;
  clearWindow();
  return true;
}

bool Provider::show (size_t index)
{
    int errc;
    if (!_created) {
        return false;
    }
    switch (errc = _iface->setBim(_bims->at(index), _purprose)) {
      case nbS_OK:
        return true;
      default:
      qDebug() << "setBim failed, errc=" << Result (errc);
        throw (QString::fromUtf8 ("Невозможно просмотреть образ"));
    }
  return true;
}

//Сохранение конфигурации
bool Provider::saveConfig (const QString &userSid, const QString &name, const Data &options){
  return false;
}

//Загрузка конфигурации
bool Provider::loadConfig (const QString &userSid, const QString &name, Data &options){
  return false;
}

//Чтение конфигурации компонента
bool Provider::getConfig(Data &options){
    return false;
}

//Установка конфигурации компонента
bool Provider::setConfig(const Data options){
    return false;
}

Uuid Provider::getId(){
	return _id;
}

bool Provider::load (Uuid id, QString path, ModuleInfo info)
{
  int errc;
    QLibrary lib (path);
    reset ();
    NbQueryComponentPtr query = NULL;
    if (!lib.load ()) {
      throw (QString::fromUtf8 ("Невозможно загрузить библиотеку биометрического провайдера"));
    }
    query = (NbQueryComponentPtr)lib.resolve("NbQueryComponent");
    if (!query) {
      throw (QString::fromUtf8 ("Невозможно загрузить интерфейс библиотеки биометрического провайдера"));
    }
    switch (errc = query(id, nbUUID_IBIM_PROVIDER, (void **)&_iface) )
    {
    case nbS_OK:
        break;
    default:
      qDebug() << "query failed, errc=" << Result (errc);
        throw (QString::fromUtf8 ("Невозможно загрузить интерфейс библиотеки биометрического провайдера"));
    }

    _created = true;
    _id = id;
    _info = info;

    return true;
}

Uuid Provider::getBimType ()
{
    return _info.bim_type;
}
