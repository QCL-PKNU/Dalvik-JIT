/*********************************************************************
*   DtcDexBasicBlock.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#ifndef _DTC_DEX_BASIC_BLOCK_H_
#define _DTC_DEX_BASIC_BLOCK_H_

#include "DtcNode.h"
#include "DtcDexcode.h"
#include "DtcCommon.h"

///////////////////////////////////////////////////////////////////////
// User-defined Type Definition
///////////////////////////////////////////////////////////////////////

typedef vector<DtcDexcode *> DtcDexcodeList_t;

typedef map<uint16_t, DtcDexLiveWeb *> DtcDexLiveWebTable_t;

///////////////////////////////////////////////////////////////////////
// DtcDexBasicBlock Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcDexBasicBlock : public DtcNode {
	
private :
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////

	// The start address of the basic block
	uint32_t m_nStartAddress;

	// Flags for additional information
	const uint8_t *m_pFlags;

	// The dex code of the basic block
	const uint16_t *m_pInsns;

	// The size of the dex code
	uint32_t m_nInsnsSize;

	// Decoded dex code list
	DtcDexcodeList_t m_rDexcodes;

public :
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcDexBasicBlock(const uint16_t *pInsns, const uint8_t *pFlags, uint32_t nSpc, uint32_t nEpc);
	
	virtual ~DtcDexBasicBlock();

	///////////////////////////////////////////////////////////////////
	// Member Function Declaration
	///////////////////////////////////////////////////////////////////	

	uint32_t ResolveDexcodes();

	DtcError_t AnalyseLiveness(DtcDexLiveWebTable_t &rLocalVarTable);

	///////////////////////////////////////////////////////////////////
	// Accessor & Mutator Declaration/Definition
	///////////////////////////////////////////////////////////////////

#if defined(INCLUDE_DEPRECATED)

	/**
	 * This function will return the address of the first instruction of the basic block.
	 *
	 * @return the address of the first instruction
	 */
	inline const uint16_t *GetInsns()			{ return m_pInsns;		}

	/**
	 * This function will return the number of the instructions in the basic block.
	 *
	 * @return the number of the instructions in the basic block
	 */
	inline uint32_t GetInsnsSize()			{ return m_nInsnsSize;	}

#endif

	/** 
	 * This function will the start address of the basic block.
	 *
	 * @return the start address
	 */	
	inline uint32_t GetStartAddress()		{ return m_nStartAddress;	}

	/**
	 * This function will return the list of the dexcode objects.
	 *
	 * @return the list of the dexcode obejcts
	 */
	inline DtcDexcodeList_t &GetDexcodes()	{ return m_rDexcodes;	}

	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////
	
	virtual void Write(ostream &pOut);

	friend ostream &operator << (ostream &pOut, DtcDexBasicBlock *pBlock);	
};

#endif
