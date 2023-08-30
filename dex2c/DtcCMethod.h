/*********************************************************************
*   DtcCMethod.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#ifndef _DTC_C_METHOD_H_
#define _DTC_C_METHOD_H_

#include "DtcCommon.h"
#include "DtcDexMethod.h"
#include "DtcCBasicBlock.h"

///////////////////////////////////////////////////////////////////////
// User-defined Type Definition
///////////////////////////////////////////////////////////////////////

typedef vector<DtcCBasicBlock *> DtcCBasicBlockList_t;

///////////////////////////////////////////////////////////////////////
// DtcCMethod Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcCMethod {
	
private :
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////
	 
	// The dex method to be translated
	DtcDexMethod *m_pDexMethod;

	// The list of the translated C basic blocks
	DtcCBasicBlockList_t m_rBasicBlockList;

	// The table of the symbols used in the generated C method
	DtcCSymbolTable_t m_rSymbolTable;

public :
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcCMethod(DtcDexMethod *pDexMethod);
	
	virtual ~DtcCMethod();

	///////////////////////////////////////////////////////////////////
	// Member Function Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcError_t Translate();

private:

	void WritePrologue(ostream &pOut);

	void WriteEpilogue(ostream &pOut);

public:
	
	void Write(ostream &pOut);
	
	friend ostream &operator << (ostream &pOut, DtcCMethod *pMethod);

};

#endif
