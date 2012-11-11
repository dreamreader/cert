/****************************************************************************
** файл:        psnReadWrite.cpp
** версия:
** дата:
** изменен:
** автор:       Захаров О.С.
**
** описание:    Поддерживает работу с файлами (чтение и запись).
****************************************************************************/

#include "nb\nbIBimProvider.h"
#include "nbclasses\nbcTypes.h"
#include "psnReadWrite.h"
#include "exp\expTypes.h"
#include "..\nbsign\SignProcessor.h"

// Заголовок файла с биометрическими параметрами
#define SIG_PARAMS 0x20504942 // const char paramSign[4]	= {'B','I','P',' '};

//////////////////////////////////////////////////////////////////////////
// Получить количество образов, хранящихся в ресурсах
nbResult GetBimCount ( const uint8_t data[], uint32_t *maxCount ) {
  if(!data)
    return nbE_POINTER;

  // Считать заголовок файла
  nbBimFileHeader *header = (nbBimFileHeader*)data; // указатель на заголовок набора образов
  if (header->count > *maxCount) {
    *maxCount = header->count;
    return nbE_INSUFFICIENT_BUFFER;
  }
  *maxCount = header->count;

  return nbS_OK;
}

//////////////////////////////////////////////////////////////////////////
//назначение: загрузить биометрические образы из файла ресурсов
//описание:   считать записанные в базу образы СВОЕГО или ЧУЖОГО.
//            На вход подаётся вектор указателей на nbBim размером maxCount.
//            Если maxCount меньше количества реально хранящихся в базе образов, 
//            то возвращает nbE_OUT_OF_MEMORY и в maxCount записываем реальное
//            количество образов.
//            Память под bimBase выделяется внутри функции.
//            Нужно очистить после вызова функции.
//параметры:
//  data     - [вх]     указатель на данные, хрянящиеся в файле ресурса
//  maxCount - [вх\вых] максимально возможное количество образов
//	bimBase	 - [вх\вых] биометрические образы
//возвращает: nbS_OK, если всё нормально, либо код ошибки
//
nbResult LoadBimFromResource ( const uint8_t data[], uint32_t *maxCount, nbBim **bimBase ) {
  if (!bimBase || !data)
    return nbE_POINTER;

  // Считать заголовок файла
  nbBimFileHeader *header = (nbBimFileHeader*)data;         // указатель на заголовок набора образов
  nbBim* bimOne = (nbBim*)(data + sizeof(nbBimFileHeader)); // указатель на данные первого образа
  if (header->count > *maxCount) {
    *maxCount = header->count;
    return nbE_INSUFFICIENT_BUFFER;
  }
  *maxCount = header->count;

  // Считать последовательно все образы, выделяя под них память
  for (uint32_t i=0; i<header->count; ++i) {
    uint32_t size = sizeof(nbBim) + bimOne->size;  // полный размер образа
    bimBase[i] = (nbBim*)new uint8_t[size];
    if (!bimBase[i]) return nbE_OUT_OF_MEMORY;     // !по хорошему нужно освободить ранее выделенную память
    memcpy(bimBase[i],bimOne,size);                // скопировать данные
    bimOne = (nbBim*)((uint8_t*)bimOne + size);    // перейти к следующему образу
  }

  return nbS_OK;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//назначение: загрузить матрицу векторов биометрических параметров из файла ресурсов
//описание:   считать записанные в базу параметры образов СВОЕГО или ЧУЖОГО.
//            На вход подаётся пустая матрица векторов биом. параметров.
//            Память под vbpsBase выделяется внутри функции.
//            Нужно очистить после вызова функции.
//параметры:
//  data      - [вх]  указатель на данные, хрянящиеся в файле ресурса
//	vbpsBase  - [вых] матрица векторов биометрических параметров
//возвращает: nbS_OK, если всё нормально, либо код ошибки
nbResult LoadMatrixFromResource ( const uint8_t data[], nbMatrix **vbpsBase ) {
  if (!vbpsBase)
    return nbE_POINTER;

  // Считать заголовок файла
  nbMatrixFileHeader *header = (nbMatrixFileHeader*)data;         // указатель на заголовок набора образов
  if ( header->signature != SIG_PARAMS &&
       header->bimType != nbBIM_TYPE_HANDWR &&
       header->providerCid != nbUUID_SIGN_PROVIDER &&
       header->processorCid != nbUUID_SIGN_PROCESSOR) {
    return nbE_IDENTIFIER;
  }
  // Считать заголовок блока
  //nbBlockHeader *blHeader = (nbBlockHeader*)(data + sizeof(nbMatrixFileHeader));

  // Считать данные
  nbData buffer = (nbData)(data + sizeof(nbMatrixFileHeader) + sizeof(nbBlockHeader));

  // Преобразовать данные в матрицу ВБП
  if (!dataToMatrix(buffer,vbpsBase)) {
    return nbE_FAIL;
  }

  return nbS_OK;
}

