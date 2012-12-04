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
//  примечания:
//    номера страниц образуют непрерывную последовательность
//    для указанного интерфейса возвращается число страниц и номер первой
//    для нулевого интерфейса возвращается общее число страниц
//.............................................................................

// Идентификатор интерфейса nbIConfigurate
// {D262F70D-062D-4a50-B7C4-3DFD47CBDEE5}
static const nbUuid nbUUID_ICONFIGURATE = 
  {0xd262f70d,0x62d,0x4a50,{0xb7,0xc4,0x3d,0xfd,0x47,0xcb,0xde,0xe5}};

#undef  nbINTERFACE
#define nbINTERFACE nbIConfigurate

nbDECLARE_INTERFACE(nbIConfigurate,nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //Загрузка текущей конфигурации
  //  page  - [вх] идентификатор страницы конфигурации
  //  opt   - [вх/вых] данные конфигурации
  nbMETHOD(setConfig)(int32_t page, nbData opt) nbPURE;

  //Чтение текущей конфигурации
  //примечание: установленная конфигурация запоминается как последняя
  //  page  - [вх] идентификатор страницы конфигурации
  //  opt   - [вх] данные конфигурации
  nbMETHOD(getConfig)(int32_t page, nbData opt) nbPURE;

  //Запрос конфигурации "по умолчанию"
  //  page  - [вх] идентификатор страницы конфигурации
  //  opt   - [вх/вых] данные конфигурации
  nbMETHOD(getDefaultConfig)(int32_t page, nbData opt) nbPURE;

  //Настройка конфигурации
  //  handle- [вх] графическое окно, в котором отображается информация
  //  page  - [вх] идентификатор страницы конфигурации
  //  opt   - [опц/вых] данные конфигурации
  nbMETHOD(tuneUp)(nbHandle handle, int32_t page, nbData opt) nbPURE;

  //Перечисление страниц конфигурации
  //  iid   - идентификатор интерфейса компонента (значение nbUUID_NIL возвращает общее число страниц)
  //  count - число страниц
  //  start - номер первой страницы для интерфейса
  nbMETHOD(getPageCount)(nbUuid *iid, int32_t *count, int32_t *start) nbPURE;

  //Получение описания страницы конфигурации
  //  page  - [вх] страница
  //  name  - [вых] заголовок страницы (может быть nbNULL)(не больше 128 байт)
  //  descr - [вых] описания страницы (может быть nbNULL)(не больше 1024 байт)
  //примечание: если выделенной памяти недостаточно возвращается ошибка размера буфера
  nbMETHOD(getTitle)(int32_t page, nbData name, nbData descr) nbPURE;
};

#endif
