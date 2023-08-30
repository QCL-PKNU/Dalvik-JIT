/*********************************************************************
*   DtcDexBasicBlock.cpp:
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
#include "DtcDexBasicBlock.h"

///////////////////////////////////////////////////////////////////////
// Constructor & Destructor Definition
///////////////////////////////////////////////////////////////////////

DtcDexBasicBlock::DtcDexBasicBlock(const uint16_t *pInsns, const uint8_t *pFlags, uint32_t nSpc, uint32_t nEpc) {

	m_nStartAddress = nSpc;

	m_pFlags = (const uint8_t *)(pFlags + nSpc);

	m_pInsns = (const uint16_t *)(pInsns + nSpc);

	m_nInsnsSize = nEpc - nSpc + 1;

	m_rDexcodes.clear();

	DTC_DEBUG(DEX2C_TAG, "Basic block: spc - %04X, epc - %04X", nSpc, nEpc);
}

DtcDexBasicBlock::~DtcDexBasicBlock() {
	// Do nothing
}

///////////////////////////////////////////////////////////////////////
// Member Function Definition
///////////////////////////////////////////////////////////////////////

/**
 * This function will resolve the dex codes of the basic block and 
 * return the number of codes.
 *
 * @return the number of dex codes in the basic block
 */
uint32_t 
DtcDexBasicBlock::ResolveDexcodes() {

	DTC_DEBUG(DEX2C_TAG, "DtcDexBasicBlock->ResolveDexcodes Invoked...");

	m_rDexcodes.clear();

	// Get the raw instructions of the basic block
	const uint16_t *insns = m_pInsns;
	uint32_t insnsSize = m_nInsnsSize;
	uint32_t insnsWidth = 0;

	// Resolve the dex codes from the raw instructions
	for(uint32_t i = 0; i < insnsSize; i += insnsWidth) {

		DTC_DEBUG(DEX2C_TAG, "(%04d) %s", i, dexGetOpcodeName((Opcode)((*insns) & 0xFF)));
		
		m_rDexcodes.push_back(new DtcDexcode(insns, m_nStartAddress + i));

		// Get the next instruction
		insnsWidth = dexGetWidthFromInstruction(insns);
		insns += insnsWidth;
	}

	return (uint32_t)m_rDexcodes.size();
}

/**
 * This function will perform the local liveness analysis in the scope of a basic block.
 *
 * @param rLocalVarTable the list of resolved local variables
 * @return error information
 */
DtcError_t 
DtcDexBasicBlock::AnalyseLiveness(DtcDexLiveWebTable_t &rLocalVarTable) {

	DTC_DEBUG(DEX2C_TAG, "DtcDexBasicBlock->AnalyseLiveness Invoked...");

	DtcError_t error;

	DtcDexcode *dexcode = NULL;
	DtcDexcode *prevcode = NULL;

	DecodedInstruction *decinsn = NULL;

	// Live web table
	DtcDexLiveWebTable_t liveWebTable;

	if(m_rDexcodes.size() == 0) {
		
		DTC_ERROR(DEX2C_TAG, "AnalyseLiveness - Dexcodes are not resolved yet.");
		return DTC_ERROR_RESOLVE_DEXCODES;
	}

	liveWebTable.clear();
	
	/////////////////////////////////////////////////////////////////
	// Macros for resolving the dexcodes
	/////////////////////////////////////////////////////////////////

#define APPEND_DEF_WEB(DEXCODE, REGNO)				\
do {														\
	uint32_t regno = REGNO;								\
	DtcDexLiveWeb *web = NULL;							\
	if(rLocalVarTable.count(regno) > 0) {					\
		web = rLocalVarTable[regno];						\
	}													\
	else {												\
		web = new DtcDexLiveWeb(regno);					\
		liveWebTable[regno] = web;							\
	}													\
	DEXCODE->AppendDefWeb(web);						\
} while(0)

#define APPEND_USE_WEB(DEXCODE, REGNO)				\
do {														\
	uint32_t regno = REGNO;								\
	DtcDexLiveWeb *web = NULL;							\
	if(rLocalVarTable.count(regno) > 0) {					\
		web = rLocalVarTable[regno];						\
	}													\
	else if(liveWebTable.count(regno) > 0) {					\
		web = liveWebTable[regno];							\
	}													\
	else {												\
		web = new DtcDexLiveWeb(regno);					\
		liveWebTable[regno] = web;							\
	}													\
	DEXCODE->AppendUseWeb(web);						\
} while(0)

	/////////////////////////////////////////////////////////////////
	// Resolve each of the dexcodes      			
	/////////////////////////////////////////////////////////////////

	DtcDexcodeList_t::iterator iter;
	
	for(iter = m_rDexcodes.begin(); iter != m_rDexcodes.end(); iter++) {

		decinsn = (dexcode = *iter)->GetDecodedInfo();

		//DTC_DEBUG(DEX2C_TAG, ">> Opcode(%s):", dexGetOpcodeName(decinsn->opcode));

		switch(decinsn->opcode) {

			// 00: nop ------------------------------------------
			case OP_NOP: 
			case OP_RETURN_VOID: case OP_RETURN_VOID_BARRIER:
			case OP_GOTO: case OP_GOTO_16: case OP_GOTO_32: {
				break;
			}
			
			// 01: move A, B ------------------------------------------
			case OP_MOVE: case OP_MOVE_16: case OP_MOVE_FROM16: 
			case OP_MOVE_WIDE: case OP_MOVE_WIDE_16: case OP_MOVE_WIDE_FROM16: 
			case OP_MOVE_OBJECT: case OP_MOVE_OBJECT_16: case OP_MOVE_OBJECT_FROM16: 
			// 07: instance-of vA, vB, @vC ---------------------------------
			case OP_INSTANCE_OF: 
			// 08: array-length vA, vB -------------------------------------
			case OP_ARRAY_LENGTH: 			
			// 10: new-array vA, vB, @vC ----------------------------------
			case OP_NEW_ARRAY: 
			// 21: iget vA, vB, @vC ----------------------------------------
			case OP_IGET: case OP_IGET_QUICK: case OP_IGET_WIDE_QUICK:
			case OP_IGET_WIDE: case OP_IGET_OBJECT: case OP_IGET_BOOLEAN: 
			case OP_IGET_BYTE: case OP_IGET_CHAR: case OP_IGET_SHORT:
			// 27: unary-op vA, vB ----------------------------------------
			case OP_NEG_INT: case OP_NOT_INT: case OP_NEG_LONG:
			case OP_NOT_LONG: case OP_NEG_FLOAT: case OP_NEG_DOUBLE:
			case OP_INT_TO_LONG: case OP_INT_TO_FLOAT: case OP_INT_TO_DOUBLE:
			case OP_LONG_TO_INT: case OP_LONG_TO_FLOAT: case OP_LONG_TO_DOUBLE:
			case OP_FLOAT_TO_INT: case OP_FLOAT_TO_LONG: case OP_FLOAT_TO_DOUBLE:
			case OP_DOUBLE_TO_INT: case OP_DOUBLE_TO_LONG: case OP_DOUBLE_TO_FLOAT:
			case OP_INT_TO_BYTE: case OP_INT_TO_CHAR: case OP_INT_TO_SHORT:
			// 30: binary-op/lit16 vA, vB, #+vC ------------------------------
			case OP_ADD_INT_LIT16: case OP_RSUB_INT: case OP_MUL_INT_LIT16:
			case OP_DIV_INT_LIT16: case OP_REM_INT_LIT16: case OP_AND_INT_LIT16:
			case OP_OR_INT_LIT16: case OP_XOR_INT_LIT16:		
			// 30: binary-op/lit8 vA, vB, #+vC ------------------------------
			case OP_ADD_INT_LIT8: case OP_RSUB_INT_LIT8: case OP_MUL_INT_LIT8:
			case OP_DIV_INT_LIT8: case OP_REM_INT_LIT8: case OP_AND_INT_LIT8:
			case OP_OR_INT_LIT8: case OP_XOR_INT_LIT8: case OP_SHL_INT_LIT8:
			case OP_SHR_INT_LIT8: case OP_USHR_INT_LIT8: {
				
				// def: vA, use: vB
				APPEND_USE_WEB(dexcode, decinsn->vB);
				APPEND_DEF_WEB(dexcode, decinsn->vA);
				break;
			}
			
			// 04: const vA, #+vB ----------------------------------------
			case OP_CONST: case OP_CONST_4: case OP_CONST_16:
			case OP_CONST_HIGH16: case OP_CONST_WIDE: case OP_CONST_WIDE_16:
			case OP_CONST_WIDE_32: case OP_CONST_WIDE_HIGH16: case OP_CONST_STRING:
			case OP_CONST_STRING_JUMBO: case OP_CONST_CLASS: 				
			// 09: new-instance vA, @vB ----------------------------------
			case OP_NEW_INSTANCE: 
			// 23: sget vA, @vB ------------------------------------------	
			case OP_SGET: case OP_SGET_WIDE: case OP_SGET_OBJECT:
			case OP_SGET_BOOLEAN: case OP_SGET_BYTE: case OP_SGET_CHAR:
			case OP_SGET_SHORT: {
				
				// def: vA
				APPEND_DEF_WEB(dexcode, decinsn->vA);
				break;
			}		
			
			// 02: move-result A -----------------------------------------
			case OP_MOVE_RESULT: case OP_MOVE_RESULT_WIDE: 
			case OP_MOVE_RESULT_OBJECT: case OP_MOVE_EXCEPTION: {

				// Since we are going to perform the local liveness analysis in a basic block scope,
				// some of the dex codes might not have the previous code.
				if(prevcode != NULL) {				
					// def: vA (def of the previous code)
					APPEND_DEF_WEB(prevcode, decinsn->vA);	
				}

				// The use web will be used for resolving the data type. 
				APPEND_USE_WEB(dexcode, decinsn->vA);
				break;
			}
			
			// 03: return A ----------------------------------------------
			case OP_RETURN: case OP_RETURN_WIDE: case OP_RETURN_OBJECT: 
			// 05: monitor vA --------------------------------------------
			case OP_MONITOR_ENTER: case OP_MONITOR_EXIT: 
			// 06: check-cast vA, @vB -------------------------------------
			case OP_CHECK_CAST: 
			// 13: fill-array-data vA, +vB ----------------------------------
			case OP_FILL_ARRAY_DATA:
			// 14: throw vA ----------------------------------------------
			case OP_THROW:
			// 15: switch vA, +vB -----------------------------------------
			case OP_PACKED_SWITCH: case OP_SPARSE_SWITCH:
			// 18: if[-testz] vA, +vB ---------------------------------------		
			case OP_IF_EQZ: case OP_IF_NEZ: case OP_IF_LTZ:
			case OP_IF_GEZ: case OP_IF_GTZ: case OP_IF_LEZ:
			// 24: sput vA, @vB -------------------------------------------	
			case OP_SPUT: case OP_SPUT_WIDE: case OP_SPUT_OBJECT:
			case OP_SPUT_BOOLEAN: case OP_SPUT_BYTE: case OP_SPUT_CHAR:
			case OP_SPUT_SHORT:	 {
				
				// use: vA
				APPEND_USE_WEB(dexcode, decinsn->vA);
				break;
			}
			
			// 11: filled-new-array vA, @vB, {vC, vD, vE, vF, vG} --------------
			case OP_FILLED_NEW_ARRAY: 
			// 25: invoke[-kind] vA, @vB, {vC, vD, vE, vF, vG} ----------------
			case OP_INVOKE_VIRTUAL: case OP_INVOKE_SUPER: case OP_INVOKE_DIRECT:
			case OP_INVOKE_VIRTUAL_QUICK: case OP_INVOKE_SUPER_QUICK:
			case OP_INVOKE_STATIC: case OP_INVOKE_INTERFACE: {
				
				// use: vA registers among {vC, vD, vE, vF, vG}
				for(uint32_t j = 0; j < (int32_t)decinsn->vA; j++) {
					APPEND_USE_WEB(dexcode, decinsn->arg[j]);
				}
				break;
			}
			
			// 12: filled-new-array/range vA, @vB, {vC ... vN} -----------------
			case OP_FILLED_NEW_ARRAY_RANGE: 
			// 26: invoke[-kind]/range vA, @vB, {vC ... vN} -------------------
			case OP_INVOKE_VIRTUAL_RANGE: case OP_INVOKE_SUPER_RANGE:
			case OP_INVOKE_VIRTUAL_QUICK_RANGE: case OP_INVOKE_SUPER_QUICK_RANGE:
			case OP_INVOKE_DIRECT_RANGE: case OP_INVOKE_STATIC_RANGE:
			case OP_INVOKE_INTERFACE_RANGE: {
				
				// use: vA registers among {vC ... vN}
				for(uint32_t j = 0; j < decinsn->vA; j++) {
					APPEND_USE_WEB(dexcode, decinsn->vC + j);
				}
				break;
			}		
			
			// 16: cmp[-kind] vA, vB, vC -----------------------------------	
			case OP_CMPL_FLOAT: case OP_CMPG_FLOAT: case OP_CMPL_DOUBLE:
			case OP_CMPG_DOUBLE: case OP_CMP_LONG:
			// 19: aget vA, vB, vC ----------------------------------------
			case OP_AGET: 
			case OP_AGET_WIDE: case OP_AGET_OBJECT: case OP_AGET_BOOLEAN:
			case OP_AGET_BYTE: case OP_AGET_CHAR: case OP_AGET_SHORT:
			// 28: binary-op vA, vB, vC ------------------------------------
			case OP_ADD_INT: case OP_SUB_INT: case OP_MUL_INT:
			case OP_DIV_INT: case OP_REM_INT: case OP_AND_INT:
			case OP_OR_INT: case OP_XOR_INT: case OP_SHL_INT:
			case OP_SHR_INT: case OP_USHR_INT: 
				
			case OP_ADD_LONG: case OP_SUB_LONG: case OP_MUL_LONG: 
			case OP_DIV_LONG: case OP_REM_LONG: case OP_AND_LONG: 
			case OP_OR_LONG: case OP_XOR_LONG: case OP_SHL_LONG: 
			case OP_SHR_LONG: case OP_USHR_LONG: case OP_ADD_FLOAT:
			case OP_SUB_FLOAT: case OP_MUL_FLOAT: case OP_DIV_FLOAT:
			case OP_REM_FLOAT:
	
			case OP_ADD_DOUBLE: case OP_SUB_DOUBLE: case OP_MUL_DOUBLE:
			case OP_DIV_DOUBLE: case OP_REM_DOUBLE: {
	
				// def: vA, use: vB, vC
				APPEND_USE_WEB(dexcode, decinsn->vB);
				APPEND_USE_WEB(dexcode, decinsn->vC);
				APPEND_DEF_WEB(dexcode, decinsn->vA);
				break;
			}
			
			// 29: binary-op/2addr vA, vB ----------------------------------
			case OP_ADD_INT_2ADDR: case OP_SUB_INT_2ADDR: case OP_MUL_INT_2ADDR:
			case OP_DIV_INT_2ADDR: case OP_REM_INT_2ADDR: case OP_AND_INT_2ADDR:
			case OP_OR_INT_2ADDR: case OP_XOR_INT_2ADDR: case OP_SHL_INT_2ADDR:
			case OP_SHR_INT_2ADDR: case OP_USHR_INT_2ADDR:
	
			case OP_ADD_LONG_2ADDR: case OP_SUB_LONG_2ADDR: case OP_MUL_LONG_2ADDR:
			case OP_DIV_LONG_2ADDR: case OP_REM_LONG_2ADDR: case OP_AND_LONG_2ADDR:
			case OP_OR_LONG_2ADDR: case OP_XOR_LONG_2ADDR: case OP_SHL_LONG_2ADDR:
			case OP_SHR_LONG_2ADDR: case OP_USHR_LONG_2ADDR:
				
			case OP_ADD_FLOAT_2ADDR: case OP_SUB_FLOAT_2ADDR: case OP_MUL_FLOAT_2ADDR:
			case OP_DIV_FLOAT_2ADDR: case OP_REM_FLOAT_2ADDR:
	
			case OP_ADD_DOUBLE_2ADDR: case OP_SUB_DOUBLE_2ADDR: case OP_MUL_DOUBLE_2ADDR:
			case OP_DIV_DOUBLE_2ADDR: case OP_REM_DOUBLE_2ADDR: {
	
				// def: vA, use: vA, vB
				APPEND_USE_WEB(dexcode, decinsn->vA);
				APPEND_USE_WEB(dexcode, decinsn->vB);
				APPEND_DEF_WEB(dexcode, decinsn->vA);
				break;
			}					
			
			// 17: if[-test] vA, vB, +vC -------------------------------------
			case OP_IF_EQ: case OP_IF_NE: case OP_IF_LT:
			case OP_IF_GE: case OP_IF_GT: case OP_IF_LE:
			// 22: iput vA, vB, @vC ---------------------------------------
			case OP_IPUT: case OP_IPUT_QUICK: case OP_IPUT_WIDE_QUICK:
			case OP_IPUT_WIDE: case OP_IPUT_OBJECT: case OP_IPUT_BOOLEAN:
			case OP_IPUT_BYTE: case OP_IPUT_CHAR: case OP_IPUT_SHORT: {
	
				// use: vA, vB
				APPEND_USE_WEB(dexcode, decinsn->vA);
				APPEND_USE_WEB(dexcode, decinsn->vB);
				break;
			}
			
			// 20: aput vA, vB, vC ----------------------------------------
			case OP_APUT:
			case OP_APUT_WIDE: case OP_APUT_OBJECT: case OP_APUT_BOOLEAN:
			case OP_APUT_BYTE: case OP_APUT_CHAR: case OP_APUT_SHORT: {
	
				// use: vA, vB, vC
				APPEND_USE_WEB(dexcode, decinsn->vA);
				APPEND_USE_WEB(dexcode, decinsn->vB);
				APPEND_USE_WEB(dexcode, decinsn->vC);
				break;
			}
			
			default: {
				DTC_ERROR(DEX2C_TAG, "Unknown opcode: %s", dexGetOpcodeName(decinsn->opcode));
				return DTC_ERROR_UNKNOWN_OPCODE;
			}
		}
			
		// Resolve the data types of the registers being employed by the dex code
		if((error = dexcode->ResolveDataTypes()) != DTC_SUCCESS) {
			return error;
		}
	
		// Keep the previous dex code for the further use.
		prevcode = dexcode;		
	}
	
	return DTC_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
// Debugging Function Definition
///////////////////////////////////////////////////////////////////////

/**
 * This function will print out all of the resolved dex codes.
 *
 * @param pOut output stream
 */

void
DtcDexBasicBlock::Write(ostream &pOut) {

	DtcDexcodeList_t::iterator iter;

	char buf[BUF_LEN];
	sprintf(buf, "0x%04X", m_nStartAddress);

	pOut << "## BasicBlock [" << buf << "] #######\n";
		
	for(iter = m_rDexcodes.begin(); iter != m_rDexcodes.end(); iter++) {

		pOut << '\t' << *iter << '\n';
	}
}

ostream &operator << (ostream &pOut, DtcDexBasicBlock *pBlock) {
	
	pBlock->Write(pOut);
	return pOut;
}
