/*********************************************************************
*   DtcCBasicBlock.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#ifndef _DTC_C_BASIC_BLOCK_H_
#define _DTC_C_BASIC_BLOCK_H_

#include "DtcCommon.h"
#include "DtcDexBasicBlock.h"
#include "DtcCStatement.h"
#include "DtcCExpression.h"
#include "DtcCSymbol.h"

///////////////////////////////////////////////////////////////////////
// User-defined Type Definition
///////////////////////////////////////////////////////////////////////

typedef vector<DtcCStatement *> DtcCStmtList_t;

typedef map<const char *, DtcCSymbol *> DtcCSymbolTable_t;

///////////////////////////////////////////////////////////////////////
// DtcCBasicBlock Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcCBasicBlock {
	
private :
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////

	DtcDexBasicBlock *m_pDexBasicBlock;

	// A list of the C statements
	DtcCStmtList_t m_rStmtList;

public :
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcCBasicBlock(DtcDexBasicBlock *pDexBasicBlock);
	
	virtual ~DtcCBasicBlock();

	///////////////////////////////////////////////////////////////////
	// Membuer Function Declaration
	///////////////////////////////////////////////////////////////////

	DtcError_t Translate(DtcCSymbolTable_t &rSymbolTable);
	
	void Write(ostream &pOut);

	friend ostream &operator << (ostream &pOut, DtcCBasicBlock *pBlock);	
};

#endif
