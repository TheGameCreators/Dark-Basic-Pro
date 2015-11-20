// Statement.cpp: implementation of the CStatement class.
//
//////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)
#include "ParserHeader.h"

// Custom Includes
#include "VarTable.h"
#include "InstructionTable.h"
#include "StatementList.h"
#include "Declaration.h"
#include "Statement.h"
#include "DBMWriter.h"
#include "Error.h"
#include "Str.h"
#include "DebugInfo.h"

// Struct Classes
#include "VarTable.h"
#include "DataTable.h"
#include "LabelTable.h"
#include "StructTable.h"

// Token Classes
#include "ParseLoop.h"
#include "ParseJump.h"
#include "ParseType.h"
#include "ParseInit.h"
#include "ParseFunction.h"
#include "ParseInstruction.h"
#include "ParseUserFunction.h"
#include "DBPCompiler.h"

// External References
extern CError *g_pErrorReport;
extern CVarTable *g_pVarTable;
extern CDataTable *g_pDataTable;
extern CLabelTable *g_pLabelTable;
extern CDBMWriter *g_pDBMWriter;
extern CStructTable *g_pStructTable;
extern CStatementList *g_pStatementList;
extern CInstructionTable *g_pInstructionTable;
extern CDebugInfo g_DebugInfo;
extern CDBPCompiler* g_pDBPCompiler;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatement::CStatement()
{
	m_dwLineNumber=0;
	m_dwStartChar=0;
	m_dwEndChar=0;
	m_bPerformJumpChecks=true; 

	m_dwObjectType=0;
	m_pObjectClass=NULL;
	m_pParameters=NULL;

	m_pNext=NULL;
}

CStatement::~CStatement()
{
	FreeObjects();
	SAFE_DELETE(m_pParameters);
}

void CStatement::Free(void)
{
	CStatement* pCurrent = this;
	while(pCurrent)
	{
		CStatement* pNext = pCurrent->GetNext();
		delete pCurrent;
		pCurrent = pNext;
	}
}

void CStatement::FreeObjects(void)
{
	// Delete Object Classes By Type
	CStatement* pCurrent=this;
	switch(pCurrent->m_dwObjectType)
	{
		case 1 :	{	CParseLoop* pPtr = (CParseLoop*)pCurrent->m_pObjectClass;
						SAFE_DELETE(pPtr);
						break;
					}

		case 2 :	{	CParseType* pPtr = (CParseType*)pCurrent->m_pObjectClass;
						SAFE_DELETE(pPtr);
						break;
					}

		case 3 :	{	CParseInit* pPtr = (CParseInit*)pCurrent->m_pObjectClass;
						SAFE_DELETE(pPtr);
						break;
					}

		case 6 :	{	CParseUserFunction* pPtr = (CParseUserFunction*)pCurrent->m_pObjectClass;
						SAFE_DELETE(pPtr);
						break;
					}

		case 8 :	{	CParseJump* pPtr = (CParseJump*)pCurrent->m_pObjectClass;
						SAFE_DELETE(pPtr);
						break;
					}

		case 11 :	{	CParseInstruction* pPtr = (CParseInstruction*)pCurrent->m_pObjectClass;
						SAFE_DELETE(pPtr);
						break;
					}

		case 12 :	{	CParseFunction* pPtr = (CParseFunction*)pCurrent->m_pObjectClass;
						SAFE_DELETE(pPtr);
						break;
					}
	}
}

void CStatement::Add(CStatement *pNext)
{
	CStatement* pCurrent = this;
	while(pCurrent->m_pNext)
	{
		pCurrent=pCurrent->GetNext();
	}
	pCurrent->m_pNext=pNext;
}

CStatement* CStatement::FindLastStatement(void)
{
	// lee - 290306 - u6rc4 - resursive calls kill the stack, use loop instead
//	if(GetNext()) return GetNext()->FindLastStatement();
//	return this;
	CStatement* pFoundStatement = this;
	if ( pFoundStatement )
	{
		// loop until no more, then we have found last statement
		while ( pFoundStatement->GetNext() ) pFoundStatement = pFoundStatement->GetNext();
	}
	return pFoundStatement;
}

void CStatement::SetData(DWORD LineNumber, DWORD dwObjectType, void* pPtr)
{
	m_dwLineNumber = LineNumber;
	m_dwStartChar=0;
	m_dwEndChar=0;
	m_dwObjectType = dwObjectType;
	m_pObjectClass = pPtr;
}

bool CStatement::DoPreScanBlock(DWORD RequiredTerminator)
{
	// Get Next Token from File Data
	DWORD TokenID = GetMainToken();
	while(1)
	{
		// Terminate if condition met
		if(TokenID==RequiredTerminator || TokenID==ENDTK || TokenID==0)
			break;

		// Respond to pre-scan items (lee: double check DIMTK in prescan!!)
		if(TokenID==DIMTK || TokenID==GLOBALTK || TokenID==USERFUNCTIONTK || TokenID==LABELTK || TokenID==TYPETK)
		{
			// Parse valid pre-scan item
			if(DoStatement(TokenID)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoPreScanBlock::DoStatement(TokenID)'");
				return false;
			}
		}
		else
		{
			// Advance pointer to next line (add3d==13 at 240604 as a leefix)
			if ( *g_pStatementList->GetFileDataPointer()==13 ) g_pStatementList->IncLineNumber();
			g_pStatementList->IncFileDataPointer();
			SkipToCR();
		}

		// Advance past REMSTART/REMEND Blocks
		if(TokenID==REMSTARTTK)
		{
			// Comment eveyrthing before remend
			LPSTR pPointer=g_pStatementList->GetFileDataPointer();
			pPointer = SeekToRemEnd(pPointer);
			g_pStatementList->SetFileDataPointer(pPointer);
			g_pStatementList->SetDisableParsingFull(false);
		}

		// Get next token
		TokenID = GetMainToken();
	}

	// Determine Block failure
	if(TokenID==0)
	{
		// Failed to compile block
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+11);
		return false;
	}

	// Determine Block not nested correctly
	if(TokenID!=RequiredTerminator)
	{
		// Failed to close nest
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+12);
		return false;
	}

	// Complete
	return true;
}

bool CStatement::DoLocalScanBlock(DWORD RequiredTerminator)
{
	// Get Next Token from File Data
	DWORD TokenID = GetMainToken();
	while(1)
	{
		// Terminate if condition met
		if(TokenID==RequiredTerminator || TokenID==ENDTK || TokenID==0)
			break;

		// Only look for local defs (to determine return type)
		// lee - 130206 - it seems DIM inside function is GLOBAL by legacy demands, so add GLOBAL too (for u60 bugfix)
		if(TokenID==LOCALTK || TokenID==LABELTK || TokenID==DIMTK || TokenID==GLOBALTK )
		{
			// Perform parsing of statement
			if(DoStatement(TokenID)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoStatement(TokenID)'");
				return false;
			}
		}
		else
		{
			// Advance pointer to next line (add3d==13 at 240604 as a leefix)
			if ( *g_pStatementList->GetFileDataPointer()==13 ) g_pStatementList->IncLineNumber();
			g_pStatementList->IncFileDataPointer();
			SkipToCR();
		}

		// Get next token
		TokenID = GetMainToken();
	}

	// Complete
	return true;
}

bool CStatement::DoInitCode(void)
{
	// Add a STORE STACK statement here
	AddInternalStatement(0, IT_INTERNAL_STARTPROGRAM);

	// Complete
	return true;
}

bool CStatement::DoEndCode(void)
{
	// Line Number
	DWORD dwCodeIndex = g_pStatementList->GetLineNumber();

	// Add a RESTORE STACK statement here
	CStatement *pRESTORECodeForStopper = AddInternalStatement(dwCodeIndex, IT_INTERNAL_ENDPROGRAM);
	pRESTORECodeForStopper->SurpressJumpChecks();

	// Add a RETURN statement here
	CStatement* pRetStat = AddInternalStatement(dwCodeIndex, IT_INTERNAL_PURERETURN);
	pRetStat->SurpressJumpChecks();

	// Create string and keep for usage later on
	CStr* pLabelName = new CStr("$labelend");
	g_pStatementList->IncLabelIndexCounter(1);

	// Add Label to Table
	if(g_pLabelTable->AddLabel(pLabelName->GetStr(), dwCodeIndex, 0, pRESTORECodeForStopper)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'CStatement::DoEndCode::AddLabel'");
		SAFE_DELETE(pLabelName);
		return false;
	}

	// Free usages
	SAFE_DELETE(pLabelName);

	// Complete
	return true;
}

CStatement* CStatement::AddInternalStatement(DWORD dwCodeIndex, DWORD dwInternalCode)
{
	// Add a RESTORE STACK statement here
	CStatement *pStatement = new CStatement();
	CParseInstruction *pInstruction = new CParseInstruction();
	pInstruction->SetType(2);
	pInstruction->SetValue(g_pInstructionTable->GetIIValue(dwInternalCode));
	pInstruction->SetInstructionRef(g_pInstructionTable->GetRef(dwInternalCode));
	pInstruction->SetParamMax(0);
	pInstruction->SetReturnParameter(NULL);
	pInstruction->SetParameter(NULL);
	pInstruction->SetLineNumber(dwCodeIndex);
	pStatement->m_dwObjectType = 11;
	pStatement->m_pObjectClass = (void*)pInstruction;
	pStatement->m_pParameters = NULL;
	pStatement->SetLineAndCharPos(dwCodeIndex);
	Add(pStatement);

	// Complete
	return pStatement;
}

void CStatement::SetLine(DWORD dwLine)
{
	// Set Line of Statement
	m_dwLineNumber = dwLine;
}

void CStatement::SetLineAndCharPos(DWORD dwLine)
{
	// Set Line of Statement
	m_dwLineNumber = dwLine;

	// Set Start and End Chars of Statement
	m_dwStartChar = g_pStatementList->GetLastCharInDataPosition();

	// Make sure no spaces or : symbols at end (neater)
	LPSTR pPtr = g_pStatementList->GetFileDataPointer();
	while(pPtr-g_pStatementList->GetFileDataStart()>(int)m_dwStartChar)
	{
		if(*(unsigned char*)(pPtr-1)<=32 || *(pPtr-1)==':')
			pPtr--;
		else
			break;
	}
	m_dwEndChar = (pPtr-g_pStatementList->GetFileDataStart());
}

void CStatement::SetLineAndCharPos(DWORD dwLine, int iFlag)
{
	SetLineAndCharPos(dwLine);
	m_dwStartChar = g_pStatementList->GetLastStampedCharInDataPosition();
}

bool CStatement::DoBlock(DWORD RequiredTerminator, DWORD* dwLastToken)
{
	// If terminator is CRTK, can exit when line changes too
	int iMustStayOnThisLineOrTerminate=-1;
	if(RequiredTerminator==CRTK || RequiredTerminator==ELSECRTK)
		iMustStayOnThisLineOrTerminate=g_pStatementList->GetLineNumber();

	// Get Next Token from File Data
	bool bCloseNestValid=false;
	DWORD TokenID = GetMainToken();
	while(1)
	{
		// Terminator token ends a block
		if(g_pStatementList->GetDisableParsingFull()==false)
			if(TokenID==RequiredTerminator)
			{
				bCloseNestValid=true;
				break;
			}

		// Terminate if no choice
		if(TokenID==ENDTK || TokenID==0)
			break;

		// Terminate if combination terminator
		if(RequiredTerminator==ELSEENDIFTK && (TokenID==ELSETK || TokenID==ENDIFTK))
		{
			bCloseNestValid=true;
			break;
		}
		if(RequiredTerminator==ELSECRTK && TokenID==ELSETK)
		{
			bCloseNestValid=true;
			break;
		}

		// Perform parsing of statement
		if(DoStatement(TokenID)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoStatement(TokenID)'");
			return false;
		}

		// Advance past any commented text
		if(g_pStatementList->GetDisableParsingFull()==true)
		{
			if(g_pStatementList->GetDisableParsingToCR()==true)
			{
				// Comment the line
				g_pStatementList->SetDisableParsingToCR(false);
				SkipToCR();
			}
			else
			{
				// Comment eveyrthing before remend
				LPSTR pPointer=g_pStatementList->GetFileDataPointer();
				pPointer = SeekToRemEnd(pPointer);
				g_pStatementList->SetFileDataPointer(pPointer);
			}

			// Restore normal parsing
			g_pStatementList->SetDisableParsingFull(false);
		}

		// One line statements can be terminated this way
		if(iMustStayOnThisLineOrTerminate!=-1)
		{
			if(g_pStatementList->GetLineNumber()!=(DWORD)iMustStayOnThisLineOrTerminate)
			{
				bCloseNestValid=true;
				break;
			}
		}

		// Get next statement
		TokenID = GetMainToken();
	}

	// Determine Block failure
	if(TokenID==0)
	{
		// Failed to compile block
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+11);
		return false;
	}

	// One line statements can be terminated this way
	if(iMustStayOnThisLineOrTerminate!=-1)
	{
		if(TokenID==10010)
		{
			// If reached end of program, same line IF nest complete
			bCloseNestValid=true;
		}
	}

	// Determine Block not nested correctly
	if(bCloseNestValid==false)
	{
		// Failed to close nest
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+12);
		return false;
	}

	// Store value of last token
	if(dwLastToken)
	{
		*dwLastToken=TokenID;
	}

	// Complete
	return true;
}

bool CStatement::DoStatement(DWORD TokenID)
{
	// Get Statement Line Number
	DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();

	// Progress Reporting Tool
	g_pErrorReport->ProgressReport("Parser now at line ",StatementLineNumber);

	// Determine Statement Type
	DWORD dwStatementType=0;
	DWORD dwStatementTypeData=0;
	switch(TokenID)
	{
		case DOTK:
		case WHILETK:			
		case REPEATTK:			
		case FORTK:				
		case EXITTK:			dwStatementType=1;
								break;

		case TYPETK:			dwStatementType=2;
								break;

		case GLOBALTK:			dwStatementTypeData=1;
								dwStatementType=3;
								break;

		case LOCALTK:			dwStatementTypeData=2;
								dwStatementType=3;
								break;


		case DIMTK:				dwStatementType=4;
								break;

		case UNDIMTK:			dwStatementType=13;
								break;

		case ASSIGNMENTTK:		dwStatementType=5;
								break;

		case USERFUNCTIONTK:	dwStatementType=6;
								break;

		case USERFUNCTIONCALLTK:dwStatementType=7;
								break;

		case EXITUSERFUNCTIONTK:dwStatementType=14;
								break;

		case IFTK:				
		case GOTOTK:			
		case GOSUBTK:			
		case SELECTTK:			dwStatementType=8;
								break;

		case LABELTK :			dwStatementType=9;
								break;



		case REMLINETK:			if(g_pStatementList->GetDisableParsingFull()==false)
								{
									g_pStatementList->SetDisableParsingToCR(true);	
									g_pStatementList->SetDisableParsingFull(true);
									dwStatementType=10;
								}
								break;

		case REMSTARTTK:		if(g_pStatementList->GetDisableParsingFull()==false)
								{
									g_pStatementList->SetDisableParsingFull(true);
									dwStatementType=10;
								}
								break;

		case REMENDTK:			if(g_pStatementList->GetDisableParsingFull()==true)
								{
									g_pStatementList->SetDisableParsingFull(false);
									dwStatementType=10;
								}
								break;

		case INSTRUCTIONTK :	dwStatementType=11;
								break;

		case DATATK :			dwStatementType=12;
								break;

		case ENDTK :			dwStatementType=99;
								break;
	}

	// If parsing Enabled when no comments
	if(g_pStatementList->GetDisableParsingFull()==true) dwStatementType=10;

	// Handle Statement Types
	switch(dwStatementType)
	{
		case 1 :	return DoLoop(StatementLineNumber, TokenID);
		case 2 :	return DoType(StatementLineNumber, TokenID);
		case 3 :	
		case 4 :	{
						// Process declaration of variable/array name
						bool bDoneDim=false;
						if(dwStatementType==4) bDoneDim=true;
						CDeclaration* pDecChain = NULL;
						bool bIsGlobal=false;
						if(dwStatementTypeData==1) bIsGlobal=true;
						if(dwStatementTypeData==0 && dwStatementType==4)
						{
							// leefix - 290703 - a DIM in a userfunction is GLOBAL unless otherwise regarded
							bIsGlobal=true;
						}
						if(DoDeclaration(true, CRTK, &pDecChain, bDoneDim, true, bIsGlobal, false)==false)
						{
							g_pErrorReport->AddErrorString("Failed to 'DoStatement::DoDeclaration'");
							SAFE_DELETE(pDecChain);
							return false;
						}
						SAFE_DELETE(pDecChain);
						return true;
					}
		case 5 :	return DoAssignment(StatementLineNumber, TokenID);
		case 6 :	return DoUserFunction(StatementLineNumber, TokenID);
		case 7 :	return DoUserFunctionCall(StatementLineNumber, TokenID);
		case 8 :	return DoJump(StatementLineNumber, TokenID);
		case 9 :	return DoLabel(StatementLineNumber, TokenID);
		case 10 :	return true;
		case 11 :	return DoInstruction(StatementLineNumber, TokenID);
		case 12 :	return DoDataStatement(StatementLineNumber, TokenID);
		case 13 :	return DoDeAllocation(StatementLineNumber);
		case 14 :	return DoUserFunctionExit(StatementLineNumber, TokenID);
		case 99 :	return true;
	}

	// Must find Token or failure
	if(TokenID!=0)
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+13);
	else
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+11);

	// Failed to understand token soft fail
	return false;
}

bool CStatement::DoLoop(DWORD StatementLineNumber, DWORD TokenID)
{
	// Each time step into LOOP, increment nest count
	g_pStatementList->m_iNestCount++;

	// This is LOOP Type 1
	DWORD dwStatementType=1;

	// Terminator leaves current loop
	if(TokenID==EXITTK) 
	{
		// If within a parsing loop, give exit label to exit statement
		if(g_pStatementList->GetLatestLoopExitLabel())
		{
			// Construct into full label ($label +)
			CParameter* pExitLabelRef = g_pStatementList->GetLatestLoopExitLabel();

			// Create Object
			CParseJump *pJump = new CParseJump();

			// Complete Object Data
			pJump->SetType(JUMPTYPE_GOTO);
			pJump->SetBlockA(NULL);
			pJump->SetBlockALabel(NULL);
			pJump->SetBlockB(NULL);
			pJump->SetBlockBLabel(NULL);
			pJump->SetBlockChain(NULL);
			pJump->SetExitLabelRefParameterRef(pExitLabelRef);
			pJump->SetConditionParameter(NULL);
			pJump->SetConditionCaseParameter(NULL);
			pJump->SetConditionLabelParameter(NULL);
			pJump->SetStartLineNumber(StatementLineNumber);
			pJump->SetMiddleLineNumber(StatementLineNumber);
			pJump->SetEndLineNumber(StatementLineNumber);

			// Add The Object To This Statement
			CStatement *TheObject = new CStatement();
			TheObject->SetLineAndCharPos(StatementLineNumber);
			TheObject->m_dwObjectType = 8;
			TheObject->m_pObjectClass = (void*)pJump;
			this->Add(TheObject);
		}

		// Each time step out of LOOP, decrement nest count
		g_pStatementList->m_iNestCount--;

		// complete early
		return true;
	}

	// Prepare Exit Param now (in case of EXIT statement ref)
	CParameter* pHoldOldLabel = g_pStatementList->GetLatestLoopExitLabel();
	CParameter* pEndLabel = new CParameter;
	g_pStatementList->SetLatestLoopExitLabel(pEndLabel);

	// Determine Type Of Loop
	DWORD dwLoopType=0, dwEndToken=0;
	if(TokenID==DOTK)		{ dwLoopType=LOOPTYPE_DO;		dwEndToken=LOOPTK;		}
	if(TokenID==WHILETK)	{ dwLoopType=LOOPTYPE_WHILE;	dwEndToken=ENDWHILETK;	}
	if(TokenID==REPEATTK)	{ dwLoopType=LOOPTYPE_REPEAT;	dwEndToken=UNTILTK;		}
	if(TokenID==FORTK)		{ dwLoopType=LOOPTYPE_FORNEXT;	dwEndToken=NEXTTK;		}

	// Parse off any trailing parameters of loop starter (WHILE xxxx)
	CStatement* pSkipInitLabelStatement = NULL;
	CParameter* pConditionParameter = NULL;
	CParameter* pForNextInitParameter = NULL;
	CParameter* pForNextIncParameter = NULL;
	CParameter* pForNextCheckParameter = NULL;
	if(TokenID==FORTK) 
	{
		// Gather fornext details
		CStr* pVar = new CStr("");
		CStr* pInitValue = new CStr("");
		CStr* pEndValue = new CStr("");
		CStr* pStepValue = new CStr("");
		ExtractDetailsFromForNext(pVar, pInitValue, pEndValue, pStepValue);
		bool bFixedStepDirection = true;
		if(pStepValue->Length()==0)
		{
			pStepValue->SetText("1");
		}
		else
		{
			// lee - 030406 - u6rc5 - if step specified, and not literal, use any-step-direction code
			if ( pStepValue->IsTextNumericValue()==false )
			{
				// expression () or () means any function will be called twice!
				bFixedStepDirection = false;
			}
		}

		// leefix - 210604 - u54 -Make sure var is not pure number
		if ( pVar->IsTextIntegerOnlyValue() )
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+34);
			return false;
		}

		// leefix - 010306 - u60 - further, only allow strict variables, no types (ie leevar.b)
		if ( pVar->IsTextASingleVariable()==false )
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+68);
			return false;
		}

		// Overwrite result with variable name (ENSURE local variables are handled too!)
		CStr* pActualVar = new CStr("");
		CDeclaration* pGlobalDecChain = g_pStatementList->GetUserFunctionDecChain();
		if(pGlobalDecChain)
		{
			// Local Variable
			pActualVar->SetText("FS@");
			pActualVar->AddText(g_pStatementList->GetUserFunctionName());
			pActualVar->AddText("@");
			pActualVar->AddText(pVar);
		}
		else
		{
			// Global Variable
			pActualVar->SetText("@");
			pActualVar->AddText(pVar);
		}

		// LEEFIX - 281102 - Can cause float values to be written into an integer var
		// so we find the variable type and ensure all values are same type
			
		// Add Variable to variable table (if not exist)
		DWORD dwAction=0;
		LPSTR pDecType = g_pVarTable->MakeDefaultVarType(pActualVar->GetStr());
//		if(g_pVarTable->AddVariable(pActualVar->GetStr()+1, pDecType, 0, 0, false, &dwAction, false)==false) -leefix-040803-turns out a new var was created
		if(g_pVarTable->AddVariable(pActualVar->GetStr()+1, pDecType, 0, 0, true, &dwAction, false)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoValueSingleVariable::Add Variable'");
			SAFE_DELETE(pDecType);
			return false;
		}
		SAFE_DELETE(pDecType);
			
		// Find/Make Variable from pVar (required for type)
//		CVarTable* pVarTest = g_pVarTable->FindVariable(g_pStatementList->GetUserFunctionName(), pActualVar->GetStr(), 0);-leefix-040803-use the actual varname nt the @ symbol
		CVarTable* pVarTest = g_pVarTable->FindVariable(g_pStatementList->GetUserFunctionName(), pActualVar->GetStr()+1, 0);
		if(pVarTest==NULL)
		{
			g_pErrorReport->AddErrorString("Failed to 'FindVariable::DoLoop'");
			SAFE_DELETE(pDecType);
			return false;
		}

		// Determine direction of fornext loop
		bool bForwardStep=true;
		if(pStepValue->GetValue()<0) bForwardStep=false;

		// Create Init Maths - result in T
		CStr* pInitStr = new CStr("");
		pInitStr->SetText(pInitValue);
		pInitStr->AddText("-");
		pInitStr->AddText("(");
		pInitStr->AddText(pStepValue);
		pInitStr->AddText(")");
		pForNextInitParameter = new CParameter;
		if(DoExpression(pInitStr, pForNextInitParameter)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoLoop::DoExpression'");
			SAFE_DELETE(pVar);
			SAFE_DELETE(pInitStr);
			SAFE_DELETE(pInitValue);
			SAFE_DELETE(pEndValue);
			SAFE_DELETE(pStepValue);
			SAFE_DELETE(pForNextInitParameter);
			SAFE_DELETE(pEndLabel);
			SAFE_DELETE(pActualVar);
			return false;
		}
		CStr* pResult = pForNextInitParameter->GetMathItem()->FindResultStringTokenForDBM();
		SAFE_DELETE(pInitStr);

		// Ensure value type matches variable type
		DWORD dwValueType = pForNextInitParameter->GetMathItem()->FindResultTypeValueForDBM();
		DWORD dwTheVarType = pVarTest->GetVarTypeValue();

// leefix - 040803 - only prevent float and non-float types to co-exist in same 
//		if(dwValueType!=dwTheVarType)
		bool bAllowTypes = true;
		if ( dwTheVarType%100==2 && dwTheVarType!=dwValueType ) bAllowTypes = false;
		if ( dwTheVarType%100==8 && dwTheVarType!=dwValueType ) bAllowTypes = false;
		if ( bAllowTypes==false )
		{
			// Value type must match variable type
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+49);
			SAFE_DELETE(pVar);
			SAFE_DELETE(pInitStr);
			SAFE_DELETE(pInitValue);
			SAFE_DELETE(pEndValue);
			SAFE_DELETE(pStepValue);
			SAFE_DELETE(pForNextInitParameter);
			SAFE_DELETE(pEndLabel);
			SAFE_DELETE(pActualVar);
			return false;
		}
			
		// Set Result based on actual variable
		pResult->SetText(pActualVar->GetStr());

		// Create Increment Maths - result in T
		CStr* pIncStr = new CStr("");
		pIncStr->SetText(pVar);
		pIncStr->AddText("+");
		pIncStr->AddText(pStepValue);

		pForNextIncParameter = new CParameter;
		if(DoExpression(pIncStr, pForNextIncParameter)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoLoop::DoExpression'");
			SAFE_DELETE(pForNextIncParameter);
			SAFE_DELETE(pIncStr);
			SAFE_DELETE(pInitStr);
			SAFE_DELETE(pVar);
			SAFE_DELETE(pInitValue);
			SAFE_DELETE(pEndValue);
			SAFE_DELETE(pStepValue);
			SAFE_DELETE(pForNextInitParameter);
			SAFE_DELETE(pEndLabel);
			SAFE_DELETE(pActualVar);
			return false;
		}
		SAFE_DELETE(pIncStr);

		// Return Var should be FORNEXT Var
		pResult = pForNextIncParameter->GetMathItem()->FindResultStringTokenForDBM();
		pResult->SetText(pActualVar->GetStr());

		// Create Compare Maths
// leefix-250603-better condition to handle variable step value
//		pCompareStr->SetText(pVar);
//		if(bForwardStep)
//			pCompareStr->AddText("<=");
//		else
//			pCompareStr->AddText(">=");
//		pCompareStr->AddText(pEndValue);
		/* lee - 310306 - u6rc4 - double call to functions using this FORNEXT condition
		// solve by ensuring step values are literals ONLY, so we can exaluate here
		CStr* pCompareStr = new CStr("");
		pCompareStr->SetText("(");
		pCompareStr->AddText(pStepValue);
		pCompareStr->AddText(">0 AND ");
		pCompareStr->AddText(pVar);
		pCompareStr->AddText("<=");
		pCompareStr->AddText(pEndValue);
		pCompareStr->AddText(") OR (");
		pCompareStr->AddText(pStepValue);
		pCompareStr->AddText("<0 AND ");
		pCompareStr->AddText(pVar);
		pCompareStr->AddText(">=");
		pCompareStr->AddText(pEndValue);
		pCompareStr->AddText(")");
		*/
		// lee - 030406 - u6rc5 - based on step existence
		CStr* pCompareStr = new CStr("");
		if ( bFixedStepDirection==true )
		{
			// known direction
			pCompareStr->SetText(pVar);
			if(bForwardStep)
				pCompareStr->AddText("<=");
			else
				pCompareStr->AddText(">=");
			pCompareStr->AddText(pEndValue);
		}
		else
		{
			// either way
			pCompareStr->SetText("(");
			pCompareStr->AddText(pStepValue);
			pCompareStr->AddText(">0 AND ");
			pCompareStr->AddText(pVar);
			pCompareStr->AddText("<=");
			pCompareStr->AddText(pEndValue);
			pCompareStr->AddText(") OR (");
			pCompareStr->AddText(pStepValue);
			pCompareStr->AddText("<0 AND ");
			pCompareStr->AddText(pVar);
			pCompareStr->AddText(">=");
			pCompareStr->AddText(pEndValue);
			pCompareStr->AddText(")");
		}
		pForNextCheckParameter = new CParameter;
		if(DoExpression(pCompareStr, pForNextCheckParameter)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoLoop::DoExpression'");
			SAFE_DELETE(pCompareStr);
			SAFE_DELETE(pForNextCheckParameter);
			SAFE_DELETE(pForNextIncParameter);
			SAFE_DELETE(pIncStr);
			SAFE_DELETE(pInitStr);
			SAFE_DELETE(pVar);
			SAFE_DELETE(pInitValue);
			SAFE_DELETE(pEndValue);
			SAFE_DELETE(pStepValue);
			SAFE_DELETE(pForNextInitParameter);
			SAFE_DELETE(pEndLabel);
			SAFE_DELETE(pActualVar);
			return false;
		}
		SAFE_DELETE(pCompareStr);

		// Create Result Parameter for condition
		CMathOp* pMathOp = new CMathOp;
		CStr* pTempVarToken = new CStr("");
		pMathOp->ProduceNewTempToken(pTempVarToken, 1);
		SAFE_DELETE(pMathOp);

		// Assign final result to new temp variable
		pResult = pForNextCheckParameter->GetMathItem()->FindResultStringTokenForDBM();
		pResult->SetText(pTempVarToken->GetStr());

		SAFE_DELETE(pTempVarToken);

		// Free usages
		SAFE_DELETE(pVar);
		SAFE_DELETE(pInitValue);
		SAFE_DELETE(pEndValue);
		SAFE_DELETE(pStepValue);
		SAFE_DELETE(pActualVar);
	}
	else
	{
		if(TokenID==WHILETK) 
		{
			// Create Parameter Object
			bool bMoreParams=false;
			SAFE_DELETE(pConditionParameter);
			if(DoExpressionList(&pConditionParameter,&bMoreParams)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'WHILETK::Do ExpressionList'");
				SAFE_DELETE(pConditionParameter);
				SAFE_DELETE(pEndLabel);
				return false;
			}
			// Not one param, fail
			if(bMoreParams==true || pConditionParameter==NULL)
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+14);
				SAFE_DELETE(pConditionParameter);
				SAFE_DELETE(pEndLabel);
				return false;
			}
		}
	}

	// Get dwTopOfLoopLine
	DWORD dwTopOfLoopLine = g_pStatementList->GetTokenLineNumber();

	// Create Statement object for Main Loop Object
	CStatement* TheLoopObject = new CStatement();
	TheLoopObject->SetLineAndCharPos(dwTopOfLoopLine);

	// Create BLOCK Object for nested code
	CStatement *pLoopBlock = new CStatement();
	pLoopBlock->SetLineAndCharPos(dwTopOfLoopLine);

	// Now Parse Block from File Data
	DWORD dwReturnToken=NULL;
	if(pLoopBlock->DoBlock(dwEndToken,&dwReturnToken)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'pLoopBlock->DoBlock()'");
		SAFE_DELETE(pConditionParameter);
		SAFE_DELETE(pLoopBlock);
		SAFE_DELETE(pEndLabel);
		return false;
	}

	// Get dwBottomOfLoopLine
	DWORD dwBottomOfLoopLine = g_pStatementList->GetLineNumber();

	// Parse off any trailing parameters of loop terminator (UNTIL xxxx)
	if(dwEndToken==UNTILTK) 
	{
		// Create Parameter Object
		bool bMoreParams=false;
		SAFE_DELETE(pConditionParameter);
		if(DoExpressionList(&pConditionParameter,&bMoreParams)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'UNTILTK::Do ExpressionList'");
			SAFE_DELETE(pConditionParameter);
			SAFE_DELETE(pLoopBlock);
			SAFE_DELETE(pEndLabel);
			return false;
		}
		// If Not one param, fail
		if(bMoreParams==true || pConditionParameter==NULL)
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+15);
			SAFE_DELETE(pConditionParameter);
			SAFE_DELETE(pLoopBlock);
			SAFE_DELETE(pEndLabel);
			return false;
		}
	}
	if(dwEndToken==NEXTTK) 
	{
		// Parse off variable
		bool bMoreParams=false;
		CParameter* pNextVarParameter = NULL;
		if(DoExpressionList(&pNextVarParameter,&bMoreParams)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'NEXTTK::Do ExpressionList'");
			SAFE_DELETE(pNextVarParameter);
			SAFE_DELETE(pLoopBlock);
			SAFE_DELETE(pEndLabel);
			return false;
		}
		// If Not one param, fail
//		if(bMoreParams==true || pNextVarParameter==NULL) -lefix-2606040u54-will allow no vars at end of NEXT
		if(bMoreParams==true)
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+16);
			SAFE_DELETE(pNextVarParameter);
			SAFE_DELETE(pLoopBlock);
			SAFE_DELETE(pEndLabel);
			return false;
		}
		SAFE_DELETE(pNextVarParameter);
	}

	// Add Empty Statement To Mark LoopMiddle Label
	if(pForNextInitParameter)
	{
		CStatement* pSkipInitLabelStatement = new CStatement;
		pSkipInitLabelStatement->m_dwObjectType = 0;
		pSkipInitLabelStatement->m_pObjectClass = NULL;
		pSkipInitLabelStatement->m_pParameters = pForNextInitParameter;
		pSkipInitLabelStatement->SetLine(dwTopOfLoopLine);
		this->Add(pSkipInitLabelStatement);
	}

	// This is an internal label between fornext init code and inc code
	CStatement* pTopLabelStatement = NULL;
	if(dwLoopType==LOOPTYPE_DO) pTopLabelStatement = pLoopBlock;
	if(dwLoopType==LOOPTYPE_WHILE) pTopLabelStatement = TheLoopObject;
	if(dwLoopType==LOOPTYPE_REPEAT) pTopLabelStatement = pLoopBlock;
	if(dwLoopType==LOOPTYPE_FORNEXT)
	{
		pTopLabelStatement = new CStatement;
		pTopLabelStatement->m_dwObjectType = 0;
		pTopLabelStatement->m_pObjectClass = NULL;
		pTopLabelStatement->m_pParameters = pForNextIncParameter;
		pTopLabelStatement->SetLine(dwTopOfLoopLine);
		this->Add(pTopLabelStatement);
	}

	// Create Unique internal label TOP
	CStr* pTopOfLoopInternalLabel=NULL;
	if(pTopLabelStatement->AddInternalLabel(&pTopOfLoopInternalLabel)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'AddInternalLabel' pTopOfLoopInternalLabel");
		SAFE_DELETE(pTopOfLoopInternalLabel);
		SAFE_DELETE(pTopLabelStatement);
		SAFE_DELETE(TheLoopObject);
		SAFE_DELETE(pConditionParameter);
		SAFE_DELETE(pLoopBlock);
		SAFE_DELETE(pEndLabel);
		return false;
	}
	CParameter* pStartLabel = new CParameter;
	pStartLabel->SetParamAsLabel(pTopOfLoopInternalLabel);
	SAFE_DELETE(pTopOfLoopInternalLabel);

	// Create Object
	CParseLoop *pLoop = new CParseLoop();

	// Complete Object Data
	pLoop->SetType(dwLoopType);
	pLoop->SetBlock(pLoopBlock);
	pLoop->SetSLabelParameter(pStartLabel);
	pLoop->SetConditionParameter(pConditionParameter);
	pLoop->SetForNextInitParameter(NULL);
	pLoop->SetForNextIncParameter(NULL);
	pLoop->SetForNextCheckParameter(pForNextCheckParameter);
	pLoop->SetStartLineNumber(dwTopOfLoopLine);
	pLoop->SetEndLineNumber(g_pStatementList->GetTokenLineNumber());

	// Create Unique internal END label BOTTOM
	CStatement* pBlankStatement = new CStatement();
	pBlankStatement->SetLineAndCharPos(dwBottomOfLoopLine);
	CStr* pBottomOfLoopInternalLabel=NULL;
	if(pBlankStatement->AddInternalLabel(&pBottomOfLoopInternalLabel)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'AddInternalLabel' pBottomOfLoopInternalLabel");
		SAFE_DELETE(pBottomOfLoopInternalLabel);
		SAFE_DELETE(pBlankStatement);
		SAFE_DELETE(pLoop);
		SAFE_DELETE(pStartLabel);
		SAFE_DELETE(pTopLabelStatement);
		SAFE_DELETE(TheLoopObject);
		SAFE_DELETE(pConditionParameter);
		SAFE_DELETE(pLoopBlock);
		SAFE_DELETE(pEndLabel);
		return false;
	}

	// Exit Label prepared at top
	pEndLabel->SetParamAsLabel(pBottomOfLoopInternalLabel);
	pLoop->SetELabelParameter(pEndLabel);
	SAFE_DELETE(pBottomOfLoopInternalLabel);

	// Add The Main Loop Object To Statements
	TheLoopObject->m_dwObjectType = dwStatementType;
	TheLoopObject->m_pObjectClass = (void*)pLoop;
	this->Add(TheLoopObject);

	// Add Empty Statement To Mark LoopEnd Label
	pBlankStatement->m_dwObjectType = 999;//debug comment
	pBlankStatement->m_pObjectClass = NULL;
	pBlankStatement->m_pParameters = NULL;
	pBlankStatement->SetLineAndCharPos(dwBottomOfLoopLine);
	this->Add(pBlankStatement);

	// Restore old exit loop label reference (for deep nests)
	g_pStatementList->SetLatestLoopExitLabel(pHoldOldLabel);

	// Each time step out of LOOP, decrement nest count
	g_pStatementList->m_iNestCount--;

	// complete
	return true;
}

bool CStatement::DoJump(DWORD StatementLineNumber, DWORD TokenID)
{
	// This is JUMP Type
	DWORD dwStatementType=8;

	// Create main statement now at start of jump declaration
	CStatement* TheObject = new CStatement();
	g_pStatementList->SetLastStampedCharInDataPosition();

	// Determine Type Of Jump
	DWORD dwJumpType=0, dwEndToken=0;
	if(TokenID==IFTK) { dwJumpType=JUMPTYPE_IF;			dwEndToken=ENDIFTK;		}
	if(TokenID==GOTOTK) { dwJumpType=JUMPTYPE_GOTO;		dwEndToken=0;		}
	if(TokenID==GOSUBTK) { dwJumpType=JUMPTYPE_GOSUB;	dwEndToken=0;		}
	if(TokenID==SELECTTK) { dwJumpType=JUMPTYPE_SELECT;	dwEndToken=ENDSELECTTK;		}

	// Make THEN a seperator
	bool bOneLineIfThen=false;
	if(TokenID==IFTK)
	{
		// Modify THEN line into seperator
		if(ReplaceTHENandELSEwithSep()==true)
			bOneLineIfThen=true;
	}

	// Only GOTO and GOSUB can use Labels as Values
	if(dwJumpType==JUMPTYPE_GOTO || dwJumpType==JUMPTYPE_GOSUB)
	{
		g_pStatementList->SetAllowLabelAsValue(true);
	}

	// Create Parameter Object
	bool bMoreParams=false;
	CParameter* pConditionParameter = NULL;
	CParameter* pConditionCaseParameter = NULL;
	CParameter* pConditionLabelParameter = NULL;
	if(DoExpressionList(&pConditionParameter,&bMoreParams)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoJump::Do ExpressionList'");
		SAFE_DELETE(pConditionParameter);
		SAFE_DELETE(TheObject);
		return false;
	}

	// Only GOTO and GOSUB can use Labels as Values
	g_pStatementList->SetAllowLabelAsValue(false);

	// If Not one param, fail
	if(bMoreParams==true || pConditionParameter==NULL)
	{
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+17);
		SAFE_DELETE(pConditionParameter);
		SAFE_DELETE(TheObject);
		return false;
	}

	// leeadd - 280305 - condition datatype must result as an integer
	if ( TokenID==IFTK || TokenID==SELECTTK )
	{
		// Ensure select variable is casted out of types 4,5,6
		if ( (pConditionParameter->GetMathItem()->FindResultTypeValueForDBM() % 100 )>=4
		&&   (pConditionParameter->GetMathItem()->FindResultTypeValueForDBM() % 100 )<=6 )
		{
			CMathOp* pValueToCast = pConditionParameter->GetMathItem();
			pConditionParameter->GetMathItem()->DoCastOnMathOp ( &pValueToCast, 1 );
			pConditionParameter->SetMathItem(pValueToCast);
		}
	}

	CStatement *pJumpBlockA = NULL;
	CStatement* pJumpBlockB = NULL;
	CStatementChain* pJumpBlockChain = NULL;
	CStr* pInternalLabelStringA = NULL;
	CStr* pInternalLabelStringB = NULL;
	DWORD StatementElseLineNumber=0;
	if(TokenID==IFTK)
	{
		// Create BLOCK Object for A Block
		pJumpBlockA = new CStatement();

		// Parse A Block
		DWORD dwlastToken=0;

		if(bOneLineIfThen)
			dwEndToken=ELSECRTK;
		else
			dwEndToken=ELSEENDIFTK;

		TheObject->SetLineAndCharPos(StatementLineNumber,1);
		if(pJumpBlockA->DoBlock(dwEndToken,&dwlastToken)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'pJumpBlockA->DoBlock'");
			SAFE_DELETE(pConditionParameter);
			SAFE_DELETE(pJumpBlockA);
			SAFE_DELETE(TheObject);
			return false;
		}

		// If Next token is ELSE, parse a B Block
		if(dwlastToken==ELSETK)
		{
			if(bOneLineIfThen)
				dwEndToken=CRTK;
			else
				dwEndToken=ENDIFTK;

			// BlockB statement is else marker for jump
			pJumpBlockB = new CStatement();
			pJumpBlockB->SetLineNumber(g_pStatementList->GetLineNumber());
			pJumpBlockB->SetLineAndCharPos(StatementLineNumber);

			// Create Unique internal label for B (else default block)
			if(pJumpBlockB->AddInternalLabel(&pInternalLabelStringB)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'AddInternalLabel' B");
				SAFE_DELETE(pInternalLabelStringB);
				SAFE_DELETE(pConditionParameter);
				SAFE_DELETE(pJumpBlockA);
				SAFE_DELETE(pJumpBlockB);
				SAFE_DELETE(TheObject);
				return false;
			}

			// Process code block for else
			if(pJumpBlockB->DoBlock(dwEndToken,NULL)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'pJumpBlockB->DoBlock()'");
				SAFE_DELETE(pConditionParameter);
				SAFE_DELETE(pJumpBlockA);
				SAFE_DELETE(pJumpBlockB);
				SAFE_DELETE(TheObject);
				return false;
			}
		}
	}
	if(TokenID==GOTOTK || TokenID==GOSUBTK)
	{
		// Make label from passed parameter
		CStr* pParamStr = pConditionParameter->GetMathItem()->GetResultStringToken();
		CParameter* pJumpToLabel = NULL;
		CStr* pFullLabel = NULL;
		LPSTR pLabel = NULL;
		if ( pParamStr )
		{
			// Get label data and skip marker
			pLabel = pParamStr->GetStr();
			if(*pLabel=='@') pLabel++;

			// Construct into full label ($label +)
			pFullLabel = new CStr("$label ");
			pFullLabel->AddText(pLabel);
			pJumpToLabel = new CParameter;
			pJumpToLabel->SetParamAsLabel(pFullLabel);
			if(g_pLabelTable->FindLabel(pFullLabel->GetStr())==NULL)
			{
				SAFE_DELETE(pFullLabel);
				SAFE_DELETE(pJumpToLabel);
				pLabel=NULL;
			}
		}

		// Ensure label exists
		if(pLabel==NULL)
		{
			if ( pFullLabel )
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+18, pFullLabel->GetStr());
			else
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+18, "?");

			SAFE_DELETE(pInternalLabelStringB);
			SAFE_DELETE(pConditionParameter);
			SAFE_DELETE(pJumpBlockA);
			SAFE_DELETE(pJumpBlockB);
			SAFE_DELETE(TheObject);
			return false;
		}
		SAFE_DELETE(pConditionParameter);
		SAFE_DELETE(pFullLabel);

		// Proceed to use valid label
		TheObject->SetLineAndCharPos(StatementLineNumber,1);
		pConditionParameter=pJumpToLabel;
	}
	if(TokenID==SELECTTK)
	{
		// Get Case Value
		DWORD dwlastToken=0;
		DWORD dwToken=0;

		// Parameter Chains to hold:
		CParameter* pCaseCondition = NULL;
		CParameter* pCaseLabel = NULL;

		// Create BLOCK Object for Main Case Run
		TheObject->SetLineAndCharPos(StatementLineNumber,1);
		bool bCaseDefaultProcessed=false;
		while(1)
		{
			// Get Pointer
			LPSTR pPointer = g_pStatementList->GetFileDataPointer();

			// Get Token
			bool bTokenUsed=false;
			dwToken = FindToken(pPointer, true);

			// lee - 240306 - end case found in wrong order
			if(dwToken==ENDCASETK)
			{
				// provoke error
				dwToken = 0;
			}

			// Proceed if CASE block
			if(dwToken==CASETK)
			{
				// Get Pointer
				bTokenUsed=true;
				LPSTR pPointer = g_pStatementList->GetFileDataPointer();

				// leefix - 250604 - u54 - advance beyond any spaces
				while ( *pPointer==32 && pPointer<g_pStatementList->GetFileDataEnd()-2 )
					pPointer++;

				g_pStatementList->SetFileDataPointer(pPointer);

				// default or literal
				if(strnicmp(pPointer, "default", 7)==NULL)
				{
					// CASE DEFAULT
					g_pStatementList->SetFileDataPointer(pPointer+7);
					bCaseDefaultProcessed=true;
					dwToken=CASEDEFAULTTK;
				}
				else
				{
					// Create a Label for this case block
					CParameter* pOneCaseLabelParam = NULL;

					// Run through all items in CASE statement
					bool bAtLeastOne=false;
					bool bMoreParams=true;
					while(bMoreParams)
					{
						bMoreParams=false;
						CParameter* pOneCaseParam=NULL;
						if(DoExpressionList(&pOneCaseParam,&bMoreParams)==false)
						{
							g_pErrorReport->AddErrorString("Failed to 'DoJump::Case::Do ExpressionList'");
							SAFE_DELETE(pCaseLabel);
							SAFE_DELETE(pCaseCondition);
							SAFE_DELETE(pOneCaseLabelParam);
							SAFE_DELETE(pOneCaseParam);
							SAFE_DELETE(pJumpBlockChain);
							SAFE_DELETE(pInternalLabelStringB);
							SAFE_DELETE(pConditionParameter);
							SAFE_DELETE(pJumpBlockA);
							SAFE_DELETE(pJumpBlockB);
							SAFE_DELETE(TheObject);
							return false;
						}

						// Add to param chain
						if(pOneCaseParam)
						{
							// Ensure param is literal constant!
							DWORD dwType=0;
							if(pOneCaseParam->GetMathItem()->IsLiteral(pOneCaseParam->GetMathItem()->GetResultStringToken(), &dwType)==false)
							{
								// Not literal, leave to report error
								bAtLeastOne=false;
								break;
							}
							else
							{
								// Add condition to chain
								bAtLeastOne=true;
								if(pCaseCondition==NULL)
									pCaseCondition=pOneCaseParam;
								else
									pCaseCondition->Add(pOneCaseParam);

								// Add Label for conditional jump
								CParameter* pAnotherLabel = new CParameter;
								if(pOneCaseLabelParam==NULL)
									pOneCaseLabelParam = pAnotherLabel;
								else
									pOneCaseLabelParam->Add(pAnotherLabel);
							}
						}
					}

					// If no param, fail
					if(bAtLeastOne==false)
					{
						DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
						g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+19);
						SAFE_DELETE(pCaseLabel);
						SAFE_DELETE(pCaseCondition);
						SAFE_DELETE(pOneCaseLabelParam);
						SAFE_DELETE(pJumpBlockChain);
						SAFE_DELETE(pInternalLabelStringB);
						SAFE_DELETE(pConditionParameter);
						SAFE_DELETE(pJumpBlockA);
						SAFE_DELETE(pJumpBlockB);
						SAFE_DELETE(TheObject);
						return false;
					}
				
					// Get Case Code
					dwEndToken=ENDCASETK;
					pJumpBlockA = new CStatement();
					pJumpBlockA->SetLineAndCharPos(StatementLineNumber);
					if(pJumpBlockA->DoBlock(dwEndToken,&dwlastToken)==false)
					{
						g_pErrorReport->AddErrorString("Failed to 'pJumpBlockA->DoBlock'");
						SAFE_DELETE(pCaseLabel);
						SAFE_DELETE(pCaseCondition);
						SAFE_DELETE(pOneCaseLabelParam);
						SAFE_DELETE(pJumpBlockChain);
						SAFE_DELETE(pInternalLabelStringB);
						SAFE_DELETE(pConditionParameter);
						SAFE_DELETE(pJumpBlockA);
						SAFE_DELETE(pJumpBlockB);
						SAFE_DELETE(TheObject);
						return false;
					}

					if(pJumpBlockA)
					{
						CStatementChain* pNewStatements = new CStatementChain;
						pNewStatements->SetStatementBlock(pJumpBlockA);
						if(pJumpBlockChain==NULL)
							pJumpBlockChain=pNewStatements;
						else
							pJumpBlockChain->Add(pNewStatements);
					}

					// Create Unique internal label for endcase markers
					CStr* pInternalLabelStringMarker=NULL;
					pJumpBlockA->SetLineNumber(g_pStatementList->GetLineNumber());
					if(pJumpBlockA->AddInternalLabel(&pInternalLabelStringMarker)==false)
					{
						g_pErrorReport->AddErrorString("Failed to 'AddInternalLabel' pInternalLabelStringMarker");
						SAFE_DELETE(pInternalLabelStringMarker);
						SAFE_DELETE(pCaseLabel);
						SAFE_DELETE(pCaseCondition);
						SAFE_DELETE(pOneCaseLabelParam);
						SAFE_DELETE(pJumpBlockChain);
						SAFE_DELETE(pInternalLabelStringB);
						SAFE_DELETE(pConditionParameter);
						SAFE_DELETE(pJumpBlockA);
						SAFE_DELETE(pJumpBlockB);
						SAFE_DELETE(TheObject);
						return false;
					}

					// Now property of statement chain!
					pJumpBlockA=NULL;

					// Write label name to parameter used to hold label to caseblock
					if(pInternalLabelStringMarker)
					{
						// Fill param labels with actual label name
						CParameter* pCurrent = pOneCaseLabelParam;
						while(pCurrent)
						{
							pCurrent->SetParamAsLabel(pInternalLabelStringMarker);
							pCurrent=pCurrent->GetNext();
						}

						// Add chain of labels onto main label chain
						if(pCaseLabel==NULL)
							pCaseLabel=pOneCaseLabelParam;
						else
							pCaseLabel->Add(pOneCaseLabelParam);

						// Delete usage
						SAFE_DELETE(pInternalLabelStringMarker);
					}

					// New token after block
					dwToken=dwlastToken;
				}
			}

			// Peek next, if another case go around ahain
			if(dwToken==ENDSELECTTK || bCaseDefaultProcessed==true)
			{
				bTokenUsed=true;
				break;
			}

			// Skip line that is not used
			if(bTokenUsed==false)
			{
				// lee - 110307 - u6.6 - ensure the ptr is updated to avoid (counting line inc twice (comment between ENDCASE and CASE)
				pPointer = g_pStatementList->GetFileDataPointer();

				// leefix - 310305 - replace with better comment skipper
				LPSTR pPtrEnd = g_pStatementList->GetFileDataEnd();
				LPSTR pStorePtr = pPointer;
				pPointer = SkipAllComments ( pPointer, pPtrEnd );

				// U76 - 300710 - above function would count line inc for CR, but new 'advanced' pointer needs
				// assignment back to the class (otherwise SkipToCR will go ahead and count the CR again!)
				g_pStatementList->SetFileDataPointer(pPointer); // this would return the ptr back to before the line was increemented causing the line numbers to get +1 out of whack

				// lee - 110307 - u6.6 - moved to above
				//if ( pPointer==pStorePtr ) pPointer = g_pStatementList->GetFileDataPointer();
				//// leenote - 150405 - stil not 100% - need further work here!
				//SkipToCR();
				//g_pStatementList->SetFileDataPointer(pPointer); // this would return the ptr back to before the line was increemented causing the line numbers to get +1 out of whack

				// lee - 110307 - u6.6 - if at end of comment, use this to skip the CR of it
				SkipToCR();
			}

			// Not found token error (lee - 240306 - u6b4 - added dwToken==ENDTK)
			if ( dwToken==0 || dwToken==ENDTK )
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+20);
				SAFE_DELETE(pCaseLabel);
				SAFE_DELETE(pCaseCondition);
				SAFE_DELETE(pJumpBlockChain);
				SAFE_DELETE(pInternalLabelStringB);
				SAFE_DELETE(pConditionParameter);
				SAFE_DELETE(pJumpBlockA);
				SAFE_DELETE(TheObject);
				return false;
			}
		}

		// If Next token is CASEDEFAULTTK, parse a B Block 'kindofelse'
		if(bCaseDefaultProcessed==true)
		{
			dwEndToken=ENDCASETK;
			pJumpBlockB = new CStatement();
			pJumpBlockB->SetLineAndCharPos(StatementLineNumber);
			if(pJumpBlockB->DoBlock(dwEndToken, &dwlastToken)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'pJumpBlockB->DoBlock()'");
				SAFE_DELETE(pCaseLabel);
				SAFE_DELETE(pCaseCondition);
				SAFE_DELETE(pJumpBlockChain);
				SAFE_DELETE(pInternalLabelStringB);
				SAFE_DELETE(pConditionParameter);
				SAFE_DELETE(pJumpBlockA);
				SAFE_DELETE(pJumpBlockB);
				SAFE_DELETE(TheObject);
				return false;
			}

			// Get Pointer for ENDSELECT
			LPSTR pPointer = g_pStatementList->GetFileDataPointer();

			// Get Token for ENDSELECT Only
			dwToken = FindToken(pPointer, true);
		}

		// LEEFIX - 141102 - Ensure no comments here
		while(dwToken==REMLINETK || dwToken==REMSTARTTK)
		{
			LPSTR pPointer;
			if(dwToken==REMLINETK)
			{
				// Skip to end of line
				SkipToCR();
				pPointer = g_pStatementList->GetFileDataPointer();
			}
			else
			{
				// Skip To Remend
				pPointer = g_pStatementList->GetFileDataPointer();
				pPointer = SeekToRemEnd( pPointer );
			}
			g_pStatementList->SetFileDataPointer(pPointer);
			dwToken = FindToken(pPointer, true);
		}

		if(dwToken!=ENDSELECTTK)
		{
			// If another case after case default alert of ordering error
			if(dwToken==CASETK && bCaseDefaultProcessed==true)
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+21);
			}
			else
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+22);
			}
			SAFE_DELETE(pCaseLabel);
			SAFE_DELETE(pCaseCondition);
			SAFE_DELETE(pJumpBlockChain);
			SAFE_DELETE(pInternalLabelStringB);
			SAFE_DELETE(pConditionParameter);
			SAFE_DELETE(pJumpBlockA);
			SAFE_DELETE(pJumpBlockB);
			SAFE_DELETE(TheObject);
			return false;
		}

		// Transfer chains to condition params for actual use
		pConditionCaseParameter=pCaseCondition;
		pConditionLabelParameter=pCaseLabel;
	}

	// Create Unique internal label for A (end of jumpblocks)
	CStatement* pBlankStatementForEndMarker = NULL;
	if(dwJumpType==JUMPTYPE_IF || dwJumpType==JUMPTYPE_SELECT)
	{
		// Create blank statement for jump end marker
		pBlankStatementForEndMarker = new CStatement;
		pBlankStatementForEndMarker->SetLineNumber(g_pStatementList->GetLineNumber());

		// Add the label
		if(pBlankStatementForEndMarker->AddInternalLabel(&pInternalLabelStringA)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'AddInternalLabel' A");
			SAFE_DELETE(pBlankStatementForEndMarker);
			SAFE_DELETE(pInternalLabelStringA);
			SAFE_DELETE(pJumpBlockChain);
			SAFE_DELETE(pInternalLabelStringB);
			SAFE_DELETE(pConditionParameter);
			SAFE_DELETE(pJumpBlockA);
			SAFE_DELETE(pJumpBlockB);
			SAFE_DELETE(TheObject);
			return false;
		}
	}

	// Create Object
	CParseJump *pJump = new CParseJump();

	// Complete Object Data
	pJump->SetType(dwJumpType);
	pJump->SetBlockA(pJumpBlockA);
	pJump->SetBlockALabel(pInternalLabelStringA);
	pJump->SetBlockB(pJumpBlockB);
	pJump->SetBlockBLabel(pInternalLabelStringB);
	pJump->SetBlockChain(pJumpBlockChain);
	pJump->SetConditionParameter(pConditionParameter);
	pJump->SetConditionCaseParameter(pConditionCaseParameter);
	pJump->SetConditionLabelParameter(pConditionLabelParameter);
	pJump->SetStartLineNumber(StatementLineNumber);
	pJump->SetMiddleLineNumber(StatementElseLineNumber);
	pJump->SetEndLineNumber(g_pStatementList->GetTokenLineNumber());

	// Add The Object To This Statement
	TheObject->m_dwObjectType = dwStatementType;
	TheObject->m_pObjectClass = (void*)pJump;
	this->Add(TheObject);

	// End marker is last
	if(pBlankStatementForEndMarker)
	{
		this->Add(pBlankStatementForEndMarker);
	}

	return true;
}

bool CStatement::DoType(DWORD StatementLineNumber, DWORD TokenID)
{
	// Create BLOCK Object for nested code
	CStatement *pTypeBlock = new CStatement();

	// Read off name of Type
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();
	LPSTR pTypeName = ProduceNextToken(&pPointer, true, false, false);
	g_pStatementList->SetFileDataPointer(pPointer);

	// leefix - 040803 - type name can be missing, cause error
	if(pTypeName==NULL)
	{
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+25, "type");
		return false;
	}

	// Deactivate ability add variables while parsing structure
	g_pStatementList->SetVariableAddParse(false);

	// Now Parse Declaration from File Data
	CDeclaration* pDecChain = NULL;
	if(pTypeBlock->DoDeclaration(false, ENDTYPETK, &pDecChain, false, true, true, true)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'pTypeBlock->Do Declaration(ENDTYPETK)'");
		SAFE_DELETE(pDecChain);
		SAFE_DELETE(pTypeBlock);
		SAFE_DELETE(pTypeName);
		return false;
	}

	// if no declaration chain declared, this type has no details
	if ( pDecChain==NULL )
	{
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+54);
		SAFE_DELETE(pDecChain);
		SAFE_DELETE(pTypeBlock);
		SAFE_DELETE(pTypeName);
		return false;
	}

	// Activate ability add variables again
	g_pStatementList->SetVariableAddParse(true);

	// Create Object
	CParseType *pType = new CParseType();

	// Add New Type To Table
	bool bReportErrorInStructureDec = false;
	if(g_pStructTable->AddStructUserType(0, pTypeName, 'U', pDecChain, pTypeBlock, 1, &bReportErrorInStructureDec)==false)
	{
		if ( bReportErrorInStructureDec ) g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+54);
		SAFE_DELETE(pTypeName);
		SAFE_DELETE(pTypeBlock);
		SAFE_DELETE(pType);
		return true;
	}
	SAFE_DELETE(pTypeName);

	// Complete Object Data
	pType->SetStartLineNumber(StatementLineNumber);
	pType->SetEndLineNumber(g_pStatementList->GetTokenLineNumber());
	pTypeBlock->SetLineAndCharPos(StatementLineNumber);
	pTypeBlock->m_dwObjectType = 2;
	pTypeBlock->m_pObjectClass = (void*)pType;
	return true;
}

bool CStatement::DoLabel(DWORD StatementLineNumber, DWORD TokenID)
{
	// Set line number
	DWORD dwCodeIndex = g_pStatementList->GetLineNumber();

	// Get filedata pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();

	// Label name
	CStr* pLabelName = GetLabel(&pPointer);
	CStr* pFullLabelName = new CStr("$label ");
	pFullLabelName->AddText(pLabelName);
	SAFE_DELETE(pLabelName);
	if(pFullLabelName->CheckChar(pFullLabelName->Length()-1,':'))
		pFullLabelName->SetChar(pFullLabelName->Length()-1,0);

	// Which pass is it (pre or real)
	DWORD dwDataIndex = g_pStatementList->GetDataIndexCounter();
	if(g_pStatementList->GetImplementationParse()==false)
	{
		// Add label
		if(g_pLabelTable->AddLabel(pFullLabelName->GetStr(), dwCodeIndex, dwDataIndex, this)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoLabel::AddLabel'");
			SAFE_DELETE(pFullLabelName);
			return false;
		}
	}
	else
	{
		// Create blank statement for label marker
		CStatement* pBlankStatement = new CStatement;
		pBlankStatement->SetLineNumber(dwCodeIndex);
		CStatement* pStatementAtLabel = FindLastStatement(); 
		pStatementAtLabel->Add(pBlankStatement);

		// Update label location information
		if(g_pLabelTable->UpdateLabel(pFullLabelName->GetStr(), dwCodeIndex, dwDataIndex, pBlankStatement)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoLabel::UpdateLabel'");
			SAFE_DELETE(pFullLabelName);
			return false;
		}
	}
	SAFE_DELETE(pFullLabelName);

	// Update filedata pointer
	g_pStatementList->SetFileDataPointer(pPointer);

	// Complete
	return true;
}

bool CStatement::DoDataStatement(DWORD StatementLineNumber, DWORD TokenID)
{
	// Get filedata pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();

	// Determine size of line
	LPSTR pEndPointer = SeekToSeperator(pPointer, false, true);
	DWORD length = pEndPointer-pPointer;

	// Make string from line
	CStr* pStrData = new CStr(length+1);
	LPSTR pPointerEnd=g_pStatementList->GetFileDataEnd();
	pStrData->CopyFromPtr(pPointer, pPointerEnd, length);
	pStrData->SetChar(length,0);

	// Clean up string
	pStrData->EatEdgeSpacesandTabs(NULL);

	// Can be initialised by a string of parameters
	CParameter* pFirstParameter = NULL;
	if(DoParameterListString(pStrData, &pFirstParameter)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoDataStatement::DoParameterListString'");
		SAFE_DELETE(pFirstParameter);
		SAFE_DELETE(pStrData);
		return false;
	}

	// Parse data in parameters to data table
	CParameter* pCurrent = pFirstParameter;
	while(pCurrent)
	{
		// Get data item from parameter
		CStr* pItemStr = pCurrent->GetMathItem()->GetResultStringToken();

		// Determine type of data item
		bool bUnderstood=false;
		DWORD dwIndex = g_pStatementList->GetDataIndexCounter() + 1;
		if(pItemStr->IsTextNumericValue()==true)
		{
			// Add Numeric to datatable
			if(g_pDataTable->AddNumeric(pItemStr->GetValue(), dwIndex))
				g_pStatementList->IncDataIndexCounter(1);
			
			bUnderstood=true;
		}
		else
		{
			// lee - 140406 - u6rc9 - support HEX, OCT and BIN
			if(pItemStr->IsTextHexValue()
			|| pItemStr->IsTextOctalValue()
			|| pItemStr->IsTextBinaryValue())
			{
				// get actual value from literal format
				DWORD dwExtraDWORD = 0;
				DWORD dwRealDWORD = pItemStr->GetDWORDRepresentation ( 7, &dwExtraDWORD );

				// Add Numeric to datatable
				if(g_pDataTable->AddNumeric((double)dwRealDWORD, dwIndex)) g_pStatementList->IncDataIndexCounter(1);
				bUnderstood=true;
			}
			else
			{
				if(pItemStr->IsTextSpeechMarked()==true)
				{
					// Add String to datatable
					CStr* pNoSpeechMarks = new CStr();
					pNoSpeechMarks->SetText(pItemStr->GetStr());
					pNoSpeechMarks->EatSpeechMarks();
					if(g_pDataTable->AddString(pNoSpeechMarks->GetStr(), dwIndex))
						g_pStatementList->IncDataIndexCounter(1);

					SAFE_DELETE(pNoSpeechMarks);
					bUnderstood=true;
				}
			}
		}
		if(bUnderstood==false)
		{
			DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+23, pItemStr->GetStr());
			SAFE_DELETE(pFirstParameter);
			SAFE_DELETE(pStrData);
			return false;
		}
		pCurrent=pCurrent->GetNext();
	}

	// Update filedata pointer
	pPointer+=length;
	g_pStatementList->SetFileDataPointer(pPointer);

	// Data items always exist on a per line basis
	g_pStatementList->IncLineNumber();

	// Free usage
	SAFE_DELETE(pFirstParameter);
	SAFE_DELETE(pStrData);

	// Complete
	return true;
}

bool CStatement::DoDeclaration(bool bVariableDeclaration, DWORD dwTerminatorType, CDeclaration** ppDecChain, bool bDoneDim, bool bAutoInitialiseData, bool bIsGlobal, bool bDefineOnly)
{
	// Get filedata pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();

	// Determine terminator state for parse
	bool bTerminatorIsCRTK=false;
	if(dwTerminatorType==CRTK) bTerminatorIsCRTK=true;

	// Get token string from filedata (if not already from a DIKTK)
	LPSTR pString = NULL;
	DWORD dwToken = 0;
	bool bMustBeLiteralDim=true;
	if(bDoneDim)
	{
		// DIM Declared at runtime can use dynamic size
		dwToken=DIMTK;
		bMustBeLiteralDim=false;
	}
	else
	{
		// leefix-220703-spaces cannot be seperators ONLY in some cases
		bool bIgnoreSpacesAroundEquateSymbol=true;

		// Get initial token
		pString = ProduceNextTokenEx(&pPointer, true, bTerminatorIsCRTK, true, bIgnoreSpacesAroundEquateSymbol);
		dwToken = DetermineToken(pString);

		// leeadd - 210604 - bypass all line remarks
		while(dwToken==REMLINETK || dwToken==CRTK)
		{
			// Ignore rest of line and find next dec string
			SkipToCR();

			// Find string from next line
			pPointer = g_pStatementList->GetFileDataPointer();
			SAFE_DELETE(pString);
			pString = ProduceNextToken(&pPointer, true, bTerminatorIsCRTK, true);
			dwToken = DetermineToken(pString);
			g_pStatementList->SetFileDataPointer(pPointer);
		}

		// Go through declaration for tokens
		if(bVariableDeclaration==true && dwToken>0 && dwToken!=DIMTK)
		{
			DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+24, pString);
			SAFE_DELETE(pString);
			return false;
		}
		g_pStatementList->SetFileDataPointer(pPointer);
	}

	// If no result from token, declaration is empty
	if(pString==NULL && dwToken==0)
	{
		// Empty Declataion (user function(void))
		return true;
	}

	// If params in declatation, parse them...
	while(dwToken!=dwTerminatorType && dwToken!=ENDTK)
	{
		// Declaration Data required
		DWORD dwDecArr=0;
		LPSTR pDecArrValue=NULL;
		LPSTR pDecName=NULL;
		LPSTR pDecType=NULL;
		LPSTR pDecInit=NULL;
		DWORD LineNumberRef=0;

		// Handle declaration token (ie DIM)
		if(dwToken==DIMTK)
		{
			// Array Name (ie MYARR(5))
			SAFE_DELETE(pString);
			LPSTR pArrayName = ProduceNextArrayToken(&pPointer);
			LPSTR pInitValue = SeperateInitFromType(pArrayName);

			// Extract array value from string
			if(SeperateValueFromArrayString(&pArrayName, &pDecArrValue, bMustBeLiteralDim)==false)
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+43);
				SAFE_DELETE(pString);
				SAFE_DELETE(pInitValue);
				SAFE_DELETE(pDecArrValue);
				SAFE_DELETE(pArrayName);
				SAFE_DELETE(pDecInit);
				return false;
			}

			dwDecArr=1;
			pDecName=pArrayName;
			pDecInit=pInitValue;
		}
		else
		{
			// Ensure more to process, else leave
			if(stricmp(pString,"")!=NULL)
			{
				// Variable Name (ie MYVAR [=x])
				LPSTR pInitValue = SeperateInitFromType(pString);
				pDecName=pString;
				pDecInit=pInitValue;
				dwDecArr=0;
			}
			else
			{
				// Leave declaration processing
				break;
			}
		}

		// lee - 270-306 - u6b5 - if type saturate with REM CRTK, skip over them
		if(dwToken!=CRTK)
		{
			// Ensure dec name is valid
			CStr* pTempNameStr = new CStr(pDecName);
			pTempNameStr->EatEdgeSpacesandTabs(NULL);
			strcpy(pDecName, pTempNameStr->GetStr() );
			if(pTempNameStr->IsTextASingleVariable()==false)
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+25, pTempNameStr->GetStr());
				SAFE_DELETE(pTempNameStr);
				SAFE_DELETE(pString);
				SAFE_DELETE(pDecArrValue);
				SAFE_DELETE(pDecInit);
				return false;
			}
			SAFE_DELETE(pTempNameStr);
		}

		// Record position of declaration line
		LineNumberRef = g_pStatementList->GetTokenLineNumber();

		// Optional datatype specifier (ie AS)
		LPSTR pTypeSpecifier = ProduceNextToken(&pPointer, true, bTerminatorIsCRTK, true);
		dwToken = DetermineToken(pTypeSpecifier);
		g_pStatementList->SetFileDataPointer(pPointer);
		if(dwToken==ASTK)
		{
			// Type (ie INTEGER)
			SAFE_DELETE(pTypeSpecifier);
			LPSTR pWhatType = ProduceNextToken(&pPointer, true, bTerminatorIsCRTK, true);
			dwToken = DetermineToken(pWhatType);
			g_pStatementList->SetFileDataPointer(pPointer);
			if(dwToken==DOUBLETK)
			{
				// Pre-Declaration Token (ie DOUBLE INTEGER)
				LPSTR pWhatTypeMore = ProduceNextToken(&pPointer, true, bTerminatorIsCRTK, true);
				dwToken = DetermineToken(pWhatTypeMore);
				g_pStatementList->SetFileDataPointer(pPointer);
				CStr* pAddTypes = new CStr(pWhatType);
				pAddTypes->AddText(" ");
				pAddTypes->AddText(pWhatTypeMore);
				LPSTR pNewTypeString = new char[pAddTypes->Length()+1];
				strcpy(pNewTypeString,pAddTypes->GetStr());
				SAFE_DELETE(pWhatTypeMore);
				SAFE_DELETE(pWhatType);
				pWhatType=pNewTypeString;
				SAFE_DELETE(pAddTypes);
			}
			else
			{
				// Notmal Declaration Type
			}

			// Do we have an initialisation value
			LPSTR pInitValue = SeperateInitFromType(pWhatType);
			if(pInitValue)
			{
				// Backtrack pointer to exclude '=' symbol (see next code)
				int iBacktrak=0;
				pPointer = g_pStatementList->GetFileDataPointer();
				if(SeekCharAsPrevChar('=', &iBacktrak)==true)
				{
					// Set new position before the '=' symbol
					g_pStatementList->SetFileDataPointer(pPointer+iBacktrak);
				}
				SAFE_DELETE(pInitValue);
			}

			// leefix - 250604 - u54 - no Init if seperator located as next character
			pPointer = g_pStatementList->GetFileDataPointer();
			while ( *pPointer==' ' && pPointer<g_pStatementList->GetFileDataEnd()-2 )
				pPointer++;

			// Check for '=' symbol trailing type specifier
			if ( *pPointer=='=' )
			{
				DWORD dwAdvance=0;
				if(SeekCharAsNextChar('=', &dwAdvance)==true)
				{
					// Remainder of line after '=' symbol is init data
					pPointer = g_pStatementList->GetFileDataPointer()+1;
					g_pStatementList->SetFileDataPointer(pPointer+dwAdvance);
					pPointer = g_pStatementList->GetFileDataPointer();
					pInitValue = GetStringToEndOfLine();

					// leeadd - 310305 - handle seperator after delcarations
					// lee - 270306 - u6b5 - also handle speech marks to exclude seperator ':'
					DWORD dwSpeechMark=0;
					LPSTR pMarkSep = pInitValue;
					bool bIfFoundColonSep=false;
					for ( DWORD c=0; c<strlen(pInitValue); c++ )
					{
						if ( pInitValue [ c ]=='"' ) dwSpeechMark=1-dwSpeechMark;
						if ( pInitValue [ c ]==':' && dwSpeechMark==0 )
						{
							// found seperator within initvalue string
							bIfFoundColonSep=true;
							pMarkSep = pInitValue + c;

							// lee - 130206 - u60 - before we leave, we must ensure the pointer carries on from
							// the seperator, NOT from the end of the grabbed initvalue string
							g_pStatementList->SetFileDataPointer ( (pPointer+c) );

							// break out of further scan for seperator
							break;
						}
					}
					if ( bIfFoundColonSep ) pInitValue [ pMarkSep-pInitValue ] = 0;
					pPointer = g_pStatementList->GetFileDataPointer();
					if(*(unsigned char*)(pPointer-1)==13) pPointer--;
				}
			}

			// Record type and init data
			pDecType=pWhatType;
			pDecInit=pInitValue;

			// Next Item Of Data - until no more (token may produce CRTK)
			pString = ProduceNextToken(&pPointer, true, bTerminatorIsCRTK, true);
			dwToken = DetermineToken(pString);

			// If reached end of program duing dec parse, leave gracefully
			if(pPointer>=g_pStatementList->GetFileDataEnd()-2)
			{
				// leefix - 200703 - and make sure we skip past endtypetk string data
				g_pStatementList->SetFileDataPointer(g_pStatementList->GetFileDataEnd());
				dwToken=dwTerminatorType;
			}
			else
				g_pStatementList->SetFileDataPointer(pPointer);
		}
		else
		{
			// LEEFIX-REMARK HANDLER WAS HERE-040803
			if(dwToken==REMLINETK)
			{
				// done later on in code
			}
			else
			{
				// Actually the Next Item, so rename it
				pString = pTypeSpecifier;
			}
		}

		// leefix-040803-handle a comment after type declaration line
		if ( dwTerminatorType==ENDTYPETK )
		{
			// LEEFIX-REMARK HANDLER MOVED HERE TO HANDLE BOTH (field `comment and field as integer `comment)
			if(dwToken==CRTK)
			{
				// Next Item Of Data, either next type field or remark
				pString = ProduceNextToken(&pPointer, true, bTerminatorIsCRTK, true);
				dwToken = DetermineToken(pString);
			}
		}

		// handle remarks by skipping line
		if(dwToken==REMLINETK)
		{
			// Bypass all line remarks (can be muiltiple lines (210604 - added dwToken==CRTK)
			LPSTR pNextString = NULL;
			while(dwToken==REMLINETK || dwToken==CRTK)
			{
				// Ignore rest of line and find next dec string
				SkipToCR();

				// Find string from next line
				pPointer = g_pStatementList->GetFileDataPointer();
				pNextString = ProduceNextToken(&pPointer, true, bTerminatorIsCRTK, true);
				dwToken = DetermineToken(pNextString);
				g_pStatementList->SetFileDataPointer(pPointer);
			}

			// Non remark string..continue.
			pString = pNextString;
		}

		// Default declaration settings
		if(pDecType==NULL)
		{
			// Use default type
			pDecType = g_pVarTable->MakeDefaultVarType(pDecName);
		}
		if(pDecInit==NULL && bAutoInitialiseData==true)
		{
			// Use default init
			pDecInit = new char[8];
			strcpy(pDecInit,"0");
		}

		// leefix - 230604 - u54 - If using DIM and no init value, means DIM arr(), so use -1 to make an EMPTY array!
		if ( pDecArrValue )
		{
			if ( strcmp( pDecArrValue, "")==NULL )
			{
				SAFE_DELETE(pDecArrValue);
				pDecArrValue = new char[8];
				strcpy(pDecArrValue,"-1");
			}
		}

		// Add declaration only for global types (user function uses add variable..)
		CDeclaration* pGlobalDecChain = g_pStatementList->GetUserFunctionDecChain();
		if(pGlobalDecChain==NULL)
		{
			// For Array Variables Only
			CStr* pArrFullname = NULL;
			if(dwDecArr==1)
			{
				if(bDefineOnly==true)
				{
					DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
					g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+26);
					SAFE_DELETE(pDecArrValue);
					SAFE_DELETE(pDecName);
					SAFE_DELETE(pDecType);
					SAFE_DELETE(pDecInit);
					SAFE_DELETE(pString);
					SAFE_DELETE(pDecArrValue);
					SAFE_DELETE(pArrFullname);
					return false;
				}

				// Array name has prefix
				pArrFullname = new CStr("&");
				pArrFullname->AddText(pDecName);
			}
			else
				pArrFullname = new CStr(pDecName);

			// Create Declaration
			CDeclaration* pNewDec = new CDeclaration;
			pNewDec->SetDecData(dwDecArr, pDecArrValue, pArrFullname->GetStr(), pDecType, pDecInit, LineNumberRef);
			SAFE_DELETE(pArrFullname);

			// Add to chain GLOBAL
			if((*ppDecChain)==NULL)
				*ppDecChain=pNewDec;
			else
				(*ppDecChain)->Add(pNewDec);
		}

		// Do not generate init code when prescanning outside a function
		bool bGenerateImpCode=true;
		if(g_pStatementList->GetImplementationParse()==false)
		{
			// LEEFIX - 290703 - There should be no implementation in a pre-scan
			// as DIM LEE(x,y,z) would mean X,Y,Z would be declared as local (even if they are global!)
			// if(pGlobalDecChain==NULL)
			bGenerateImpCode=false;
		}

		// Create Variable Entry for table (not for variable declarations in typedefs)
		if(bVariableDeclaration==true || pGlobalDecChain )
		{
			// For Array vars
			CStr* pArrFullname = NULL;
			if(dwDecArr==1)
			{
				pArrFullname = new CStr("&");
				pArrFullname->AddText(pDecName);
			}
			else
				pArrFullname = new CStr(pDecName);

			// lee - 270206 - u60 - if local, ensure we have not already declared same name var
			/* lee - 200306 - u6b4 - however, DIM, UNDIM and DIM would fail - have to remove this fix
			if ( g_pStatementList->GetImplementationParse()==false )
			{
				bool bDuplicatedVarName = false;
				if ( g_pVarTable->FindVariable ( g_pStatementList->GetUserFunctionName(), pArrFullname->GetStr(), dwDecArr )!=NULL ) bDuplicatedVarName = true;
				if ( bDuplicatedVarName==true )
				{
					DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
					g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+36, pArrFullname->GetStr());
					SAFE_DELETE(pDecArrValue);
					SAFE_DELETE(pDecName);
					SAFE_DELETE(pDecType);
					SAFE_DELETE(pDecInit);
					SAFE_DELETE(pString);
					SAFE_DELETE(pDecArrValue);
					SAFE_DELETE(pArrFullname);
					return false;
				}
			}
			*/

			// Admin adding var to table
			DWORD dwAction=0;
			if(g_pVarTable->AddVariable(pArrFullname->GetStr(), pDecType, dwDecArr, LineNumberRef, false, &dwAction, bIsGlobal)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoDeclaration::Add Variable'");
				SAFE_DELETE(pDecArrValue);
				SAFE_DELETE(pDecName);
				SAFE_DELETE(pDecType);
				SAFE_DELETE(pDecInit);
				SAFE_DELETE(pString);
				SAFE_DELETE(pDecArrValue);
				SAFE_DELETE(pArrFullname);
				return false;
			}
			SAFE_DELETE(pArrFullname);

			// If variable is also array, dynamically create
			if(dwDecArr==1 && bGenerateImpCode)
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				if(DoAllocation(StatementLineNumber, pDecName, pDecArrValue)==false)
				{
					g_pErrorReport->AddErrorString("Failed to 'DoDeclaration::DoAllocation'");
					SAFE_DELETE(pDecArrValue);
					SAFE_DELETE(pDecName);
					SAFE_DELETE(pDecType);
					SAFE_DELETE(pDecInit);
					SAFE_DELETE(pString);
					SAFE_DELETE(pDecArrValue);
					return false;
				}
			}
		}

		// If Init Code present, add to code part, only if not typedef
		if(pDecInit && bGenerateImpCode && bDefineOnly==false)
		{
			// Create New Init Object
			CParseInit* pInit = new CParseInit;
			DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
			pInit->SetLineNumber(StatementLineNumber);

			// Math Name is actual (as it can be userfunction local var)
			CMathOp* pMathOp = new CMathOp;

			// Var name
			CStr* pVarInitName = NULL;
			if(dwDecArr==1)
			{
				pVarInitName = new CStr("&");
				pVarInitName->AddText(pDecName);
			}
			else
				pVarInitName = new CStr(pDecName);

			if(pMathOp->DoValue(pVarInitName)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoDeclaration::DoValue(pVarInitName)'");
				SAFE_DELETE(pInit);
				SAFE_DELETE(pMathOp);
				SAFE_DELETE(pVarInitName);
				SAFE_DELETE(pDecArrValue);
				SAFE_DELETE(pDecName);
				SAFE_DELETE(pDecType);
				SAFE_DELETE(pDecInit);
				SAFE_DELETE(pString);
				SAFE_DELETE(pDecArrValue);
				return false;
			}
			pInit->SetVariableNameMathOp(pMathOp);
			SAFE_DELETE(pVarInitName);
			
			// Can be initialised by a string of parameters
			CStatement* pStatement = new CStatement;
			CParameter* pFirstParameter = NULL;
			CStr* pVarInitData = new CStr(pDecInit);
			if(pVarInitData->Length()>0)
			{
				if(pStatement->DoParameterListString(pVarInitData, &pFirstParameter)==false)
				{
					g_pErrorReport->AddErrorString("Failed to 'DoDeclaration::DoParameterListString'");
					SAFE_DELETE(pVarInitData);
					SAFE_DELETE(pFirstParameter);
					SAFE_DELETE(pStatement);
					SAFE_DELETE(pInit);
//					SAFE_DELETE(pMathOp); crashed
					SAFE_DELETE(pVarInitName);
					SAFE_DELETE(pDecArrValue);
					SAFE_DELETE(pDecName);
					SAFE_DELETE(pDecType);
					SAFE_DELETE(pDecInit);
					SAFE_DELETE(pString);
					SAFE_DELETE(pDecArrValue);
					return false;
				}
			}
			SAFE_DELETE(pStatement);
			SAFE_DELETE(pVarInitData);

			// Assign param chain to init object
			pInit->SetVariableParamList(pFirstParameter);

			// Ensure all init data items are compatable with var type
			DWORD dwVarType=g_pVarTable->GetBasicTypeValue(pDecType);
			CParameter* pCurrent = pFirstParameter;
			while(pCurrent)
			{
				bool bDataItemTypeIsOk=false;
// leefix - 240604 - u54 - this did not work for arrays
//				DWORD dwItemType = pCurrent->GetMathItem()->GetResultType();
//				LPSTR pItemString = pCurrent->GetMathItem()->GetResultStringToken()->GetStr();
				DWORD dwItemType = pCurrent->GetMathItem()->FindResultTypeValueForDBM();
				LPSTR pItemString = pCurrent->GetMathItem()->FindResultStringTokenForDBM()->GetStr();
				if ( strcmp(pItemString,"0")==NULL || strcmp(pItemString,"")==NULL )
				{
					// Zero inits all numbers
//					if(dwVarType==3) strcpy(pItemString," ");//hack!! - improved the hack on 250604 -54
					if(dwVarType==3)
					{
						pCurrent->GetMathItem()->FindResultStringTokenForDBM()->SetText("0");
						pItemString = pCurrent->GetMathItem()->FindResultStringTokenForDBM()->GetStr();
					}
					bDataItemTypeIsOk=true;
				}
				else
				{
					if(dwVarType==2 || dwVarType==8)
					{
						// Float
						if(dwItemType==2 || dwItemType==8) bDataItemTypeIsOk=true;
					}
					else if(dwVarType==3)
					{
						// String
						if(dwItemType==3) bDataItemTypeIsOk=true;
					}
					else
					{
						// Integer
						if(dwItemType!=2 && dwItemType!=3 && dwItemType!=8)
							bDataItemTypeIsOk=true;
					}
				}
				if(bDataItemTypeIsOk)
				{
					// Even if correct category, init data must be forced to type of variable
					CStr* pStr = new CStr(pItemString);
					pCurrent->GetMathItem()->SetResult(pStr->GetStr(), dwVarType, 0);
					SAFE_DELETE(pStr);
				}
				else
				{
					DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
					g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+27, pItemString);
					SAFE_DELETE(pVarInitData);
					SAFE_DELETE(pInit);
					SAFE_DELETE(pVarInitName);
					SAFE_DELETE(pDecArrValue);
					SAFE_DELETE(pDecName);
					SAFE_DELETE(pDecType);
					SAFE_DELETE(pDecInit);
					SAFE_DELETE(pString);
					SAFE_DELETE(pDecArrValue);
					return false;
				}
				pCurrent=pCurrent->GetNext();
			}

			// Add The Object To This Statement
			CStatement *TheObject = new CStatement();
			TheObject->m_dwObjectType = 3;
			TheObject->m_pObjectClass = (void*)pInit;
			TheObject->SetLineAndCharPos(StatementLineNumber);
			this->Add(TheObject);
		}

		// Release memory usage
		SAFE_DELETE(pDecArrValue);
		SAFE_DELETE(pDecName);
		SAFE_DELETE(pDecType);
		SAFE_DELETE(pDecInit);
	}

	// leefix - 240604 - u54 - END in TYPE struct, invalid!
	if ( dwToken==ENDTK )
	{
		// and not end of program
		if(pPointer<g_pStatementList->GetFileDataEnd()-2)
		{
			DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+61);
			SAFE_DELETE(pString);
			return false;
		}
	}

	// Free memory usage
	SAFE_DELETE(pString);

	// lee - 270206 - u60 - Check for ENDTYPE
	if ( dwTerminatorType==ENDTYPETK && dwToken!=ENDTYPETK )
	{
		// TYPE / ENDTYPE needs correct terminator - end found in type
		g_pErrorReport->SetError(g_pStatementList->GetTokenLineNumber(), ERR_SYNTAX+61);
		return false;
	}

	// Complete
	return true;
}

bool CStatement::DoInstruction(DWORD StatementLineNumber, DWORD TokenID)
{
	// This is INSTRUCTION Type 11
	DWORD dwStatementType=11;

	// Parameter Chain
	CParameter* pFirstParameter = NULL;
	CStr* pFullLabelName = NULL;

	// Get Details of instruction
	bool bOneParamPerRepeatedInstruction=false;
	CInstructionTableEntry* pRef = g_pStatementList->GetInstructionRef();
	DWORD dwInstructionType = g_pStatementList->GetInstructionType();
	DWORD dwInstructionValue = g_pStatementList->GetInstructionValue();
	DWORD dwInstructionParamMax = g_pStatementList->GetInstructionParamMax();

	// Record line after instruciton/before params
	StatementLineNumber = g_pStatementList->GetLineNumber();

	// Create Parameter Object
	DWORD dwParamCount=0;
	bool bMoreParams=true;
	while(bMoreParams==true)
	{
		CParameter* pReturnParameter = NULL;
		if(DoExpressionList(&pReturnParameter,&bMoreParams)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoInstruction::Do ExpressionList'");
			SAFE_DELETE(pReturnParameter);
			SAFE_DELETE(pFirstParameter);
			return false;
		}

		// Param Valid
		if(pReturnParameter)
		{
			// Param created
			dwParamCount++;

			// Add Param to Parameter Chain
			if(pFirstParameter==NULL)
				pFirstParameter=pReturnParameter;
			else
				pFirstParameter->Add(pReturnParameter);
		}
		else
			break;
	}

	// If Assignment Instruction, further refine instruction to use
	bool bMustNotCastIntoAssignment=false;
	DWORD dwValidInstructionToUse=0;
	DWORD dwValidInstructionValue=0, dwValidParamMax=0;
	if(dwInstructionType==2 && dwInstructionValue==g_pInstructionTable->GetIIValue(IT_INTERNAL_ASSIGNLL))
	{
		// If too many parameters than instruction requires
		if ( dwParamCount > dwInstructionParamMax )
		{
			// just too many parameters for this instruction
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+29, "=");
			SAFE_DELETE(pFirstParameter);
			return false;
		}

		// First parameter must be L-value
		CStr* pLValue = pFirstParameter->GetMathItem()->FindResultStringTokenForDBM();
		DWORD dwLValueTypeValue=pFirstParameter->GetMathItem()->FindResultTypeValueForDBM();
		if(pLValue==NULL)
		{
			g_pErrorReport->AddErrorString("Failed to DoInstruction::'pLValue==NULL'");
			SAFE_DELETE(pFirstParameter);
			return false;
		}

		// Check if LValue is literal
		if(pLValue->IsTextNumericValue()==true)
		{
			// Tried to do something like 20 = lee(10)
			dwLValueTypeValue=0;
		}

		if(dwLValueTypeValue==0)
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+28);
			SAFE_DELETE(pFirstParameter);
			return false;
		}

		// Select correct assignment instruction for L-value type
		DWORD dwUseNewInstruction=0;
		switch(dwLValueTypeValue)
		{
			case 1 : dwUseNewInstruction=IT_INTERNAL_ASSIGNLL;			break;
			case 2 : dwUseNewInstruction=IT_INTERNAL_ASSIGNFF;			break;
			case 3 : dwUseNewInstruction=IT_INTERNAL_ASSIGNSS;			break;
			case 4 : dwUseNewInstruction=IT_INTERNAL_ASSIGNBB;			break;
			case 5 : dwUseNewInstruction=IT_INTERNAL_ASSIGNYY;			break;
			case 6 : dwUseNewInstruction=IT_INTERNAL_ASSIGNWW;			break;
			case 7 : dwUseNewInstruction=IT_INTERNAL_ASSIGNDD;			break;
			case 8 : dwUseNewInstruction=IT_INTERNAL_ASSIGNOO;			break;
			case 9 : dwUseNewInstruction=IT_INTERNAL_ASSIGNRR;			break;

			case 101 : dwUseNewInstruction=IT_INTERNAL_ASSIGNLL;			break;
			case 102 : dwUseNewInstruction=IT_INTERNAL_ASSIGNFF;			break;
			case 103 : dwUseNewInstruction=IT_INTERNAL_ASSIGNSS;			break;
			case 104 : dwUseNewInstruction=IT_INTERNAL_ASSIGNBB;			break;
			case 105 : dwUseNewInstruction=IT_INTERNAL_ASSIGNYY;			break;
			case 106 : dwUseNewInstruction=IT_INTERNAL_ASSIGNWW;			break;
			case 107 : dwUseNewInstruction=IT_INTERNAL_ASSIGNDD;			break;
			case 108 : dwUseNewInstruction=IT_INTERNAL_ASSIGNOO;			break;
			case 109 : dwUseNewInstruction=IT_INTERNAL_ASSIGNRR;			break;

			case 201 : dwUseNewInstruction=IT_INTERNAL_ASSIGNLL;			break;
			case 202 : dwUseNewInstruction=IT_INTERNAL_ASSIGNFF;			break;
			case 203 : dwUseNewInstruction=IT_INTERNAL_ASSIGNSS;			break;
			case 204 : dwUseNewInstruction=IT_INTERNAL_ASSIGNBB;			break;
			case 205 : dwUseNewInstruction=IT_INTERNAL_ASSIGNYY;			break;
			case 206 : dwUseNewInstruction=IT_INTERNAL_ASSIGNWW;			break;
			case 207 : dwUseNewInstruction=IT_INTERNAL_ASSIGNDD;			break;
			case 208 : dwUseNewInstruction=IT_INTERNAL_ASSIGNOO;			break;
			case 209 : dwUseNewInstruction=IT_INTERNAL_ASSIGNRR;			break;

			case 1001 : dwUseNewInstruction=IT_INTERNAL_ASSIGNUDT;			break;
			case 1101 : dwUseNewInstruction=IT_INTERNAL_ASSIGNUDT;			break;
		}
		if(dwUseNewInstruction==0)
		{
			// Not implemented yet!
			g_pErrorReport->AddErrorString("Failed to DoInstruction::'dwUseNewInstruction==0'");
			SAFE_DELETE(pFirstParameter);
			return false;
		}

		dwInstructionValue=g_pInstructionTable->GetIIValue(dwUseNewInstruction);
		pRef=g_pInstructionTable->GetRef(dwUseNewInstruction);
		if(pRef==NULL)
		{
			// Not implemented yet!
			g_pErrorReport->AddErrorString("Failed to DoInstruction::'pRef==NULL'");
			SAFE_DELETE(pFirstParameter);
			return false;
		}
		dwValidInstructionToUse=dwInstructionValue;

		// Ensure assign has an R-value to provide secondary value
		if(pFirstParameter->GetNext()==NULL)
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+42);
			SAFE_DELETE(pFirstParameter);
			return false;
		}

		// Ensure second param is identical type to first if UserDefinedType Assignment used
		if(dwUseNewInstruction==IT_INTERNAL_ASSIGNUDT)
		{
			// same-flag - leefix - 170403 - new way to check for type names ( lee, lee.fred lee(5), lee(5).fred, etc)
			bool bTypesSame=false;
			CStr* pLocalLeftTypeName = new CStr("");
			CStr* pLocalRightTypeName = new CStr("");
			CStr* pLeftTypeName = NULL;
			CStr* pRightTypeName = NULL;

			// left value
			DWORD dwLArrayType=0;
			CResultData* pLResult = pFirstParameter->GetMathItem()->FindResultDataForDBM();
			if ( pLResult->m_pStringToken->CheckChar(1,'&') ) dwLArrayType=1;
			if ( pLResult->m_pStruct )
				pLeftTypeName = pLResult->m_pStruct->GetTypeName();

			// right value
			DWORD dwRArrayType=0;
			CResultData* pRResult = pFirstParameter->GetNext()->GetMathItem()->FindResultDataForDBM();
			if ( pRResult->m_pStringToken->CheckChar(1,'&') ) dwRArrayType=1;
			if ( pRResult->m_pStruct )
				pRightTypeName = pRResult->m_pStruct->GetTypeName();

			// Check two type descriptions
			if(pLeftTypeName && pRightTypeName)
				if(stricmp(pLeftTypeName->GetStr(), pRightTypeName->GetStr())==NULL)
					bTypesSame=true;

			// LEEFIX - 171002 - If dealing with UDT only - allow others to carry on
			if ( pLeftTypeName )
			{
				if(pLeftTypeName->GetStr())
				{
					// UDT assigns have an extra parameter to store type size
					DWORD dwSize = g_pStructTable->GetSizeOfType(pLeftTypeName->GetStr());
					CMathOp* pSizeMathOp = new CMathOp;
					CStr* pSizeString = new CStr("");
					pSizeString->SetNumericText(dwSize);
					pSizeMathOp->SetResult(pSizeString->GetStr(), 7, 0);
					SAFE_DELETE(pSizeString);

					// Copy temp var into input-param (cast maybe required)
					CParameter* pSizeParam = new CParameter;
					pSizeParam->SetMathItem(pSizeMathOp);
					pFirstParameter->Add(pSizeParam);

					// Error if not same
					if(bTypesSame==false)
					{
						g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+47);
						SAFE_DELETE(pFirstParameter);
						SAFE_DELETE(pLocalLeftTypeName);
						SAFE_DELETE(pLocalRightTypeName);
						return false;
					}
				}
			}

			// Free usages
			SAFE_DELETE(pLocalLeftTypeName);
			SAFE_DELETE(pLocalRightTypeName);
		}

		// If Assign instruction uses indirect(*), no casting!
		if(pFirstParameter->GetMathItem()->GetResultType()>=201
		&& pFirstParameter->GetMathItem()->GetResultType()<=299)
		{
			bMustNotCastIntoAssignment=true;
		}
	}
	else
	{
		// Scan Each Matching Instruction and find match with parameters used, else error
		if(FindCorrectInstruction(&pRef, pFirstParameter, dwInstructionValue, dwInstructionType, dwInstructionParamMax, &dwValidInstructionToUse, &bOneParamPerRepeatedInstruction)==false)
		{
			dwValidInstructionToUse=0;
			DB3_CRASH();
		}
		if(pRef)
		{
			// Some instructions only parse in single param items
			if(stricmp(pRef->GetName()->GetStr(),"INPUT")==NULL)
			{
				dwValidInstructionToUse=pRef->GetInternalID();
				bOneParamPerRepeatedInstruction=true;
			}
		}
	}

	// Erro or success
	if(dwValidInstructionToUse>0)
	{
		// Use This Instruction (values retained as left immediately)
		dwValidInstructionValue=dwValidInstructionToUse;
		dwValidParamMax=dwInstructionParamMax;
	}
	else
	{
		// Parameter-mismatch
		if(pRef)
		{
			CStr* pParamDesc=pRef->GetFullParamDesc();
			if(pParamDesc)
			{
				// lee - 220306 - u6b4 - detect and report requirement of a label (rather than ambiguous string)
				if ( pRef->GetParamTypes()->GetChar(0)=='Q' )
					g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+18, pFirstParameter->GetMathItem()->FindResultStringTokenForDBM()->GetStr());
				else
					g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+8, pRef->GetName()->GetStr(), pParamDesc->GetStr());
			}
			else
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+29, pRef->GetName()->GetStr());
		}
		else
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+42);
		}

		SAFE_DELETE(pFirstParameter);
		return false;
	}

	// REGULAR INSTRUCTION
	if(dwInstructionType==2)
	{
		// If RESTORE (requiring data label) instruction, change label
		if(pRef->GetParamTypes()->CheckChar(0, 'Q'))
		{
			if(pFirstParameter)
			{
				// Get label name
				CStr* pTemp = new CStr(pFirstParameter->GetMathItem()->GetResultStringToken()->GetStr());

				// LEEFIX - 141102 - Ensure any localisation of label is stripped out
				if(pTemp->CheckChars(0, 3, "FS@"))
				{
					// Strip out FS premarker
					LPSTR lpStr = pTemp->GetStr();
					for(DWORD i=strlen(lpStr); i>0; i--)
					{
						if(lpStr[i]=='@')
						{
							pTemp->SetText(lpStr+i+1);
							break;
						}
					}
				}

				// Set param with full label string
				pFirstParameter->GetMathItem()->GetResultStringToken()->SetText("$dabel ");
				pFirstParameter->GetMathItem()->GetResultStringToken()->AddText(pTemp);
				SAFE_DELETE(pTemp);
			}
		}
	}

	// If User Function, create label
	if(dwInstructionType==3)
	{
		// Construct full label name for function
		pFullLabelName = new CStr("$label ");
		pFullLabelName->AddText(pRef->GetName()->GetStr());
	}

	// If instruction is not typeA seperated, check casting
	CStr* pReturnParameterToken=NULL;
	CStatement* pInputAsOutputAssignment = NULL;
	if(pRef && bOneParamPerRepeatedInstruction==false)
	{
		if(pFirstParameter && bMustNotCastIntoAssignment==false)
		{
			// Instruction by default treats input as input params, cast input (print a)
			if(pFirstParameter->CastAllParametersToInstruction(pRef)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoInstruction::CastAllParametersToInstruction'");
				SAFE_DELETE(pFirstParameter);
				return false;
			}
		}
	}

	// Make possibly several instructions
	DWORD dwResumeNonConcatInstruction=0;
	int iMultipleInstructions=1;

	// Special case where only param in command has a concat (print "lee";)
	if(pFirstParameter)
		if(pFirstParameter->GetMathItem())
			if(pFirstParameter->GetMathItem()->GetConcatFlag()==true)
				bOneParamPerRepeatedInstruction=true;

	// If parameters are one-per-instruction (type A like READ and PRINT), do so here
	if ( pFirstParameter && bOneParamPerRepeatedInstruction==true )
	{
		// Number of commands to generate
		iMultipleInstructions=(int)pFirstParameter->Count();

		// Determine which commands are concats
		DWORD dwConcatMode=0;
		if(stricmp(pRef->GetName()->GetStr(),"PRINT")==NULL) dwConcatMode=1;
		if(stricmp(pRef->GetName()->GetStr(),"INPUT")==NULL) dwConcatMode=2;
		if(stricmp(pRef->GetName()->GetStr(),"READ")==NULL) dwConcatMode=3;

		// Some multi-commands require CONCAT instruction (ie printc)
		if(dwConcatMode>0)
		{
			DWORD dwTokenData=0, dwParamMax=0, dwLength=0;
			if(dwConcatMode==1 || dwConcatMode==2) // PRINT and INPUT (only last is INPUT)
			{
				if(g_pInstructionTable->FindInstruction(false, "PRINTC", 1, &dwTokenData, &dwParamMax, &dwLength, &pRef))
				{
					// Only resume if PRINT does not use CONCAT at end (;)
					if(pFirstParameter->GetMathItem()->GetConcatFlag()==false)
					{
						// Will NOT concatenate last instruction in multi-instructions
						dwResumeNonConcatInstruction=dwInstructionValue;
					}

					dwInstructionValue=pRef->GetInternalID();
					dwValidInstructionToUse=dwInstructionValue;
				}
			}
			if(dwConcatMode==3) // READ (all items are READ)
			{
				if(g_pInstructionTable->FindInstruction(false, "READ", 1, &dwTokenData, &dwParamMax, &dwLength, &pRef))
				{
					dwInstructionValue=pRef->GetInternalID();
					dwValidInstructionToUse=dwInstructionValue;
				}
			}
		}
	}

	// Create instructions
	CParameter* pUseParameter=pFirstParameter;
	CParameter* pCurrentParameter=pFirstParameter;
	while(iMultipleInstructions)
	{
		// During Last Instruction in multi-instruction, restore if concat used
		if(iMultipleInstructions==1 && dwResumeNonConcatInstruction>0)
		{
			dwInstructionValue=dwResumeNonConcatInstruction;
			dwResumeNonConcatInstruction=0;
		}

		// Create Object
		DWORD dwCorrectInstructionValue=dwValidInstructionValue;
		CParseInstruction *pInstruction = new CParseInstruction();

		// Determine parameter to use
		if(bOneParamPerRepeatedInstruction==true)
		{
			// Create param from one of the chain params
			CParameter* pNewTempParam = new CParameter;
			if ( pCurrentParameter )
			{
				pNewTempParam->SetMathItem(pCurrentParameter->GetMathItem());
				pCurrentParameter->SetMathItem(NULL);
				pUseParameter=pNewTempParam;
			}

			// Determine the correct instruction for the param string submitted
			DWORD dwValidParamMaxToUse=0;
			if(FindCorrectInstruction(&pRef, pNewTempParam, dwInstructionValue, dwInstructionType, dwInstructionParamMax, &dwCorrectInstructionValue, NULL)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoInstruction::FindCorrectInstruction'");
				SAFE_DELETE(pNewTempParam);
				SAFE_DELETE(pFirstParameter);
				return false;
			}

			// Ensure params marked as output params (ie INPUT, READ) are cast ok..
			if(pRef->GetReturnParam()>=11 && pRef->GetReturnParam()<=19)
			{
				// Instruction treats input as output params, cast output (input a)
				unsigned char cSourceChar = pRef->GetParamTypes()->GetChar(0);
				DWORD dwSourceTypeValue = g_pVarTable->GetTypeValueOfChar(cSourceChar);
// LEEFIX - 121002 - Sometimes arrays are used as params, and have a deep nest
//				DWORD dwRequiredTypeValue = pUseParameter->GetMathItem()->GetResultType();
				DWORD dwRequiredTypeValue = pUseParameter->GetMathItem()->FindResultTypeValueForDBM();

				if(dwSourceTypeValue==3 || dwSourceTypeValue==dwRequiredTypeValue)
				{
					// String or same-type result does not need casting
// LEEFIX - 121002 - Sometimes arrays are used as params, and have a deep nest
//					pReturnParameterToken = new CStr(pUseParameter->GetMathItem()->GetResultStringToken()->GetStr());
					pReturnParameterToken = new CStr(pUseParameter->GetMathItem()->FindResultStringTokenForDBM()->GetStr());
				}
				else
				{
					// Return into temp var of function return type
					pReturnParameterToken = new CStr("");
					CStr TempVarToken(1);
					pUseParameter->GetMathItem()->ProduceNewTempToken(&TempVarToken, dwSourceTypeValue);
					pReturnParameterToken->SetText("");
					pReturnParameterToken->AddText(&TempVarToken);

					// Create Math Op to hold source variable
					CMathOp* pMathOp = new CMathOp;
					CStr* pSourceString = new CStr(pReturnParameterToken->GetStr());
					pMathOp->SetResult(pSourceString->GetStr(), dwSourceTypeValue, 0);
					SAFE_DELETE(pSourceString);

					// leefix-050803-I would assume any cast would never to be an array (as there are no temp arrays)
					if ( dwRequiredTypeValue>100 && dwRequiredTypeValue<200 )
						dwRequiredTypeValue-=100;

					// Copy temp var into input-param (cast maybe required)
					pInputAsOutputAssignment = new CStatement;
					CParameter* pInputAsOutputParam = new CParameter;
					if(pMathOp->DoCastOnMathOp(&pMathOp, dwRequiredTypeValue)==false)
					{
						g_pErrorReport->AddErrorString("Failed to 'DoInstruction::DoCastOnMathOp'");
						SAFE_DELETE(pMathOp);
						SAFE_DELETE(pReturnParameterToken);
						SAFE_DELETE(pInputAsOutputParam);
						SAFE_DELETE(pInputAsOutputAssignment);
						SAFE_DELETE(pFirstParameter);
						return false;
					}

					// Assign mathop (with possible cast) to optional assignment param
// LEEFIX - 141102 - When cutting up read into array, array accesses are deep and need the FIND function
//					CStr* pFinalVar = new CStr(pUseParameter->GetMathItem()->GetResultStringToken()->GetStr());
// LEEFIX - 060803 - Only copies result string which means array and type info ignored, so..
//					CStr* pFinalVar = new CStr(pUseParameter->GetMathItem()->FindResultStringTokenForDBM()->GetStr());
//					pMathOp->GetResultStringToken()->SetText(pFinalVar->GetStr());
					CResultData* pFinalVar = pUseParameter->GetMathItem()->FindResultDataForDBM();
					if ( pFinalVar ) pMathOp->SetResultData(*pFinalVar);
					pInputAsOutputAssignment->SetParameter(pInputAsOutputParam);
					pInputAsOutputParam->SetMathItem(pMathOp);
				}
			}
			else
			{
				if(bMustNotCastIntoAssignment==false)
				{
					if(pUseParameter->CastAllParametersToInstruction(pRef)==false)
					{
						g_pErrorReport->AddErrorString("Failed to 'DoInstruction::CastAllParametersToInstruction'");
						SAFE_DELETE(pNewTempParam);
						SAFE_DELETE(pFirstParameter);
						return false;
					}
				}
			}

			// Go to next in chain for next time
			if ( pCurrentParameter ) pCurrentParameter=pCurrentParameter->GetNext();
		}
		else
		{
			// Not a multi-instruction command, so normal..
			dwCorrectInstructionValue=dwValidInstructionValue;
			pUseParameter=pFirstParameter;

			// If instruction shows a return type (and is not multi-instruction like INPUT, READ), setup a return param string
			if(pRef->GetReturnParam()>=11 && pRef->GetReturnParam()<=19)
			{
				// A command like READ BYTE 1,A would return a var
				int iWhichParamIndex = (int)pRef->GetReturnParamPlace();
				CParameter* pInputParamAsResult=pUseParameter;
				while(iWhichParamIndex>1)
				{
					pInputParamAsResult=pInputParamAsResult->GetNext();
					iWhichParamIndex--;
				}

				// This Input Param Is the Return Output too!
//				pReturnParameterToken = new CStr(pInputParamAsResult->GetMathItem()->GetResultStringToken()->GetStr());
				// Above line redundant due to : pInputParamToUseAsOutput in ParseInstructionn.cpp
			}
		}

		// Complete Object Data
		pInstruction->SetType(dwInstructionType);
		pInstruction->SetValue(dwCorrectInstructionValue);
		pInstruction->SetParamMax(dwValidParamMax);
		pInstruction->SetParameter(pUseParameter);
		pInstruction->SetLineNumber(StatementLineNumber);
		pInstruction->SetInstructionRef(pRef);
		pInstruction->SetReturnParameter(pReturnParameterToken);
		pInstruction->SetLabelParam(pFullLabelName);

		// Add The Object To This Statement
		CStatement *TheObject = new CStatement();
		TheObject->m_dwObjectType = dwStatementType;
		TheObject->m_pObjectClass = (void*)pInstruction;
		TheObject->m_pParameters = NULL;
		TheObject->SetLineAndCharPos(StatementLineNumber);
		this->Add(TheObject);

		// Add Optional Result Assignment (for instructions whole input params where output (input a))
		if(pInputAsOutputAssignment)
		{
			this->Add(pInputAsOutputAssignment);
			pInputAsOutputAssignment=NULL;
		}

		// Done one more
		iMultipleInstructions--;
	}

	// Free temp allocations
	if(bOneParamPerRepeatedInstruction==true)
	{
		// As we used locally created single params
		SAFE_DELETE(pFirstParameter);
	}

	// Complete
	return true;
}

bool CStatement::DoAssignment(DWORD StatementLineNumber, DWORD TokenID)
{
	// Re-order assignment to look like instruction
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();
	LPSTR pAlternateFullString = ProduceFullSegment(&pPointer);
	CStr* pAltString = new CStr(pAlternateFullString);
	DWORD dwPos = pAltString->FindFirstChar('=');
	SAFE_DELETE(pAlternateFullString);
	SAFE_DELETE(pAltString);

	// Substitute assignment symbol for comma
	pPointer = g_pStatementList->GetFileDataPointer();
	pPointer[dwPos]=',';

	// Assignment instruction
	g_pStatementList->SetInstructionType(2);
	g_pStatementList->SetInstructionRef(g_pInstructionTable->GetRef(IT_INTERNAL_ASSIGNLL));
	g_pStatementList->SetInstructionValue(g_pInstructionTable->GetIIValue(IT_INTERNAL_ASSIGNLL));
	g_pStatementList->SetInstructionParamMax(2);
	if(DoInstruction(StatementLineNumber, TokenID)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoAssignment::DoInstruction'");
		return false;
	}

	return true;
}

bool CStatement::DoAllocation(DWORD StatementLineNumber, LPSTR pVarName, LPSTR pValue)
{
	// Create Object
	CParseInstruction *pInstruction = new CParseInstruction();

	// Parameter Chain
	CParameter* pFirstParameter = NULL;

	// Create String for array name
	CStr* pStr = new CStr("&");
	pStr->AddText(pVarName);

	// Create Parameter Object to hold Array Name
	pFirstParameter = new CParameter;
	if(DoExpression(pStr, pFirstParameter)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoAllocation::DoExpression'");
		SAFE_DELETE(pFirstParameter);
		SAFE_DELETE(pInstruction);
		SAFE_DELETE(pStr);
		return false;
	}

	// Clear D1=D9 parameter ptrs
	CParameter* pSizeParameter[10];
	for(DWORD d=0; d<=9; d++) pSizeParameter[d]=NULL;

	// lee - 150206 - u60 - brackets in array-based-subscript causes wrong comma to be picked up
	int iBracketCount = 0;

	// Get dim-size params
	DWORD dwSizeIndex=0;
	LPSTR pValueStr = pValue;
	LPSTR pLastNum = pValueStr;
	for(d=0; d<strlen(pValue)+1; d++)
	{
		// Handle Bracket Count
		if(pValueStr[d]=='(') iBracketCount++;
		if(pValueStr[d]==')') iBracketCount--;

		// lee - 150206 - u60 - if comma seperater found
		if ( iBracketCount==0 )
		{
			if(pValueStr[d]==',' || d==strlen(pValue))
			{
				if(dwSizeIndex>=9)
				{
					// Too many dimensions
					g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+40);
					SAFE_DELETE(pInstruction);
					SAFE_DELETE(pStr);
					SAFE_DELETE(pFirstParameter);
					for(DWORD r=0; r<=9; r++) SAFE_DELETE(pSizeParameter[r]);
					return false;
				}

				// Get dimension size entry
				LPSTR pPtr = pValueStr + d;
				DWORD dwLength = pPtr - pLastNum;
				LPSTR pNum = new char[dwLength+1];
				CStr* pNumStr = new CStr("");
				memcpy(pNum, pLastNum, dwLength);
				pNum[dwLength]=0;
				pNumStr->SetText(pNum);
				SAFE_DELETE(pNum);

				// Parse param string as ARRAY SIZE for this dimension
				CParameter* pSizeParameterOne = new CParameter;
				if(DoExpression(pNumStr, pSizeParameterOne)==false)
				{
					g_pErrorReport->AddErrorString("Failed to 'DoAllocation::DoExpression'");
					SAFE_DELETE(pInstruction);
					SAFE_DELETE(pStr);
					SAFE_DELETE(pFirstParameter);
					SAFE_DELETE(pNumStr);
					SAFE_DELETE(pSizeParameterOne);
					for(DWORD r=0; r<=9; r++) SAFE_DELETE(pSizeParameter[r]);
					return false;
				}
				pSizeParameter[dwSizeIndex] = pSizeParameterOne;
				dwSizeIndex++;

				// Free and next
				SAFE_DELETE(pNumStr);
				pLastNum=pPtr+1;
			}
		}
	}

	// Rest must be zero is param form (for DIM input)
	while(dwSizeIndex<9)
	{
		CStr* pNumStr = new CStr("0");
		CParameter* pSizeParameterOne = new CParameter;
		if(DoExpression(pNumStr, pSizeParameterOne)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoAllocation::DoExpression2'");
			SAFE_DELETE(pInstruction);
			SAFE_DELETE(pStr);
			SAFE_DELETE(pFirstParameter);
			SAFE_DELETE(pNumStr);
			SAFE_DELETE(pSizeParameterOne);
			for(DWORD r=0; r<=9; r++) SAFE_DELETE(pSizeParameter[r]);
			return false;
		}
		SAFE_DELETE(pNumStr);
		pSizeParameter[dwSizeIndex] = pSizeParameterOne;
		dwSizeIndex++;
	}

	// Find out type of array element
	DWORD dwArrType=1;
	LPSTR pVarType=NULL;
	g_pVarTable->FindTypeOfVariable(pStr->GetStr(),dwArrType,&pVarType);
	DWORD dwTypeSize=g_pStructTable->GetSizeOfType(pVarType);

	// LEEFIX - 151101 - Can produce an error if type does not exist
	CStructTable* pStruct = g_pStructTable->DoesTypeEvenExist(pVarType);
	if(pStruct==NULL)
	{
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+35, pVarType);
		SAFE_DELETE(pInstruction);
		SAFE_DELETE(pStr);
		SAFE_DELETE(pFirstParameter);
		for(DWORD r=0; r<=9; r++) SAFE_DELETE(pSizeParameter[r]);
		return false;
	}
	DWORD dwTypeValue=pStruct->GetTypeValue();

	// Cleveryly incorporate type-value into type-size
	if ( 0 )
	{
		// old way - pre U71
		dwTypeSize=dwTypeSize*10;
		if(dwTypeValue>0 && dwTypeValue<10)
			dwTypeSize+=(dwTypeValue-1);
		else
			dwTypeSize+=9;
	}
	else
	{
		// leeadd - 211008 - u71 - increased capacity to store actual Type Value (index)
		dwTypeSize=dwTypeSize*4096; // data size multiple of 4096 (parsed out in core.dll DimDDD)
		if(dwTypeValue>0 && dwTypeValue<10)
			dwTypeSize+=(dwTypeValue-1);
		else
		{
			// was just 9, but now reflects the user type used
			int iTypeIndexInStructTable = g_pStructTable->FindIndex(pVarType);
			if ( iTypeIndexInStructTable <= 4095 )
				dwTypeSize+=iTypeIndexInStructTable; // holds up to 4095 type value
			else
				dwTypeSize+=4095; // cap to prevent crashes
		}
	}

	CStr* pDataTypeSize = new CStr("");
	pDataTypeSize->AddNumericText(dwTypeSize);
	SAFE_DELETE(pVarType);

	// Parse third param string as TYPE SIZE
	CParameter* pTypeSizeParameter = new CParameter;
	if(DoExpression(pDataTypeSize, pTypeSizeParameter)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoAllocation::DoExpression'");
		SAFE_DELETE(pFirstParameter);
		SAFE_DELETE(pInstruction);
		SAFE_DELETE(pStr);
		SAFE_DELETE(pDataTypeSize);
		SAFE_DELETE(pTypeSizeParameter);
		for(DWORD r=0; r<=9; r++) SAFE_DELETE(pSizeParameter[r]);
		return false;
	}

	// Free usages
	SAFE_DELETE(pDataTypeSize);
	SAFE_DELETE(pStr);

	// Make sure array size is cast to DWORD
	for(d=0; d<=8; d++)
	{
		if(pSizeParameter[d])
		{
			if(pSizeParameter[d]->GetMathItem()->FindResultTypeValueForDBM()!=7)
			{
				// Create New Math Op to Cast Value
				CMathOp* pValueToCast = pSizeParameter[d]->GetMathItem();
				pSizeParameter[d]->GetMathItem()->DoCastOnMathOp(&pValueToCast, 7);
				pSizeParameter[d]->SetMathItem(pValueToCast);
			}
		}
	}

	// Make sure data type size is cast to DWORD
	if(pTypeSizeParameter->GetMathItem()->FindResultTypeValueForDBM()!=7)
	{
		// Create New Math Op to Cast Value
		CMathOp* pValueToCast = pTypeSizeParameter->GetMathItem();
		pTypeSizeParameter->GetMathItem()->DoCastOnMathOp(&pValueToCast, 7);
		pTypeSizeParameter->SetMathItem(pValueToCast);
	}

	// Add second param
	pFirstParameter->Add(pTypeSizeParameter);

	// Add D1 to D9 params
	for(d=0; d<=8; d++)
		if(pSizeParameter[d])
			pFirstParameter->Add(pSizeParameter[d]);

	// Make Sure First Param is DWORD (so actual alloc address goes into array ptr var)
	pFirstParameter->GetMathItem()->GetResultData()->m_dwType=7;

	// Complete Object Data
	pInstruction->SetType(2);
	pInstruction->SetValue(g_pInstructionTable->GetIIValue(IT_INTERNAL_ALLOC));
	pInstruction->SetInstructionRef(g_pInstructionTable->GetRef(IT_INTERNAL_ALLOC));
	pInstruction->SetParamMax(10);
	pInstruction->SetParameter(pFirstParameter);
	pInstruction->SetLineNumber(StatementLineNumber);

	// Add The Object To This Statement
	CStatement *TheObject = new CStatement();
	TheObject->m_dwObjectType = 11;
	TheObject->m_pObjectClass = (void*)pInstruction;
	TheObject->m_pParameters = NULL;
	TheObject->SetLineAndCharPos(StatementLineNumber);
	this->Add(TheObject);
	return true;
}

bool CStatement::DoDeAllocation(DWORD StatementLineNumber)
{
	// Create Object
	CParseInstruction *pInstruction = new CParseInstruction();

	// Parameter Chain
	CParameter* pFirstParameter = NULL;

	// Create String for array name
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();
	LPSTR pArrayName = ProduceNextArrayToken(&pPointer);

	// Ensure leading/trailing spaces removed
	CStr* pStr2 = new CStr(pArrayName);
	pStr2->EatEdgeSpacesandTabs(NULL);
	SAFE_DELETE(pArrayName);

	// compare with void
	if(pStr2->Length()==0)
	{
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+43);
		SAFE_DELETE(pInstruction);
		SAFE_DELETE(pStr2);
		return false;
	}
	g_pStatementList->SetFileDataPointer(pPointer);
	CStr* pStr = new CStr("&");
	pStr->AddText(pStr2->GetStr());

	// Cut off all but the array name
	DWORD dwPos = pStr->FindFirstChar('(');
	pStr->SetChar(dwPos,0);

	// Create pArrayParameter Object
	CParameter* pArrayParameter = new CParameter;
	if(DoExpression(pStr, pArrayParameter)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoDeAllocation::pArrayParameter::DoExpression'");
		SAFE_DELETE(pArrayParameter);
		SAFE_DELETE(pInstruction);
		SAFE_DELETE(pStr);
		return false;
	}

	// Create Parameter Object
	CParameter* pReturnParameter = new CParameter;
	if(DoExpression(pStr, pReturnParameter)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoDeAllocation::pReturnParameter::DoExpression'");
		SAFE_DELETE(pReturnParameter);
		SAFE_DELETE(pArrayParameter);
		SAFE_DELETE(pInstruction);
		SAFE_DELETE(pStr);
		return false;
	}
	SAFE_DELETE(pStr);

	// Ensure return param and dealloc address uses POINTER MATHS (actual address, not relative address(as in array use))
	pReturnParameter->GetMathItem()->SetResultType(7);
	pArrayParameter->GetMathItem()->SetResultType(7);

	// First param set
	pFirstParameter=pReturnParameter;

	// Add Actual first param
	pFirstParameter->Add(pArrayParameter);

	// Complete Object Data
	pInstruction->SetType(2);
	pInstruction->SetValue(g_pInstructionTable->GetIIValue(IT_INTERNAL_FREE));
	pInstruction->SetInstructionRef(g_pInstructionTable->GetRef(IT_INTERNAL_FREE));
	pInstruction->SetParamMax(2);
	pInstruction->SetReturnParameter(pStr);
	pInstruction->SetParameter(pFirstParameter);
	pInstruction->SetLineNumber(StatementLineNumber);

	// Add The Object To This Statement
	CStatement *TheObject = new CStatement();
	TheObject->m_dwObjectType = 11;
	TheObject->m_pObjectClass = (void*)pInstruction;
	TheObject->m_pParameters = NULL;
	TheObject->SetLineAndCharPos(StatementLineNumber);
	this->Add(TheObject);
	return true;
}

bool CStatement::DoUserFunction(DWORD StatementLineNumber, DWORD TokenID)
{
	// Modify UserFunction line in filedata for parsing
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();

	// MYFUNC(dec) -> MYFUNC dec (remove brackets)
	DWORD SPos=0, EPos=0;
	LPSTR pFunctionPointer=pPointer;
	RemoveEdgeBracketFromSegment(pPointer, &SPos, &EPos);

	// leefix - 250604 - u54 - allow to skip initial spaces between FUNCTION and name
	bool bAllowInitialSpaces = true;

	// Check the name does not contain a space
	for ( DWORD i=0; i<SPos; i++ )
	{
		// name contains a space!
		if ( pPointer [ i ]==32 )
		{
			// Function declaration error if space INSIDE name
			if ( bAllowInitialSpaces==false )
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+45);
				return false;
			}
		}
		else
		{
			// started getting non-spaces, cannot have any more
			bAllowInitialSpaces=false;
		}
	}

	// Read off name of UserFunction
	LPSTR pUserFunctionName = ProduceNextToken(&pPointer, true, false, false);

	// Is function name alphanumeric?
	DWORD dwPos = 0;
	CStr* pStr = new CStr(pUserFunctionName);
	if ( !pStr->IsAlphaNumericLabel(dwPos) )
	{
		// Function declaration error
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+45);
		SAFE_DELETE(pUserFunctionName);
		SAFE_DELETE(pStr);
		return false;
	}
	// is first character numeric, if so, canot be function name
	if ( pStr->GetChar(0)>='0' && pStr->GetChar(0)<='9' )
	{
		// Function declaration error
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+45);
		SAFE_DELETE(pUserFunctionName);
		SAFE_DELETE(pStr);
		return false;
	}
	SAFE_DELETE(pStr);

	// Advance past any nullspaces
	LPSTR pPointerEnd = g_pStatementList->GetFileDataEnd();
	while(pPointer<pPointerEnd)
	{
		if(*pPointer!=32 && *pPointer!=9) break;
		pPointer++;
	}

	// If reach end before reach param decs, skip dec parse
	bool bDecsInFunctionDef=true;
	if(*pPointer==13 || *pPointer==10 || *pPointer==':')
		bDecsInFunctionDef=false;
	else
	{
		// Possible space inside user function declaration
		// (only valid char now is bracket)
		if(SPos==0)
		{
			// Function declaration error
			DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+45);
			SAFE_DELETE(pUserFunctionName);
			return false;
		}
	}

	// Record new pointer
	g_pStatementList->SetFileDataPointer(pPointer);

	// Create Full Label Name
	CStr* pFullLabelName = new CStr("$label ");
	pFullLabelName->AddText(pUserFunctionName);

	// Ensure user function name is unique at PRESCAN
	if(g_pStatementList->GetImplementationParse()==false)
	{
		if(g_pLabelTable->FindLabel(pFullLabelName->GetStr())!=NULL)
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+53, pUserFunctionName);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pFullLabelName);
			return false;
		}
	}

	// Check Global Current user Function Dec Chain
	if(g_pStatementList->GetUserFunctionDecChain())
	{
		// Already defining a function
		DWORD StatementLineNumber = g_pStatementList->GetLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+65);
		SAFE_DELETE(pUserFunctionName);
		SAFE_DELETE(pFullLabelName);
		return false;
	}

	// PRESCAN or FullCode Parse
	if(g_pStatementList->GetImplementationParse()==false)
	{
		// Create Blank Declaration to start Chain
		CDeclaration* pDecChain = new CDeclaration;
		pDecChain->SetDecData(0,"","returnvalue","integer","",StatementLineNumber);
		g_pStatementList->SetUserFunctionDecChain(pDecChain);
		g_pStatementList->SetUserFunctionName(pUserFunctionName);

		// Now Parse Declaration from File Data
		DWORD dwNumberOfUFParams=0;
		LPSTR pParamTypeString=NULL;
		CStatement* pDecBlock = NULL;
		if(bDecsInFunctionDef==false)
		{
			pParamTypeString = new char[2];
			strcpy(pParamTypeString, "0");
		}
		else
		{
			pDecBlock = new CStatement(); // user function params are local, not global
			if(pDecBlock->DoDeclaration(false, CRTK, &pDecChain, false, false, false, false)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoUserFunction::Do Declaration'");
				SAFE_DELETE(pUserFunctionName);
				SAFE_DELETE(pDecBlock);
				SAFE_DELETE(pDecChain);
				SAFE_DELETE(pFullLabelName);
				return false;
			}
			SAFE_DELETE(pDecBlock);

			// Count Number of Parameters for this UserFunction
			if(pDecChain) pDecChain->GetNumberOfDecsInChain(&dwNumberOfUFParams);
			if(pDecChain) pDecChain->GetTypeStringOfDecsInChain(&pParamTypeString);
		}

		// Now Parse Block from File Data
		DWORD dwEndToken = ENDUSERFUNCTIONTK;
		CStatement *pCodeBlock = new CStatement();
		if(pCodeBlock->DoLocalScanBlock(dwEndToken)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoUserFunction::DoLocalScanBlock'");
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pDecBlock);
			SAFE_DELETE(pDecChain);
			SAFE_DELETE(pFullLabelName);
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pCodeBlock);
			return false;
		}
		SAFE_DELETE(pCodeBlock);

		// LEEFIX - 210703 - situation where prescan is declaring a return param
		// at the end of ENDFUNCTION can hold a global var, but it would normally
		// be parsed by the next code as local (as its part of the function dec).
		g_pStatementList->SetLocalVarUsageAsGlobal(true);

		// Create Parameter Object
		bool bMoreParams=false;
		CParameter* pReturnParameter = NULL;
		if(DoExpressionList(&pReturnParameter,&bMoreParams)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoUserFunction::Do ExpressionList'");
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pDecBlock);
			SAFE_DELETE(pDecChain);
			SAFE_DELETE(pFullLabelName);
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pReturnParameter);
			return false;
		}

		// Restore behaviour to normal - local=local / global=global
		g_pStatementList->SetLocalVarUsageAsGlobal(false);

		// If more than one param, fail
		if(bMoreParams==true && pReturnParameter)
		{
			DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+30);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pDecBlock);
			SAFE_DELETE(pDecChain);
			SAFE_DELETE(pFullLabelName);
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pReturnParameter);
			return false;
		}

		// Finished adding local vars within scope of user function
		g_pStatementList->SetUserFunctionDecChain(NULL);
		g_pStatementList->SetUserFunctionName("");

		// Determine Return Type of function
		DWORD dwUserFunctionReturnType=0;
		if(pReturnParameter)
		{
			// leefix - 240604 - u54 - do not allow temp vars
			LPSTR pReturnVarStr = NULL;
			if ( pReturnParameter->GetMathItem()->GetResultStringToken() )
				pReturnVarStr = pReturnParameter->GetMathItem()->GetResultStringToken()->GetStr();
			if ( pReturnVarStr!=NULL )
				if ( pReturnParameter->GetMathItem()->GetNext() && strnicmp ( pReturnVarStr, "FS@", 3 )!=NULL )
					pReturnVarStr=NULL;

			// leefix - 280305 - sometimes no result string, fixed null ptr crash
			if ( pReturnVarStr==NULL )
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+58);
				SAFE_DELETE(pParamTypeString);
				SAFE_DELETE(pUserFunctionName);
				SAFE_DELETE(pFullLabelName);
				SAFE_DELETE(pDecChain);
				return false;
			}
			else
			{
				for ( DWORD c=strlen(pReturnVarStr); c>0; c-- )
				{
					if ( (unsigned char)pReturnVarStr[c]=='@' )					
					{
						// now check character to right, if it is $ then it is a temp var and not allowed
						if ( (unsigned char)pReturnVarStr[c+1]=='$' )					
						{
							DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
							g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+58);
							SAFE_DELETE(pParamTypeString);
							SAFE_DELETE(pUserFunctionName);
							SAFE_DELETE(pFullLabelName);
							SAFE_DELETE(pDecChain);
							return false;
						}
						break;
					}
				}
				dwUserFunctionReturnType = pReturnParameter->GetMathItem()->FindResultTypeValueForDBM();
			}
		}
		SAFE_DELETE(pReturnParameter);

		// LEEFIX - 111002 - Cannot have array return-types. They return in the data type they represent
		if(dwUserFunctionReturnType>=100 && dwUserFunctionReturnType<=199) dwUserFunctionReturnType-=100;

		// Add User Function to database
		if(dwNumberOfUFParams>0) dwNumberOfUFParams-=1;
		if(g_pInstructionTable->AddUserFunction(pUserFunctionName, dwUserFunctionReturnType, pParamTypeString+1, dwNumberOfUFParams, pDecChain)==false)
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+24, pUserFunctionName);
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pFullLabelName);
			SAFE_DELETE(pDecChain);
			return false;
		}

		// Add label to go with user function (for jump call)
		if(g_pLabelTable->AddLabel(pFullLabelName->GetStr(), 0, 0, NULL)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoUserFunction::AddLabel'");
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pFullLabelName);
			SAFE_DELETE(pDecChain);
			return false;
		}

		// Free usages
		SAFE_DELETE(pParamTypeString);
	}
	else
	{
		// leefix - 220604 - u54 - FIRST ensure user function is not in any nests
		if ( g_pStatementList->m_iNestCount > 0 )
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+56, pUserFunctionName);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pFullLabelName);
			return false;
		}

		// Create Blank Declaration to start Chain
		CDeclaration* pDecChain = new CDeclaration;
		pDecChain->SetDecData(0,"","returnvalue","integer","",StatementLineNumber);
		g_pStatementList->SetUserFunctionDecChain(pDecChain);
		g_pStatementList->SetUserFunctionName(pUserFunctionName);

		// Now Parse Declaration from File Data
		DWORD dwNumberOfUFParams=0;
		LPSTR pParamTypeString=NULL;
		CStatement* pDecBlock = NULL;
		if(bDecsInFunctionDef==false)
		{
			pParamTypeString = new char[2];
			strcpy(pParamTypeString, "0");
		}
		else
		{
			pDecBlock = new CStatement(); //as above, params in user function are local not global
			if(pDecBlock->DoDeclaration(false, CRTK, &pDecChain, false, false, false, false)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoUserFunction::Do Declaration'");
				SAFE_DELETE(pDecChain);
				SAFE_DELETE(pDecBlock);
				SAFE_DELETE(pUserFunctionName);
				SAFE_DELETE(pFullLabelName);
				return false;
			}
			SAFE_DELETE(pDecBlock);

			// Count Number of Parameters for this UserFunction
			if(pDecChain) pDecChain->GetNumberOfDecsInChain(&dwNumberOfUFParams);
			if(pDecChain) pDecChain->GetTypeStringOfDecsInChain(&pParamTypeString);
		}

		// Create BLOCK Object for nested code
		CStatement *TheUserFunctionObject = new CStatement();
		CStatement *pCodeBlock = new CStatement();

		// Update label for User-Function-Call
		if(g_pLabelTable->UpdateLabel(pFullLabelName->GetStr(), StatementLineNumber, 0, TheUserFunctionObject)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'AddInternalLabel' in CStatement::DoUserFunction");
			SAFE_DELETE(pCodeBlock);
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pDecChain);
			SAFE_DELETE(pDecBlock);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pFullLabelName);
			SAFE_DELETE(TheUserFunctionObject);
			return false;
		}

		// Now Parse Block from File Data
		DWORD dwEndToken = ENDUSERFUNCTIONTK;
		if(pCodeBlock->DoBlock(dwEndToken,NULL)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoUserFunction::DoBlock'");
			SAFE_DELETE(pCodeBlock);
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pDecChain);
			SAFE_DELETE(pDecBlock);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pFullLabelName);
			SAFE_DELETE(TheUserFunctionObject);
			return false;
		}

		// Create Parameter Object
		bool bMoreParams=false;
		CParameter* pReturnParameter = NULL;
		if(DoExpressionList(&pReturnParameter,&bMoreParams)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'DoUserFunction::Do ExpressionList'");
			SAFE_DELETE(pCodeBlock);
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pDecChain);
			SAFE_DELETE(pDecBlock);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pFullLabelName);
			SAFE_DELETE(TheUserFunctionObject);
			return false;
		}

		// If more than one param, fail
		if(bMoreParams==true && pReturnParameter)
		{
			DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+30);
			SAFE_DELETE(pCodeBlock);
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pDecChain);
			SAFE_DELETE(pDecBlock);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pFullLabelName);
			SAFE_DELETE(TheUserFunctionObject);
			return false;
		}

		// leefix - 230604 - u54 - Return param must be a single value, not an expression
		if ( pReturnParameter )
		{
			if ( pReturnParameter->GetMathItem()->GetMathSymbol()!=0 )
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+58);
				SAFE_DELETE(pCodeBlock);
				SAFE_DELETE(pParamTypeString);
				SAFE_DELETE(pDecChain);
				SAFE_DELETE(pDecBlock);
				SAFE_DELETE(pUserFunctionName);
				SAFE_DELETE(pFullLabelName);
				SAFE_DELETE(TheUserFunctionObject);
				return false;
			}
		}

		// Finished adding local vars within scope of user function
		g_pStatementList->SetUserFunctionDecChain(NULL);
		g_pStatementList->SetUserFunctionName("");

		// Create Object
		CParseUserFunction *pUserFunction = new CParseUserFunction();

		// Add New "UserFunctionDataType" To Table
		bool bReportError = false;
		g_pStructTable->AddStructUserType(0, pUserFunctionName, 'U', pDecChain, pDecBlock, 2, &bReportError, dwNumberOfUFParams);

		// Complete Object Data
		CStr* pStrName = new CStr(pUserFunctionName);
		pUserFunction->SetName(pStrName);
		pUserFunction->SetBlock(pCodeBlock);
		pUserFunction->SetResultParameter(pReturnParameter);
		pUserFunction->SetStartLineNumber(StatementLineNumber);
		pUserFunction->SetEndLineNumber(g_pStatementList->GetTokenLineNumber());

		// Provide reference to dec chain
		pUserFunction->SetParamMax(dwNumberOfUFParams);
		pUserFunction->SetDecChainRef(pDecChain);

		// Add an END statement here
		// leeadd - 270206 - u60 - users hitting this and gettin confused, add error
		AddInternalStatement(StatementLineNumber, IT_INTERNAL_ENDERROR);

		// Add The Object To This Statement
		TheUserFunctionObject->m_dwObjectType = 6;
		TheUserFunctionObject->m_pObjectClass = (void*)pUserFunction;
		TheUserFunctionObject->m_pParameters = NULL;
		TheUserFunctionObject->SetLineAndCharPos(StatementLineNumber);
		this->Add(TheUserFunctionObject);

		// Add User Function to database
		if(dwNumberOfUFParams>0) dwNumberOfUFParams-=1;
		if(g_pInstructionTable->AddUserFunction(pUserFunctionName, 1, pParamTypeString, dwNumberOfUFParams, NULL)==false)
		{
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+24, pUserFunctionName);
			SAFE_DELETE(pParamTypeString);
			SAFE_DELETE(pUserFunctionName);
			SAFE_DELETE(pFullLabelName);
			return false;
		}

		// Free usages
		SAFE_DELETE(pParamTypeString);
	}

	// Free usages
	SAFE_DELETE(pUserFunctionName);
	SAFE_DELETE(pFullLabelName);

	// Restore MYFUNC dec  -> MYFUNC(dec) (replace brackets)
	if(SPos>0)
	{
		*(pFunctionPointer+SPos)='(';
		*(pFunctionPointer+EPos)=')';
	}

	// Complete
	return true;
}

bool CStatement::DoUserFunctionCall(DWORD StatementLineNumber, DWORD TokenID)
{
	// Modify UserFunction line in filedata for parsing
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();

	// MYFUNC(dec) -> MYFUNC dec (remove brackets)
	RemoveEdgeBracketFromSegment(pPointer, NULL, NULL);

	// Process exactly like instruction
	if(DoInstruction(StatementLineNumber, TokenID)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoUserFunctionCall::DoInstruction'");
		return false;
	}

	// Complete
	return true;
}

bool CStatement::DoUserFunctionExit(DWORD StatementLineNumber, DWORD TokenID)
{
	// Create Object
	CParseInstruction *pInstruction = new CParseInstruction();

	// Create Parameter Object
	bool bMoreParams=false;
	CParameter* pParameter = NULL;
	if(DoExpressionList(&pParameter,&bMoreParams)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoUserFunctionExit::ExpressionList'");
		SAFE_DELETE(pParameter);
		return false;
	}

	// If more than one param, fail
	if(bMoreParams==true && pParameter)
	{
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+31);
		SAFE_DELETE(pParameter);
		return false;
	}

	// leefix - 220604 - u54 - Check if return param matches function return type
	CInstructionTableEntry* pEntry = g_pInstructionTable->FindUserFunction ( g_pStatementList->GetUserFunctionName() );
	DWORD dwReturnType = pEntry->GetReturnParam();
	if ( pParameter )
	{
		// lee - 130206 - u60 - ignore distinction between Integer, DWORD
		DWORD dwParamResultType = pParameter->GetMathItem()->GetResultType();
		if ( ( dwParamResultType==1 || dwParamResultType==7 ) && ( dwReturnType==1 || dwReturnType==7 ) )
		{
			// end function and exit function are either INTEGER or DWORD, both interchangable in U6.0
		}
		else
		{
			if ( dwParamResultType!=dwReturnType )
			{
				// Types not the same
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+57);
				SAFE_DELETE(pParameter);
				return false;
			}
		}
		if ( pParameter->GetMathItem()->GetNext() )
		{
			// Cannot have expressions as a return value - single var or value
			DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+58);
			SAFE_DELETE(pParameter);
			return false;
		}
	}

	// Complete Object Data
	pInstruction->SetType(2);
	pInstruction->SetValue(g_pInstructionTable->GetIIValue(IT_INTERNAL_USERFUNCTIONEXIT));
	pInstruction->SetInstructionRef(g_pInstructionTable->GetRef(IT_INTERNAL_USERFUNCTIONEXIT));
	pInstruction->SetParamMax(2);
	pInstruction->SetParameter(pParameter);
	pInstruction->SetLineNumber(StatementLineNumber);

	// Add The Object To This Statement
	CStatement *TheObject = new CStatement();
	TheObject->m_dwObjectType = 11;
	TheObject->m_pObjectClass = (void*)pInstruction;
	TheObject->m_pParameters = NULL;
	TheObject->SetLineAndCharPos(StatementLineNumber);
	this->Add(TheObject);

	// Complete
	return true;
}

bool CStatement::DoParameterListString(CStr* pOrigParamString, CParameter** ppFirstParameter)
{
	// Work pParamString String
	CStr* pParamString = new CStr(pOrigParamString->GetStr());

	// Before processing function params, crop spaces, tabs and equal brackets
	do { pParamString->EatEdgeSpacesandTabs(NULL);
	} while(pParamString->CropEqualEdgeBrackets(NULL)==true);

	// Proceed to chop off all items from exp list string
	bool bEndOfExpressionsReached=false;
	while(bEndOfExpressionsReached==false)
	{
		// Create Parameter Object
		CParameter* pReturnParameter = NULL;

		// Create temp string for exp list parsing
		CStr* pUptoSeperator = new CStr(pParamString->GetStr());

		// Chop one item off string
		DWORD ReturnDistance=0;
		if(DoExpressionListString(&pReturnParameter, pUptoSeperator, &ReturnDistance, &bEndOfExpressionsReached)==false)
		{
			SAFE_DELETE(pUptoSeperator);
			SAFE_DELETE(pParamString);
			SAFE_DELETE(pReturnParameter);
			return false;
		}
		SAFE_DELETE(pUptoSeperator);

		// Create new string using rest of it
		if(bEndOfExpressionsReached==false)
		{
			CStr* pNewStr = new CStr(pParamString->GetStr()+ReturnDistance);
			SAFE_DELETE(pParamString);
			pParamString=pNewStr;
		}

		// If param made..
		if(pReturnParameter)
		{
			// Add Param to Parameter Chain
			if((*ppFirstParameter)==NULL)
				(*ppFirstParameter)=pReturnParameter;
			else
				(*ppFirstParameter)->Add(pReturnParameter);
		}
		else
			break;
	}

	// Clear memory
	SAFE_DELETE(pParamString);

	// Complete
	return true;
}

bool CStatement::DoExpressionList(CParameter** ppParameter, bool* bNoMoreParams)
{
	// Find Carriage return in File Data
	DWORD dwSpeechMark=0;
	LPSTR pStringPointer = g_pStatementList->GetFileDataPointer();
	LPSTR pStringPointerEnd = g_pStatementList->GetFileDataEnd();
	DWORD length=0;
	bool bAlphaNotSeenYet=true;
	if(pStringPointer<pStringPointerEnd)
	{
		while(*(unsigned char*)(pStringPointer+length)!=0)
		{
			if(*(unsigned char*)(pStringPointer+length+0)=='"') dwSpeechMark=1-dwSpeechMark;

			if(*(unsigned char*)(pStringPointer+length+0)==13
			&& *(unsigned char*)(pStringPointer+length+1)==10)
				break;

			if(*(unsigned char*)(pStringPointer+length+0)==10)
				break;

			if(pStringPointer+length>=g_pStatementList->GetFileDataEnd())
				break;

			if(dwSpeechMark==0)
				if(*(unsigned char*)(pStringPointer+length+0)==':')
					break;

			length++;
		}
	}
	else
	{
		// End of program means end of expression
		*bNoMoreParams=true;
		return true;
	}

	// If speech mark is open when leave, this is an error
	if ( dwSpeechMark==1 )
	{
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+62);
		return false;
	}

	// Store end of expressions
	bool bEndOfExpressionsReached=false;
	if(length>0)
	{
		// Make a string of just the expression
		CStr* pUptoSeperator = new CStr(length);
		LPSTR pPointerEnd=g_pStatementList->GetFileDataEnd();
		pUptoSeperator->CopyFromPtr(pStringPointer, pPointerEnd, length);

		// Convert all non-speechmark semicolons to commas
		DWORD dwEatenDistance=0;
		pUptoSeperator->EatEdgeSpacesandTabs(&dwEatenDistance);
		bool bLastCharIsConcat = pUptoSeperator->ReplaceSemicolons();
		if ( length==1 && bLastCharIsConcat )
		{
			// leefix - 230604 - u54 - one a concat as param means end of expression
			bEndOfExpressionsReached=true;
		}
		else
		{
			// Perform expression traversal
			DWORD ReturnDistance=0;
			if(DoExpressionListString(ppParameter, pUptoSeperator, &ReturnDistance, &bEndOfExpressionsReached)==false)
			{
				g_pErrorReport->AddErrorString("Failed to 'DoExpressionList::DoExpressionListString'");
				SAFE_DELETE(pUptoSeperator);
				return false;
			}

			// Set concat flag of param mathitem
			if(bLastCharIsConcat)
			{
				(*ppParameter)->GetMathItem()->SetConcatFlag(true);
			}

			// If expression is empty, reached end as its blank
			if(ReturnDistance==0) bEndOfExpressionsReached=true;

			// Ensure FileData pointer is updated
			pStringPointer = g_pStatementList->GetFileDataPointer();
			pStringPointer += ReturnDistance+dwEatenDistance;
			g_pStatementList->SetFileDataPointer(pStringPointer);
		}

		// Cleam memory usage
		SAFE_DELETE(pUptoSeperator);
	}
	else
	{
		// No params means end of expression
		bEndOfExpressionsReached=true;
	}

	// Advance the line when reached end of expressions list
	if(bEndOfExpressionsReached)
	{
		// Current filedata pointer
		LPSTR pPointer = g_pStatementList->GetFileDataPointer();

		// If hit a CR(13) on the way)
		bool bCRValid=false;

		// Skip characters until seperator char reached
		while(	*(unsigned char*)(pPointer)!=10
			&&	*(unsigned char*)(pPointer)!=0
			&&	*(unsigned char*)(pPointer)!=':' )
		{
			if(*(unsigned char*)(pPointer)==13) bCRValid=true;
			pPointer++;
		}

		// Advance line number if carriage return
		if(bCRValid==true)
			if(*(unsigned char*)(pPointer)==10)
				g_pStatementList->IncLineNumber();

		// Skip seperator character
		pPointer++;

		// New filedata position
		g_pStatementList->SetFileDataPointer(pPointer);
	}

	// Store result of param list ending
	if(bNoMoreParams) *bNoMoreParams=!bEndOfExpressionsReached;

	// Complete
	return true;
}

bool CStatement::DoExpressionListString(CParameter** ppParameter, CStr* pExpressionListString, DWORD* pReturnDistance, bool* bEndOfExpressionsReached)
{
	// Find end of expression seperator (CR, COMMA, COLON)
	DWORD len=5;
	char SepChars[5];
	SepChars[0]=13;
	SepChars[1]=10;
	SepChars[2]=',';
	SepChars[3]=':';
	SepChars[4]=0;

	// Speech mark exclusion
	int iBracketCount=0;
	unsigned int iWithinSpeechMarks=0;
	unsigned char SpeechMark = '"';

	// Before processing, crop spaces, tabs and equal brackets
	DWORD HowManyCroppedTotal=0;
	pExpressionListString->EatEdgeSpacesandTabs(&HowManyCroppedTotal);
//	pExpressionListString->CropAll(&HowManyCroppedTotal); //made (6-3)+2 not work!

	// Ensure has contents
	if(pExpressionListString->Length()==0)
	{
		// Empty no params
		if(pReturnDistance) *pReturnDistance = HowManyCroppedTotal;
		return true;
	}

	// Create param obj for contents
	(*ppParameter) = new CParameter;

	// Go through File Data
	LPSTR pStringPointer = pExpressionListString->GetStr();
	LPSTR pStartStringPointer = pStringPointer;
	LPSTR pStringPointerEnd = pStringPointer + pExpressionListString->Length();
	while(true)
	{
		// Terminate if reached end of data
		if(pStringPointer>=pStringPointerEnd)
		{
			if(bEndOfExpressionsReached)
				*bEndOfExpressionsReached=true;

			break;
		}

		// Handle End of Line Detection by CR or Terminator
		if((*(unsigned char*)(pStringPointer+0)==0))
		{
			if(bEndOfExpressionsReached)
			{
				*bEndOfExpressionsReached=true;
				break;
			}
		}

		if((*(unsigned char*)(pStringPointer+0)==13
		&&  *(unsigned char*)(pStringPointer+1)==10))
		{
			if(bEndOfExpressionsReached)
				*bEndOfExpressionsReached=true;
		}

		// Inside or Outside Speech Marks
		if(*(unsigned char*)pStringPointer==SpeechMark)
			iWithinSpeechMarks=1-iWithinSpeechMarks;

		// Expression Seperator
		bool bFlag=false;
		for(unsigned int n=0; n<len; n++)
		{
			if(iWithinSpeechMarks==0)
			{
				// Handle Bracket Count
				if(*(unsigned char*)(pStringPointer+0)=='(') iBracketCount++;
				if(*(unsigned char*)(pStringPointer+0)==')') iBracketCount--;

				// Handle Seperator Detection
				if(iBracketCount==0)
					if(*(unsigned char*)pStringPointer==(unsigned char)SepChars[n])
						bFlag=true;
			}
		}

		// End Expression
		if(bFlag==true)
		{
			// No parameter taken (might be ,,)
			if(pStringPointer==pStartStringPointer)
			{
				DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
				g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+32);
				return false;
			}

			// Remove Seperator character
			*(unsigned char*)pStringPointer=0;
			pStringPointer++;
			break;
		}

		// Next byte
		pStringPointer++;
	}
	if(pReturnDistance)
	{
		*pReturnDistance = (pStringPointer-pStartStringPointer)+HowManyCroppedTotal;
	}

	// pStartStringPointer to g_pStatementList is ONE EXPRESSION 
	CStr StrOneExpressionString(1);
	DWORD length=pStringPointer-pStartStringPointer;
	LPSTR pPointerEnd=pStringPointerEnd;
	StrOneExpressionString.CopyFromPtr(pStartStringPointer, pPointerEnd, length);
	if(DoExpression(&StrOneExpressionString, (*ppParameter))==false)
	{
		return false;
	}

	return true;
}

bool CStatement::DoExpression(CStr* pStr, CParameter* pParameter)
{
	CMathOp* pMathOp = new CMathOp;
	if(pMathOp)
	{
		if(pMathOp->DoValue(pStr)==false)
		{
			SAFE_DELETE(pMathOp);
			return false;
		}
	}
	else
	{
		// soft fail
		return false;
	}

	// Place in param obj
	pParameter->SetMathItem(pMathOp);

	// Complete
	return true;
}

LPSTR CStatement::ProduceNextTokenEx(LPSTR* pString, bool bIncrementLineNumber, bool bProduceCRTK, bool bIncludeCommas, bool bIgnoreSpacesAroundEquateSymbol)
{
	// Find first token at pointer
	char SepChars[33];
	for(unsigned int s=0; s<32; s++) SepChars[s]=1+s;
	SepChars[32]=0;

	// Wipe out space as seperator (overwride space char)
	if ( bIgnoreSpacesAroundEquateSymbol) SepChars[31]=31;

	// Start/End Flag
	LPSTR pFindStartOfToken=NULL;

	// Find Seperators
	DWORD dwSpeechMark=0;
	bool bLineSeperatorFlag=false;
	bool bIncToAvoidRepeatCR=false;
	LPSTR pStringPointer = *pString;
	unsigned int len=strlen(SepChars);
	while(pStringPointer<g_pStatementList->GetFileDataEnd())
	{
		bool bFlag=false;
		for(unsigned int n=0; n<len; n++)
		{
			if(*(unsigned char*)pStringPointer==(unsigned char)SepChars[n])
				bFlag=true;
		}
		if(*(unsigned char*)(pStringPointer+0)==13
		&& *(unsigned char*)(pStringPointer+1)==10)
			bLineSeperatorFlag=true;

		// leefix - 220703 - if space found, allow it if equate near by
		// leefix - 230305 - and space is OUTSIDE quotes
		if ( bIgnoreSpacesAroundEquateSymbol && dwSpeechMark==0 )
		{
			bool bSpaceIsAllowed=false;
			if(*(unsigned char*)pStringPointer==' ')
			{
				// check back
				for ( LPSTR pChk=pStringPointer-1; pChk>=*pString; pChk-- )
				{
					if ( *(unsigned char*)pChk!=' ' && *(unsigned char*)pChk!='=' )
						break;

					if ( *(unsigned char*)pChk=='=' )
					{
						bSpaceIsAllowed=true;
						break;
					}
				}

				// check forward
				for ( pChk=pStringPointer+1; pChk<g_pStatementList->GetFileDataEnd(); pChk++ )
				{
					if ( *(unsigned char*)pChk!=' ' && *(unsigned char*)pChk!='=' )
						break;

					if ( *(unsigned char*)pChk=='=' )
					{
						bSpaceIsAllowed=true;
						break;
					}
				}

				// if not allowed, flag as a seperator
				if ( bSpaceIsAllowed==false )
					bFlag=true;
			}
		}

		if(pStringPointer>=g_pStatementList->GetFileDataEnd())
			break;

		if(*(unsigned char*)(pStringPointer+0)=='"') dwSpeechMark=1-dwSpeechMark;

		bool bQuickQuit=false;
		if(dwSpeechMark==0)
		{
			if(*(unsigned char*)(pStringPointer+0)==':')
			{
				if(pFindStartOfToken==NULL)
					bQuickQuit=true;
				else
					break;
			}
		}

		if(pFindStartOfToken==NULL)
			if(pStringPointer>=g_pStatementList->GetFileDataEnd()-1)
				bQuickQuit=true;

		if(bQuickQuit==true)
		{
			// Produce CRTK if flagged
			LPSTR pProducedToken = new char[3];
			pProducedToken[0]=13;
			pProducedToken[1]=10;
			pProducedToken[2]=0;

			// Advance beyond : in filedata
			pStringPointer+=1;
			*pString = pStringPointer;

			return pProducedToken;
		}

		if(dwSpeechMark==0)
			if(bIncludeCommas==true && *(unsigned char*)(pStringPointer+0)==',')
				bFlag=true;

		if(dwSpeechMark==0) // Comment Symbol needs to space to make into token
			if(*(unsigned char*)(pStringPointer+0)=='`')
			{
				pFindStartOfToken=pStringPointer;
				pStringPointer++;
				if(*(unsigned char*)(pStringPointer+0)==13) bLineSeperatorFlag=true;
				break;
			}

		if(bLineSeperatorFlag==true)
		{
			// If producing CRTK and reach one, if token filled, leave without absorbing CR
			if(bProduceCRTK && pStringPointer>(*pString))
			{
				if(pFindStartOfToken==NULL)
				{
					// Produce CRTK if flagged
					LPSTR pProducedToken = new char[3];
					pProducedToken[0]=13;
					pProducedToken[1]=10;
					pProducedToken[2]=0;
					return pProducedToken;
				}
				else
					break;
			}

			// If producing CRTK and reach one, if token starts with CR, pass it out
			if(bProduceCRTK && pStringPointer==(*pString))
			{
				if(bIncrementLineNumber)
					g_pStatementList->IncLineNumber();

				// Produce CRTK if flagged
				LPSTR pProducedToken = new char[3];
				pProducedToken[0]=13;
				pProducedToken[1]=10;
				pProducedToken[2]=0;

				// Advance beyond CR in filedata
				pStringPointer+=2;
				*pString = pStringPointer;

				return pProducedToken;
			}

			// leefix - 240604 - u54 - make sure we increment line number when encounter lineCR
			if(bIncrementLineNumber)
			{
				// increment line 
				if(bIncrementLineNumber)
				{
					g_pStatementList->IncLineNumber();
					bIncToAvoidRepeatCR = true;
				}
				bLineSeperatorFlag = false;
				if ( pFindStartOfToken!=NULL )
					break;
			}

		}
		if(pFindStartOfToken==NULL)
		{
			if(bFlag==false)
			{
				g_pStatementList->SetTokenLineNumber(g_pStatementList->GetLineNumber());
				g_pStatementList->SetLastCharInDataPosition((DWORD)(pStringPointer-g_pStatementList->GetFileDataStart()));
				pFindStartOfToken=pStringPointer;
			}
		}
		else
		{
			if(bFlag==true) break;
		}
		pStringPointer++;
	}

// leefix - 240604 - u54 - now done above (as many CDs can pass by)
//	if(bLineSeperatorFlag==true && bProduceCRTK==false)
//		if(bIncrementLineNumber)
//			g_pStatementList->IncLineNumber();

	// Return the created token
	LPSTR pProducedToken = NULL;
	if(pFindStartOfToken!=NULL)
	{
		unsigned int length = pStringPointer-pFindStartOfToken;
		if(length>0)
		{
			pProducedToken = new char[length+1];
			memcpy(pProducedToken, pFindStartOfToken, length+1);
			pProducedToken[length]=0;

// leefix - 350604 - u54 - i do not want to inc ptr as (GLOBAL WIZ=1: LOAD BITMAP failed as colon was skipped)
//			if(bProduceCRTK && bLineSeperatorFlag)
//			{
//				// Leave pointer so CR can be grabbed next turn
//			}
//			else
//			{
//				pStringPointer++;
//			}
		}
	}
	if ( bIncToAvoidRepeatCR ) pStringPointer++;
	*pString = pStringPointer;

	return pProducedToken;
}

LPSTR CStatement::ProduceNextToken(LPSTR* pString, bool bIncrementLineNumber, bool bProduceCRTK, bool bIncludeCommas)
{
	return ProduceNextTokenEx(pString, bIncrementLineNumber, bProduceCRTK, bIncludeCommas, false);
}

LPSTR CStatement::ProduceNextArrayToken(LPSTR* pOrigPointer)
{
	// Scan to end of segment
	int iBracketCount=0;
	DWORD dwSpeechMarks=0;
	LPSTR pPointer=*pOrigPointer;
	LPSTR pStart=pPointer;
	LPSTR pEndOfBracket=NULL;
	DWORD dwInitGetStage=0;
	while(true)
	{
		if(*(unsigned char*)(pPointer+0)==13
		&& *(unsigned char*)(pPointer+1)==10)
			break;

		if(pPointer>=g_pStatementList->GetFileDataEnd())
			break;

		if(*(unsigned char*)pPointer=='"') dwSpeechMarks=1-dwSpeechMarks;

		if(pEndOfBracket==NULL)
		{
			// First Part array name and bracket
			if(*(unsigned char*)pPointer=='(') iBracketCount++;
			if(iBracketCount>0)
			{
				if(*(unsigned char*)pPointer==')') iBracketCount--;
				if(iBracketCount==0)
				{
					// Reached end of array and bracket() segment
					pPointer++;
					pEndOfBracket=pPointer;
				}
			}
		}
		if(pEndOfBracket)
		{
			// Possible init part
			if(dwInitGetStage==0)
			{
				if(*(unsigned char*)pPointer=='=')
				{
					// Ok, get next literal
					dwInitGetStage=1;
				}
				else
				{
					if(*(unsigned char*)pPointer==' ')
					{
						// Can accept spaces before = symbol
					}
					else
					{
						// No init at end of array name
						break;
					}
				}
			}
			if(dwInitGetStage==1)
			{
				if(*(unsigned char*)pPointer!=' ') dwInitGetStage=2;
			}
			if(dwInitGetStage==2)
			{
				if(*(unsigned char*)pPointer==' ')
					break;
			}
		}

		if(dwSpeechMarks==0)
			if(*(unsigned char*)pPointer==':')
				break;

		pPointer++;
	}

	// Use longest recorded segment from string
	if(dwInitGetStage>0)
	{
		// Use longest part of string (that contains init code)
	}
	else
		pPointer=pEndOfBracket;

	// Produce segment string
	if(pPointer)
	{
		unsigned int length = pPointer-pStart;
		if(length>0)
		{
			LPSTR pProduceLine = new char[length+1];
			memcpy(pProduceLine, pStart, length+1);
			pProduceLine[length]=0;
			*pOrigPointer=pPointer;
			return pProduceLine;
		}
	}

	// No segment found
	return NULL;
}

LPSTR CStatement::ProduceFullSegment(LPSTR* pOrigPointer)
{
	// Scan to end of segment
	DWORD dwSpeechMarks=0;
	LPSTR pPointer=*pOrigPointer;
	LPSTR pStart=pPointer;
	while(true)
	{
		if(*(unsigned char*)(pPointer+0)==13
		&& *(unsigned char*)(pPointer+1)==10)
			break;

		if(pPointer>=g_pStatementList->GetFileDataEnd())
			break;

		if(*(unsigned char*)pPointer=='"') dwSpeechMarks=1-dwSpeechMarks;
		if(dwSpeechMarks==0)
			if(*(unsigned char*)pPointer==':')
				break;

		pPointer++;
	}

	// Produce segment string
	unsigned int length = pPointer-pStart;
	if(length>0)
	{
		LPSTR pProduceLine = new char[length+1];
		memcpy(pProduceLine, pStart, length+1);
		pProduceLine[length]=0;
		*pOrigPointer=pPointer;
		return pProduceLine;
	}

	// No segment found
	return NULL;
}

LPSTR CStatement::SeperateInitFromType(LPSTR pPossibleTypeAndInit)
{
	if(pPossibleTypeAndInit)
	{
		DWORD length=strlen(pPossibleTypeAndInit);
		for(DWORD n=0; n<length; n++)
		{
			if(pPossibleTypeAndInit[n]=='=')
			{
				// Modify string so it only shows type name
				pPossibleTypeAndInit[n++]=0;

				// Extract init value
				CStr* pInitValue = new CStr("");
				for(; n<length; n++)
				{
					pInitValue->AddChar(pPossibleTypeAndInit[n]);
				}
				pInitValue->EatEdgeSpacesandTabs(NULL);
				LPSTR pInitString = new char[pInitValue->Length()+1];
				strcpy(pInitString, pInitValue->GetStr());
				SAFE_DELETE(pInitValue);
				return pInitString;
			}
		}
	}
	return NULL;
}

bool CStatement::SeekCharAsPrevChar(unsigned char c, int* piBacktrak)
{
	// Get Pointer and start of line pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();
	LPSTR pStartPointer = pPointer-255;
	if(pStartPointer<g_pStatementList->GetFileDataStart())
		pStartPointer=g_pStatementList->GetFileDataStart();

	while(pPointer>pStartPointer)
	{
		if(*pPointer==c) return true;
		if(*pPointer==10) return false; //soft fail
		*piBacktrak=(*piBacktrak)-1;
		pPointer--;
	}

	// soft fail
	return false;
}

bool CStatement::SeekCharAsNextChar(unsigned char c, DWORD* pdwAdvance)
{
	// Get Pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();
	LPSTR pEndPointer = SeekToCRReadOnly(pPointer);
	while(pPointer<pEndPointer)
	{
		if(*pPointer==c) return true;
		*pdwAdvance=(*pdwAdvance)+1;
		pPointer++;
	}

	// soft fail
	return false;
}

LPSTR CStatement::GetStringToEndOfLine(void)
{
	// Get Pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();
	LPSTR pEndPointer = SeekToCRReadOnly(pPointer)-1;

	// Extract string to end of line
	CStr* pStringStr = new CStr("");
	while(pPointer<pEndPointer)
	{
		pStringStr->AddChar(*pPointer);
		pPointer++;
	}

	// Remove redundant spaces
	pStringStr->EatEdgeSpacesandTabs(NULL);

	// Create string
	LPSTR pString = new char[pStringStr->Length()+1];
	strcpy(pString, pStringStr->GetStr());
	SAFE_DELETE(pStringStr);

	// Update pointer to end of line
	g_pStatementList->SetFileDataPointer(pPointer);

	// return string
	return pString;
}

bool CStatement::SeperateValueFromArrayString(LPSTR* pArrayString, LPSTR* pArrValue, bool bMustBeLiteralDim)
{
	// Result
	bool bResult=false;

	// Create and clean string
	//CStr* pString = new CStr(*pArrayString);
	CStr pString(*pArrayString);
	pString.EatEdgeSpacesandTabs(NULL);

	// Find Where bracket value starts
	DWORD dwPos = pString.FindFirstChar('(');
	if(dwPos>0)
	{
		// Skip open bcracket
		dwPos++;

		// Get string length
		DWORD length = pString.Length();

		// Skip close bracket
		length--;

		// if length less than position, array (..) format incomplete
		if ( dwPos <= length )
		{
			// Extract out value
			*pArrValue = new char[(length-dwPos)+1];
			for(DWORD n=0; n<length-dwPos; n++)
				*(*pArrValue+n)=pString.GetChar(dwPos+n);
			*(*pArrValue+n)=0;

			// Shorten array string so just name is showing
			pString.SetChar(dwPos-1, 0);
			pString.EatEdgeSpacesandTabs(NULL);

			// Make new shortened array name
			SAFE_DELETE(*pArrayString);
			*pArrayString = new char[pString.Length()+1];
			strcpy(*pArrayString, pString.GetStr());

			// Success
			bResult=true;
		}
	}

	// Free memory
	//SAFE_DELETE(pString);

	// Complete
	return bResult;
}

bool CStatement::ContainsAssignmentOperator(CStr* pString)
{
	bool bResult=false;
	DWORD dwPos = pString->FindFirstChar('=');
	if(dwPos>0)
	{
		LPSTR pLeft = pString->GetLeftOfPosition(dwPos);
		LPSTR pRight = pString->GetRightOfPosition(dwPos+1);
		CStr* pLStr = new CStr(pLeft);
		CStr* pRStr = new CStr(pRight);
		if(pLStr->IsTextLValue()) bResult=true;
		SAFE_DELETE(pLStr);
		SAFE_DELETE(pRStr);
		SAFE_DELETE(pLeft);
		SAFE_DELETE(pRight);
	}
	return bResult;
}

bool CStatement::RemoveEdgeBracketFromSegment(LPSTR pPointer, DWORD *pdwSPos, DWORD *pdwEPos)
{
	// Find initial bracket
	LPSTR pEndPointer = SeekToSeperator(pPointer, false, false);

	// Create String upto seperator
	DWORD length = pEndPointer-pPointer;
	CStr* pStr = new CStr(length+1);
	LPSTR pPointerEnd=g_pStatementList->GetFileDataEnd();
	pStr->CopyFromPtr(pPointer, pPointerEnd, length);

	// Find first and last bracket
	DWORD dwSPos = pStr->FindFirstChar('(');
	DWORD dwEPos = pStr->FindLastChar(')');

	// Validate brackets
	if(dwSPos<length && dwEPos>0 && dwEPos<length)
	{
		// Remove brackets
		*(pPointer+dwSPos)=32;
		*(pPointer+dwEPos)=32;

		// Record Bracket erasures
		if(pdwSPos) *pdwSPos=dwSPos;
		if(pdwEPos) *pdwEPos=dwEPos;
	}

	// Free memory
	SAFE_DELETE(pStr);

	// Complete
	return true;
}

bool CStatement::ExtractDetailsFromForNext(CStr* pVar, CStr* pInit, CStr* pEnd, CStr* pStep)
{
	// Get Pointer and advance it (with line advance if so)
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();
	LPSTR pEndPointer = SeekToSeperator(pPointer, true, true);
	g_pStatementList->SetFileDataPointer(pEndPointer);

	// Transform stage
	DWORD dwStage=0;

	// Make extraction string
	DWORD length = pEndPointer-pPointer;
	CStr* pStr = new CStr(length);
	LPSTR pPointerEnd=g_pStatementList->GetFileDataEnd();
	pStr->CopyFromPtr(pPointer, pPointerEnd, length);
	pStr->SetChar(length-1,0);

	// Find Equals
	DWORD p=0;
	while(p<length)
	{
		// Find Equals
		if(dwStage==0 && pStr->CheckChar(p,'='))
		{
			dwStage=1;
			p+=1;
		}
		else
			if(dwStage==0) pVar->AddChar(pStr->GetChar(p));

		// Find _TO_
		if(dwStage==1 && pStr->CheckChars(p,4," TO "))
		{
			dwStage=2;
			p+=4;
		}
		else
			if(dwStage==1) pInit->AddChar(pStr->GetChar(p));

		// Find STEP
		if(dwStage==2 && pStr->CheckChars(p,6," STEP "))
		{
			dwStage=3;
			p+=6;
		}
		else
			if(dwStage==2) pEnd->AddChar(pStr->GetChar(p));

		// Rest is step
		if(dwStage==3) pStep->AddChar(pStr->GetChar(p));

		// Next symbol
		p++;
	}

	// Free usage
	SAFE_DELETE(pStr);

	if(dwStage<2)
	{
		// FOR NEXT syntax error
		DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
		g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+34);
		return false;
	}
	else
	{
		// Trim spaces from variable name
		pVar->EatEdgeSpacesandTabs(NULL);

		// Trim values
		pInit->EatEdgeSpacesandTabs(NULL);
		pEnd->EatEdgeSpacesandTabs(NULL);
		pStep->EatEdgeSpacesandTabs(NULL);

		// If no init or end value, error
		if ( pInit->Length()==0 || pEnd->Length()==0 || (dwStage==3 && pStep->Length()==0) )
		{
			// FOR NEXT syntax error
			DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
			g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+34);
			return false;
		}
	}

	// Complete
	return true;
}

bool CStatement::ReplaceTHENandELSEwithSep(void)
{
	// Get Pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();
	LPSTR pEndPointer = SeekToCRReadOnly(pPointer);

	// Transform stage
	DWORD dwStage=0;

	// Make extraction string
	DWORD length = pEndPointer-pPointer;
	CStr* pStr = new CStr(length);
	LPSTR pPointerEnd=g_pStatementList->GetFileDataEnd();
	pStr->CopyFromPtr(pPointer, pPointerEnd, length);

	// leefix - 250604 - u54 - remove trailing CRs and colons
	for ( DWORD c=length-1; c>0; c-- )
	{
		if ( pStr->GetChar(c)<=32 || pStr->GetChar(c)==':' )
		{
			// mirror change in actual parse data
			if ( pStr->GetChar(c)==':' ) *(pPointer+c)=32;
			pStr->SetChar(c,0);
		}
		else
			break;
	}

	// Find Equals
	DWORD p=0;
	DWORD dwSpeechMark=0;
	while(p<length)
	{
		if(pStr->CheckChar(p,'"')) dwSpeechMark=1-dwSpeechMark;
		if(dwSpeechMark==0)
		{
			// leefix - 220604 - u54 - detect termination here
			if(pStr->CheckChar(p,':')) break;

			// Find THEN
			if(dwStage==0 && pStr->CheckChars(p,4,"THEN"))
			{
				pPointer[p]=':';
				pPointer[p+1]=' ';
				pPointer[p+2]=' ';
				pPointer[p+3]=' ';
				dwStage=1;
				p+=4;
			}

			// Find THEN
			if(dwStage==1 && pStr->CheckChars(p,4,"ELSE"))
			{
				pPointer[p-1]=':';
				pPointer[p+4]=':';
				dwStage=2;
				p+=4;
			}
		}

		// Next symbol
		p++;
	}

	// Free usage
	SAFE_DELETE(pStr);

	if(dwStage==0)
	{
		// Not a THEN line soft fail
		return false;
	}
	else
	{
		// Complete
		return true;
	}
}

DWORD CStatement::GetMainToken(void)
{
	// TokenID
	DWORD dwToken=0;

	// Get Pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();

	// Ensure pointer within file data
	if(pPointer<g_pStatementList->GetFileDataEnd()-2)
	{
		// Ensure TempVarIndex is zero for new statement
		g_pStatementList->SetTempVarIndex(0);

		// First ensure pointer string is beyond string terminators
		while(*(pPointer)==0 && pPointer<g_pStatementList->GetFileDataEnd()) pPointer++;

		// Skip past CR garbage
		AdvancePastCRandSPACES(&pPointer);

		// Remember start of this token (leefix - 250604 - u54 - beyond CR skips!)
		DWORD dwStorePointerOfToken=pPointer-g_pStatementList->GetFileDataStart();
		DWORD dwStoreLineOfToken=g_pStatementList->GetLineNumber();

		if(pPointer>=g_pStatementList->GetFileDataEnd()-2)
		{
			// Reached End of Program (file data)
			return ENDTK;
		}

		// Is it a label:
		dwToken = PeekLabel(pPointer);
		if(dwToken>0)
		{
			g_pStatementList->SetFileDataPointer(pPointer);
			return dwToken;
		}

		// Check if a recognised instruction
		CInstructionTableEntry* pRef=NULL;
		DWORD dwTokenData=0, dwParamMax=0, dwLength=0;
		if(g_pInstructionTable->FindInstruction(true, pPointer, 0, &dwTokenData, &dwParamMax, &dwLength, &pRef))
		{
			// Yes, token is instruction
			dwToken=INSTRUCTIONTK;

			// Record start position of this token (instruction)
			g_pStatementList->SetTokenLineNumber(dwStoreLineOfToken);
			g_pStatementList->SetLastCharInDataPosition(dwStorePointerOfToken);

			// Record instruction for later parsing (doinstruction)
			g_pStatementList->SetInstructionRef(pRef);
			g_pStatementList->SetInstructionType(2);
			g_pStatementList->SetInstructionValue(dwTokenData);
			g_pStatementList->SetInstructionParamMax(dwParamMax);

			// Advance pPointer
			pPointer+=dwLength;
		}
		else
		{
			// Peek Token for reserved word
			dwToken = PeekToken(pPointer);

			// If token not recognise..
			if(dwToken>0)
			{
				// Get Token for reserved word
				dwToken = FindToken(pPointer, true);

				// Establish new pointer
				pPointer = g_pStatementList->GetFileDataPointer();
			}
			else
			{
				if(dwToken==0)
				{
					// Determine where end of line is
					LPSTR pEndOfLinePtr = pPointer;
					while(*(unsigned char*)(pEndOfLinePtr)!=13 && pEndOfLinePtr<g_pStatementList->GetFileDataEnd())
					{
						pEndOfLinePtr++;
					}

					// Produce string for rest of this line
					DWORD dwLengthOfRestOfLine = pEndOfLinePtr-pPointer;
					CStr* pLineStr = new CStr(dwLengthOfRestOfLine+1);
					LPSTR pPointerEnd=g_pStatementList->GetFileDataEnd();
					pLineStr->CopyFromPtr(pPointer, pPointerEnd, dwLengthOfRestOfLine);

					// Check if it qualifies as a variable declaration or assignment
					if(ContainsAssignmentOperator(pLineStr))
					{
						dwToken=ASSIGNMENTTK;
					}
					if(dwToken==0)
					{
						if(pLineStr->ContainsASOperator())
						{
							dwToken=LOCALTK;
						}
					}

					// Free memory usage
					SAFE_DELETE(pLineStr);
				}
				if(dwToken==0)
				{
					// If in prescan, do not process userfunction calls
					if(g_pStatementList->GetImplementationParse()==false)
						return CRTK;

					// Check if a user defined function
					dwTokenData=0;
					dwParamMax=0;
					dwLength=0;
					if(g_pInstructionTable->FindUserFunction(pPointer, 0, &dwTokenData, &dwParamMax, &dwLength))
					{
						// Yes, token is userfunction
						dwToken=USERFUNCTIONCALLTK;

						// Record instruction data
						g_pStatementList->SetInstructionRef(NULL);
						g_pStatementList->SetInstructionType(3);
						g_pStatementList->SetInstructionValue(dwTokenData);
						g_pStatementList->SetInstructionParamMax(dwParamMax);

						// Advance pPointer
						pPointer+=dwLength;
					}
				}
			}
		}

		// Ensure pPointer updates main pointer
		g_pStatementList->SetFileDataPointer(pPointer);
	}
	else
	{
		// Reached End of Program (file data)
		dwToken=ENDTK;
	}

	// Return TokenID if any
	return dwToken;
}

DWORD CStatement::GetToken(void)
{
	// TokenID
	DWORD dwToken=0;

	// Get Pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();

	// Ensure pointer within file data
	if(pPointer<g_pStatementList->GetFileDataEnd()-2)
	{
		// Ensure TempVarIndex is zero for new statement
		g_pStatementList->SetTempVarIndex(0);

		// First ensure pointer string is beyond string terminators
		while(*(pPointer)==0 && pPointer<g_pStatementList->GetFileDataEnd()) pPointer++;

		// Get Token FileData
		dwToken = FindToken(pPointer, true);
	}
	else
	{
		// Reached End of Program (file data)
		dwToken=ENDTK;
	}

	// Return TokenID if any
	return dwToken;
}

LPSTR CStatement::SkipAllComments ( LPSTR pPtr, LPSTR pPtrEnd )
{
	// Search for comment lines to skip
	if(strnicmp(pPtr, "`", 1)==NULL
	|| strnicmp(pPtr, "'", 1)==NULL
	|| strnicmp(pPtr, "rem", 3)==NULL
	|| strnicmp(pPtr, "remstart", 8)==NULL)
	{
		if(strnicmp(pPtr, "remstart", 8)==NULL)
		{
			// Ensure followed by sub32 character
			if(*(pPtr+8)<=32)
			{
				// Skip code block
				while(pPtr<pPtrEnd)
				{
					if(strnicmp(pPtr-5, "remend", 6)==NULL) break;

					// leefix - 110405 - must account for line number inc - tweaked 150405
					if ( *(unsigned char*)(pPtr+0)==10
					&&   *(unsigned char*)(pPtr-1)==13 )
						g_pStatementList->IncLineNumber();

					pPtr++;
				}
			}
		}
		else
		{
			// Ensure followed by 32+ character
			bool bValidComment=false;
			if(strnicmp(pPtr, "`", 1)==NULL && *(pPtr+1)>=32) bValidComment=true;
			if(strnicmp(pPtr, "'", 1)==NULL && *(pPtr+1)>=32) bValidComment=true;
			if(strnicmp(pPtr, "rem", 3)==NULL && *(pPtr+3)>=32) bValidComment=true;
			if(bValidComment)
			{
				// Only skip line
				while(pPtr<pPtrEnd)
				{
					// leefix - 110405 - must account for line number inc - tweaked 150405
					if ( *(unsigned char*)(pPtr+0)==10
					&&   *(unsigned char*)(pPtr-1)==13 )
					{
						g_pStatementList->IncLineNumber();
						break;
					}
					pPtr++;
				}
			}
		}
	}

	// new position (after all comments)
	return pPtr;
}

void CStatement::SkipToCR(void)
{
	// Get Pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();

	// First ensure pointer string is beyond string terminators
	if(*(pPointer)==0)
	{
		while(pPointer<g_pStatementList->GetFileDataEnd())
		{
			if(*(pPointer)!=0) break;
			pPointer++;
		}
	}

	// If already at CR(10), then line has already been incremented
	if(*(pPointer)==10) pPointer++;

	// Continue until reach CR (end of line)
	bool bMustIncLine=false;
	LPSTR pStartPointer = g_pStatementList->GetFileDataStart();
	while(pPointer<g_pStatementList->GetFileDataEnd())
	{
		if(pPointer>pStartPointer+1)
			if(*(unsigned char*)(pPointer-2)==13 && *(unsigned char*)(pPointer-1)==10)
				break;

		pPointer++;
		bMustIncLine=true;
	}

	// Advance line number
	if(bMustIncLine)
	{
		// leefix - 250604 - u54 - keep up to date for CR skips
		g_pStatementList->IncLineNumber();
		g_pStatementList->SetTokenLineNumber(g_pStatementList->GetLineNumber());
	}

	// Set Updated Pointer
	g_pStatementList->SetFileDataPointer(pPointer);
}

LPSTR CStatement::SeekToSeperator(LPSTR pPointer, bool bAdvanceLine, bool bStopAtComment)
{
	// Keep going until hit seperator
	DWORD dwSpeechMark=0;
	while(pPointer<g_pStatementList->GetFileDataEnd())
	{
		if((*(unsigned char*)(pPointer+0)=='"')) dwSpeechMark=1-dwSpeechMark;
		if((*(unsigned char*)(pPointer+0)==0))
		{
			pPointer++;
			break;
		}
		if(dwSpeechMark==0)
		{
			// detect break by colon
			if((*(unsigned char*)(pPointer+0)==':'))
			{
				pPointer++;
				break;
			}

			// LEEFIX - 171102 - detect break by comment
			if(bStopAtComment==true)
			{
				if(strnicmp(pPointer, "'", 1)==NULL)			break;
				if(strnicmp(pPointer, "`", 1)==NULL)			break;
				if(strnicmp(pPointer, "//", 2)==NULL)			break;
				if(strnicmp(pPointer, "rem ", 4)==NULL)			break;
				if(strnicmp(pPointer, "remstart ", 9)==NULL)	break;
			}
		}

		if((*(unsigned char*)(pPointer+0)==13
		&&	*(unsigned char*)(pPointer+1)==10))
		{
			if(bAdvanceLine) g_pStatementList->IncLineNumber();
			pPointer+=2;
			break;
		}
		pPointer++;
	}

	return pPointer;
}

LPSTR CStatement::SeekToCRReadOnly(LPSTR pPointer)
{
	// Keep going until hit seperator
	DWORD dwSpeechMark=0;
	while(pPointer<g_pStatementList->GetFileDataEnd())
	{
		if((*(unsigned char*)(pPointer+0)==0))
		{
			pPointer++;
			break;
		}
		if((*(unsigned char*)(pPointer+0)==13
		&&  *(unsigned char*)(pPointer+1)==10))
		{
			pPointer+=2;
			break;
		}
		pPointer++;
	}
	return pPointer;
}

PSTR CStatement::SeekToRemEnd(LPSTR pPointer)
{
	// Keep going until hit remend
	int iSpeechMarks=0;
	LPSTR pPointerEnd = g_pStatementList->GetFileDataEnd();
	while(pPointer<pPointerEnd)
	{
//		leefix - 280305 - within REMSTART/REMEND, speech marks are ignored
//		if((*(unsigned char*)(pPointer+0)=='"')) iSpeechMarks=1-iSpeechMarks;
		if((*(unsigned char*)(pPointer+0)==0))
		{
			if(pPointer<pPointerEnd) pPointer++;
			break;
		}
		if(iSpeechMarks==0)
		{
			if(strnicmp((char*)(pPointer), "remend", 6)==NULL)
			{
				if(pPointer+5<pPointerEnd) pPointer+=6;
				break;
			}
		}
		if((*(unsigned char*)(pPointer+0)==13
		&&  *(unsigned char*)(pPointer+1)==10))
		{
			g_pStatementList->IncLineNumber();
			if(pPointer<pPointerEnd) pPointer++;
		}
		if(pPointer<pPointerEnd) pPointer++;
	}

	return pPointer;
}

DWORD CStatement::GetTokenToSeperator(void)
{
	// Get Pointer
	LPSTR pPointer = g_pStatementList->GetFileDataPointer();

	// Determine token
	DWORD dwToken = FindToken(pPointer, false);

	// Keep going until hit seperator
	DWORD dwSpeechMark=0;
	while(pPointer<g_pStatementList->GetFileDataEnd())
	{
		if((*(unsigned char*)(pPointer+0)=='"'))
			dwSpeechMark=1-dwSpeechMark;

		if((*(unsigned char*)(pPointer+0)==0))
		{
			pPointer++;
			break;
		}

		if(dwSpeechMark==0)
		{
			if((*(unsigned char*)(pPointer+0)==':'))
			{
				pPointer++;
				break;
			}
		}

		if((*(unsigned char*)(pPointer+0)==13
		&&  *(unsigned char*)(pPointer+1)==10))
		{
			g_pStatementList->IncLineNumber();
			pPointer+=2;
			break;
		}

		pPointer++;
	}

	// Set Updated Pointer
	g_pStatementList->SetFileDataPointer(pPointer);

	return dwToken;
}

DWORD CStatement::FindToken(LPSTR pPointer, bool bIncrementLineNumber)
{
	DWORD dwToken=0;

	// Get Token FileData
	LPSTR pToken = ProduceNextToken(&pPointer, bIncrementLineNumber, false, false);

	// Determine ID from token string
	dwToken=DetermineToken(pToken);

	// Set Updated Pointer
	if(dwToken>0) g_pStatementList->SetFileDataPointer(pPointer);

	// Free Token created
	SAFE_DELETE(pToken);

	return dwToken;
}

DWORD CStatement::PeekToken(LPSTR pPointer)
{
	DWORD dwToken=0;

	// Get Token FileData
	LPSTR pToken = ProduceNextToken(&pPointer, false, false, false);

	// Determine ID from token string
	dwToken=DetermineToken(pToken);

	// Free Token created
	SAFE_DELETE(pToken);

	return dwToken;
}

DWORD CStatement::PeekLabel(LPSTR pPointer)
{
	// Return token
	DWORD dwToken=0;

	// Make string from line
	LPSTR pEndPointer = SeekToSeperator(pPointer, false, false);
	DWORD length = pEndPointer-pPointer;

	CStr* pStr = new CStr(length+1);
	LPSTR pPointerEnd=g_pStatementList->GetFileDataEnd();
	pStr->CopyFromPtr(pPointer, pPointerEnd, length);
	pStr->SetChar(length,0);

	// Clean up string
	pStr->EatEdgeSpacesandTabs(NULL);
	
	// Determine if label
	if(pStr->IsTextALabel())
	{
		// Must not be a token word
		CStr* pCheckWord = new CStr(pStr->GetStr());
		pCheckWord->SetChar(pCheckWord->Length()-1,0);
		if(DetermineToken(pCheckWord->GetStr())==0)
		{
			// must not be a reserved word
			if ( !DetermineIfReservedWord ( pCheckWord->GetStr() ) )
				dwToken=LABELTK;
		}

		// free usages
		SAFE_DELETE(pCheckWord);
	}

	// Free usage
	SAFE_DELETE(pStr);

	return dwToken;
}

CStr* CStatement::GetLabel(LPSTR* pPointer)
{
	// Make string from line
	LPSTR pEndPointer = SeekToSeperator(*pPointer, false, false);
	DWORD length = pEndPointer-(*pPointer);

	CStr* pStr = new CStr(length+1);
	LPSTR pPointerEnd=g_pStatementList->GetFileDataEnd();
	pStr->CopyFromPtr(*pPointer, pPointerEnd, length);
	pStr->SetChar(length,0);
	(*pPointer)+=length;

	// Clean up string
	pStr->EatEdgeSpacesandTabs(NULL);
	
	return pStr;
}

void CStatement::AdvancePastCRandSPACES(LPSTR* pPointerPtr)
{
	// Use pPointer temp var
	LPSTR pStringPointer=*pPointerPtr;

	// Move Pointer over CRs and SPACES, count lines as progress
	while(true)
	{
		bool bFlag=false;
		if(*(unsigned char*)pStringPointer<=32)
			bFlag=true;

		if(*(unsigned char*)(pStringPointer+0)==13
		&& *(unsigned char*)(pStringPointer+1)==10)
		{
			g_pStatementList->IncLineNumber();
			// leefix - 250604 - u54 - this code skips token code, so need to update token line number
			g_pStatementList->SetTokenLineNumber(g_pStatementList->GetLineNumber());
		}

		if(pStringPointer>=g_pStatementList->GetFileDataEnd())
			break;

		if(bFlag==false)
			break;

		pStringPointer++;
	}

	// Restore new pointer position
	*pPointerPtr=pStringPointer;
}


bool CStatement::AddInternalLabel(CStr** pReturnString)
{
	// Create string and keep for usage later on
	DWORD dwCodeIndex = m_dwLineNumber;
	CStr* pLabelName = new CStr("$label");
	pLabelName->AddNumericText(g_pStatementList->GetLabelIndexCounter());
	pLabelName->AddText("[");
	pLabelName->AddNumericText(dwCodeIndex);
	pLabelName->AddText("]");
	g_pStatementList->IncLabelIndexCounter(1);
	*pReturnString = pLabelName;

	// Add Label to Table
	DWORD dwDataIndex = g_pStatementList->GetDataIndexCounter();
	if(g_pLabelTable->AddLabel(pLabelName->GetStr(), dwCodeIndex, dwDataIndex, this)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'AddInternalLabel::AddLabel'");
		SAFE_DELETE(pLabelName);
		return false;
	}

	// Complete
	return true;
}

bool CStatement::FindCorrectInstruction(CInstructionTableEntry** pRef, CParameter* pFirstParameter, DWORD dwOrigValue, DWORD dwOrigType, DWORD dwOrigParamMax, DWORD* pdwValidInstructionToUse, bool* pbIfFindTypeA)
{
	// aaron - 201112 - for debugging purposes, declarations have been moved
	CInstructionTableEntry *entry;
	bool bInValidParams=false;
	CInstructionTableEntry* pValidEntryRef=NULL; 
	DWORD dwParamCount=0;
	DWORD dwTryInstruction = 0;

	// First Instruction Data
	DWORD dwOriginalReturnParam=0;
	if ( *pRef ) dwOriginalReturnParam = (*pRef)->GetReturnParam();
	DWORD dwInstructionValue=dwOrigValue;
	DWORD dwInstructionType=dwOrigType;

	// Initially not found
	CStr* pValidParamTypes = NULL;
	*(pdwValidInstructionToUse)=0;
	if(dwInstructionType==1)
	{
		// Internal instructions have no set params to check for
		g_pErrorReport->AddErrorString("Failed to 'FindCorrectInstruction::dwInstructionType==1'");
		return false;
	}

	bool bTypeAFlag=false;
	bool bBestIsTypeA=false;
	DWORD dwScoreBestMatch=0;
	DWORD dwBestScoreSoFar=0;
	CInstructionTableEntry* pFirstEntryRef=NULL; 
	CInstructionTableEntry* pNextEntryRef=NULL; 
	DWORD dwValidInstructionValue=0, dwValidParamMax=0;

	// pTrack kicks in on 'second' interation (search list bby entry, not by index)
	CInstructionTableEntry* pTrack = NULL;

	// Try 16 commands before give up looking for param permutations
	DWORD dwTryingIndex=0;

	// LEEFIX - 201102 - some situations 16 instances not enough, so 32
	// aaronfix - 201112 - new system does not really use indexes so a limit is pointless
	entry = g_pInstructionTable->GetEntryByIndex(dwOrigValue);
	while(entry)
	{
		// Try This Instruction
		//dwTryInstruction = dwInstructionValue + dwTryingIndex;
		dwTryInstruction = entry->GetInternalID();
		if(pTrack) dwTryInstruction = pTrack->GetInternalID();

		// Check for parameter-mismatch
		bInValidParams = false;
		pValidEntryRef = nullptr;
		dwParamCount = 0;
		if(dwInstructionType==2)
		{
			if(g_pInstructionTable->FindInstructionParams(dwTryInstruction, dwParamCount, &dwValidInstructionValue, &dwValidParamMax, &pValidParamTypes, &pValidEntryRef)==false)
			{
				bInValidParams=true;
				DB3_CRASH();
			}
		}
		if(dwInstructionType==3)
		{
			if(g_pInstructionTable->FindUserFunctionParams(dwTryInstruction, dwParamCount, &dwValidInstructionValue, &dwValidParamMax, &pValidParamTypes, &pValidEntryRef)==true)
			{
				// leefix - 250604 - u54 - ONLY if original NAME matches current instruction considered
				if ( pValidEntryRef && *pRef )
				{
					if ( stricmp ( (*pRef)->GetName()->GetStr(), pValidEntryRef->GetName()->GetStr() )!=NULL ) 
					{
						bInValidParams=true;
					}
				}

				// leefix - 170403 - user functions can also have UDTs - so check these match too
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
											g_pErrorReport->SetError(g_pStatementList->GetTokenLineNumber(), ERR_SYNTAX+8, pValidEntryRef->GetName()->GetStr(), pFunctionTypeName->GetStr());
											bInValidParams=true;
											break;
										}
									}
								}

								// UDT arrays not supported
								if ( dwDataType==1101 )
								{
									g_pErrorReport->SetError(g_pStatementList->GetTokenLineNumber(), ERR_SYNTAX+64);
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
		bTypeAFlag=false;
		dwScoreBestMatch=0;
		if(pValidParamTypes)
		{
			if(pFirstParameter)
			{
				if(pValidParamTypes->CheckChars(0,1,"A"))
				{
					// Type A means instructions are repeated with each param (read a,b,c is read a read b read c)
					bInValidParams=false;
					bTypeAFlag=true;
				}
				else
				{
					if(pFirstParameter->ValidateWithCorrectCall(pValidParamTypes, &dwScoreBestMatch, pValidEntryRef->GetHardcoreInternalValue())==false)
					{
						bInValidParams=true;
						//DB3_CRASH();
					}
				}
			}
			else
			{
				if(strcmp(pValidParamTypes->GetStr(),"")==NULL)
				{
					dwScoreBestMatch=9;
				}
				else
				{
					// If no params, and need some, this command no good
					bInValidParams=true;
				}
			}
		}

		// Store instruction that passes all checks
		if(bInValidParams==false)
		{
			// lee - 140206 - u60 - ensure commands with matching return param to initial command wins
			if ( pValidEntryRef->GetReturnParam()==dwOriginalReturnParam )
				dwScoreBestMatch++;

			// if better score, choose this command
			if(dwScoreBestMatch>=dwBestScoreSoFar)
			{
				bBestIsTypeA=bTypeAFlag;
				dwBestScoreSoFar=dwScoreBestMatch;
				*(pdwValidInstructionToUse)=dwValidInstructionValue;
				if(pValidEntryRef) *pRef=pValidEntryRef;
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
		dwTryingIndex++;
		entry = entry->GetNext();
	}

	// Use Best bOneParamPerRepeatedInstruction
	if(pbIfFindTypeA) *pbIfFindTypeA=bBestIsTypeA;

	// Does instruction exist with desired params..
	if(*(pdwValidInstructionToUse)==0)
	{
		// Could not find match soft fail
		DB3_CRASH();
		return false;
	}

	// Complete
	return true;
}

DWORD CStatement::DetermineNameToken(LPSTR pToken)
{
	DWORD dwToken=0;

	// Determine if LOOP
	if(dwToken==0 && stricmp(pToken, "DO")==NULL) dwToken=DOTK;
	if(dwToken==0 && stricmp(pToken, "LOOP")==NULL) dwToken=LOOPTK;
	if(dwToken==0 && stricmp(pToken, "WHILE")==NULL) dwToken=WHILETK;
	if(dwToken==0 && stricmp(pToken, "ENDWHILE")==NULL) dwToken=ENDWHILETK;
	if(dwToken==0 && stricmp(pToken, "REPEAT")==NULL) dwToken=REPEATTK;
	if(dwToken==0 && stricmp(pToken, "UNTIL")==NULL) dwToken=UNTILTK;

	// Determine if FOR NEXT
	if(dwToken==0 && stricmp(pToken, "FOR")==NULL) dwToken=FORTK;
	if(dwToken==0 && stricmp(pToken, "NEXT")==NULL) dwToken=NEXTTK;

	// Determine if USER FUNCTION
	if(dwToken==0 && stricmp(pToken, "FUNCTION")==NULL) dwToken=USERFUNCTIONTK;
	if(dwToken==0 && stricmp(pToken, "EXITFUNCTION")==NULL) dwToken=EXITUSERFUNCTIONTK;
	if(dwToken==0 && stricmp(pToken, "ENDFUNCTION")==NULL) dwToken=ENDUSERFUNCTIONTK;

	// Determine if TERMINATOR
	if(dwToken==0 && stricmp(pToken, "EXIT")==NULL) dwToken=EXITTK;

	// Determine if JUMP
	if(dwToken==0 && stricmp(pToken, "IF")==NULL) dwToken=IFTK;
	if(dwToken==0 && stricmp(pToken, "ELSE")==NULL) dwToken=ELSETK;
	if(dwToken==0 && stricmp(pToken, "ENDIF")==NULL) dwToken=ENDIFTK;
	if(dwToken==0 && stricmp(pToken, "GOTO")==NULL) dwToken=GOTOTK;
	if(dwToken==0 && stricmp(pToken, "GOSUB")==NULL) dwToken=GOSUBTK;
	if(dwToken==0 && stricmp(pToken, "SELECT")==NULL) dwToken=SELECTTK;
	if(dwToken==0 && stricmp(pToken, "ENDSELECT")==NULL) dwToken=ENDSELECTTK;
	if(dwToken==0 && stricmp(pToken, "CASE")==NULL) dwToken=CASETK;
	if(dwToken==0 && stricmp(pToken, "ENDCASE")==NULL) dwToken=ENDCASETK;
	if(dwToken==0 && stricmp(pToken, "END")==NULL) dwToken=ENDTK;

	// Determine if TYPE
	if(dwToken==0 && stricmp(pToken, "TYPE")==NULL) dwToken=TYPETK;
	if(dwToken==0 && stricmp(pToken, "ENDTYPE")==NULL) dwToken=ENDTYPETK;
	if(dwToken==0 && stricmp(pToken, "GLOBAL")==NULL) dwToken=GLOBALTK;
	if(dwToken==0 && stricmp(pToken, "LOCAL")==NULL) dwToken=LOCALTK;
	if(dwToken==0 && stricmp(pToken, "DIM")==NULL) dwToken=DIMTK;
	if(dwToken==0 && stricmp(pToken, "UNDIM")==NULL) dwToken=UNDIMTK;
	if(dwToken==0 && stricmp(pToken, "AS")==NULL) dwToken=ASTK;
	if(dwToken==0 && stricmp(pToken, "BOOLEAN")==NULL) dwToken=BOOLEANTK;
	if(dwToken==0 && stricmp(pToken, "BYTE")==NULL) dwToken=BYTETK;
	if(dwToken==0 && stricmp(pToken, "WORD")==NULL) dwToken=WORDTK;
	if(dwToken==0 && stricmp(pToken, "DWORD")==NULL) dwToken=DWORDTK;
	if(dwToken==0 && stricmp(pToken, "INTEGER")==NULL) dwToken=INTEGERTK;
	if(dwToken==0 && stricmp(pToken, "FLOAT")==NULL) dwToken=FLOATTK;
	if(dwToken==0 && stricmp(pToken, "STRING")==NULL) dwToken=STRINGTK;
	if(dwToken==0 && stricmp(pToken, "DOUBLE")==NULL) dwToken=DOUBLETK;

	// Determine if COMMENTS
	if(dwToken==0 && stricmp(pToken, "REMSTART")==NULL) dwToken=REMSTARTTK;
	if(dwToken==0 && stricmp(pToken, "REM")==NULL) dwToken=REMLINETK;
	if(dwToken==0 && stricmp(pToken, "//")==NULL) dwToken=REMLINETK;
	if(dwToken==0 && stricmp(pToken, "`")==NULL) dwToken=REMLINETK;
	if(dwToken==0 && stricmp(pToken, "'")==NULL) dwToken=REMLINETK;
	if(dwToken==0 && stricmp(pToken, "REMEND")==NULL) dwToken=REMENDTK;
	if(dwToken==0 && stricmp(pToken, "HIDESTART")==NULL) dwToken=REMLINETK;
	if(dwToken==0 && stricmp(pToken, "HIDEEND")==NULL) dwToken=REMLINETK;

	// Determine if DATA Statements
	if(dwToken==0 && stricmp(pToken, "DATA")==NULL) dwToken=DATATK;

	return dwToken;
}

DWORD CStatement::DetermineToken(LPSTR pToken)
{
	// Determine ID from token string
	DWORD dwToken=0;
	if(pToken!=NULL)
	{
		// Determine if CRTK
		if(dwToken==0 && pToken[0]==13 && pToken[1]==10) dwToken=CRTK;

		// Determine if name token
		if(dwToken==0) dwToken=DetermineNameToken(pToken);
	}

	// Ensure pointer within file data
	if(g_pStatementList->GetFileDataPointer()>=g_pStatementList->GetFileDataEnd()-2)
	{
		dwToken=ENDTK;
	}

	// Return with token value result if any
	return dwToken;
}

bool CStatement::DetermineIfReservedWord(LPSTR pWord)
{
	// Eliminate tokens
	if ( DetermineNameToken ( pWord )!=NULL )
		return true;

	// Other internal reserved words (please rewrite me!)
	if ( stricmp(pWord, "NEXT")==NULL ) return true;
	if ( stricmp(pWord, "STEP")==NULL ) return true;
	if ( stricmp(pWord, "TO")==NULL ) return true;
	if ( stricmp(pWord, "THEN")==NULL ) return true;
	if ( stricmp(pWord, "SYNC")==NULL ) return true;
	if ( stricmp(pWord, "RETURN")==NULL ) return true;
	if ( stricmp(pWord, "END")==NULL ) return true;

	// not a reserved word
	return false;
}

bool CStatement::DetermineIfFunctionName(LPSTR pWord, bool bIncludeUserFunctions)
{
	// Check if a recognised instruction
	CInstructionTableEntry* pRef=NULL;
	DWORD dwTokenData=0, dwParamMax=0, dwLength=0;
	if(g_pInstructionTable->FindInstruction(false, pWord, 0, &dwTokenData, &dwParamMax, &dwLength, &pRef))
		return true;

	// Check if a recognised user function
	if(bIncludeUserFunctions)
		if(g_pInstructionTable->FindUserFunction( pWord, 0, &dwTokenData, &dwParamMax, &dwLength))
			return true;

	// not a function name
	return false;
}

// U73 - 270309 - used to fill in gaps where line number DBM output missed (ENDFUNCTION)
DWORD g_dwLastDBMLineNumber = 1;

#define __AARON_DBMPERF__ 1
#define MAX_SCRATCH_BUFFER 65536

bool CStatement::WriteDBM(void)
{
#ifdef __AARON_DBMPERF__
	static char s_Buf[MAX_SCRATCH_BUFFER];
#endif
	// Place Description of Program Line Here
	if(m_dwLineNumber>0 && (m_pParameters || GetObjectClass()))
	{
		// U73 - 270309 - Paste any lines that may have been missed (ENDFUNCTION lines usually are)
		WriteDBMBit(0, "", "");
		while ( g_dwLastDBMLineNumber <= m_dwLineNumber )
		{
#ifdef __AARON_DBMPERF__
			g_pStatementList->GetLineText(g_dwLastDBMLineNumber, s_Buf, sizeof(s_Buf));
			WriteDBMBit(g_dwLastDBMLineNumber, "LINE : ", s_Buf);
			WriteDBMBit(0, "", "");
#else
			LPSTR pProgramLineText=NULL;
			g_pStatementList->FindStartOfFileDataProgramLine(g_dwLastDBMLineNumber, &pProgramLineText);
			WriteDBMBit(g_dwLastDBMLineNumber, "LINE : ", pProgramLineText);
			SAFE_DELETE(pProgramLineText);
			WriteDBMBit(0, "", "");
#endif

			// catch up to current line, which is written next..
			g_dwLastDBMLineNumber++;
		}

		// If Debug Mode, generate DEBUGHOOK
		if(m_bPerformJumpChecks)
		{
			if(g_DebugInfo.DebugModeOn())
			{
				// Check if breakpoint needs to be inserted here
				if(g_pDBPCompiler->GetBreakPointMax()>0)
				{
					DWORD nIndex=g_pDBPCompiler->GetBreakPointIndex();
					if(g_pDBPCompiler->GetBreakPointLine(nIndex)<=m_dwLineNumber && nIndex<g_pDBPCompiler->GetBreakPointMax())
					{
						// If breakpoint, force an escape into debugger
						g_pASMWriter->WriteASMForceEscapeAtCodeBREAK();
						g_pASMWriter->WriteASMComment("BREAKPOINT ESCAPE", "", "", "");
						
						// Prepare for next breakpoint
						while(g_pDBPCompiler->GetBreakPointLine(nIndex)<=m_dwLineNumber && nIndex<g_pDBPCompiler->GetBreakPointMax())
						{
							g_pDBPCompiler->IncBreakPointIndex();
							nIndex=g_pDBPCompiler->GetBreakPointIndex();
						}
					}
				}

				// If Running At Full Speed, skip debug hook
				g_pASMWriter->WriteASMLeapForwardMarker();

				// Record LEAP-BACK Position Here
				g_pASMWriter->WriteASMLeapMarkerTop();

				// Also write SYNC to keep App Window Alive (for window move, close, etc)
				CParseInstruction* pTempInstr = new CParseInstruction();
				pTempInstr->SetLineNumber(m_dwLineNumber);
				pTempInstr->PassStartEndCharForPossibleDebugHook(m_dwStartChar, m_dwEndChar);
				pTempInstr->WriteDBMHardCode(BUILD_SYNC, NULL, NULL, NULL);
				delete pTempInstr;

				// statement should contain actual char data!!!
				int iLineNumber=m_dwLineNumber;
				int iStartChar=m_dwStartChar;
				int iEndChar=m_dwEndChar;

				// Write Debug Hook
				g_pASMWriter->WriteASMTaskCoreP2(iLineNumber, ASMTASK_DEBUGSTATEMENTHOOK, NULL, iStartChar, NULL, iEndChar);

				// If EAX is zero, leap back
				g_pASMWriter->WriteASMLeapMarkerJumpToTop();

				// Complete LEAP-FORWARD Marker
				g_pASMWriter->WriteASMLeapMarkerEnd(0);
			}
		}
	}

	// Progress Reporting Tool
	g_pErrorReport->ProgressReport("Builder now at line ",m_dwLineNumber);

	// Update label byte offset
	g_pStatementList->UpdateLineDBMData(this);

	// Proceed to write specific DBM Code
	switch(GetObjectType())
	{
		case 0 :	// Blank Object
					if(m_pParameters)
					{
						// Sometimes contains inter-code (like fornextinitcode)
						m_pParameters->WriteDBM();
					}
					break;

		case 1 :	// LOOP Object
					{
						// LOOP OBJECT
						CStatement* pStatementToUse = this;
						CParseLoop* pLoop = (CParseLoop*)pStatementToUse->GetObjectClass();

						// TOP OF LOOP
						pLoop->PassStartEndCharForPossibleDebugHook(m_dwStartChar, m_dwEndChar);
						pLoop->WriteDBM(DBMPLACEMENT_TOP);

						// LOOP CONTENTS
						pLoop->GetBlock()->WriteDBM();

						// END OF LOOP
						pLoop->WriteDBM(DBMPLACEMENT_BOTTOM);
					}
					break;

		case 2 :	// TYPE Object
					{
						// TYPE OBJECT
						CStatement* pStatementToUse = this;
						CParseType* pType = (CParseType*)pStatementToUse->GetObjectClass();
						pType->WriteDBM();
					}
					break;

		case 3 :	// DECLARATION Object
					{
						// DECLARATION OBJECT (Init Part)
						CStatement* pStatementToUse = this;
						CParseInit* pInit = (CParseInit*)pStatementToUse->GetObjectClass();
						pInit->WriteDBM();
					}
					break;

		case 6 :	// USERFUNCTION Object
					{
						// USERFUNCTION OBJECT
						CStatement* pStatementToUse = this;
						CParseUserFunction* pUserFunction = (CParseUserFunction*)pStatementToUse->GetObjectClass();

						// TOP OF USERFUNCTION
						pUserFunction->WriteDBM(DBMPLACEMENT_TOP);

						// USERFUNCTION CONTENTS
						pUserFunction->GetBlock()->WriteDBM();

						// END OF USERFUNCTION
						pUserFunction->WriteDBM(DBMPLACEMENT_BOTTOM);
					}
					break;

		case 8 :	// JUMP Object
					{
						// JUMP OBJECT
						CStatement* pStatementToUse = this;
						CParseJump* pJump = (CParseJump*)pStatementToUse->GetObjectClass();

						// TOP 
						pJump->WriteDBM(DBMPLACEMENT_TOP);
					}
					break;

		case 11 :	// INSTRUCTION Object
					{
						CStatement* pStatementToUse = this;
						CParseInstruction* pInstruction = (CParseInstruction*)pStatementToUse->GetObjectClass();
						pInstruction->PassStartEndCharForPossibleDebugHook(m_dwStartChar, m_dwEndChar);

						// FUNCTION PRE-CALL CODE
						if(pInstruction->WriteDBM()==false) return false;
					}
					break;

		case 12 :	// FUNCTION Object
					{
						CStatement* pStatementToUse = this;
						CParseFunction* pFunction = (CParseFunction*)pStatementToUse->GetObjectClass();

						// FUNCTION PRE-CALL CODE
						pFunction->WriteDBM();
					}
					break;

		case 999 :	// COMMENT (Debug) Object
					break;
	}

	// lee - 240306 - u6b4 - unconventional flag for reversing a condition needs shutting down after instruction parse
	if ( g_pASMWriter ) g_pASMWriter->SetCondToggle(false);

	// For Valid Lines/Statements
	if(GetObjectType()!=6)
	{
		// leefix - 120108 - U71 - dont need RT hook after end of function (and solves line number issue in DBM)
		if(m_dwLineNumber>0 && (m_pParameters || GetObjectClass()))
		{
			// If RuntimeErrorOn Mode, generate RUNTIMEERRORJUMP
			if(m_bPerformJumpChecks && g_pDBPCompiler->GetRuntimeErrorMode())
			{
				// Write Runtime Error Jump Hook
				g_pASMWriter->WriteASMTaskCoreP2(m_dwLineNumber, ASMTASK_RUNTIMEERRORHOOK, NULL, 0, NULL, 0);
			}
		}
	}

	// Also write next statement
	if(GetNext()) GetNext()->WriteDBM();

	// Complete
	return true;
}

#define __AARON_DBMPERF__ 1
#define DBM_MAX_SCRATCH_BUF 65536

bool CStatement::WriteDBMBit(DWORD dwLineNumber, LPSTR pText, LPSTR pResult)
{
#ifdef __AARON_DBMPERF__
	static char buf[DBM_MAX_SCRATCH_BUF];
	int len;

	len = sprintf_s(buf, "   %u %s%s", static_cast<unsigned int>(dwLineNumber), pText, pResult);
	if (len < 0)
		return false;

	return g_pDBMWriter->OutputDBM(buf, static_cast<size_t>(len));
#else
	// Write out text
	CStr strDBMLine("");
	strDBMLine.SetText("   ");
	if(dwLineNumber>0) strDBMLine.AddNumericText(dwLineNumber);
	strDBMLine.AddText(" ");
	strDBMLine.AddText(pText);
	strDBMLine.AddText(pResult);

	//DB3_CRASH_MSG(strDBMLine.GetStr());

	return g_pDBMWriter->OutputDBM(&strDBMLine);
#endif
}
