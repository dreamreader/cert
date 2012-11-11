/****************************************************************************
** файл:         SignProcessor.cpp
** версия:
** дата:
** изменен:
** автор:
**
** описание:     Обработчик рукописных образов.
****************************************************************************/

#include <qresource.h>
#include <QFile>
#include <QTextStream>
#include "hw\psnConversionSFN.h"
#include "hw\BimHw.h"
#include "SignProcessor.h"
#include "psnReadWrite.h"

#pragma warning(disable:4100) // игнорировать ошибку unreferenced formal parameter

QAtomicInt SignProcessor::_cRefs(0); // счетчик ссылок на компоненты

// Конструктор
SignProcessor::SignProcessor() {
  _cRefs.ref();
}

// Деструктор
SignProcessor::~SignProcessor() {
  _cRefs.deref();
}

// Значение глобального счетчика компонентов
nbResult SignProcessor::GetCoRefs() {
  return (nbResult)_cRefs;
}

///////////////////////nbIUnknown//////////////////////////////////////

// Запрос описателя интерфейса компонента
nbMETHOD_DEF(SignProcessor,queryInterface)(nbRefUuid iid, void **icd) {
  if ( iid == nbUUID_IUNKNOWN )
    *icd = static_cast<nbIBimProcessor*>(this);
  else if ( iid == nbUUID_IBIM_PROCESSOR )
    *icd = static_cast<nbIBimProcessor*>(this);
  else if ( iid == nbUUID_ICONFIGURATE )
    *icd = static_cast<nbIConfigurate*>(this);
  else {
    *icd = nbNULL;
    return nbE_NO_INTERFACE;
  }
  static_cast<nbIUnknown*>(*icd)->retain();
  return nbS_OK;
}

// Увеличение счетчика ссылок на интерфейс (в случае дублирования ссылки на него)
nbMETHOD_DEF(SignProcessor,retain)(nbTHIS) {
  return (nbResult)_iRefs.ref();
}

// Освобождение интерфейса, путем уменьшения счетчика ссылок на него
nbMETHOD_DEF(SignProcessor,release)(nbTHIS) {
  if (!_iRefs.deref()){
    delete this;
    return nbS_OK;
  }
  return _iRefs;
}

///////////////////////nbIBimProcessor///////////////////////////////////

// Установка обработчика событий от обработчика образов
nbMETHOD_DEF(SignProcessor,setEventHandler)(nbTHISc nbIEventHandler *handler) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// Обработка базы (вектора) биометрических образов
nbMETHOD_DEF(SignProcessor,process)(nbTHISc nbMatrix **any, const nbBim **bimBase, uint32_t bimCount, nbMatrix **ind) {
  nbResult rslt = nbS_OK;
  // Проверить входные параметры
/*qDebug() << "step 1. bimCount = " << QString::number(bimCount);*/
  if ( !any || !bimBase || bimCount == 0) return nbE_PARAM;
  for (uint32_t i = 0; i < bimCount; ++i){
    if (bimBase[i]->bimType != nbBIM_TYPE_HANDWR) return nbE_IDENTIFIER;
  }

  // Выделить память под матрицу векторов биометрических параметров
  rslt = queryProcessRules(any,nbBIM_TYPE_HANDWR);
  if (nbFAILED(rslt)) return rslt;
  NbMatrix *m = *(NbMatrix**)(any);
  m->resizeData(bimCount);

  // Вычислить функционалы
  rslt = NcCalcImagesCoef( bimBase, *m );

//////////////////////////////////////////////////////////////////////////
// ВРЕМЕННО
/*
  // Сохранить коэффициенты в лог-файл
  nbMatrix *vbps = *baseAny;
  float *ptr;
  QFile file("coeffs_log.txt");
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    for ( uint32_t i = 0; i < vbps->ncols; i++ ) {
      ptr = (float*)vbps->data[0][i];
      for ( uint32_t j = 0; j < vbps->meta->count; j++ ) {
        out << ptr[j] << "\n";
      }
    }
    out << "\n";
  }
  file.close();
*/
// ВРЕМЕННО
//////////////////////////////////////////////////////////////////////////

  return rslt;
}
// Получение шаблонов биометрических образов
nbMETHOD_DEF(SignProcessor, createTemplate)(nbTHISc nbMatrix **baseOwn, nbMatrix **pattern, const nbBim **bimBase, uint32_t bimCount) {
  *pattern = nbNULL;
  return process(baseOwn, bimBase, bimCount, nbNULL);
}

// Установка сформированного  ранее шаблона
nbMETHOD_DEF(SignProcessor, setTemplate)(nbTHISc const nbMatrix *pattern, nbRefUuid bimType) {
  if ( pattern ) return nbS_SKIP;
  if ( bimType != nbBIM_TYPE_HANDWR ) return nbE_IDENTIFIER;
  return nbS_OK;
}

// Запрос шаблона обработки биометрического образа заданного типа
nbMETHOD_DEF(SignProcessor,queryProcessRules)(nbTHISc nbMatrix **pattern, nbRefUuid bimType) {
  // Проверить входные параметры
  if ( !pattern ) return nbE_PARAM;
  if ( bimType != nbBIM_TYPE_HANDWR ) return nbE_IDENTIFIER;

  // Создать матрицу шаблона для выдаваемых параметров
  NbMeta meta(psnFFT_COEF_COUNT + psnP_COEF_COUNT, nbMF_R32, nbMT_CONTINUOUS);
  *pattern = new NbMatrix(1,0,&meta);
  if (!*pattern) return nbE_OUT_OF_MEMORY;

  return nbS_OK;
}

//////////////////////////////////////////////////////////////////////////
// Чтение базы "Все чужие" или "Свой", представленной в виде матрицы ВБП
nbMETHOD_DEF(SignProcessor,queryBaseAny)(nbTHISc nbMatrix **any, uint32_t maxCount, nbRefUuid bimType, int32_t userId) {
  // Проверить тип биометрического образа и пользователя
  if (!any || maxCount == 0) return nbE_PARAM;
  if (bimType != nbBIM_TYPE_HANDWR) return nbE_IDENTIFIER;
  if (userId != 0) return nbE_PARAM;

  nbResult rslt = nbS_OK;

// ВАРИАНТ 1. Считывание базы координат и вычисление матрицы ВБП
/*
  QResource resBimBase(":/base/simgbase");

  // Считать "зашитую" базу "Сжатая база Все Чужие"
  rslt = GetBimCount(resBimBase.data(),&maxCount);
  if (nbFAILED(rslt)) {
    return rslt;
  }
  nbBim **bimBase = (nbBim**)new nbHandle[maxCount];
  if (!bimBase) return nbE_OUT_OF_MEMORY;
  
  rslt = LoadBimFromResource(resBimBase.data(),&maxCount, bimBase);
  if (nbSUCCEEDED(rslt)){
    rslt = Process(baseAny, (const nbBim**)bimBase, maxCount);
    // Освободить память, выделенную под образы
    for (uint32_t i = 0; i < maxCount; ++i) {
      BimHw img;
      img.attach(bimBase[i]);
      img.free();
    }
  }
  delete [] bimBase;
*/

// ВАРИАНТ 2. Считывание матрицы ВБП
  QResource resBase(":/base/sprmbase"); // база "Сжатая база Все Чужие"
  rslt = LoadMatrixFromResource(resBase.data(),any);

  return rslt;
}

// Освобождение памяти из-под базы "Все Чужие" или базы, полученной во время обработки
nbMETHOD_DEF(SignProcessor,releaseBase)(nbTHISc nbMatrix *any) {
  if (any) {
    delete (NbMatrix*)any;
  }
  return nbS_OK;
}

///////////////////////nbIConfigurate///////////////////////////////////

// Загрузка текущей конфигурации
nbMETHOD_DEF(SignProcessor,setConfig)(nbUuid page, nbData opt) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// Чтение текущей конфигурации
nbMETHOD_DEF(SignProcessor,getConfig)(nbUuid page, nbData opt) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// Запрос конфигурации "по умолчанию"
nbMETHOD_DEF(SignProcessor,getDefaultConfig)(nbUuid page, nbData opt) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// Настройка конфигурации
nbMETHOD_DEF(SignProcessor,tuneUp)(nbHandle handle, nbUuid page, nbData opt, nbIEventHandler **guihandler) {
  return nbE_UNSUPPORTED_FUNCTION;
}

// Перечисление страниц конфигурации, которые можно настроить для указанного интерфейса
nbMETHOD_DEF(SignProcessor,enumPages)(nbIEventHandler *eh, nbRefUuid iid) {
  return nbS_OK;
}

// Получение описания страницы конфигурации
nbMETHOD_DEF(SignProcessor,getTitle)(nbUuid page, nbData name, nbData descr) {
  return nbS_OK;
}
