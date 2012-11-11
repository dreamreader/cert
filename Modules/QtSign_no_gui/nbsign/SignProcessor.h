/****************************************************************************
** файл:         SignProcessor.h
** версия:
** дата:
** изменен:
** автор:
**
** описание:     Обработчик рукописных образов.
****************************************************************************/

#ifndef _SIGN_PROCESSOR_H
#define _SIGN_PROCESSOR_H

#include <qatomic.h>

#include "nb\nb.h"
#include "nbclasses\nbcTypes.h"

//////////////////////////////////////////////////////////////////////////
// Идентификатор компонента ввода рукописных образов
// {A9EBF452-CF25-408c-8647-5E3A1D57FF4E}
static const nbUuid nbUUID_SIGN_PROVIDER =
  {0xa9ebf452,0xcf25,0x408c,{0x86,0x47,0x5e,0x3a,0x1d,0x57,0xff,0x4e}};

//////////////////////////////////////////////////////////////////////////
// Идентификатор компонента обработки рукописных образов
// {702CBB42-7367-48cf-BED1-D78D1774BA52}
static const nbUuid nbUUID_SIGN_PROCESSOR =
  {0x702cbb42,0x7367,0x48cf,{0xbe,0xd1,0xd7,0x8d,0x17,0x74,0xba,0x52}};

//////////////////////////////////////////////////////////////////////////
// Компонент обработки рукописных образов
class SignProcessor: public nbIBimProcessor, nbIConfigurate {
public:
  // Конструктор
  SignProcessor();
  // Деструктор
  ~SignProcessor();

  // Значение глобального счетчика компонентов
  static nbResult GetCoRefs();

  ///////////////////////nbIUnknown//////////////////////////////////////

  // Запрос описателя интерфейса компонента
  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd);
  // Увеличение счетчика ссылок на интерфейс (в случае дублирования ссылки на него)
  nbMETHOD(retain)(nbTHIS);
  // Освобождение интерфейса, путем уменьшения счетчика ссылок на него
  nbMETHOD(release)(nbTHIS);

  ///////////////////////nbIBimProcessor///////////////////////////////////

  // Установка обработчика событий от обработчика образов
  nbMETHOD(setEventHandler)(nbTHISc nbIEventHandler *handler);
  // Обработка базы (вектора) биометрических образов
  nbMETHOD(process)(nbTHISc nbMatrix **any, const nbBim **bimBase, uint32_t bimCount, nbMatrix **ind);
  // Получение шаблонов биометрических образов
  nbMETHOD(createTemplate)(nbTHISc nbMatrix **own, nbMatrix **pattern, const nbBim **bimBase, uint32_t bimCount);
  // Установка сформированного  ранее шаблона
  nbMETHOD(setTemplate)(nbTHISc const nbMatrix *pattern, nbRefUuid bimType);
  // Запрос шаблона обработки биометрического образа заданного типа
  nbMETHOD(queryProcessRules)(nbTHISc nbMatrix **pattern, nbRefUuid bimType);
  // Чтение базы "Все чужие" или "Свой", представленной в виде матрицы ВБП
  nbMETHOD(queryBaseAny)(nbTHISc nbMatrix **any, uint32_t maxCount, nbRefUuid bimType, int32_t userId);
  // Освобождение памяти из-под базы "Все Чужие" или базы, полученной во время обработки
  nbMETHOD(releaseBase)(nbTHISc nbMatrix *any);

  ///////////////////////nbIConfigurate///////////////////////////////////

  // Загрузка текущей конфигурации
  nbMETHOD(setConfig)(nbUuid page, nbData opt);
  // Чтение текущей конфигурации
  nbMETHOD(getConfig)(nbUuid page, nbData opt);
  // Запрос конфигурации "по умолчанию"
  nbMETHOD(getDefaultConfig)(nbUuid page, nbData opt);
  // Настройка конфигурации
  nbMETHOD(tuneUp)(nbHandle handle, nbUuid page, nbData opt, nbIEventHandler **guihandler);
  // Перечисление страниц конфигурации, которые можно настроить для указанного интерфейса
  nbMETHOD(enumPages)(nbIEventHandler *eh, nbRefUuid iid);
  //Получение описания страницы конфигурации
  nbMETHOD(getTitle)(nbUuid page, nbData name, nbData descr);

private:
  QAtomicInt        _iRefs;         // счетчик ссылок на интерфейсы компонента
  static QAtomicInt _cRefs;         // счетчик ссылок на компоненты

};

#endif  // _SIGN_PROCESSOR_H
