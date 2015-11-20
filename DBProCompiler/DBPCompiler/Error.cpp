// Error.cpp: implementation of the CError class.
//
//////////////////////////////////////////////////////////////////////

// Common Includes
#include "Error.h"
#include "macros.h"
#include "StatementList.h"
#include "DBPCompiler.h"
#include "IncludeTable.h"

// External Class Pointer
extern CDBPCompiler* g_pDBPCompiler;
extern CStatementList* g_pStatementList;
extern CIncludeTable* g_pIncludeTable;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CError::CError()
{
	m_bErrorExist							= false;
	m_pErrorString							= NULL;

	m_bParserErrorExist						= false;
	m_pParserErrorString					= NULL;

	m_bEstablishedConnectionToMonitor		= false;
	m_hMonitorFileMap						= NULL;
	m_lpVoidMonitor							= NULL;
//	m_hWndMonitor							= NULL;

	// Establish Connection To A Progress Monitor
	// leechange - 130306 - u60 - remove dependence on FindWindow, just broadcast filemap openly
//	m_hWndMonitor = FindWindow("TDBPROEDITOR",NULL);
//	if(m_hWndMonitor)
//	{
		m_bEstablishedConnectionToMonitor=true;
		m_hMonitorFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,256,"DBPROEDITORMESSAGE");
		m_lpVoidMonitor = MapViewOfFile(m_hMonitorFileMap,FILE_MAP_WRITE,0,0,256);
//	}

	m_dwInternalErrorsMax					= 0;
	m_pInternalError						= NULL;
	m_dwParserErrorsMax						= 0;
	m_pParserError							= NULL;
	m_dwRuntimeErrorsMax					= 0;
	m_pRuntimeError							= NULL;
}

CError::~CError()
{
	// Free Errors Database
	FreeErrorDatabase();

	// Free Monitor Vars
	if(m_bEstablishedConnectionToMonitor)
	{
		// Release virtual file
		UnmapViewOfFile(m_lpVoidMonitor);
		CloseHandle(m_hMonitorFileMap);
	}

	SAFE_DELETE(m_pParserErrorString);
	SAFE_DELETE(m_pErrorString);
}

void CError::PrepareVerboseErrorHeader(DWORD LineNumber, LPSTR ErrorString)
{
	db3::CAutolock autolock(m_Lock);

	// Dump Contents of Line to Error Window
	if(g_pStatementList)
	{
		// Error Report
		AddErrorString("PARSER ERROR");
		AddErrorString("------------");
		AddErrorString(ErrorString);
		AddErrorString("");

		// Error Header
		AddErrorString("PROGRAM TRACE");
		AddErrorString("-------------");

		// Sample Of Line in error
		CStr* pStr = new CStr(65);
		LPSTR pPointer=g_pStatementList->GetFileDataPointer();
		LPSTR pPointerEnd=g_pStatementList->GetFileDataEnd();
		if(pPointer)
		{
			pStr->CopyFromPtr(pPointer, pPointerEnd, 64);
			pStr->SetChar(64,0);
			LPSTR pLineDump = new char[pStr->Length()+1];
			strcpy(pLineDump, pStr->GetStr());
			AddErrorString(pLineDump);
			SAFE_DELETE(pLineDump);
			SAFE_DELETE(pStr);
			AddErrorString("");
		}

		AddErrorString("ERROR TRACE");
		AddErrorString("-----------");
	}
}

void CError::AddErrorString(LPSTR ErrorString)
{
	db3::CAutolock autolock(m_Lock);

	if(m_pParserErrorString==NULL)
	{
		if(g_pStatementList)
		{
			DWORD dwLineNum = 0;
			g_pStatementList->GetTokenLineNumber();
			m_pParserErrorString = new CStr(1);
			m_pParserErrorString->SetText(ErrorString);
			if(dwLineNum>0)
			{
				LPSTR pUseLineNumber = g_pDBPCompiler->GetWord(11);
				if ( strcmp ( pUseLineNumber, "")!=NULL )
				{
					m_pParserErrorString->AddText(" ");
					m_pParserErrorString->AddText(pUseLineNumber);
					m_pParserErrorString->AddText(" ");
					m_pParserErrorString->AddNumericText(dwLineNum);
				}
			}
			m_bParserErrorExist=true;

			// Dump Contents of Line to Error Window
			PrepareVerboseErrorHeader(dwLineNum, "UNDEFINED PARSER ERROR");
		}
	}

	// Calc string sizes
	DWORD oldsize = 0;
	DWORD addsize = strlen(ErrorString);
	DWORD length = addsize;

	// Concat two strings
	if(m_pErrorString)
	{
		oldsize=m_pErrorString->Length();
		length+=oldsize;
	}
	length+=2;
	CStr* pNewErrorString = new CStr(length+1);
	if(oldsize>0) memcpy(pNewErrorString->GetStr(), m_pErrorString->GetStr(), oldsize);
	memcpy(pNewErrorString->GetStr()+oldsize, ErrorString, addsize+1);
	*((pNewErrorString->GetStr()+oldsize+addsize)+0)=13;
	*((pNewErrorString->GetStr()+oldsize+addsize)+1)=10;
	*((pNewErrorString->GetStr()+oldsize+addsize)+2)=0;

	// Replace with new
	SAFE_DELETE(m_pErrorString);
	m_pErrorString=pNewErrorString;
	m_bErrorExist=true;
}

void CError::SetParserError(DWORD dwLine, LPSTR ErrorString)
{
	db3::CAutolock autolock(m_Lock);

	if(m_pParserErrorString==NULL)
	{
		m_pParserErrorString = new CStr(1);
		m_pParserErrorString->SetText(ErrorString);
		if(dwLine>0)
		{
			LPSTR pUseLineNumber = g_pDBPCompiler->GetWord(11);
			if ( strcmp ( pUseLineNumber, "")!=NULL )
			{
				m_pParserErrorString->AddText(" ");
				m_pParserErrorString->AddText(pUseLineNumber);
				m_pParserErrorString->AddText(" ");
				m_pParserErrorString->AddNumericText(dwLine);
			}
		}
		m_bParserErrorExist=true;

		// Dump Contents of Line to Error Window
		PrepareVerboseErrorHeader(dwLine, ErrorString);
	}
}

void CError::OutputInternalErrorReport(void)
{
	// Output Verbose Error Report To File
	LPSTR lpString = GetErrorString();

	// Deposit in File
	HANDLE hFile = CreateFile(g_pDBPCompiler->GetInternalFile(PATH_TEMPERRORFILE), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		DWORD BytesWritten=0;
		DWORD ActualBytesToWrite=strlen(lpString);
		WriteFile(hFile, lpString, ActualBytesToWrite, &BytesWritten, NULL);
		SAFE_CLOSE(hFile);
	}
}

// DATABASE ERROR STRING FUNCTIONS

DWORD CError::CountDatabaseSubset(LPSTR pSection, LPSTR pErrorFilename)
{
	char label[_MAX_PATH];
	char tempfile[_MAX_PATH];
	for(DWORD i=1; i<65535; i++)
	{
		wsprintf(label, "%d", i);
		GetPrivateProfileString(pSection, label, "", tempfile, _MAX_PATH, pErrorFilename);
		if(strcmp(tempfile,"")==NULL) break;
	}
	return i;
}

LPSTR* CError::CreateDatabaseSubset(LPSTR pSection, DWORD dwMax, LPSTR pErrorFilename)
{
	// Temp Vars
	char label[_MAX_PATH];
	char tempfile[_MAX_PATH];
	LPSTR* pDatabase = new LPSTR[dwMax];
	ZeroMemory(pDatabase, dwMax * sizeof(LPSTR));
	for(DWORD i=1; i<dwMax; i++)
	{
		wsprintf(label, "%d", i);
		GetPrivateProfileString(pSection, label, "", tempfile, _MAX_PATH, pErrorFilename);
		if(strcmp(tempfile,"")!=0)
		{
			pDatabase[i] = (LPSTR)new char[strlen(tempfile)+1];
			strcpy(pDatabase[i], tempfile);
		}
	}
	return pDatabase;
}

LPSTR* CError::CreateRuntimeDatabaseSubset(LPSTR pSection, DWORD dwMax, LPSTR pErrorFilename)
{
	// Temp Vars
	char label[_MAX_PATH];
	char tempfile[_MAX_PATH];
	LPSTR* pDatabase = new LPSTR[dwMax];
	ZeroMemory(pDatabase, dwMax * sizeof(LPSTR));
	for(DWORD i=1; i<dwMax; i++)
	{
		// Runtime has large gaps in the numeric sequence (speed things up)
		if(i==150) i=300;
		if(i==350) i=500;
		if(i==550) i=1000;
		if(i==1050) i=1500;
		if(i==1550) i=2000;
		if(i==2050) i=3000;
		if(i==3050) i=3200;
		if(i==3250) i=3500;
		if(i==3550) i=4000;
		if(i==4050) i=4100;
		if(i==4200) i=5000;
		if(i==5200) i=7000;
		if(i==7250) i=7300;
		if(i==7310) i=7600;
		if(i==7650) i=7700;
		if(i==7850) i=7900;
		if(i==7950) i=8000;
		if(i==9010) i=9700;
		if(i==9750) i=9998;

		// Get field
		char work[_MAX_PATH];
		wsprintf(work, "%d", 10000+i);
		strcpy(label, work+1);
		GetPrivateProfileString(pSection, label, "", tempfile, _MAX_PATH, pErrorFilename);
		if(strcmp(tempfile,"")!=0)
		{
			pDatabase[i] = (LPSTR)new char[strlen(tempfile)+1];
			strcpy(pDatabase[i], tempfile);
		}
	}
	return pDatabase;
}

void CError::LoadErrorDatabase(LPSTR pErrorFilename)
{
	// Load Internal Errors
	m_dwInternalErrorsMax = CountDatabaseSubset("INTERNAL", pErrorFilename);
	m_pInternalError = CreateDatabaseSubset("INTERNAL", m_dwInternalErrorsMax, pErrorFilename);

	// Load Parser Errors
	m_dwParserErrorsMax = CountDatabaseSubset("SYNTAX", pErrorFilename);
	m_pParserError = CreateDatabaseSubset("SYNTAX", m_dwParserErrorsMax, pErrorFilename);

	// Load Runtime Errors
	m_dwRuntimeErrorsMax = 9999;
	m_pRuntimeError = CreateRuntimeDatabaseSubset("RUNTIME", m_dwRuntimeErrorsMax, pErrorFilename);
}

void CError::FreeDatabaseSubset(LPSTR* pDatabase, DWORD dwMax)
{
	if(pDatabase)
	{
		for(DWORD i=0; i<dwMax; i++)
		{
			delete pDatabase[i];
			pDatabase[i]=NULL;
		}
	}
}

void CError::FreeErrorDatabase(void)
{
	// Free Internal Database
	FreeDatabaseSubset(m_pInternalError, m_dwInternalErrorsMax);
	SAFE_DELETE(m_pInternalError);

	// Free Parser Database
	FreeDatabaseSubset(m_pParserError, m_dwParserErrorsMax);
	SAFE_DELETE(m_pParserError);

	// Free RuntimeError Database
	FreeDatabaseSubset(m_pRuntimeError, m_dwRuntimeErrorsMax);
	SAFE_DELETE(m_pRuntimeError);
}

void CError::GetErrorConstruction(DWORD dwLine, DWORD dwErrCode, CStr** pRawErrorString)
{
	// Find String from database
	DWORD dwIndex=0;
	LPSTR* ppDatabase=NULL;
	if(dwErrCode>=ERR_INTERNAL && dwErrCode<=ERR_SYNTAX) { ppDatabase=m_pInternalError; dwIndex=dwErrCode; }
	if(dwErrCode>=ERR_SYNTAX && dwErrCode<=ERR_COMPILER) { ppDatabase=m_pParserError; dwIndex=dwErrCode-ERR_SYNTAX; }

	// Use database to return correct error construction line
	if(pRawErrorString && ppDatabase && dwIndex<m_dwParserErrorsMax)
	{
		// scan with dwErrCode
		(*pRawErrorString)->SetText(ppDatabase[dwIndex]);
	}
	else
	{
		if(m_pInternalError)
			(*pRawErrorString)->SetText(m_pInternalError[1]);
		else
			(*pRawErrorString)->SetText("");
	}

	// Remove AT LINE X. if line is zero
	if(dwLine==0)
	{
		LPSTR pOrig = m_pInternalError[2];
		LPSTR pRepl = m_pInternalError[3];
		CStr* pStr = (*pRawErrorString);
		if(strnicmp((pStr->GetStr()+pStr->Length())-strlen(pOrig), pOrig, strlen(pOrig))==NULL)
		{
			// Remove at line part
			pStr->SetChar(pStr->Length()-strlen(pOrig), 0);
			pStr->AddText(pRepl);
		}
	}
}

DWORD CError::GetTokenIndex(CStr* pTokenFieldString)
{
	// Choose from tokens
	if(pTokenFieldString->CheckChar(0, 'L')) return 99;
	if(pTokenFieldString->CheckChar(0, '1')) return 1;
	if(pTokenFieldString->CheckChar(0, '2')) return 2;
	if(pTokenFieldString->CheckChar(0, '3')) return 3;

	// No tokens matched
	return 0;
}

LPSTR CError::CreateAndReword ( LPSTR pI )
{
	LPSTR pStr = NULL;
	if ( pI )
	{
		DWORD dwSize = strlen(pI)+1;
		char pNewString [ 512 ];
		if ( pI[0]=='@' && pI[1]=='$' )
		{
			// rename
			strcpy ( pNewString, "TEMP" );
			strcat ( pNewString, pI+3 );
			dwSize = strlen(pNewString)+1;
			pStr = new char [ dwSize ];
			strcpy ( pStr, pNewString );
		}
		else
		{
			if ( pI[0]=='@' )
			{
				// straight copy +1
				pStr = new char [ dwSize ];
				strcpy ( pStr, pI+1 );
			}
			else
			{
				// straight copy
				pStr = new char [ dwSize ];
				strcpy ( pStr, pI );
			}
		}
	}
	return pStr;
}

void CError::ConstructError(DWORD dwLine, DWORD dwErrCode, LPSTR pIA, LPSTR pIB, LPSTR pIC)
{
	// temp 
	LPSTR pA=NULL;
	LPSTR pB=NULL;
	LPSTR pC=NULL;

	// make temp string (and reword if required)
	pA = CreateAndReword ( pIA );
	pB = CreateAndReword ( pIB );
	pC = CreateAndReword ( pIC );

	// CharPos From Line
	DWORD dwCharPosAt = 0;
	if(g_pStatementList) dwCharPosAt = g_pStatementList->GetLastCharInDataPosition();

	// End of Main Program Text
	DWORD dwMainCharPosMax = 0;
	if(g_pIncludeTable->GetNext())
		dwMainCharPosMax = g_pIncludeTable->GetNext()->GetFirstByte();

	// Line as text
	bool bRemoveAtLine=false;
	CStr* pLine = new CStr("");
	if(dwCharPosAt<=dwMainCharPosMax || dwMainCharPosMax==0)
	{
		// Within main program
		pLine->SetNumericText(dwLine);
	}
	else
	{
		// Find which include error is in
		CIncludeTable* pMustBeWithin = g_pIncludeTable->GetNext();
		CIncludeTable* pCurrent = pMustBeWithin->GetNext();
		while(pCurrent)
		{
			if(dwCharPosAt<=pCurrent->GetFirstByte()) break;
			pMustBeWithin=pCurrent;
			pCurrent=pCurrent->GetNext();
		}

		// State which include program
		if(pMustBeWithin)
		{
			pLine->SetText("inside ");
			pLine->AddText(pMustBeWithin->GetFilename());
			bRemoveAtLine=true;
		}
	}

	// Build String
	DWORD n=0;
	int iStart=-1;
	DWORD dwTokenID=0;
	CStr* pToken = new CStr("");
	CStr* pWorkStr = new CStr("");
	CStr* pConstruction = new CStr("");
	GetErrorConstruction(dwLine, dwErrCode, &pConstruction);
	while(n<pConstruction->Length())
	{
		// Find replacement token
		if(iStart==-1)
		{
			if(pConstruction->GetChar(n)=='#')
			{
				// Token start
				iStart=n;
			}
			else
			{
				// Build New Line
				pWorkStr->AddChar(pConstruction->GetChar(n));
			}
		}
		else
		{
			if(pConstruction->GetChar(n)=='#')
			{
				// Token end
				iStart=-1;

				// Use Token to append data to new line
				switch(GetTokenIndex(pToken))
				{
					case 1 :	pWorkStr->AddText(pA);		break;
					case 2 :	pWorkStr->AddText(pB);		break;
					case 3 :	pWorkStr->AddText(pC);		break;

					case 99 :	// Line Or Include Program
								if(bRemoveAtLine==true)
									if(strnicmp(pWorkStr->GetStr()+pWorkStr->Length()-8, "at line ", 8)==NULL)
										pWorkStr->SetChar(pWorkStr->Length()-8, 0);

								pWorkStr->AddText(pLine);
								break;
				}

				// Clear Token
				pToken->SetText("");
			}
			else
			{
				// Build Token
				pToken->AddChar(pConstruction->GetChar(n));
			}
		}

		// Next character
		n++;
	}

	// Set parser error with latest construction technique
	if(m_pParserErrorString==NULL)
	{
		m_pParserErrorString = new CStr(1);
		LPSTR pUseNumeric = g_pDBPCompiler->GetWord(10);
		if ( strcmp ( pUseNumeric, "")!=NULL )
		{
			m_pParserErrorString->SetText(pUseNumeric);
			m_pParserErrorString->AddNumericText(dwErrCode);
			m_pParserErrorString->AddText(": ");
		}
		m_pParserErrorString->AddText(pWorkStr->GetStr());
		m_bParserErrorExist=true;
	}

	// Dump Contents of Line to Error Window
	PrepareVerboseErrorHeader(dwLine, pWorkStr->GetStr());

	// Free usages
	SAFE_DELETE(pLine);
	SAFE_DELETE(pToken);
	SAFE_DELETE(pWorkStr);
	SAFE_DELETE(pConstruction);
	SAFE_DELETE(pA);
	SAFE_DELETE(pB);
	SAFE_DELETE(pC);
}

void CError::SetError(DWORD dwLine, DWORD dwErrCode)
{
	ConstructError(dwLine, dwErrCode, NULL, NULL, NULL);
	DB3_CRASH();
}

void CError::SetError(DWORD dwLine, DWORD dwErrCode, DWORD dw1)
{
	CStr* pNum1 = new CStr();
	pNum1->SetNumericText(dw1);
	ConstructError(dwLine, dwErrCode, pNum1->GetStr(), NULL, NULL);
	SAFE_DELETE(pNum1);
	DB3_CRASH();
}

void CError::SetError(DWORD dwLine, DWORD dwErrCode, LPSTR lp1)
{
	ConstructError(dwLine, dwErrCode, lp1, NULL, NULL);
	DB3_CRASH();
}

void CError::SetError(DWORD dwLine, DWORD dwErrCode, LPSTR lp1, LPSTR lp2)
{
	ConstructError(dwLine, dwErrCode, lp1, lp2, NULL);
	DB3_CRASH();
}


// PROGRESS FUNCTIONS

void CError::ProgressReport(LPSTR lpString, DWORD dwValue)
{
	if(dwValue==0)
		return;

	if(m_bEstablishedConnectionToMonitor)
	{
		// Copy to Virtual File
		LPSTR pTemp = new char[256];
		wsprintf(pTemp, "%s %d", lpString, dwValue);
		*(DWORD*)m_lpVoidMonitor = dwValue;
		strcpy((LPSTR)m_lpVoidMonitor+4, pTemp);
		delete pTemp;
	}
	else
	{
		// Find Editor to send to
		// leechange - 130306 - u60 - remove dependence on FindWindow, just broadcast filemap openly
//		HWND hWnd = FindWindow("TDBPROEDITOR",NULL);
//		if(hWnd)
//		{
			// Create Virtual File for Error Transfer
			HANDLE hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,256,"DBPROEDITORMESSAGE");
			LPVOID lpVoid = MapViewOfFile(hFileMap,FILE_MAP_WRITE,0,0,256);

			// Copy to Virtual File
			LPSTR pTemp = new char[256];
			wsprintf(pTemp, "%s %d", lpString, dwValue);
			*(DWORD*)lpVoid = dwValue;
			strcpy((LPSTR)lpVoid+4, pTemp);
			delete pTemp;

			// Release virtual file
			UnmapViewOfFile(lpVoid);
			CloseHandle(hFileMap);
//		}
	}
}
