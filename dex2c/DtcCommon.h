/*****************************************************************************
*   DtcCommon.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
******************************************************************************/

#ifndef _DTC_COMMON_H_
#define _DTC_COMMON_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <set>
#include <map>
#include <string>
#include <stack>
#include <queue>
#include <fstream>
#include <ostream>
#include <iostream>
#include <strstream>

#include "../../libdex/DexFile.h"
#include "../../libdex/DexProto.h"
#include "../../libdex/DexClass.h"
#include "../../libdex/DexOpcodes.h"
#include "../../libdex/InstrUtils.h"
#include "../../vm/Bits.h"

using namespace std;

//////////////////////////////////////////////////
// Common Configuration
//////////////////////////////////////////////////

#define ANDROID_SUPPORT

//#define INCLUDE_DEPRECATED

#ifdef ANDROID_SUPPORT
#include "Dalvik.h"
#endif

#include "DtcDebug.h"

#define STR_LEN		32
#define BUF_LEN		128

#define DEX2C_TAG	"DEX2C"

//////////////////////////////////////////////////
// User-defined Data Types
//////////////////////////////////////////////////

#ifndef _bool_t_defined_
#define _bool_t_defined_
typedef unsigned char bool_t;
#endif

#ifndef _uint8_t_defined_
#define _uint8_t_defined_
typedef unsigned char uint8_t;
#endif

#ifndef _int8_t_defined_
#define _int8_t_defined_
typedef signed char int8_t;
#endif

#ifndef _uint16_t_defined_
#define _uint16_t_defined_
typedef unsigned short uint16_t;
#endif

#ifndef _int16_t_defined_
#define _int16_t_defined_
typedef signed short int16_t;
#endif

#ifndef _uint32_t_defined_
#define _uint32_t_defined_
typedef unsigned int uint32_t;
#endif

#ifndef _int32_t_defined_
#define _int32_t_defined_
typedef signed int int32_t;
#endif

#ifndef _uint64_t_defined_
#define _uint64_t_defined_
typedef unsigned long long uint64_t;
#endif

#ifndef _int64_t_defined_
#define _int64_t_defined_
typedef signed long long int64_t;
#endif

#ifndef _cstr_t_defined_
#define _cstr_t_defined_
typedef const char *cstr_t;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

//////////////////////////////////////////////////
// User-defined Java Data Types
//////////////////////////////////////////////////

typedef unsigned char j_boolean;
typedef unsigned char j_byte;
typedef unsigned short j_char;
typedef short j_short;
typedef int j_int;
typedef long long j_long;
typedef float j_float;
typedef double j_double;
typedef void * j_object;

#endif
