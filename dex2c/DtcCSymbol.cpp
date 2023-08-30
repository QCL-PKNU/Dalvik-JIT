/*********************************************************************
*   DtcCSymbol.cpp:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#include "DtcCSymbol.h"

///////////////////////////////////////////////////////////////////////
// DtcCSymbol Class Definition
///////////////////////////////////////////////////////////////////////

DtcCSymbol::DtcCSymbol(DtcCSymbolType_t nType /* = DTC_C_SYM_UNKNOWN */) {

	m_nType = nType;
}

DtcCSymbol::~DtcCSymbol(void) {
	// Do nothing
}

ostream &operator <<(ostream &pOut, DtcCSymbol *pSymbol) {
	
	pSymbol->Write(pOut);
	return pOut;
}

///////////////////////////////////////////////////////////////////////
// DtcCConstSymbol Class Definition
///////////////////////////////////////////////////////////////////////

DtcCVarSymbol::DtcCVarSymbol(DtcDexLiveWeb *pWeb)
:DtcCSymbol(DTC_C_SYM_VAR) {

	DTC_ASSERT(m_pWeb != NULL);

	m_pWeb = pWeb;
}	

DtcCVarSymbol::~DtcCVarSymbol() {
	// Do nothing
}

/**
 * This function will return the type string of this variable symbol.
 * 
 * @return the type string of the varable symbol
 */
const char *
DtcCVarSymbol::GetTypeString() {

	static const char *dataTypeString[] = {

		"j_boolean",
		"j_byte",
		"j_char",
		"j_short",
		"j_int",
		"j_long",
		"j_float",
		"j_double",
		"j_object"
	};

	DTC_ASSERT(m_pWeb != NULL);

	DtcDexDataType_t dataType = m_pWeb->GetDataType();

	if(dataType < NUM_OF_JTYPES) {

		return dataTypeString[dataType];
	}

	return NULL;
}

void
DtcCVarSymbol::Write(ostream &pOut) {

	pOut << m_pWeb;
}

///////////////////////////////////////////////////////////////////////
// DtcCFuncSymbol Class Definition
///////////////////////////////////////////////////////////////////////

DtcCFuncSymbol::DtcCFuncSymbol(const char *pFuncName) {

	m_pFuncName = pFuncName;

}
	
DtcCFuncSymbol::~DtcCFuncSymbol() {
	// Do nothing
}

void
DtcCFuncSymbol::Write(ostream &pOut) {

	pOut << m_pFuncName; 
};


///////////////////////////////////////////////////////////////////////
// DtcCConstSymbol Class Definition
///////////////////////////////////////////////////////////////////////

DtcCConstSymbol::DtcCConstSymbol(DtcDexDataType_t nType, uint32_t nHigh32, uint32_t nLow32)
:DtcCSymbol(DTC_C_SYM_CONST) {

	m_nDataType = nType;

	m_nHigh32 = nHigh32;

	m_nLow32 = nLow32;
}
	
DtcCConstSymbol::~DtcCConstSymbol() {
	// Do nothing
}

void
DtcCConstSymbol::Write(ostream &pOut) {

	char buf[STR_LEN];
	
	struct {uint32_t h; uint32_t l;} value64b = {m_nHigh32, m_nLow32};		
	
	switch(m_nDataType) {
		case J_BOOLEAN:	
			sprintf(buf, "(j_boolean)%d", (j_boolean)m_nLow32);
			break;
		case J_BYTE:		
			sprintf(buf, "(j_byte)%d", (j_byte)m_nLow32);
			break;
		case J_CHAR:	
			sprintf(buf, "(j_char)%d", (j_char)m_nLow32);
			break;
		case J_SHORT:	
			sprintf(buf, "(j_short)%d", (j_short)m_nLow32);
			break;
		case J_INT:		
			sprintf(buf, "(j_int)%d", (j_int)m_nLow32);
			break;
		case J_LONG:		
			sprintf(buf, "(j_long)%ld", *((j_long *)&value64b));
			break;
		case J_FLOAT:	
			sprintf(buf, "(j_float)%f", (j_float)m_nLow32);
			break;
		case J_DOUBLE:	
			sprintf(buf, "(j_double)%lf", *((j_double *)&value64b));
			break;
		case J_OBJECT:			
			sprintf(buf, "(j_object)0x%08X", (j_object)m_nLow32);
			break;			
		default:			
			sprintf(buf, "(Unknown data type)0x%04X04X", m_nHigh32, m_nLow32);
			break;
	}

	pOut << buf;
}

///////////////////////////////////////////////////////////////////////
// DtcCLabelSymbol Class Definition
///////////////////////////////////////////////////////////////////////

DtcCLabelSymbol::DtcCLabelSymbol(uint32_t nLabelValue) {

	char *buf = (char *)malloc(STR_LEN);
	sprintf(buf, "%08X", nLabelValue);
	m_pLabelString = buf;
}

DtcCLabelSymbol::DtcCLabelSymbol(const char *pLabelString) {

	m_pLabelString = pLabelString;

}
	
DtcCLabelSymbol::~DtcCLabelSymbol() {
	// Do nothing
}

void
DtcCLabelSymbol::Write(ostream &pOut) {

	pOut << ".L" << m_pLabelString; 
};
