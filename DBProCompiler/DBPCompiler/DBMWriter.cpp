// DBMWriter.cpp: implementation of the CDBMWriter class.
//
//////////////////////////////////////////////////////////////////////
#include "ParserHeader.h"
#include "StructTable.h"
#include "LabelTable.h"
#include "DataTable.h"
#include "VarTable.h"
#include "DBPCompiler.h"

// Custom Includes
#include "DBMWriter.h"

#include <DB3Time.h>

// External Class Pointer
extern CDBPCompiler* g_pDBPCompiler;
extern CVarTable* g_pVarTable;
extern CDataTable* g_pDataTable;
extern CDataTable* g_pStringTable;
extern CDataTable* g_pDLLTable;
extern CDataTable* g_pCommandTable;
extern CLabelTable* g_pLabelTable;
extern CStructTable* g_pStructTable;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBMWriter::CDBMWriter()
{
	m_pDBMData=NULL;
	m_pDBMDataPointer=NULL;
	m_dwDBMDataSize=0;
}

CDBMWriter::~CDBMWriter()
{
	SAFE_FREE(m_pDBMData);
}

bool CDBMWriter::OutputDBM(const char *pDBMStr, size_t length)
{
	if (!m_pDBMData)
		return true;

	// Calculate length of data to add
	if (!length)
		length = strlen(pDBMStr);
	length += 2;

	// First ensure memory is not exceeded
	CheckAndExpandDBMMemory(length);

	// Proceed to add to memory
	LPSTR pPointer = GetDBMDataPointer();
	if(pPointer+length >= m_pDBMData+m_dwDBMDataSize)
	{
		// Failed
		g_pErrorReport->AddErrorString("Failed to 'OutputDBM'");
		return false;
	}

	memcpy(pPointer, pDBMStr, length);
	pPointer += length;
	*(pPointer++)=13;
	*(pPointer++)=10;
	SetDBMDataPointer(pPointer);

	// Complete
	return true;
}
bool CDBMWriter::OutputDBM(CStr* pDBMStr)
{
	return OutputDBM(pDBMStr->GetStr(), pDBMStr->Length());
}

DWORD CDBMWriter::EatCarriageReturn(void)
{
	DWORD dwCount=0;
	if(m_pDBMData)
	{
		// Backtrack two to eat carriage return
		LPSTR pPointer = GetDBMDataPointer()-2;
		SetDBMDataPointer(pPointer);

		// Count characters in line
		while(pPointer>m_pDBMData)
		{
			if(*pPointer==10) break;
			pPointer--;
			dwCount++;
		}
	}
	return dwCount;
}

bool CDBMWriter::CheckAndExpandDBMMemory(DWORD dwLengthOfNewAddData)
{
	if(!m_pDBMData)
		return true;

	// If within 10K of end, expand memory
	LPSTR pDBMDataBarrier=(m_pDBMData+m_dwDBMDataSize)-(dwLengthOfNewAddData*2);
	if(m_pDBMDataPointer<=pDBMDataBarrier)
		// Did not expand
		return false;

	// Work out offset of pointer
	DWORD dwOffset = m_pDBMDataPointer-m_pDBMData;

	// Create New Larger memory (another 1MB)
	DWORD dwNewSize = m_dwDBMDataSize+(102400*10);
	LPSTR pNewMem = (LPSTR)GlobalAlloc(GMEM_FIXED, dwNewSize);

	// Copy current data to new memory
	memcpy(pNewMem, m_pDBMData, m_dwDBMDataSize);

	// Erase old
	SAFE_FREE(m_pDBMData);

	// Rereference to new memory
	m_dwDBMDataSize=dwNewSize;
	m_pDBMData=pNewMem;
	m_pDBMDataPointer=pNewMem+dwOffset;

	// Mem was expanded
	return true;
}

bool CDBMWriter::WriteProgramAsEXEOrDEBUG(LPSTR lpEXEFilename, bool bParsingMainProgram)
{
	// Free any previous usage
	SAFE_DELETE(m_pDBMData);

	// Only parse if new code to parse
	if(GetNewCodeFlag()==true)
	{
		// Create ASM Header
		g_pASMWriter->CreateASMHeader();

		// Create DBM Buffer (default 1MB)?
		if(g_pDBPCompiler->GetProduceDBMFile())
		{
			m_dwDBMDataSize=102400*10;
			m_pDBMData = (LPSTR)GlobalAlloc(GMEM_FIXED, m_dwDBMDataSize);
			m_pDBMDataPointer=m_pDBMData;
		}
		else
		{
			m_dwDBMDataSize = 0;
			m_pDBMData = NULL;
			m_pDBMDataPointer = NULL;
		}

		// Write DBM starting with first statement
		g_pStatementList->SetWriteStarted(true);
		if(bParsingMainProgram)
		{
			// Main program
			{
				db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> g_pStatementList->WriteDBM()");
				g_pStatementList->WriteDBM();
			}

			CStatement *pCurrentStatement;

			// Write program code
			{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> \"Write program code\"");
//			if(g_pStatementList->GetProgramStatements()) g_pStatementList->GetProgramStatements()->WriteDBM();
			pCurrentStatement = g_pStatementList->GetProgramStatements();
			while(pCurrentStatement)
			{
				CStatement* pNextStatement = pCurrentStatement->GetNext();
				pCurrentStatement->SetNext(NULL);
				pCurrentStatement->WriteDBM();
				pCurrentStatement=pNextStatement;
			}
			}

			// Write prescan code
			{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> \"Write prescan code\"");
//			if(g_pStatementList->GetPreScanStatements()) g_pStatementList->GetPreScanStatements()->WriteDBM();
			pCurrentStatement = g_pStatementList->GetPreScanStatements();
			while(pCurrentStatement)
			{
				CStatement* pNextStatement = pCurrentStatement->GetNext();
				pCurrentStatement->SetNext(NULL);
				pCurrentStatement->WriteDBM();
				pCurrentStatement=pNextStatement;
			}
			}

			// Figure Out Var Offset and Final Varspace Size
			DWORD dwVarSize=0;
			g_pVarTable->EstablishVarOffsets(&dwVarSize);
			g_pStatementList->SetVarOffsetCounter(dwVarSize);
		}
		else
		{
			// Mini Program from CLI
			if(g_pStatementList->GetMiniStatements()) g_pStatementList->GetMiniStatements()->WriteDBM();

			// Figure Out Var Offset and Final Varspace Size
			DWORD dwVarSize=g_pStatementList->GetVarOffsetCounter();
			g_pVarTable->EstablishVarOffsets(&dwVarSize);
			g_pStatementList->SetVarOffsetCounter(dwVarSize);
		}

		// Scan all DLLS, and add any that are link-associated
		int iCount=g_pDLLTable->CompleteAnyLinkAssociates();
		if(iCount>0) g_pStatementList->IncDLLIndexCounter(iCount);

		// Write DBM of Variables (recalcualtes offset to track offset positions)
		if(g_pDBPCompiler->GetProduceDBMFile())
		{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> \"Write DBM or Variables\"");
			g_pVarTable->WriteDBMHeader();
			g_pVarTable->WriteDBM();
			g_pVarTable->WriteDBMFooter(g_pStatementList->GetVarOffsetCounter());	
		}

		// Write DBM Data
		if(g_pDBPCompiler->GetProduceDBMFile())
		{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> Write DBM Data (g_pStringTable)");
			if(g_pStringTable->GetNext())
			{
				g_pStringTable->WriteDBMHeader(1);
				g_pStringTable->GetNext()->WriteDBM();
			}
		}

		// Write DBM Data
		if(g_pDBPCompiler->GetProduceDBMFile())
		{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> Write DBM Data (g_pDataTable)");
			if(g_pDataTable->GetNext())
			{
				g_pDataTable->WriteDBMHeader(2);
				g_pDataTable->GetNext()->WriteDBM();
			}
		}

		// Write DBM Data
		if(g_pDBPCompiler->GetProduceDBMFile())
		{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> Write DBM Data (g_pDLLTable)");
			if(g_pDLLTable->GetNext())
			{
				g_pDLLTable->WriteDBMHeader(3);
				g_pDLLTable->GetNext()->WriteDBM();
			}
		}

		// Write DBM Data
		if(g_pDBPCompiler->GetProduceDBMFile())
		{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> Write DBM Data (g_pCommandTable)");
			if(g_pCommandTable->GetNext())
			{
				g_pCommandTable->WriteDBMHeader(4);
				g_pCommandTable->GetNext()->WriteDBM();
			}
		}

		// Write DBM Data
		if(g_pDBPCompiler->GetProduceDBMFile())
		{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> Write DBM Data (g_pLabelTable)");
			if(g_pLabelTable->GetNext())
			{
				g_pLabelTable->WriteDBMHeader();
				g_pLabelTable->GetNext()->WriteDBM();
			}
		}

		// Write DBM of Structures (debug)
		if(g_pDBPCompiler->GetProduceDBMFile())
		{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> Write DBM Data (g_pStructTable)");
			if(g_pStructTable)
			{
				g_pStructTable->WriteDBMHeader();
				g_pStructTable->WriteDBM();
			}
		}

		// Deposit in DBM File
		//#ifdef _DEBUG
		//{
		if(g_pDBPCompiler->GetProduceDBMFile())
		{
			db3::CProfile<> prof("CDBMWriter::WriteProgramAsEXEOrDEBUG() -> Deposit in DBM File");
			HANDLE hFile = CreateFile(g_pDBPCompiler->GetInternalFile(PATH_TEMPDBMFILE), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile!=INVALID_HANDLE_VALUE)
			{
				DWORD BytesWritten=0;
				DWORD ActualBytesToWrite=m_pDBMDataPointer-m_pDBMData;
				WriteFile(hFile, m_pDBMData, ActualBytesToWrite, &BytesWritten, NULL);
				SAFE_CLOSE(hFile);
			}
		}
		//}
		//#endif

		// Free DBM memory
		g_pStatementList->SetWriteStarted(false);
		if(g_pDBPCompiler->GetProduceDBMFile()) SAFE_FREE(m_pDBMData);
	}

	// Progress Reporting Tool Reset For Percentage Step Through
	g_pErrorReport->SetMaxLines(g_pStatementList->GetLineNumber());

	// Create ASM Header
	return g_pASMWriter->PrepareEXE(lpEXEFilename, bParsingMainProgram, GetNewCodeFlag());
}
