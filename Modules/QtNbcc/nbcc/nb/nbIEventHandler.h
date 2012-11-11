///////////////////////////////////////////////////////////////////////////////
//файл:         nbIEventHandler.h
//версия:       0.9
//дата:         01.01.2010
//изменен:
//автор:
//
//цель:         Описание ПИК Обработчик событий nbIEventHandler,
//              типа nbEvent, константы nbUUID_IEVENT_HANDLER
///////////////////////////////////////////////////////////////////////////////

#ifndef _NB_IEVENTHANDLER_H
#define _NB_IEVENTHANDLER_H

#include "nbTypes.h"
#include "nbIUnknown.h"

//!.............................................................................

//Событие
//  Используются для уведомления о событиях, происходящих внутри компонентов,
//  других компонентов. Каждый компонент может определять свой набор событий.
//  Номера событий [0x00000000..0x0000FFFF] зарезервированы.
typedef int32_t  nbEvent;

//Событие: объект перечисления
//  int32_t paramA - (входной) интерпретируется как порядковый номер объекта или 0.
//  void*   paramB - (входной) интерпретируется как указатель на перечисляемый объект.
//                    Тип объекта должен быть известен на момент установки обработчика сообщений.
//  возвращает: nbS_OK, nbS_CANCEL
#define nbEVENT_ENUMERATE_ITEM                      (0x00000001)  //Объект перечисления

//Событие: сообщение графическому окну (параметры зависят от ОС и платформы реализации)
//  void* paramA  - (выходной)  указатель на список параметров Callback 
//                              оконной функции обработки сообщений
//  void* paramB  - () зарезервирован
#define nbEVENT_WINDOW_MESSAGE                      (0x00000002)  //Обработка события

//.............................................................................
//  ПИК Обработчик событий
//.............................................................................

// Идентификатор интерфейса nbIEventHandler
// {585A9237-8D9D-4edb-AB24-8F41C9A22AC6}
static const nbUuid nbUUID_IEVENTHANDLER = 
  {0x585A9237,0x8D9D,0x4edb,{0xab,0x24,0x8f,0x41,0xc9,0xa2,0x2a,0xc6}};

#undef  nbINTERFACE
#define nbINTERFACE nbIEventHandler

nbDECLARE_INTERFACE(nbIEventHandler, nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //Прием события
  //  evt     - [вх]      Событие
  //  sender  - [вх]      Источник события
  //  paramA  - [вх/вых]  Параметр A (обычно входной)
  //  paramB  - [вх/вых]  Параметр B (обычно выходной)
  nbMETHOD(handleEvent)(nbTHISc nbEvent evt, nbHandle sender, const nbData paramA, nbData paramB) nbPURE;
};

#endif
