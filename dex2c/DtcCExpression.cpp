/*********************************************************************
*   DtcCExpression.cpp:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#include "DtcCExpression.h"

///////////////////////////////////////////////////////////////////////
// Static Function Declaration
///////////////////////////////////////////////////////////////////////

/**
 * This function will return the type string of the expression specified by the given type ID. 
 *
 * @param nType the type of the expression
 * @return the type string
 */
static const char *
GetCExprString(DtcCExprType_t nType) {

	if(nType >= DTC_C_NUM_OF_EXPR) {
		return NULL;
	}

	static const char *exprTypeString[DTC_C_NUM_OF_EXPR] = {

		"UNK",		/* DTC_C_EXPR_UNKNOWN */
		"ID",		/* DTC_C_EXPR_ID */
		"-",			/* DTC_C_EXPR_UNARY_NEG */
		"!",			/* DTC_C_EXPR_UNARY_NOT */
		"|",			/* DTC_C_EXPR_UNARY_BOR */
		"&",			/* DTC_C_EXPR_UNARY_BAND */
		"(j_long)"	,	/* DTC_C_EXPR_UNARY_I2J */
		"(j_float)",	/* DTC_C_EXPR_UNARY_I2F */
		"(j_double)",	/* DTC_C_EXPR_UNARY_I2D */
		"(j_int)",		/* DTC_C_EXPR_UNARY_J2I */
		"(j_float)",	/* DTC_C_EXPR_UNARY_J2F */
		"(j_double)",	/* DTC_C_EXPR_UNARY_J2D */
		"(j_int)",		/* DTC_C_EXPR_UNARY_F2I */
		"(j_long)",	/* DTC_C_EXPR_UNARY_F2J */
		"(j_double)",	/* DTC_C_EXPR_UNARY_F2D */
		"(j_int)",		/* DTC_C_EXPR_UNARY_D2I */
		"(j_long)",	/* DTC_C_EXPR_UNARY_D2J */
		"(j_float)",	/* DTC_C_EXPR_UNARY_D2F */
		"+",			/* DTC_C_EXPR_BINARY_ADD */
		"-",			/* DTC_C_EXPR_BINARY_SUB */
		"*",			/* DTC_C_EXPR_BINARY_MUL */
		"/",			/* DTC_C_EXPR_BINARY_DIV */
		"%",			/* DTC_C_EXPR_BINARY_REM */
		"&",			/* DTC_C_EXPR_BINARY_AND */
		"|",			/* DTC_C_EXPR_BINARY_OR */
		"^",			/* DTC_C_EXPR_BINARY_XOR */
		"<<",		/* DTC_C_EXPR_BINARY_SHL */
		">>",		/* DTC_C_EXPR_BINARY_SHR */
		">>",		/* DTC_C_EXPR_BINARY_USHR */
		"==",		/* DTC_C_EXPR_BINARY_EQ */
		"!=",		/* DTC_C_EXPR_BINARY_NE */
		"<=",		/* DTC_C_EXPR_BINARY_LE */
		"<",			/* DTC_C_EXPR_BINARY_LT */
		">=",		/* DTC_C_EXPR_BINARY_GE */
		">",			/* DTC_C_EXPR_BINARY_GT */
		"CALL",		/* DTC_C_EXPR_CALL */
	};

	return exprTypeString[nType];
};

///////////////////////////////////////////////////////////////////////
// DtcCExpression Class Definition
///////////////////////////////////////////////////////////////////////

DtcCExpression::DtcCExpression(DtcCExprType_t nType /* = DTC_C_EXPR_UNKNOWN */) {

	m_nType = nType;
}

DtcCExpression::~DtcCExpression() {
	// Do nothing
}

ostream &operator << (ostream &pOut, DtcCExpression *pExpr) {
	pExpr->Write(pOut);
	return pOut;
}

///////////////////////////////////////////////////////////////////////
// DtcCIdExpr Class Definition
///////////////////////////////////////////////////////////////////////

DtcCIdExpr::DtcCIdExpr(DtcCSymbol *pSymbol /* = NULL */)
:DtcCExpression(DTC_C_EXPR_ID) {

	m_pSymbol = pSymbol;
}

DtcCIdExpr::~DtcCIdExpr(void) {

	delete m_pSymbol;
}

void 
DtcCIdExpr::Write( ostream &o ) {

	if(m_pSymbol != NULL) {
		
		o << m_pSymbol;
	}
}

///////////////////////////////////////////////////////////////////////
// DtcCUnaryExpr Class Definition
///////////////////////////////////////////////////////////////////////

DtcCUnaryExpr::DtcCUnaryExpr(DtcCExprType_t nType, DtcCExpression *pExpr) {

	m_nType = nType;

	m_pExpr = pExpr;
}

DtcCUnaryExpr::~DtcCUnaryExpr() {
	
	delete m_pExpr;
}

void
DtcCUnaryExpr::Write(ostream &pOut) {
	
	if(m_pExpr->GetType() != DTC_C_EXPR_ID) {
		
		pOut << "( ";
	}

	pOut << GetCExprString(m_nType);

	pOut << m_pExpr;

	if(m_pExpr->GetType() != DTC_C_EXPR_ID) {

		pOut << " )";
	}
}

///////////////////////////////////////////////////////////////////////
// DtcCBinaryExpr Class Definition
///////////////////////////////////////////////////////////////////////

DtcCBinaryExpr::DtcCBinaryExpr(DtcCExprType_t nType, DtcCExpression *pLhsExpr, DtcCExpression *pRhsExpr) {

	m_nType = nType;

	m_pLhsExpr = pLhsExpr;

	m_pRhsExpr = pRhsExpr;
}

DtcCBinaryExpr::~DtcCBinaryExpr(void) {

	delete m_pLhsExpr;
	delete m_pRhsExpr;
}

void
DtcCBinaryExpr::Write(ostream &pOut) {

	if(m_pLhsExpr == NULL || m_pRhsExpr == NULL) {
		
		pOut << "INVALID BINARY OP: " << GetCExprString(m_nType) << ends; 
		return;
	}
	
	pOut << m_pLhsExpr << " " << GetCExprString(m_nType) << " " << m_pRhsExpr;
}

///////////////////////////////////////////////////////////////////////
// DtcCCallExpr Class Definition
///////////////////////////////////////////////////////////////////////

DtcCCallExpr::DtcCCallExpr(DtcCIdExpr *pIdExpr)
:DtcCExpression(DTC_C_EXPR_CALL) {

	m_pIdExpr = pIdExpr;
}

DtcCCallExpr::~DtcCCallExpr() {

	delete m_pIdExpr;
}

void
DtcCCallExpr::Write(ostream &pOut) {

	pOut << m_pIdExpr << '(';

	DtcCExprList_t::iterator iter;

	for(iter = m_rParamExprList.begin(); iter != m_rParamExprList.end(); ) {

		pOut << *iter;

		iter++;

		if(iter != m_rParamExprList.end())	pOut << ", ";
	}

	pOut << ')';
}
