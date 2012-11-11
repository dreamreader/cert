/****************************************************************************
** файл:        expTypes.cpp
** версия:      0.0
** дата:        04.07.12
** изменен:     04.07.12
** автор:       Захаров Олег
**
** описание:    Работа с типами nbData и nbMatrix.
****************************************************************************/

#include "expTypes.h"

//////////////////////////////////////////////////////////////////////////
// Выделить память под nbData
bool createNbc(nbData *nbc, uint32_t size) {
  uint8_t* p = new uint8_t[sizeof(uint32_t) + size + sizeof(uint16_t)];
  if (!p) return false;
  memset(p,0,sizeof(uint32_t)+size+sizeof(uint16_t));
  // Записать размер и установить указатель
  *(uint32_t*)p = size;
  *nbc = (nbData)(p + sizeof(uint32_t));
  return true;
}

//////////////////////////////////////////////////////////////////////////
// Освободить nbData
bool freeNbc(nbData nbc) {
  if (nbc) {
    uint8_t *p = (uint8_t*)nbc - sizeof(uint32_t);
    delete [] p;
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////
//Длина элемента ВБП в битах
uint32_t sizeOfItem(nbMeta &meta) {
  //Таблица, позволяющая выполнить переход от размера элемента в бита к размеру
  static const uint8_t sizes[16*8] = {
    //0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    0, 1, 2, 4, 8,16,32,64, 0, 0, 0, 0, 0, 0, 0, 0, // 0
    0, 1, 2, 4, 8,16,32,64, 0, 0, 0, 0, 0, 0, 0, 0, // 16
    0, 0, 0, 0, 0,16,32,64,80, 0, 0, 0, 0, 0, 0, 0, // 32
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 96
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 112
  };
  return sizes[meta.format&0x7F];
}
//Длина ВБП в байтах с выравниванием по байту
uint32_t metaSize(nbMeta &meta) {
  uint32_t bits = sizeOfItem(meta)*meta.count;
  return ((bits>>3) + (0 != (bits & 7)));
}
//Получить размер матрицы в байтах
uint32_t getMatrixSize(nbMatrix *m) {
  uint32_t size = 0;

  // размер заголовка матрицы ВБП
  size += sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t); // nrows + ncols + *meta + **data

  // размер метаописания матрицы ВБП
  if (m->nrows) {
    for (uint32_t row = 0; row < m->nrows; row++) {
      size += sizeof(nbMeta);
    }
  }

  // размер данных матрицы ВБП
  if (m->nrows && m->ncols) {
    for (uint32_t row = 0; row < m->nrows; row++) {
      for (uint32_t col = 0; col < m->ncols; col++) {
        size += metaSize(m->meta[row]);
      }
    }
  }

  return size;
}
//
Vbp& nbmatrix_at(const nbMatrix &nbm, uint32_t irow, uint32_t icol) {
  return (Vbp&)nbm.data[irow][icol];
}

//////////////////////////////////////////////////////////////////////////
// Запись nbMatrix в nbData
bool matrixToData(nbMatrix *m, nbData *data) {
  if (!m)
    return false;

  uint32_t nrowsBuff    = m->nrows;
  uint32_t ncolsBuff    = m->ncols;
  uint32_t metaPointer  = 0;
  uint32_t dataPointer  = 0;

  uint32_t size = getMatrixSize(m);

  if (!createNbc(data, size)) {
    return false;
  }
  
  uint32_t shift = 0;

  // Заголовок матрицы ВБП
  memcpy(*data + shift, &nrowsBuff,   sizeof(uint32_t));   // число частей
  shift += sizeof(uint32_t);
  memcpy(*data + shift, &ncolsBuff,   sizeof(uint32_t));   // число примеров
  shift += sizeof(uint32_t);  
  memcpy(*data + shift, &metaPointer, sizeof(uint32_t));   // смещение метаописания
  shift += sizeof(uint32_t);
  memcpy(*data + shift, &dataPointer, sizeof(uint32_t));   // смещение данных
  shift += sizeof(uint32_t);

  // Метаописание матрицы ВБП
  if (m->nrows) {
    for (uint32_t row = 0; row < m->nrows; row++) {
      memcpy(*data + shift, &m->meta[row], sizeof(nbMeta));
      shift += sizeof(nbMeta);
    }
  }

  // Данные матрицы ВБП
  if (m->nrows && m->ncols) {
    for (uint32_t row = 0; row < m->nrows; row++) {
      for (uint32_t col = 0; col < m->ncols; col++) {
        memcpy(*data + shift, nbmatrix_at(*m,row,col), metaSize(m->meta[row]));
        shift += metaSize(m->meta[row]);
      }
    }
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////
// Запись nbData в nbMatrix
bool dataToMatrix(nbData data, nbMatrix **m) {
  if (!m)
    return false;

  uint32_t nrows = 0;
  uint32_t ncols = 0;
  uint32_t metaPointer = 0;
  uint32_t dataPointer = 0;
  uint32_t shift = 0;

  // Заголовок матрицы ВБП
  memcpy(&nrows, data + shift, sizeof(uint32_t));         // число частей
  shift += sizeof(uint32_t);
  memcpy(&ncols, data + shift, sizeof(uint32_t));         // число примеров
  shift += sizeof(uint32_t);
  memcpy(&metaPointer, data + shift, sizeof(uint32_t));   // смещение метаописания
  shift += sizeof(uint32_t);
  memcpy(&dataPointer, data + shift, sizeof(uint32_t));   // смещение данных
  shift += sizeof(uint32_t);

  // Метаописание матрицы ВБП
  NbMeta *meta = new NbMeta[nrows];
  if (nrows) {
    for (uint32_t row = 0; row < nrows; row++) {
      memcpy(&meta[row], data + shift, sizeof(nbMeta));
      shift += sizeof(nbMeta);
    }
  }

  NbMatrix *base = new NbMatrix;
  base->resizeMeta(nrows, meta );
  base->resizeData(ncols);

  // Данные матрицы ВБП
  if (nrows && ncols) {
    for (uint32_t row = 0; row < nrows; row++) {
      for (uint32_t col = 0; col < ncols; col++) {
        memcpy(base->at(row,col), data + shift, metaSize(meta[row]));
        shift += metaSize(meta[row]);
      }
    }
  }

  *m = base;

  delete [] meta;

  return true;
}
