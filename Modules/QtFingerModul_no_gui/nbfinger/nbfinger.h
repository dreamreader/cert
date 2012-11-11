#ifndef NBFINGER_H
#define NBFINGER_H

#include "nbfinger_global.h"
#include "nb/nb.h"

extern "C" {
  NBFINGERSHARED_EXPORT nbResult nbCALL NbCanUnload();
  NBFINGERSHARED_EXPORT nbResult nbCALL NbQueryComponent(nbRefUuid cid, nbRefUuid iid, void **icd);
  NBFINGERSHARED_EXPORT nbResult nbCALL NbEnumComponents(nbIEventHandler *eh);
}

#endif // NBFINGER_H
