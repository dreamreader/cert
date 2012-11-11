//////////////////////////////////////////////////////////////////////////
//файл:         FingerProcessor.h
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Обработчик отпечатка пальца
//////////////////////////////////////////////////////////////////////////

#ifndef _FINGER_PROCESSOR_H
#define _FINGER_PROCESSOR_H

#include <qatomic.h>

#include "nb/nb.h"
#include "nbclasses/nbcTypes.h"

//////////////////////////////////////////////////////////////////////////
// Идентификатор компонента обработки паппилярного рисунка отпечатка пальца
//  {6B2E0B0B-FF32-4DB7-B22F-2E60F6D3C68C}
static const nbUuid nbUUID_FINGER_PROCESSOR = 
  {0x6b2e0b0b,0xff32,0x4db7,{0xb2,0x2f,0x2e,0x60,0xf6,0xd3,0xc6,0x8c} };


//////////////////////////////////////////////////////////////////////////
// Компонент обработки паппилярного рисунка отпечатка пальца
class FingerProcessor: public nbIBimProcessor, nbIConfigurate {
public:
	// Конструктор
  FingerProcessor();
	// Деструктор
	~FingerProcessor();

  //Значение глобального счетчика компонентов
  static nbResult getCoRefs();

  ///////////////////////nbIUnknown//////////////////////////////////////

  //Запрос описателя интерфейса компонента
  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd);
  //Увеличение счетчика ссылок на интерфейс (в случае дублирования ссылки на него)
  nbMETHOD(retain)(nbTHIS);
  //Освобождение интерфейса, путем уменьшения счетчика ссылок на него
  nbMETHOD(release)(nbTHIS);

  ///////////////////////nbIBimProcessor///////////////////////////////////

  //Установка обработчика событий от обработчика образов
  nbMETHOD(setEventHandler)(nbTHISc nbIEventHandler *handler);
  //Обработка базы (вектора) биометрических образов
  nbMETHOD(process)(nbTHISc nbMatrix **baseAny, const nbBim **bimBase, uint32_t bimCount, nbMatrix **ind);

  //Запрос шаблона обработки биометрического образа заданного типа
  nbMETHOD(queryProcessRules)(nbTHISc nbMatrix **meta, nbRefUuid bimType);


  //Чтение базы "Все чужие" или "Свой", представленной в виде матрицы ВБП
  nbMETHOD(queryBaseAny)(nbTHISc nbMatrix **baseAny, uint32_t maxCount, nbRefUuid bimType, int32_t userId);
  //Освобождение памяти из-под базы "Все Чужие" или базы, полученной во время обработки
  nbMETHOD(releaseBase)(nbTHISc nbMatrix *baseAny);

  //Получение шаблонов биометрических образов
  nbMETHOD(createTemplate)(nbTHISc nbMatrix **baseOwn, nbMatrix **pattern, const nbBim **bimBase, uint32_t bimCount);

  //Установка сформированного  ранее шаблона обработки образов
  nbMETHOD(setTemplate)(nbTHISc const nbMatrix *pattern, nbRefUuid bimType);

  ///////////////////////nbIConfigurate///////////////////////////////////

  //Загрузка текущей конфигурации
  nbMETHOD(setConfig)(nbUuid page, nbData opt);
  //Чтение текущей конфигурации
  nbMETHOD(getConfig)(nbUuid page, nbData opt);
  //Запрос конфигурации "по умолчанию"
  nbMETHOD(getDefaultConfig)(nbUuid page, nbData opt);
  //Настройка конфигурации
  nbMETHOD(tuneUp)(nbHandle handle, nbUuid page, nbData opt, nbIEventHandler **guihandler);
  //Перечисление страниц конфигурации, которые можно настроить для указанного интерфейса
  nbMETHOD(enumPages)(nbIEventHandler *eh, nbRefUuid iid);
  //Получение описания страницы конфигурации
  nbMETHOD(getTitle)(nbUuid page, nbData name, nbData descr);

private:

  nbHandle          _module;
  QAtomicInt        _iRefs; // счетчик ссылок на интерфейсы компонента
  static QAtomicInt _cRefs; // счетчик ссылок на компоненты
  NbMatrix          _pattern;
};

#endif	// FINGER_PROCESSOR_H
