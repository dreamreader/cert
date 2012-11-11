
#include <qresource.h>

#include "nbfinger.h"
#include "nbclasses/nbcTypes.h"
#include "FingerProcessor.h"

//Проверка возможности выгрузки библиотеки
NBFINGERSHARED_EXPORT nbResult nbCALL NbCanUnload() {
  return (nbResult)(FingerProcessor::getCoRefs());
}

//Создание компонента
NBFINGERSHARED_EXPORT nbResult nbCALL NbQueryComponent(nbRefUuid cid, nbRefUuid iid, void **icd){
  nbResult rslt;
  if (cid == nbUUID_FINGER_PROCESSOR){
    if (iid == nbUUID_IUNKNOWN || iid == nbUUID_IBIM_PROCESSOR || iid == nbUUID_ICONFIGURATE){
      FingerProcessor *processor = new FingerProcessor();
      rslt = processor->queryInterface(iid,icd);
      if (nbFAILED(rslt)) delete processor;
    } else {
      return nbE_NO_INTERFACE;
    }
  } else {
    return nbE_NO_COMPONENT;
  }
  return rslt;
}

//Перечисление компонентов
NBFINGERSHARED_EXPORT nbResult nbCALL NbEnumComponents(nbIEventHandler *eh){
  if (!eh) return nbE_PARAM;

  //
  QResource resProc(":/info/proc");

  //F_BASE_ALL_SIZE примеров
  char* xmlProc  = (char*)resProc.data();

  NbData proc;
  proc.copyFrom(xmlProc,resProc.size());

  eh->handleEvent(nbEVENT_ENUMERATE_ITEM,(nbHandle)NULL,(nbData)&nbUUID_FINGER_PROCESSOR,(nbData)proc);

  return nbS_OK;
}

