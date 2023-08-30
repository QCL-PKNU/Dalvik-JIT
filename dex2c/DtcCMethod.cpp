/*********************************************************************
*   DtcCMethod.cpp:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#include "DtcCMethod.h"

///////////////////////////////////////////////////////////////////////
// Constructor & Destructor Definition
///////////////////////////////////////////////////////////////////////
/**
 * Constructor of DtcCMethod class
 *
 * @param pDexMethod the dex method to be translated into C code
 */
DtcCMethod::DtcCMethod(DtcDexMethod *pDexMethod) {

	// Check the validity of the given method
	if(pDexMethod == NULL) {
		DTC_ERROR(DEX2C_TAG, "DtcCMethod - Null dex method");
	}

	m_pDexMethod = pDexMethod;

	// Initialize the internal data 
	m_rBasicBlockList.clear();
	m_rSymbolTable.clear();
}

DtcCMethod::~DtcCMethod() {

	m_rBasicBlockList.clear();
	m_rSymbolTable.clear();
}

///////////////////////////////////////////////////////////////////////
// Member Function Definition
///////////////////////////////////////////////////////////////////////

/**
 * This function will translate all the Dex basic blocks into C basic blocks.
 *
 * @return error information
 */
DtcError_t
DtcCMethod::Translate() {

	DTC_DEBUG(DEX2C_TAG, "DtcCMethod->Translate Invoked...");
	
	DtcError_t error;
	
	DTC_CHECK(m_pDexMethod != NULL, INVALID_DEX_METHOD);

	// Initialize both of the symbol table and the basic block list
	m_rBasicBlockList.clear();

#if defined(INCLUDE_DEPRECATED)

	m_rSymbolTable.clear();

	// Register the symbols of the local variable array to the symbol table
	DtcDexLiveWebTable_t::iterator lwiter;
	DtcDexLiveWebTable_t &localVarTable = m_pDexMethod->GetLocalVarTable();

	for(lwiter = localVarTable.begin(); lwiter != localVarTable.end(); lwiter++) {

		DtcDexLiveWeb *web = lwiter->second;

		strstream o;
		o << web << ends;
		const char *symname = o.str();

		if(m_rSymbolTable.count(symname) == 0) {
			
			m_rSymbolTable[symname] = web;
		}
	}

#endif

	// Perform the Dex-to-C translation for each of the basic blocks in the method
	DtcDexBasicBlockTable_t::iterator bbiter;
	DtcDexBasicBlockTable_t &dexBlocks = m_pDexMethod->GetBasicBlocks();

	for(bbiter = dexBlocks.begin(); bbiter != dexBlocks.end(); bbiter++) {
	
		DtcCBasicBlock *cBlock = new DtcCBasicBlock(bbiter->second);

		if((error = cBlock->Translate(m_rSymbolTable)) != DTC_SUCCESS) {
			break;
		}

		m_rBasicBlockList.push_back(cBlock);
	}

	return DTC_SUCCESS;
}

/**
 * This function will print out the prologue code of the translated method.
 *
 * @param pOut output stream
 */
void
DtcCMethod::WritePrologue(ostream &pOut) {

	pOut << "Method Prologue [" << m_pDexMethod->GetName() << "] -------------\n";
}

/**
 * This function will print out the epilogue code of the translated method.
 *
 * @param pOut output stream
 */
void
DtcCMethod::WriteEpilogue(ostream &pOut) {

	pOut << "Method Epilogue -----------------------------\n";
}

/**
 * This function will print out the whole code, including prologue, epilogue, and body, of the translated method.
 *
 * @param pOut output stream
 */
void
DtcCMethod::Write(ostream &pOut) {

	// Prologue
	WritePrologue(pOut);

	// Body
	DtcCBasicBlockList_t::iterator iter;

	for(iter = m_rBasicBlockList.begin(); iter != m_rBasicBlockList.end(); iter++) {

		pOut << *iter;
	}

	// Epilogue
	WriteEpilogue(pOut);
}

ostream &operator << (ostream &pOut, DtcCMethod *pMethod) {
	pMethod->Write(pOut);
	return pOut;
}
