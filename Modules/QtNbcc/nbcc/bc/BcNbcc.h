#ifndef BCNBCC_H
#define BCNBCC_H

#include "qatomic.h"
#include "qthread.h"

#include "../nb/nbc.h"
#include "BcScheme.h"

namespace Bc {

using namespace  Nb;

  //.............................................................................
  //Идентификатор компонента преобразования биометрия-код
  // {9AC0FE89-596E-4414-9436-05122BD94D34}
  static const nbUuid nbUUID_NBCC =
  { 0x9ac0fe89, 0x596e, 0x4414, { 0x94, 0x36, 0x5, 0x12, 0x2b, 0xd9, 0x4d, 0x34 } };


  //.............................................................................
  //Преобразователь биометрия-код
  //.............................................................................
  class BcNbcc:public nbINbcc {
  public:
    BcNbcc();
    ~BcNbcc();

    nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd);
    nbMETHOD(retain)(nbTHIS);
    nbMETHOD(release)(nbTHIS);

    nbMETHOD(setEventHandler)(nbTHISc nbIEventHandler *handler);
    nbMETHOD(setConvScheme)(nbTHISc const nbData cScheme);
    nbMETHOD(enumConvScheme)(nbTHISc nbRefUuid csid, const nbMatrix *mIn, const nbMatrix *mCode, nbData cScheme);
    nbMETHOD(getSlotDescr)(nbTHISc nbSlotID id, nbData slotDescr);
    nbMETHOD(setConstraint)(nbTHISc nbSlotID id, nbConstraintType conType, nbMatrix *mCon);
    nbMETHOD(getConstraint)(nbTHISc nbSlotID id, nbConstraintType conType, nbMatrix *mCon);
    nbMETHOD(extract)(nbTHISc const nbMatrix *mIn, nbMatrix *mOut, const nbSlotID *ids);
    nbMETHOD(train)(nbTHISc const nbMatrix *mInOwn, const nbMatrix *mInAll, const nbMatrix *mCode, nbTimeout timeout);
    nbMETHOD(postTrain)(nbTHISc nbSlotID id, nbTrainMode trainMode, nbBlockState blockState, nbBlockFlags blockFlags, nbTimeout timeout);
    nbMETHOD(indicateGrossErrors)(nbTHISc const nbMatrix *mInOwn, const nbMatrix *mInAll, nbMatrix *mGe);
    nbMETHOD(test)(nbTHISc nbTestMode testMode, nbMatrix *mEstimate, nbTimeout timeout);
    nbMETHOD(reset)(nbTHISc nbNbccState nbccState);
    nbMETHOD(exportNbc)(nbTHISc nbData nbc, nbRefUuid nbcType);
    nbMETHOD(importNbc)(nbTHISc const nbData nbc);

    static nbResult getCoRefs();
  private:
    QAtomicInt _iRefs;
    static QAtomicInt _cRefs;
    BcScheme  _scheme;
  };

}



#endif // BCNBCC_H
