//FPProcess.h Обработчик биометрического образа в 2 вектора параметров (непрерывного и дискретного)

#ifndef _FP_PROCESS_H
#define _FP_PROCESS_H

#include "../nb/nb.h"
#include "CFPMinutiae.h"

//Преобразование изображения отпечатка пальца в 2 вектора параметров:
//непрерывный и дискретный
//  img    - [вх] 256-цветное серое изображение nWt*nHt [320 * 480]
//  сparam - [вых] вектор непрерывных параметров [416]
//  dparam - [вых] вектор дискретных параметров [96], принимающих 1 и 0
//Возвращает: признак успешной обработки изображения
bool fp_process(int img[], FPPoint minutiae[], int &numberMinutiae, float cparam[], int dparam[]);

#endif
