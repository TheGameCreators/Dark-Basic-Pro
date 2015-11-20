// Parameter.cpp: implementation of the CParameter class.
//
//////////////////////////////////////////////////////////////////////
#include "InstructionTable.h"
#include "ParserHeader.h"
#include "DataTable.h"
#include "VarTable.h"

// External Class Pointer
extern CVarTable* g_pVarTable;
extern CDataTable* g_pStringTable;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParameter::CParameter()
{
	m_pMainMathOp=NULL;
	m_pNext=NULL;
	m_pPrev=NULL;
}

CParameter::~CParameter()
{
	SAFE_DELETE(m_pMainMathOp);
	SAFE_DELETE(m_pNext);
}

void CParameter::Add(CParameter* pParameter)
{
	if(m_pNext==NULL)
	{
		m_pNext=pParameter;
		pParameter->m_pPrev=this;
	}
	else
		m_pNext->Add(pParameter);
}

bool CParameter::MakeParamList(CStr* pStrList)
{
	// Find result from math item
	CMathOp* pMathOpFind = GetMathItem();
	if(pMathOpFind)
	{
		// Find end of nest
		while(pMathOpFind->GetNext())
		{
			pMathOpFind=pMathOpFind->GetNext();
		}

		// Start or add with comma to list
		if(pStrList->Length()>0)
		{
			pStrList->AddText(",");
			pStrList->AddText(pMathOpFind->GetResultStringToken());
		}
		else
		{
			pStrList->SetText(pMathOpFind->GetResultStringToken());
		}
	}

	// Do next parameter in list
	if(GetNext())
	{
		if(GetNext()->MakeParamList(pStrList)==false)
		{
			return false;
		}
	}

	return true;
}

bool CParameter::ValidateWithCorrectCall(CStr* pValidParamTypes, DWORD* pdwScore, DWORD dwInternalCode)
{
	DWORD dwParamCount = pValidParamTypes->Length();
	DWORD dwPtr=0;
	DWORD dwParamsProcessed=0;
	CParameter* pCurrent = this;
	DWORD dwTotalScore=0;

	DWORD dwThisScore=0;
	DWORD dwArrFlag=0;
	DWORD dwTypeRequired=0;
	DWORD dwArrayPtrRequired=0;
	DWORD dwParsedType=0;
	CStr* pMathString=nullptr;
	LPSTR pVarString=nullptr;
	CStr* pFindVarName=nullptr;
	LPSTR ReturnTypeString=nullptr;
	while(pCurrent)
	{
		// Determine Required Type
		dwThisScore=0;
		dwArrFlag=0;
		dwTypeRequired=0;
		dwArrayPtrRequired=0;
		if(pValidParamTypes->GetChar(dwPtr)=='L') dwTypeRequired=1;
		if(pValidParamTypes->GetChar(dwPtr)=='F') dwTypeRequired=2;
		if(pValidParamTypes->GetChar(dwPtr)=='S') dwTypeRequired=3;
		if(pValidParamTypes->GetChar(dwPtr)=='B') dwTypeRequired=4;
		if(pValidParamTypes->GetChar(dwPtr)=='Y') dwTypeRequired=5;
		if(pValidParamTypes->GetChar(dwPtr)=='W') dwTypeRequired=6;
		if(pValidParamTypes->GetChar(dwPtr)=='D') dwTypeRequired=7;
		if(pValidParamTypes->GetChar(dwPtr)=='O') dwTypeRequired=8;
		if(pValidParamTypes->GetChar(dwPtr)=='R') dwTypeRequired=9;
		if(pValidParamTypes->GetChar(dwPtr)=='P') dwTypeRequired=10;
		if(pValidParamTypes->GetChar(dwPtr)=='Q') dwTypeRequired=20;
		if(pValidParamTypes->GetChar(dwPtr)=='X') dwTypeRequired=501;
		if(pValidParamTypes->GetChar(dwPtr)=='H') { dwTypeRequired=7; dwArrayPtrRequired=1; }
		if(pValidParamTypes->GetChar(dwPtr)=='E') dwTypeRequired=1001;

		// If may need casting..
		dwParsedType=0;
		if(dwTypeRequired==501)
		{
			// No cast Required
			dwParsedType = dwTypeRequired;
			dwThisScore=8;
		}
		else
		{
			// Get Math Item
			if ( pCurrent->GetMathItem() )
			{
				// Get Math String
				pMathString = pCurrent->GetMathItem()->FindResultStringTokenForDBM();

				// Determine Parsed Type 
				if(pCurrent->GetMathItem()->IsLabel(pMathString))
				{
					// Confirmed Label
					dwParsedType=10;
					if(dwTypeRequired==20) dwParsedType=20;
					dwThisScore=10;
				}
				if(pMathString->IsTextNumericValue())
				{
					// Float
					dwParsedType=2;
					dwThisScore=10;
				}
				if(pMathString->IsTextIntegerOnlyValue()
				|| pMathString->IsTextHexValue()
				|| pMathString->IsTextBinaryValue()
				|| pMathString->IsTextOctalValue())
				{
					// Can cast integer literals into floats if required
					if(dwTypeRequired==2)
					{
						dwParsedType=2;
						dwThisScore=9;
					}
					else
					{
						// Integer
						dwParsedType=1;
						dwThisScore=10;
					}
				}
				if(pMathString->IsTextSpeechMarked())
				{
					// String
					dwParsedType=3;
					dwThisScore=10;
				}
				if(dwParsedType==0)
				{
					// Not a Literal, is it a variable (skip @ or $ with +1)
					pVarString=pMathString->GetStr();
					if(*pVarString=='@' || (*pVarString=='$' && *(pVarString+1)!='$')
					|| pMathString->CheckChars(0,3,"FS@"))
					{
						// Skip var token
						pFindVarName = NULL;
						if(*pVarString=='@') pVarString++;
						if(pMathString->CheckChars(0,3,"FS@"))
						{
							pFindVarName = new CStr(pMathString->GetStr()+3);
							pVarString = pFindVarName->GetStr() + pFindVarName->FindFirstChar('@') + 1;
						}
						if(*pVarString=='&') dwArrFlag=1;

						// Check if variable exists
						if(g_pVarTable->FindVariableExist(pVarString, dwArrFlag))
						{
							ReturnTypeString=NULL;
							if(g_pVarTable->FindTypeOfVariable(pVarString, dwArrFlag, &ReturnTypeString))
							{
								// Determine var type
								dwParsedType=g_pVarTable->GetBasicTypeValue(ReturnTypeString);
								if(dwParsedType==1001)
								{
									// leefix - 280603 - If Array Ptr Required, use DWORD here
									if ( dwArrayPtrRequired==1 )
									{
										// Type 'H' means to obtain array ptr only
										dwParsedType=7;
									}
									else
									{
										// Seek ultimate type inside usertype
										dwParsedType=pCurrent->GetMathItem()->FindResultData()->m_dwType;
									}
								}
								else
								{
									// Pointers to datatypes take on datatype value for passing-in
									if(dwArrFlag==1 && dwTypeRequired==10) dwParsedType=10;
								}
								if(dwParsedType%1000>100) dwParsedType-=100;
								if(dwTypeRequired==dwParsedType) dwThisScore=10;
								SAFE_DELETE(ReturnTypeString);
							}
						}

						// Free usages
						SAFE_DELETE(pFindVarName);
					}
					if(pMathString->Length()>1)
					{
						pVarString=pMathString->GetStr();
						if(*pVarString=='$' && *(pVarString+1)=='$')
						{
							// Skip immstring tokens and check if it exists
							pVarString+=2;
							if(g_pStringTable->FindIndexStr(pVarString))
							{
								dwParsedType=3;
								dwThisScore=10;
							}
						}
					}
				}
			}
		}

		// If INC/DEC, first param MUST be match (cannot cast the var!)
		if(dwInternalCode==IT_INTERNAL_INCVAR)
		{
			if(dwTypeRequired!=dwParsedType && dwParamsProcessed==0)
			{
				// Parameter does not match at this character soft fail
				//DB3_CRASH();
				return false;
			}
		}
		if(dwInternalCode==IT_INTERNAL_DECVAR)
		{
			if(dwTypeRequired!=dwParsedType && dwParamsProcessed==0)
			{
				// Parameter does not match at this character soft fail
				//DB3_CRASH();
				return false;
			}
		}

		// CAST EXCPETIONS: Do not do strings and pointers, rest can be cast
		if(dwTypeRequired!=dwParsedType)
		{
			if(dwParsedType!=3 && dwParsedType!=10 && dwTypeRequired!=3 && dwTypeRequired!=10)
			{
				switch(dwParsedType)
				{
					case 1 : 
					case 4 : 
					case 5 : 
					case 6 : 
					case 7 : 
					case 9 :	// Non-Floats
								dwParsedType=dwTypeRequired;
								if(dwTypeRequired==2 || dwTypeRequired==8)
								{
									dwThisScore=8;
								}
								else
								{
									dwThisScore=9;
								}
								break;
					case 2 : 
					case 8 :	// Floats
								dwParsedType=dwTypeRequired;
								if(dwTypeRequired==2 || dwTypeRequired==8)
								{
									dwThisScore=9;
								}
								else
								{
									dwThisScore=8;
								}
								break;
				}
			}
			else
			{
				// There are cases where string array can be assigned to DWORD Ptrs (save array f,X)..allow for that
				if(dwTypeRequired==7 && dwParsedType==3 && dwArrFlag==1)
				{
					// Permit string to DWORD
					dwTypeRequired=dwParsedType;
				}
			}
		}

		// If parameter does not match required
		if(dwParsedType!=dwTypeRequired)
		{
			// Parameter does not match at this character soft fail
			return false;
		}

		// Next parameter
		pCurrent = pCurrent->GetNext();

		// Next character
		dwPtr++;

		// If index exceeds length of param, exit with false
		if(dwPtr>pValidParamTypes->Length())
		{
			// soft failure
			return false;
		}

		// Count processed param items
		dwTotalScore=dwTotalScore+dwThisScore;
		dwParamsProcessed++;
	}

	// Adjust final score by iterations (an exact match for two params would be 20 (20/2)=10)
	if(dwParamsProcessed>0) dwTotalScore=dwTotalScore/dwParamsProcessed;
	*pdwScore=dwTotalScore;

	// If params match, but there are too few or too many, also not a match
	if(dwParamCount==dwParamsProcessed
	||(dwParamCount==dwParamsProcessed+1 && pValidParamTypes->GetChar(dwParamCount-1)=='*'))
	{
		// patameter count match ok (as * symbol is added to input params and does not count)
	}
	else
	{
		// soft failure
		//DB3_CRASH();
		return false;
	}

	// Complete
	return true;
}

bool CParameter::CastAllParametersToInstruction(CInstructionTableEntry* pRef)
{
	// Run through parameters of validated user function and make any required casts
	CStr* pSuggestedParamTypes = pRef->GetParamTypes();
	if(pSuggestedParamTypes->GetChar(0)!='A')
	{
		if(pSuggestedParamTypes->GetChar(0)=='Q')
		{
			CMathOp* pCurrent = this->GetMathItem();
			pCurrent->SetResultType(20);
		}
		else
		{
			// Make sure all parameters are casted to instruction requirements
			DWORD dwPPos=0;
			CParameter* pCheckParam=this;
			DWORD dwRequiredTypeValue = g_pVarTable->GetTypeValueOfChar(pSuggestedParamTypes->GetChar(dwPPos));
			while(pCheckParam)
			{
				// Instruction Needs..
				DWORD dwNextRequiredTypeValue = g_pVarTable->GetTypeValueOfChar(pSuggestedParamTypes->GetChar(dwPPos));
				if(dwNextRequiredTypeValue>0) dwRequiredTypeValue=dwNextRequiredTypeValue;

				// Special H Param GRABS the array address directly by casting the array using Type=7
				bool bForceParamToTheArrayAddress=false;
				if(pSuggestedParamTypes->GetChar(dwPPos)=='H')
				{
					dwRequiredTypeValue=0;
					bForceParamToTheArrayAddress=true;
				}

				// Get Root Math Op
				CMathOp* pCurrent = pCheckParam->GetMathItem();

				// Find Result of Math
				while(pCurrent->GetNext()) pCurrent=pCurrent->GetNext();

				// Cases where casting not required
				bool bCastingRequired=true;

				// Type501 is any-type and does not need casting
				if(pCurrent->GetResultType()==501) bCastingRequired=false;

				// leefix - 280305 - casting IS required as cannot PRINT *PTR without casting to what PRINT wants
				if(pCurrent->GetResultType()>=201 && pCurrent->GetResultType()<=299)
				{
					int lee=42;
					//bCastingRequired=false;
				}

				// Do not cast a UDT variable
				if(pCurrent->GetResultType()==1001) bCastingRequired=false;

				// No need to cast if type is an array of require type
				if(pCurrent->GetResultType()>=100 && pCurrent->GetResultType()-100==dwRequiredTypeValue) bCastingRequired=false;

				// Casting must be performed
				if(bCastingRequired)
				{
// THIS FORCES ANY ARRAY TO USE TYPE=7 (ARRAY PTR ADDRESS) - Commented out as RGBR ( array(1) ) would not access the array (TYPE=101)
//					// Special case where D is an instruction param type (all array types are assumed as array ptr accesses)
//					if(dwRequiredTypeValue==7 && pCurrent->GetResultType()>=101 && pCurrent->GetResultType()<=109)
					if(bForceParamToTheArrayAddress==true)
					{
						// No cast, source type is reassigned as a DWORD (Pointer to address rather than contents)
						pCurrent->GetResultData()->m_dwType=7;
					}
					else
					{
						// Check With what I have..
						// LEEFIX - 121102 - Seperated OffsetLValue from && chain with an || for better logic
						if((pCurrent->GetResultType()!=dwRequiredTypeValue
						&&  pCurrent->GetResultType()!=100+dwRequiredTypeValue)
						||  pCurrent->GetResultOffsetLValueTypeValue()!=dwRequiredTypeValue)
						{
							// Create New Math Op to Cast Value
							DWORD StatementLineNumber = g_pStatementList->GetTokenLineNumber();
							CMathOp* pValueToCast = pCheckParam->GetMathItem();
							pValueToCast->SetLineNumber(StatementLineNumber);
							if(pCheckParam->GetMathItem()->DoCastOnMathOp(&pValueToCast, dwRequiredTypeValue)==false)
							{
								LPSTR pTypeStr = g_pVarTable->MakeTypeNameOfTypeValue(dwRequiredTypeValue);
								LPSTR pR = pValueToCast->FindResultStringTokenForDBM()->GetStr();
								g_pErrorReport->SetError(StatementLineNumber, ERR_SYNTAX+10, pTypeStr, pR);
								SAFE_DELETE(pTypeStr);
								return false;
							}
							pCheckParam->SetMathItem(pValueToCast);
						}
					}
				}
				pCheckParam=pCheckParam->GetNext();
				dwPPos++;
			}
		}
	}

	// Complete
	return true;
}

DWORD CParameter::Count(void)
{
	DWORD dwCount=0;
	CParameter* pCurrent = this;
	while(pCurrent)
	{
		dwCount++;
		pCurrent=pCurrent->GetNext();
	}
	return dwCount;
}

CParameter* CParameter::GetLast(void)
{
	if(GetNext()) return GetNext()->GetLast();
	return this;
}

bool CParameter::SetParamAsLabel(CStr* pInternalLabelName)
{
	// Prepare string
	CStr* pStr = NULL;
	if(pInternalLabelName==NULL)
		pStr = new CStr("");
	else
		pStr = new CStr(pInternalLabelName->GetStr());

	// Create math object to hold param lebel
	CMathOp* pMathOp = new CMathOp;
	pMathOp->SetResult(pStr->GetStr(), 10, 0);
	SetMathItem(pMathOp);
	SAFE_DELETE(pStr);

	// Complete	
	return true;
}

bool CParameter::WriteDBM(void)
{
	// Current parameter
	if(m_pMainMathOp)
	{
		if(m_pMainMathOp->WriteDBM()==false) return false;
	}

	// Next parameter
	if(GetNext())
	{
		if(GetNext()->WriteDBM()==false) return false;
	}

	return true;
}
