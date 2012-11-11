#ifndef NBCC_H
#define NBCC_H

#include "nbcc_global.h"

extern "C" {
  NBCCSHARED_EXPORT nbResult nbCALL NbCanUnload();
  NBCCSHARED_EXPORT nbResult nbCALL NbQueryComponent(nbRefUuid cid, nbRefUuid iid, void **icd);
  NBCCSHARED_EXPORT nbResult nbCALL NbEnumComponents(nbIEventHandler *eh);
}

#endif // NBCC_H
