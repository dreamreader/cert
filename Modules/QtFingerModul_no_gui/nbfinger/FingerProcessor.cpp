//////////////////////////////////////////////////////////////////////////
//файл:         FingerProcessor.cpp
//версия:
//дата:
//изменен:
//автор:
//
//описание:     Класс обработки отпечатков пальцев
//////////////////////////////////////////////////////////////////////////

#include <qresource.h>

#include "fp/FPProcess.h"
#include "fp/CFPMinutiae.h"
#include "fp/CFPTemplate.h"
#include "fp/CFunctional.h"
#include "fp/FPDefine.h"
#include "BimFp.h"
#include "FingerProcessor.h"


//Размер базы "Все чужие"
#define F_BASE_ALL_SIZE 120

//Среднее математическое ожидание "Все чужие"
float g_mo[fpCpNum] = {
    168.0333333f, 0.25f, 1.25f, 0.8f, -3.2833333f, -0.0666667f, 0.7f, 2.1833333f,
    2.8f, -1.3166667f, -5.05f, -0.8333333f, -2.4833333f, 0.3333333f, 1.0333333f,
    -0.75f, 2.5f, -0.1833333f, 1.15f, 0.8666667f, -0.6166667f, -0.1666667f,
    -0.8666667f, -1.4833333f, 0.3333333f, 0.3833333f, 0.3166667f, -0.9f, -0.15f,
    -0.5f, -0.3333333f, 1.3833333f, 3.8f, -0.4833333f, 1.15f, 1.0f, 2.0166667f,
    -1.2f, 0.3333333f, -0.8833333f, -1.4666667f, -0.0833333f, -1.05f, 0.9333333f,
    -3.2833333f, -0.2333333f, -1.5666667f, 0.0833333f, -5.6f, 0.4166667f,
    1.7166667f, -1.8666667f, 0.2166667f, -1.2333333f, -2.9f, -0.0833333f, -1.0f,
    0.95f, 1.65f, 0.2f, 0.5833333f, 1.5333333f, 2.4666667f, 1.2166667f,
    7.3166667f, 0.4f, 2.7333333f, -1.05f, -13.4333333f, -2.4166667f, -4.3833333f,
    6.0333333f, -0.4166667f, 0.2666667f, 0.4666667f, 1.55f, -0.6f, 0.4833333f,
    3.05f, 0.6666667f, -2.4166667f, 0.7666667f, -1.0333333f, -1.7833333f,
    -0.7666667f, 3.95f, 5.1166667f, -0.2333333f, -1.2833333f, -0.2333333f,
    1.0333333f, -2.3166667f, 0.6f, 2.1833333f, -0.25f, -0.2666667f, -3.3833333f,
    0.1333333f, -0.3666667f, -0.25f, -2.3333333f, 5.1833333f, 14.5166667f,
    1.0333333f, -1.95f, 0.3666667f, 0.9333333f, 0.85f, 2.0f, 0.7833333f, 4.3833333f,
    -0.7f, 2.4166667f, -0.3666667f, -0.7333333f, -1.9833333f, 4.7333333f, 1.95f,
    3.2166667f, -3.3666667f, -0.0833333f, -0.4666667f, 0.3666667f, 0.5166667f,
    -0.2666667f, -0.1833333f, -1.85f, -1.6333333f, 8.0833333f, -0.2333333f,
    0.4666667f, -1.3166667f, 1.5333333f, 0.6833333f, 1.8833333f, -0.7f,
    -8.8833333f, 7.6f, 14.9f, -0.6833333f, 37.8f, 1.95f, 4.4166667f, -2.5666667f,
    -4.6166667f, 0.1f, -0.0666667f, -0.6833333f, 0.9f, -0.25f, 1.8833333f,
    -0.0666667f, 6.2166667f, 0.2666667f, -1.5666667f, 0.15f, 1.3f, -1.5166667f,
    -3.1833333f, 1.2666667f, -10.6166667f, -0.2333333f, 1.7f, 1.55f, 0.1f, -0.25f,
    -1.15f, 0.2333333f, 15.65f, -0.5f, -1.1666667f, 0.75f, 1.1333333f, -4.35f,
    -9.85f, -3.0f, 1.4833333f, 0.2333333f, 1.5666667f, -2.35f, -0.8666667f, 0.35f,
    0.9166667f, 1.3333333f, 1.55f, -0.3666667f, -0.8333333f, -0.4166667f, -4.9f,
    -0.0166667f, -2.1833333f, 2.2333333f, -19.0333333f, -0.4833333f, -0.85f,
    -2.2333333f, -1.0166667f, 0.7333333f, 2.6666667f, -0.3166667f, 35.3f, 2.05f,
    4.6166667f, -4.7666667f, 8.55f, -6.7666667f, -12.1666667f, 0.7833333f,
    2.6666667f, 0.05f, -2.1833333f, 0.7333333f, -0.7833333f, 0.3333333f,
    0.8666667f, -0.75f, 2.7333333f, -1.55f, -3.7833333f, 0.0666667f, -7.15f, -0.7f,
    -2.1666667f, 0.8166667f, 2.8f, -0.35f, -2.8166667f, -0.9666667f, 1.95f,
    0.8666667f, 1.5f, -0.9833333f, 5.9666667f, -3.9166667f, -11.5833333f, -1.2f,
    -18.25f, -1.8666667f, -1.1f, 1.0166667f, 1.5f, 0.7833333f, -1.2833333f, 1.8f,
    2.3166667f, -0.7333333f, -1.6333333f, 0.45f, -4.0f, -0.5166667f, -2.1833333f,
    3.7f, -0.2166667f, 1.1333333f, 0.9f, -0.8166667f
};

//Нормирующая дисперсия "Все чужие
float g_d[fpCpNum]  = {
    42.6076936f, 0.8088057f, 11.7183972f, 13.2769223f, 20.9603528f, 10.8572045f, 21.8592467f, 9.1460222f,
    28.139711f, 6.5357776f, 16.5624324f, 11.2467279f, 25.4089824f, 6.6712484f, 16.0535216f, 11.1857424f,
    5.9019771f, 4.8183388f, 11.025463f, 12.2024132f, 9.4729293f, 8.0863396f, 10.0481618f, 8.7606919f,
    5.9249941f, 5.3823529f, 12.4250978f, 12.7399634f, 8.5573068f, 7.7824589f, 9.9367444f, 11.0409717f,
    19.6996616f, 2.9748483f, 13.7256755f, 12.4425346f, 15.2277068f, 9.5538823f, 17.5776057f, 9.1979919f,
    16.531754f, 5.3300771f, 14.1355639f, 11.1480442f, 18.6878674f, 7.0754662f, 15.7022999f, 10.0520175f,
    18.5496631f, 3.333125f, 12.5393935f, 14.3920889f, 15.1559578f, 7.580384f, 16.0833247f, 9.6320501f,
    15.2025217f, 5.4019287f, 14.2686428f, 11.8396227f, 17.2359041f, 7.6266346f, 13.8563664f, 11.1670522f,
    24.5794573f, 2.1656408f, 12.1468331f, 13.4094805f, 18.9875983f, 9.3937775f, 21.5708844f, 8.9563137f,
    21.7050621f, 6.2911755f, 17.243324f, 11.8031422f, 20.3868422f, 6.5459699f, 16.8190616f, 11.1222999f,
    13.734739f, 4.0077702f, 11.3731653f, 11.6391461f, 12.1351647f, 8.4585755f, 13.6358494f, 10.4353033f,
    11.788542f, 5.1311684f, 13.6093922f, 10.9521561f, 13.5335386f, 7.7146866f, 12.1375794f, 10.8449169f,
    24.9813875f, 2.6202205f, 12.9941867f, 12.5414313f, 20.4378462f, 9.5629348f, 18.8736957f, 10.3496645f,
    20.6457623f, 6.619332f, 15.0768771f, 9.8367085f, 21.4604287f, 7.0073572f, 15.5328594f, 10.8455828f,
    14.5468114f, 3.6989488f, 11.895891f, 11.63399f, 13.2392934f, 9.0137395f, 14.5282388f, 9.4612661f,
    14.7955981f, 5.7270314f, 13.0862099f, 12.466611f, 14.234192f, 7.7642164f, 13.1628581f, 11.2538389f,
    30.8902852f, 1.4590712f, 11.6475558f, 14.3729974f, 19.374353f, 8.5371573f, 18.6199639f, 8.8690473f,
    27.4585091f, 7.2219573f, 17.9445442f, 11.2657174f, 28.6241739f, 7.394649f, 19.4115873f, 10.8303073f,
    8.619922f, 4.6804558f, 12.0669889f, 10.6606624f, 10.1705457f, 7.2861169f, 11.4994082f, 10.5551988f,
    8.4085109f, 4.7727234f, 12.5311168f, 12.9702544f, 11.5221526f, 7.1914108f, 10.7447532f, 10.8264594f,
    18.9965713f, 2.6762328f, 11.8754649f, 12.837997f, 17.9612546f, 8.9510241f, 19.6017219f, 10.2076224f,
    17.0174665f, 6.503845f, 16.4323529f, 11.1946788f, 20.1609909f, 6.8308735f, 12.5190854f, 11.0627001f,
    15.6647073f, 3.7097469f, 12.7669713f, 11.9538348f, 14.6446426f, 8.4337121f, 15.9930106f, 10.764396f,
    16.8002232f, 5.4236109f, 15.3391293f, 12.0489165f, 13.8590524f, 7.8389873f, 13.683922f, 9.0219485f,
    27.7491541f, 1.67821f, 11.9789051f, 12.4336461f, 20.9956279f, 8.8399975f, 19.4406504f, 10.1316199f,
    25.1990741f, 7.7318066f, 18.6145925f, 11.9580749f, 30.0490848f, 7.2131054f, 17.4706102f, 11.9806673f,
    10.6005765f, 4.2950553f, 11.0536444f, 13.0631373f, 10.1309619f, 8.1591802f, 13.2526308f, 8.7017719f,
    10.2442938f, 5.3709869f, 11.318262f, 12.3009033f, 12.3690811f, 8.3102347f, 11.7822843f, 9.3354016f,
    21.5903528f, 2.809656f, 12.0713044f, 13.0951221f, 17.4646548f, 7.2594f, 16.0509605f, 10.0141428f,
    20.2018701f, 6.5225038f, 15.4405221f, 10.925963f, 19.5709181f, 7.8845137f, 14.6312451f, 9.8395319f,
    14.2851905f, 3.2766429f, 11.6563168f, 12.194671f, 14.7054317f, 7.7402986f, 13.6607304f, 9.0763888f,
    12.999359f, 5.3181816f, 12.9028313f, 11.5870618f, 16.0925362f, 7.3868502f, 12.0501729f, 9.88516f
};

QAtomicInt FingerProcessor::_cRefs(0); // счетчик ссылок на компоненты

// Конструктор
FingerProcessor::FingerProcessor(){
  _cRefs.ref();
}

// Деструктор
FingerProcessor::~FingerProcessor(){
  _cRefs.deref();
}

//Значение глобального счетчика компонентов
nbResult FingerProcessor::getCoRefs(){
  return (nbResult)_cRefs;
}

///////////////////////nbIUnknown//////////////////////////////////////

//Запрос описателя интерфейса компонента
nbMETHOD_DEF(FingerProcessor,queryInterface)(nbRefUuid iid, void **icd){
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

//Увеличение счетчика ссылок на интерфейс (в случае дублирования ссылки на него)
nbMETHOD_DEF(FingerProcessor,retain)(nbTHIS){
  return (nbResult)_iRefs.ref();
}

//Освобождение интерфейса, путем уменьшения счетчика ссылок на него
nbMETHOD_DEF(FingerProcessor,release)(nbTHIS){
  if (!_iRefs.deref()){
    delete this;
    return nbS_OK;
  }
  return _iRefs;
}


///////////////////////nbIBimProcessor///////////////////////////////////


//Установка обработчика событий от обработчика образов
nbMETHOD_DEF(FingerProcessor,setEventHandler)(nbTHISc nbIEventHandler *handler){
  return nbE_UNSUPPORTED_FUNCTION;
}

//Установка сформированного  ранее шаблона обработки образов
nbMETHOD_DEF(FingerProcessor,setTemplate)(nbTHISc const nbMatrix *pattern, nbRefUuid bimType){

    _pattern.copy(*pattern);
    if (_pattern.empty()) return nbE_PARAM;
    FPPoint *tMinutiae = (FPPoint*)_pattern.at(0,0).v;
    for (uint32_t j=0; j<5; ++j){
        if ((tMinutiae->x < 0) || (tMinutiae->x > fpWt) || (tMinutiae->y < 0) || (tMinutiae->y > fpHt)) {
            _pattern.free();
            return nbE_PARAM;
        }
         tMinutiae++;
    }
    return nbS_OK;

}

//Получение шаблонов биометрических образов
nbMETHOD_DEF(FingerProcessor,createTemplate)(nbTHISc nbMatrix **baseOwn, nbMatrix **pattern, const nbBim **bimBase, uint32_t bimCount){

    //Получение шаблонов биометрических образов
    //под матрицу baseOwn - аналогично Process
    //под матрицу pattern в теле функции в соответствии с необходимым метаописанием
/*
    NbMeta m1(5*sizeof(FPPoint),nbMF_U8,nbMT_DISCRETE);
    NbMatrix *m = new NbMatrix(1,5,&m1);
    FPPoint *pt = (FPPoint*)m->at(0,0).v;
    *pattern = m;

    // Запись шаблона
    NbMatrix *mPattern = *(NbMatrix**)(pattern);
    mPattern->resizeData(1);
    FPPoint *tMinutiae = (FPPoint*)mPattern->at(0,0).v;
*/

    nbResult rslt;
    int rsltGetTemplate;
    //Проверить входные параметры
    if (!baseOwn) return nbE_PARAM;
    if (!pattern) return nbE_PARAM;
    if (bimCount < 2) return nbE_PARAM;
    for (uint32_t i=0; i<bimCount; ++i){
      if (bimBase[i]->bimType != nbBIM_TYPE_FINGER) return nbE_IDENTIFIER;
    }

    CFPTemplate templObject;
    CFunctional fObject;
    FPPoint *allMinutiae = new FPPoint[fpMtsNum*bimCount];
    FPPoint *bestMinutiae = new FPPoint[fpMtsNum*bimCount];
    FPPoint minutiae[fpMtsNum];
    FPPoint templMinutiae[5]; //шаблон
    int numberMinutiae = 0;
    int *numberArrMinutiae = new int [bimCount];
    int *numberArrBestMinutiae = new int [bimCount];
    int *rightImg = new int [bimCount];
    int allNumberMinutiae = 0;
    int newBimCount = 0;

    //Выделить память под матрицу векторов биометрических параметров
    rslt = queryProcessRules(baseOwn,nbBIM_TYPE_FINGER);
    if (nbFAILED(rslt)) return rslt;
    NbMatrix *m = *(NbMatrix**)(baseOwn);
    m->resizeData(bimCount);

    //В цикле выполнить найти все минуции и вычислить дискретные коэффициенты
    BimFp fp;
    static int32_t img[fpHWt];
    static int32_t dparam[fpDpNum];
    for (uint32_t i=0; i<bimCount; ++i){
      fp.attach((nbBim*)bimBase[i]);
      fp.toInt32(img);
      Vbp &d = m->at(1,i);
      Vbp &c = m->at(0,i);
      // Проверить на обнаружение минуций
      if (fp_process(img,minutiae, numberMinutiae, c.r32,dparam)){
        fp.detach();
        // Обнулить данные
        memset(allMinutiae,0, sizeof(FPPoint)*fpMtsNum*bimCount);
        memset(bestMinutiae,0, sizeof(FPPoint)*fpMtsNum*bimCount);
        memset(minutiae,0, sizeof(FPPoint)*fpMtsNum);
        memset(templMinutiae,0, sizeof(FPPoint)*5);
        memset(numberArrMinutiae,0, sizeof(int)*bimCount);
        memset(rightImg,0, sizeof(int)*bimCount);
        memset(numberArrBestMinutiae,0, sizeof(int)*bimCount);
        return nbE_NO_DATA_OWN;
      }
      // Преобразовать дискретные
      for (int32_t j=0; j<fpDpNum; ++j){
        d.i1(j,dparam[j]>0);
      }
     // Записать все минуции примера в общий вектор
       for (int32_t j=0; j<numberMinutiae; ++j){
          allMinutiae[j+allNumberMinutiae]= minutiae[j];
      }
	  allNumberMinutiae += numberMinutiae;
      numberArrMinutiae[i] = numberMinutiae;

      fp.detach();
    }

    memset(img,0,sizeof(int32_t)*fpHWt);
    memset(dparam,0,sizeof(int32_t)*fpDpNum);

    // Получить шаблон
    rsltGetTemplate = templObject.getTemplate(allMinutiae, numberArrMinutiae, bimCount, rightImg, bestMinutiae, numberArrBestMinutiae, templMinutiae);

    // Получить число "годных" примеров
    for (uint32_t j=0; j<bimCount; ++j){
        if (rightImg[j] == 1) newBimCount++;
    }
   /*
    // Вычислить непрерывные по лучшим минуциям
    allNumberMinutiae = 0;
    for (int32_t i=0; i<bimCount; ++i){
      Vbp &c = m->at(0,i);
      c.r32 = fObject.WelshAdamar(bestMinutiae,numberArrBestMinutiae[i]);
	  bestMinutiae += numberArrBestMinutiae[i];


        //  Нормализовать непрерывные
        for (int32_t j=0; j<fpCpNum;; ++j){
            //c.r32[j] = (c.r32[j]-g_mo[j])/g_d[j];
        }

    }
*/
    // Получить параметры с учетом шаблона
    if (!rsltGetTemplate){
        // Вычислить непрерывные по смещенным минуциям
        static float dX = 0.0f;
        static float dY = 0.0f;
        static float dR = 0.0f;
        static float res = 0.0f;
        allNumberMinutiae = 0;
        for (uint32_t i=0; i<bimCount; ++i){
            Vbp &c = m->at(0,i);
            templObject.MatchingSpeed(templMinutiae,5,allMinutiae+allNumberMinutiae,numberArrMinutiae[i],&res,&dX,&dY,&dR);
            dX *= (-1.0f);
            dY *= (-1.0f);
            templObject.AffineTransform(allMinutiae+allNumberMinutiae,numberArrMinutiae[i],dX,dY,0.0f);
            c.r32 = fObject.WelshAdamar(allMinutiae+allNumberMinutiae,numberArrMinutiae[i]);
            allNumberMinutiae += numberArrMinutiae[i];
        }

        // Выделить память под шаблон
        NbMeta m1(5*sizeof(FPPoint),nbMF_U8,nbMT_DISCRETE);
        *pattern = new NbMatrix(1,5,&m1);
        if (!*pattern) return nbE_OUT_OF_MEMORY;

        // Запись шаблона
        NbMatrix *mPattern = *(NbMatrix**)(pattern);
        mPattern->resizeData(1);
        FPPoint *tMinutiae = (FPPoint*)mPattern->at(0,0).v;

        for (int32_t i=0; i<5; ++i){
          tMinutiae->x = templMinutiae[i].x;
          tMinutiae->y = templMinutiae[i].y;
          tMinutiae->direct = templMinutiae[i].direct;
          tMinutiae++;
        }

    }

    else {
        *pattern = nbNULL;
    }

    //  Нормализовать непрерывные
    for (uint32_t i=0; i<bimCount; ++i){
        Vbp &c = m->at(0,i);
        for (int32_t j=0; j<fpCpNum; ++j){
            c.r32[j] = (c.r32[j]-g_mo[j])/g_d[j];
        }
    }


    // Обнулить данные
    memset(allMinutiae,0, sizeof(FPPoint)*fpMtsNum*bimCount);
    memset(bestMinutiae,0, sizeof(FPPoint)*fpMtsNum*bimCount);
    memset(minutiae,0, sizeof(FPPoint)*fpMtsNum);
    memset(templMinutiae,0, sizeof(FPPoint)*5);
    memset(numberArrMinutiae,0, sizeof(int)*bimCount);
    memset(rightImg,0, sizeof(int)*bimCount);
    memset(numberArrBestMinutiae,0, sizeof(int)*bimCount);

    return nbS_OK;


}

//Обработка базы (вектора) биометрических образов
nbMETHOD_DEF(FingerProcessor,process)(nbTHISc nbMatrix **baseAny, const nbBim **bimBase, uint32_t bimCount, nbMatrix **ind){
      nbResult rslt;
     //Проверить входные параметры
      if (!baseAny) return nbE_PARAM;
      for (uint32_t i=0; i<bimCount; ++i){
        if (bimBase[i]->bimType != nbBIM_TYPE_FINGER) return nbE_IDENTIFIER;
      }

      CFPTemplate templObject;
      CFunctional fObject;

      //Выделить память под матрицу векторов биометрических параметров
      rslt = queryProcessRules(baseAny,nbBIM_TYPE_FINGER);
      if (nbFAILED(rslt)) return rslt;
      NbMatrix *m = *(NbMatrix**)(baseAny);
      m->resizeData(bimCount);

      //Выделить память под матрицу индикаторов
      NbMatrix *mi;
      if (ind){
       *ind  = new NbMatrix(bimCount,NbMeta(1,nbMF_I32,nbMT_DISCRETE_OWN));  
       if (!*ind) return nbE_OUT_OF_MEMORY;
       mi = *(NbMatrix**)(ind);
       mi->resizeData(bimCount);
      }

      static FPPoint templMinutiae[5];
      // Загрузить минуции шаблона
      if (!_pattern.empty()){
	  FPPoint *tMinutiae = (FPPoint*)_pattern.at(0,0).v;
          for (uint32_t j=0; j<5; ++j){
              templMinutiae[j].x = tMinutiae->x;
              templMinutiae[j].y = tMinutiae->y;
              templMinutiae[j].direct = tMinutiae->direct;
              tMinutiae++;
          }
      }

      //В цикле выполнить преобразование и заполнить матрицу baseAny
      static FPPoint minutiae[fpMtsNum];
      static int numberMinutiae;
      static float dX = 0.0f;
      static float dY = 0.0f;
      static float dR = 0.0f;
      static float res = 0.0f;
      BimFp fp;
      static int32_t img[fpHWt];
      static int32_t dparam[fpDpNum];
      for (uint32_t i=0; i<bimCount; i++){
        fp.attach((nbBim*)bimBase[i]);
        fp.toInt32(img);
        Vbp &c = m->at(0,i);
        Vbp &d = m->at(1,i);

        if (!fp_process(img, minutiae, numberMinutiae, c.r32,dparam)){
          // Сместить по шаблону
          if (!_pattern.empty()){
            templObject.MatchingSpeed(templMinutiae,5,minutiae,numberMinutiae,&res,&dX,&dY,&dR);
            dX *= (-1.0f);
            dY *= (-1.0f);
            templObject.AffineTransform(minutiae,numberMinutiae,dX,dY,0.0f);
            c.r32 = fObject.WelshAdamar(minutiae,numberMinutiae);
            fObject.DiskretCell(minutiae,numberMinutiae,fpBlkSize,fpBlkSize,fpWt,fpHt,dparam);
          } //Сместить по шаблону

        }

        //  Нормализовать непрерывные
        for (int32_t j=0; j<fpCpNum; ++j){
          c.r32[j] = (c.r32[j]-g_mo[j])/g_d[j];
        }

       // Преобразовать дискретные
        for (int32_t j=0; j<fpDpNum; ++j){
          d.i1(j,dparam[j]>0);
        }

        //Вычислить значение индикатора
        if (ind){
            res = fpResThr - res;
            if ((res < 0) || (_pattern.empty())) mi->at(0,i).i32[0] = 0; // Значение индикатора "Свой"
            else mi->at(0,i).i32[0] = int32_t(fpCpNum*res/fpResThr); // Значения индикатора "Чужой"
        }
        fp.detach();
      }

      // Обнулить данные
      memset(img,0,sizeof(int32_t)*fpHWt);
      memset(dparam,0,sizeof(int32_t)*fpDpNum);
      memset(minutiae,0, sizeof(FPPoint)*fpMtsNum);
      memset(templMinutiae,0, sizeof(FPPoint)*5);

      return nbS_OK;
}


//Запрос шаблона обработки биометрического образа заданного типа
nbMETHOD_DEF(FingerProcessor,queryProcessRules)(nbTHISc nbMatrix **meta, nbRefUuid bimType){
  //Проверить входные параметры
  if (!meta) return nbE_PARAM;
  if (bimType != nbBIM_TYPE_FINGER) return nbE_IDENTIFIER;

  //Создать матрицу шаблона для выдаваемых параметров
  NbMeta matr[2] = { NbMeta(256,nbMF_R32,nbMT_CONTINUOUS), NbMeta(96,nbMF_I1,nbMT_DISCRETE)};
  *meta = new NbMatrix(2,0,matr);
  if (!*meta) return nbE_OUT_OF_MEMORY;
  return nbS_OK;
}


//Чтение базы "Все чужие" или "Свой", представленной в виде матрицы ВБП
nbMETHOD_DEF(FingerProcessor,queryBaseAny)(nbTHISc nbMatrix **baseAny, uint32_t maxCount, nbRefUuid bimType, int32_t userId){
  
  //Проверить тип биометрического образа и пользователя
  if (!baseAny) return nbE_PARAM;
  if (bimType != nbBIM_TYPE_FINGER) return nbE_IDENTIFIER;
  if (userId != 0) return nbE_PARAM;
  
  //
  QResource resBaseC(":/base/cparam");
  QResource resBaseD(":/base/dparam");

  //F_BASE_ALL_SIZE примеров
  float* baseC    = (float*)resBaseC.data();
  uint8_t* baseD  = (uint8_t*)resBaseD.data();  //содержит готовые битовые вектора
  if (!baseC || !baseD) return nbE_OUT_OF_MEMORY;

  nbResult rslt = queryProcessRules(baseAny,nbBIM_TYPE_FINGER);
  if (nbFAILED(rslt)) return rslt;
  NbMatrix *m = *(NbMatrix**)(baseAny);
  m->resizeData(F_BASE_ALL_SIZE);

  //Преобразование данные в матрицы параметров
  for (uint32_t i=0; i<F_BASE_ALL_SIZE; ++i){
    Vbp &c = m->at(0,i);
    Vbp &d = m->at(1,i);
    //  Скопировать непрерывные параметры
    memcpy(c.r32,baseC,fpCpNum*sizeof(float));
    //  Скопировать дискретные параметры
    memcpy(d.v,baseD,fpDpNum/8);
    //Сдвинуть вектора дискретных и непрерывных параметров    
    baseC += fpCpNum;
    baseD += fpDpNum/8;
  }
  return nbS_OK;
}


//Освобождение памяти из-под базы "Все Чужие" или базы, полученной во время обработки
nbMETHOD_DEF(FingerProcessor,releaseBase)(nbTHISc nbMatrix *baseAny){
  if (baseAny) {
    delete (NbMatrix*)baseAny;
  }
  return nbS_OK;
}


///////////////////////nbIConfigurate///////////////////////////////////


//Загрузка текущей конфигурации
nbMETHOD_DEF(FingerProcessor,setConfig)(nbUuid page, nbData opt){
  return nbE_PARAM;
}


//Чтение текущей конфигурации
nbMETHOD_DEF(FingerProcessor,getConfig)(nbUuid page, nbData opt){
  return nbE_PARAM;
}


//Запрос конфигурации "по умолчанию"
nbMETHOD_DEF(FingerProcessor,getDefaultConfig)(nbUuid page, nbData opt){
  return nbE_PARAM;
}


//Настройка конфигурации
nbMETHOD_DEF(FingerProcessor,tuneUp)(nbHandle handle, nbUuid page, nbData opt, nbIEventHandler **guihandler){
  return nbE_PARAM;
}


//Перечисление страниц конфигурации, которые можно настроить для указанного интерфейса
nbMETHOD_DEF(FingerProcessor,enumPages)(nbIEventHandler *eh, nbRefUuid iid){
  return nbS_OK;
}

//Получение описания страницы конфигурации
nbMETHOD_DEF(FingerProcessor,getTitle)(nbUuid page, nbData name, nbData descr){
  return nbS_OK;
}
