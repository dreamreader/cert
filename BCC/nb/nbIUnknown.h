//////////////////////////////////////////////////////////////////////////
//файл:         nbIUnknown.h
//версия:       0.9
//дата:         01.01.2010
//изменен:
//автор:
//
//цель:         Описание ПИК Неизвестный nbIUnknown, сопутствующих макросов,
//              типа nbResult, кодов успеха, кодов ошибок,
//              константы nbUUID_IUNKNOWN
//////////////////////////////////////////////////////////////////////////

#ifndef _NB_IUNKNOWN_H
#define _NB_IUNKNOWN_H

#include "nbTypes.h"
#include "nbResult.h"

#pragma pack(1)

//.............................................................................
//  МАКРОСЫ единообразного описания интерфейсов на C,C++
//.............................................................................

#if defined(__cplusplus)
  #if defined(interface)
    #undef interface
  #endif
  #define interface struct//__interface

  #define nbPURE =0
  #define nbTHIS  void
  #define nbTHISc

  #define nbMETHOD(method)  virtual nbResult nbCALL method
  #define nbMETHOD_DEF(iclass,method) nbResult nbCALL iclass::method
  
  #define nbDECLARE_INTERFACE0(iface)            interface nbNOVTABLE iface
  #define nbDECLARE_INTERFACE(iface, baseiface)  interface nbNOVTABLE iface : public baseiface
  
#else
  #define interface struct

  #define PURE
  #define THIS  nbINTERFACE *this
  #define THISc nbINTERFACE *this,

  #define nbMETHOD(method)  nbResult (nbCALL *method)
  #define nbMETHOD_DEF(iclass,method) nbResult nbCALL iclass##_##method

  #define nbDECLARE_INTERFACE0(iface)\
    typedef interface iface { \
      struct iface##Vtbl * vtbl; \
    } iface; \
    typedef struct iface##Vtbl iface##Vtbl; \
    struct iface##Vtbl
  #define DECLARE_INTERFACE(iface, baseiface)    nbDECLARE_INTERFACE0(iface)
#endif

//.............................................................................
//  ПИК Неизвестный
//.............................................................................

// Идентификатор интерфейса nbIUnknown
// {00000000-0000-0000-C000-000000000046}
static const nbUuid nbUUID_IUNKNOWN = 
  {0x00000000,0x0000,0x0000,{0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x46}};

#undef  nbINTERFACE
#define nbINTERFACE   nbIUnknown

nbDECLARE_INTERFACE0(nbIUnknown){
  
  //Запрос описателя интерфейса компонента
  //  iid     - [вх]    Ссылка на идентификатор интерфейса
  //  icd     - [вых]   Указатель на описатель интерфейса компонента
  //возвращает:  nbS_OK
  //             nbE_NO_INTERFACE
  nbMETHOD(queryInterface)(nbTHISc nbRefUuid iid, void **icd) nbPURE;
  
  //Увеличение счетчика ссылок на интерфейс (в случае дублирования ссылки на него)
  //возвращает:  положительное значение
  nbMETHOD(retain)(nbTHIS) nbPURE;
  
  //Освобождение интерфейса, путем уменьшения счетчика ссылок на него
  //возвращает: неотрицательное значение
  nbMETHOD(release)(nbTHIS) nbPURE;
};

//.............................................................................
//  ПИМ модуля НПБК
//.............................................................................

//Указатель на функцию 'NbQueryComponent'. Запрос и создание компонента
//  cid - [вх]  Идентификатор компонента
//  iid - [вх]  Идентификатор ПИК
//  icd - [вых] Описатель ПИК
//возвращает: nbS_OK, nbE_NO_INTERFACE, nbE_NO_COMPONENT, nbE_NO_MODULE, nbE_FAIL
typedef nbResult (nbCALL *NbQueryComponentPtr)(nbRefUuid cid, nbRefUuid iid, void **icd);

#pragma pack()

#endif
