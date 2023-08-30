/*****************************************************************************
*   DtcDebug.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
******************************************************************************/

#ifndef _DTC_DEBUG_H_
#define _DTC_DEBUG_H_

#include "DtcCommon.h"

//////////////////////////////////////////////////
// Dex-To-C (DTC) Error Types
//////////////////////////////////////////////////

typedef enum {
	DTC_SUCCESS = 0,

	DTC_ERROR_TOO_SHORT_BUFFER,
	DTC_ERROR_MEMORY_ALLOC,

	DTC_ERROR_UNKNOWN_OPCODE,
	DTC_ERROR_INVALID_PARAMETER,
	DTC_ERROR_INVALID_DEX_CODE,
	DTC_ERROR_INVALID_DEX_METHOD,
	DTC_ERROR_INVALID_DEX_METHOD_TYPE,
	DTC_ERROR_INVALID_CLASS_DATA,
	DTC_ERROR_INVALID_DEBUG_INFO_STREAM,

	DTC_ERROR_INVALID_DEF_WEB,
	DTC_ERROR_INVALID_USE_WEB,

	DTC_ERROR_RESOLVE_DEXCODES,
	DTC_ERROR_RESOLVE_LOCAL_VARS,
	DTC_ERROR_RESOLVE_BASIC_BLOCKS,
	DTC_ERROR_BUILD_CONTROL_FLOW_GRAPH,
	DTC_ERROR_ANALYSE_LIVENESS,
	DTC_ERROR_DEX2C_TRANSLATION,

	DTC_FAILURE,
	
} DtcError_t;

//////////////////////////////////////////////////
// DTC Debug Function Declaration
//////////////////////////////////////////////////

void DtcDebugPrintln(const char * sTag, const char * sFmt,...);

//////////////////////////////////////////////////
// Debug Interface Macros for DTC Debugging Functions
//////////////////////////////////////////////////

#define DTC_ENABLE_DEBUG
#define DTC_ENABLE_ERROR
#define DTC_ENABLE_ASSERT
#define DTC_ENABLE_CHECK

#ifdef DTC_ENABLE_DEBUG
#define DTC_DEBUG(TAG, FMT, ...)		DtcDebugPrintln(TAG, FMT, ##__VA_ARGS__)
#else
#define DTC_DEBUG(TAG, FMT, ...)		//
#endif

#ifdef DTC_ENABLE_DEBUG
#define DTC_ERROR(TAG, FMT, ...)			DtcDebugPrintln(TAG" ERROR", FMT, ##__VA_ARGS__)
#else
#define DTC_ERROR(TAG, FMT, ...)			//
#endif

#ifdef DTC_ENABLE_CHECK
#define DTC_CHECK(EXPR, ERROR)		\
do{									\
	if(!(EXPR)) {						\
		return DTC_ERROR_##ERROR;	\
	}								\
} while(0)
#else
#define DTC_CHECK(EXPR, ERROR)		//
#endif

#ifdef DTC_ENABLE_ASSERT
#define DTC_ASSERT(EXPR)			\
do{									\
	assert(EXPR);						\
} while(0)
#else
#define DTC_ASSERT(EXPR)			//
#endif

#endif /* _DTC_DEBUG_H_ */
