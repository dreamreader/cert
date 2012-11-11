//Provider
//Класс-обертка модуля биометрического провайдера

#ifndef PROVIDER_H_INCLUDED
#define PROVIDER_H_INCLUDED

#include <QLibrary>

#include "nb.h"

#include "Uuid.h"
#include "Data.h"

#include "ModuleInfo.h"
#include "BimBase.h"

#define PROVIDER_TIMEOUT 10000000

using namespace Nb;

class Provider : public nbIEventHandler {
private:
  nbMETHOD (queryInterface)(nbTHISc nbRefUuid iid, void **icd);
  nbMETHOD (retain)(nbTHISc);
  nbMETHOD (release)(nbTHISc);
  nbMETHOD (handleEvent)(nbTHISc nbEvent evt, nbHandle sender, const nbData paramA, nbData paramB);

public:
  Provider();
  virtual ~Provider();

  bool isCreated();
  bool create(nbIBimProvider *iface, Uuid id);
  bool load (Uuid id, QString path, ModuleInfo info);
  bool reset();
  Uuid getId();

  void setTestPurprose();
  void setAuthenticatePurprose();
  void setTrainPurprose();

  bool canAttachWindow();
  bool attachWindow(QWidget* window);
  bool attachDevice (int32_t deviceId = 0);
  bool detachWindow();
  bool isAttached();

  bool updateWindow();
  bool clearWindow();

  //Чтение биометрического образа
  bool startCapturing(BimBase *bims);
  bool capture();

  //Отображение биометрического образа
  bool show(size_t index);

  //Загрузка и сохранение конфигурации в реестр
  bool loadConfig(const QString &userSid, const QString &name, Data &options);
  bool saveConfig(const QString &userSid, const QString &name, const Data &options);

  //Чтение и установка конфигурации в компонент
  bool getConfig(Data &options);
  bool setConfig(const Data options);

  Uuid getBimType ();

private:
  nbIBimProvider *_iface;
  QWidget *_window;
  bool _created, _attached;
  nbPurpose _purprose;
  Uuid _id;
  nbBim *_bim;
  BimBase *_bims;
  ModuleInfo _info;
};

#endif
