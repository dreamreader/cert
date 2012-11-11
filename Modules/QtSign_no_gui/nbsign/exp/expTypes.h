/****************************************************************************
** ����:        expTypes.h
** ������:      0.0
** ����:        04.07.12
** �������:     04.07.12
** �����:       ������� ����
**
** ��������:    ������ � ������ nbData � nbMatrix.
****************************************************************************/

#ifndef _EXPTYPES_H
#define _EXPTYPES_H

#include "..\nbclasses\nbcTypes.h"

bool createNbc(nbData *nbc, uint32_t size);
bool freeNbc(nbData nbc);

bool matrixToData(nbMatrix *m, nbData *data);
bool dataToMatrix(nbData data, nbMatrix **m);

#endif
