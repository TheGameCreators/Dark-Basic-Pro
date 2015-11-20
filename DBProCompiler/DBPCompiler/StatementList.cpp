// StatementList.cpp: implementation of the CStatementList class.
//
//////////////////////////////////////////////////////////////////////
#include "ParserHeader.h"

// Custom Includes
#include "InstructionTable.h"
#include "StatementList.h"
#include "StructTable.h"
#include "LabelTable.h"
#include "VarTable.h"
#include "Error.h"

// External Class Pointer
extern CInstructionTable *g_pInstructionTable;
extern CStructTable* g_pStructTable;
extern CLabelTable* g_pLabelTable;
extern CVarTable* g_pVarTable;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatementList::CStatementList()
{
	// Reference Vars
	m_pFileData=NULL;
	m_pFileDataEnd=NULL;
	m_pFileDataPointer=NULL;
	m_FileDataSize=0;

	m_dwLastCharInDataPosition=0;
	m_dwTokenLineNumber=0;
	m_dwLineNumber=0;
	m_dwTempVariableIndex=0;

	m_dwVarOffsetCounter=0;
	m_pCurrentUserFunctionName.SetText("");
	m_pCurrentUserFunctionDecChain=NULL;

	m_dwDataIndexCounter=0;
	m_dwStringIndexCounter=0;

	m_dwDLLIndexCounter=0;
	m_dwCommandIndexCounter=0;

	m_dwLabelIndexCounter=0;
	m_dwLabelQtyCounter=0;
	m_dwVarQtyCounter=0;

	m_pInstructionRef=NULL;
	m_dwInstructionType=0;
	m_dwInstructionValue=0;
	m_dwInstructionParamMax=0;

	m_bParseVariableAdds=false;
	m_bParseImplementation=false;
	m_bDisableParsingToCR=false;
	m_bDisableParsingFull=false;
	m_bPermitLabelAsValue=false;

	m_dwLastLineNumber=0;

	m_pLatestLoopExitLabelRef=NULL;//ref only

	m_pPreScanStatements=NULL;
	m_pProgramStatements=NULL;
	m_pMiniStatements=NULL;
}

CStatementList::~CStatementList()
{
	// FileData Pointers are REFERENCE POINTERS
	if(m_pPreScanStatements)
	{
		m_pPreScanStatements->Free();
		m_pPreScanStatements=NULL;
	}
	if(m_pProgramStatements)
	{
		m_pProgramStatements->Free();
		m_pProgramStatements=NULL;
	}
	if(m_pMiniStatements)
	{
		m_pMiniStatements->Free();
		m_pMiniStatements=NULL;
	}
}

void CStatementList::UpdateLineDBMData(CStatement* pStatementAt)
{
	// Add DBM position data to label information
	DWORD dwBytePositionOfLastInstruction=g_pASMWriter->GetBytePosOfLastInstruction();
	g_pLabelTable->UpdateDataIndexOfLabelsAtLine(pStatementAt, dwBytePositionOfLastInstruction);
	
	// For reference only (the writedbm error uses it to catch unknown local structures)
	m_pRefStatementDuringWrite = pStatementAt;
}

void CStatementList::SetLineNumber(DWORD line)
{
	// New Line
	m_dwLineNumber=line;
}

void CStatementList::IncLineNumber(void)
{
	// New Line
	m_dwLineNumber++;
}

bool CStatementList::MakeStatements(LPSTR pData, DWORD Size)
{
	// Init Pointers and Vars
	m_pFileData=pData;
	m_pFileDataEnd=(pData+Size)-1;
	m_FileDataSize=Size;

	// Place pointer to start of File Data
	ResetParserPointers();

	// Initialise prior to parsing
	SetLabelQtyCounter(0);
	SetVarQtyCounter(0);
	g_pVarTable->SetVarDefaults();

	// Create first statement
	SAFE_DELETE(m_pPreScanStatements);
	SAFE_DELETE(m_pProgramStatements);
	m_pPreScanStatements = new CStatement;
	m_pProgramStatements = new CStatement;

	// PreScan (handles recording of user function names, labels, data, etc)
	m_iNestCount = 0;
	SetImplementationParse(false);
	if(m_pPreScanStatements->DoPreScanBlock(ENDTK)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoPreScanBlock(0)'");
		return false;
	}

	// Calculate sizes of structures (all typedefs parsed)
	if(g_pStructTable->CalculateAllSizes()==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'CalculateAllSizes'");
		return false;
	}

	// Reset Parser for another run
	ResetParserPointers();

	// Init Code for Program (store stack ptr)
	if(m_pProgramStatements->DoInitCode()==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'EndCode'");
		return false;
	}

	// Process all statements starting at beginning
	m_iNestCount = 0;
	SetImplementationParse(true);
	if(m_pProgramStatements->DoBlock(ENDTK,NULL)==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'DoBlock(0)'");
		return false;
	}

	// End Code for Program (restore stack ptr)
	if(m_pProgramStatements->DoEndCode()==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'EndCode'");
		return false;
	}

	// Calculate sizes of structures (again for function structures)
	if(g_pStructTable->CalculateAllSizes()==false)
	{
		g_pErrorReport->AddErrorString("Failed to 'CalculateAllSizes'");
		return false;
	}

	// Complete
	return true;
}

bool CStatementList::AddMiniStatements(LPSTR pData, DWORD Size)
{
	// Init Pointers and Vars to MiniProgram
	m_pFileData=pData;
	m_pFileDataEnd=(pData+Size)-1;
	m_FileDataSize=Size;
	SetFileDataPointer(m_pFileData);

	// Create first statement
	SAFE_DELETE(m_pMiniStatements);
	m_pMiniStatements = new CStatement;

	// Init Code for Program (store stack ptr)
	if(m_pMiniStatements->DoInitCode()==false)
	{
		g_pErrorReport->AddErrorString("Failed to m_pMiniStatements 'InitCode'");
		return false;
	}

	// Process all statements starting at beginning
	SetImplementationParse(true);
	if(m_pMiniStatements->DoBlock(ENDTK,NULL)==false)
	{
		g_pErrorReport->AddErrorString("Failed to m_pMiniStatements 'DoBlock(0)'");
		return false;
	}

	// End Code for Program (restore stack ptr)
	if(m_pMiniStatements->DoEndCode()==false)
	{
		g_pErrorReport->AddErrorString("Failed to m_pMiniStatements 'EndCode'");
		return false;
	}

	// Calculate sizes of structures
	if(g_pStructTable->CalculateAllSizes()==false)
	{
		g_pErrorReport->AddErrorString("Failed to m_pMiniStatements 'CalculateAllSizes'");
		return false;
	}

	// Complete
	return true;
}

void CStatementList::ResetParserPointers(void)
{
	// Place pointer to start of File Data
	m_pFileDataPointer=m_pFileData;

	// Set starting vars
	SetLineNumber(1);
	SetTokenLineNumber(0);
	SetDataIndexCounter(0);
	SetStringIndexCounter(0);
	SetDLLIndexCounter(0);
	SetCommandIndexCounter(0);
	SetLabelIndexCounter(0);
	SetVariableAddParse(true);
	SetLastLine(0);
}

bool CStatementList::FindStartOfFileDataProgramLine(DWORD dwFindLineNumber, LPSTR* pReturnText)
{
#ifdef __AARON_PERFIMPROVE__
	size_t len;

	len = GetLineText(dwFindLineNumber, nullptr, 0);
	if (!len)
		return false;

	*pReturnText = new char[len];
	if (!*pReturnText)
		return false;

	return !!GetLineText(dwFindLineNumber, *pReturnText, len);
#else
	DWORD dwCountLine=1;
	LPSTR pPointer=m_pFileData;
	while(pPointer<m_pFileDataEnd)
	{
		if(*(pPointer+0)==13 && *(pPointer+1)==10) dwCountLine++;
		if(dwCountLine>=dwFindLineNumber) break;
		pPointer++;
	}
	if(pPointer>=m_pFileDataEnd)
	{
		// soft fail
		return false;
	}

	// Create String for text from search
	DWORD n=0;
	pPointer+=2;
	CStr* pNewText = new CStr(256);
	while(pPointer<m_pFileDataEnd)
	{
		if(*(pPointer+0)==13 && *(pPointer+1)==10) break;
		if(pPointer>m_pFileDataEnd) break;
		pNewText->AddChar(*pPointer);
		pPointer++;
	}
	DWORD length=pNewText->Length();
	*pReturnText = new char[length+1];
	strcpy(*pReturnText, pNewText->GetStr());
	SAFE_DELETE(pNewText);

	return true;
#endif
}
size_t CStatementList::GetLineText(DWORD dwLineNumber, char *pDst, size_t DstLen)
{
	static bool didInitLines = false;

	if (!didInitLines)
	{
		static db3::CLock lock;
		db3::CAutolock autolock(lock);
		char *p, *q;

		m_LinePtrs.Push(m_pFileData);

		for(p=m_pFileData; p < m_pFileDataEnd; p=q)
		{
			bool nl = false;

			if (*p=='\r')
			{
				nl = true;
				p++;
			}

			if (*p=='\n')
			{
				nl = true;
				p++;
			}

			if (nl)
			{
				q = p;
				m_LinePtrs.Push(p);
			}
			else
				q = p + 1;
		}

		m_LinePtrs.Push(m_pFileDataEnd);

		didInitLines = true;
	}

	if (dwLineNumber - 1 >= m_LinePtrs.Size())
		return 0;

	char *s, *e;

	s = m_LinePtrs[dwLineNumber - 1];
	e = m_LinePtrs[dwLineNumber];

	size_t len = static_cast<size_t>(e - s + 1);

	if (!pDst)
		return len;

	if (len > DstLen)
		return 0;

	memcpy(pDst, s, e - s);
	pDst[e - s] = '\0';

	if (e - s > 1 && pDst[e - s - 2]=='\r')
		pDst[e - s - 2] = '\0';
	else if(e - s > 0 && pDst[e - s - 1]=='\n')
		pDst[e - s - 1] = '\0';

	return len;
}

bool CStatementList::WriteDBM(void)
{
	// Write out text
	CStr strDBMLine(256);
	strDBMLine.SetText("######CODE:");
	if(g_pDBMWriter->OutputDBM(&strDBMLine)==false) return false;

	return true;
}
