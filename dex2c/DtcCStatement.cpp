/*********************************************************************
*   DtcCStatement.cpp:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#include "DtcCStatement.h"

///////////////////////////////////////////////////////////////////////
// Static Function Declaration
///////////////////////////////////////////////////////////////////////

/**
 * This function will put the space to the output stream as many as 
 * 4 times of the given indent number.
 *
 * @param pOut output stream
 * @param nIndent the number of the indent to be inserted.
 */
static void 
InsertIndentSpace(ostream &pOut, uint32_t nIndent) {

#define DTC_TAB_CHAR	((char)(' '))
#define DTC_TAB_STRING	((char *)("    "))

	for(uint32_t i = 0; i < nIndent; i++ ) {
		
		pOut << DTC_TAB_STRING;
	}
}

///////////////////////////////////////////////////////////////////////
// DtcCStatement Class Definition
///////////////////////////////////////////////////////////////////////

DtcCStatement::DtcCStatement(DtcCStmtType_t nType /* = DTC_C_STMT_UNKNOWN */) {

	m_nType = nType;
}

DtcCStatement::~DtcCStatement(void) {
	// Do nothing
}

ostream &operator <<(ostream &pOut, DtcCStatement *pStmt) {
	
	uint32_t indent = 1;
	pStmt->Write(pOut, indent);
	return pOut;
}

///////////////////////////////////////////////////////////////////////
// DtcCAssignStmt Class Definition
///////////////////////////////////////////////////////////////////////

DtcCAssignStmt::DtcCAssignStmt(DtcCIdExpr *pIdExpr, DtcCExpression *pRhsExpr)
:DtcCStatement(DTC_C_STMT_ASSIGN) {

	m_pIdExpr = pIdExpr;

	m_pRhsExpr = pRhsExpr;
}

DtcCAssignStmt::~DtcCAssignStmt() {
	
	delete m_pRhsExpr;
	delete m_pIdExpr;
}

void
DtcCAssignStmt::Write(ostream &pOut, uint32_t &nIndent) {

	InsertIndentSpace(pOut, nIndent);

	if(m_pIdExpr != NULL) {

		pOut << m_pIdExpr << " = ";
	}

	pOut << m_pRhsExpr << ";" << endl;
}

///////////////////////////////////////////////////////////////////////
// DtcCBranchStmt Class Definition
///////////////////////////////////////////////////////////////////////

DtcCBranchStmt::DtcCBranchStmt(DtcCExpression *pCondExpr, DtcCIdExpr *pTargetExpr) 
:DtcCStatement(DTC_C_STMT_BRANCH) {

	m_pCondExpr = pCondExpr;

	m_pTargetExpr = pTargetExpr;
}
	
DtcCBranchStmt::~DtcCBranchStmt(void) {

	delete m_pTargetExpr;
	delete m_pCondExpr;
}
	
void
DtcCBranchStmt::Write(ostream &pOut, uint32_t &nIndent) {

	InsertIndentSpace(pOut, nIndent);

	pOut << "if(" << m_pCondExpr << ")\tgoto " << m_pTargetExpr << ";" << endl;
}
