//////////////////////////////////////////////////////////////////////////
//файл:         nbIComponentDispatcher.h
//версия:
//дата:
//изменен:
//автор:
//
//цель:         Описание ПИК Диспетчер компонентов
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_ICOMPONENT_DISPATCHER_H
#define _NB_ICOMPONENT_DISPATCHER_H

#include "nbTypes.h"
#include "nbIEventHandler.h"

#pragma pack(1)

//.............................................................................
//  ПИК Диспетчер компонентов
//.............................................................................

// Идентификатор интерфейса nbIComponentDispatcher
// {6D5A8E41-158A-4718-A090-DDF4E49BC8B6}
static const nbUuid nbUUID_ICOMPONENT_DISPATCHER = 
  {0x6D5A8E41,0x158a,0x4718,{0xa0,0x90,0xdd,0xf4,0xe4,0x9b,0xc8,0xb6}};

#undef  nbINTERFACE
#define nbINTERFACE   nbIComponentDispatcher

nbDECLARE_INTERFACE(nbIComponentDispatcher,nbIUnknown) {

  //Запрос описателя интерфейса компонента
  //  iid     - [вх]    Ссылка на идентификатор интерфейса
  //  icd     - [вых]   Указатель на описатель интерфейса компонента
  //возвращает: nbS_OK
  //            nbE_NO_INTERFACE
  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;

  //Увеличение счетчика ссылок на интерфейс (в случае дублирования ссылки на него)
  //возвращает: положительное значение
  nbMETHOD(retain)(nbTHIS) nbPURE;

  //Освобождение интерфейса, путем уменьшения счетчика ссылок на него
  //возвращает: неотрицательное значение  
  nbMETHOD(release)(nbTHIS) nbPURE;

  //Регистрация компонента в системе
  //параметры:
  //  cid     - [вх] Идентификатор компонента
  //  coInfo  - [вх] Описание компонента, включая путь к модулю компонента
  //возвращает: nbS_OK
  //            nbE_OBJECT_ALREADY_EXISTS
  //            nbE_PARAM
  //            nbE_ACCESS_DENIED
  //            nbE_OBJECT_NOT_FOUND
  nbMETHOD(registerComponent)(nbTHISc nbRefUuid cid, const nbData coInfo, const nbData key) nbPURE;

  //Дерегистрация компонента в системе
  //параметры:
  //  cid   - [вх] Идентификатор компонента
  //возвращает: nbS_OK
  //            nbE_PARAM
  //            nbE_NO_COMPONENT
  //            nbE_ACCESS_DENIED
  //            nbE_OBJECT_NOT_FOUND
  nbMETHOD(unregisterComponent)(nbTHISc nbRefUuid cid, const nbData key) nbPURE;
  
  //Запрос на загрузку и создание компонента
  //параметры:
  //  сid   - [вх] Идентификатор компонента
  //  iid   - [вх] Идентификатор интерфейса
  //  icd   - [вых] Описатель ПИК
  //возвращает: nbS_OK
  //            nbE_PARAM
  //            nbE_NO_MODULE
  //            nbE_NO_COMPONENT
  //            nbE_NO_INTERFACE
  //            nbE_INTEGRITY_FAIL
  nbMETHOD(queryComponent)(nbTHISc nbRefUuid cid, nbRefUuid iid, void **icd) nbPURE;
  
  //Запрос информации о компоненте
  //  cid     - [вх]  Идентификатор компонента
  //  coInfo  - [вых] Указатель на заполняемый информационный блок
  //возвращает: nbS_OK
  //            nbE_PARAM
  //            nbE_NO_COMPONENT
  //            nbE_INSUFFICIENT_BUFFER
  nbMETHOD(queryComponentInfo)(nbTHISc nbRefUuid cid, nbData *coInfo) nbPURE;

  //Перечисление компонентов с заданным интерфейсом, зарегистрированных в системе
  //описание:   Вызывает функцию RaiseEvent обработчика событий, передавая ему *cid в качестве paramA.
  //            Если iid==nbUUID_NULL перечисляются все зарегистрированные в системе компоненты.
  //            Значение обработчика nbS_OK продолжает перебор, значение nbS_CANCEL прекращает.  
  //  iid     - [вх] Запрашиваемый интерфейс компонента или nbUUID_NULL, если нужно перечислить все компоненты
  //  handler - [вх] Описатель ПИК Обработчик сообщений
  //возвращает: nbS_OK
  //            nbE_PARAM
  //            nbE_NO_COMPONENT
  //            nbE_CANCELLED
  nbMETHOD(enumComponents)(nbTHISc nbRefUuid iid, nbIEventHandler *eh) nbPURE;
  
  //Выделение памяти для блока данных
  nbMETHOD(allocData)(nbTHISc nbData *data, uint32_t size);
  //Освобождение блока данных, выделенного компонентом
  nbMETHOD(releaseData)(nbTHISc nbData data);
};

//.............................................................................
//  ПИМ
//.............................................................................

//Указатель на функцию 'NbEnumComponents'. Перечисляет информацию о всех
//имеющихся в модуле компонентах и поддерживаемых ими интерфейсах.
//примечание:
//  В функцию RaiseEvent в качестве параметров передаются
//    (const nbUuid*) paramA  - идентификатор компонента
//    (nbData)        paramB  - блок данных в формате xml (UTF-8) с информацией о компоненте
//  key - [вх] регистрационный ключ (используется для защиты от несанкционированного удаления)
//возвращает: nbS_OK
typedef nbResult (nbCALL *NbEnumComponentsPtr)(nbIEventHandler *eh);

//Проверка возможности выгрузки модуля
//возвращает: nbS_OK, если модуль не используется и его можно выгрузить
//            другое значение успеха, как счетчик блокировок модуля
//            (допустимо в качестве другого значения всегда возвращать 1)
typedef nbResult (nbCALL *NbCanUnloadPtr)();

#pragma pack()

#endif
