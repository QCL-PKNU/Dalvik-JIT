/*********************************************************************
*   DtcDexcode.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#ifndef _DTC_DEX_CODE_H_
#define _DTC_DEX_CODE_H_

#include "DtcCommon.h"
#include "DtcDexLiveWeb.h"

///////////////////////////////////////////////////////////////////////
// User-defined Type Definition
///////////////////////////////////////////////////////////////////////

typedef vector<DtcDexLiveWeb *> DtcDexLiveWebList_t;

///////////////////////////////////////////////////////////////////////
// DtcDexcode Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcDexcode {
	
private :
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////

	// Decoded instruction from the dex code
	DecodedInstruction *m_pDecodedInsn;

	// Instruction index
	uint32_t m_nInsnIndex;

	// Def webs
	DtcDexLiveWebList_t m_rDefWebs;
	
	// Use webs
	DtcDexLiveWebList_t m_rUseWebs;

public :
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcDexcode(const uint16_t *pInsn, uint32_t nInsnIndex);
	
	virtual ~DtcDexcode();

	///////////////////////////////////////////////////////////////////
	// Accessor & Mutator Declaration/Definition
	///////////////////////////////////////////////////////////////////

	/**
	 * This function will return the decoded instruction including opcode and operands.
	 *
	 * @return the decoded instruction
	 */
	inline DecodedInstruction *GetDecodedInfo()	{ return m_pDecodedInsn;	}

	/**
	 * This function will return the instruction index.
	 *
	 * @return the instruction index
	 */
	inline uint32_t GetInsnIndex()				{ return m_nInsnIndex;	}
	
	/**
	 * This function will return the list of the def webs.
	 *
	 * @return the list of the def webs
	 */
	inline DtcDexLiveWebList_t &GetDefWebs()		{ return m_rDefWebs;		}

	/**
	 * This function will return the list of the use webs.
	 *
	 * @return the list of the use webs
	 */
	inline DtcDexLiveWebList_t &GetUseWebs()		{ return m_rUseWebs;		}

	/**
	 * This function will return a def web indicated by the given index from the def web list
	 *
	 * @return a def web at the position of the given index
	 */
	inline DtcDexLiveWeb *GetDefWeb(uint32_t nIndex) {
		return (nIndex < m_rDefWebs.size()) ? m_rDefWebs[nIndex] : NULL;
	}

	/**
	 * This function will return an use web indicated by the given index from the def web list
	 *
	 * @return an use web at the position of the given index
	 */
	inline DtcDexLiveWeb *GetUseWeb(uint32_t nIndex) {
		return (nIndex < m_rUseWebs.size()) ? m_rUseWebs[nIndex] : NULL;
	}
	
	/**
	 * This function will be used to append a new live web to the def web list.
	 *
	 * @param pWeb a live web to be inserted into the def web list
	 */
	inline void AppendDefWeb(DtcDexLiveWeb *pWeb)	{ 
		m_rDefWebs.push_back(pWeb);	
	}

	/**
	 * This function will be used to append a new live web to the use web list.
	 *
	 * @param pWeb a live web to be inserted into the use web list
	 */
	inline void AppendUseWeb(DtcDexLiveWeb *pWeb) {
		m_rUseWebs.push_back(pWeb);	
	}

	///////////////////////////////////////////////////////////////////
	// Member Function Declaration
	///////////////////////////////////////////////////////////////////	

	DtcError_t ResolveDataTypes();

	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////
	
	virtual void Write(ostream &pOut);

	friend ostream &operator << (ostream &pOut, DtcDexcode *pCode);	
};

#endif
