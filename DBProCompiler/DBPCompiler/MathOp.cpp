// MathOp.cpp: implementation of the CMathOp class.
//
//////////////////////////////////////////////////////////////////////
#include "ParserHeader.h"

// Custom Includes
#include "VarTable.h"
#include "DataTable.h"
#include "Statement.h"
#include "LabelTable.h"
#include "StructTable.h"
#include "ParseFunction.h"
#include "ParseInstruction.h"
#include "InstructionTable.h"
#include "DBPCompiler.h"

// for the POW function
#include "math.h"

// External Class Pointers
extern CVarTable *g_pVarTable;
extern CDataTable *g_pDataTable;
extern CDataTable *g_pStringTable;
extern CStructTable *g_pStructTable;
extern CInstructionTable *g_pInstructionTable;
extern CLabelTable* g_pLabelTable;
extern CDBPCompiler* g_pDBPCompiler;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMathOp::CMathOp()
{
	m_dwLineNumber=0;

	m_Result.m_pStringToken=NULL;
	m_Result.m_pAdditionalOffset=NULL;
	m_Result.m_dwType=0;
	m_Result.m_dwDataOffset=0;

	m_dwOffsetLValueTypeValue=0;
	m_bConcatFlagUsed=false;

	m_dwMathSymbol=0;
	m_pRightMathOp=NULL;
	m_pLeftMathOp=NULL;

	m_pStatement=NULL;
	m_pNext=NULL;
}

CMathOp::~CMathOp()
{
	SAFE_DELETE(m_Result.m_pStringToken);
	SAFE_DELETE(m_Result.m_pAdditionalOffset);
	SAFE_DELETE(m_pRightMathOp);
	SAFE_DELETE(m_pLeftMathOp);
	SAFE_DELETE(m_pStatement);
	SAFE_DELETE(m_pNext);
}

void CMathOp::Add(CMathOp* pNext)
{
	if(m_pNext==NULL)
		m_pNext = pNext;
	else
		m_pNext->Add(pNext);
}

void CMathOp::SetResult(LPSTR pString, DWORD dwType, DWORD dwDataOffset)
{
	SAFE_DELETE(m_Result.m_pStringToken);
	m_Result.m_pStringToken = new CStr(pString);
	m_Result.m_dwType = dwType;
	m_Result.m_dwDataOffset = dwDataOffset;
}

void CMathOp::SetResultData(CResultData ResultData)
{
	SAFE_DELETE(m_Result.m_pStringToken);
	m_Result = ResultData;
	if ( ResultData.m_pStringToken ) m_Result.m_pStringToken = new CStr(ResultData.m_pStringToken->GetStr());
	if ( ResultData.m_pAdditionalOffset ) m_Result.m_pAdditionalOffset = new CStr(ResultData.m_pAdditionalOffset->GetStr());
}

void CMathOp::SetArrayOffsetResult(LPSTR pString)
{
	SAFE_DELETE(m_Result.m_pAdditionalOffset);
	m_Result.m_pAdditionalOffset = new CStr(pString);
}

CStr* CMathOp::FindResultStringTokenForDBM(void)
{
	if(m_pNext==NULL && m_Result.m_pStringToken)
	{
		// Translate result value
		if(TranslateStringTokenForDBM()==false)
		{
			return NULL;
		}
		return m_Result.m_pStringToken;
	}
	else
	{
		if(m_pNext)
			return m_pNext->FindResultStringTokenForDBM();
		else
			return NULL;
	}
}

CResultData* CMathOp::FindResultDataForDBM(void)
{
	if(m_pNext==NULL && m_Result.m_pStringToken)
	{
		// Translate result value
		if(TranslateStringTokenForDBM()==false)
		{
			return NULL;
		}
		return &m_Result;
	}
	else
	{
		if(m_pNext)
			return m_pNext->FindResultDataForDBM();
		else
			return NULL;
	}
}

DWORD CMathOp::FindResultTypeValueForDBM(void)
{
	if(m_pNext==NULL && m_Result.m_pStringToken)
	{
		// Result value
		return m_Result.m_dwType;
	}
	else
	{
		if(m_pNext)
			return m_pNext->FindResultTypeValueForDBM();
		else
			return 0;
	}
}

CResultData* CMathOp::GetResultData(void)
{
	return &m_Result;
}

CResultData* CMathOp::FindResultData(void)
{
	if(m_pNext==NULL && m_Result.m_pStringToken)
	{
		// Translate result value
		if(TranslateStringTokenForDBM()==false)
		{
			return NULL;
		}
		// Result
		return &m_Result;
	}
	else
	{
		if(m_pNext)
			return m_pNext->FindResultData();
		else
			return NULL;
	}
}

bool CMathOp::TranslateStringTokenForDBM(void)
{
	// Ensure string present..
	if(m_Result.m_pStringToken==NULL)
	{
		// Nothing to translate
		return true;
	}

	// Translate and resolve item described by string
	CResultData* pResultPtr = GetResultData();
	m_Result.m_pStringToken->TranslateForDBM(pResultPtr);
	if(m_Result.m_pAdditionalOffset) m_Result.m_pAdditionalOffset->TranslateForDBM(pResultPtr);

	// Complete
	return true;
}

bool CMathOp::DoValue(CStr* pExpression)
{
	DWORD dwType=0;
	DWORD dwExpValueType=0;

	// Before processing, crop spaces, tabs and equal brackets
	do { pExpression->EatEdgeSpacesandTabs(NULL);
	} while(pExpression->CropEqualEdgeBrackets(NULL)==true);

	// Upper-case comparisons
	CStr UpperExpression = CStr(pExpression->GetStr());
	UpperExpression.MakeUpper();

	// If starts with "*", it can only be a pointer assignment
	DWORD dwPosition=0, dwMathSymbol=0, dwMathSymbolWidth=0, dwSciNot=0;
	if(UpperExpression.GetChar(0)!='*')
	{
		// Find Best Math Symbol
		if(FindHighestPres(&UpperExpression, &dwPosition, &dwMathSymbol, &dwMathSymbolWidth, &dwSciNot)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'CMathOp::DoValue::FindHighestPres'");
			return false;
		}

		// LEEFIX - 171002 - If negative symbol used as first char (ie -99 or -a#), only literals are direct values
		if(UpperExpression.GetChar(0)=='-')
		{
			DWORD dwType=0;
			if(IsLiteral(&UpperExpression, &dwType)==false)
			{
				dwPosition=0;
				dwMathSymbol=5;
				dwMathSymbolWidth=1;
			}
		}
	}

	// Record Line Number at math operation
	DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
	SetLineNumber(StatementLineNumber);

	// Direct Value or another MathOp split
	if(dwMathSymbol==0)
	{
		// No math symbol, only one value here
		m_dwMathSymbol=0;

		// Determine if function, array, variable or value
		CStr* pExpressionValue = NULL;
		if(pExpression)
		{
			// Create the exp value
			pExpressionValue = new CStr(1);
			pExpressionValue->SetText(pExpression);

			// leeadd - U71 - 111008 - if scientific notation
			if ( dwSciNot>0 )
			{
				// find seperation between first and second values (XX e [+/-] YY)
				pExpressionValue->ResolveSciNot();
			}

			// Check what kind of value it is
			dwType=0;
			dwExpValueType=0;

			if(g_pStatementList->GetAllowLabelAsValue()==true)
			{
				if(dwExpValueType==0 && IsLabel(pExpressionValue))			dwExpValueType=5;
			}
			if(dwExpValueType==0 && IsFunction(pExpressionValue))			dwExpValueType=1;
			if(dwExpValueType==0 && IsReserved(pExpressionValue))			dwExpValueType=6;
			if(dwExpValueType==0 && IsLiteral(pExpressionValue, &dwType))	dwExpValueType=2;
			if(dwExpValueType==0 && IsSingleVariable(pExpressionValue))		dwExpValueType=3;
			if(dwExpValueType==0 && IsComplexVariable(pExpressionValue))	dwExpValueType=4;
			if(dwExpValueType>0)
			{
				if(dwExpValueType==1)
				{
					if(DoValueFunction(pExpressionValue)==false)
					{
						SAFE_DELETE(pExpressionValue);
						return false;
					}
				}
				if(dwExpValueType==2)
				{
					if(DoValueLiteral(pExpressionValue, dwType)==false)
					{
						g_pErrorReport->AddErrorString("Failed to DoValueLiteral");
						SAFE_DELETE(pExpressionValue);
						return false;
					}
				}
				if(dwExpValueType==3)
				{
					if(DoValueSingleVariable(pExpressionValue)==false)
					{
						g_pErrorReport->AddErrorString("Failed to DoValueSingleVariable");
						SAFE_DELETE(pExpressionValue);
						return false;
					}
				}
				if(dwExpValueType==4)
				{
					if(DoValueComplexVariable(pExpressionValue)==false)
					{
						SAFE_DELETE(pExpressionValue);
						return false;
					}
				}
				if(dwExpValueType==5)
				{
					if(DoValueLabel(pExpressionValue)==false)
					{
						g_pErrorReport->AddErrorString("Failed to DoValueLabel");
						SAFE_DELETE(pExpressionValue);
						return false;
					}
				}
				if(dwExpValueType==6)
				{
					DWORD dwLine = g_pStatementList->GetTokenLineNumber();
					g_pErrorReport->SetError(dwLine, ERR_SYNTAX+60, pExpressionValue->GetStr());
					SAFE_DELETE(pExpressionValue);
					return false;
				}
				SAFE_DELETE(pExpressionValue);
			}
		}

		// If still have this expression, report error
		if(pExpressionValue)
		{
			DWORD dwLine = g_pStatementList->GetTokenLineNumber();
			g_pErrorReport->SetError(dwLine, ERR_SYNTAX+1, pExpressionValue->GetStr());
			SAFE_DELETE(pExpressionValue);
			return false;
		}
	}
	else
	{
		// Get Both Sides as strings
		LPSTR pLeftText = pExpression->GetLeftOfPosition(dwPosition);
		LPSTR pRightText = pExpression->GetRightOfPosition(dwPosition+dwMathSymbolWidth);
		CStr StrLeft = CStr(pLeftText);
		CStr StrRight = CStr(pRightText);
		// LEEFIX - 201102 - Added prechop space eat as a string such as " -1" would actually chop before the minus
		// LEEFIX - 211102 - Put dwLeftEaten/dwRightEaten as the rightmostpos was being calculated incorrectly for later
		DWORD dwLeftEaten=0, dwRightEaten=0;
		StrLeft.EatEdgeSpacesandTabs(&dwLeftEaten);
		StrRight.EatEdgeSpacesandTabs(&dwRightEaten);
		DWORD dwLeftMostPos = ChopOffOneItemFromRight(&StrLeft);
		DWORD dwRightMostPos = ChopOffOneItemFromLeft(&StrRight);
		dwLeftMostPos+=dwLeftEaten;
		dwRightMostPos+=dwRightEaten;
		StrLeft.EatEdgeSpacesandTabs(NULL);
		StrRight.EatEdgeSpacesandTabs(NULL);
		dwRightMostPos=dwPosition+dwMathSymbolWidth+dwRightMostPos;
		SAFE_DELETE(pRightText);
		SAFE_DELETE(pLeftText);

// LEEFIX - 230604 - IS IT NEEDED ANY MORE? AS -X IS DONE ELSEWHEER (I THINK)
//		// DOES THIS WORK FOR ALL UNARY OPERATORS?
//		// A blank literal is translated to a numeric zero (so -99 becomes 0-99)
//		if(strcmp(StrLeft.GetStr(),"")==NULL)
//			StrLeft.SetText("0");

		// for NOT cases, copy string to left side
		if(dwMathSymbol==43)
		{
			// A NOT B [B-this part ignored in compiler]
			StrLeft.SetText(StrRight.GetStr());

			// leefix - 250604 - u54 - wiped this out as user functions could be called twice!
			StrRight.SetText("");
		}
		else
		{
			// leefix - 230604 - u54 - in all other on-uniary cases, a missing operand is an error
			// except dwMathSymbol==5 (negative value)
			if ( dwMathSymbol==5 )
			{
				// A blank literal is translated to a numeric zero (so -99 becomes 0-99)
				if(strcmp(StrLeft.GetStr(),"")==NULL)
					StrLeft.SetText("0");
			}
			else
			{
				if(strcmp(StrLeft.GetStr(),"")==NULL
				|| strcmp(StrRight.GetStr(),"")==NULL )
				{
					g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+59);
					SAFE_DELETE(m_pRightMathOp);
					SAFE_DELETE(m_pLeftMathOp);
					return false;
				}
			}
		}

		// Found a math symbol (xxx * xxx)
		m_dwMathSymbol=dwMathSymbol;

		// Traverse each side for final values
		m_pLeftMathOp = new CMathOp;
		if(m_pLeftMathOp)
		{
			if(m_pLeftMathOp->DoValue(&StrLeft)==false)
			{
// lee - 150306 - u60b3 - surfaced at end user level, replaced with line number error
//				g_pErrorReport->AddErrorString("Failed to 'DoValue::m_pLeftMathOp->DoValue'");
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+1, StrLeft.GetStr() );
				SAFE_DELETE(m_pLeftMathOp);
				return false;
			}
		}
		m_pRightMathOp = new CMathOp;
		if(m_pRightMathOp)
		{
			if(m_pRightMathOp->DoValue(&StrRight)==false)
			{
// lee - 150306 - u60b3 - surfaced at end user level, replaced with line number error
//				g_pErrorReport->AddErrorString("Failed to 'DoValue::m_pRightMathOp->DoValue'");
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+1, StrRight.GetStr() );
				SAFE_DELETE(m_pLeftMathOp);
				SAFE_DELETE(m_pRightMathOp);
				return false;
			}
		}

		// Conversion Table A + B = C type result handling
		DWORD dwLType=m_pLeftMathOp->FindResultTypeValueForDBM();
		DWORD dwRType=m_pRightMathOp->FindResultTypeValueForDBM();

		// Check if right param has offset-lvalue-typevalue
		if(m_pRightMathOp->GetResultOffsetLValueTypeValue()>0)
			m_dwOffsetLValueTypeValue=m_pRightMathOp->GetResultOffsetLValueTypeValue();

		// If L-Value is non-specified userdatatype, assign LValue gathered from offset field
		if(dwLType==1001 && m_dwOffsetLValueTypeValue>0)
		{
			dwLType=100+m_dwOffsetLValueTypeValue;
		}
		if(dwLType==1101 && m_dwOffsetLValueTypeValue>0)
		{
			dwLType=100+m_dwOffsetLValueTypeValue;
		}

		// Determine Result Type
		bool bSkipMathInputCasting=false;
// LEEFIX - 281102 - Type Mode can only be a number from 0-99
//		DWORD dwTypeMode=dwLType%100;
		DWORD dwTypeMode=dwLType%100;

//lee, why does 'not equal' add three params to stack????

		// Determine if cast required
		if(dwLType==1 && dwRType==1) dwTypeMode=1;
		if(dwLType==2 && dwRType==2) dwTypeMode=2;
		if(dwLType==4 && dwRType==4) dwTypeMode=4;
		if(dwLType==5 && dwRType==5) dwTypeMode=5;
		if(dwLType==6 && dwRType==6) dwTypeMode=6;
		if(dwLType==7 && dwRType==7) dwTypeMode=7;
		if(dwLType==9 || dwRType==9) dwTypeMode=9;
		if(dwLType==2 || dwRType==2) dwTypeMode=2;
		if(dwLType==8 || dwRType==8) dwTypeMode=8;
		// LEEFIX - 281102 - Added additional checks to arrive at correct result type
		if(dwLType==102 || dwRType==102)
			dwTypeMode=2;
		if(dwLType==109 || dwRType==109) dwTypeMode=9;
		if(dwLType==108 || dwRType==108) dwTypeMode=8;
		if((dwLType==3 || dwLType==103) && (dwRType==3 || dwRType==103))
		{
			// Comparisons return Integer, not type-specific value
			bSkipMathInputCasting=true;
			if(m_dwMathSymbol>=22 && m_dwMathSymbol<=27)
				dwTypeMode=1;
			else
			{
				// lee - 290306 - u6rc3 - only ADDITION is supported by strings
				if ( m_dwMathSymbol==4 )
				{
					dwTypeMode=3;
				}
				else
				{
					// tried to subtract or power two strings
					g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+11);
					SAFE_DELETE(m_pRightMathOp);
					SAFE_DELETE(m_pLeftMathOp);
					return false;
				}
			}
		}
		if((dwLType%100>=4 && dwLType%100<=7) || (dwLType%100>=4 && dwLType%100<=7))
		{
			// Comparisons against BOOL,BYTE,WORD,DWORD should use 4 bytes
			//if(m_dwMathSymbol>=22 && m_dwMathSymbol<=27)
			// Plus any sort of maths needs to hold a value greater than its datatype
			// leefix - 040803 - except where floats or double floats are concerned..
			if ( dwLType%100!=2 && dwLType%100!=8 && dwRType%100!=2 && dwRType%100!=8 )
				dwTypeMode=7;
		}
		if(dwLType==10 || dwRType==10) dwTypeMode=0;
		if(dwLType==20 || dwRType==20) dwTypeMode=0;

		// Maybe a range of 'unique' type values..
		if(dwLType>=1001 && dwLType==dwRType) dwTypeMode=dwLType;

		// leefix - 050803 - Attempts to assign or compare a type and non-type results in failure
		if(dwLType>=1001 && dwRType<1000) dwTypeMode=0;
		if(dwRType>=1001 && dwLType<1000) dwTypeMode=0;

		// Types are in-compatable
		if(dwTypeMode==0)
		{
			LPSTR pL = m_pLeftMathOp->FindResultStringTokenForDBM()->GetStr();
			LPSTR pR = m_pRightMathOp->FindResultStringTokenForDBM()->GetStr();
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+2, pL, pR);
			SAFE_DELETE(m_pRightMathOp);
			SAFE_DELETE(m_pLeftMathOp);
			return false;
		}

		// Only cast if regular values, not pointers..
// LEEFIX - 281102 - This would miss something like ( 0 - array#(x) )
//		if(dwLType<100 && dwRType<100 && bSkipMathInputCasting==false)
		if((dwLType<100 || dwRType<100) && bSkipMathInputCasting==false)
		{
			// First Cast Left Or Right 'not' matching produced type
// LEEFIX - 281102 - As the values can be var or array, ensure this does not affect check
			DWORD dwRealLValue = dwLType%100;
			DWORD dwRealRValue = dwRType%100;
			if(dwRealLValue!=dwTypeMode || dwRealRValue!=dwTypeMode)
			{
				if(dwRealLValue!=dwTypeMode)
				{
					if(DoCastOnMathOp(&m_pLeftMathOp, dwTypeMode)==false)
					{
						LPSTR pL = m_pLeftMathOp->FindResultStringTokenForDBM()->GetStr();
						LPSTR pR = m_pRightMathOp->FindResultStringTokenForDBM()->GetStr();
						g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+2, pL, pR);
						SAFE_DELETE(m_pRightMathOp);
						SAFE_DELETE(m_pLeftMathOp);
						return false;
					}
				}
				if(dwRealRValue!=dwTypeMode)
				{
					if(DoCastOnMathOp(&m_pRightMathOp, dwTypeMode)==false)
					{
						LPSTR pL = m_pLeftMathOp->FindResultStringTokenForDBM()->GetStr();
						LPSTR pR = m_pRightMathOp->FindResultStringTokenForDBM()->GetStr();
						g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+2, pL, pR);
						SAFE_DELETE(m_pRightMathOp);
						SAFE_DELETE(m_pLeftMathOp);
						return false;
					}
				}
			}
		}
		else
		{
			// If L-Value is array, must cast to its actual datatype
			if(dwLType>100 && dwRType!=7 && dwRType!=3)
			{
				// leefix - U71 - 081208 - in the case of T(1).int * T(1).float, the float is cast to INT (making 400 * 0.5 = 0 wrong)
				// so we make a check to see if either is FLOAT vs INT and cast the integers to floats to make above work
				DWORD dwActualLType = dwLType-100;
				if ( dwLType==101 && dwRType==102 )
				{
					dwActualLType = 2;
					if(DoCastOnMathOp(&m_pLeftMathOp, dwActualLType)==false)
					{
						LPSTR pL = m_pLeftMathOp->FindResultStringTokenForDBM()->GetStr();
						LPSTR pR = m_pRightMathOp->FindResultStringTokenForDBM()->GetStr();
						g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+3, pL, pR);
						SAFE_DELETE(m_pRightMathOp);
						SAFE_DELETE(m_pLeftMathOp);
						return false;
					}
				}
				else
				{
					if(DoCastOnMathOp(&m_pRightMathOp, dwActualLType)==false)
					{
						LPSTR pL = m_pLeftMathOp->FindResultStringTokenForDBM()->GetStr();
						LPSTR pR = m_pRightMathOp->FindResultStringTokenForDBM()->GetStr();
						g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+3, pL, pR);
						SAFE_DELETE(m_pRightMathOp);
						SAFE_DELETE(m_pLeftMathOp);
						return false;
					}
				}

				// Get Actual Type Value
				if(m_dwMathSymbol>=22 && m_dwMathSymbol<=27)
				{
					// No need to adjust result type for comparisons!
				}
				else
				{
					// If Array succeeds in recasting right, then output maybe changed
					dwTypeMode=dwActualLType;
				}
			}

			// LEEFIX - 101102 - Ensure R-Value is cast to Desired Type (if comparison) ie a#>b(0)
			if(m_dwMathSymbol>=22 && m_dwMathSymbol<=27 && bSkipMathInputCasting==false)
			{
				if(dwRType%100 != dwTypeMode%100)
				{
					if(DoCastOnMathOp(&m_pRightMathOp, dwTypeMode)==false)
					{
						LPSTR pL = m_pLeftMathOp->FindResultStringTokenForDBM()->GetStr();
						LPSTR pR = m_pRightMathOp->FindResultStringTokenForDBM()->GetStr();
						g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+3, pL, pR);
						SAFE_DELETE(m_pRightMathOp);
						SAFE_DELETE(m_pLeftMathOp);
						return false;
					}
				}
			}

			// LEEFIX - 040803 - Ensure L-Value is ALSO cast to Desired Type (if comparison) ie byte(10)>byte(10)
			if(m_dwMathSymbol>=22 && m_dwMathSymbol<=27 && bSkipMathInputCasting==false)
			{
				if(dwLType%100 != dwTypeMode%100)
				{
					if(DoCastOnMathOp(&m_pLeftMathOp, dwTypeMode)==false)
					{
						LPSTR pL = m_pLeftMathOp->FindResultStringTokenForDBM()->GetStr();
						LPSTR pR = m_pRightMathOp->FindResultStringTokenForDBM()->GetStr();
						g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+3, pL, pR);
						SAFE_DELETE(m_pRightMathOp);
						SAFE_DELETE(m_pLeftMathOp);
						return false;
					}
				}
			}
		}
		
		// Once params are properly cast, comparisons ALWAYS return integer
		if(m_dwMathSymbol>=22 && m_dwMathSymbol<=27)
		{
			// Comparison always returns an integer result
			dwTypeMode=1;
		}

		// Ensure temp return type holds direct value
		if(dwTypeMode>100 && dwTypeMode<1000) dwTypeMode-=100;

		// Return value to a Temporary Produced Item
		CStr TempVarToken(1);
		ProduceNewTempToken(&TempVarToken, dwTypeMode);

		// Take Remainder of String and combine with temp var token from math result
		LPSTR pRemainderLeftText = pExpression->GetLeftOfPosition(dwLeftMostPos);
		LPSTR pRemainderRightText = pExpression->GetRightOfPosition(dwRightMostPos);
		CStr StrRemainderLeft = CStr(pRemainderLeftText);
		CStr StrRemainderRight = CStr(pRemainderRightText);
		CStr strNewString(1);
		strNewString.SetText(&StrRemainderLeft);
		strNewString.AddText(&TempVarToken);
		strNewString.AddText(&StrRemainderRight);
		SAFE_DELETE(pRemainderLeftText);
		SAFE_DELETE(pRemainderRightText);

		// Store Result String Token
		SetResult(TempVarToken.GetStr(), dwTypeMode, 0);
		SetResultStruct(NULL);

		// When Right Operand L-Value found, record and send it back...
		DWORD dwDiscoveredOffsetLValueTypeValue = GetResultOffsetLValueTypeValue();
		if(m_pRightMathOp->GetResultOffsetLValueTypeValue()>0) dwDiscoveredOffsetLValueTypeValue = m_pRightMathOp->GetResultOffsetLValueTypeValue();

		// Traverse until no more
		CMathOp* pAnotherMathOp = new CMathOp;
		if(pAnotherMathOp)
		{
			// Travserse remaining maths
			pAnotherMathOp->m_dwOffsetLValueTypeValue = dwDiscoveredOffsetLValueTypeValue;
			if(pAnotherMathOp->DoValue(&strNewString)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoValue::pAnotherMathOp->DoValue'");
				SAFE_DELETE(m_pRightMathOp);
				SAFE_DELETE(m_pLeftMathOp);
				SAFE_DELETE(pAnotherMathOp);
				return false;
			}

			// If new math item redundant (it doesn't do maths, delete it)
			if(pAnotherMathOp->GetMathSymbol()==0)
			{
				SAFE_DELETE(pAnotherMathOp);
			}

			// Ensure Offset L-Value is not forgotten
			if(pAnotherMathOp && dwDiscoveredOffsetLValueTypeValue>0)
			{
				pAnotherMathOp->m_dwOffsetLValueTypeValue = dwDiscoveredOffsetLValueTypeValue;
			}

			// Add onto chain if still valid
			if(pAnotherMathOp) Add(pAnotherMathOp);
		}

		// Carry back Offset LValue Type Value
		if(pAnotherMathOp)
			m_dwOffsetLValueTypeValue = pAnotherMathOp->GetResultOffsetLValueTypeValue();
		else
			m_dwOffsetLValueTypeValue = dwDiscoveredOffsetLValueTypeValue;
	}

	// Complete
	return true;
}

bool CMathOp::DoCastOnMathOp(CMathOp** ppMathOp, DWORD dwTypeWant)
{
	// Produce Temp to hold cast
	CStr* pTempVarToken = new CStr("");
	ProduceNewTempToken(pTempVarToken, dwTypeWant);

	// Produce Code to specify new type (in math instruction)
	CStr* pTypeCodeStr = new CStr("");
	pTypeCodeStr->SetNumericText(dwTypeWant);

	// Value as it is originally
	CMathOp* pValueToCast = *ppMathOp;

	// Determine what I am ultimately casting into
	DWORD dwTypeHave = pValueToCast->FindResultTypeValueForDBM();

	if(dwTypeWant==501)
	{
		SAFE_DELETE(pTempVarToken);
		SAFE_DELETE(pTypeCodeStr);
		return true;
	}

	// Some types simply cannot be cast
	if(((dwTypeWant%100)==3 && (dwTypeHave%100)!=3) || ((dwTypeHave%100)==3 && (dwTypeWant%100)!=3))
	{
		SAFE_DELETE(pTempVarToken);
		SAFE_DELETE(pTypeCodeStr);
		return false;
	}

	// Some types are different, but dont need casting
	if((dwTypeWant==4 && dwTypeHave==5) || (dwTypeHave==4 && dwTypeWant==5))
	{
		SAFE_DELETE(pTempVarToken);
		SAFE_DELETE(pTypeCodeStr);
		return true;
	}

	// Ensure relative pointers to data merely revert to the datatype value it ultimately uses 
	if(dwTypeHave>100 && dwTypeHave<1000)
	{
		// indirect addressing 200-299
		if(dwTypeHave>200)
			dwTypeHave-=200;
		else
			dwTypeHave-=100;
	}
	if(dwTypeHave==1001) dwTypeHave=7;
	if(dwTypeHave==1101) dwTypeHave=7;

	// If no cast required, skip new cast task
	if(dwTypeHave==dwTypeWant)
	{
		SAFE_DELETE(pTempVarToken);
		SAFE_DELETE(pTypeCodeStr);
		*ppMathOp = pValueToCast;
		return true;
	}

	// Create a Cast Math Instruction
	CMathOp* pNewMath = new CMathOp;
	pNewMath->m_dwLineNumber=GetLineNumber();
	pNewMath->m_pLeftMathOp=pValueToCast;
	pNewMath->m_pRightMathOp = new CMathOp;
	pNewMath->m_pRightMathOp->DoValue(pTypeCodeStr);
	pNewMath->m_pStatement=NULL;

	// Produce result token as var
	pNewMath->SetResult(pTempVarToken->GetStr(), dwTypeWant, 0);
	pNewMath->SetResultStruct(NULL);
	SAFE_DELETE(pTempVarToken);

	// Types that are ptrs, can only be cast to actual datatypes
	if(dwTypeWant>=101 && dwTypeWant<=109) dwTypeWant-=100;

	// Work out Casting Math Symbol (ie 101-110 L to ??? 111-120 F to ???)
	pNewMath->m_dwMathSymbol=100+dwTypeWant+((dwTypeHave-1)*10);

	// Assign to original math chain
	*ppMathOp = pNewMath;

	// Free usages
	SAFE_DELETE(pTypeCodeStr);

	// Complete
	return true;
}

bool CMathOp::DoValueFunction(CStr* pExpressionValue)
{
	//
	//	TODO: This looks a lot like certain other methods and seems to duplicate some functionality. This method should
	//	      be reviewed and refactored.
	//

	// Process Value as Function
	CStr* pFunctionNameString = new CStr(1);
	CStr* pFunctionDataString = new CStr(1);
	pFunctionNameString->SetText(pExpressionValue);
	pFunctionDataString->SetText(pExpressionValue);
	DWORD dwPos = pFunctionDataString->FindFirstChar('(');
	LPSTR pLeft = pFunctionNameString->GetLeftOfPosition(dwPos);
	LPSTR pRight = pFunctionDataString->GetRightOfPosition(dwPos);
	pFunctionNameString->SetText(pLeft);
	pFunctionDataString->SetText(pRight);
	SAFE_DELETE(pLeft);
	SAFE_DELETE(pRight);

	// Get Details of instruction
	CInstructionTableEntry* pRef = g_pStatementList->GetInstructionRef();
	DWORD dwInstructionType = g_pStatementList->GetInstructionType();
	DWORD dwInstructionValue = g_pStatementList->GetInstructionValue();
	DWORD dwInstructionParamMax = g_pStatementList->GetInstructionParamMax();

	// Create param chain from param string
	m_pStatement = new CStatement;
	CParameter* pFirstParameter = NULL;
	if(m_pStatement->DoParameterListString(pFunctionDataString, &pFirstParameter)==false)
	{
		g_pErrorReport->SetError ( g_pStatementList->GetLineNumber(), ERR_SYNTAX+55, pFunctionNameString->GetStr() );
		SAFE_DELETE(pFunctionNameString);
		SAFE_DELETE(pFunctionDataString);
		SAFE_DELETE(pFirstParameter);
		SAFE_DELETE(m_pStatement);
		return false;
	}

	// Work out if right number of parameterd parsed
	DWORD dwParamCount=0;
	CParameter* pCurrent = pFirstParameter;
	while(pCurrent)
	{
		dwParamCount++;
		pCurrent = pCurrent->GetNext();
	}

	// pTrack kicks in on 'second' interation (search list bby entry, not by index)
	CInstructionTableEntry* pTrack = NULL;

	// Special invalid param reason
	int iInvalidParamReason=0;
	LPSTR pValidEntryStr = NULL;
	LPSTR pFunctionTypeStr = NULL;

	// Scan Each Matching Instruction and find match with parameters used, else error
	DWORD dwScoreBestMatch=0;
	DWORD dwBestScoreSoFar=0;
	DWORD StatementLineNumber = g_pStatementList->GetLineNumber();
	DWORD dwValidInstructionToUse=0;
	CInstructionTableEntry* pFirstEntryRef=NULL; 
	CInstructionTableEntry* pNextEntryRef=NULL; 
	DWORD dwValidInstructionValue=0, dwValidParamMax=0;
	DWORD dwTryingIndex=0;

	// LEEFIX - 201102 - CALL DLL has more than 16 instances, so increased to 32 instances..
	// aaronfix -211112 - There's no need for this...
	DWORD dwTryInstruction;
	auto entry = pRef ? pRef : g_pInstructionTable->GetEntryByIndex(dwInstructionValue);
	while(entry)
	{
		// Try This Instruction
		//DWORD dwTryInstruction = dwInstructionValue + dwTryingIndex;
		dwTryInstruction = entry->GetInternalID();
		if(pTrack) dwTryInstruction = pTrack->GetInternalID();

		// Check for parameter-mismatch
		bool bInValidParams=false;
		CInstructionTableEntry* pValidEntryRef=NULL; 
		CStr* pValidParamTypes = NULL;
		if(dwInstructionType==1)
		{
			dwValidInstructionValue=dwInstructionValue;
			dwValidParamMax=dwInstructionParamMax;
		}
		if(dwInstructionType==2)
			if(g_pInstructionTable->FindInstructionParams(dwTryInstruction, dwParamCount, &dwValidInstructionValue, &dwValidParamMax, &pValidParamTypes, &pValidEntryRef)==false)
				bInValidParams=true;
		if(dwInstructionType==3)
		{
			// leenote - 230306 - u6b4 - odd that there are two areas to parse and validate a user function call ( based on a=userfunc() and userfunc() )
			// if(g_pInstructionTable->FindUserFunctionParams(dwTryInstruction, dwParamCount, &dwValidInstructionValue, &dwValidParamMax, &pValidParamTypes, &pValidEntryRef)==false)
			if(g_pInstructionTable->FindUserFunctionParams(dwTryInstruction, dwParamCount, &dwValidInstructionValue, &dwValidParamMax, &pValidParamTypes, &pValidEntryRef)==true)
			{
				// lee - 230306 - u6b4 - copied from Statement.cpp line 5641 (handle UDT missue)
				// bInValidParams=true;
				if ( bInValidParams==false )
				{
					CDeclaration* pDecChain = pValidEntryRef->GetDecChain();
					if ( pDecChain )
					{
						CDeclaration* pDec = pDecChain->GetNext();
						CParameter* pCurrent = pFirstParameter;
						while ( pCurrent && pDec )
						{
							DWORD dwDataType = pCurrent->GetMathItem()->FindResultTypeValueForDBM();
							if ( dwDataType>=1001 )
							{
								// UDT vars passed in is ok
								if ( dwDataType==1001 )
								{
									CStr* pFunctionTypeName = pDec->GetType();
									CResultData* pParamData = pCurrent->GetMathItem()->FindResultData();
									if ( pParamData )
									{
										CStr* pParamTypeName = pParamData->m_pStruct->GetTypeName();
										if ( stricmp ( pFunctionTypeName->GetStr(), pParamTypeName->GetStr() )!=NULL )
										{
											iInvalidParamReason=2;
											SAFE_DELETE ( pValidEntryStr );
											pValidEntryStr = new char [ strlen(pValidEntryRef->GetName()->GetStr())+1 ];
											strcpy ( pValidEntryStr, pValidEntryRef->GetName()->GetStr() );
											SAFE_DELETE ( pFunctionTypeStr );
											pFunctionTypeStr = new char [ strlen(pFunctionTypeName->GetStr())+1 ];
											strcpy ( pFunctionTypeStr, pFunctionTypeName->GetStr() );
											//g_pErrorReport->SetError(g_pStatementList->GetTokenLineNumber(), ERR_SYNTAX+8, pValidEntryRef->GetName()->GetStr(), pFunctionTypeName->GetStr());
											bInValidParams=true;
											break;
										}
									}
								}

								// UDT arrays not supported
								if ( dwDataType==1101 )
								{
									iInvalidParamReason=3;
									//g_pErrorReport->SetError(g_pStatementList->GetTokenLineNumber(), ERR_SYNTAX+64);
									bInValidParams=true;
									break;
								}
							}
							pCurrent = pCurrent->GetNext();
							pDec = pDec->GetNext();
						}
					}
				}
			}
			else
			{
				// lee - 180406 - u6rc10 - as originally done, if not find function, also invalid
				bInValidParams=true;
			}
		}

		// Ensure first instruction entry ref is recorded
		if(pFirstEntryRef==NULL)
			pFirstEntryRef=pValidEntryRef;
		else
			pNextEntryRef=pValidEntryRef;

		// If instruction name matches original, continue
		if(pFirstEntryRef && pNextEntryRef)
			if(g_pInstructionTable->CompareInstructionNames(pFirstEntryRef, pNextEntryRef)==false)
				break;

		// Validate type of parameters to call
		if(pFirstParameter && pValidParamTypes)
		{
			dwScoreBestMatch=0;
			if(pFirstParameter->ValidateWithCorrectCall(pValidParamTypes, &dwScoreBestMatch, 0)==false)
			{
				bInValidParams=true;
			}
		}
		else
		{
			// if parse-has no params and function HAS params, no dice
			if(pFirstParameter==NULL)
			{
				if(pValidEntryRef->GetParamMax()>0)
				{
					bInValidParams=true;
				}
			}
		}

		// Must have a return type, or ref function cannot be a function
		if(pValidEntryRef)
		{
			if(pValidEntryRef->GetReturnParam()==0)
			{
				iInvalidParamReason=1;
				bInValidParams=true;
			}
		}

		// Store instruction that passes all checks
		if(bInValidParams==false)
		{
			if(dwScoreBestMatch>=dwBestScoreSoFar)
			{
				dwBestScoreSoFar=dwScoreBestMatch;
				dwValidInstructionToUse=dwValidInstructionValue;
				if(pValidEntryRef) pRef=pValidEntryRef;
				dwScoreBestMatch=0;
			}
		}

		// Try again with next matching instruction
		if(pTrack==NULL) pTrack = pValidEntryRef;
		if(pTrack)
		{
			pTrack = pTrack->GetNext();
			if(pTrack==NULL) break;
		}

		// Next try..
		entry = entry->GetNext();
		dwTryingIndex++;
	}
	if(dwValidInstructionToUse>0)
	{
		// Use This Instruction (values retained as left immediately)
		dwValidInstructionValue=dwValidInstructionValue;
		dwValidParamMax=dwValidParamMax;
	}
	else
	{
		// Parameter-mismatch
		if(iInvalidParamReason==0)
		{
			CStr* pParamDesc=NULL;
			if(pRef) pParamDesc=pRef->GetFullParamDesc();
			if(pParamDesc)
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+8, pFunctionNameString->GetStr(), pParamDesc->GetStr());
			else
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+4, pFunctionNameString->GetStr());
			
		}
		else
		{
			switch(iInvalidParamReason)
			{
				case 1 :	// Function does not return a value and it should as this is a function!
							g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+46, pFunctionNameString->GetStr());
							break;
				case 2 :	// Function dec error
							g_pErrorReport->SetError(g_pStatementList->GetTokenLineNumber(), ERR_SYNTAX+8, pValidEntryStr, pFunctionTypeStr);
							break;

				case 3 :	// Function dec error
							g_pErrorReport->SetError(g_pStatementList->GetTokenLineNumber(), ERR_SYNTAX+64);
							break;
			}
		}

		SAFE_DELETE(pFunctionNameString);
		SAFE_DELETE(pFunctionDataString);
		SAFE_DELETE(pFirstParameter);
		SAFE_DELETE(m_pStatement);
		return false;
	}

	// free usages
	SAFE_DELETE(pValidEntryStr);
	SAFE_DELETE(pFunctionTypeStr);

	// Run through parameters of validated user function and make any required casts
	if(pRef)
	{
		if(pFirstParameter->CastAllParametersToInstruction(pRef)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoValueFunction::CastAllParametersToInstruction'");
			SAFE_DELETE(pFirstParameter);
			return false;
		}
	}

	// Type Of Result
	DWORD dwTypeValue=7;
	if(pRef) dwTypeValue=pRef->GetReturnParam();

	// Create temp symbol and use as return data store for function
	CStr* pResultStr = new CStr("");
	if(dwTypeValue>0)
	{
		CStr TempVarToken(1);
		ProduceNewTempToken(&TempVarToken, dwTypeValue);
		pResultStr->SetText("");
		pResultStr->AddText(&TempVarToken);
	}

	// Also write result param to this math object
	SetResult(pResultStr->GetStr(), dwTypeValue, 0);
	SetResultStruct(NULL);

	// Construct full label name for function
	CStr* pFullLabelName = new CStr("$label ");
	pFullLabelName->AddText(pFunctionNameString->GetStr());

	// Complete Object Data
	CParseInstruction *pInstruction = new CParseInstruction();
	pInstruction->SetType(dwInstructionType);
	pInstruction->SetValue(dwInstructionValue);
	pInstruction->SetParamMax(dwInstructionParamMax);
	pInstruction->SetParameter(pFirstParameter);
	pInstruction->SetLineNumber(StatementLineNumber);
	pInstruction->SetReturnParameter(pResultStr);
	pInstruction->SetLabelParam(pFullLabelName);
	pInstruction->SetInstructionRef(pRef);
	m_pStatement->SetData(StatementLineNumber, 11, (void*)pInstruction);

	// Clear memory usage
	SAFE_DELETE(pFunctionNameString);
	SAFE_DELETE(pFunctionDataString);

	// Complete
	return true;
}

bool CMathOp::CalculateDataOffsetAndTypeFromFieldString(CStr* pVarName, DWORD dwArrayType, CStr* pFieldData, DWORD* pdwSize, DWORD* pdwLType, DWORD* pdwSizeOfWholeType, CStructTable** ppStruct)
{
	// Get Statement Line
	DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();

	// Create name to begin structure search
	CStr* pFirstName = NULL;
	if(dwArrayType==1)
	{
		pFirstName = new CStr("&");
		pFirstName->AddText(pVarName->GetStr());
	}
	else
		pFirstName = new CStr(pVarName->GetStr());
	
	if(pFirstName->Length()==0)
	{
		g_pErrorReport->AddErrorString("Failed to 'Calculate DataOffsetAndTypeFromFieldString::pFirstName->Length()==0'");
		SAFE_DELETE(pFirstName);
		return false;
	}

	// Eat spaces and tabs
	pFirstName->EatEdgeSpacesandTabs(NULL);

	// Determine type of this variable
	LPSTR pTypeName=NULL;
	if(g_pVarTable->FindVariableExist(pFirstName->GetStr(), dwArrayType)==true)
	{
		if(g_pVarTable->FindTypeOfVariable(pFirstName->GetStr(), dwArrayType, &pTypeName)==false)
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+5, pFirstName->GetStr());
			SAFE_DELETE(pFirstName);
			SAFE_DELETE(pTypeName);
			return false;
		}
	}
	else
	{
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+6, pFirstName->GetStr());
		SAFE_DELETE(pFirstName);
		return false;
	}

	// Store Size of overall type
	*pdwSizeOfWholeType = g_pStructTable->GetSizeOfType(pTypeName);
	
	// If simple array, take array element type
	DWORD dwTrackLastValidLValueFieldType=g_pVarTable->GetBasicTypeValue(pTypeName);

	// Proceed to find all field names
	CStructTable* pLastStruct = NULL;
	DWORD dwTrackOffsetToActualData=0;
	int iStartNewName=0;
	DWORD dwEndNewName=0;
	DWORD length=pFieldData->Length();
	for(DWORD n=0; n<length; n++)
	{
		if(iStartNewName==-1)
		{
			if(pFieldData->GetChar(n)=='.')
			{
				// Record Start Of New Name
				iStartNewName=n+1;
			}
			else
			{
				char pErrorDetail[512];
				wsprintf(pErrorDetail, "Failed to 'Calculate DataOffsetAndTypeFromFieldString' : %s %s %s %s", pVarName->GetStr(), pFieldData->GetStr(), pFirstName, pTypeName );
				g_pErrorReport->AddErrorString(pErrorDetail);
				SAFE_DELETE(pFirstName);
				SAFE_DELETE(pTypeName);
				return false;
			}
		}
		else
		{
			if((pFieldData->IsAlphaNumericLabel(n)==false
			&& pFieldData->IsSpaceCharacter(n)==false)
			|| n==length-1)
			{
				// Determine real end
				if(n==length-1)
					dwEndNewName=n+1;
				else
					dwEndNewName=n;

				// Get fieldname
				CStr* pFieldname = new CStr(1);
				for(DWORD o=iStartNewName; o<dwEndNewName; o++)
					pFieldname->AddChar(pFieldData->GetChar(o));

				// Eat spaces and tabs form field name
				pFieldname->EatEdgeSpacesandTabs(NULL);

				// Eat spaces between name and symbol
				for(; n<length; n++)
					if(pFieldData->GetChar(n)!=' ')
						break;

				// Get name of new type
				DWORD dwArrFlag=0;
				DWORD dwFieldOffset=0;
				LPSTR pNewTypeName=NULL;
				CDeclaration* pLastDec = g_pStructTable->FindFieldInType(pTypeName, pFieldname->GetStr(), &pNewTypeName, &dwArrFlag, &dwFieldOffset);
				if ( pLastDec )
				{
					CStr* pLastDecTypeName = pLastDec->GetType();
					pLastStruct = g_pStructTable->DoesTypeEvenExist(pLastDecTypeName->GetStr());
				}
				if ( pLastDec==NULL )
				{
					g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+38, pFieldname->GetStr(), pTypeName);
					SAFE_DELETE(pTypeName);
					SAFE_DELETE(pFieldname);
					SAFE_DELETE(pFirstName);
					return false;
				}

				// Retain newly found type
				SAFE_DELETE(pTypeName);
				pTypeName=pNewTypeName;

				// Store field type for L-Value process
				dwTrackLastValidLValueFieldType=g_pVarTable->GetBasicTypeValue(pTypeName);

				// If field is still traversing type-nest, apply offset
				dwTrackOffsetToActualData+=dwFieldOffset;

				// Free field name
				SAFE_DELETE(pFieldname);

				// Restart and add non-ltr char too (ony if not at end of string)
				if(n<length-1)
				{
					iStartNewName=-1;
					if(pFieldData->GetChar(n)=='.')	iStartNewName=n+1;
				}
			}
		}
	}

	SAFE_DELETE(pTypeName);
	SAFE_DELETE(pFirstName);

	// Return Data gathered from traversal
	*pdwLType=dwTrackLastValidLValueFieldType;
	*pdwSize=dwTrackOffsetToActualData;
	*ppStruct=pLastStruct;
	
	// Complete
	return true;
}

bool CMathOp::DoValueComplexVariable(CStr* pExpressionValue)
{
	// Statement Line
	DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();

	// Assign Value as Array or Variable(with/without type)
	CStr* pName = new CStr(pExpressionValue->GetStr());

	// Before processing DataType, crop spaces, tabs and equal brackets
	do { pName->EatEdgeSpacesandTabs(NULL);
	} while(pName->CropEqualEdgeBrackets(NULL)==true);
	
	// Just eat all non-important spacea
	// removed due to: "sample(array count(sample(0))).value" occurance - space would be deleted!
//	pName->EatNonImportantChars();

	// Complex variables are arrays or types
	DWORD dwArrayType=0;

	// Determine seperation between var and field specifiers
	DWORD dwTypeFieldSep = pName->FindFirstCharAtBracketLevel('.');
	DWORD dwSepPos = dwTypeFieldSep;
	CStr* pFixedDataOffset = NULL;
	if(dwSepPos==0)
	{
		// No datatype field part
		dwSepPos = pName->Length();
		pFixedDataOffset = new CStr("");
	}
	else
	{
		// Seperate Dynamic and Fixed Components
		pFixedDataOffset = new CStr(pName->GetStr()+dwSepPos+1);
	}

	// Check if array-subscript provided for var
	CStr* pSubscriptString = NULL;
	int iGrabSubscriptString=-1;
	for(DWORD n=0; n<dwSepPos; n++)
	{
		if(pName->CheckChar(n, '(')) { iGrabSubscriptString=n; break; }
	}
	if(iGrabSubscriptString!=-1)
	{
		for(n=dwSepPos; n>0; n--)
		{
			if(pName->CheckChar(n, ')')) break;
		}
		if(n==dwSepPos-1)
		{
			// Subscript String Value
			pSubscriptString = new CStr("");
			pSubscriptString->SetText(pName->GetStr()+iGrabSubscriptString+1);
			pSubscriptString->SetChar((dwSepPos-iGrabSubscriptString)-2,0);

			// Value is array
			dwArrayType=1;
		}
	}

	// Take only name part
	if(iGrabSubscriptString==-1)
		pName->SetChar(dwSepPos, 0);
	else
		pName->SetChar(iGrabSubscriptString, 0);

	// Verify name is accurate for a variable
	if(pName->IsTextASingleVariable()==false)
	{
		if ( pName->Length() > 0 )
		{
			// ensure error responds only if something for user to see, else use a higher error report
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+7, pName->GetStr());
		}
		SAFE_DELETE(pFixedDataOffset);
		SAFE_DELETE(pSubscriptString);
		SAFE_DELETE(pName);
		return false;
	}

	// Calculate actual size of data offset (from type data)
	DWORD dwDataOffset = 0;
	CStructTable* pStruct = NULL; // used for inner-UDT type determination
	DWORD dwLValueType = g_pVarTable->MakeDefaultVarTypeValue(pName->GetStr());
	DWORD dwSizeOfWholeType = 0;
	if(pFixedDataOffset->Length()>0)
	{
		if(CalculateDataOffsetAndTypeFromFieldString(pName, dwArrayType, pFixedDataOffset, &dwDataOffset, &dwLValueType, &dwSizeOfWholeType, &pStruct)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'Calculate DataOffsetAndTypeFromFieldString'");
			SAFE_DELETE(pFixedDataOffset);
			SAFE_DELETE(pSubscriptString);
			SAFE_DELETE(pName);
			return false;
		}
	}
	else
	{
		// LEEFIX - 171102 - Dot, but no data offset field..error
		if(dwTypeFieldSep>0)
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+38, "", pName->GetStr());
			SAFE_DELETE(pFixedDataOffset);
			SAFE_DELETE(pSubscriptString);
			SAFE_DELETE(pName);
			return false;
		}
	}

	// Obtain user-fucntion-scope if any..
	LPSTR pScope=NULL;
	if(g_pStatementList->GetUserFunctionDecChain())
		if(g_pStatementList->GetUserFunctionName())
			pScope = g_pStatementList->GetUserFunctionName();

	// Check if name already exists as a global
	bool bNameAlreadyExistsAsAGlobal=false;
	CStr* pFindName = new CStr("");
	if(dwArrayType==1) pFindName->SetText("&");
	pFindName->AddText(pName->GetStr());
	CVarTable* pVarTest;
	pVarTest = g_pVarTable->FindVariable(pScope, pFindName->GetStr(), dwArrayType);
	if(pVarTest==NULL)
	{
		pVarTest = g_pVarTable->FindVariable("", pFindName->GetStr(), dwArrayType);
		if(pVarTest)
		{
			// LEEFIX - 271102 - Fix so arrays and types in functions
			DWORD dwBasicType = pVarTest->GetVarTypeValue();
			if(dwBasicType<1000)
			{
				dwBasicType = dwBasicType % 100;
				if(dwBasicType>=1 && dwBasicType<=9)
				{
					// LEEFIX - 171102 - If find as global, get true type
					dwLValueType = dwBasicType;
				}
			}
			else
			{
				// leefix-240603-if datatype with no field part, dest value is UDT
				if ( dwTypeFieldSep==0 )
				{
					dwLValueType = dwBasicType;
				}
			}
			bNameAlreadyExistsAsAGlobal=true;
		}
	}
	else
	{
		// LEEFIX - 171002 - Update array access type (if a basic type)
		DWORD dwBasicType = pVarTest->GetVarTypeValue();
		if(dwBasicType<1000)
		{
			dwBasicType = dwBasicType % 100;
			if(dwBasicType>=1 && dwBasicType<=9)
			{
				// IN case where string
				dwLValueType = dwBasicType;
			}
		}
		else
		{
			// leefix-240603-if datatype with no field part, dest value is UDT
			if ( dwTypeFieldSep==0 )
			{
				dwLValueType = dwBasicType;
			}
		}
	}
	SAFE_DELETE(pFindName);

	// Get UDT of var if not got
	if ( pStruct==NULL && pVarTest )
		pStruct = pVarTest->GetVarStruct();

	// Produce result name
	CStr* pResultName = new CStr("");
	CDeclaration* pGlobalDecChain = g_pStatementList->GetUserFunctionDecChain();
	if(pGlobalDecChain && bNameAlreadyExistsAsAGlobal==false)
	{
		// Local Name
		pResultName->SetText("FS@");
		pResultName->AddText(g_pStatementList->GetUserFunctionName());
		pResultName->AddText("@");
		if(dwArrayType==1) pResultName->AddText("&");
		pResultName->AddText(pName);
	}
	else
	{
		// Global Name
		if(pName->CheckChar(0, '@')==false)
		{
			pResultName->SetText("@");
			if(dwArrayType==1) pResultName->AddText("&");
			pResultName->AddText(pName);
		}
		else
			pResultName->SetText(pName);
	}

	// Begin with math for dynamic array offset
	if(pSubscriptString==NULL)
	{
		// VAR.VIA TYPEDEF
		SetLineNumber(StatementLineNumber);
		SetResult(pResultName->GetStr(), dwLValueType, dwDataOffset);
		SetResultStruct ( pStruct );
	}
	else
	{
		// Add all subscripts to stack in reverse order
		int iBracketCount=0;
		DWORD dwSubscriptCount=0;
		int iIndex = pSubscriptString->Length()-1;
		while(iIndex>=0)
		{
			// Get result of subscript calc
			CStr* pOneSubscript = new CStr("");

			// Find subscript
			int iSpeechmark=0;
			while((pSubscriptString->GetChar(iIndex)!=',' || iSpeechmark==1 || iBracketCount!=0) && iIndex>=0)
			{
				if(pSubscriptString->GetChar(iIndex)=='(') iBracketCount++;
				if(pSubscriptString->GetChar(iIndex)==')') iBracketCount--;
				if(pSubscriptString->GetChar(iIndex)=='"') iSpeechmark=1-iSpeechmark;
				pOneSubscript->AddChar(pSubscriptString->GetChar(iIndex));
				iIndex--;
				if(iIndex<0) break;
			}
			pOneSubscript->Reverse();
			dwSubscriptCount++;
			iIndex--;

			// calculate result subscript
			CMathOp* pSubscriptResult = new CMathOp;
			if(pSubscriptResult->DoValue(pOneSubscript)==false)
			{
				SAFE_DELETE(pFixedDataOffset);
				SAFE_DELETE(pSubscriptString);
				SAFE_DELETE(pName);
				SAFE_DELETE(pSubscriptResult);
				SAFE_DELETE(pOneSubscript);
				return false;
			}
			Add(pSubscriptResult);

			// Subscript must be INTEGER or DWORD
			if ( pSubscriptResult->FindResultTypeValueForDBM()%100!=1
			&&   pSubscriptResult->FindResultTypeValueForDBM()%100!=7 )
			{
				// otherwise compiler error
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+63);
				return false;
			}

			// Create stack adder for subscript
			if(pSubscriptResult->GetMathSymbol()!=0 || pSubscriptResult->GetNext())
			{
				// Need new math for stack instruction
				CMathOp* pAddSubscript = new CMathOp;
				CResultData* pResultFromSubscript = pSubscriptResult->FindResultData();
				pAddSubscript->SetResult(pResultFromSubscript->m_pStringToken->GetStr(), pResultFromSubscript->m_dwType, pResultFromSubscript->m_dwDataOffset );
				pAddSubscript->SetResultStruct(NULL);
				if(pResultFromSubscript->m_pAdditionalOffset)
				{
					pAddSubscript->SetArrayOffsetResult(pResultFromSubscript->m_pAdditionalOffset->GetStr());
				}
				pAddSubscript->SetMathSymbol(10002);
				Add(pAddSubscript);
			}
			else
			{
				// As no chain can make this single item stack instruction
				pSubscriptResult->SetMathSymbol(10002);
			}

			// Prev subscript (reverse order)
			SAFE_DELETE(pOneSubscript);
		}

		// If subscript count is zero, array must use internal index
		if(dwSubscriptCount==0)
		{
			// Extracts internal index and pushes it to stack (similar to passing calculated params to stack)
			CMathOp* pSubscriptResult = new CMathOp;
			pSubscriptResult->SetLineNumber(StatementLineNumber);
			pSubscriptResult->SetResult(pResultName->GetStr(), 107, 0);
			pSubscriptResult->SetResultStruct(NULL);
			pSubscriptResult->SetMathSymbol(10004);
			Add(pSubscriptResult);
		}

		// Create temp-token to hold final offset
		CStr* pTempTokenOffset = new CStr("");
		ProduceNewTempToken(pTempTokenOffset, 7);

		// Array-passed-in-for-offset-calculation
		CMathOp* pPassArrayForOffsetCalc = new CMathOp;
		pPassArrayForOffsetCalc->SetLineNumber(StatementLineNumber);
		pPassArrayForOffsetCalc->SetResult(pResultName->GetStr(), 107, 0);
		pPassArrayForOffsetCalc->SetResultStruct(NULL);

		// Math to calculate offset for array
		CMathOp* pCalculateArrayOffset = new CMathOp;
		pCalculateArrayOffset->SetLineNumber(StatementLineNumber);
		pCalculateArrayOffset->SetResult(pTempTokenOffset->GetStr(), 7, dwSubscriptCount);
		pCalculateArrayOffset->SetResultStruct(NULL);
		pCalculateArrayOffset->SetMathSymbol(10003);
		pCalculateArrayOffset->m_pLeftMathOp = pPassArrayForOffsetCalc;
		Add(pCalculateArrayOffset);

		// Make math for [Array Name, Dynamic Offset and Data Offset]
		CMathOp* pArrayAccess = new CMathOp;
		pArrayAccess->SetLineNumber(StatementLineNumber);
		pArrayAccess->SetResult(pResultName->GetStr(), 100+dwLValueType, dwDataOffset);
		pArrayAccess->SetResultStruct(pStruct);
		pArrayAccess->SetArrayOffsetResult(pTempTokenOffset->GetStr());
		SAFE_DELETE(pTempTokenOffset);
		Add(pArrayAccess);

		// Free usages
		SAFE_DELETE(pSubscriptString);
	}

	// Clear memory
	SAFE_DELETE(pName);
	SAFE_DELETE(pResultName);
	SAFE_DELETE(pFixedDataOffset);

	// Complete
	return true;
}

bool CMathOp::ResolveStructValue(CStr* pExpressionValue)
{
	// Resolve STRUCT and FUNCTIONSTRUCT values to a result
	bool bIsStructItem=false;
	bool bIsStructFunction=false;
	CStr* pString = new CStr(pExpressionValue->GetStr());

	// Is it a struct from a userfunction
	if(pString->CheckChars(0,3,"FS@")==true)
	{
		LPSTR pRest = pString->GetRightOfPosition(3);
		pString->SetText(pRest);
		SAFE_DELETE(pRest);
		bIsStructItem=true;
		bIsStructFunction=true;
	}

	// Yes, proceed..
	if(bIsStructItem)
	{
		// Is it a type or field speciier
		DWORD dwPos = pString->FindFirstChar('@');
		if(dwPos>0)
		{
			// Find subtype name
			LPSTR pSubtypename = pString->GetLeftOfPosition(dwPos);

			// Determine if field is array
			DWORD dwArrayType = 0;
			if(pString->GetChar(dwPos+1)=='&') dwArrayType=1;
			LPSTR pFieldname = pString->GetRightOfPosition(dwPos+1);

			// Use correct L-Value for later resolution to reading/writing
			DWORD dwLValueType = 0;
			CStructTable* pStructRef = NULL;
			CDeclaration* pDec=g_pStructTable->FindDecInType(pSubtypename, pFieldname);
			if(pDec)
			{
				// Use Structure To Get LValue Type
				dwLValueType=g_pVarTable->GetBasicTypeValue(pDec->GetType()->GetStr());
				pStructRef = g_pVarTable->GetStruct(pDec->GetType()->GetStr());
			}
			else
			{
				// Struct not found, so try looking in var table (instantly updated)
				CVarTable* pVar = g_pVarTable->FindVariable(pSubtypename, pFieldname, dwArrayType);
				if(pVar)
				{
					dwLValueType = pVar->GetVarTypeValue();
					pStructRef = pVar->GetVarStruct();
				}
			}

			// This value is a simple offset (part of address calculation)		
			m_dwOffsetLValueTypeValue=dwLValueType;

			// Set Result Data
			SetResult(pExpressionValue->GetStr(), dwLValueType, GetResultDataOffset());
			SetResultStruct(pStructRef);

			// Free memory used
			SAFE_DELETE(pSubtypename);
			SAFE_DELETE(pFieldname);
		}
		else
		{
			// Type Specifier - Use Size of Type Structure
			DWORD dwTypeSize=g_pStructTable->GetSizeOfType(pString->GetStr());
			CStr* pStr = new CStr(1);
			pStr->SetNumericText(dwTypeSize);
			SetResult(pStr->GetStr(), 7, 0);
			SetResultStruct(NULL);
			SAFE_DELETE(pStr);
		}
	}

	// Free temp string
	SAFE_DELETE(pString);

	// Complete
	return true;
}

bool CMathOp::DoValueSingleVariable(CStr* pExpressionValue)
{
	// Struct is an absolute value (parsed later)
	if(pExpressionValue->CheckChars(0, 3, "FS@"))
	{
		// Determine type value from details
		if(ResolveStructValue(pExpressionValue)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoValueSingleVariable::ResolveStructValue'");
			return false;
		}
	}
	else
	{
		// Strip expression of @ symbol if prev. added
		if(pExpressionValue->CheckChar(0, '@'))
		{
			LPSTR pStr = pExpressionValue->GetRightOfPosition(1);
			pExpressionValue->SetText(pStr);
			SAFE_DELETE(pStr);
		}

		// Determine Array Marker from name
		DWORD dwArrFlag=0;
		if(pExpressionValue->CheckChar(0, '&'))
			dwArrFlag=1;

		// Determine If Pointing Into Variable
		DWORD dwPointingIntoVar=0;
		if(pExpressionValue->CheckChar(0, '*'))
		{
			pExpressionValue->EatChar(0);
			dwPointingIntoVar=1;
		}

		// Add Variable to variable table
		DWORD dwAction=0;
		LPSTR pDecName = pExpressionValue->GetStr();
		LPSTR pDecType = g_pVarTable->MakeDefaultVarType(pDecName);
		if(g_pVarTable->AddVariable(pDecName, pDecType, dwArrFlag, 0, true, &dwAction, false)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoValueSingleVariable::AddVariable'");
			SAFE_DELETE(pDecType);
			return false;
		}
		SAFE_DELETE(pDecType);

		// Is variable a userfunction local variabale
		bool bVariableIsLocal=false;
		CDeclaration* pGlobalDecChain = g_pStatementList->GetUserFunctionDecChain();
		if(pGlobalDecChain && pExpressionValue->GetChar(0)!='$')	bVariableIsLocal=true;
		if(dwAction==2)												bVariableIsLocal=false;

		// If local, produce indirect access to local data block		
		if(bVariableIsLocal)
		{
			// LOCAL VARIABLE
			CStr* pLocalVar = new CStr("");
			pLocalVar->SetText("FS@");
			pLocalVar->AddText(g_pStatementList->GetUserFunctionName());
			pLocalVar->AddText("@");
			pLocalVar->AddText(pExpressionValue);

			// Parse function immediate
			if(DoValue(pLocalVar)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoValueSingleVariable::DoValue'");
				SAFE_DELETE(pLocalVar);
				return false;
			}

			// LEEFIX - 111002 - Placed array (+=100) code and not in DBMtranslator - messes up array dimming in functions

			// Ensure arrays handled
			DWORD dwArr=0;
			if(pExpressionValue->CheckChar(0,'&')) dwArr=1;

			// Find Type Of Variable
			LPSTR pTypeName=NULL;
			if(g_pVarTable->FindTypeOfVariable(pExpressionValue->GetStr(), dwArr, &pTypeName)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoValueSingleVariable::FindTypeOfVariable'");
				return false;
			}

			// Amend result for whether local array or not
			DWORD dwType = g_pVarTable->GetBasicTypeValue(pTypeName);
			if(dwArr==1) dwType+=100;

			// is variable 'indirect' specified by * symbol
			if(dwPointingIntoVar==1)
			{
				// only for variables (not arrays)
				if(dwType<100) dwType+=200;
			}

			// finally set result
			SetResultType(dwType);
			SetResultStruct(g_pVarTable->GetStruct(pTypeName));

			// Free usages
			SAFE_DELETE(pTypeName);
			SAFE_DELETE(pLocalVar);
		}
		else
		{
			// GLOBAL OR ABSOLUTE VARIABLE
			DWORD dwArr=0;
			if(pExpressionValue->CheckChar(0,'&')) dwArr=1;

			// Find Type Of Variable
			LPSTR pTypeName=NULL;
			if(g_pVarTable->FindTypeOfVariable(pExpressionValue->GetStr(), dwArr, &pTypeName)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoValueSingleVariable::FindTypeOfVariable'");
				return false;
			}

			// Create result (array or not)
			CStr* pStr = new CStr("@");
			pStr->AddText(pExpressionValue);
			DWORD dwType = g_pVarTable->GetBasicTypeValue(pTypeName);
			if(dwArr==1) dwType+=100;
			SetResult(pStr->GetStr(), dwType, 0);
			SetResultStruct(g_pVarTable->GetStruct(pTypeName));

			// Free usages
			SAFE_DELETE(pTypeName);
			SAFE_DELETE(pStr);
		}

		// If pointer to this var, make direct ref indirect
		if(dwPointingIntoVar==1)
		{
			// only for variables (not arrays)
			if(GetResultType()<100)
			{
				SetResultType(GetResultType()+200);
			}
		}
	}

	// Complete
	return true;
}

bool CMathOp::DoValueLabel(CStr* pExpressionValue)
{
	// Assign Value as Label
	CStr* pStr = new CStr(pExpressionValue->GetStr());
	SetResult(pStr->GetStr(), 10, 0);
	SetResultStruct(NULL);
	SAFE_DELETE(pStr);

	// Complete
	return true;
}

bool CMathOp::DoValueLiteral(CStr* pExpressionValue, DWORD dwTypeValue)
{
	// Assign Value as Literal
	CStr* pStr = new CStr(pExpressionValue->GetStr());

	// leeadd - handle scientific notation
	if ( pStr->IsSciNot() ) { pStr->ResolveSciNot(); dwTypeValue=8; }

	// Ensure literal has no leading zero (hex and oct and bin are type=7)
	if ( dwTypeValue!=7 && dwTypeValue!=3 )
	{
		// non DWORD types can remove the leading zeros
		LPSTR pString = pStr->GetStr();
		DWORD dwLen = strlen(pString);
		if ( pString[0]=='0' )
		{
			for ( DWORD c=0; c<dwLen; c++ )
			{
				// leefix - 300305 - except where followed by a dot (float/double float)
				if ( pString[c]!='0' && c>0 )
				{
					// leefix - 310305 - ensure only eliminate EXTRA zeros, and only handle '.' if not at start!
					if ( pString[c-1]=='0' )
					{
						if ( c>0 && pString[c]=='.' ) c-=1;
						pStr->SetText ( pString+c );
						break;
					}
				}
			}
		}
	}

	// Confirm data result
	SetResult(pStr->GetStr(), dwTypeValue, 0);
	SetResultStruct(NULL);
	SAFE_DELETE(pStr);

	// Complete
	return true;
}

bool CMathOp::FindHighestPres(CStr* pString, DWORD *dwPosition, DWORD *dwType, DWORD *dwReturnSymbolWidth, DWORD *dwIsSciNot)
{
	// Bracket Vars
	int iBracketLevel=0;
	DWORD dwSpeechDisable=0;

	// Best Result Vars
	DWORD dwBestMathType=99;
	DWORD dwBestPriority=99;
	DWORD dwBestMathPosition=0;
	DWORD dwBestSymbolWidth=0;

	// usually zero
	*dwIsSciNot=0;

	// Find Math Symbols
	DWORD dwSP=0;
	DWORD dwLength = pString->Length();
	while(dwSP<dwLength)
	{
		// Detect Bracket Levels and Speech Mark Disable
		if(dwSpeechDisable==0)
		{
			if(pString->CheckChar(dwSP,'('))		iBracketLevel++;
			if(pString->CheckChar(dwSP,')'))		iBracketLevel--;
		}
		if(pString->CheckChar(dwSP,'"'))		dwSpeechDisable=1-dwSpeechDisable;

		// Detect Math Symbol
		if(iBracketLevel==0 && dwSpeechDisable==0)
		{
			// Math Symbols Table (in precedence order)
			DWORD dwMathType=0, dwSymbolWidth=0, dwPriority=0;
			CheckForSymbol(pString, dwSP, &dwMathType, &dwPriority, &dwSymbolWidth);

			// Ensure its not a negaive value (-XXX)
			if(dwMathType==5 && dwSP==0) dwMathType=0;

			// leeadd - U71 - 111008 - scientific notation (1.0E+38 or 1.0E-38)
			if( (dwMathType==4 || dwMathType==5) && dwSP>=2 )
			{
				// symbol found was +, make sure it is not part of a scientific notation
				int iDetectNotation = 0;
				bool bANonNumericInStringCanOnlyBeVariable = false;
				for ( DWORD dwN=dwSP-1; dwN>0; dwN-- )
				{
					if ( iDetectNotation==0 )
					{
						if ( pString->CheckChar(dwN,'e') || pString->CheckChar(dwN,'E') )
						{
							// found an E - check next stage
							iDetectNotation = 1;
							dwN--;
						}
						else
						{
							if ( pString->CheckChar(dwN,' ') )
							{
								// allow spaces to pass
							}
							else
							{
								// anything else and this is no scientific notation!
								break;
							}
						}
					}
					if ( iDetectNotation==1 )
					{
						unsigned char num = pString->GetChar(dwN);
						if ( num>='0' && num<='9' )
						{
							// found first numeric before E, this may be scientific notation!
							iDetectNotation=2;
							dwN--;
						}
						else
						{
							if ( pString->CheckChar(dwN,' ') )
							{
								// allow spaces to pass
							}
							else
							{
								// anything else and this is no scientific notation!
								break;
							}
						}
					}
					if ( iDetectNotation==2 )
					{
						unsigned char num = pString->GetChar(dwN);
						if ( num=='.' || (num>='0' && num<='9') )
						{
							// numerics are allowed
						}
						else
						{
							if ( pString->CheckChar(dwN,' ') )
							{
								// allow spaces to pass
							}
							else
							{
								// anything else and this is no scientific notation!
								bANonNumericInStringCanOnlyBeVariable = true;
								break;
							}
						}
					}
				}
				if ( bANonNumericInStringCanOnlyBeVariable==true ) iDetectNotation=0;
				if ( iDetectNotation==2 )
				{
					// not a maths operation, replace with actual number 
					// represented by notation..
					*dwIsSciNot=dwMathType;
					dwMathType=0;
				}
			}

			// Only the highest..
			if(dwMathType>0 && dwPriority<=dwBestPriority)
			{
				// Precedence by Occurance Order (0-no pref/1-righttoleft/2-leftoright)
				DWORD dwOrder=2;
// leefix - 240604 - u54 - LEFT TO RIGHT PRECIDENCE 100/10/2 and 100-10-2
//				if(dwMathType==1) dwOrder=1;
//				if(dwMathType==5) dwOrder=2;

				// Remove other best vars if better symb found
				if(dwPriority<dwBestPriority)
					dwBestMathPosition=0;

				// Record best so far
				if((dwOrder==0)
				|| (dwOrder==1 && dwSP>dwBestMathPosition)
				|| (dwOrder==2 && dwBestMathPosition==0) )
				{
					dwBestMathPosition=dwSP;
					dwBestMathType=dwMathType;
					dwBestPriority=dwPriority;
					dwBestSymbolWidth=dwSymbolWidth;
				}
			}
		}
		dwSP++;
	}

	// If bracket count irregular, then error
	if(iBracketLevel!=0)
	{
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+9);
		return false;
	}

	// If Found Math Symbol
	if(dwBestMathType!=99)
	{
		*dwPosition=dwBestMathPosition;
		*dwType=dwBestMathType;
		*dwReturnSymbolWidth=dwBestSymbolWidth;
	}
	else
	{
		*dwPosition=0;
		*dwType=0;
		*dwReturnSymbolWidth=0;
	}

	// Complete
	return true;
}

bool CMathOp::CheckForSymbol(CStr* pString, DWORD dwSP, DWORD *dwMathType, DWORD *dwPriority, DWORD *dwSymbolWidth)
{
	*dwMathType=0;
	if(pString->CheckChar	(dwSP,'^'))			{ *dwMathType=1; *dwPriority=1; *dwSymbolWidth=1; }
	if(pString->CheckChar	(dwSP,'/'))			{ *dwMathType=3; *dwPriority=2; *dwSymbolWidth=1; }
	if(pString->CheckChar	(dwSP,'*'))			{ *dwMathType=2; *dwPriority=3; *dwSymbolWidth=1; }
	if(pString->CheckChar	(dwSP,'+'))			{ *dwMathType=4; *dwPriority=5; *dwSymbolWidth=1; }
	if(pString->CheckChar	(dwSP,'-'))			{ *dwMathType=5; *dwPriority=4; *dwSymbolWidth=1; }
	if(pString->CheckChar	(dwSP,'='))			{ *dwMathType=27; *dwPriority=27; *dwSymbolWidth=1; }
	if(pString->CheckChar	(dwSP,'>'))			{ *dwMathType=25; *dwPriority=25; *dwSymbolWidth=1; }
	if(pString->CheckChar	(dwSP,'<'))			{ *dwMathType=26; *dwPriority=26; *dwSymbolWidth=1; }

	if(pString->CheckChars	(dwSP,2,"<>"))		{ *dwMathType=22; *dwPriority=22; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,">="))		{ *dwMathType=23; *dwPriority=23; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,"=>"))		{ *dwMathType=23; *dwPriority=23; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,"<="))		{ *dwMathType=24; *dwPriority=24; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,"=<"))		{ *dwMathType=24; *dwPriority=24; *dwSymbolWidth=2; }

	if(pString->CheckChars	(dwSP,2,"%%"))		{ *dwMathType=6; *dwPriority=3; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,"&&"))		{ *dwMathType=31; *dwPriority=31; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,"||"))		{ *dwMathType=32; *dwPriority=32; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,"~~"))		{ *dwMathType=33; *dwPriority=33; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,".."))		{ *dwMathType=34; *dwPriority=34; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,">>"))		{ *dwMathType=11; *dwPriority=11; *dwSymbolWidth=2; }
	if(pString->CheckChars	(dwSP,2,"<<"))		{ *dwMathType=12; *dwPriority=12; *dwSymbolWidth=2; }

	if(pString->CheckChars	(dwSP,5," AND "))	{ *dwMathType=41; *dwPriority=41; *dwSymbolWidth=5; }
	if(pString->CheckChars	(dwSP,5," XOR "))	{ *dwMathType=33; *dwPriority=44; *dwSymbolWidth=5; }
	if(pString->CheckChars	(dwSP,4," OR "))	{ *dwMathType=42; *dwPriority=42; *dwSymbolWidth=4; }
	if(pString->CheckChars	(dwSP,4,"NOT "))	{ *dwMathType=43; *dwPriority=43; *dwSymbolWidth=4; }
	if(pString->CheckChars	(dwSP,5," DIV "))	{ *dwMathType=3; *dwPriority=3; *dwSymbolWidth=5;	}
	if(pString->CheckChars	(dwSP,5," MOD "))	{ *dwMathType=6; *dwPriority=3; *dwSymbolWidth=5;	}

	// confirm result
	if(*dwMathType>0)
	{
		// success
		return true;
	}
	else
	{
		// soft fail
		return false;
	}
}

bool CMathOp::ProduceNewTempToken(CStr* pTempVarToken, DWORD dwTypeMode)
{
	// Create temp name
	CStr* pTempName = new CStr("$");
	pTempName->AddChar(g_pVarTable->GetCharOfType(dwTypeMode));
	pTempName->AddNumericText(g_pStatementList->GetTempVarIndex());

	// Produce temp var based on parse location
	CDeclaration* pGlobalDecChain = g_pStatementList->GetUserFunctionDecChain();
	if(pGlobalDecChain)
	{
		// Inside a function
		pTempVarToken->SetText("FS@");
		pTempVarToken->AddText(g_pStatementList->GetUserFunctionName());
		pTempVarToken->AddText("@");
		pTempVarToken->AddText(pTempName);
	}
	else
	{
		// Main program temp var
		pTempVarToken->SetText("@");
		pTempVarToken->AddText(pTempName);
	}
	g_pStatementList->IncTempVarIndex();

	// Any temporary variables created must be added to variable table space
	LPSTR pDecName = pTempName->GetStr();
	LPSTR pDecType = g_pVarTable->MakeTypeNameOfTypeValue(dwTypeMode);
	if(g_pVarTable->AddVariable(pDecName, pDecType, 0, 0, true, NULL, false)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'Produce NewTempToken::AddVariable'");
		SAFE_DELETE(pDecType);
		return false;
	}

	// Free usages
	SAFE_DELETE(pDecType);
	SAFE_DELETE(pTempName);

	return true;
}

bool CMathOp::IsReserved ( CStr* pString )
{
	// If any of these match, trying to use a sole reserved word in the expression
	if(stricmp(pString->GetStr(), "AND")==NULL)	return true;
	if(stricmp(pString->GetStr(), "OR")==NULL)	return true;
	if(stricmp(pString->GetStr(), "NOT")==NULL)	return true;
	if(stricmp(pString->GetStr(), "DIV")==NULL)	return true;
	if(stricmp(pString->GetStr(), "MOD")==NULL)	return true;

	// not matched
	return false;
}

bool CMathOp::IsItLabelFollowedByBracket(CStr* pExpressionValue, DWORD *pdwLabelLength)
{
	if(pExpressionValue->IsAlpha(0))
	{
		DWORD length=pExpressionValue->Length();
		if(length>1)
		{
			bool bHoweverNoMoreLetters=false;
			for(DWORD n=1; n<length-1; n++)
			{
				bool bValid=false;

				// Alphanumeric and other valid charactersvalid
				if(bHoweverNoMoreLetters==false)
				{
					if(pExpressionValue->IsAlphaNumericLabel(n)==true) bValid=true;
					if(pExpressionValue->GetChar(n)=='$' || pExpressionValue->GetChar(n)=='#')
					{
						bHoweverNoMoreLetters=true;
						bValid=true;
					}
				}
				if(pExpressionValue->IsSpaceCharacter(n)==true) bValid=true;

				// If not valid, no more
				if(bValid==false) break;
			}
			
			if(pExpressionValue->GetChar(n)=='(')
			{
				// Record length of function
				if(pdwLabelLength) *pdwLabelLength=n;

				return true;
			}
		}
	}

	// soft fail
	return false;
}

bool CMathOp::IsFunction(CStr* pExpressionValue)
{
	bool bFound=false;
	DWORD dwLabelLength=0;
	if(IsItLabelFollowedByBracket(pExpressionValue, &dwLabelLength))
	{
		// If function exists
		CStr* pPossibleName = new CStr(pExpressionValue->GetStr());
		pPossibleName->Shorten(dwLabelLength);
		if(SearchForFunction(pPossibleName))
			bFound=true;

		// Free usage
		SAFE_DELETE(pPossibleName);
	}
	return bFound;
}

bool CMathOp::IsLiteral(CStr* pExpressionValue, DWORD* pdwTypeValue)
{
	// Determine if this is a literal value (string, numeric, etc)
	if ( !pExpressionValue )
		return false;

	// Is it a string
	if(pExpressionValue->IsTextSpeechMarked())
	{
		*pdwTypeValue=3;
		return true;
	}

	// Is it an decimal value
	if(pExpressionValue->IsTextNumericValue())
	{
		if(pExpressionValue->IsTextIntegerOnlyValue())
		{
			if(pExpressionValue->IsIntegerBiggerThanDWORD())
				*pdwTypeValue=9;
			else
				*pdwTypeValue=1;
		}
		else
		{
			if(g_pDBPCompiler->m_bDoubleLiterals)
			{
				// Compiler may exclusively use doubles..
				*pdwTypeValue=8;
			}
			else
			{
				// By default, float..
				*pdwTypeValue=2;

				// LEEFIX - 141102 - Added back in as I have a better way to check
				if(pExpressionValue->IsFloatBiggerThanDWORD())
					*pdwTypeValue=8;
				else
					*pdwTypeValue=2;
			}
		}

		return true;
	}

	// Is it an hexidecimal value
	if(pExpressionValue->IsTextHexValue())
	{
		// leefix - 210703 - HEX, OCT and BINARY all report as DWORD values now
		*pdwTypeValue=7;
		return true;
	}

	// Is it an octal value
	if(pExpressionValue->IsTextOctalValue())
	{
		// leefix - 210703 - HEX, OCT and BINARY all report as DWORD values now
		*pdwTypeValue=7;
		return true;
	}

	// Is it an binary value
	if(pExpressionValue->IsTextBinaryValue())
	{
		// leefix - 210703 - HEX, OCT and BINARY all report as DWORD values now
		*pdwTypeValue=7;
		return true;
	}

	// leeadd - 121008 - u71 - Is scientific notation XX E [+/-]YY
	if(pExpressionValue->IsSciNot())
	{
		return true;
	}

	// Could not recognise soft fail
	return false;
}

bool CMathOp::IsSingleVariable(CStr* pExpressionValue)
{
	// Is it a temporary assigned variable
	if(pExpressionValue->CheckChars(0,1,"$"))
	{
		// lee - 130206 - u60 - can only be certain alpha/numeric symbols
		unsigned char cChar = pExpressionValue->GetChar(1);
		if ( cChar >= 48 && cChar <= 'z' )
			return true;
	}

	// Is it an array variable
	if(pExpressionValue->CheckChars(0,1,"&"))
		return true;

	// Last, treat as variable
	if(pExpressionValue->IsTextASingleVariable())
		return true;

	// If structure constant for a user function
	if(pExpressionValue->CheckChars(0,3,"FS@")==true)
		return true;

	// Could not recognise soft fail
	return false;
}

bool CMathOp::IsLabel(CStr* pExpressionValue)
{
	// Make label
	CStr FullLabel;
	FullLabel.SetText("$label ");
	FullLabel.AddText(pExpressionValue);

	// Search for label..
	if(g_pLabelTable->FindLabel(FullLabel.GetStr())!=NULL)
		return true;

	// Could not recognise soft fail
	return false;
}

bool CMathOp::IsComplexVariable(CStr* pExpressionValue)
{
	// Complex variable is one with fields and array specifiers ( () & .)
	if(pExpressionValue->IsTextAComplexVariable())
		return true;

	// Could not recognise soft fail
	return false;
}

bool CMathOp::IsAnything(CStr* pExpressionValue)
{
	// Always try and resolve it
	return true;
}

bool CMathOp::SearchForFunction(CStr* pFunctionName)
{
	// Remove spaces before checking for existence
	pFunctionName->EatEdgeSpacesandTabs(NULL);

	// Check if a recognised instruction
	CInstructionTableEntry* pRef = NULL;
	DWORD dwTokenData=0, dwParamMax=0, dwLength=0;
	if(g_pInstructionTable->FindInstruction(false, pFunctionName->GetStr(), 1, &dwTokenData, &dwParamMax, &dwLength, &pRef))
	{
		// Record instruction for later parsing (doinstruction)
		g_pStatementList->SetInstructionRef(pRef);
		g_pStatementList->SetInstructionType(2);
		g_pStatementList->SetInstructionValue(dwTokenData);
		g_pStatementList->SetInstructionParamMax(dwParamMax);
		return true;
	}

	// Check if a user defined function
	dwTokenData=0, dwParamMax=0, dwLength=0;
	if(g_pInstructionTable->FindUserFunction(pFunctionName->GetStr(), 1, &dwTokenData, &dwParamMax, &dwLength))
	{
		// Record instruction for later parsing (doinstruction)
		g_pStatementList->SetInstructionRef(NULL);
		g_pStatementList->SetInstructionType(3);
		g_pStatementList->SetInstructionValue(dwTokenData);
		g_pStatementList->SetInstructionParamMax(dwParamMax);
		return true;
	}

	// No function of this name soft fail
	//DB3_CRASH_MSG(pFunctionName->GetStr());
	return false;
}

DWORD CMathOp::ChopOffOneItemFromLeft(CStr* pString)
{
	int iBracketLevel=0;
	DWORD pos = 0;
	DWORD dwSpeechDisable=0;
	DWORD length = pString->Length();
	while(pos<length)
	{
		if ( dwSpeechDisable==0 )
		{
			if(pString->CheckChar(pos,'('))		iBracketLevel++;
			if(pString->CheckChar(pos,')'))		iBracketLevel--;
		}
		if(pString->CheckChar(pos,'"'))		dwSpeechDisable=1-dwSpeechDisable;
		if(iBracketLevel==0 && dwSpeechDisable==0)
		{
			DWORD dwMathType=0, dwSymbolWidth=0, dwPriority=0;
			if(CheckForSymbol(pString, pos, &dwMathType, &dwPriority, &dwSymbolWidth)==true)
			{
				if(pos==0)
				{
					if(!pString->CheckChar(0,'-') && !pString->CheckChar(0,'+'))
						break;
				}
				else
					break;
			}
		}
		pos++;
	}
	pString->SetChar(pos,0);
	return pos;
}

DWORD CMathOp::ChopOffOneItemFromRight(CStr* pString)
{
	int iBracketLevel=0;
	DWORD length = pString->Length();
	DWORD dwSpeechDisable=0;
	DWORD pos = length;
	DWORD dwMathType=0;
	DWORD dwPriority=0;
	DWORD dwSymbolWidth=0;
	while(pos>0)
	{
		// leefix - 270206 - bracket only valid outside speech marks
		if(pString->CheckChar(pos,'"'))		dwSpeechDisable=1-dwSpeechDisable;
		if ( dwSpeechDisable==0 )
		{
			if(pString->CheckChar(pos,'('))		iBracketLevel++;
			if(pString->CheckChar(pos,')'))		iBracketLevel--;
			if(iBracketLevel==0)
			{
				if(CheckForSymbol(pString, pos, &dwMathType, &dwPriority, &dwSymbolWidth)==true) break;
			}
		}
		pos--;
	}
	DWORD nread=pos+dwSymbolWidth;
	DWORD nwrite=0;
	while(nread<length)
	{
		pString->SetChar(nwrite, pString->GetChar(nread));
		nwrite++;
		nread++;
	}
	pString->SetChar(nwrite, 0);
	return pos+dwSymbolWidth;
}

bool CMathOp::DetermineIfPointerMaths(DWORD dwMathSymbol, DWORD dwTypeValue)
{
	if(dwTypeValue>=101 && dwTypeValue<=109)
		if(dwMathSymbol<100)
			return true;

	// soft faul
	return false;
}

//
// WriteDBM Segment
//

bool CMathOp::WriteDBM(void)
{
	// If need to process extra statements
	if(m_pStatement)
	{
		m_pStatement->WriteDBM();
	}

	// Write out text
	if(m_dwMathSymbol==0)
	{
//		Moved to above: an 'array count()' expression in a stack calc had no DBM code!
//		// If need to process extra statements
//		if(m_pStatement)
//		{
//			m_pStatement->WriteDBM();
//		}
	}
	else
	{
		// instructions 10000 and above are special codes
		if(m_dwMathSymbol<10000)
		{
			// First Traverse Deeper
			if(m_pLeftMathOp) m_pLeftMathOp->WriteDBM();
			if(m_pRightMathOp) m_pRightMathOp->WriteDBM();

			// Write out math command
			if(WriteDBMBit(GetLineNumber())==false) return false;
		}
		else
		{
			// Special array-offset-calculation instructions
			// LEEFIX - 281102 - Added m_pAdditionalOffset to translateDBM
			switch(GetMathSymbol())
			{
				case 10002 :	// Add contents of 'this' result to stack (used for subscript passing for dynamic array)
								if(GetResultData()->m_pStringToken) GetResultData()->m_pStringToken->TranslateForDBM(GetResultData());
								if(GetResultData()->m_pAdditionalOffset) GetResultData()->m_pAdditionalOffset->TranslateForDBM(GetResultData());
								g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, GetResultData());
								break;

				case 10003 :	// Process stack-items to calculate array-offset (array stored in left mathitem)
								if(GetResultData()->m_pStringToken) GetResultData()->m_pStringToken->TranslateForDBM(GetResultData());
								if(GetResultData()->m_pAdditionalOffset) GetResultData()->m_pAdditionalOffset->TranslateForDBM(GetResultData());
								if(m_pLeftMathOp) if(m_pLeftMathOp->GetResultData()->m_pStringToken) m_pLeftMathOp->GetResultData()->m_pStringToken->TranslateForDBM(GetResultData());
								if(m_pLeftMathOp) if(m_pLeftMathOp->GetResultData()->m_pAdditionalOffset) m_pLeftMathOp->GetResultData()->m_pAdditionalOffset->TranslateForDBM(GetResultData());
								g_pASMWriter->WriteASMTaskP2(m_dwLineNumber, ASMTASK_CALCARRAYOFFSET, GetResultData(), m_pLeftMathOp->GetResultData());
								break;

				case 10004 :	// Adds internal index from array to stack
								if(GetResultData()->m_pStringToken) GetResultData()->m_pStringToken->TranslateForDBM(GetResultData());
								if(GetResultData()->m_pAdditionalOffset) GetResultData()->m_pAdditionalOffset->TranslateForDBM(GetResultData());
								g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSHINTERNALARRAYINDEX, GetResultData());
								break;
			}
		}
	}

	// Then Do Next One
	if(m_pNext)
	{
		if(m_pNext->WriteDBM()==false) return false;
	}

	return true;
}


bool CMathOp::WriteDBMBit(DWORD dwLineNumber)
{
	/* moved to praseinstruction
	// Special math commands (10000>)
	if(m_dwMathSymbol>10000)
	{
		switch(m_dwMathSymbol)
		{
			case 10002 :	// Add contents of 'this' result to stack (used for subscript passing for dynamic array)
							g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_PUSH, GetResultData());
							break;
		}
		return true;
	}
	*/

	// Cast Maths have one param, regular maths have two
	DWORD dwNumberOfPopsToMake=2;
	if(m_dwMathSymbol>=101)
		dwNumberOfPopsToMake=1;

	// Data type to handle data (treat indirect addressing as regular values
	DWORD dwDataTypeA = m_pLeftMathOp->FindResultTypeValueForDBM();
	if ( dwDataTypeA>=200 && dwDataTypeA<=299 )
		dwDataTypeA-=200;

	// Internal Math Call to DLL
	DWORD dwUseNewInstruction=g_pInstructionTable->DetermineInternalCommandCode(m_dwMathSymbol, dwDataTypeA);
	if(dwUseNewInstruction==0)
	{
		// Command not yet implemented
		g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_UNKNOWN, NULL);
		return true;
	}

	// Work out more final DBM data (moved from after stack adds)
	TranslateStringTokenForDBM();

	// CALL Instruction with current stack state
	CInstructionTableEntry* pRef=g_pInstructionTable->GetRef(dwUseNewInstruction);
	if(pRef==NULL)
	{
		// Command not yet implemented
		g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_UNKNOWN, NULL);
		return true;
	}

	// If math is hardcoded, build directly (no need for DLL call)
	bool bMathCommandIsHardCoded=false;
	if(pRef->GetBuildID()>0) bMathCommandIsHardCoded=true;
	if(bMathCommandIsHardCoded)
	{
		// Work out build
		DWORD dwASMToBuild;
		if(pRef->GetBuildID()==BUILD_MUL) dwASMToBuild=ASMTASK_MUL;
		if(pRef->GetBuildID()==BUILD_DIV) dwASMToBuild=ASMTASK_DIV;
		if(pRef->GetBuildID()==BUILD_ADD) dwASMToBuild=ASMTASK_ADD;
		if(pRef->GetBuildID()==BUILD_SUB) dwASMToBuild=ASMTASK_SUB;
		if(pRef->GetBuildID()==BUILD_MOD) dwASMToBuild=ASMTASK_MOD;

		if(pRef->GetBuildID()==BUILD_SHR) dwASMToBuild=ASMTASK_SHR;
		if(pRef->GetBuildID()==BUILD_SHL) dwASMToBuild=ASMTASK_SHL;
		if(pRef->GetBuildID()==BUILD_BITAND) dwASMToBuild=ASMTASK_AND;
		if(pRef->GetBuildID()==BUILD_BITOR) dwASMToBuild=ASMTASK_OR;
		if(pRef->GetBuildID()==BUILD_BITXOR) dwASMToBuild=ASMTASK_XOR;
		if(pRef->GetBuildID()==BUILD_BITNOT) dwASMToBuild=ASMTASK_BITNOT;

		if(pRef->GetBuildID()==BUILD_AND) dwASMToBuild=ASMTASK_AND;
		if(pRef->GetBuildID()==BUILD_OR) dwASMToBuild=ASMTASK_OR;
		if(pRef->GetBuildID()==BUILD_NOT) dwASMToBuild=ASMTASK_NOT;

		if(pRef->GetBuildID()==BUILD_EQUAL) dwASMToBuild=ASMTASK_EQUAL;
		if(pRef->GetBuildID()==BUILD_NOTEQUAL) dwASMToBuild=ASMTASK_NOTEQUAL;
		if(pRef->GetBuildID()==BUILD_GREATER) dwASMToBuild=ASMTASK_GREATER;
		if(pRef->GetBuildID()==BUILD_GREATEREQUAL) dwASMToBuild=ASMTASK_GREATEREQUAL;
		if(pRef->GetBuildID()==BUILD_LESS) dwASMToBuild=ASMTASK_LESS;
		if(pRef->GetBuildID()==BUILD_LESSEQUAL) dwASMToBuild=ASMTASK_LESSEQUAL;

		// Call hard code builder
		CResultData* pA = m_pLeftMathOp->FindResultData();
		CResultData* pB = m_pRightMathOp->FindResultData();
		if(GetResultData()->m_pStringToken) GetResultData()->m_pStringToken->TranslateForDBM(GetResultData());
		if(GetResultData()->m_pAdditionalOffset) GetResultData()->m_pAdditionalOffset->TranslateForDBM(GetResultData());
		g_pASMWriter->WriteASMTaskCore(	m_dwLineNumber, dwASMToBuild,
										pA->m_pStringToken, pA->m_pAdditionalOffset, pA->m_dwType, pA->m_dwDataOffset,
										pB->m_pStringToken, pB->m_pAdditionalOffset, pB->m_dwType, pB->m_dwDataOffset,
										GetResultData()->m_pStringToken, GetResultData()->m_pAdditionalOffset, GetResultData()->m_dwType, GetResultData()->m_dwDataOffset);
	}
	else
	{
		// Push Parameters for Math Call to Stack (in reverse order)
		if(m_dwMathSymbol<101)
		{
			// Ignore second param of casting maths (redundant)
			DWORD dwDataTypeB = m_pRightMathOp->FindResultTypeValueForDBM();
			g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, m_pRightMathOp->FindResultData());
			if(dwDataTypeB==8 || dwDataTypeB==9 || dwDataTypeB==108 || dwDataTypeB==109) dwNumberOfPopsToMake++;
		}
		g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, m_pLeftMathOp->FindResultData());
		if(dwDataTypeA==8 || dwDataTypeA==9 || dwDataTypeA==108 || dwDataTypeA==109) dwNumberOfPopsToMake++;

		// Some Core Instructions require additional 'internal' params
		if(GetResultData()->m_dwType==3)
		{
			// Add Destination string (as it needs to be freed if not-NULL)
			g_pASMWriter->WriteASMTaskP1(m_dwLineNumber, ASMTASK_PUSH, GetResultData());
			dwNumberOfPopsToMake++;
		}

		DWORD dwInstructionValue=g_pInstructionTable->GetIIValue(dwUseNewInstruction);
		LPSTR pMathDLL=pRef->GetDLL()->GetStr();
		LPSTR pMathCommand=pRef->GetDecoratedName()->GetStr();
		g_pASMWriter->WriteASMCall(dwLineNumber, pMathDLL, pMathCommand);

		// Fundamental pop of all calls
		g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_POPEBX, NULL);

		// Copy EAX (holding call result) to temp var used to hold return value
		CStr* pReturnData = GetResultStringToken();
		if(pReturnData)
		{
			DWORD dwReturnDataType = GetResultType();
			g_pASMWriter->WriteASMTaskP2(dwLineNumber, ASMTASK_ASSIGN, GetResultData(), NULL);
		}

		// Pop params off stack in same quantity as those added
		for(DWORD i=0; i<dwNumberOfPopsToMake-1; i++)
			g_pASMWriter->WriteASMTaskP1(dwLineNumber, ASMTASK_POPEAX, NULL);
	}

	return true;
}

bool CMathOp::WriteDBMLine(DWORD dwLineNumber, LPSTR pText, LPSTR pResult)
{
	// Write out text
	CStr strDBMLine(256);
	strDBMLine.SetNumericText(dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(pText);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(pResult);
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	return true;
}
