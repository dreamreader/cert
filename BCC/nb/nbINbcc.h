 ///////////////////////////////////////////////////////////////////////////////
//файл:         nbINbcc.h
//версия:
//дата:
//изменен:
//автор:
//
//цель:         Описание ПИК НПБК
///////////////////////////////////////////////////////////////////////////////
#ifndef _NB_INBCC_H
#define _NB_INBCC_H

#include "nbTypes.h"
#include "nbResult.h"
#include "nbIUnknown.h"

//.............................................................................

//  События ПИК НПБК

//Событие: прогресс
//  nbSlotID  paramA - (входной) интерпретируется номер текущего слота.
//  int32_t paramB - (входной) интерпретируется как текущее значение от 0 до 2^31 (максимальное значение означает завершенное обучение).
//                    Номер слота, значение общего прогресса.
//возвращает: nbS_OK, nbS_DENY
#define nbEVENT_NBCC_PROGRESS              (0x00000401)    //  Обработка (обучение)

//.............................................................................
//  ПИК НПБК
//.............................................................................

// Идентификатор интерфейса nbINbcc
// {A3213E60-57B3-4643-84E4-11CAB9E81FFE}
static const nbUuid nbUUID_INBCC = 
  {0xa3213e60,0x57b3,0x4643,{0x84,0xe4,0x11,0xca,0xb9,0xe8,0x1f,0xfe}};

#undef  nbINTERFACE
#define nbINTERFACE   nbINbcc

nbDECLARE_INTERFACE(nbINbcc, nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //Установка обработчика событий НПБК
  //  handler     - [вх]  Описатель ПИК Обработчик событий (если nbNULL, обработчик сбрасывается)
  //возвращает: nbS_OK
  //            nbECODE_UNSUPPORTED_PARAM
  nbMETHOD(setEventHandler)(nbTHISc nbIEventHandler *handler) nbPURE;

  //Установка схемы преобразования
  //  cscheme     - [вх]  Схема преобразования в двоичном формате или nbNULL,
  //                      если должна использоваться схема "по умолчанию"
  //возвращает: nbS_OK
  //            nbECODE_OUT_OF_MEMORY
  //            nbECODE_CONSISTENCY_FAIL
  //            nbECODE_UNSUPPORTED_CSCHEME
  //            nbECODE_UNSUPPORTED_BLOCK_TYPE
  //            nbECODE_META
  //            nbECODE_PARAM_IN_NUMBER
  //            nbECODE_PARAM_SPEC_IN_NUMBER
  //            nbECODE_PARAM_SPEC_OUT_NUMBER
  //            nbECODE_BLOCK_FLAG
  //            nbECODE_BLOCK_STATE
  //            nbECODE_NUMERATION_RULE
  nbMETHOD(setConvScheme)(nbTHISc const nbData cScheme) nbPURE;

  //Перечисление известных НПБК схем преобразования и их адаптация к входным/выходным параметрам
  //  csid        - [вх]  Глобальный идентификатор схемы преобразования.
  //                      Если значение идентификатора меньше 65536, идентификатор определяет порядковый номер схемы преобразования,
  //                      известный НПБК. Значение nbUUID_NIL задает схему по умолчанию.
  //  mIn         - [вх/необ] Шаблон для матрицы входных биометрических параметров
  //  mCode       - [вх/необ] Шаблон выходного кода.
  //  convScheme  - [вых] Схема преобразования
  //возвращает: nbS_OK
  //            nbECODE_IDENTIFIER
  //            nbECODE_UNBOUND_INDEX
  //            nbECODE_INSUFFICIENT_BUFFER
  //            nbECODE_META
  //            nbECODE_PARAM_IN_NUMBER
  //            nbECODE_PARAM_OUT_NUMBER
  nbMETHOD(enumConvScheme)(nbTHISc nbRefUuid csid, const nbMatrix *mIn, const nbMatrix *mCode, nbData cScheme) nbPURE;

  //Получение описания слота на момент вызова в формате схемы преобразования
  //  id        - [вх] Номер слота в схеме
  //  slotDescr - [вых/необ] Описание слота (как часть схемы преобразования).
  //                  Если равен nbNULL, данные не передаются.
  //                  Если размер недостаточен, записывается требуемый размер буфера
  //возвращает: номер следующего слота по схеме (номера образуют кольцо)
  //            nbECODE_INVALID_NBCC_STATE
  //            nbECODE_INSUFFICIENT_BUFFER
  nbMETHOD(getSlotDescr)(nbTHISc nbSlotID id, nbData slotDescr) nbPURE;

  //Установка параметра или свойства для его чтения/записи
  //  id          - [вх]  Номер слота НПБК.
  //  conType     - [вх]  Тип ограничения.
  //  mCon        - [вх]  Значение ограничения.
  //                  Если равно NIL, ограчение (или значение параметра) принимается по умолчанию.
  //                  Если метаописание равно META_NIL, значение параметра не должно вычисляться или использоваться.
  //возвращает: nbS_OK
  //            nbECODE_ACCESS_DENIED
  //            nbECODE_INVALID_NBCC_STATE
  //            nbECODE_INVALID_SLOT
  //            nbECODE_UNSUPPORTED_CONSTRAINT
  //            nbECODE_UNSUPPORTED_PARAM
  //            nbECODE_NO_DATA
  //            nbECODE_UNBOUND_DATA
  //            nbECODE_MATRIX_NOTATION
  //            nbECODE_META
  nbMETHOD(setConstraint)(nbTHISc nbSlotID id, nbConstraintType conType, nbMatrix *mCon) nbPURE;
  
  //Чтение свойства или параметра без установки
  //  Если содержит только метаописание, возвращается его метаописание, а также заполняется поле ncols
  //примечание:
  //  1. формат ограничения определяется последней версией стандарта.
  //  2. формат параметра определяется схемой
  nbMETHOD(getConstraint)(nbTHISc nbSlotID id, nbConstraintType conType, nbMatrix *mCon) nbPURE;

  //Извлечение кода-доступа из параметров НПБК.
  //(должен поддерживаться как минимум режим эмуляции с одним примером)
  //(входные идентификаторы настраиваются)
  //  Преобразователь должен быть предварительно настроен через обучение или
  //  импорт нейростевого биометрического контейнера.
  //  Выделение памяти проводится вызывающей стороной.
  //
  //  mIn       - [вх]  Матрица ВБП биометрических образов. Если на входе несколько примеров,
  //                      то матрица выходных кодов также должна содержать набор выходных кодов.
  //  mOut      - [вых] Матрица кодов доступа, содержащая 1 или более вектор кода доступа.
  //  ids       - [вх/необ] Список слотов, с которых снимаются значения.
  //                    Значение nbNULL определяет один слот выхода.
  //возвращает: nbS_OK
  //            см. коды ошибок SetConstraint
  //            nbECODE_NBCC_INSUFFICIENT_BASE_ANY
  //            nbECODE_WRONG_IN_NUMBER
  //            nbECODE_WRONG_OUT_NUMBER
  nbMETHOD(extract)(nbTHISc const nbMatrix *mIn, nbMatrix *mOut, const nbSlotID *ids) nbPURE;

  //Обучение
  //  mInOwn      - [вх]  Матрица ВБП "Свой".
  //  mInAll      - [вх/необ] Матрица ВБП "Все Чужие". Если не задан, используются внутренние гипотезы распределения
  //                    образов "Все Чужие" или заданные предварительно специальные параметры.
  //  mOutCode    - [вх]  Матрица кода доступа. Должна содержать только 1 пример кода доступа.
  //  timeout     - [вх] Максимальное время ожидания завершения обучения. Если время истекает раньше завершения обучения, обучение прерывается.
  //возвращает: nbS_OK
  //            см. коды ошибок SetParam
  //            см. коды ошибок PostTrain
  nbMETHOD(train)(nbTHISc const nbMatrix *mInOwn, const nbMatrix *mInAll, const nbMatrix *mCode, nbTimeout timeout) nbPURE;

  //Дообучение отдельного блока преобразования (или связанной группы)
  //  Используется для блоков, для которых задан флаг nbBLOCK_FLAG_POST_TRAIN
  //  id          - [вх]  Слот блока обработки
  //  trainMode   - [вх]  Режим дообучения
  //  blockState  - [вх]  указатель на состояние блока, до которого выполняется дообучение (как в большую сторону, так и в меньшую)
  //  blockFlags  - [вх]  указатель на флаги устанавливаемые/сбрасываемые флаги блока
  //  timeout     - [вх]  Максимальное время ожидания завершения обучения. Если время истекает раньше завершения обучения, обучение прерывается.
  //возвращает: номер текущего состояния
  //            nbECODE_TIMEOUT_EXPIRED
  //            nbECODE_CANCELLED
  //            nbECODE_ACCESS_DENIED
  //            nbECODE_INVALID_NBCC_STATE
  //            nbECODE_INVALID_SLOT
  //
  //            nbECODE_BLOCK_STATE
  //            nbECODE_BLOCK_FLAG
  //            nbECODE_UNATTAINABLE_BLOCK_STATE
  //            nbECODE_UNSUPPORTED_TRAIN_MODE
  //
  //            nbECODE_NO_DATA
  //            nbECODE_NO_DATA_OWN
  //            nbECODE_NO_DATA_ALL
  //            nbECODE_POOR_DIVERGENCE
  //            nbECODE_POOR_QUALITY
  //            nbECODE_POOR_STABILITY
  //            nbECODE_POOR_ORIGINALITY
  //            nbECODE_TOO_MANY_GROSS_ERRORS
  nbMETHOD(postTrain)(nbTHISc nbSlotID id, nbTrainMode trainMode, nbBlockState blockState, nbBlockFlags blockFlags, nbTimeout timeout) nbPURE;
  
  //Индикация грубых ошибок входных параметров 
  //(начальная фильтрация заведомо плохих образов путем их ранжирования)
  //  Используется перед вызовом функции Learn для отсечения заведомо "слабых", "некачественных" образов.
  //  mInOwn     - [вх]  Матрица ВБП "Свой".
  //  mInAll     - [вх]  Матрица ВБП "Все чужие".
  //  mGe        - [вых] Матрица, содержащая вектор индикаторов, показывающих нормализованное
  //                      отклонение примеров от среднего значения. Число элементов вектора
  //                      равно числу примеров матрицы ВБП "Свой". Элементами могут быть
  //                      вещественные и целые числа. Значения [0.0 .. 1.0] или [0 .. 100] означают 
  //                      подходящие образы, а значения больше 1.0 или 100 - сильно отличающиеся
  //                      образы для вещественных и целых чисел соответственно.
  //возвращает: nbS_OK
  nbMETHOD(indicateGrossErrors)(nbTHISc const nbMatrix *mInOwn, const nbMatrix *mInAll, nbMatrix *mGe) nbPURE;

  //Экспресс тестирование параметров НПБК
  //(может задаваться несколько параметров)
  //  Выполняет преобразование биометрия-код и собирает статистическую информацию о результатах.
  //  testMode    - [вх] Режим тестирования
  //  mEstimate   - [вых] Матрица одного параметра (в вещественном или целом формате)
  //  timeout     - [вх] Максимальное время, в течение которого выполняется расчет
  //возвращает: nbS_OK
  //            nbECODE_NBCC_STATE
  //            nbECODE_CANCELLED
  //            nbECODE_UNSUPPORTED_TEST_MODE
  nbMETHOD(test)(nbTHISc nbTestMode testMode, nbMatrix *mEstimate, nbTimeout timeout) nbPURE;

  //Сброс буферов преобразователя биометрия-код
  //  flags       - [вх]  Набор сбрасываемых флагов НПБК, объединенных операцией "или"
  //возвращает: фактическое состояние после вызова (сбрасываются флаги, которые могут быть сброшены)
  //            nbECODE_FAIL
  //            nbECODE_INVALID_NBCC_STATE
  nbMETHOD(reset)(nbTHISc nbNbccState nbccState) nbPURE;

  //Экспорт биометрического контейнера или блока в двоичном формате
  //  nbc         - [вых] Нейросетевой биометрический контейнер или блок
  //  nbcType     - [вх] Требуемый формат НБК (по умолчанию nbUUID_NIL)
  //возвращает: nbS_OK
  //            nbECODE_INSUFFICIENT_BUFFER
  //            nbECODE_INVALID_NBCC_STATE
  //            nbECODE_INCORRECT_VERSION
  nbMETHOD(exportNbc)(nbTHISc nbData nbc, nbRefUuid nbcType) nbPURE;
  
  //Импорт биометрического контейнера или блока в двоичном формате
  //  nbc         - [вх]  Нейросетевой биометрический контейнер или блок
  //возвращает: nbS_OK
  //            см. коды ошибок SetConnScheme
  //            nbECODE_OUT_OF_MEMORY
  //            nbECODE_INCORRECT_VERSION
  //            nbECODE_CONSISTENCY_FAIL
  //            nbECODE_DATA
  nbMETHOD(importNbc)(nbTHISc const nbData nbc) nbPURE;

};

#endif
