// VarTable.cpp: implementation of the CVarTable class.
//
//////////////////////////////////////////////////////////////////////

// Custom Includes
#include "StatementList.h"
#include "StructTable.h"
#include "VarTable.h"
#include "time.h"

// Special access to global pointer to struct table (so can do full scan)
extern CStructTable* g_pStructTable;
extern CStatementList* g_pStatementList;

#ifdef __AARON_VARTABLEPERF__
# define ALLOWED_LOWER "abcdefghijklmnopqrstuvwxyz"
# define ALLOWED_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
# define ALLOWED_ALPHA ALLOWED_LOWER ALLOWED_UPPER
# define ALLOWED_DIGIT "0123456789"
# define ALLOWED_ALNUM ALLOWED_ALPHA ALLOWED_DIGIT
# define ALLOWED_IDENT ALLOWED_ALNUM "_"
# define ALLOWED_TYPES "#$%"
# define ALLOWED_SCOPE ":"
# define ALLOWED_INTRN "&@"
# define ALLOWED_MISCL ALLOWED_TYPES ALLOWED_INTRN ALLOWED_SCOPE

# define ALLOWED_DBVAR ALLOWED_IDENT ALLOWED_MISCL

typedef db3::TDictionary<CVarTable> map_type;
map_type CVarTable::g_Table(ALLOWED_DBVAR, map_type::ECase::Insensitive, map_type::EOnDestruct::DeleteEntries);

inline const char *MakeIntVarName(const char *scope, const char *name)
{
	static char buf[2048];

	if (!scope || !scope[0])
		return name;

	sprintf_s(buf, "%s:%s", scope, name);
	return buf;
}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVarTable::CVarTable()
{
	m_dwLineNumber=0;

	m_pVarScope=NULL;
	m_pVarName=NULL;
	m_pVarType=NULL;
	m_dwVarTypeValue=0;
	m_dwArrFlag=0;
	m_dwFinalDBMOffset=0;

	m_bOffsetAssigned=false;
	m_pAdditionalDataString=NULL;

	m_pNext=NULL;
	m_pPrev=NULL;
}

CVarTable::CVarTable(LPSTR pStr)
{
	m_dwLineNumber=0;

	m_pVarScope=new CStr("");
	m_pVarName=new CStr(pStr);
	m_pVarType=new CStr("dword");
	m_dwVarTypeValue=7;
	m_dwArrFlag=0;
	m_dwFinalDBMOffset=0;

	m_bOffsetAssigned=false;
	m_pAdditionalDataString=NULL;

	m_pNext=NULL;
	m_pPrev=NULL;

#ifdef __AARON_VARTABLEPERF__
	auto entry = g_Table.Lookup(pStr);
	assert_msg(entry != nullptr, "g_Table.Lookup() failed!");
	assert_msg(entry->P == nullptr, "Variable already exists");

	entry->P = this;
#endif
}

CVarTable::~CVarTable()
{
	SAFE_DELETE(m_pAdditionalDataString);
	SAFE_DELETE(m_pVarScope);
	SAFE_DELETE(m_pVarName);
	SAFE_DELETE(m_pVarType);
}

void CVarTable::Free(void)
{
	CVarTable* pCurrent = this;
	while(pCurrent)
	{
		CVarTable* pNext = pCurrent->GetNext();
		delete pCurrent;
		pCurrent = pNext;
	}
}

void CVarTable::Add(CVarTable* pNew)
{
	CVarTable* pCurrent = this;
	while(pCurrent->m_pNext)
	{
		pCurrent=pCurrent->GetNext();
	}
	pCurrent->m_pNext=pNew;
	pNew->m_pPrev=pCurrent;
}

void CVarTable::Insert(CVarTable* pNew)
{
	// Get neighbors
	CVarTable* pNeighA = m_pPrev;
	CVarTable* pNeighB = this;

	// Instruct neighbours to point to me
	if(pNeighA) pNeighA->m_pNext = pNew;
	pNeighB->m_pPrev = pNew;

	// Insruct new to point to neighbors
	pNew->m_pNext = pNeighB;
	pNew->m_pPrev = pNeighA;
}

void CVarTable::SetVarDefaults(void)
{
	// One _ESP_ var in table
	DWORD dwAddDefaultVars=1;

	// Used To Hold Pointer to Runtime Error DWORD (filled by DLLs during RT-error)
	Add(new CVarTable("$_ERR_")); dwAddDefaultVars++;
	Add(new CVarTable("$_ESC_")); dwAddDefaultVars++;
	Add(new CVarTable("$_REK_")); dwAddDefaultVars++;
	Add(new CVarTable("$_SLN_")); dwAddDefaultVars++;
	
	// Used To Hold Float Immediates (bound to/from ST0)
	Add(new CVarTable("$_TEMPA_")); dwAddDefaultVars++;
	Add(new CVarTable("$_TEMPB_")); dwAddDefaultVars++;

	// Update Variable Counter
	g_pStatementList->IncVarQtyCounter(dwAddDefaultVars);
}

void CVarTable::AddInOrder(LPSTR pName, CVarTable* pNew)
{
	// Find place to insert new variable
	CVarTable* pLocation = this->GetNext();
	while(pLocation)
	{
		if(stricmp(pName,pLocation->GetVarName()->GetStr())<0) break;
		pLocation=pLocation->GetNext();
	}
	if(pLocation)
	{
		// Insert before this location
		pLocation->Insert(pNew);
	}
	else
	{
		// Add to end of list
		Add(pNew);
	}
}

CVarTable* CVarTable::Advance(DWORD dwCountdown)
{
	if(dwCountdown==0)
		return this;
	else
		if(m_pNext)
			return m_pNext->Advance(dwCountdown-1);

	return NULL;
}

CVarTable* CVarTable::Subtract(DWORD dwCountdown)
{
	if(dwCountdown==0)
		return this;
	else
		if(m_pPrev)
			return m_pPrev->Subtract(dwCountdown-1);

	return NULL;
}

bool CVarTable::AddVariable(LPSTR pName, LPSTR pType, DWORD dwArrFlag, DWORD dwLineNumber, bool bFromActualCodeNotFromTypeDefing, DWORD* pdwAction, bool bIsGlobal)
{
	// Ignore if no variables addable
	if(g_pStatementList->GetVariableAddParse()==false)
		return true;

	// Do not attempt to add system vars
	if(pName[0]=='$' && pName[1]=='$')
		return true;

	// LOCAL Temporary variables must not be confused with GLOBAL ones
	bool bVarIsTemporaryVariable=false;
	if(pName[0]=='$') bVarIsTemporaryVariable=true;

	// If defining var in userfunction, must be added to local dec chain
	LPSTR pScope=NULL;
	LPSTR pUserFunc = g_pStatementList->GetUserFunctionName();
	if(stricmp(pUserFunc,"")!=NULL)
		pScope = g_pStatementList->GetUserFunctionName();

	// leefix - 210703 - added for global var specified in ENDFUNCTION param
	// This flag is set when a variable being scanned for checks globally FIRST!
	bool bCheckGloballyFirst = g_pStatementList->GetLocalVarUsageAsGlobal();

	// Check if variable in local scope
	bool bVariableHasBeenAdded=false;
	CVarTable* pFoundVar = FindVariable(pScope, pName, dwArrFlag);
	if(pFoundVar)
	{
		// If variable created during a pre-scan, accept as added
		if(pFoundVar->GetPreScanAddFlag()==true)
		{
			// Variable re-use is fine - but only once more!
			bVariableHasBeenAdded=true;
			SetPreScanAddFlag(false);
		}
		else
		{
			if(bFromActualCodeNotFromTypeDefing==true)
			{
				// Variables used mid-code can be added many times (only one counts though)
				// Action: Variable already exists as local variable
				if(pdwAction) *pdwAction=3;
				return true;
			}
			else
			{
				// Cannot duplicate a variable declaration by assignment a different type
				if(stricmp(pFoundVar->GetVarType()->GetStr(),pType)==NULL)
				{
					// Same type, so treat as a re-init of variable
					if(pdwAction) *pdwAction=3;
					return true;
				}
				else
				{
					g_pErrorReport->SetError(dwLineNumber, ERR_SYNTAX+36, pFoundVar->GetVarType()->GetStr());
					return false;
				}
			}
		}
	}

	// leefix - 290703 - for backwards compatibility, prescan : arrays declared in a function and is global should be global (and not explicitly local)
	if ( pScope && dwArrFlag==1 && g_pStatementList->GetImplementationParse()==false && bIsGlobal==true )
		bCheckGloballyFirst=true;

	// Skip global var check if userfunction is declaring its local variables...
	if(stricmp(pUserFunc,"")!=NULL
	&& ( g_pStatementList->GetImplementationParse()==true || bCheckGloballyFirst==true ) )
	{
		/* leefix - 230603
		   It seems this code only strikes on the main pass all userfunction declarations
		   that use names also used by global variables. HOWEVER local names must take precidence!!
		   BUT if no local scope var was found, can look for global..
		*/
		if ( pFoundVar==NULL )
		{
			// Ensure it does not match a global variable
			if(pdwAction) *pdwAction=0;
			CVarTable* pEntry = FindVariable("", pName, dwArrFlag);
			if(pEntry)
			{
				// Only for non-temp vars
				if(bVarIsTemporaryVariable==false)
				{
					// Only if user specified GLOBAL (or is global by default) for this variable
					if(pEntry->GetSpecifiedAsGlobalFlag()==true)
					{
						// Yes global variable, so use global
						// Action: Variable already exists as global variable
						if(pdwAction) *pdwAction=2;
						return true;
					}
				}
			}
		}
	}

	// If defining var in userfunction, must be added to local dec chain
	CDeclaration* pGlobalDecChain = g_pStatementList->GetUserFunctionDecChain();
	if(pGlobalDecChain)
	{
		// LEEFIX - 230603 - u54 - ONLY IF GLOBAL NOT EXPLICITLY REQUIRED
		if ( bIsGlobal==false )
		{
			// Create Declaration for variable
			CDeclaration* pNewDec = new CDeclaration;
			pNewDec->SetDecData(dwArrFlag, "", pName, pType, "", dwLineNumber);

			// Add to chain USERFUNCTION LOCAL (if unique)
			if(pGlobalDecChain->Find(pName,dwArrFlag)==NULL)
				pGlobalDecChain->Add(pNewDec);
			else
				delete pNewDec;
		}
	}

	// If added, need go no further
	if(bVariableHasBeenAdded==true)
	{
		if(pdwAction) *pdwAction=3;
		return true;
	}

	// If scope is NULL, create a global string for it
	CStr* pVarScopeStr = new CStr(pScope);
//  LEEFIX - 230604 - u54 - GLOBAL DIM arr(x) in scope must have no scope name!
//	if((dwArrFlag==0 && pScope==NULL && bIsGlobal==true) 
	if((dwArrFlag==0 && bIsGlobal==true)
	|| (dwArrFlag==1 && (pScope==NULL || bIsGlobal==true)) )
	{
		// Global or Non-nested local arrays are also global
		pVarScopeStr->SetText("");
	}
	if(pType==NULL)
	{
		// Cannot duplicate a variable declaration
		g_pErrorReport->AddErrorString("Failed to 'AddVariable::pType==NULL'");
		SAFE_DELETE(pVarScopeStr);
		return false;
	}

	// lee - 110406 - u6rc7 - if array name, skip the &
	if ( pName )
	{
		if ( pName[0]=='&' )
		{
			// Before confirm, check if variable name is a reserved word
			if ( g_pStatementList->GetProgramStatements()->DetermineIfReservedWord ( pName+1 ) )
			{
				g_pErrorReport->SetError(g_pStatementList->GetLineNumber()-1, ERR_SYNTAX+7, pName+1);
				SAFE_DELETE(pVarScopeStr);
				return false;
			}

			// before confirm, check if variable name is a function name
			if ( g_pStatementList->GetProgramStatements()->DetermineIfFunctionName ( pName+1, true ) )
			{
				g_pErrorReport->SetError(g_pStatementList->GetLineNumber()-1, ERR_SYNTAX+7, pName+1);
				SAFE_DELETE(pVarScopeStr);
				return false;
			}
		}
		else
		{
			// Before confirm, check if variable name is a reserved word
			if ( g_pStatementList->GetProgramStatements()->DetermineIfReservedWord ( pName ) )
			{
				g_pErrorReport->SetError(g_pStatementList->GetLineNumber(), ERR_SYNTAX+7, pName);
				SAFE_DELETE(pVarScopeStr);
				return false;
			}

			// leefix-040803-Before confirm, check if variable name is a function name
			if ( g_pStatementList->GetProgramStatements()->DetermineIfFunctionName ( pName, true ) )
			{
				g_pErrorReport->SetError(g_pStatementList->GetLineNumber(), ERR_SYNTAX+7, pName);
				SAFE_DELETE(pVarScopeStr);
				return false;
			}
		}
	}

	// Create new variable
	CVarTable* pNewVar = new CVarTable;
	CStr* pVarNameStr = new CStr(pName);
	CStr* pVarTypeStr = new CStr(pType);
	pNewVar->SetVarScope(pVarScopeStr);
	pNewVar->SetVarName(pVarNameStr);
	pNewVar->SetVarType(pVarTypeStr);
	pNewVar->SetVarTypeValue(GetBasicTypeValue(pVarTypeStr->GetStr()));
	pNewVar->SetVarStruct(g_pStructTable->DoesTypeEvenExist(pVarTypeStr->GetStr()));
	pNewVar->SetArrFlag(dwArrFlag);
	pNewVar->SetLineNumber(dwLineNumber);
	pNewVar->SetSpecifiedAsGlobalFlag(bIsGlobal);

	// leefix - 170303 - Ensure arrays declared outside of scope are also forced as GLOBAL
	if ( dwArrFlag==1 )
	{
		if ( pVarScopeStr )
			if ( strcmp ( pVarScopeStr->GetStr(), "" )==NULL )
				pNewVar->SetSpecifiedAsGlobalFlag(true);

		if ( pVarScopeStr==NULL )
			pNewVar->SetSpecifiedAsGlobalFlag(true);
	}

	// Set prescan add flag if defined during pre-scam (locals in func required)
	if(g_pStatementList->GetImplementationParse()==false)
		pNewVar->SetPreScanAddFlag(true);
	else
		pNewVar->SetPreScanAddFlag(false);

	// Add Variable to Variables Table
	AddInOrder(pName, pNewVar);

#ifdef __AARON_VARTABLEPERF__
	const char *pIntVarName = MakeIntVarName(pVarScopeStr->GetStr(), pName);

	auto entry = g_Table.Lookup(pIntVarName);
	assert_msg(entry != nullptr, "g_Table.Lookup() failed!");
	assert_msg(entry->P == nullptr, "Variable already exists");

	entry->P = pNewVar;
#endif

	// Increment var qty index counter
	g_pStatementList->IncVarQtyCounter(1);

	// Action: Added for first time
	if(pdwAction) *pdwAction=1;

	// Complete
	return true;
}

CVarTable* CVarTable::FindVariable(LPSTR pScope, LPSTR pName, DWORD dwArrFlag)
{
#ifdef __AARON_VARTABLEPERF__
	const char *pIntName = MakeIntVarName(pScope, pName);

	auto entry = g_Table.Find(pIntName);
	if (!entry || !entry->P)
		return nullptr;

	if (entry->P->GetArrFlag()!=dwArrFlag)
		return nullptr;

	return entry->P;
#else
	// Start search in middle of list
	CVarTable* pCurrent = this;
	DWORD dFirst = 0;
	DWORD dLast = g_pStatementList->GetVarQtyCounter();
	DWORD dMiddlePos = 0;
	DWORD dwStep = ((dLast-dFirst)/2);
	if(dwStep<1) dwStep=1;
	dMiddlePos = dFirst + dwStep;
	pCurrent = pCurrent->Advance(dwStep);

	// First find name in list (irrespective of scope)
	DWORD dwDirectionSwitchAtOneStep=0;
	while(pCurrent)
	{
		// Close in on item
		DWORD dwDirection=0;
		if(stricmp(pName, pCurrent->GetVarName()->GetStr())<0)
		{
			dLast = dMiddlePos;
			dwStep = ((dLast-dFirst)/2);
			if(dwStep<1) dwStep=1;
			dMiddlePos = dMiddlePos - dwStep;
			pCurrent = pCurrent->Subtract(dwStep);
			dwDirection=1;
		}
		else
		{
			if(stricmp(pName, pCurrent->GetVarName()->GetStr())>0)
			{
				dFirst = dMiddlePos;
				dwStep = ((dLast-dFirst)/2);
				if(dwStep<1) dwStep=1;
				dMiddlePos = dMiddlePos + dwStep;
				pCurrent = pCurrent->Advance(dwStep);
				dwDirection=2;
			}
		}

		// If one step and direction switch, not here in list!
		if(abs(static_cast<int>(dLast - dFirst)) < 2)
		{
			if(dwDirectionSwitchAtOneStep==0)
				dwDirectionSwitchAtOneStep=dwDirection;
			else
				if(dwDirectionSwitchAtOneStep!=dwDirection)
					break;
		}

		// No more items
		if(pCurrent==NULL)
			break;

		if(dFirst==dLast)
			break;

		// Check if this is the one
		if(stricmp(pCurrent->GetVarName()->GetStr(),pName)==NULL)
			break;
	}

	// Make sure its the first name in the list (may be several) (trackback)
	while(pCurrent)
	{
		if(pCurrent->GetPrev())
		{
			if(stricmp(pCurrent->GetPrev()->GetVarName()->GetStr(),pName)!=NULL) break;
			pCurrent=pCurrent->GetPrev();
		}
		else
			break;
	}

	// Check name(s) against scope
	while(pCurrent)
	{
		if(stricmp(pCurrent->GetVarName()->GetStr(),pName)==NULL)
		{
			if(pCurrent->GetArrFlag()==dwArrFlag)
			{
				if(pScope)
				{
					if(pCurrent->GetVarScope())
					{
						if(pCurrent->GetVarScope()->GetStr()!=NULL)
						{
							if(stricmp(pCurrent->GetVarScope()->GetStr(),pScope)==NULL)
								return pCurrent;
						}
					}
				}
				else
				{
					if(pCurrent->GetVarScope())
					{
						if(stricmp(pCurrent->GetVarScope()->GetStr(),"")==NULL) // 25-01-02:does this matter?
							return pCurrent;
					}
				}
			}
		}
		else
			break;

		pCurrent = pCurrent->GetNext();
	}

	// Could not find var name
	return NULL;
#endif
}

bool CVarTable::FindVariableExist(LPSTR pFindVar, DWORD dwArrFlag)
{
	// Ensure pScope is observed
	LPSTR pScope=NULL;
	if(g_pStatementList->GetUserFunctionDecChain())
		if(g_pStatementList->GetUserFunctionName())
			pScope = g_pStatementList->GetUserFunctionName();

	// Scan list and match variable name
	CVarTable* pFoundVar = FindVariable(pScope, pFindVar, dwArrFlag);
	if(pFoundVar==NULL)
	{
		// Try as global
		pFoundVar = FindVariable("", pFindVar, dwArrFlag);
	}

	// True if found..
	if(pFoundVar)
		return true;
	else
	{
		// soft fail
		return false;
	}
}

bool CVarTable::FindTypeOfVariable(LPSTR pFindVar, DWORD dwArrType, LPSTR* pReturnType)
{
	/*
	// Skip var token
	CStr* pFindVarName = NULL;
	if(*pVarString=='@') pVarString++;
	if(pMathString->CheckChars(0,3,"FS@"))
	{
		pFindVarName = new CStr(pMathString->GetStr()+3);
		pVarString = pFindVarName->GetStr() + pFindVarName->FindFirstChar('@') + 1;
	}
	if(*pVarString=='&') dwArrFlag=1;
	*/

	// Ensure pScope is observed
	LPSTR pScope=NULL;
	if(g_pStatementList->GetUserFunctionDecChain())
		if(g_pStatementList->GetUserFunctionName())
			pScope = g_pStatementList->GetUserFunctionName();

	// Scan list and match variable name
	CVarTable* pFoundVar = FindVariable(pScope, pFindVar, dwArrType);
	if(pFoundVar==NULL)
	{
		// Try as global
		pFoundVar = FindVariable("", pFindVar, dwArrType);
		if(pFoundVar==NULL) pFoundVar = FindVariable("", pFindVar, dwArrType);
	}

	// Get Type Data
	if(pFoundVar)
	{
		// Get type string
		DWORD length = strlen(pFoundVar->GetVarType()->GetStr());
		*pReturnType = new char[length+2];
		strcpy(*pReturnType, pFoundVar->GetVarType()->GetStr());
		return true;
	}

	// Nope soft fail
	return false;
}

LPSTR CVarTable::MakeDefaultVarType(LPSTR pDecName)
{
	if(pDecName)
	{
		LPSTR pDecType = new char[8];
		strcpy(pDecType,"integer");

		// Suffix rule can pre-define variable types
		DWORD length=strlen(pDecName);
		if(pDecName[length-1]=='#')
		{
			strcpy(pDecType,"float");
		}
		if(pDecName[length-1]=='$')
		{
			strcpy(pDecType,"string");
		}
		return pDecType;
	}
	else
		return NULL;
}

DWORD CVarTable::MakeDefaultVarTypeValue(LPSTR pDecName)
{
	DWORD dwTypeValue=1;
	if(pDecName)
	{
		// Suffix rule can pre-define variable types
		DWORD length=strlen(pDecName);
		if(pDecName[length-1]=='#')
		{
			dwTypeValue=2;
		}
		if(pDecName[length-1]=='$')
		{
			dwTypeValue=3;
		}
	}
	return dwTypeValue;
}

LPSTR CVarTable::MakeTypeNameOfTypeValue(DWORD dwTypeValue)
{
	CStructTable* pCurrent = g_pStructTable;
	while(pCurrent)
	{
		if(pCurrent->GetTypeValue()==dwTypeValue)
		{
			LPSTR pStr = new char[pCurrent->GetTypeName()->Length()+1];
			strcpy(pStr, pCurrent->GetTypeName()->GetStr());
			return pStr;
		}
		pCurrent = pCurrent->GetNext();
	}
	return NULL;
}

DWORD CVarTable::GetBasicTypeValue(LPSTR pTypeString)
{
	CStructTable* pCurrent = g_pStructTable;
	while(pCurrent)
	{
		LPSTR pCurrentStr = pCurrent->GetTypeName()->GetStr();
		if(stricmp(pCurrentStr, pTypeString)==NULL)
		{
			return pCurrent->GetTypeValue();
		}
		pCurrent = pCurrent->GetNext();
	}
	return 0;
}

CStructTable* CVarTable::GetStruct(LPSTR pTypeString)
{
	CStructTable* pCurrent = g_pStructTable;
	while(pCurrent)
	{
		LPSTR pCurrentStr = pCurrent->GetTypeName()->GetStr();
		if(stricmp(pCurrentStr, pTypeString)==NULL)
		{
			return pCurrent;
		}
		pCurrent = pCurrent->GetNext();
	}
	return 0;
}

char CVarTable::GetCharOfType(DWORD dwTypeValue)
{
	CStructTable* pCurrent = g_pStructTable;
	while(pCurrent)
	{
		if(pCurrent->GetTypeValue()==dwTypeValue)
		{
			return pCurrent->GetTypeChar();
		}
		pCurrent = pCurrent->GetNext();
	}
	return 0;
}

DWORD CVarTable::GetTypeValueOfChar(unsigned char cTypeChar)
{
	CStructTable* pCurrent = g_pStructTable;
	while(pCurrent)
	{
		if(pCurrent->GetTypeChar()==cTypeChar)
		{
			return pCurrent->GetTypeValue();
		}
		pCurrent = pCurrent->GetNext();
	}
	return 0;
}

bool CVarTable::VerifyVariableStructures(void)
{
	// Scan list and match variable name
	if(GetVarType())
	{
		if(g_pStructTable->DoesTypeEvenExist(GetVarType()->GetStr())==NULL)
		{
			DWORD LineNumber = GetLineNumber();
			g_pErrorReport->SetError(LineNumber, ERR_SYNTAX+37, GetVarType()->GetStr());
			return false;
		}
	}

	// And next one..
	if(GetNext())
		return GetNext()->VerifyVariableStructures();

	// Variable ok
	return true;
}

DWORD CVarTable::EstablishVarOffsets(DWORD* pdwOffsetValue)
{
	// Only if variable has name
	if(m_pVarName)
	{
		// Only variables that not been assigned offsets
		if(m_bOffsetAssigned==false)
		{
			// Only if variable GLOBAL
			if(m_pVarScope->Length()==0 || m_pVarScope->CheckChars(0, 6, "GLOBAL")==true)
			{
				// Assign offset to this variable
				m_dwFinalDBMOffset = *pdwOffsetValue;
				m_bOffsetAssigned = true;

				// Work out size of variable datatype
				DWORD dwAddSize=0;
				if(m_dwArrFlag)
					dwAddSize = 4; // Pointer Only
				else
				{
					if(GetVarType())
					{
						dwAddSize = g_pStructTable->GetSizeOfType(GetVarType()->GetStr());
					}
				}

				// Add size to offset for next one
				*pdwOffsetValue = (*pdwOffsetValue) + dwAddSize;
			}
		}
	}

	// Write next one
	if(GetNext())
		return GetNext()->EstablishVarOffsets(pdwOffsetValue);

	// Return final accumilated size
	return (*pdwOffsetValue);
}

bool CVarTable::WriteDBMHeader(void)
{
	// Blank Line
	CStr strDBMBlank(1);
	if(g_pDBMWriter->OutputDBM(&strDBMBlank)==false) return false;

	// header Line
	CStr strDBMLine(256);
	strDBMLine.SetText("VARIABLES:");
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	return true;
}

bool CVarTable::WriteDBMFooter(DWORD dwSizeOfVariableBuffer)
{
	CStr strDBMLine(256);
	strDBMLine.SetText("SIZE OF VARIABLE BUFFER = ");
	strDBMLine.AddNumericText(dwSizeOfVariableBuffer);
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	// Complete
	return true;
}

bool CVarTable::WriteDBM(void)
{
	// Only if variable has name
	if(m_pVarName)
	{
		// Get Offset Value
		DWORD dwOffsetValue=m_dwFinalDBMOffset;

		// Only if variable GLOBAL
		if(m_pVarScope->Length()==0 || m_pVarScope->CheckChars(0, 6, "GLOBAL")==true)
		{
			// Write out text
			CStr strDBMLine(256);
			strDBMLine.SetText("@");
			strDBMLine.AddText(m_pVarName);
			strDBMLine.AddText("=");
			strDBMLine.AddNumericText(dwOffsetValue);

			// Array variables are pointers not actual data mem
			strDBMLine.AddText("  [STRUCT@");
			strDBMLine.AddText(m_pVarType);
			strDBMLine.AddText("]");

			// Output details
			if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;
		}
	}

	// Write next one
	if(GetNext())
	{
		if((GetNext()->WriteDBM())==false) return false;
	}

	// Complete
	return true;
}
