/*********************************************************************
*   DtcCBasicBlock.cpp:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#include "DtcDexcode.h"
#include "DtcCBasicBlock.h"

///////////////////////////////////////////////////////////////////////
// Static Function Definition
///////////////////////////////////////////////////////////////////////

static DtcCIdExpr *NewVarExpr(DtcCSymbolTable_t &rSymbolTable, DtcDexLiveWeb *pWeb);

///////////////////////////////////////////////////////////////////////
// Constructor & Destructor Definition
///////////////////////////////////////////////////////////////////////

/**
 * Constructor of DtcCBasicBlock class
 *
 * @param pDexBasicBlock the dex basic block to be translated into C code
 */
DtcCBasicBlock::DtcCBasicBlock(DtcDexBasicBlock *pDexBasicBlock) {

	// Check the validity of the given basic block
	if(pDexBasicBlock == NULL) {
		DTC_ERROR(DEX2C_TAG, "DtcCBasicBlock - Null dex basic block");
	}

	m_pDexBasicBlock = pDexBasicBlock;

	m_rStmtList.clear();
}

DtcCBasicBlock::~DtcCBasicBlock() {
	delete m_pDexBasicBlock;
	m_rStmtList.clear();
}

///////////////////////////////////////////////////////////////////////
// Member Function Definition
///////////////////////////////////////////////////////////////////////

/**
 * This funciton will be used to simply the process making a new ID expression
 * from a live web.
 *
 * @param rSymbolTable a symbol table
 * @param pWeb a live web
 * @return a new ID expression
 */
static DtcCIdExpr *
NewVarExpr(DtcCSymbolTable_t &rSymbolTable, DtcDexLiveWeb *pWeb) {

	if(pWeb == NULL) return NULL;

	DtcCVarSymbol *sym = new DtcCVarSymbol(pWeb);			

	if(rSymbolTable.count(sym->GetNameString()) == 0) {			
		rSymbolTable[sym->GetNameString()] = sym;		
	}													

	return new DtcCIdExpr(sym);	
}

/**
 * This function will translate a Dex basic block into a C basic block.
 *
 * @return error information
 */
DtcError_t
DtcCBasicBlock::Translate(DtcCSymbolTable_t &rSymbolTable) {

	DtcDexcode *code = NULL;
	DtcCStatement *stmt = NULL;
	DecodedInstruction *codeInfo = NULL;
	
	DtcDexcodeList_t::iterator iter;
	DtcDexcodeList_t &codes = m_pDexBasicBlock->GetDexcodes();

	m_rStmtList.clear();

	///////////////////////////////////////////////////////////////////////
	// Macros for simply building up the C IR 
	///////////////////////////////////////////////////////////////////////	

#define DEF_WEB(INDEX)					code->GetDefWeb(INDEX)
#define USE_WEB(INDEX)					code->GetUseWeb(INDEX)

#define NEW_VAR_EXPR(WEB)				NewVarExpr(rSymbolTable, WEB)
#define NEW_FUNC_EXPR(NAME)			(new DtcCIdExpr(new DtcCFuncSymbol(NAME)))
#define NEW_CONST_EXPR(TYPE, H32, L32)	(new DtcCIdExpr(new DtcCConstSymbol(TYPE, H32, L32)))
#define NEW_LABEL_EXPR(LABEL)			(new DtcCIdExpr(new DtcCLabelSymbol(LABEL)))

	// Translate each dex code into a C statement IR
	for(iter = codes.begin(); iter != codes.end(); iter++) {

		codeInfo = (code = *iter)->GetDecodedInfo();

		switch(codeInfo->opcode) {

			// kFmt21s
			case OP_CONST_WIDE_16: {

				DtcDexLiveWeb *web = DEF_WEB(0);
				stmt = new DtcCAssignStmt(
					NEW_VAR_EXPR(DEF_WEB(0)),
					NEW_CONST_EXPR(web->GetDataType(), 0, codeInfo->vB));
				break;
			}

			// kFmt12x
			case OP_INT_TO_DOUBLE: {

				stmt = new DtcCAssignStmt(
					NEW_VAR_EXPR(DEF_WEB(0)),
					new DtcCUnaryExpr(DTC_C_EXPR_UNARY_I2D, 
						NEW_VAR_EXPR(USE_WEB(0))));
				break;
			}

			// kFmt23x
#define case_OP_BINARY_ARITH(OP)							\
			case OP_##OP##_INT:							\
			case OP_##OP##_LONG:							\
			case OP_##OP##_FLOAT:							\
			case OP_##OP##_DOUBLE: 						\
				stmt = new DtcCAssignStmt(					\
					NEW_VAR_EXPR(DEF_WEB(0)),			\
					new DtcCBinaryExpr(						\
						DTC_C_EXPR_BINARY_##OP,			\
						NEW_VAR_EXPR(USE_WEB(0)),		\
						NEW_VAR_EXPR(USE_WEB(1))));		\
				break													

#define case_OP_BINARY_BITWISE(OP)							\
			case OP_##OP##_INT:							\
			case OP_##OP##_LONG:							\
				stmt = new DtcCAssignStmt(					\
					NEW_VAR_EXPR(DEF_WEB(0)),			\
					new DtcCBinaryExpr(						\
						DTC_C_EXPR_BINARY_##OP,			\
						NEW_VAR_EXPR(USE_WEB(0)),		\
						NEW_VAR_EXPR(USE_WEB(1))));		\
				break													

			case_OP_BINARY_ARITH(ADD);
			case_OP_BINARY_ARITH(SUB);
			case_OP_BINARY_ARITH(MUL);
			case_OP_BINARY_ARITH(DIV);
			case_OP_BINARY_ARITH(REM);
			
			case_OP_BINARY_BITWISE(AND);
			case_OP_BINARY_BITWISE(OR);
			case_OP_BINARY_BITWISE(XOR);
			case_OP_BINARY_BITWISE(SHL);
			case_OP_BINARY_BITWISE(SHR);			
			case_OP_BINARY_BITWISE(USHR);			

			// kFmt23x
			case OP_CMPG_DOUBLE: {

				DtcCCallExpr *callExpr = new DtcCCallExpr(NEW_FUNC_EXPR("cmpg_double"));

				callExpr->AppendFuncParam(NEW_VAR_EXPR(USE_WEB(0)));
				callExpr->AppendFuncParam(NEW_VAR_EXPR(USE_WEB(1)));
				
				stmt = new DtcCAssignStmt(NEW_VAR_EXPR(DEF_WEB(0)), callExpr);
				break;
			}
				
			// kFmt21t
			case OP_IF_GEZ: {

				uint32_t targetAddress = code->GetInsnIndex() + (int32_t)codeInfo->vB;

				stmt = new DtcCBranchStmt(
					new DtcCBinaryExpr(
						DTC_C_EXPR_BINARY_GE,
						NEW_VAR_EXPR(USE_WEB(0)),
						NEW_CONST_EXPR(J_INT, 0, 0)),
					NEW_LABEL_EXPR(targetAddress));
				
				break;
			}

			// kFmt21c
			case OP_NEW_INSTANCE: {

				DtcCCallExpr *callExpr = new DtcCCallExpr(NEW_FUNC_EXPR("new_instance"));

				// The constant expression must be replaced with the type string from the type index.
				callExpr->AppendFuncParam(NEW_CONST_EXPR(J_INT, 0, codeInfo->vB));

				stmt = new DtcCAssignStmt(NEW_VAR_EXPR(DEF_WEB(0)), callExpr);
				break;
			}			

			// kFmt35c
			case OP_INVOKE_DIRECT: {

				DtcCCallExpr *callExpr = new DtcCCallExpr(NEW_FUNC_EXPR("invoke_direct"));

				// The constant expression must be replaced with the type string from the type index.
				callExpr->AppendFuncParam(NEW_CONST_EXPR(J_INT, 0, codeInfo->vB));

				// Function arguments
				for(uint32_t i = 0; i < (int32_t)codeInfo->vA; i++) {
					callExpr->AppendFuncParam(NEW_VAR_EXPR(USE_WEB(i)));
				}
				
				stmt = new DtcCAssignStmt(NEW_VAR_EXPR(DEF_WEB(0)), callExpr);
				break;
			}				

			default:
				return DTC_SUCCESS;
		}

		m_rStmtList.push_back(stmt);
	}

	return DTC_SUCCESS;
}

/**
 * This function will print out the whole code of the translated basic block.
 *
 * @param pOut output stream
 */
void
DtcCBasicBlock::Write(ostream &pOut) {

	char buf[STR_LEN];
	sprintf(buf, ".L%08X:\n", m_pDexBasicBlock->GetStartAddress());	
	pOut << buf;

	DtcCStmtList_t::iterator iter;
	
	for(iter = m_rStmtList.begin(); iter != m_rStmtList.end(); iter++) {

		pOut << *iter << '\n';
	}
}

ostream &operator << (ostream &pOut, DtcCBasicBlock *pBlock) {
	pBlock->Write(pOut);
	return pOut;
}
