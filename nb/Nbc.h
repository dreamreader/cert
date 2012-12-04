//Нейросетевой биометрический контейнер
#ifndef NBC_H
#define NBC_H

#include <QFile>

#include "../qtnb/Container.h"

//Пользовательские типы
#define nbBT_SCHEME_NUMBER (129)
#define nbBT_USER_NAME     (130)
#define nbBT_NBC_NAME      (131)
#define nbBT_PATTERN       (132)
#define nbBT_CODE          (133)
#define nbBT_SLOT_ID       (134)
#define nbBT_CONTAINER_ID  (135)

using namespace Nb;

class Nbc : public Container {
public:
  Nbc();
  ~Nbc();

  Uuid getId();

  //Чтение контейнера из файла с сигнатурой
  bool readFrom(QString path);
  //Запись контейнера в файл с сигнатурой
  bool writeTo(QString path);
  bool readFrom(Data data);
  bool writeTo(Data &data);

  bool setScheme (Scheme scheme);
  Scheme getScheme ();

  QString getUsername();
  bool setUsername(QString);

  QString getName();
  bool setName(QString);

  bool getCode(Matrix &code);
  bool setCode(Matrix &code);

  bool getBimPattern(SlotId slot_id, Uuid bim_type, Matrix &code);
  bool setBimPattern(SlotId slot_id, Uuid bim_type, Matrix code);

  Uuids getSlotInfo (SlotId slot);
  bool setSlotInfo (SlotId slot, Uuids uuids);

  //temporary
  bool setSchemeNumber (size_t number);
  size_t getSchemeNumber ();

private:
  bool findBlocks (Blocks &found, int type);
  bool update ();

  Uuid _id;
  Container::Factory factory;
};

#endif
