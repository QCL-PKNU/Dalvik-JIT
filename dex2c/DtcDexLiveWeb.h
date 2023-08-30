/*********************************************************************
*   DtcDexLiveWeb.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#ifndef _DTC_DEX_LIVE_WEB_H_
#define _DTC_DEX_LIVE_WEB_H_

#include "DtcCommon.h"

///////////////////////////////////////////////////////////////////////
// User-defined Type Definition
///////////////////////////////////////////////////////////////////////

// Data type
typedef enum {

	J_UNKNOWN = 0,
	J_BOOLEAN,	/* Z */
	J_BYTE,		/* B */
	J_CHAR,		/* C */
	J_SHORT,	/* S */
	J_INT,		/* I */
	J_LONG,		/* J */
	J_FLOAT,	/* F */
	J_DOUBLE,	/* D */
	J_OBJECT,	/* L, [ */
	NUM_OF_JTYPES,
	
} DtcDexDataType_t;

// Flags for live webs
#define DTC_WEB_UNKNOWN		0x00
#define DTC_WEB_LOCAL_VAR		0x01
#define DTC_WEB_FUNC_ARG		0x02

///////////////////////////////////////////////////////////////////////
// DtcDexLiveWeb Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcDexLiveWeb {

private:
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////

	// data type
	DtcDexDataType_t m_nDataType;

	// register number
	uint16_t m_nRegnum;

	// a status flag for the web
	uint32_t m_nFlag;

public:
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcDexLiveWeb(uint16_t nRegnum, uint32_t nFlag = 0);

	virtual ~DtcDexLiveWeb();
	
	///////////////////////////////////////////////////////////////////
	// Accessor & Mutator Declaration/Definition
	///////////////////////////////////////////////////////////////////

	/**
	 * This function will be used to update the data type of the live web.
	 * 
	 * @param nDataType the resolved data type
	 */
	inline void SetDataType(DtcDexDataType_t nDataType)	{ 
		m_nDataType = nDataType; 
	}

	/**
	 * This function will return the data type of the live web.
	 * 
	 * @return the resolved data type
	 */
	inline DtcDexDataType_t GetDataType()	{ return m_nDataType ; }

	/**
	 * This function will indicate whether the web is for a local variable or not. 
	 *
	 * @return if it is the web of a local variable
	 */
	inline bool_t GetIsLocal()	{ 
		return (m_nFlag & DTC_WEB_LOCAL_VAR) ? TRUE : FALSE;	
	}

	/**
	 * This function will indicate whether the web is for a function argument or not. 
	 *
	 * @return if it is the web of a function argument
	 */
	inline bool_t GetIsArgument() { 
		return (m_nFlag & DTC_WEB_FUNC_ARG) ? TRUE : FALSE;		
	}	
	
	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &pOut);

	friend ostream &operator << (ostream &pOut, DtcDexLiveWeb *pWeb);		
};

#endif
