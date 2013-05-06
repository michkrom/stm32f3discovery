/*****************************************************************************
 *   type.h:  Type definition Header file for NXP Family 
 *   Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.04.01  ver 1.00    Preliminary version, first Release
 *
******************************************************************************/
#ifndef __TYPE_H__
#define __TYPE_H__

// CodeRed - ifdef for GNU added to avoid potential clash with stdint.h
#if defined   (  __GNUC__  )
#include <stdint.h>
#else

/* exact-width signed integer types */
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       __int64 int64_t;

 /* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __int64 uint64_t;

#endif // __GNUC__ 

typedef unsigned long long  u64;
typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u08;

typedef signed long long   s64;
typedef signed long    s32;
typedef signed short   s16;
typedef signed char    s08;

typedef unsigned char       uint8;   //8 bits
typedef unsigned short      uint16;  //16 bits
typedef unsigned long       uint32;  //32 bits
typedef unsigned long long  uint64;  //64 bits


typedef signed char         int8;    //8 bits
typedef signed short        int16;   //16 bits
typedef signed long         int32;   //32 bits
typedef signed long long    int64;   //64 bits



typedef union _union16 {
  u16 u;
  s16 i;
  u08 b[2];
} union16;

typedef union _union32 {
  u32 lu;
  s32 li;
  u16 u[2];
  s16 i[2];
  u08 b[4];
} union32;

typedef union _union64 {
  u32 lu[2];
  s32 li[2];
  u16 u[4];
  s16 i[4];
  u08 b[8];
} union64;
#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#endif  /* __TYPE_H__ */
