#include <qresource.h>

#include "nb/nbc.h"
#include "bc/BcNbcc.h"
#include "nbcc.h"

using namespace Nb;
using namespace Bc;

//Проверка возможности выгрузки библиотеки
NBCCSHARED_EXPORT nbResult nbCALL NbCanUnload() {
  return (nbResult)BcNbcc::getCoRefs();
}

//Создание компонента
NBCCSHARED_EXPORT nbResult nbCALL NbQueryComponent(nbRefUuid cid, nbRefUuid iid, void **icd){
  nbResult rslt;
  if (Uuid(cid) == Uuid(nbUUID_NBCC)){
    if (Uuid(iid) == Uuid(nbUUID_IUNKNOWN) || Uuid(iid) == Uuid(nbUUID_INBCC)){
      BcNbcc *nbcc = new BcNbcc();
      rslt = nbcc->queryInterface(iid,icd);
      if (nbFAILED(rslt)) delete nbcc;
    } else {
      return nbE_NO_INTERFACE;
    }
  } else {
    return nbE_NO_COMPONENT;
  }
  return rslt;
}

//Перечисление компонентов
NBCCSHARED_EXPORT nbResult nbCALL NbEnumComponents(nbIEventHandler *eh){
  if (!eh) return nbE_PARAM;
  QResource resNbcc(":/info/nbcc");

  uint8_t* xmlNbcc  = (uint8_t*)resNbcc.data();

  Data data; data.copy(xmlNbcc,resNbcc.size());

  eh->handleEvent(nbEVENT_ENUMERATE_ITEM,(nbHandle)NULL,(nbData)&nbUUID_NBCC,(nbData)data);
  return nbS_OK;
}

