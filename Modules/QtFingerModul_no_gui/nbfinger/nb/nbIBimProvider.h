///////////////////////////////////////////////////////////////////////////////
//файл:         nbIBimProvider.h
//версия:       
//дата:         
//изменен:
//автор:
//
//цель:         Описание ПИК Поставщик биометрических образов nbBimProvider,
//              типа nbPurpose, типа nbBim, специальных событий,
//              константы nbUUID_BIM_PROVIDER
///////////////////////////////////////////////////////////////////////////////
#ifndef _NB_IBIMPROVIDER_H
#define _NB_IBIMPROVIDER_H

#include "nbTypes.h"
#include "nbIEventHandler.h"

#pragma pack(1)

//.............................................................................

//Биометрический образ
//  реализация структуры зависит от технологии, 
//  но первые 2 поля всегда одни и те же.
typedef struct nbBim_t {
  uint32_t  size;                     // Общая длина (в байтах) биометрического образа
                                      // (без учета длины заголовка nbBim)
  nbUuid    bimType;                  // Тип биометрического образа
  //  данные
} nbBim;

//.............................................................................
//
//СПЕЦИФИКАЦИЯ ФАЙЛА, хранящего биометрические образы
//
// *.bim
//
//  signature sess_id puporse multipart (parttype (if0x0000)(provider_cid count bim[count]))[multipart]
//
//  signature        = "BIM" = 0x204d4942;
//  sess_id          = nbUuid идентификатор сессии
//  purpose          = uint8_t цель сбора образов
//  multipart        = uint8_t число частей в файле
//  parttype         = uint16_t тип части (0x0000) для биометрических образов
//    provider_cid     = nbUuid идентификатор провайдера
//    count            = uint32_t (число биометрических образов)
//    далее последовательно друг за другом идут "тела" биометрических образов
//    bim[count]
//    ...
//  parttype
//  ...
//.............................................................................

//Заголовок файла биометрических образов
typedef struct nbBimFileHeader_t {
  uint32_t  signature;  // сигнатура BIM 
  nbUuid    sess_id;    // идентификатор сесии, во время которой получены образы
  nbPurpose purpose;    // цель сбора образов
  uint8_t   multipart;  // число частей
  //подзаголовок каждой части
  uint16_t  parttype;   // тип части (равен 0x0000 для биометрического образа)
  nbUuid    providerCid;// идентификатор провайдера
  uint32_t  count;      // число наборов образов
} nbBimFileHeader;


typedef int32_t nbBimInputMode;

#define nbBIM_HIDDEN              (0x00000001)  // режим скрытого ввода образа
#define nbBIM_AUTO_CORRECTION     (0x00000002)  // с коррекцией вводимого образа (может задаваться устройством по умолчанию)
#define nbBIM_FILTER_BAD_QUALITY  (0x00000003)  // не пропускать образы с заведомо плохим качеством (сабботаж)

//.............................................................................

//  СОБЫТИЯ

//Событие: начало ввода образа
//  IBimProvider* sender
//  nbBim* paramA - (входной) указатель на область, в которую записывается образ
//  nbData paramB - (входной) nbNULL
//возвращает: nbS_OK, nbS_CANCEL
#define nbEVENT_BIM_STARTED             (0x00000100)

//Событие: биометрический образ модифицирован (изменился)
//  IBimProvider* sender
//  nbBim* paramA - (входной) указатель на область, в которую записывается образ
//  int32_t paramB- (входной) состояние прогресса для образа 0..max(int32_t)
//возвращает: nbS_OK, nbS_CANCEL
#define nbEVENT_BIM_PROGRESS            (0x00000101)

//Событие: конец ввода образа, образ введен полностью
//  IBimProvider* sender
//  nbBim* paramA - (входной) указатель на область, в которую записывается образ
//  nbData paramB - (входной) nbNULL
//возвращает: nbS_OK, nbS_CANCEL
#define nbEVENT_BIM_READY               (0x00000103)

//Событие: ввод образа прерван извне (или по времени)
//  IBimProvider* sender
//  nbBim* paramA - (входной) nbNULL
//  nbData paramB - (входной) nbNULL
//возвращает: nbS_OK, nbS_CANCEL
#define nbEVENT_BIM_CANCEL              (0x00000104)

//.............................................................................
//  ПИК Поставщик биометрических образов
//.............................................................................

// Идентификатор интерфейса nbIBimProvider
// {4077DC01-8AB9-4322-B37E-26C2A4A27A2C}
static const nbUuid nbUUID_IBIM_PROVIDER = 
  {0x4077DC01,0x8ab9,0x4322,{0xb3,0x7e,0x26,0xc2,0xa4,0xa2,0x7a,0x2c}};

#undef  nbINTERFACE
#define nbINTERFACE nbIBimProvider

nbDECLARE_INTERFACE(nbIBimProvider,nbIUnknown){

  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  nbMETHOD(retain)(nbTHIS) nbPURE;
  nbMETHOD(release)(nbTHIS) nbPURE;

  //Установка обработчика событий от биометрического устройства
  //  handler   - [вх] Описатель ПИК Обработчик сообщений
  //возвращает: nbS_OK или код ошибки
  nbMETHOD(setEventHandler)(nbTHISc nbIEventHandler *handler) nbPURE;
  
  //Присоединение устройства
  //  handle  - [вх] описатель устройства (0, по умолчанию; -1 для отключения)
  nbMETHOD(connectToDevice)(nbTHISc nbHandle handle) nbPURE;
  
  //Присоединение графического окна
  //  handle      - [вх] описатель графического окна (0, по умолчанию; -1, для отключения)
  //  guihandler  - [опц/вых] интерфейс обработчика сообщений графическому окну
  nbMETHOD(connectToWindow)(nbTHISc nbHandle handle, nbIEventHandler **guihandler) nbPURE;

  //Настройка биометрического устройства
  //параметры:
  //  evt     - [вх]      Команда конфигурирования
  //  paramA  - [вх/вых]  Первый параметр 
  //  paramB  - [вх/вых]  Второй параметр
  //возвращает: nbS_OK (Принято), nbS_CANCEL (Отклонено) или код ошибки
  nbMETHOD(control)(nbTHISc nbEvent evt, const nbData paramA, nbData paramB) nbPURE;

  //Сброс биометрического образа
  //примечание: устройство очищает текущий образ (и возможно скрытые)
  //            если образ захвачен, ввод начинается заново
  //возвращает: nbS_OK или код ошибки
  nbMETHOD(resetBim)(nbTHIS) nbPURE;

  //Установка биометрического образа
  //  bim   - [вх] биометрический образ (0, для сброса образа)
  nbMETHOD(setBim)(nbTHISc const nbBim *bim, nbPurpose purpose) nbPURE;

  //Установка режима ввода биометрических образов
  //  mode  - [вх/вых] флаги режима отображения. в mode записывается предыддущее значение режима
  //              (в случае ошибки режима, возвращаются номера флагов, операция для которых не поддерживается)
  //  mask  - [вх] побитовая маска для флагов, с которые будут устанавливаться/сбрасываться
  //возвращает: nbS_OK или код ошибки
  nbMETHOD(changeInputMode)(nbTHISc nbBimInputMode *mode, nbBimInputMode mask) nbPURE;
    
  //Монопольный захват биометрического устройства
  //примечание: функция должна возвращать управления, не ожидая завершения операций устройства
  //  bim       - [вых] Считываемый биометрический образ
  //                    (память выделяется устройством и может изменяться, пока не будет вызвана WaitForBimReady)
  //  bimType   - [вх]  Тип биометрического образа
  //  purpose   - [вх]  Цель получения биометрического образа
  //возвращает:  nbS_OK или код ошибки
  nbMETHOD(captureBim)(nbTHISc nbBim **bim, nbRefUuid bimType, nbPurpose purpose) nbPURE;

  //Ожидание завершения ввода образа (или мгновенная проверка его ввода)
  //  bim       - [вх]  Тот же указатель на считываемый биометрический образ, что был передан в функцию CaptureBim
  //  timeout   - [вх]  Время ожидания
  nbMETHOD(waitForBimReady)(nbTHISc nbBim **bim, nbTimeout timeout) nbPURE;
    
  //Освобождение выделенной под образ или образы памяти
  //  bim       - [вх] Указатель на биометрический образ, память под который была выделена компонентом
  //возвращает: nbS_OK или код ошибки
  nbMETHOD(releaseBim)(nbTHISc nbBim *bim) nbPURE;
};

#pragma pack()

#endif
