#include "BimBase.h"


BimBase::BimBase(){
  _provider = NULL;
  _changed = false;
}

BimBase::~BimBase(){
  clear();
}

nbBim* BimBase::newBim(uint32_t size){
  if (!size) {
    return NULL;
  }
  return (nbBim*) new uint8_t[size];
}

bool BimBase::freeBim(nbBim* bim, nbIBimProvider *provider){
  if (NULL == bim) {
    return true;
  }
  if (provider){
    return nbSUCCEEDED(provider->releaseBim(bim));
  } else {
    delete[] (uint8_t*)bim;
    return true;
  }
}

//Удалить все образы
bool BimBase::clear(){
  bool ok = true;
  //Удалить все образы
  for (uint32_t i=0; i<_bims.size(); i++) {
    ok &= freeBim(_bims[i]);
  }
  _bims.clear();
  _quals.clear();
  _provider = NULL;
  _changed = true;
  return ok;
}

//Доступ к элементам как к массиву
nbBim** BimBase::data(){
  if (size()) {
    //NOTE неочевидный и некошерный кодъ
    return &_bims[0];
  }
  return NULL;
}

vector<float>& BimBase::quals(){
  return _quals;
}

//Получить размер
uint32_t BimBase::size() const {
  return (uint32_t) _bims.size();
}

//Получить тип образов
Uuid BimBase::getBimType(uint32_t index){
  if (index<size()) {
    return _bims[index]->bimType;
  }
  return Uuid(nbUUID_NIL);
}

bool BimBase::set(uint32_t index, float qual){
  if (index>=size()) {
    return false;
  }
  _quals[index] = qual;
  return true;
}

bool BimBase::set(uint32_t index, nbBim *bim){
  if (!bim || bim->size || _provider) {
    return false;
  }
  if (index >= size()) {
    return false;
  }
  freeBim(_bims[index]);
  _bims[index] = newBim(sizeof(nbBim) + bim->size);
  memcpy(_bims[index],bim,sizeof(nbBim) + bim->size);
  _changed = true;
  return true;
}

//Добавить в заданную позицию (0 - в начало, >=size - в конец)
bool BimBase::insert(uint32_t index, nbBim *bim, float qual){
  if (!bim || !bim->size || _provider) {
    return false;
  }

  nbBim *bim2 = newBim(sizeof(nbBim) + bim->size);
  if (!bim2) {
    return false;
  }
  memcpy(bim2,bim,sizeof(nbBim) + bim->size);

  nbBims::iterator itrBim;
  if (index >= size()) {
    itrBim = _bims.end();
  } else {
    itrBim = _bims.begin() + index;
  }

  vector<float>::iterator itrQ;
  if (index >= size()) {
    itrQ = _quals.end();
  } else {
    itrQ = _quals.begin() + index;
  }

  _bims.insert(itrBim,bim2);
  _quals.insert(itrQ,qual);
  _changed = true;
  return true;
}

//Удалить в заданной позиции (0 - из головы, size-1 - последний)
bool BimBase::erase(uint32_t index){
  if (index > size()) {
    return false;
  }
  nbBims::iterator itrBim = _bims.begin() + index;
  vector<float>::iterator itrQ = _quals.begin() + index;
  freeBim(_bims[index]);
  _bims.erase(itrBim);
  _quals.erase(itrQ);
  _changed = true;
  return true;
}

//Сохранение образов в файл
bool BimBase::save(QString filename, Uuid bim_type)
{
  size_t written;
  nbBimFileHeader header;
  header.signature  = SIG_BIMS;
  header.sess_id    = nbUUID_NIL;
  header.purpose    = nbPURPOSE_UNKNOWN;
  header.multipart  = 0;
  header.parttype   = 0;
  header.providerCid= bim_type;
  header.count      = size();

  QFile file (filename);
  //Открыть файл
  if (!file.open(QFile::ReadWrite)) {
    throw QString::fromUtf8("Невозможно создать файл");
  }

  //Записать заголовок
  written = file.write((char *)&header, sizeof (header));
  if (written != sizeof(header)) {
    throw QString::fromUtf8("Невозможно записать файл");
  };

  //Записать образы
  for (uint i = 0; i < size(); ++i){
    nbBim *current = at(i);
    if (!current){
      throw QString::fromUtf8("Ошибка в базе биометрических образов");
    };
    written = file.write((char *)current, current->size + sizeof(nbBim));
    if (written != (sizeof(nbBim) + current->size)) {
      throw QString::fromUtf8("Невозможно записать файл");
    }
  }
  return true;
}

//Загрузка образов из файла
bool BimBase::load(QString filename, Uuid bim_type)
{
  nbBimFileHeader header;
  nbBim bh;
  uint read;
  QFile file (filename);
  //Открыть файл
  if (!file.open(QFile::ReadOnly)) {
    throw QString::fromUtf8("Невозможно открыть файл");
    return false;
  }

  //Прочитать заголовок
  read = file.read((char *)&header, sizeof (header));
  if (read != sizeof(header)) {
    throw QString::fromUtf8("Невозможно прочитать файл");
  };
  if (header.signature != SIG_BIMS) {
    throw QString::fromUtf8("Неверный формат файла");
  }
  if (bim_type != (Uuid)header.providerCid) {
    throw QString::fromUtf8("Неверный тип биометрических образов");
  }

  clear();

  //Считать образы
  for (uint i = 0; i < header.count; i++) {
    read = file.read((char *)&bh, sizeof(nbBim));
    if (read != sizeof(nbBim)) {
      clear ();
      throw QString::fromUtf8("Невозможно прочитать файл");
    };
    nbBim *bim = newBim(sizeof(nbBim) + bh.size);
    *bim = bh;
    read = file.read((char *)bim + sizeof(nbBim), bh.size);
    if (read != bh.size) {
      clear ();
      throw QString::fromUtf8("Невозможно прочитать файл");
    };
    _bims.push_back(bim);
    _quals.push_back(0.0f);
  }

  return true;
}

//Признак изменения базы образов
bool BimBase::changed(){
  return _changed;
}

//Сбросить признак изменения базы
void BimBase::unchange(){
  _changed = false;
}

