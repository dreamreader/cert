//////////////////////////////////////////////////////////////////////////
//файл:         nbIConfigurate.h
//версия:
//дата:
//изменен:
//автор:
//
//цель:       Интерфейс конфигурации компонента.
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_ICONFIGURATE_H
#define _NB_ICONFIGURATE_H

#include "nbTypes.h"
#include "nbIEventHandler.h"

//.............................................................................
//  ПИК Конфигурирование компонента
//.............................................................................

// Идентификатор интерфейса nbIConfigurate
// {D262F70D-062D-4a50-B7C4-3DFD47CBDEE4}
static const nbUuid nbUUID_ICONFIGURATE = 
  {0xd262f70d,0x62d,0x4a50,{0xb7,0xc4,0x3d,0xfd,0x47,0xcb,0xde,0xe4}};

#undef  nbINTERFACE
#define nbINTERFACE nbIConfigurate

nbDECLARE_INTERFACE(nbIConfigurate,nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //Загрузка текущей конфигурации
  //  page  - [вх] идентификатор страницы конфигурации
  //  opt   - [вх/вых] данные конфигурации
  nbMETHOD(setConfig)(nbUuid page, nbData opt) nbPURE;

  //Чтение текущей конфигурации
  //примечание: установленная конфигурация запоминается как последняя
  //  page  - [вх] идентификатор страницы конфигурации
  //  opt   - [вх] данные конфигурации
  nbMETHOD(getConfig)(nbUuid page, nbData opt) nbPURE;

  //Запрос конфигурации "по умолчанию"
  //  page  - [вх] идентификатор страницы конфигурации
  //  opt   - [вх/вых] данные конфигурации
  nbMETHOD(getDefaultConfig)(nbUuid page, nbData opt) nbPURE;

  //Настройка конфигурации
  //  handle- [вх] графическое окно, в котором отображается информация
  //  guihandler  - [вых] интерфейс обработчика сообщений графическому окну
  //  page  - [вх] идентификатор страницы конфигурации
  //  opt   - [опц/вых] данные конфигурации
  nbMETHOD(tuneUp)(nbHandle handle, nbUuid page, nbData opt, nbIEventHandler **guihandler) nbPURE;

  //Перечисление страниц конфигурации, которые можно настроить для указанного интерфейса
  //примечание: функция RaiseEvent вызывается с параметрами
  //                (nbUuid*)ParamA - идентификатор страницы конфигурации
  //                (nbData)ParamB  - строка с заголовком окна (опционально)
  //  eh  - обработчик события nbEVENT_ENUMERATE_ITEM
  //  iid - идентификатор интерфейса компонента (значение nbUUID_NIL перечисляет все страницы настроек компонента)
  nbMETHOD(enumPages)(nbIEventHandler *eh, nbRefUuid iid) nbPURE;

  //Получение описания страницы конфигурации
  //  page  - [вх] страница
  //  name  - [вых] заголовок страницы (может быть nbNULL)
  //  descr - [вых] описания страницы (может быть nbNULL)
  //примечание: если выделенной памяти недостаточно возвращается ошибка размера буфера
  nbMETHOD(getTitle)(nbUuid page, nbData name, nbData descr) nbPURE;
};

#endif
