//FPDifine.h: глобальные константы для отпечатка пальцев

#ifndef FP_DEFINE_H
#define FP_DEFINE_H

#define _USE_MATH_DEFINES
#include <math.h>

#undef    F_PI
#define   F_PI  ((float)M_PI)

#define   F_PI_180  (F_PI/180.0f)
#define	  F_EPS			0.00000000000000022204f	// - величина -> 0

//-----------------------------------------------------------------------------

#define		fpWt		  320	// ширина изображения (число точек)
#define		fpHt			480	// высота изображения (число точек)
#define   fpHWt     (fpWt*fpHt) // размер изображения (число точек)

#define		fpBlkNum16 16	// размер блока поля ориентации
#define   fpBlkSize  40  // размер блока изображения


#define   fpHBlk16  (fpHt/fpBlkNum16)
#define   fpWBlk16  (fpWt/fpBlkNum16)
#define   fpHWBlk16 (fpHBlk16*fpWBlk16) // число блоков в изображении

#define   fpCpNum 256     // число непрерывных параметров (коэффициентов Уолша-Адамара)
#define   fpDpNum (fpHWt/(fpBlkSize*fpBlkSize)) // число дискретных параметров

#define   fpOverlap   40  // размер области перекрытия
#define   fpMtsNum    300 // максимальное число обнаруживаемых минуций
#define   fpResThr    0.09 // максимальное число обнаруживаемых минуций

//-----------------------------------------------------------------------------

#define		fpBlkNum12  12	                            // размер блока 12
#define		fpOverlap6  6	                              // размер перекрытия 6
#define		fpHBlk12   (uint32_t)((fpHt-2*fpOverlap6)/fpBlkNum12)	//39
#define		fpWBlk12	  (uint32_t)((fpWt-2*fpOverlap6)/fpBlkNum12)	//25
#define		fpBlk12R(r) (uint32_t)((r-fpOverlap6)/fpBlkNum12)
#define		fpBlk12C(c) (uint32_t)((c-fpOverlap6)/fpBlkNum12)

// Перевод из радиан в угловую меру
#define RadianToAngle(r) (r/0.01745)
// Перевод в радианую меру
#define AngleToRadian(r) (r*0.01745)

#endif //FP_DEFINE_H
