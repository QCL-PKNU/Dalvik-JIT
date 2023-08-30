/*********************************************************************
*   DtcDexcode.cpp:
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

///////////////////////////////////////////////////////////////////////
// Constructor & Destructor Definition
///////////////////////////////////////////////////////////////////////
 
DtcDexcode::DtcDexcode(const uint16_t *pInsn, uint32_t nInsnIndex) {

	m_pDecodedInsn = (DecodedInstruction *)malloc(sizeof(struct DecodedInstruction));

	if(m_pDecodedInsn == NULL) {
		DTC_ERROR(DEX2C_TAG, "DtcDexcode - Invalid memory allocation");
		return;
	}

	// Decode the given dex instruction
	dexDecodeInstruction(pInsn, m_pDecodedInsn);

	// Clear the def & use webs
	m_rDefWebs.clear();
	m_rUseWebs.clear();

	// Set the instruction index from the start address of the whole method
	m_nInsnIndex = nInsnIndex;
}

DtcDexcode::~DtcDexcode() {
	
	m_rDefWebs.clear();
	m_rUseWebs.clear();

	delete m_pDecodedInsn;
}

///////////////////////////////////////////////////////////////////////
// Member Function Definition
///////////////////////////////////////////////////////////////////////

/**
 * This function will be used to resolve the data types of the registers for the dex code.
 *
 * @return error information
 */
DtcError_t 
DtcDexcode::ResolveDataTypes() {

	//DTC_DEBUG(DEX2C_TAG, "DtcDexcode->ResolveDataTypes Invoked...");

	///////////////////////////////////////////////////////////////////
	// Macros for resolving the data types
	///////////////////////////////////////////////////////////////////

#define SET_DEF_TYPE(INDEX, TYPE)				\
do {												\
	DtcDexLiveWeb *web = GetDefWeb(INDEX);		\
	if(web != NULL) { 								\
		if(!web->GetIsLocal())						\
			web->SetDataType(TYPE);				\
	}											\
	else return DTC_ERROR_INVALID_DEF_WEB;		\
} while(0)

#define SET_USE_TYPE(INDEX, TYPE)				\
do {												\
	DtcDexLiveWeb *web = GetUseWeb(INDEX);		\
	if(web != NULL) { 								\
		if(!web->GetIsLocal())						\
			web->SetDataType(TYPE);				\
	}											\
	else return DTC_ERROR_INVALID_USE_WEB;		\
} while(0)

#define GET_USE_TYPE(INDEX, TYPE)				\
do {												\
	DtcDexLiveWeb *web = GetUseWeb(INDEX);		\
	if(web != NULL) TYPE = web->GetDataType();		\
	else return DTC_ERROR_INVALID_USE_WEB;		\
} while(0)
		
	///////////////////////////////////////////////////////////////////
	// Resolve the data types of the registers employed by the dexcode 
	///////////////////////////////////////////////////////////////////

	switch(m_pDecodedInsn->opcode) {

		// def: usetype of use
		case OP_MOVE: case OP_MOVE_16: case OP_MOVE_FROM16:
		case OP_MOVE_WIDE: case OP_MOVE_WIDE_16: case OP_MOVE_WIDE_FROM16: {
			DtcDexDataType_t type;
			GET_USE_TYPE(0, type); SET_DEF_TYPE(0, type);
			break;
		}
		
		// def: object, use: object
		case OP_MOVE_OBJECT:
		case OP_MOVE_OBJECT_16:
		case OP_MOVE_OBJECT_FROM16: {
			SET_USE_TYPE(0, J_OBJECT); SET_DEF_TYPE(0, J_OBJECT); 
			break;
		}
		
		// use: object
		case OP_MOVE_RESULT_OBJECT: case OP_MOVE_EXCEPTION: case OP_RETURN_OBJECT:
		case OP_CONST_STRING: case OP_CONST_STRING_JUMBO: case OP_CONST_CLASS:
		case OP_MONITOR_ENTER: case OP_MONITOR_EXIT: case OP_CHECK_CAST:
		case OP_THROW: {
			SET_USE_TYPE(0, J_OBJECT); 
			break;
		}
		
		// def: int, use: object
		case OP_INSTANCE_OF:
		case OP_ARRAY_LENGTH: {
			SET_USE_TYPE(0, J_OBJECT); SET_DEF_TYPE(0, J_INT); 
			break;
		}

		// def: object
		case OP_NEW_INSTANCE: {
			SET_DEF_TYPE(0, J_OBJECT); 
			break;
		}

		// def: object, use: int
		case OP_NEW_ARRAY: {
			SET_USE_TYPE(0, J_INT); SET_DEF_TYPE(0, J_OBJECT); 
			break;
		}

		// def: int, use: float, float
		case OP_CMPL_FLOAT:
		case OP_CMPG_FLOAT: {
			SET_USE_TYPE(0, J_FLOAT); SET_USE_TYPE(1, J_FLOAT); SET_DEF_TYPE(0, J_INT); 
			break;
		}

		// def:int, use: double, double
		case OP_CMPL_DOUBLE:
		case OP_CMPG_DOUBLE: {
			SET_USE_TYPE(0, J_DOUBLE); SET_USE_TYPE(1, J_DOUBLE); SET_DEF_TYPE(0, J_INT);
			break;
		}

		// def:int, use: long, long
		case OP_CMP_LONG: {	
			SET_USE_TYPE(0, J_LONG); SET_USE_TYPE(1, J_LONG); SET_DEF_TYPE(0, J_INT);
			break;
		}

		// def: type, use: object, int
#define case_OP_XGET(TYPE)						\
		case OP_AGET_##TYPE: 					\
		case OP_IGET_##TYPE: 					\
			SET_USE_TYPE(0, J_OBJECT);			\
			SET_USE_TYPE(1, J_INT);				\
			SET_DEF_TYPE(0, J_##TYPE);			\
			break								

		case_OP_XGET(OBJECT);
		case_OP_XGET(BOOLEAN);
		case_OP_XGET(BYTE);
		case_OP_XGET(CHAR);
		case_OP_XGET(SHORT);
		
		// use: type, object, int
#define case_OP_XPUT(TYPE)						\
		case OP_APUT_##TYPE: 					\
		case OP_IPUT_##TYPE: 					\
			SET_USE_TYPE(0, J_##TYPE);			\
			SET_USE_TYPE(1, J_OBJECT);			\
			SET_USE_TYPE(2, J_INT);				\
			break	
			
		case_OP_XPUT(OBJECT);
		case_OP_XPUT(BOOLEAN);
		case_OP_XPUT(BYTE);
		case_OP_XPUT(CHAR);
		case_OP_XPUT(SHORT);		

		// def: type
#define case_OP_SGET(TYPE)						\
		case OP_SGET_##TYPE: 					\
			SET_DEF_TYPE(0, J_##TYPE);			\
			break								

		case_OP_SGET(OBJECT);
		case_OP_SGET(BOOLEAN);
		case_OP_SGET(BYTE);
		case_OP_SGET(CHAR);
		case_OP_SGET(SHORT);

		// use: type
#define case_OP_SPUT(TYPE)						\
		case OP_SPUT_##TYPE: 					\
			SET_USE_TYPE(0, J_##TYPE);			\
			break		
			
		case_OP_SPUT(OBJECT);
		case_OP_SPUT(BOOLEAN);
		case_OP_SPUT(BYTE);
		case_OP_SPUT(CHAR);
		case_OP_SPUT(SHORT);	
		
		// def: unknown, use: object, int
		case OP_AGET: case OP_APUT: case OP_AGET_WIDE: case OP_APUT_WIDE: 
		case OP_IGET: case OP_IPUT: case OP_IGET_WIDE: case OP_IPUT_WIDE: {			
			SET_USE_TYPE(0, J_OBJECT); SET_USE_TYPE(1, J_INT);
			break;
		}

		// def: int, use: int, int
		case OP_ADD_INT: case OP_SUB_INT: case OP_MUL_INT: case OP_DIV_INT: 
		case OP_REM_INT: case OP_AND_INT: case OP_OR_INT: case OP_XOR_INT: 
		case OP_SHL_INT: case OP_USHR_INT: {
			SET_USE_TYPE(0, J_INT); SET_USE_TYPE(1, J_INT); SET_DEF_TYPE(0, J_INT);
			break;			
		}			

		// def: int, use: int
		case OP_NEG_INT: case OP_NOT_INT: 

		case OP_ADD_INT_2ADDR: case OP_SUB_INT_2ADDR: case OP_MUL_INT_2ADDR: 
		case OP_DIV_INT_2ADDR: case OP_REM_INT_2ADDR: case OP_AND_INT_2ADDR: 
		case OP_OR_INT_2ADDR: case OP_XOR_INT_2ADDR: case OP_SHL_INT_2ADDR: 
		case OP_USHR_INT_2ADDR:

		case OP_ADD_INT_LIT16: case OP_RSUB_INT: case OP_MUL_INT_LIT16:
		case OP_DIV_INT_LIT16: case OP_REM_INT_LIT16: case OP_AND_INT_LIT16:
		case OP_OR_INT_LIT16: case OP_XOR_INT_LIT16: 

		case OP_ADD_INT_LIT8: case OP_RSUB_INT_LIT8: case OP_MUL_INT_LIT8:
		case OP_DIV_INT_LIT8: case OP_REM_INT_LIT8: case OP_AND_INT_LIT8:
		case OP_OR_INT_LIT8: case OP_XOR_INT_LIT8: case OP_SHL_INT_LIT8:
		case OP_SHR_INT_LIT8: case OP_USHR_INT_LIT8: {
			SET_USE_TYPE(0, J_INT); SET_DEF_TYPE(0, J_INT);
			break;
		}

		// def: long, use: long, long
		case OP_ADD_LONG: case OP_SUB_LONG: case OP_MUL_LONG: case OP_DIV_LONG:
		case OP_REM_LONG: case OP_AND_LONG: case OP_OR_LONG: case OP_XOR_LONG:
		case OP_SHL_LONG: case OP_USHR_LONG: {
			SET_USE_TYPE(0, J_LONG); SET_USE_TYPE(1, J_LONG); SET_DEF_TYPE(0, J_LONG);
			break;
		}

		// def: long, use: long, long
		case OP_NEG_LONG: case OP_NOT_LONG:

		case OP_ADD_LONG_2ADDR: case OP_SUB_LONG_2ADDR: case OP_MUL_LONG_2ADDR:
		case OP_DIV_LONG_2ADDR: case OP_REM_LONG_2ADDR: case OP_AND_LONG_2ADDR:
		case OP_OR_LONG_2ADDR: case OP_XOR_LONG_2ADDR: case OP_SHL_LONG_2ADDR:
		case OP_USHR_LONG_2ADDR: {			
			SET_USE_TYPE(0, J_LONG); SET_DEF_TYPE(0, J_LONG);
			break;
		}

		// def: float, use: float, float
		case OP_ADD_FLOAT: case OP_SUB_FLOAT: case OP_MUL_FLOAT:
		case OP_DIV_FLOAT: case OP_REM_FLOAT:
			
		case OP_ADD_FLOAT_2ADDR: case OP_SUB_FLOAT_2ADDR: case OP_MUL_FLOAT_2ADDR:
		case OP_DIV_FLOAT_2ADDR: case OP_REM_FLOAT_2ADDR: {			
			SET_USE_TYPE(0, J_FLOAT); SET_USE_TYPE(1, J_FLOAT); SET_DEF_TYPE(0, J_FLOAT);
			break;
		}

		// def: float, use: float
		case OP_NEG_FLOAT: {
			SET_USE_TYPE(0, J_FLOAT); SET_DEF_TYPE(0, J_FLOAT);
			break;
		}

		// def: double, use: double, double
		case OP_ADD_DOUBLE: case OP_SUB_DOUBLE: case OP_MUL_DOUBLE:
		case OP_DIV_DOUBLE: case OP_REM_DOUBLE:

		case OP_ADD_DOUBLE_2ADDR: case OP_SUB_DOUBLE_2ADDR: case OP_MUL_DOUBLE_2ADDR:
		case OP_DIV_DOUBLE_2ADDR: case OP_REM_DOUBLE_2ADDR: {			
			SET_USE_TYPE(0, J_DOUBLE); SET_USE_TYPE(1, J_DOUBLE); SET_DEF_TYPE(0, J_DOUBLE);
			break;
		}

		// def: double, use: double			
		case OP_NEG_DOUBLE: {
			SET_USE_TYPE(0, J_DOUBLE); SET_DEF_TYPE(0, J_DOUBLE);
			break;
		}

		// def: type2, use: type1
#define case_OP_TYPE_CAST(TYPE1, TYPE2)		\
		case OP_##TYPE1##_TO_##TYPE2:		\
			SET_USE_TYPE(0, J_##TYPE1);		\
			SET_DEF_TYPE(0, J_##TYPE2);		\
			break

		case_OP_TYPE_CAST(INT, LONG);
		case_OP_TYPE_CAST(INT, FLOAT);
		case_OP_TYPE_CAST(INT, DOUBLE);
		case_OP_TYPE_CAST(LONG, INT);
		case_OP_TYPE_CAST(LONG, FLOAT);
		case_OP_TYPE_CAST(LONG, DOUBLE);
		case_OP_TYPE_CAST(FLOAT, INT);
		case_OP_TYPE_CAST(FLOAT, LONG);
		case_OP_TYPE_CAST(FLOAT, DOUBLE);
		case_OP_TYPE_CAST(DOUBLE, INT);
		case_OP_TYPE_CAST(DOUBLE, LONG);
		case_OP_TYPE_CAST(DOUBLE, FLOAT);
		case_OP_TYPE_CAST(INT, BYTE);
		case_OP_TYPE_CAST(INT, CHAR);
		case_OP_TYPE_CAST(INT, SHORT);

		default:
			break;
	}

	return DTC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Debugging Function Definition
///////////////////////////////////////////////////////////////////////

/**
 * This function will print out the resolved dex codes with the following format.
 * Format - def := opcode (use1, use2, ... )
 *
 * @param pOut output stream
 */
void
DtcDexcode::Write(ostream &pOut) {

	// def
	if(m_rDefWebs.size() > 0) {
		
		pOut << GetDefWeb(0) << " := ";
	}
	
	// opcode
	pOut << dexGetOpcodeName(m_pDecodedInsn->opcode) << '(';

	// use1, use2, ...
	DtcDexLiveWebList_t::iterator iter; 

	for(iter = m_rUseWebs.begin(); iter != m_rUseWebs.end(); ) {
		
		pOut << *iter;
		
		iter++;

		if(iter != m_rUseWebs.end())	pOut << ',';
	}

	pOut << ')';
}

ostream &operator << (ostream &pOut, DtcDexcode *pCode) {
	
	pCode->Write(pOut);
	return pOut;
}

