//////////////////////////////////////////////////////////////////////////
//����:
//������:
//����:
//�������:
//�����:        ������� �.�.
//
//����:         ���������� ������� �������������������� ����� ������
//              �� ���� � 52633.4
//
//
//////////////////////////////////////////////////////////////////////////
#ifndef _NB_BASE_H
#define _NB_BASE_H

//.............................................................................
//  ���������� � ����������� ����� �� ������ ����������
//.............................................................................
//
//  Data Type  LP32  ILP32   ILP64   LLP64   LP64
//  char        8      8      8       8       8
//  short       16    16      16      16      16
//  int         16    32      64      32      32
//  long        32    32      64      32      64
//  long long (int64)         64   
//  pointer     32    32      64      64      64
//
//  DOS                                                         - LP32
//  Microsoft Win32, Unix 32-bit                                - ILP32
//  Most Unix (like Solaris) and Unix-like systems (like Linux) - LP64
//  Microsoft Win64 (X64/IA64)                                  - LLP64
//  HAL                                                         - ILP64
//
//.............................................................................

#undef  _LP32
#undef  _ILP32
#undef  _ILP64
#undef  _LLP64
#undef  _LP64

#if defined(_DOS)
#define _LP32
#define _OS_DOS
#elif defined(__WIN32__) || defined(__WIN32) || defined(_WIN32) || defined(WIN32) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#if defined(_WIN64)
#define _LLP64
#define _OS_WIN
#else  
#define _ILP32
#define _OS_WIN
#endif
#elif defined(_POSIX_VERSION)
#if defined(_M_X64)
#define _LP64
#define _OS_UNIX
#else
#define _ILP64
#endif
#else
//����������� ��������� � ����������� ����������
#error Unknown platform. See basic type definition
#endif


//.............................................................................
//  ������� ���� ������
//.............................................................................
//  ����������:
//  1. � ���������� ������ ������������ little-endian ������� ������ �����
//  2. � ����������, �������� ��������� �������� ������ 64 ���, ������������
//     ���������� �� 64 ��� � ������� ������� nbRESERVED(i)
//  3. ������ ������������� �� �������� ����
//  4. � Unix-�������� �������� ������ ���������������� ����� ������
//      ��� ����������� ����� ������������ ������������ ���� <inttypes.h>
//  5. ������������ �� ��������� �, ��������������, �� ����������������
//     ��������� ���� ������:
//      (u)char, (u)short, void, float, double, wchar_t
//  6. �������� ����:
//      (u)int8_t, (u)int16_t, (u)int32_t, (u)int64_t
//      wchar_t, float, double, half
//      enum
//      bitfield
//.............................................................................

#pragma pack(1)

typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;


#if   defined(_LP32)
typedef long              int32_t;
typedef unsigned long     uint32_t;
typedef long long         int64_t;
typedef unsigned long long uint64_t;
#elif defined(_ILP32)
typedef int               int32_t;
typedef unsigned int      uint32_t;
typedef long long         int64_t;
typedef unsigned long long uint64_t;
#elif defined(_ILP64)
typedef short int         int32_t;
typedef unsigned short int uint32_t;
typedef long              int64_t;
typedef unsigned long     uint64_t;
#elif defined(_LLP64)
typedef int               int32_t;
typedef unsigned int      uint32_t;
typedef long long         int64_t;
typedef unsigned long long uint64_t;
#elif defined(_LP64)
typedef int               int32_t;
typedef unsigned int      uint32_t;
typedef long              int64_t;
typedef unsigned long     uint64_t;
#endif

// ����������� ��� ���������� �� 64-���
#if defined(_ILP64) || defined(_LLP64) || defined(_LP64)
#define nbRESERVED(i)
#else
#define nbRESERVED(i) uint32_t reserved##i;
#endif

#pragma pack()

//.............................................................................
//  ����������
//  _MICROSOFT_SPECIFIC - ��������� ��� ����������� MS Visual Studio
//.............................................................................

#if defined(_OS_WIN)
#define nbCALL      __cdecl                 //  ���������� � ������� �������:
//  1.�������� ���������� ������ ������;
//  2.�������������� ���������� �������������� �� ��������,
//    ����� ���������� ��� ������;
//  3.���������� ������� ��������� ���� ��������� �� �����
//    ��������������;
//  4.�� ��������� � ������ ����� ������� ����������� (_),
//    � � ����� (@) � ����� ���� ������� ���������� � ���������� �.�.;
//  5.���������� �� �������� ���������� �� �����������;
#define nbNOVTABLE
#else
#define nbCALL      __cdecl
#define nbNOVTABLE
#endif

//.............................................................................

#define nbNIL           0

#endif
