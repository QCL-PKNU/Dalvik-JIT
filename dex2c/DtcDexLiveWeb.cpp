/*********************************************************************
*   DtcDexLiveWeb.cpp:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#include "DtcDexLiveWeb.h"

///////////////////////////////////////////////////////////////////////
// Constructor & Destructor Definition
///////////////////////////////////////////////////////////////////////
 
DtcDexLiveWeb::DtcDexLiveWeb(uint16_t nRegnum, uint32_t nFlag) {

	m_nDataType = J_UNKNOWN;
	m_nRegnum = nRegnum;
	m_nFlag = nFlag;
}

DtcDexLiveWeb::~DtcDexLiveWeb() {

	// Do nothing
}

///////////////////////////////////////////////////////////////////////
// Debugging Function Definition
///////////////////////////////////////////////////////////////////////

/**
 * This function will print out the signature of the live web.
 *
 * @param pOut output stream
 */
void
DtcDexLiveWeb::Write(ostream &pOut) {

	const static char typeCharArray[NUM_OF_JTYPES] = {
		'x',	/* J_UNKNOWN */ 
		'z',	/* J_BOOLEAN */
		'b',	/* J_BYTE */
		'c',	/* J_CHAR */
		's',	/* J_SHORT */
		'i',	/* J_INT */
		'l',	/* J_LONG */
		'f',	/* J_FLOAT */
		'd',	/* J_DOUBLE */
		'o',	/* J_OBJECT */
	};

	pOut << 'v' << typeCharArray[m_nDataType] << m_nRegnum;
}

ostream &operator << (ostream &pOut, DtcDexLiveWeb *pWeb) {
	
	pWeb->Write(pOut);
	return pOut;
}
