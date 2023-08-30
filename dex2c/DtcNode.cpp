/*********************************************************************
*   DtcNode.cpp:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#include "DtcNode.h"

///////////////////////////////////////////////////////////////////////
// Constructor & Destructor Definition
///////////////////////////////////////////////////////////////////////
 
DtcNode::DtcNode(uint32_t nId /* = 0 */) {

	m_nId = nId;

	m_rSuccNodes.clear();
	m_rPredNodes.clear();
}

DtcNode::~DtcNode() {
	
	m_rSuccNodes.clear();
	m_rPredNodes.clear();
}

///////////////////////////////////////////////////////////////////////
// Member Function Definition
///////////////////////////////////////////////////////////////////////

/**
 * This function will be used to connect the given node and this node.
 *
 * @param pNode the node to be connected to this node
 */
void
DtcNode::Connect(DtcNode *pNode) {

	//pNode->this
	this->AppendSuccNode(pNode);
	
	//this->pNode
	pNode->AppendPredNode(this);
}

///////////////////////////////////////////////////////////////////////
// Debugging Function Definition
///////////////////////////////////////////////////////////////////////

ostream &operator << (ostream &pOut, DtcNode *pNode) {
	
	pNode->Write(pOut);
	return pOut;
}

