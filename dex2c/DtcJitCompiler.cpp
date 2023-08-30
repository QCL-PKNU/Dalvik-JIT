/*********************************************************************
*   DtcJitCompiler.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#include "DtcJitCompiler.h"

///////////////////////////////////////////////////////////////////////
// Constructor & Destructor
///////////////////////////////////////////////////////////////////////

DtcJitCompiler::DtcJitCompiler()  {

	m_pCMethod = NULL;
}

DtcJitCompiler::~DtcJitCompiler() {
	// Do nothing
}

///////////////////////////////////////////////////////////////////////
// Member Functions
///////////////////////////////////////////////////////////////////////

/**
 * This function will translate the dex code of the given method into C code.
 *
 * @param pMethod the resolved method to be translated
 * @return error information
 */
DtcError_t
DtcJitCompiler::TranslateMethod(Method *pMethod) {

	DtcError_t error;

	DTC_DEBUG(DEX2C_TAG, "DtcJitCompiler->TranslateMethod Invoked...");

	if(pMethod == NULL) {
		DTC_ERROR(DEX2C_TAG, "Invalid input Dex method - %X", pMethod);
		return DTC_ERROR_INVALID_PARAMETER;
	}
	
	DtcDexMethod *dexMethod = new DtcDexMethod(pMethod);

	if((error = dexMethod->BuildIR()) != DTC_SUCCESS) {
		
		DTC_ERROR(DEX2C_TAG, "TranslateMethod - Build IR (error:%d)", error);
		return error;
	}

	// Print out the dex IR
#if 0
	strstream o;
	o << dexMethod << ends;
	DTC_DEBUG(DEX2C_TAG, "%s", o.str());
#endif

	DtcCMethod *cMethod = new DtcCMethod(dexMethod);

	if((error = cMethod->Translate()) != DTC_SUCCESS) {
		
		DTC_ERROR(DEX2C_TAG, "TranslateMethod - Translate Dex to C (error:%d)", error);
		return error;
	}
	
	// Print out the C code
#if 1
	strstream o;
	o << cMethod << ends;
	DTC_DEBUG(DEX2C_TAG, "%s", o.str());
#endif

	return DTC_SUCCESS;
}

/**
 * This function will print out the generated C code to the file at the given path.
 *
 * @param pFilePath file path
 */
void 
DtcJitCompiler::DumpFile(const char * pFilePath /* = NULL */) {

	if(m_pCMethod == NULL) {
		DTC_ERROR(DEX2C_TAG, "DumpFile - Invalid C method: %X", m_pCMethod);
		return;
	}

	if(pFilePath == NULL) {
		pFilePath = "./dtcjit.out.tmp"; /* Method Signature */
	}

	ofstream outFile(pFilePath);
	outFile << m_pCMethod << endl;
	outFile.close();
}

///////////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////////

void
DtcJitCompiler::Write(ostream &pOut) {
}

ostream &operator << (ostream &pOut, DtcJitCompiler *pCompiler) {
	pCompiler->Write(pOut);
	return pOut;
}

