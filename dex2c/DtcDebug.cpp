/*****************************************************************************
*   DtcDebug.cpp:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
******************************************************************************/

#include "DtcDebug.h"

//////////////////////////////////////////////////
// Debugging Function Definition
//////////////////////////////////////////////////

/**
 * This function will be used to print out the debugging message.
 */
 
void 
DtcDebugPrintln(const char *sTag, const char *sFmt, ...) {

	char buf[256];
		
	va_list args;
	va_start(args, sFmt);
	vsprintf(buf, sFmt, args);
	va_end(args);

#ifdef ANDROID_SUPPORT
	ALOGD("%s:%s", sTag, buf);
#else
	printf("%s]: %s\n", sTag, buf); 
#endif
}

