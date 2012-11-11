#ifndef NBSIGN_H
#define NBSIGN_H

#include "nbsign_global.h"
#include "nb\nb.h"

extern "C" {
  NBSIGNSHARED_EXPORT nbResult nbCALL NbCanUnload();
  NBSIGNSHARED_EXPORT nbResult nbCALL NbQueryComponent(nbRefUuid cid, nbRefUuid iid, void **icd);
  NBSIGNSHARED_EXPORT nbResult nbCALL NbEnumComponents(nbIEventHandler *eh);
}

#endif // NBSIGN_H
