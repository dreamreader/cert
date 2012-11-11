#include "Processor.h"

Processor::Processor ():
  _iface(NULL),_created(false),_id(nbUUID_NIL)
{
}

Processor::~Processor(){
	reset ();
}

bool Processor::create (nbIBimProcessor *iface, Uuid id){
	_created = true;
	_iface = iface;
    _id = id;
	return true;
}

bool Processor::reset (){
	if (_iface) {
        _iface->release ();
    _iface = NULL;
	}
	_created = false;
	return true;
}

bool Processor::isCreated(){
	return _created;
}

bool Processor::process(Matrix &params, BimBase &images)
{
  int errc;
    //indicator = 0 ... 256
    //или 0 если не задан шаблон
    if (!images.size()) return false;
    //Предварительно обработать биометрические образы
    nbMatrix *matrix = NULL, *indicator = NULL;

    switch (errc = _iface->process(&matrix,(const nbBim**)images.data(), images.size(), &indicator)) {
    case nbS_OK:
        break;
    default:
        qDebug() << "process failed, errc=" << Result (errc);
        throw (QString::fromUtf8 ("Невозможно выполнить преобразование биометрических образов в параметры"));
    }

    params.remeta(Matrix::fromM(*matrix));
    params.resize(matrix->ncols);
    params.copy(Matrix::fromM(*matrix));

    _iface->releaseBase(matrix);
    _iface->releaseBase(indicator);

    return true;
}

bool Processor::getForeignBase (Matrix &base)
{
	nbMatrix *params = NULL;
  int errc;

    switch (errc = _iface->queryBaseAny(&params, -1, _info.bim_type, 0)) {
	case nbS_OK:
		break;
	default:
        qDebug() << "queryBaseAny failed, errc=" << Result (errc);
		return false;
	}
    base.remeta(Matrix::fromM(*params));
    base.resize(params->ncols);
    base.copy(Matrix::fromM(*params));

    _iface->releaseBase(params);

	return true;
}

Uuid Processor::getId(){
	return _id;
}

bool Processor::load (Uuid id, QString path, ModuleInfo info)
{
  int errc;
    QLibrary lib (path);
    NbQueryComponentPtr query = NULL;
    if (!lib.load ()) {
      throw (QString::fromUtf8 ("Невозможно загрузить библиотеку биометрического процессора"));
    }
    query = (NbQueryComponentPtr)lib.resolve("NbQueryComponent");
    if (!query) {
      throw (QString::fromUtf8 ("Невозможно загрузить интерфейс библиотеки биометрического процессора"));
    }
    switch (errc = query(id, nbUUID_IBIM_PROCESSOR, (void **)&_iface) )
    {
    case nbS_OK:
        break;
    default:
        qDebug() << "query failed, errc=" << Result (errc);
        throw (QString::fromUtf8 ("Невозможно загрузить интерфейс библиотеки биометрического процессора"));
    }
    _created = true;
    _id = id;
    _info = info;

    return true;
}

bool Processor::getPattern (Matrix &pattern, BimBase &images)
{
  int errc;
  nbMatrix *raw_pattern = NULL, *raw_own = NULL;
  switch (errc = _iface->createTemplate(&raw_own, &raw_pattern, (const nbBim**)images.data(), images.size()))
  {
  case nbS_OK:
    break;
  default:
    qDebug() << "createTemplate failed, errc=" << Result (errc);
    throw (QString::fromUtf8 ("Невозможно получить шаблон биометрического образа"));
  }

  _iface->releaseBase(raw_own);

  if (!raw_pattern) {
    return false;
  }

  pattern.remeta(Matrix::fromM(*raw_pattern));
  pattern.resize(raw_pattern->ncols);
  pattern.copy(Matrix::fromM(*raw_pattern));
  _iface->releaseBase(raw_pattern);

  return true;
}

bool Processor::setPattern (Matrix &params)
{
  int errc;
    switch (errc = _iface->setTemplate(&(nbMatrix &)params, _info.bim_type))
    {
    case nbS_OK:
    case nbS_SKIP:
        break;
    default:
        qDebug() << "setTemplate failed, errc=" << Result (errc);
        throw (QString::fromUtf8 ("Невозможно загрузить шаблон биометрического образа"));
    }

    return true;
}
