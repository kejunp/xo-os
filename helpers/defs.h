#pragma once

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef char  int8_t;
typedef short int16_t;
typedef int   int32_t;
typedef long  int64_t;

typedef unsigned long uintptr_t;

#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef long long unsigned int size_t;
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef uint64_t pde_t;

