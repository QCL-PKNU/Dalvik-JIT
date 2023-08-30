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

#ifndef _DTC_DEX_METHOD_H_
#define _DTC_DEX_METHOD_H_

#include "DtcNode.h"
#include "DtcDexcode.h"
#include "DtcCommon.h"
#include "DtcDexBasicBlock.h"

///////////////////////////////////////////////////////////////////////
// User-defined Type Definition
///////////////////////////////////////////////////////////////////////

typedef set<uint32_t> DtcCtrlDataEntry_t;

typedef map<uint32_t, DtcCtrlDataEntry_t> DtcCtrlDataTable_t;

typedef map<uint32_t, DtcDexBasicBlock *> DtcDexBasicBlockTable_t;

///////////////////////////////////////////////////////////////////////
// DtcDexMethod Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcDexMethod {
	
private :
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////
	 
	// Resolved target method
	Method *m_pMethod;

	// Resolve method index
	uint32_t m_nMethodIndex;

	// Original dex method index
	uint32_t m_nDexMethodIndex;
	
	// Dex file including the target method
	DexFile *m_pDexFile;

	// Dex code of the method
	const DexCode *m_pDexCode;

	// The table including the resolve basic blocks
	DtcDexBasicBlockTable_t m_rBasicBlockTable;

	// The table for the data types of the local variables
	DtcDexLiveWebTable_t m_rLocalVarTable;

public :
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcDexMethod(Method *pMethod = NULL);
	
	virtual ~DtcDexMethod();

	///////////////////////////////////////////////////////////////////
	// Accessor & Mutator Declaration/Definition
	///////////////////////////////////////////////////////////////////
	
	const char* GetName();

	char* GetDescriptor();

	/**
	 * This function will return the method index of the resolved method.
	 *
	 * @return the method index of the resolved method
	 */
	inline uint32_t GetMethodIndex()					{ return m_nMethodIndex;		}

	/**
	 * This function will return the method index of the original dex method.
	 *
	 * @return the method index of the original dex method
	 */
	inline uint32_t GetDexMethodIndex()			{ return m_nDexMethodIndex;	}

	/**
	 * This function will return the basic blocks.
	 *
	 * @return the reference of the basic block table
	 */
	inline DtcDexBasicBlockTable_t &GetBasicBlocks()	{ return m_rBasicBlockTable;	}

	/**
	 * This function will return the local variable table.
	 *
	 * @return the local variable table
	 */
	 inline DtcDexLiveWebTable_t &GetLocalVarTable()	{ return m_rLocalVarTable; 	}

	///////////////////////////////////////////////////////////////////
	// Member Function Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcError_t BuildIR();

private:

	DtcError_t ResolveLocalVariables();

	uint32_t ResolveBasicBlocks(DtcCtrlDataTable_t &rCtrlDataTable);

	DtcError_t BuildControlFlowGraph(DtcCtrlDataTable_t &rCtrlDataTable);
	
public:
	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////
	
	void Write(ostream &pOut);
	
	friend ostream &operator << (ostream &pOut, DtcDexMethod *pMethod);

};

#endif
