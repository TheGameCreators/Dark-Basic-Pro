// ParseUserFunction.cpp: implementation of the CParseUserFunction class.
//
//////////////////////////////////////////////////////////////////////

// Common Includes
#include "VarTable.h"
#include "StructTable.h"
#include "InstructionTable.h"
#include "ParseUserFunction.h"

// External Class Pointer
extern CVarTable* g_pVarTable;
extern CStructTable* g_pStructTable;
extern CInstructionTable* g_pInstructionTable;

// Internal Scope Ptrs (keep track of function within)
CParseUserFunction* g_pUserFunctionWithin = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParseUserFunction::CParseUserFunction()
{
	m_dwStartLineNumber=0;
	m_dwEndLineNumber=0;

	m_pName=NULL;
	m_dwParamMax=0;
	m_pCodeBlock=NULL;
	m_pParameter=NULL;

	// Reference Pointer Only
	m_pDecChainRef=NULL;
}

CParseUserFunction::~CParseUserFunction()
{
	// Statements Deleted One by One
	m_pCodeBlock->Free();
	m_pCodeBlock=NULL;

	SAFE_DELETE(m_pName);
	SAFE_DELETE(m_pParameter);
}

bool CParseUserFunction::ActOnSingleVar(DWORD dwType, int iDisplacement, DWORD PlacementCode, CStr* pDoNotFree, bool bSpecialRecreate)
{
	// Only strings or arrays
	// LEEFIX - 121102 - Limited dwType to 101-199 as nontypes where being treated as releasables in the function
	if((dwType>100 && dwType<200) || dwType==3)
	{
		// Write offset out
		CStr* pData = new CStr("");
		pData->SetText("@:");
		pData->AddNumericText(iDisplacement);

		// Local Var - not part of param-in data
		CStr* pNull = new CStr("0");
		if(PlacementCode==DBMPLACEMENT_TOP)
		{
			// Special Recreate creates a new string from current ptr address
			if(bSpecialRecreate==true)
			{
				// Used for strings passed into functions (need to be new instances)

				// Pass DEST + CURRENT STRING (same address)
				CStr* pNull = new CStr("0");
				g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_PUSH, pData, 7);
				g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_PUSH, pNull, 7);
				SAFE_DELETE(pNull);

				// CALL EQUATE to create a NEW STRING from CURRENT STRING
				g_pASMWriter->WriteASMCall(GetStartLineNumber(), "dbprocore.dll", "?EquateSS@@YAKKK@Z");

				// Put EAX overwrites DEST
				g_pASMWriter->WriteASMTaskCoreP2(GetStartLineNumber(), ASMTASK_ASSIGN, pData, 7, NULL, 7);
			}
			else
			{
				// Clear func-mem
				//g_pASMWriter->WriteASMTaskCoreP2(GetEndLineNumber(), ASMTASK_ASSIGN, pData, 7, pNull, 7); //120108 - u71 - fix line number (and above)
				g_pASMWriter->WriteASMTaskCoreP2(GetStartLineNumber(), ASMTASK_ASSIGN, pData, 7, pNull, 7);
			}
		}
		if(PlacementCode==DBMPLACEMENT_BOTTOM)
		{
			// Free func-mem (except 'any' return string)
			bool bValidFree=true;
			if(pDoNotFree)
				if(stricmp(pDoNotFree->GetStr(), pData->GetStr())==NULL)
					bValidFree=false;

			// Only the return string is not freed here
			if(bValidFree==true)
			{
				g_pASMWriter->WriteASMTaskCoreP1(GetEndLineNumber(), ASMTASK_PUSH, pData, 7);

				CInstructionTableEntry* pRef = NULL;
				if(dwType==3) pRef=g_pInstructionTable->GetRef(IT_INTERNAL_STRFREE);
				if(dwType>100 && dwType<1000) pRef=g_pInstructionTable->GetRef(IT_INTERNAL_FREE);
				if(dwType==1001) pRef=g_pInstructionTable->GetRef(IT_INTERNAL_STRFREE);
				if(dwType==1101) pRef=g_pInstructionTable->GetRef(IT_INTERNAL_FREE);
				
				LPSTR pMathCommand=pRef->GetDecoratedName()->GetStr();
				g_pASMWriter->WriteASMCall(GetEndLineNumber(), "dbprocore.dll", pMathCommand);
				g_pASMWriter->WriteASMTaskCoreP1(GetEndLineNumber(), ASMTASK_POPEBX, NULL, 0);
			}
		}

		// Free usages
		SAFE_DELETE(pNull);
		SAFE_DELETE(pData);
	}

	// Complete
	return true;
}

bool CParseUserFunction::ActOnType(CStr* pTypeName, int iDisplacement, DWORD PlacementCode, CStr* pDoNotFree, bool bSpecialRecreate)
{
	int iGlobalDisplacement = iDisplacement;
	CStructTable* pStruct = g_pStructTable->DoesTypeEvenExist(pTypeName->GetStr());
	if(pStruct)
	{
		CDeclaration* pCurrent = pStruct->GetDecChain();
		while(pCurrent)
		{
			// Determine local func-mem from local var name
			DWORD dwOffset=0, dwSizeOfData=0;
			if(g_pStructTable->FindOffsetFromField(pTypeName->GetStr(), pCurrent->GetName()->GetStr(), &dwOffset, &dwSizeOfData))
			{
				// Determine type of Field
				DWORD dwFieldType = g_pVarTable->GetBasicTypeValue(pCurrent->GetType()->GetStr());

				// Calculate displacement to Field
				int iDisplacement = (iGlobalDisplacement + dwOffset);
		
				// Process as basic or user type
				if(dwFieldType==1001)
				{
					// Field is a user type
					ActOnType(pCurrent->GetType(), iDisplacement, PlacementCode, NULL, bSpecialRecreate);
				}
				else
				{
					// Field is a basic type
					ActOnSingleVar(dwFieldType, iDisplacement, PlacementCode, NULL, bSpecialRecreate);
				}
			}

			// Next declaration
			pCurrent = pCurrent->GetNext();
		}
	}

	// Complete
	return true;
}

bool CParseUserFunction::ActOnLocalVars(DWORD PlacementCode, CStr* pDoNotFree)
{
	// Determine Size of parameters passed onto stack
	CDeclaration* pCurrent = GetDecChainRef();
	if(GetParamMax()>0) for(DWORD n=0; n<GetParamMax()-1; n++) pCurrent=pCurrent->GetNext();
	DWORD dOffsetToLastParamInStruct = pCurrent->GetOffset();

	// Determine Size of parameters passed onto stack
	DWORD dwTotalFunctionDataSize = g_pStructTable->GetSizeOfType(GetName()->GetStr());
	DWORD dwSizeOfLocalParams = dwTotalFunctionDataSize - dOffsetToLastParamInStruct;

	// Zero ALL elements of local vars for nice function
	if(PlacementCode==DBMPLACEMENT_TOP)
	{
		// ASM Task to clear current ESP position -> 
		CStr* pClearSize = new CStr();
		pClearSize->SetNumericText(dwSizeOfLocalParams);
		g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_CLEARSTACK, pClearSize, 7);
		SAFE_DELETE(pClearSize);
	}

	// Clear all local vars that are string and array pointers
	pCurrent = GetDecChainRef();
	while(pCurrent)
	{
		// LEEFIX - 111002 - Strings passed in are not handled correctly - they use the passed in ptr changing rhe original string or crashes
		// ...
		// -4 Temp/Function Variables (ONLY process these for strings and arrays)
		// 00 EBP
		// +8 Parameters Passed In (Except STRINGS in the PARAM part of DEC Chain must be re-created/freed locally)
		// ...

		// Determine type of Local Variable
		DWORD dwDecType = g_pVarTable->GetBasicTypeValue(pCurrent->GetType()->GetStr());
		if(pCurrent->GetArrFlag()==1) dwDecType+=100;

		// Default is no clearing
		bool bActOnThisParam=false;
		bool bSpecialRecreate=false;

		// If regular temp/function local variable
		if(pCurrent->GetOffset()>dOffsetToLastParamInStruct)
		{
			bActOnThisParam=true;
		}

		// If param input is string (needs special re-create / free code)
		if(pCurrent->GetOffset()<=dOffsetToLastParamInStruct)
		{
			// strings AND UDTvars (leefix-170403-types supported in user functions)
			if ( dwDecType==3 || dwDecType==1001 )
			{
				bActOnThisParam=true;
				bSpecialRecreate=true;
			}
		}

		// Do This One
		if(bActOnThisParam==true)
		{
			// Determine local func-mem from local var name
			DWORD dwOffset=0, dwSizeOfData=0;
			if(g_pStructTable->FindOffsetFromField(GetName()->GetStr(), pCurrent->GetName()->GetStr(), &dwOffset, &dwSizeOfData))
			{
				// Calculate displacement to Local Variable (or ParamInputVar)
				int iDisplacement;
				if(bSpecialRecreate==false)
					iDisplacement=((dOffsetToLastParamInStruct)-dwOffset)-dwSizeOfData;
				else
					iDisplacement=4+dwOffset;

				// Process as basic or user type
				if(dwDecType==1001)
				{
					// Local Variable is a user type
					ActOnType(pCurrent->GetType(), iDisplacement, PlacementCode, pDoNotFree, bSpecialRecreate);
				}
				else
				{
					// Local Variable is a basic type
					ActOnSingleVar(dwDecType, iDisplacement, PlacementCode, pDoNotFree, bSpecialRecreate);
				}
			}
		}
		pCurrent=pCurrent->GetNext();
	}
	
	// Complete
	return true;
}

bool CParseUserFunction::WriteDBM(DWORD PlacementCode)
{
	// Use parameters if valid
	if(PlacementCode==DBMPLACEMENT_TOP)
	{
		// Store EBP Register
		g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_PUSHEBP, NULL, 0);

		// Copy ESP to EBP Register
		g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_MOVEBPESP, NULL, 0);

		// Advance ESP Stack Register to skip 'local function space'
		CStr* pString = new CStr(GetName()->GetStr());
		DWORD dwTypeSize=g_pStructTable->GetSizeOfType(pString->GetStr());
		CStr* pStrNum = new CStr("");
		pStrNum->SetNumericText(dwTypeSize+4);//extra 4 bytes as start adds some for byte start
		g_pASMWriter->WriteASMTaskCoreP1(GetStartLineNumber(), ASMTASK_SUBESP, pStrNum, 7);
		SAFE_DELETE(pString);
		SAFE_DELETE(pStrNum);

		// Clear all vars, and especially strings and array ptrs (and those in usertypes too)
		ActOnLocalVars(PlacementCode, NULL);

		// Now within function
		g_pUserFunctionWithin = this;
	}
	if(PlacementCode==DBMPLACEMENT_BOTTOM)
	{
		// Establish Return Param
		CStr* pReturnData = NULL;
		DWORD dwReturnDataType = 0;
		if(GetResultParameter())
		{
			pReturnData = GetResultParameter()->GetMathItem()->FindResultStringTokenForDBM();
			dwReturnDataType = GetResultParameter()->GetMathItem()->FindResultTypeValueForDBM();
		}

		// check if returned string is global
		bool bDuplicateReturnString=false;
		CResultData* pResultData = NULL;
		if(GetResultParameter())
		{
			pResultData = GetResultParameter()->GetMathItem()->FindResultDataForDBM();
			if ( dwReturnDataType==3 || dwReturnDataType==103 )
			{
				DWORD dwArr=0;
				if ( dwReturnDataType==103 ) dwArr=1;
				CVarTable* pVar = g_pVarTable->FindVariable ( "", pReturnData->GetStr()+1, dwArr );
				if ( pVar )
				{
					// return is a global var or array string, must create a duplicate string to send back
					bDuplicateReturnString=true;
				}
			}
		}

		// Free any allocated dynamic memory (strings and arrays)
		ActOnLocalVars(PlacementCode, pReturnData);

		// If user function returns value, store in EAX/EDX/ST0
		if(GetResultParameter())
		{
			// Direct or 'Duplicate copy of string'
			if ( bDuplicateReturnString )
			{
				// Source global string array
				g_pASMWriter->WriteASMTaskP1(GetEndLineNumber(), ASMTASK_PUSH, pResultData);

				// Blank String - no thing to free
				CStr* pNull = new CStr("0");
				g_pASMWriter->WriteASMTaskCoreP1(GetEndLineNumber(), ASMTASK_PUSH, pNull, 7);
				SAFE_DELETE(pNull);

				// Put new string address in EAX for return passing
				g_pASMWriter->WriteASMCall(GetEndLineNumber(), "dbprocore.dll", "?EquateSS@@YAKKK@Z");
			}
			else
			{
				// direct copy of var ref to outside EAX
				g_pASMWriter->WriteASMTaskP1(GetEndLineNumber(), ASMTASK_ASSIGNTOEAX, GetResultParameter()->GetMathItem()->FindResultData());
			}
		}

		// Restore ESP from EBP Register
		g_pASMWriter->WriteASMTaskCoreP1(GetEndLineNumber(), ASMTASK_MOVESPEBP, NULL, 0);

		// Restore EBP Register
		g_pASMWriter->WriteASMTaskCoreP1(GetEndLineNumber(), ASMTASK_POPEBP, NULL, 0);

		// RETurn
		g_pASMWriter->WriteASMTaskCoreP1(GetEndLineNumber(), ASMTASK_PURERETURN, NULL, 0);

		// Not within function
		g_pUserFunctionWithin = NULL;
	}
	return true;
}

bool CParseUserFunction::WriteDBMBit(DWORD dwLineNumber, LPSTR pText, LPSTR pResult)
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
