//////////////////////////////////////////////////////////////////////////
//файл:         Enums.h
//версия:
//дата:
//изменен:
//автор:        Майоров А.В.
//
//цель:         Объявляет базовые перечисления и типы данных
//              согласно ГОСТ Р 52633.4 и ГОСТ Р 52633.7
//              в стиле C++, которые объявлены в nb.h
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_ENUMS_H
#define _NB_ENUMS_H

#include <QDebug>
#include "nb.h"

#pragma pack()

namespace Nb {

  /*template<class T> class Null {
  public:
    operator T(){return 0;}
    operator void*(){return 0;}
  };*/
  static const int Null = nbNULL;
  static const int Nil  = 0;

  //Шаблон для упакованного перечисления, кратных 1 байту
  template <typename enumT, typename storageT = unsigned int>
  class packed_enum {
    storageT _value;
  public: 
    packed_enum() { }
    packed_enum(const storageT &value):_value(value){}
    packed_enum(const enumT &value):_value(value){} 
    operator enumT() const { return enumT(_value); }
    storageT packed() const { return _value; }
  };

  //Шаблон для упакованных флагов, кратных 1 байту и с заданной маской значений
  template <typename enumT, typename storageT>
  class packed_flags {
    storageT _values;
  public:
    packed_flags():_values(0){ }
    packed_flags(const storageT values):_values(values){}
    packed_flags(const packed_flags &values):_values(values._values){}
    packed_flags(const enumT &value):_values(value){} 

    bool test(enumT &value) const { return (_values & (storageT(value))); }

    operator storageT() const { return _values; }

    bool operator!() const { return (_values == 0);}

    packed_flags & operator= ( const packed_flags & other ) { _values = other._values; return *this; }
    packed_flags 	operator& ( packed_flags mask ) const     { return packed_flags(_values & mask._values); }
    packed_flags 	operator& ( enumT mask ) const            { return packed_flags(_values & (storageT)mask); }
    packed_flags & operator&= ( packed_flags mask )         { _values &= mask._values; return *this; }
    packed_flags & operator&= ( enumT mask )                { _values &= (storageT)mask; return *this; }
    packed_flags 	operator^ ( packed_flags other ) const    { return packed_flags(_values ^ other._values); }
    packed_flags 	operator^ ( enumT other ) const           { return packed_flags(_values ^ (storageT)other); }
    packed_flags & operator^= ( packed_flags other )        { _values ^= other._values; return *this; }
    packed_flags & operator^= ( enumT other )               { _values ^= (storageT)other; return *this; }
    packed_flags 	operator| ( packed_flags other ) const    { return packed_flags(_values | other._values);}
    packed_flags 	operator| ( enumT other ) const           { return packed_flags(_values | (storageT)other);}
    packed_flags & operator|= ( packed_flags other )        { _values |= other._values; return *this; }
    packed_flags & operator|= ( enumT other )               {  _values |= (storageT)other; return *this; }
    packed_flags operator~ () const                         { return packed_flags(~_values); }
  };

  enum TimeoutEnum {
    ToInfinite = nbTIMEOUT_INFINITE,
    ToNoWait   = nbTIMEOUT_NO_WAIT
  };

  enum TimeTypeEnum {
    TtCreate      = nbTIME_CREATE,
    TtLastModify  = nbTIME_LAST_MODIFY,
    TtLastAccess  = nbTIME_LAST_ACCESS,
    TtExpirite    = nbTIME_EXPIRITE,
    TtErrorI      = nbTIME_ERROR_I,
    TtErrorII     = nbTIME_ERROR_II
  };

  enum ConstraintTypeEnum {
    CtGe      = -1,
    CtTst     = 0,
    CtAny     = nbCT_DATA,
    CtOwn     = nbCT_DATA_OWN,
    CtAll     = nbCT_DATA_ALL,

    CtSigma   = nbCT_SIGMA,
    CtInterval  = nbCT_INTERVAL,
    CtDisplacement = nbCT_DISPLACEMENT,
    CtDiscreteStep  = nbCT_DISCRETE_STEP,
    CtRangeLimit  = nbCT_RANGE_LIMIT,
    CtOwnLaw  = nbCT_OWN_LAW,
    CtAllLaw  = nbCT_ALL_LAW,

    CtAlgorithm = nbCT_ALGORITHM,
    CtSalt = nbCT_SALT,
    CtDetectionRate = nbCT_ERROR_DETECTION_RATE,
    CtCorrectionRate = nbCT_ERROR_CORRECTION_RATE,
    CtStrengthReduction = nbCT_CRYPTO_STRENGTH_REDUCTION,
    CtConnId = nbCT_CONN_ID
  };

  enum MetaFormatEnum {
    MfAny = nbMF_ANY,
    MfI1  = nbMF_I1,
    MfI2  = nbMF_I2,
    MfI4  = nbMF_I4,
    MfI8  = nbMF_I8,
    MfI16 = nbMF_I16,
    MfI32 = nbMF_I32,
    MfI64 = nbMF_I64,
    MfU1  = nbMF_U1,
    MfU2  = nbMF_U2,
    MfU4  = nbMF_U4,
    MfU8  = nbMF_U8,
    MfU16 = nbMF_U16,
    MfU32 = nbMF_U32,
    MfU64 = nbMF_U64,
    MfR16 = nbMF_R16,
    MfR32 = nbMF_R32,
    MfR64 = nbMF_R64
  };

  enum MetaTypeEnum {
    MtAny           = nbMT_ANY,
    MtContinuous    = nbMT_CONTINUOUS,
    MtDiscrete      = nbMT_DISCRETE,
    MtContinuousOwn = nbMT_CONTINUOUS_OWN,
    MtDiscreteOwn   = nbMT_DISCRETE_OWN,
    MtC             = MtContinuous,
    MtD             = MtDiscrete,
    MtCO            = MtContinuousOwn,
    MtDO            = MtDiscreteOwn
  };

  enum BlockStateEnum {
    BsUntrained = nbBS_UNTRAINED,
    BsTrained   = nbBS_TRAINED,
    BsPartially = nbBS_PARTIALLY_TRAINED
  };

  enum BlockFlagsEnum {
    BfNone          = nbBF_NONE,
    //BfExcludeUnused = nbBF_EXCLUDE_UNUSED,
    BfLocked        = nbBF_EXCLUDE_UNUSED,
    BfProtected     = nbBF_PROTECTED,
    BfHidden        = nbBF_HIDDEN,
    BfTwinContainer = nbBF_TWINCONTAINER,
    BfThrough       = nbBF_THROUGH,
    BfGenerated     = nbBF_GENERATOR,
    BfBackwardTrain = nbBF_BACKWARD_TRAIN,
    BfIterativeTrain = nbBF_ITERATIVE_TRAIN
  };

  enum BlockTypeEnum {
    BtNbc         = nbBT_NBC,
    BtAppConv     = nbBT_1,
    BtNeuralConv  = nbBT_NEURAL_NET_CONVERTER,
    BtFuzzyConv   = nbBT_FUZZY_CONVERTER,
    BtCryptoConv  = nbBT_CRYPTO_CONVERTER,
    BtHardIndicator = nbBT_HARD_INDICATOR,
    BtSoftIndicator = nbBT_SOFT_INDICATOR,
    BtErrorSpectator = nbBT_ERROR_SPECTATOR,
    BtConnector   = nbBT_CONNECTOR,
    BtBioAuditor  = nbBT_BIO_AUDITOR,
    BtFuzzyAddresser = nbBT_FUZZY_ADDRESSER,
    BtCodeTransformer = nbBT_CODE_TRANSFORMER,
    BtSecurity    = nbBT_SECURITY,
    BtCondition   = nbBT_CONDITION,

    BtAppData     = nbBT_2,
    BtNbcId       = nbBT_NBC_ID,
    BtUserId      = nbBT_USER_ID,
    BtDate        = nbBT_DATE,
    BtBioTech     = nbBT_BIO_TECH,
    BtCScheme     = nbBT_CSCHEME,
    BtTestResponse = nbBT_TEST_RESPONSE,
    BtExtention   = nbBT_EXTENSION
  };

  enum PurposeEnum {
    PuUnknown       = nbPURPOSE_UNKNOWN,
    PuAuthenticate  = nbPURPOSE_AUTHENTICATE,
    PuEnter         = nbPURPOSE_ENTER,
    PuIdentify      = nbPURPOSE_IDENTIFY,
    PuAudit         = nbPURPOSE_AUDIT,
    PuMultiUser     = nbPURPOSE_MULTI_USER,
    PuMultiImage    = nbPURPOSE_MULTI_IMAGE,
    PuBalanced      = nbPURPOSE_BALANCED,
    PuSynthetic     = nbPURPOSE_SYNTHETIC
  };

  enum TrainModeEnum {
    TmDefault       = nbTRAIN_MODE_DEFAULT,
    TmWithoutFlags  = nbTRAIN_MODE_WITHOUT_FLAGS,
    TmWithoutState  = nbTRAIN_MODE_WITHOUT_STATE,
    TmCheckOnly     = nbTRAIN_MODE_CHECK_ONLY
  };

  enum SlotTypeEnum {
    StIn      = nbSLOT_TYPE_IN,
    StSpecIn  = nbSLOT_TYPE_SPEC_IN,
    StSpecOut = nbSLOT_TYPE_SPEC_OUT,
    StOut     = nbSLOT_TYPE_CONV,
    StConvertor = StOut
  };

  enum SecureStateEnum {
    SsInitial     = nbNS_INITIAL,     //начальное состояние
    SsAll         = nbNS_ALL,         //все допустимые состояния
    SsTrained     = nbNS_TRAINED,     //сброс параметров преобразования и результатов тестирования
    SsScheme      = nbNS_CSCHEME,     //схема связывания + выходные значения
    SsInAny       = nbNS_SPEC_IN,     //входные "Любой"
    SsOutAny      = nbNS_SPEC_OUT,    //выходной "Любой" + сп.вых.
    SsInOwn       = nbNS_CRITICAL_IN, //входные "Свой", "Все чужие"
    SsOutOwn      = nbNS_CRITICAL_OUT,//выходной "Свой", "Все чужие" + сп.вых.
    SsInfo        = 0x0100            //определены информационные блоки
  };

  enum TestModeEnum {
    TmE1Bio = nbTEST_MODE_E1_BIO,
    TmE2Bio = nbTEST_MODE_E2_BIO,
    TmE2BioHumanCompromised = nbTEST_MODE_E2_BIO_HUMAN_COMPROMISED,
    TmE2BioCompromised = nbTEST_MODE_E2_BIO_COMPROMISED,
    TmE2Code = nbTEST_MODE_E2_CODE,
    TmE2BioWhiteNoise = nbTEST_MODE_E2_BIO_WHITE_NOISE,
    TmE2BioCorrNoise = nbTEST_MODE_E2_BIO_CORR_NOISE,
    TmE2Conv = nbTEST_MODE_E2_CONV,
    TmE1 = TmE1Bio,
    TmE2 = TmE2Bio
  };

  typedef packed_enum<TimeoutEnum,nbTimeout>    Timeout;
  typedef packed_enum<TimeTypeEnum,nbTimeType>  TimeType;
  typedef packed_enum<ConstraintTypeEnum,nbConstraintType> ConstraintType;
  typedef packed_enum<MetaFormatEnum, nbMetaFormat> MetaFormat;
  typedef packed_enum<MetaTypeEnum, nbMetaType> MetaType;
  typedef packed_enum<BlockStateEnum, nbBlockState> BlockState;
  typedef packed_flags<BlockFlagsEnum, nbBlockFlags> BlockFlags;
  typedef packed_enum<BlockTypeEnum, nbBlockType> BlockType;
  typedef packed_enum<PurposeEnum, nbPurpose> Purpose;
  typedef packed_flags<TrainModeEnum, nbTrainMode> TrainMode;
  typedef packed_enum<SlotTypeEnum, nbSlotID> SlotType;

  typedef QList<SlotType> SlotTypes;  //Список типов
  typedef QList<int32_t>  SlotNums;   //Список номеров позиций слотов

  typedef packed_flags<SecureStateEnum, nbNbccState> SecureState;
  typedef packed_enum<TestModeEnum, nbTestMode> TestMode;

  typedef nbHandle Handle;
  typedef nbTime   Time;
  typedef nbTestResult  TestResult;

  enum ResultCodeEnum {
    ScOk      = nbS_OK,
    ScCancel  = nbS_CANCEL,
    ScSkip    = nbS_SKIP,

    EcFail    = nbECODE_FAIL,
    EcPointer = nbECODE_POINTER,
    EcParam   = nbECODE_PARAM,
    EcIdentifier = nbECODE_IDENTIFIER,
    EcIntegrityFail = nbECODE_INTEGRITY_FAIL,
    EcConsistencyFail = nbECODE_CONSISTENCY_FAIL,
    EcOutOfMemory = nbECODE_OUT_OF_MEMORY,
    EcInsufficientBuffer = nbECODE_INSUFFICIENT_BUFFER,
    EcAccessDenied = nbECODE_ACCESS_DENIED,
    EcNoModule = nbECODE_NO_MODULE,
    EcNoComponent = nbECODE_NO_COMPONENT,
    EcNoInterface = nbECODE_NO_INTERFACE,
    EcObjectNotFound = nbECODE_OBJECT_NOT_FOUND,
    EcObjectAlreadyExists = nbECODE_OBJECT_ALREADY_EXISTS,
    EcTimeoutExpired = nbECODE_TIMEOUT_EXPIRED,
    EcCancelled = nbECODE_CANCELLED,
    EcIncorrectVersion = nbECODE_INCORRECT_VERSION,
    EcIo = nbECODE_IO,
    EcUnsupportedFunction = nbECODE_UNSUPPORTED_FUNCTION,

    EcMatrixNotation = nbECODE_MATRIX_NOTATION,
    EcUnboundIndex = nbECODE_UNBOUND_INDEX,
    EcUnboundData = nbECODE_UNBOUND_DATA,
    EcMeta = nbECODE_META,
    EcMetaPattern = nbECODE_META_PATTERN,
    EcMetaNil = nbECODE_META_NIL,

    EcNbccState = nbECODE_NBCC_STATE,
    EcSlotId = nbECODE_SLOT_ID,
    EcTestMode = nbECODE_UNSUPPORTED_TEST_MODE,
    EcTrainMode = nbECODE_UNSUPPORTED_TRAIN_MODE,
    EcConstraint = nbECODE_UNSUPPORTED_CONSTRAINT,
    EcUnsupportedParam = nbECODE_UNSUPPORTED_PARAM,
    EcCScheme = nbECODE_UNSUPPORTED_CSCHEME,
    EcBlockType = nbECODE_UNSUPPORTED_BLOCK_TYPE,
    EcNumerationRule = nbECODE_NUMERATION_RULE,

    EcNoData = nbECODE_NO_DATA,
    EcNoDataOwn = nbECODE_NO_DATA_OWN,
    EcNoDataAll = nbECODE_NO_DATA_ALL,

    EcBlockState = nbECODE_BLOCK_STATE,
    EcBlockFlag = nbECODE_BLOCK_FLAG,
    EcUnattainableBlockState = nbECODE_UNATTAINABLE_BLOCK_STATE,
    EcParamInNumber = nbECODE_PARAM_IN_NUMBER,
    EcParamSpecInNumber = nbECODE_PARAM_SPEC_IN_NUMBER,
    EcParamSpecOutNumber = nbECODE_PARAM_SPEC_OUT_NUMBER,
    EcPoorDivirgence = nbECODE_POOR_DIVERGENCE,
    EcPoorQuality = nbECODE_POOR_QUALITY,
    EcPoorStability = nbECODE_POOR_STABILITY,
    EcPoorOriginality = nbECODE_POOR_ORIGINALITY,
    EcTooManyGrossErrors = nbECODE_TOO_MANY_GROSS_ERRORS,

    EcObjectState = nbECODE_OBJECT_STATE
  };

  enum ResultFacilityEnum {
    RfSys = nbF_SYS,
    RfDef = nbF_DEF
  };

  typedef packed_enum<ResultCodeEnum, uint16_t> ResultCode;
  typedef packed_enum<ResultFacilityEnum, uint16_t> ResultFacility;


  class Result {
    nbResult _m;
  public:
    Result():_m(nbS_OK){}
    Result(const Result& r):_m(r._m){}
    Result(const nbResult r):_m(r){}
    Result(ResultFacility rf, ResultCode rc):_m(Result(nbE(rf,rc))){}
    Result(ResultCode rc):_m(Result(nbE(RfDef,rc))){}

    Result& operator=(const Result& r){ _m = r._m; return *this;}
    Result& operator=(const nbResult r){ _m = r; return *this;}

    inline bool operator==(const Result& r)const{ return (_m==r._m); }
    inline bool operator!=(const Result& r)const{ return (_m!=r._m); }

    inline operator nbResult*() {return &_m;}
    inline operator const nbResult*() const {return &_m;}
    inline operator nbResult() const { return _m; }
   
    ResultCode code() const { return ResultCode(nbRESULT_ECODE_MASK &_m); }
    ResultFacility facility() const { return ResultFacility((nbRESULT_F_MASK &_m)>>16);}

    bool succeeded() const { return nbSUCCEEDED(_m);}
    bool failed() const { return nbFAILED(_m);}

    static Result success(ResultFacility rf, ResultCode rc){ return Result(nbS(nbE(rf,rc))); }
    static Result success(ResultCode rc){ return Result(nbS(rc)); }

    static Result fail(ResultFacility rf, ResultCode rc){ return Result(nbE(rf,rc)); }
    static Result fail(uint16_t sub, ResultCode rc){ return Result(nbE(nbF_SUB(sub),rc)); }
    static Result fail(ResultCode rc){ return Result(nbE(RfDef,rc)); }

    static const Result ok;
  };


#if !defined(QT_NO_DEBUG_STREAM)

  #define ELOGQUOTE(x) #x
  #define ELOG(x) qDebug()<<"@err"<<ELOGQUOTE(x)<<" line:"<<__LINE__<<" file:"__FILE__;
  #define ELOG2(x,name) qDebug()<<"@err"<<ELOGQUOTE(x)<<ELOGQUOTE(name)<<" line:"<<__LINE__;

  QDebug operator<<(QDebug  dbg, const Timeout &x);
  QDebug operator<<(QDebug  dbg, const TimeType &x);
  QDebug operator<<(QDebug  dbg, const ConstraintType &x);
  QDebug operator<<(QDebug  dbg, const MetaFormat &x);
  QDebug operator<<(QDebug  dbg, const MetaType &x);
  QDebug operator<<(QDebug  dbg, const BlockState &x);
  QDebug operator<<(QDebug  dbg, const BlockFlags &x);
  QDebug operator<<(QDebug  dbg, const BlockType &x);
  QDebug operator<<(QDebug  dbg, const Purpose &x);
  QDebug operator<<(QDebug  dbg, const TrainMode &x);
  QDebug operator<<(QDebug  dbg, const SlotType &x);
  QDebug operator<<(QDebug  dbg, const SecureState &x);
  QDebug operator<<(QDebug  dbg, const TestMode &x);
  QDebug operator<<(QDebug  dbg, const TestResult &x);

  QDebug operator<<(QDebug  dbg, const ResultCode &x);
  QDebug operator<<(QDebug  dbg, const ResultFacility &x);
  QDebug operator<<(QDebug  dbg, const Result &x);

  //Форматирование по битам
  class AsU1 {
  public:
    AsU1(uint8_t *x, int32_t n, int32_t length=-1):_x(x),_n(n),_l(length){
      if ((length!= -1) && (2*n>length)) _n = length/2;
      if (length==1) _n=1;
    }


    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const AsU1 &x);
    #endif
  protected:
    uint8_t *_x;
    int32_t _n;
    int32_t _l;
  };

  class AsI32 {
  public:
    AsI32(int32_t *x, int32_t n, const QChar &fillChar = QLatin1Char(' '), int32_t length=-1):_x(x),_n(n),_fc(fillChar),_l(length){
      if ((length!= -1) && (2*n>length)) _n = length/2;
      if (length==1) _n=1;
    }


    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const AsI32 &x);
    #endif
  protected:
    int32_t *_x;
    int32_t _n;
    const QChar& _fc;
    int32_t _l;
  };

  class AsR32 {
  public:
    AsR32(float *x, int32_t n, const QChar &fillChar = QLatin1Char(' '), int32_t length=-1):_x(x),_n(n),_fc(fillChar),_l(length){
      if ((length!= -1) && (2*n>length)) _n = length/2;
      if (length==1) _n=1;
    }

    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const AsR32 &x);
    #endif
  protected:
    float *_x;
    int32_t _n;
    const QChar& _fc;
    int32_t _l;
  };

  class AsHex {
  public:
    AsHex(uint8_t *x, int32_t n, const QChar &fillChar = QLatin1Char(' '), int32_t length=-1):_x(x),_n(n),_fc(fillChar),_l(length){
      if ((length!= -1) && (2*n>length)) _n = length/2;
      if (length==1) _n=1;
    }

    #if !defined(QT_NO_DEBUG_STREAM)
      QDebug friend operator<<(QDebug  dbg, const AsHex &x);
    #endif
  protected:
    uint8_t *_x;
    int32_t _n;
    const QChar& _fc;
    int32_t _l;
  };

#endif

}

#pragma pack()
#endif //_NB_ENUMS_H
