/*********************************************************************
*   DtcCExpression.h:
*
*   Authors:
*      Youngsun Han (youngsun@kiu.ac.kr)
*      Minseong Kim (kim1144@naver.com)
*
*   Copyright (c) 2014
*
*   Compiler & Microarchitecture Laboratory (compiler.korea.ac.kr)
**********************************************************************/

#ifndef _DTC_C_EXPRESSION_H_
#define _DTC_C_EXPRESSION_H_

#include "DtcCommon.h"
#include "DtcCSymbol.h"

class DtcCExpression;
class DtcCIdExpr;
class DtcCUnaryExpr;
class DtcCBinaryExpr;
class DtcCCallExpr;

///////////////////////////////////////////////////////////////////////
// User-defined Type Definition
///////////////////////////////////////////////////////////////////////

// C expression types  for DEX2C
typedef enum {

	DTC_C_EXPR_UNKNOWN = 0, 
	DTC_C_EXPR_ID,				/* ID */
	DTC_C_EXPR_UNARY_NEG,	/* Unary */
	DTC_C_EXPR_UNARY_NOT,
	DTC_C_EXPR_UNARY_BOR,
	DTC_C_EXPR_UNARY_BAND,	
	DTC_C_EXPR_UNARY_I2J,
	DTC_C_EXPR_UNARY_I2F,
	DTC_C_EXPR_UNARY_I2D,
	DTC_C_EXPR_UNARY_J2I,
	DTC_C_EXPR_UNARY_J2F,
	DTC_C_EXPR_UNARY_J2D,
	DTC_C_EXPR_UNARY_F2I,
	DTC_C_EXPR_UNARY_F2J,
	DTC_C_EXPR_UNARY_F2D,
	DTC_C_EXPR_UNARY_D2I,
	DTC_C_EXPR_UNARY_D2J,
	DTC_C_EXPR_UNARY_D2F,
	DTC_C_EXPR_BINARY_ADD,	/* Binary */
	DTC_C_EXPR_BINARY_SUB,
	DTC_C_EXPR_BINARY_MUL,
	DTC_C_EXPR_BINARY_DIV,
	DTC_C_EXPR_BINARY_REM,
	DTC_C_EXPR_BINARY_AND,
	DTC_C_EXPR_BINARY_OR,
	DTC_C_EXPR_BINARY_XOR,
	DTC_C_EXPR_BINARY_SHL,
	DTC_C_EXPR_BINARY_SHR,
	DTC_C_EXPR_BINARY_USHR,
	DTC_C_EXPR_BINARY_EQ,
	DTC_C_EXPR_BINARY_NE,
	DTC_C_EXPR_BINARY_LE,
	DTC_C_EXPR_BINARY_LT,
	DTC_C_EXPR_BINARY_GE,
	DTC_C_EXPR_BINARY_GT,
	DTC_C_EXPR_CALL,			/* Call */

	// The number of the C expresson types
	DTC_C_NUM_OF_EXPR,
	
} DtcCExprType_t;

typedef vector<DtcCExpression*> DtcCExprList_t;

///////////////////////////////////////////////////////////////////////
// DtcCExpression Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcCExpression {

protected:
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcCExprType_t m_nType;

public:
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////	

	DtcCExpression(DtcCExprType_t nType = DTC_C_EXPR_UNKNOWN);
	
	virtual ~DtcCExpression();

	///////////////////////////////////////////////////////////////////
	// Accessor & Mutator Declaration/Definition
	///////////////////////////////////////////////////////////////////

	/** 
	 * This function will return the type of this expression.
	 *
	 * @return the type of the expression
	 */
	inline DtcCExprType_t GetType()				{ return m_nType; 	};

	/** 
	 * This function will be used to set the type of this expression.
	 *
	 * @param the type of the expression
	 */
	inline void SetType(DtcCExprType_t nType)		{ m_nType = nType; 	};

	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////
	
	virtual void Write(ostream &pOut) = 0;

	friend ostream &operator << (ostream &pOut, DtcCExpression *pExpr);
};

///////////////////////////////////////////////////////////////////////
// DtcCIdExpr Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcCIdExpr : public DtcCExpression {

private:
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////	
	
	DtcCSymbol *m_pSymbol;

public:
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////	
	
	DtcCIdExpr(DtcCSymbol *pSymbol = NULL);
	
	virtual ~DtcCIdExpr();

	///////////////////////////////////////////////////////////////////
	// Accessor & Mutator Declaration/Definition
	///////////////////////////////////////////////////////////////////

	/** 
	 * This function will return the ID symbol.
	 *
	 * @return the ID symbol
	 */	 
	inline DtcCSymbol *GetSymbol()		{ return m_pSymbol;		}

	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////
	
	virtual void Write(ostream &pOut);
};

///////////////////////////////////////////////////////////////////////
// DtcCUnaryExpr Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcCUnaryExpr : public DtcCExpression {
	
private:
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////	

	// Unary expression
	DtcCExpression *m_pExpr; 

public:
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////	
	
	DtcCUnaryExpr(DtcCExprType_t nType, DtcCExpression *pExpr);

	virtual ~DtcCUnaryExpr();

	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////
	
	virtual void Write(ostream &pOut);
};

///////////////////////////////////////////////////////////////////////
// DtcCBinaryExpr Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcCBinaryExpr : public DtcCExpression {

private:
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////	

	// Left and right operand
	DtcCExpression *m_pLhsExpr;
	
	DtcCExpression *m_pRhsExpr;

public:
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcCBinaryExpr(DtcCExprType_t nType, DtcCExpression *pLhsExpr, DtcCExpression *pRhsExpr);
	
	virtual ~DtcCBinaryExpr();

	///////////////////////////////////////////////////////////////////
	// Accessor & Mutator Declaration/Definition
	///////////////////////////////////////////////////////////////////

	/**
	 * This function will return the left-hand side expression of the binary expression.
	 *
	 * @return the left-hand side expression
	 */
	inline DtcCExpression *GetLhsExpr()		{ return m_pLhsExpr; 	}

	/**
	 * This function will return the right-hand side expression of the binary expression.
	 *
	 * @return the right-hand side expression
	 */
	inline DtcCExpression *GetRhsExpr()		{ return m_pRhsExpr; 	}

	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////
	
	virtual void Write(ostream &pOut);
};

///////////////////////////////////////////////////////////////////////
// DtcCCallExpr Class Declaration
///////////////////////////////////////////////////////////////////////

class DtcCCallExpr : public DtcCExpression {
	
private:
	///////////////////////////////////////////////////////////////////
	// Member Variable Declaration
	///////////////////////////////////////////////////////////////////	
	
	// Function ID
	DtcCIdExpr *m_pIdExpr;
	
	// Function parameters
	DtcCExprList_t m_rParamExprList;

public:
	///////////////////////////////////////////////////////////////////
	// Constructor & Destructor Declaration
	///////////////////////////////////////////////////////////////////
	
	DtcCCallExpr(DtcCIdExpr *pIdExpr);

	virtual ~DtcCCallExpr();

	///////////////////////////////////////////////////////////////////
	// Accessor & Mutator Declaration/Definition
	///////////////////////////////////////////////////////////////////

	/**
	 * This function will be used to append a function parameter expression to 
	 * the call expression.
	 *
	 * @param pParamExpr a parameter expression to be added
	 */
	inline void AppendFuncParam(DtcCExpression *pParamExpr) {
		m_rParamExprList.push_back(pParamExpr);
	}

	///////////////////////////////////////////////////////////////////
	// Debugging Function Declaration
	///////////////////////////////////////////////////////////////////
	
	virtual void Write(ostream &pOut);
};

#endif
