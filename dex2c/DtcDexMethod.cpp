/*********************************************************************
*   DtcDexMethod.cpp:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#include "DtcDexMethod.h"

///////////////////////////////////////////////////////////////////////
// Static Function Declaration
///////////////////////////////////////////////////////////////////////

#if defined(INCLUDE_DEPRECATED)

static MethodType ResolveMethodType(Opcode nOpcode);

#endif

static void ResolveLocalVariableCb(void *pContext, u2 nReg, u4 nStartAddress,
		        u4 nEndAddress, const char *pName, const char *pDescriptor,
		        const char *pSignature);

///////////////////////////////////////////////////////////////////////
// Constructor & Destructor Definition
///////////////////////////////////////////////////////////////////////

/**
 * Constructor of DtcDexMethod class
 *
 * @param pMethod the resolved method to be translated into C code
 */
DtcDexMethod::DtcDexMethod(Method *pMethod /* = NULL */) {

	// Check the validity of the given method
	if(pMethod == NULL) {
		DTC_ERROR(DEX2C_TAG, "DtcDexMethod - Null method");
		return;
	}

	DTC_DEBUG(DEX2C_TAG, "DtcDexMethod - Method name: %s", pMethod->name);

	m_pMethod = pMethod;

	// Get the dex file including the given method
	m_pDexFile = pMethod->clazz->pDvmDex->pDexFile;

	// Get the dex code
	m_pDexCode = dvmGetMethodCode(pMethod);

	// Get the method index of the resolved method
	m_nMethodIndex = m_pMethod->methodIndex;

	// Get the method index of the original dex method 
	m_nDexMethodIndex = m_pMethod->dexMethodIndex;

	// Initialize the internal data 
	m_rBasicBlockTable.clear();
	m_rLocalVarTable.clear();
}

DtcDexMethod::~DtcDexMethod() {

	m_rBasicBlockTable.clear();
	m_rLocalVarTable.clear();
}

///////////////////////////////////////////////////////////////////////
// Accessor & Mutator Definition
///////////////////////////////////////////////////////////////////////

/**
 * This function will return the name of the method.
 *
 * @return the name of the method
 */
const char *
DtcDexMethod::GetName() {
	
	DTC_ASSERT(m_pDexFile != NULL);
	
	const DexMethodId *dexMethodId = dexGetMethodId(m_pDexFile, GetDexMethodIndex());

	return dexStringById(m_pDexFile, dexMethodId->nameIdx);	
}

/**
 * This function will return the type descriptor of the method.
 *
 * @return the type descriptor of the method
 */
char *
DtcDexMethod::GetDescriptor() {

	DTC_ASSERT(m_pDexFile != NULL);
	
	const DexMethodId *dexMethodId = dexGetMethodId(m_pDexFile, GetDexMethodIndex());

	return dexCopyDescriptorFromMethodId(m_pDexFile, dexMethodId);
}

///////////////////////////////////////////////////////////////////////
// Member Function Definition
///////////////////////////////////////////////////////////////////////

/**
 * This function will be used to build the intermediate representation using the dex code.
 *
 * @return error information
 */
DtcError_t
DtcDexMethod::BuildIR() {

	DTC_DEBUG(DEX2C_TAG, "DtcDexMethod->BuildIR Invoked...");
	
	DtcError_t error;

	DtcCtrlDataTable_t ctrlDataTable;

	// Resolve the local variables of the method
	error = ResolveLocalVariables();
	DTC_CHECK(error == DTC_SUCCESS, RESOLVE_LOCAL_VARS);

	// Resolve the basic blocks using the sequence of the dex codes in the method
	uint32_t nblock = ResolveBasicBlocks(ctrlDataTable);
	DTC_CHECK(nblock != 0, RESOLVE_BASIC_BLOCKS);

	// FIXED - YOUNGSUN
	// Building the control flow graph is not required for the Dex-to-C translation.
#if defined(INCLUDE_DEPRECATED) 

	// Build a control flow graph from the dex code
	error = BuildControlFlowGraph(ctrlDataTable);
	DTC_CHECK(error == DTC_SUCCESS, BUILD_CONTROL_FLOW_GRAPH);

#endif

	// In order to resolve the data types of the registers employed by the dex codes,
	// we are going to perform the local liveness analysis in the scope of a basic block.
	// This is because we'd like to avoid the severe runtime overhead from the global 
	// liveness analysis within the whole method. We guess all of the data types could 
	// be resolved with only the local analysis. We have to do it globally, if it is wrong.
	
	DtcDexBasicBlockTable_t::iterator iter;

	for(iter = m_rBasicBlockTable.begin(); iter != m_rBasicBlockTable.end(); iter++) {

		DtcDexBasicBlock *block = iter->second;

		if(block->ResolveDexcodes() > 0) {

			error = block->AnalyseLiveness(m_rLocalVarTable);

			if(error != DTC_SUCCESS) {
				DTC_ERROR(DEX2C_TAG, "BuildIR - Fail to analyse the liveness(error:%d)", error);
				return DTC_ERROR_ANALYSE_LIVENESS;
			}
		}
	}

	return DTC_SUCCESS;
}

/**
 * This funciton will resolve the data type of each of the local variables in the method.
 * Each pair of a register number and its data type will be kept in rLocalVarTable.  
 *
 * @return error information
 */
DtcError_t
DtcDexMethod::ResolveLocalVariables() {

	DTC_DEBUG(DEX2C_TAG, "DtcDexMethod->ResolveLocalVariables Invoked...");

	const DexMethodId *methodId = dexGetMethodId(m_pDexFile, m_nDexMethodIndex);
	const char *classDescriptor = dexStringByTypeIdx(m_pDexFile, methodId->classIdx);

	m_rLocalVarTable.clear();

	// Resolve the local variables
	dexDecodeDebugInfo(m_pDexFile, m_pDexCode, classDescriptor, methodId->protoIdx, 
			m_pMethod->accessFlags, NULL, ResolveLocalVariableCb, &m_rLocalVarTable);

	return DTC_SUCCESS;
}

/**
 * This function will be used to split the whole sequence of the dex code 
 * into several basic blocks. It is going to find the leaders with the branch codes 
 * and exception infomation.  
 *
 * @param rCtrlDataTable the reference to the control data table
 * @return the number of the basic blocks
 */ 
 
uint32_t
DtcDexMethod::ResolveBasicBlocks(DtcCtrlDataTable_t &rCtrlDataTable) {

	DTC_DEBUG(DEX2C_TAG, "DtcDexMethod->ResolveBasicBlocks Invoked...");

	if(m_pDexCode == NULL) {
		
		DTC_ERROR(DEX2C_TAG, "ResolveBasicBlocks - Null DexCode");
		return 0;
	}

	// Dex codes
	const uint16_t*insns = m_pDexCode->insns;
	const uint32_t insnsSize = m_pDexCode->insnsSize;
	
	// Exception information
	const DexTry *tries = dexGetTries(m_pDexCode);
	const uint16_t triesSize = m_pDexCode->triesSize;

	// Flag information for making a basicblock
	uint8_t *flags = new uint8_t[insnsSize];
	memset(flags, 0, sizeof(uint8_t) * insnsSize);

	///////////////////////////////////////////////////////////////////
	// Macros for resolving the basic blocks
	///////////////////////////////////////////////////////////////////

// Dex code
#define OPCODE_FLAG							0x40
#define CHECK_OPCODE(INDEX)					flags[INDEX] |= OPCODE_FLAG
#define DEX_OPCODE(INDEX)						(insns[INDEX] & 0xFF)
#define DEX_CODESIZE(INDEX)						(dexGetWidthFromInstruction(&insns[INDEX]))
#define DEX_OPERAND16(INDEX)					(get2LE((uint8_t *)&insns[INDEX]))
#define DEX_OPERAND32(INDEX)					(get4LE((uint8_t *)&insns[INDEX]))

// Branch offset
#define BRANCH_OFFSET8(INDEX)					((int32_t)(insns[INDEX] & 0xFF))
#define BRANCH_OFFSET16(INDEX)					((int32_t)get2LE((uint8_t *)&insns[INDEX]))
#define BRANCH_OFFSET32(INDEX)					((int32_t)get4LE((uint8_t *)&insns[INDEX]))

// Leader
#define START_FLAG								0x01
#define BRANCH_TARGET_FLAG					0x02
#define BRANCH_NOT_TAKEN_FLAG				0x04
#define EXCEPTION_CATCH_FLAG					0x08
#define EXCEPTION_TRY_FLAG						0x10
#define SWITCH_FLAG							0x20
#define RETURN_FLAG								0x80

#define IS_OPCODE(INDEX)						(flags[INDEX] & OPCODE_FLAG)
#define IS_LEADER(INDEX)						(flags[INDEX] & 0x1F)
#define IS_RETURN(INDEX)						(flags[INDEX] & RETURN_FLAG)

#define CHECK_START()							flags[0] |= START_FLAG
#define CHECK_EXCEPTION_CATCH(INDEX)			flags[INDEX] |= EXCEPTION_CATCH_FLAG
#define CHECK_EXCEPTION_TRY(INDEX)				flags[INDEX] |= EXCEPTION_TRY_FLAG
#define CHECK_SWITCH(INDEX)					flags[INDEX] |= SWITCH_FLAG;
#define CHECK_RETURN(INDEX)					flags[INDEX] |= RETURN_FLAG

#define CHECK_BRANCH_TARGET(BASE,OFFSET)			\
do {													\
	flags[BASE+OFFSET] |= BRANCH_TARGET_FLAG;		\
	rCtrlDataTable[BASE].insert(BASE+OFFSET);		\
} while(0)

#define CHECK_BRANCH_NOT_TAKEN(BASE,OFFSET)		\
do {													\
	flags[BASE+OFFSET] |= BRANCH_NOT_TAKEN_FLAG;	\
	rCtrlDataTable[BASE].insert(BASE+OFFSET);		\
} while(0)

#define INCREMENT_INDEX(OFFSET)					i+=OFFSET;	

	///////////////////////////////////////////////////////////////////
	// Find the leaders of the basic blocks        			
	///////////////////////////////////////////////////////////////////

	rCtrlDataTable.clear();

	// Exceptions
	for(uint32_t i = 0; i < triesSize; i++) {

		uint32_t targetAddr = tries[i].startAddr + tries[i].insnCount;
		
		DTC_DEBUG(DEX2C_TAG, "Exception Try - 0x%X", tries[i].startAddr);
		CHECK_EXCEPTION_TRY(tries[i].startAddr);

		DTC_DEBUG(DEX2C_TAG, "Exception Catch - 0x%X", targetAddr);
		CHECK_EXCEPTION_CATCH(targetAddr);
	}

	// Start address of the function
	CHECK_START();

	for(uint32_t i = 0;  i < insnsSize;) {
	
		CHECK_OPCODE(i);

		switch(DEX_OPCODE(i)) {

			case OP_IF_EQ:		/* if-test vA, vB (0) + CCCC (1)*/ 
			case OP_IF_NE:
			case OP_IF_LT:
			case OP_IF_GE:
			case OP_IF_GT:
			case OP_IF_LE:
			case OP_IF_EQZ:
			case OP_IF_NEZ:
			case OP_IF_LTZ:
			case OP_IF_GEZ:
			case OP_IF_GTZ:
			case OP_IF_LEZ: {

				// Check the not taken and target addresses of the branch
				CHECK_BRANCH_NOT_TAKEN(i, 2);
				CHECK_BRANCH_TARGET(i, BRANCH_OFFSET16(i+1));

				// Skip to the next instruction
				INCREMENT_INDEX(2);
				break;
			}
			case OP_GOTO: {		/* goto + AA (0) */

				// Check the branch target address
				CHECK_BRANCH_TARGET(i, BRANCH_OFFSET8(i));

				// Skip to the next instruction
				INCREMENT_INDEX(1);
				break;
			}	
			case OP_GOTO_16: {	/* goto/16 (0) + AAAA (1) */
				CHECK_BRANCH_TARGET(i, BRANCH_OFFSET16(i+1));
				INCREMENT_INDEX(2);
				break;
			}
			case OP_GOTO_32: {	/* goto/32 (0) + AAAAAAAA (1) */
				CHECK_BRANCH_TARGET(i, BRANCH_OFFSET32(i+1));
				INCREMENT_INDEX(3);
				break;
			}
			case OP_PACKED_SWITCH: {

				// Check this is a switch instruction 
				CHECK_SWITCH(i);
				
				// Get the start position of the index table
				uint32_t tableIndex = i + BRANCH_OFFSET32(i+1);

				// Check the ID of the table
				if(DEX_OPERAND16(tableIndex++) != 0x0100) {
					
					DTC_ERROR(DEX2C_TAG, "Invalid packed switch instruction - %d", i);
					return 0;
				}

				// Get the number of entries in the table
				uint16_t tableSize = DEX_OPERAND16(tableIndex++);

				// Skip the first key
				tableIndex += 2;

				for(uint16_t j = 0; j < tableSize; j++, tableIndex += 2) {

					// Check each relative branch target
					CHECK_BRANCH_TARGET(i, BRANCH_OFFSET32(tableIndex));
				}
				break;
			}
			case OP_SPARSE_SWITCH: {

				// Check this is a switch instruction 
				CHECK_SWITCH(i);
				
				// Get the start position of the index table
				uint32_t tableIndex = i + BRANCH_OFFSET32(i+1);

				// Check the ID of the table
				if(DEX_OPERAND16(tableIndex++) != 0x0200) {
					
					DTC_ERROR(DEX2C_TAG, "Invalid sparse switch instruction - %d", i);
					return 0;
				}

				// Get the number of entries in the table
				uint16_t tableSize = DEX_OPERAND16(tableIndex++);

				// Skip the keys of the index table
				tableIndex += (tableSize * 2);

				for(uint16_t j = 0; j < tableSize; j++, tableIndex += 2) {

					// Check each relative branch target
					CHECK_BRANCH_TARGET(i, BRANCH_OFFSET32(tableIndex));
				}
				break;
			}
			case OP_RETURN:
			case OP_RETURN_VOID:
			case OP_RETURN_WIDE:
			case OP_RETURN_OBJECT:
			case OP_RETURN_VOID_BARRIER: 
				CHECK_RETURN(i);
			default:
				INCREMENT_INDEX(DEX_CODESIZE(i));
				break;
		}
	}
	
	//////////////////////////////////////////////////////////////
	// Split the method into several basic blocks using the leaders        			
	//////////////////////////////////////////////////////////////

	// Print out each of the leader address
#if 0
	CtrlDataTable_t::iterator cdtIter;
	
	for(cdtIter=rCtrlDataTable.begin();cdtIter!=rCtrlDataTable.end();++cdtIter)
	{
		DTC_DEBUG(DEX2C_TAG, "Leader - 0x%04X", cdtIter->first);
	}
#endif

	// Index values for indicating both the first and last instructions of a basic block
	uint32_t spc = 0;
	uint32_t epc = 0;

	// An index value for the previous instruction
	uint32_t prevpc = 0;

	for(uint32_t i = 1; i < insnsSize; i++) {

		// Each basic block will be comprised of the instructions between two adjacent leaders
		if(IS_LEADER(i)) {

			// If there are any missing leaders, check them additionally.   			
			if(rCtrlDataTable.count(prevpc) == 0 && !IS_RETURN(prevpc)) {
				
				rCtrlDataTable[prevpc].insert(i);
			}

			// Build a new basic block using the instructions between the first and last instructions
			m_rBasicBlockTable[spc] = new DtcDexBasicBlock(insns, flags, spc, (epc = i-1));

			// Get the first instruction of the next basic block
			spc = i;
		}

		if(IS_OPCODE(i))		prevpc = i;
	}

	m_rBasicBlockTable[spc] = new DtcDexBasicBlock(insns, flags, spc, insnsSize-1);

	// The number of the basic blocks
	return (uint32_t)m_rBasicBlockTable.size();
}

/*
 * This function will build the control flow graph(CFG) of the method.
 *
 * @param rCtrlDataTable the table including the control flow information to be used to build the CFG
 * @return error information
 */ 
 
DtcError_t
DtcDexMethod::BuildControlFlowGraph(DtcCtrlDataTable_t &rCtrlDataTable){

	DTC_DEBUG(DEX2C_TAG, "DtcDexMethod->BuildControlFlowGraph Invoked...");
		
	DtcCtrlDataEntry_t entry;
	DtcDexBasicBlock *block = NULL;

	DtcCtrlDataEntry_t::iterator cdeIter;
	DtcDexBasicBlockTable_t::iterator bbtIter;

	// Resolve the basic blocks and get the number
	uint32_t nblock = ResolveBasicBlocks(rCtrlDataTable);

	if(nblock == 0) {
		DTC_ERROR(DEX2C_TAG, "No basic blocks are resolved");
		return DTC_ERROR_RESOLVE_BASIC_BLOCKS;
	}

	// Connect the source and target basic blocks with each other
	for(bbtIter = m_rBasicBlockTable.begin(); bbtIter != m_rBasicBlockTable.end(); bbtIter++) {

		block = bbtIter->second;

		entry = rCtrlDataTable[bbtIter->first];
		
		for(cdeIter = entry.begin(); cdeIter != entry.end(); cdeIter++) {
			
			block->Connect(m_rBasicBlockTable[ *cdeIter]);
		}
	}
	
	return DTC_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
// Static Function Definition
///////////////////////////////////////////////////////////////////////

#if defined(INCLUDE_DEPRECATED)

static MethodType
ResolveMethodType(Opcode nOpcode) {

	switch(nOpcode) {
		
		// virtual, super
		case OP_INVOKE_VIRTUAL:
		case OP_INVOKE_VIRTUAL_RANGE:
		case OP_INVOKE_VIRTUAL_QUICK:
		case OP_INVOKE_VIRTUAL_QUICK_RANGE:
		case OP_INVOKE_SUPER:
		case OP_INVOKE_SUPER_RANGE:
		case OP_INVOKE_SUPER_QUICK:
		case OP_INVOKE_SUPER_QUICK_RANGE:
			return METHOD_VIRTUAL;

		// static, direct
		case OP_INVOKE_STATIC:
		case OP_INVOKE_STATIC_RANGE:
		case OP_INVOKE_DIRECT:
		case OP_INVOKE_DIRECT_RANGE:
			return METHOD_DIRECT;

		// invoking interface functions will not be allowed in dalvik vm.
		case OP_INVOKE_INTERFACE:
		case OP_INVOKE_INTERFACE_RANGE:
			DTC_ERROR(DEX2C_TAG, "Invalid invoke interface - %d", nOpcode); 
			break;

		default:
			DTC_ERROR(DEX2C_TAG, "Unknown invoke opcode - %d", nOpcode); 
			break;
	}

	return METHOD_UNKNOWN;
}

#endif

/**
 * This function will be invoked by dexDecodeDebugInfo function to resolve the 
 * local variable information from the debugging stream.
 *
 * @ param pContext the address of local variable array
 * @ param nReg the register number
 * @ param nStartAddress the start address of the variable (if it is 0, it will be one of the method arguments.)
 * @ param nEndAddress the end address of the variable
 * @ param pName the name string of the variable
 * @ param pDescriptor the descriptor string of the variable
 * @ param pSignature the signature string of the variable
 */
static void 
ResolveLocalVariableCb(void *pContext, u2 nReg, u4 nStartAddress,
        u4 nEndAddress, const char *pName, const char *pDescriptor,
        const char *pSignature) {

	uint32_t flag = DTC_WEB_LOCAL_VAR;

	if(nStartAddress == 0) {
		flag |= DTC_WEB_FUNC_ARG;
	}

	// Build the live web and set it as local variable's web
	DtcDexLiveWeb *web = new DtcDexLiveWeb(nReg, flag);

	// Resolve the data type of the local variable
	switch(*pDescriptor) {	
		case 'Z': web->SetDataType(J_BOOLEAN); break;
		case 'B': web->SetDataType(J_BYTE); break;
		case 'S': web->SetDataType(J_SHORT); break;
		case 'C': web->SetDataType(J_CHAR); break;
		case 'I':	web->SetDataType(J_INT); break;
		case 'J':	web->SetDataType(J_LONG); break;
		case 'F':	web->SetDataType(J_FLOAT); break;
		case 'D':	web->SetDataType(J_DOUBLE); break;
		case '[':
		case 'L': web->SetDataType(J_OBJECT); break;
		default: web->SetDataType(J_UNKNOWN); break;
	}

	// Append the local variable information to the array
	(*(DtcDexLiveWebTable_t *)pContext)[nReg] = web;
}

///////////////////////////////////////////////////////////////////////
// Debugging Function Definition
///////////////////////////////////////////////////////////////////////

void
DtcDexMethod::Write(ostream &pOut) {

	// Print out the method signature
	pOut << "Method: " << m_pMethod->name << endl;

	DtcDexLiveWebTable_t::iterator lviter;

	// Print out the local variable information
	for(lviter = m_rLocalVarTable.begin(); lviter != m_rLocalVarTable.end(); lviter++) {

		DtcDexLiveWeb *web = lviter->second;

		if(web == NULL) {
			continue;
		}
		
		if(web->GetIsArgument())	pOut << "#Argument: ";
		else 					pOut << "#Local variable: ";

		pOut << web << '\n';
	}

	// Print out the basic block information
	DtcDexBasicBlockTable_t::iterator bbiter;

	for(bbiter = m_rBasicBlockTable.begin(); bbiter != m_rBasicBlockTable.end(); bbiter++) {
		
		pOut << bbiter->second;
	}
}

ostream &operator << (ostream &pOut, DtcDexMethod *pMethod) {
	pMethod->Write(pOut);
	return pOut;
}

