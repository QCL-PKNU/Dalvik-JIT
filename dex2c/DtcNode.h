/*********************************************************************
*   DtcNode.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#ifndef _DTC_NODE_H_
#define _DTC_NODE_H_

#include "DtcCommon.h"

///////////////////////////////////////////////////////////////////////
// Macro Definition
///////////////////////////////////////////////////////////////////////

#define DTC_DFS_BLACK	0
#define DTC_DFS_GRAY	1
#define DTC_DFS_WHITE	2

///////////////////////////////////////////////////////////////////////
// User-defined Data Types
///////////////////////////////////////////////////////////////////////

class DtcNode;

typedef vector<DtcNode *> DtcNodeList_t;

///////////////////////////////////////////////////////////////////////
// DtcNode Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcNode {
	
protected :
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////

	// Node ID
	uint32_t m_nId;

	// Successor and predecessor nodes
	DtcNodeList_t m_rSuccNodes;
	DtcNodeList_t m_rPredNodes;

#if defined(INCLUDE_DEPRECATED)

	// For traversing all the nodes of a graph
	uint32_t m_nDfsColor;
	uint32_t m_nDfsDepth;
	
	// Previous and next nodes
	DtcNode *m_pPrevNode;
	DtcNode *m_pNextNode;

#endif

public :
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcNode(uint32_t nId = 0);

	virtual ~DtcNode();

	///////////////////////////////////////////////////////////////////
	// Accessor & Mutator Declaration/Definition
	///////////////////////////////////////////////////////////////////

	/**
	 * This function will return the node ID.
	 * 
	 * @return node ID
	 */
	inline uint32_t GetId() 							{ return m_nId;				}

	/**
	 * This function will return the list of the successor nodes of this node.
	 *
	 * @return the list of the successor nodes of this node
	 */
	inline DtcNodeList_t GetSuccNodes()				{ return m_rSuccNodes;		}

	/**
	 * This function will return the list of the predecessor nodes of this node.
	 *
	 * @return the list of the predecessor nodes of this node
	 */
	inline DtcNodeList_t GetPredNodes()				{ return m_rPredNodes;		}	

	/**
	 * This function will used to set the node ID.
	 * 
	 * @param nId the new node ID
	 */
	inline void SetId(uint32_t nId)						{ m_nId = nId;				}

	/**
	 * This function will be used to append a new successor node to this node.
	 *
	 * @param a new successor node of this node
	 */
	inline void AppendSuccNode(DtcNode *pNode)	{ m_rSuccNodes.push_back(pNode);	}

	/**
	 * This function will be used to append a new predecessor node to this node.
	 *
	 * @param a new predecessor node of this node
	 */	
	inline void AppendPredNode(DtcNode *pNode)	{ m_rPredNodes.push_back(pNode);	}	

#if defined(INCLUDE_DEPRECATED)

	/**
	 * This function will return the color of this node.
	 * 
	 * @return the color of the node (WHITE:0, GRAY:1, BLACK:2)
	 */	
	inline uint32_t GetColor()							{ return m_nDfsColor;			}

	/**
	 * This function will return the depth of the node in the enclosing graph.
	 *
	 * @return the depth of the node
	 */
	inline uint32_t GetDepth()						{ return m_nDfsDepth;			}

	/**
	 * This function will return the previous node of this node.
	 *
	 * @return the previous node of this node
	 */
	inline DtcNode *GetPrevNode()					{ return m_pPrevNode;		}

	/**
	 * This function will return the next node of this node.
	 *
	 * @return the next node of this node
	 */
	inline DtcNode *GetNextNode()					{ return m_pNextNode;		}

	/**
	 * This function will be used to set the color of this node.
	 * 
	 * @param nColor the new color of the node (WHITE:0, GRAY:1, BLACK:2)
	 */	
	inline void SetColor(uint32_t nColor)				{ m_nDfsColor = nColor;		}
	
	/**
	 * This function will be used to set the depth of the node in the enclosing graph.
	 *
	 * @param nDepth the depth of the node
	 */	
	inline void SetDepth(uint32_t nDepth )				{ m_nDfsDepth = nDepth;		}

	/**
	 * This function will be used to set the previous node of this node.
	 *
	 * @param pPrevNode the previous node of this node
	 */	
	inline void SetPrevNode(DtcNode *pPrevNode)		{ 
		
		m_pPrevNode = pPrevNode;	
	}

	/**
	 * This function will be used to set  the next node of this node.
	 *
	 * @param pNextNode the next node of this node
	 */	
	inline void SetNextNode(DtcNode *pNextNode)		{ 
		pNextNode->SetId(m_nId + 1);
		m_pNextNode = pNextNode;	
	}
#endif

	///////////////////////////////////////////////////////////////////
	// Member Function Declaration
	///////////////////////////////////////////////////////////////////

	void Connect(DtcNode *pNode);

	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////
	
	virtual void Write(ostream &pOut) = 0;
	
	friend ostream &operator << (ostream &pOut, DtcNode *pNode);
};

#endif
