
#include "Enums.h"

namespace Nb{

const Result Result::ok = Result();


#if !defined(QT_NO_DEBUG_STREAM)

QDebug operator<<(QDebug  dbg, const Timeout &x){
  QDebug d = dbg.nospace();
  static QList<TimeoutEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< ToInfinite << ToNoWait;
    v<<"ToInfinite"<<"ToNoWait";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<(uint32_t)x;
  return d;
}

QDebug operator<<(QDebug  dbg, const TimeType &x){
  QDebug d = dbg.nospace();
  static QList<TimeTypeEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< TtCreate << TtLastModify << TtLastAccess << TtExpirite << TtErrorI << TtErrorII;
    v<<"TtCreate"<<"TtLastModify"<<"TtLastAccess"<<"TtExpirite"<<"TtErrorI"<<"TtErrorII";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<"Tt"<<(uint8_t)x;
  return d;
}

QDebug operator<<(QDebug  dbg, const ConstraintType &x){
  QDebug d = dbg.nospace();
  static QList<ConstraintTypeEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k << CtGe << CtTst << CtAny << CtOwn << CtAll;;
    v <<"CtGe"<<"CtTst"<<"CtAny"<<"CtOwn"<<"CtAll";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<"Ct"<<(uint16_t)x;
  return d;
}

QDebug operator<<(QDebug  dbg, const MetaFormat &x){
  QDebug d = dbg.nospace();
  static QList<MetaFormatEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< MfI1 << MfI2 << MfI4 << MfI8 << MfI16 << MfI32 << MfI64 << MfU1 << MfU2 << MfU4 << MfU8 << MfU16 << MfU32 << MfU64 << MfR16 << MfR32 << MfR64;
    v<<"MfI1"<<"MfI2"<<"MfI4"<<"MfI8"<<"MfI16"<<"MfI32"<<"MfI64"<<"MfU1"<<"MfU2"<<"MfU4"<<"MfU8"<<"MfU16"<<"MfU32"<<"MfU64"<<"MfR16"<<"MfR32"<<"MfR64";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<"Mf"<<(uint8_t)x;
  return d;
}

QDebug operator<<(QDebug  dbg, const MetaType &x){
  QDebug d = dbg.nospace();
  static QList<MetaTypeEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< MtC << MtD << MtCO << MtDO;
    v<<"MtC"<<"MtD"<<"MtCO"<<"MtDO";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<"Mt"<<(uint8_t)x;
  return d;
}

QDebug operator<<(QDebug  dbg, const BlockState &x){
  QDebug d = dbg.nospace();
  static QList<BlockStateEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< BsUntrained << BsTrained << BsPartially;
    v<<"BsUntrained"<<"BsTrained"<<"BsPartially";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<(BlockStateEnum)x;
  return d;
}

QDebug operator<<(QDebug  dbg, const BlockFlags &x){
  QDebug d = dbg.nospace();
  static QList<BlockFlagsEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    //k<< BfLocked << BfProtected << BfHidden << BfTwinContainer <<BfThrough << BfGenerated << BfBackwardTrain << BfIterativeTrain;
    //v<<"BfLocked"<<"BfProtected"<<"BfHidden"<<"BfTwinContainer"<<"BfThrough"<<"BfGenerated"<<"BfBackwardTrain"<<"BfIterativeTrain";
  }

  d<<"{";
  bool once=false;
  for (int i=0; i<k.count(); ++i){
    if (x & k[i]){
      if (once) d<<"|"; else once = true;
      d<< v[i];
    }
  }
  d<<"}";
  return d;
}

QDebug operator<<(QDebug  dbg, const BlockType &x){
  QDebug d = dbg.nospace();
  static QList<BlockTypeEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k << BtNbc << BtAppConv << BtNeuralConv << BtFuzzyConv << BtCryptoConv << BtHardIndicator << BtSoftIndicator << BtErrorSpectator;
    k << BtConnector << BtBioAuditor << BtFuzzyAddresser << BtCodeTransformer << BtSecurity << BtCondition;
    k << BtAppData << BtNbcId << BtUserId << BtDate << BtBioTech << BtCScheme << BtTestResponse << BtExtention;
    v <<"BtNbc"<<"BtAppConv"<<"BtNeuralConv"<<"BtFuzzyConv"<<"BtCryptoConv"<<"BtHardIndicator"<<"BtSoftIndicator"<<"BtErrorSpectator";
    v <<"BtConnector"<<"BtBioAuditor"<<"BtFuzzyAddresser"<<"BtCodeTransformer"<<"BtSecurity"<<"BtCondition";
    v <<"BtAppData"<<"BtNbcId"<<"BtUserId"<<"BtDate"<<"BtBioTech"<<"BtCScheme"<<"BtTestResponse"<<"BtExtention";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<"Bt"<<(uint8_t)x;
  return d;
}

QDebug operator<<(QDebug  dbg, const Purpose &x){
  QDebug d = dbg.nospace();
  static QList<PurposeEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< PuAuthenticate << PuEnter << PuIdentify << PuAudit << PuMultiUser << PuMultiImage << PuBalanced << PuSynthetic;
    v<<"PuAuthenticate"<<"PuEnter"<<"PuIdentify"<<"PuAudit"<<"PuMultiUser"<<"PuMultiImage"<<"PuBalanced"<<"PuSynthetic";
  }

  d<<"{";
  bool once=false;
  for (int i=0; i<k.count(); ++i){
    if (x & k[i]){
      if (once) d<<"|"; else once = true;
      d<< v[i];
    }
  }
  d<<"}";
  return d;
}

QDebug operator<<(QDebug  dbg, const TrainMode &x){
  QDebug d = dbg.nospace();
  static QList<TrainModeEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< TmDefault << TmWithoutFlags << TmWithoutState << TmCheckOnly;
    v<<"TmDefault"<<"TmWithoutFlags"<<"TmWithoutState"<<"TmCheckOnly";
  }

  d<<"{";
  bool once=false;
  for (int i=0; i<k.count(); ++i){
    if (x & k[i]){
      if (once) d<<"|"; else once = true;
      d<< v[i];
    }
  }
  d<<"}";
  return d;
}

QDebug operator<<(QDebug  dbg, const SlotType &x){
  QDebug d = dbg.nospace();
  static QList<SlotTypeEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< StIn << StSpecIn << StSpecOut << StOut;
    v<<"StIn"<<"StSpIn"<<"StSpOut"<<"StConv";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<x;
  return d;
}

QDebug operator<<(QDebug  dbg, const SecureState &x){
  QDebug d = dbg.nospace();
  static QList<SecureStateEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< SsTrained << SsScheme << SsInAny << SsOutAny << SsInOwn << SsOutOwn << SsInfo;
    v<<"SsTrained"<<"SsScheme"<<"SsInAny"<<"SsOutAny"<<"SsInOwn"<<"SsOutOwn"<<"SsInfo";
  }

  d<<"{";
  bool once=false;
  for (int i=0; i<k.count(); ++i){
    if (x & k[i]){
      if (once) d<<"|"; else once = true;
      d<< v[i];
    }
  }
  d<<"}";
  return d;
}

QDebug operator<<(QDebug  dbg, const TestMode &x){
  QDebug d = dbg.nospace();
  static QList<TestModeEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k<< TmE1Bio << TmE2Bio << TmE2BioHumanCompromised << TmE2BioCompromised << TmE2Code << TmE2BioWhiteNoise << TmE2BioCorrNoise << TmE2Conv;
    v<<"TmE1Bio"<<"TmE2Bio"<<"TmE2BioHumanC"<<"TmE2BioC"<<"TmE2Code"<<"TmE2BioWhiteN"<<"TmE2BioCorrN"<<"TmE2Conv";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<"Tm"<<(uint16_t)x;
  return d;
}


QDebug operator<<(QDebug  dbg, const TestResult &x){
  return dbg.nospace()<<"(e1:"<<x.e1Bio<<
                        " e2:"<<x.e2Bio<<
                        " e2HC:"<<x.e2HumanCompromised<<
                        " e2BC:"<<x.e2BioCompromised<<
                        " e2Code:"<<x.e2Code<<
                        " e2WN:"<<x.e2BioWhiteNoise<<
                        " e2CN:"<<x.e2BioCorrNoise<<
                        " e2Conv:"<<x.e2Conv<<")";
}

QDebug operator<<(QDebug  dbg, const ResultCode &x){
  QDebug d = dbg.nospace();
  static QList<ResultCodeEnum> k;
  static QList<char*> v;
  if (k.isEmpty()){
    k << EcFail << EcPointer << EcParam << EcIdentifier << EcIntegrityFail << EcConsistencyFail;
    k << EcOutOfMemory << EcInsufficientBuffer << EcAccessDenied << EcNoModule << EcNoComponent << EcNoInterface;
    k << EcObjectNotFound << EcObjectAlreadyExists << EcTimeoutExpired << EcCancelled << EcIncorrectVersion;
    k << EcIo << EcUnsupportedFunction << EcMatrixNotation << EcUnboundIndex << EcUnboundData << EcMeta;
    k << EcMetaPattern << EcMetaNil << EcNbccState << EcSlotId << EcTestMode << EcTrainMode << EcConstraint;
    k << EcUnsupportedParam << EcCScheme << EcBlockType << EcNumerationRule << EcNoData << EcNoDataOwn << EcNoDataAll;
    k << EcBlockState << EcBlockFlag << EcUnattainableBlockState << EcParamInNumber << EcParamSpecInNumber << EcParamSpecOutNumber;
    k << EcPoorDivirgence << EcPoorQuality << EcPoorStability << EcPoorOriginality << EcTooManyGrossErrors << EcObjectState;

    v <<"EcFail"<<"EcPointer"<<"EcParam"<<"EcIdentifier"<<"EcIntegrityFail"<<"EcConsistencyFail";
    v <<"EcOutOfMemory"<<"EcInsufficientBuffer"<<"EcAccessDenied"<<"EcNoModule"<<"EcNoComponent"<<"EcNoInterface";
    v <<"EcObjectNotFound"<<"EcObjectAlreadyExists"<<"EcTimeoutExpired"<<"EcCancelled"<<"EcIncorrectVersion";
    v <<"EcIo"<<"EcUnsupportedFunction"<<"EcMatrixNotation"<<"EcUnboundIndex"<<"EcUnboundData"<<"EcMeta";
    v <<"EcMetaPattern"<<"EcMetaNil"<<"EcNbccState"<<"EcSlotId"<<"EcTestMode"<<"EcTrainMode"<<"EcConstraint";
    v <<"EcUnsupportedParam"<<"EcCScheme"<<"EcBlockType"<<"EcNumerationRule"<<"EcNoData"<<"EcNoDataOwn"<<"EcNoDataAll";
    v <<"EcBlockState"<<"EcBlockFlag"<<"EcUnattainableBlockState"<<"EcParamInNumber"<<"EcParamSpecInNumber"<<"EcParamSpecOutNumber";
    v <<"EcPoorDivirgence"<<"EcPoorQuality"<<"EcPoorStability"<<"EcPoorOriginality"<<"EcTooManyGrossErrors"<<"EcObjectState";
  }

  int i = k.indexOf(x);
  if (i>=0) d<<v.at(i); else d<<"Ec"<<(uint16_t)x;
  return d;
}

QDebug operator<<(QDebug  dbg, const ResultFacility &x){
  return dbg.nospace()<<(void*)(uint16_t)x;
}

QDebug operator<<(QDebug  dbg, const Result &x){
  //(ok) или (0ha3,EcInvalid)
  if (x.succeeded())
    if (x.code() == ScOk) return dbg.nospace()<<"(ScOk)";
    else                  return dbg.nospace()<<"("<<(nbResult)x<<")";
  else
    return dbg.nospace()<<"("<<x.facility()<<";"<<x.code()<<")";
}

QDebug operator<<(QDebug  dbg, const AsU1 &x){
  if (!x._x) return dbg.nospace()<<"Nil";
  QDebug d = dbg.nospace();

  QString s("%1");
  QString rev("%1%2%3%4%5%6%7%8");
  for (int i=0; i<x._n; ++i){
    QString b= s.arg(x._x[i],8,2,QChar('0'));
    d<<rev.arg(b.at(7)).arg(b.at(6)).arg(b.at(5)).arg(b.at(4)).arg(b.at(3)).arg(b.at(2)).arg(b.at(1)).arg(b.at(0)).toAscii().data();
    //d<<x._x[i]<<" ";
  }
  if (x._l>x._n){
    if (x._l!=2*x._n) d<<"..";
    if (x._l>=2*x._n)
      for (int i=x._l-x._n; i<x._l; ++i){
        QString b= s.arg(x._x[i],8,2,QChar('0'));
        d<<rev.arg(b.at(7)).arg(b.at(6)).arg(b.at(5)).arg(b.at(4)).arg(b.at(3)).arg(b.at(2)).arg(b.at(1)).arg(b.at(0)).toAscii().data();
        //d<<x._x[i]<<" ";
      }
  }
  return d;
}

QDebug operator<<(QDebug  dbg, const AsI32 &x){
  if (!x._x) return dbg.nospace()<<"Nil";
  QDebug d = dbg.nospace();

  for (int i=0; i<x._n; ++i){
    if (x._fc != QChar::Null && (i!=0)) d<<x._fc.toAscii();
    d<<x._x[i];
  }
  if (x._l>x._n){
    if (x._l!=2*x._n) d<<"..";
    if (x._l>=2*x._n)
      for (int i=x._l-x._n; i<x._l; ++i){
        if (x._fc != QChar::Null) d<<x._fc.toAscii();
        d<<x._x[i];
      }
  }
  return d;
}

QDebug operator<<(QDebug  dbg, const AsR32 &x){
  if (!x._x) return dbg.nospace()<<"Nil";
  QDebug d = dbg.nospace();

  for (int i=0; i<x._n; ++i){
    if ((x._fc != QChar::Null) && (i!=0)) d<<x._fc.toAscii();
    d<<x._x[i];
  }
  if (x._l>x._n){
    if (x._l!=2*x._n) d<<"..";
    if (x._l>=2*x._n)
      for (int i=x._l-x._n; i<x._l; ++i){
        if (x._fc != QChar::Null) d<<x._fc.toAscii();
        d<<x._x[i];
      }
  }
  return d;
}

QDebug operator<<(QDebug  dbg, const AsHex &x){
  if (!x._x) return dbg.nospace()<<"Nil";
  QDebug d = dbg.nospace();

  QString s("%1");
  for (int i=0; i<x._n; ++i){
    if ((x._fc != QChar::Null) && (i!=0)) d<<x._fc.toAscii();
    d<<s.arg(x._x[i],2,16,QChar('0')).toAscii().data();
  }
  if (x._l>x._n){
    if (x._l!=2*x._n) d<<"..";
    if (x._l>=2*x._n)
      for (int i=x._l-x._n; i<x._l; ++i){
        if (x._fc != QChar::Null) d<<x._fc.toAscii();
        d<<s.arg(x._x[i],2,16,QChar('0')).toAscii().data();
      }
  }
  return d;
}

#endif

}
