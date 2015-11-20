// DBPCompiler.cpp: implementation of the CDBPCompiler class.
//
//////////////////////////////////////////////////////////////////////

// Includes
#include <stdio.h>
#include "direct.h"
#include "macros.h"
#include "ASMWriter.h"
#include "DBPCompiler.h"
#include "InstructionTable.h"
#include "StatementList.h"
#include "StructTable.h"
#include "DBMWriter.h"
#include "IncludeTable.h"
#include "LabelTable.h"
#include "DataTable.h"
#include "VarTable.h"
#include "DebugInfo.h"
#include "Error.h"
#include "Str.h"
#include "shlobj.h"

#include <DB3Time.h>

// Internal Global Data Pointers
CEXEBlock*			g_pEXE				= NULL;
CDBPCompiler*		g_pDBPCompiler		= NULL;
CError*				g_pErrorReport		= NULL;
CASMWriter*			g_pASMWriter		= NULL;
CDBMWriter*			g_pDBMWriter		= NULL;
CStructTable*		g_pStructTable		= NULL;
CStatementList*		g_pStatementList	= NULL;
CInstructionTable*	g_pInstructionTable	= NULL;
CLabelTable*		g_pLabelTable		= NULL;
CDataTable*			g_pDataTable		= NULL;
CDataTable*			g_pStringTable		= NULL;
CDataTable*			g_pDLLTable			= NULL;
CDataTable*			g_pCommandTable		= NULL;
CVarTable*			g_pVarTable			= NULL;
CIncludeTable*		g_pIncludeTable		= NULL;
CDataTable*			g_pConstantsTable	= NULL;
CDebugInfo			g_DebugInfo;

// lee - 050406 - u6rc6 - new directive for academic non-admin users
bool				g_bLocalTempFolder	= false;
bool				g_bExternaliseDLLS	= false;

// External Values
extern DWORD g_dwEscapeValueMem;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDBPCompiler::CDBPCompiler(LPSTR pCompilerFilename)
{
	// Store Compiler Filename
	m_pCompilerFilename = new CStr(pCompilerFilename);
	m_pCompilerPathOnly = new CStr(pCompilerFilename);
	m_pCompilerPathOnly->TrimToPathOnly();

	// Clear these strings
	m_pAbsolutePathToProjectFile=NULL;
	m_pRelativePathToProjectFile=NULL;

	// Initialisation of File Data Ptrs
	m_dwOriginalFileDataSize=0;
	m_pOriginalFileData=NULL;
	m_FileDataSize=0;
	m_pFileData=NULL;

	// Initialisation of Project File Data
	m_bProjectExists=false;
	m_ProjectFileDataSize=0;
	m_pProjectFileData=NULL;

	// Initialisation of Project Setting Vars
	m_pFinalDBASource=NULL;
	m_pEXEFilename=NULL;

	// Project Compiler Settings
	m_bDebugModeOn=false;
	m_bRuntimeErrorsOn=true;
	m_bProduceDBMFileOn=true;
	m_bFullScreenModeOn=false;
	m_bFullDesktopModeOn=false;
	m_bDesktopModeOn=false;
	m_dwStartDisplayWidth=640;
	m_dwStartDisplayHeight=480;
	m_dwStartDisplayDepth=32;
	m_bHiddenModeOn=false;
	m_bEXEAloneState=false;
	m_bEXEInstallerState=false;
	m_bCompressPCKState=false;
	m_bEncryptionState=false;
	m_bDoubleLiterals=false;
	m_bSpeedOverStabilityState=false;
	m_bGenerateHelpTxtOn=false;
	#ifdef MAKEHELPTXT
	// lee - 210406 - can switch this to true to generate new HELPTXT in plugins folder
	m_bGenerateHelpTxtOn = true;
	#endif

	// Clear Path Database
	for(DWORD n=0; n<PATH_MAX; n++)
		m_pInternalFile[n]=NULL;

	// lee - 110307 - u6.6 - Clear exclusion strings
	for ( int i=0; i<MAX_EXCLUSIONS; i++)
		g_pExcludeFiles [ i ] = NULL;

	// Cleat breakpoint list
	m_pBreakpointList = NULL;
}

CDBPCompiler::~CDBPCompiler()
{
	// Compiler Path Freeing
	SAFE_DELETE(m_pCompilerFilename);
	SAFE_DELETE(m_pCompilerPathOnly);

	// Safe Deletions
	SAFE_DELETE(m_pOriginalFileData);
	SAFE_FREE(m_pFileData);
	SAFE_FREE(m_pProjectFileData);
	SAFE_FREE(m_pFinalDBASource);
	SAFE_FREE(m_pEXEFilename);

	// Release strings
	for(DWORD n=0; n<PATH_MAX; n++)
	{
		SAFE_DELETE(m_pInternalFile[n]);
	}

	// Free exclusion strings
	for ( int i=0; i<MAX_EXCLUSIONS; i++)
		SAFE_DELETE ( g_pExcludeFiles [ i ] );

	// Delete breakpoint lsit
	SAFE_DELETE(m_pBreakpointList);
}

bool CDBPCompiler::PerformCompileOnProject(void)
{
	// Result
	bool bResult=true;

	// Get DBA Source File
	LPSTR pDBAFilename = m_pFinalDBASource;

	// Create Error Report Database
	g_pErrorReport->LoadErrorDatabase(GetInternalFile(PATH_ERRORSFILE));

	// Load DBA into memory
	if(LoadDBA(pDBAFilename))
	{
		// Expand FileData to unfold any #Includes
		if(UnfoldFileDataIncludes())
		{
			// leemove - 250604 - u54 - set default and load command database
			g_pInstructionTable = new CInstructionTable;
			g_pInstructionTable->SetInternalInstructionDatabase();
			g_pInstructionTable->LoadInstructionDatabase();

			// Expand FileData to unfold any #Constants
			if(UnfoldFileDataConstants())
			{
				// Remove Breakpoints and Construct BreakPoint List
				if(RemoveAndRecordBreakpoints()==false)
				{
					g_pErrorReport->AddErrorString("Failed to 'RemoveAndRecordBreakpoints'");
					bResult=false;
				}
				else
				{
					// Begin List to collect tokenised program
					if(MakeProgram()==false)
					{
						g_pErrorReport->AddErrorString("Failed to Parse Program (MakeDBM->MakeProgram)'");
						bResult=false;
					}
				}
			}
			else
			{
				g_pErrorReport->AddErrorString("Failed to 'UnfoldFileDataConstants'");
				bResult=false;
			}

			// free instruction table
			SAFE_DELETE(g_pInstructionTable);
		}
		else
		{
			g_pErrorReport->AddErrorString("Failed to 'UnfoldIncludes'");
			bResult=false;
		}
	}
	else
	{
		g_pErrorReport->AddErrorString("Failed to 'LoadDBA'");
		bResult=false;
	}

	// Report Error if available..
	if(g_pErrorReport)
	{
		if(g_pErrorReport->IsError())
		{
			// Create Virtual File for Error Transfer
			HANDLE hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,256,"DBPROEDITORMESSAGE");
			LPVOID lpVoid = MapViewOfFile(hFileMap,FILE_MAP_WRITE,0,0,256);
			LPSTR lpString = g_pErrorReport->GetParserErrorString();
			if(g_pErrorReport->IsParserError())
				lpString = g_pErrorReport->GetParserErrorString();
			else
				lpString = g_pErrorReport->GetErrorString();

			// Copy Error to Virtual File
			if ( strlen ( lpString )>255 )
			{
				char ErrorSpace[256];
				strncpy ( ErrorSpace, lpString, 255 );
				ErrorSpace[255]=0;
				strcpy((LPSTR)lpVoid, ErrorSpace);
			}
			else
				strcpy((LPSTR)lpVoid, lpString);

			// Find Editor to send to
			HWND hWnd = FindWindow("TDBPROEDITOR",NULL);
			if(hWnd)
			{
				// Found editor, transmit
				SendMessage(hWnd, WM_USER+0, 0, 0);
			}
			else
			{
				// No Editor, use Own Window (causes crashes lots)
				MessageBox(NULL, lpString, "COMPILER ERROR", MB_OK);
			}

			// Release virtual file
			UnmapViewOfFile(lpVoid);
			CloseHandle(hFileMap);

			// Deposit Verbose Error File (test mode only)
			g_pErrorReport->OutputInternalErrorReport();
		}
	}

	// Free Include Table
	SAFE_DELETE(g_pIncludeTable);

	// Complete
	return bResult;
}

bool CDBPCompiler::LoadDBA(LPSTR pDBAFilename)
{
	db3::CProfile<> prof("CDBPCompiler::LoadDBA");

	// Release any previous usage
	SAFE_FREE(m_pFileData);

	// Load DBA Data (by file or MMF)
	bool bFileLoaded = false;
	if ( m_bSourceIsMMF )
	{
		// Load DBA MMF into memory
		if(LoadRawFromMMF(pDBAFilename, &m_pFileData, &m_FileDataSize))
			bFileLoaded=true;
	}
	else
	{
		// Load DBA file into memory
		if(LoadRaw(pDBAFilename, &m_pFileData, &m_FileDataSize))
			bFileLoaded=true;
	}

	// If loaded okay..
	if( bFileLoaded )
	{
		// Eat EdgeSpaces
		CStr* pNewStr = new CStr(m_pFileData);
		pNewStr->EatTrailingEdgeSpacesandTabs();
		ZeroMemory(m_pFileData, m_FileDataSize);
		strcpy(m_pFileData, pNewStr->GetStr());
		SAFE_DELETE(pNewStr);
	}
	else
	{
		g_pErrorReport->AddErrorString("Failed to 'LoadDBA'");
		return false;
	}

	// Make snapshot of filedata in original store
	SAFE_DELETE(m_pOriginalFileData);
	m_dwOriginalFileDataSize = m_FileDataSize;
	m_pOriginalFileData = new char[m_dwOriginalFileDataSize+256];
	memset(m_pOriginalFileData, 0, m_FileDataSize+256);
	memcpy(m_pOriginalFileData, m_pFileData, m_FileDataSize);

	// Complete
	return true;
}

bool CDBPCompiler::LoadRaw(LPSTR pDBAFilename, LPSTR* ppData, DWORD* pdwDataSize)
{
	// Load DBA into memory
	HANDLE hFile = CreateFile(pDBAFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		// Create memory and transfer file data to it
		DWORD dwLoadSize = GetFileSize(hFile, NULL);
		LPSTR pLoadData = (LPSTR)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, dwLoadSize+1);
	
		// Transfer Data
		DWORD BytesRead=0;
		ReadFile(hFile, pLoadData, dwLoadSize, &BytesRead, NULL);

		// Close File
		SAFE_CLOSE(hFile);

		// Set Data for return vars
		*ppData = pLoadData;
		*pdwDataSize = dwLoadSize;
	}
	else
	{
		return false;
	}

	// Complete
	return true;
}

bool CDBPCompiler::LoadRawFromMMF(LPSTR pDBAMMFName, LPSTR* ppData, DWORD* pdwDataSize)
{
	// Memory to be used to store string sent
	bool bResult=true;
	DWORD dwDataSize=0;
	LPSTR pData=NULL;

	// First Four Bytes are Size of Message
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_READ,FALSE,pDBAMMFName);
	if(hFileMap)
	{
		LPVOID lpVoid = MapViewOfFile(hFileMap,FILE_MAP_READ,0,0,0);
		if(lpVoid)
		{
			// First DWORD is data size
			DWORD dwDataSize = 0;
			dwDataSize = *((LPDWORD) lpVoid);
			if ( dwDataSize>0 )
			{
				// Create memory and transfer file data to it
				LPSTR pLoadData = (LPSTR)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, dwDataSize+1);
			
				// Transfer Data
				memcpy(pLoadData, (LPSTR)lpVoid+4, dwDataSize);

				// Set Data for return vars
				*ppData = pLoadData;
				*pdwDataSize = dwDataSize;
			}
			else
			{
				// MMF not found
				bResult=false;
			}

			// Close Message
			UnmapViewOfFile(lpVoid);
		}
		else
		{
			// MMF not found
			bResult=false;
		}
		CloseHandle(hFileMap);
	}
	else
	{
		// MMF not found
		bResult=false;
	}

	// Complete
	return bResult;
}

bool CDBPCompiler::UnfoldFileDataIncludes(void)
{
	db3::CProfile<> prof("CDBPCompiler::UnfoldFileDataIncludes");

	// Root FileData
	LPSTR pRootData=m_pFileData;
	DWORD dwRootDataSize=m_FileDataSize;

	// New FileData
	LPSTR pNewData=NULL;
	DWORD dwNewDataSize=0;

	// Create Include Table Entry (ROOT)
	g_pIncludeTable = new CIncludeTable;
	CStr* pRootName = new CStr("root");
	g_pIncludeTable->SetFilename(pRootName);
	g_pIncludeTable->SetFirstByte(0);

	// Fill with root source code
	CopyData(&pNewData, &dwNewDataSize, pRootData, dwRootDataSize);

	// Go through entire data (as it is being built)
	DWORD dwPtrCount=0;
	LPSTR pPtr=pNewData;
	while(1)
	{
		// Seek #include
		DWORD dwCount=0;
		LPSTR pIncludeFilename = NULL;
		LPSTR pPtrEnd=pNewData+dwNewDataSize;
		if(SeekIncludeToken(&pPtr, pPtrEnd, &dwCount, &pIncludeFilename)==false)
		{
			g_pErrorReport->AddErrorString("Failed to 'SeekIncludeToken'");
			SAFE_DELETE(pIncludeFilename);
			return false;
		}

		// Leave when no more
		if(pPtr>=pPtrEnd)
		{
			// Ready for parsing single filedata block
			SAFE_DELETE(pIncludeFilename);
			break;
		}

		// Advance counter
		dwPtrCount=dwPtrCount+dwCount;

		// Ensure include has not already been added
		if(g_pIncludeTable->FindInclude(pIncludeFilename)==false)
		{
			// Create Include Table Entry (INCLUDE BLOCK)
			CIncludeTable* pIncludeEntry = new CIncludeTable;
			CStr* pStrName = new CStr(pIncludeFilename);
			pIncludeEntry->SetFilename(pStrName);
			pIncludeEntry->SetFirstByte(dwNewDataSize);
			g_pIncludeTable->Add(pIncludeEntry);

			// Construct full include absolute path
			LPSTR pMediaRoot=GetProjectField("media root path");
			CStr* pAbsoluteIncludeFile = new CStr(pMediaRoot);
			pAbsoluteIncludeFile->AddText(pIncludeFilename);
			SAFE_DELETE(pMediaRoot);

			// Produce single FileData Block from multiple DBA Files
			LPSTR pData=NULL;
			DWORD dwDataSize=0;
			if(LoadRaw(pAbsoluteIncludeFile->GetStr(), &pData, &dwDataSize))
			{
				// Add Raw DBA to end of FileData
				CopyData(&pNewData, &dwNewDataSize, pData, dwDataSize);

				// Delete raw data after use
				SAFE_FREE(pData);
			}
			else
			{
				g_pErrorReport->SetError(0, ERR_SYNTAX+39, pIncludeFilename);
				SAFE_DELETE(pIncludeFilename);
				SAFE_DELETE(pAbsoluteIncludeFile);
				return false;
			}
			SAFE_DELETE(pAbsoluteIncludeFile);
		}

		// Free usage
		SAFE_DELETE(pIncludeFilename);

		// Update pointer
		pPtr=pNewData+dwPtrCount;
	}

	// Now erase old root data
	SAFE_FREE(m_pFileData);

	// Replace Root FileData with New Data
	m_pFileData=pNewData;
	m_FileDataSize=dwNewDataSize;

	// Make snapshot of filedata in original store
	SAFE_DELETE(m_pOriginalFileData);
	m_dwOriginalFileDataSize = m_FileDataSize;
	m_pOriginalFileData = new char[m_dwOriginalFileDataSize];
	memcpy(m_pOriginalFileData, m_pFileData, m_FileDataSize);

	// As a later compiler option, dump entire source to debugfile
	if ( 1 )
	{
		// Delete any old file that may exist
		char pFullSourceDump [ _MAX_PATH ];
		strcpy ( pFullSourceDump, g_pDBPCompiler->GetInternalFile(PATH_TEMPFOLDER) );
		strcat ( pFullSourceDump, "\\FullSourceDump.dba" );
		if ( FileExists ( pFullSourceDump ) ) DeleteFile ( pFullSourceDump );

		// Write Full Source Dump File
		HANDLE hFile = CreateFile(pFullSourceDump, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile!=INVALID_HANDLE_VALUE)
		{
			DWORD BytesWritten=0;
			WriteFile(hFile, m_pFileData, m_FileDataSize, &BytesWritten, NULL);
			SAFE_CLOSE(hFile);
		}
	}

	// Complete
	return true;
}

#define __AARON_UNFOLDPERF__ 1
#define __TOSTRING(x) #x
#define TOSTRING(x) __TOSTRING(x)
#define UNFOLD_MAX_CONSTANT_NAME 2048

void CDBPCompiler::EnsureDataMemBugEnough(LPSTR pPtr, DWORD dwPredictSize, LPSTR* pNewData, DWORD* dwNewDataSize, LPSTR* pWritePtr)
{
	if((*pWritePtr-*pNewData)+dwPredictSize>*dwNewDataSize)
	{
		// Expand new data memory
#ifdef __AARON_UNFOLDPERF__
# define EXPAND_MULTIPLY_AMOUNT 3
#else
# define EXPAND_MULTIPLY_AMOUNT 2
#endif
		DWORD dwBiggerSize = (*dwNewDataSize)*EXPAND_MULTIPLY_AMOUNT;
		LPSTR pBiggerMem = (LPSTR)GlobalAlloc(GMEM_FIXED, dwBiggerSize+1);
		memcpy(pBiggerMem, *pNewData, *dwNewDataSize);
		DWORD dwWriteOffset = (*pWritePtr)-(*pNewData);
		SAFE_FREE(*pNewData);
		*pNewData=pBiggerMem;
		*dwNewDataSize=dwBiggerSize;
		*pWritePtr=*pNewData+dwWriteOffset;
	}
}

bool CDBPCompiler::UnfoldFileDataConstants(void)
{
	db3::CProfile<> prof("CDBPCompiler::UnfoldFileDataConstants");

	// Also unfolds REMARK symbols

	// Create Constants Table
	g_pConstantsTable = new CDataTable;

	// Find all constants
	bool bLineIsRem=false;
	bool bBlockIsRem=false;

	DWORD dwIndex=0;
	LPSTR pPtr = m_pFileData;
	LPSTR pPtrEnd = m_pFileData + m_FileDataSize;
	while(pPtr<pPtrEnd)
	{
		// Record remarks
		if(strnicmp(pPtr, "remstart", 8)==NULL) bBlockIsRem=true;
		if(strnicmp(pPtr, "remend", 6)==NULL) bBlockIsRem=false;
		if(strnicmp(pPtr, "//", 2)==NULL) bLineIsRem=false;
		if(bBlockIsRem==true) bLineIsRem=true;
		if(strnicmp(pPtr, "rem ", 4)==NULL) bLineIsRem=true;
		if(*pPtr=='`') bLineIsRem=true;

		// Free line comment
		if(*pPtr==13 || *pPtr==10) bLineIsRem=false;

		// Seek #constant token
		if(bLineIsRem==false && bBlockIsRem==false)
		{
			LPSTR pStartByteOfToken=pPtr;
			if(strnicmp(pPtr, "#constant ", 10)==NULL)
			{
				// leefix - 230604 - u54 - get to first token
				// leefix - 070306 - u60 - added space after constant so you cannot write constantine :)
				// leefix - 140306 - u60b3 - oops, 109 changed to 10 :)
				pPtr+=10;
				while ( *(unsigned char*)pPtr<=32 && pPtr<pPtrEnd )
					pPtr++;

				// Record root start of constant label+value string 
				LPSTR pStringRootStart = pPtr;

//				// Get rest of line
//				LPSTR pStringStart=pPtr;
//				while(pPtr<pPtrEnd)
//				{
//					if(*(pPtr-1)==13 && *(pPtr)==10) break;
//					pPtr++;
//				}
				// lee - 100406 - u6rc7 - Get rest of line, stop at colon
				int iSpeechMark = 0;
				LPSTR pStringStart=pPtr;
				while(pPtr<pPtrEnd)
				{
					if ( *(pPtr)=='"' ) iSpeechMark=1-iSpeechMark;
					if ( iSpeechMark==0 )
						if ( *(pPtr)==':' )
							break;

					if(*(pPtr-1)==13 && *(pPtr)==10) break;
					pPtr++;
				}

				// lee - 140206 - u60 - and only go as far as commentry, and skip commentry
				LPSTR pStringEnd = pPtr;
				LPSTR pPtrScan = pStringRootStart;
				while(pPtrScan<pPtrEnd)
				{
					if(*(pPtrScan-1)==13 && *(pPtrScan)==10) break;
					if(*(pPtrScan-1)=='`') break;
					pPtrScan++;
				}
				if ( pPtrScan < pPtr ) 
				{
					// found comment before carriage return
					pStringEnd = pPtrScan - 1;
				}

				// cut out label and value of constant item
				DWORD dwStringLength = pStringEnd-pStringStart;
				if(dwStringLength>0)
				{
					// Prepare CStr
					CStr* pStrValue = new CStr(dwStringLength);
					DB_SCOPE_GUARD({SAFE_DELETE(pStrValue);});

					pStrValue->CopyFromPtr(pStringStart, pStringStart, dwStringLength);

					// Skip white chars
					pStrValue->EatEdgeSpacesandTabs(NULL);
					DWORD dwFirstSpaceOrEquate = pStrValue->FindFirstChar(' ');
					if(dwFirstSpaceOrEquate==0) dwFirstSpaceOrEquate = pStrValue->FindFirstChar('=');
					if(dwFirstSpaceOrEquate==0) dwFirstSpaceOrEquate = pStrValue->FindFirstChar(9);

					// if still no space, no second param in constant expression
					if ( dwFirstSpaceOrEquate>0 )
					{
						// Next item is constant name
#ifdef __AARON_UNFOLDPERF__
						char pConstantName[UNFOLD_MAX_CONSTANT_NAME];
						if (dwFirstSpaceOrEquate >= UNFOLD_MAX_CONSTANT_NAME)
						{
							g_pErrorReport->AddErrorString("Constant name exceeds " TOSTRING(UNFOLD_MAX_CONSTANT_NAME) " characters!");
							return false;
						}
#else
						LPSTR pConstantName = new char[dwFirstSpaceOrEquate+1];
						DB_SCOPE_GUARD({SAFE_DELETE(pConstantName);});
#endif
						memcpy(pConstantName, pStrValue->GetStr(), dwFirstSpaceOrEquate);
						pConstantName[dwFirstSpaceOrEquate]=0;

						// LEEFIX - 191102 - Validate that it ONLY containts constant character
						CStr* pValidConst = new CStr(pConstantName);
						DB_SCOPE_GUARD({SAFE_DELETE(pValidConst);});

						if(pValidConst->IsConstant())
						{
							// Yes, so rest is constant value
							LPSTR pConstantValue = pStrValue->GetRightOfPosition(dwFirstSpaceOrEquate);
							pStrValue->SetText(pConstantValue);
							pStrValue->EatEdgeSpacesandTabs(NULL);

							// remove any equate symbol as this could be added by user
							if(pStrValue->GetChar(0)=='=') pStrValue->SetChar(0, 32);

							// leefix - 250604 - u54 - ensure only comments exist after constant value
							// leefix - 260604 - u54 - only trimmer is a colon (commend or seperator) prior to end of line
//							DWORD dwFirstSpaceOrEquate = pStrValue->FindFirstChar(':');
//							if(dwFirstSpaceOrEquate==0) dwFirstSpaceOrEquate = pStrValue->FindFirstChar(9);
//							if(dwFirstSpaceOrEquate==0) dwFirstSpaceOrEquate = pStrValue->FindFirstChar(27);
							// leefix - 270206 - u60 - colon is okay inside speech marks
							int iSpeechMark = 0;
							DWORD dwFirstSpaceOrEquate = 0;
							for ( DWORD c=0; c<pStrValue->Length(); c++ )
							{
								if ( pStrValue->GetChar(c)=='"' ) iSpeechMark=1-iSpeechMark;
								if ( iSpeechMark==0 )
								{
									if ( pStrValue->GetChar(c)==':' )
									{
										dwFirstSpaceOrEquate = c;
										break;
									}
								}
							}
							if ( dwFirstSpaceOrEquate > 0 )
							{
								// trim constant value
								pStrValue->SetChar(dwFirstSpaceOrEquate,0);
							}

							// another space eating and we have the value
							pStrValue->EatEdgeSpacesandTabs(NULL);

							// Constant value
							if(pConstantName && pStrValue->Length()>0)
							{
								// leefix - 250604 - u54 - make sure constant name is not a reserved word
								if ( g_pInstructionTable->EnsureWordIsNotPartOfACommand ( pConstantName )==false )
								{
									// Add Unique Constant to Table (used later to replace instances of it)
									DWORD dwTry=dwIndex+1;
									if(!g_pConstantsTable->AddTwoStrings(pConstantName, pStrValue->GetStr(), &dwTry))
									{
										// leeadd - 270206 - u60 - report duplicate CONSTANT as an error
										g_pErrorReport->SetError(0, ERR_SYNTAX+67, pConstantName);
										return false;
									}
									else
									{
										// success
										dwIndex=dwTry;
									}

									// Mark token as a used constant
									*(pStartByteOfToken)='`';
								}
								else
								{
									// CONSTANT NAME is a RESERVED WORD (cannot share command name)
									g_pErrorReport->SetError(0, ERR_SYNTAX+66, pConstantName);
									return false;
								}
							}

						}
						else
						{
							// Report that it is not valud constant name
							g_pErrorReport->SetError(0, ERR_SYNTAX+48, pConstantName);
							return false;
						}
					}
					else
					{
						// Report that it is not valud constant name
						g_pErrorReport->SetError(0, ERR_SYNTAX+52, pStrValue->GetStr());
						SAFE_DELETE(pStrValue);
						return false;
					}

				}
			}
		}
		pPtr++;
	}

	// Record speech mark flag
	int iSpeechMark=0;

	// Create a New Space to expand into
	DWORD dwNewDataSize = m_FileDataSize + 32768;
	LPSTR pNewData = (LPSTR)GlobalAlloc(GMEM_FIXED, dwNewDataSize+1);
	LPSTR pWritePtr = pNewData;

	// Search Data for instance of constant and replace it with value
	pPtr = m_pFileData;
	pPtrEnd = m_pFileData + m_FileDataSize;
	LPSTR pLastReadPtr = pPtr;
	while(pPtr<pPtrEnd)
	{
		// Search for comment lines to skip
		// lee - 270206 - u60 - cannot respond to REM and comment symbols inside a speech mark
		if ( iSpeechMark==0 )
		{
			// lee - 200306 - u6b4 - blank out remark areas with ' symbol (allows REMarks in more places)
			DWORD dwFillWithMarks = 0;
			LPSTR pFillStart = NULL;

			// skip code
			if(strnicmp(pPtr, "`", 1)==NULL
			|| strnicmp(pPtr, "'", 1)==NULL
			|| strnicmp(pPtr, "//", 2)==NULL
			|| strnicmp(pPtr, "rem ", 4)==NULL
			|| strnicmp(pPtr, "remstart", 8)==NULL)
			{
				pFillStart = pPtr;
				if(strnicmp(pPtr, "remstart", 8)==NULL)
				{
					// Ensure followed by sub32 character
					if(*(pPtr+8)<=32)
					{
						// Skip code block
						while(pPtr<pPtrEnd)
						{
							if(strnicmp(pPtr-5, "remend", 6)==NULL) break;
							pPtr++;
						}
					}
					// lee - 270306 - u6b5 - leave REMSTART/REMEND alone
					//dwFillWithMarks = (pPtr-1) - pFillStart;
				}
				else
				{
					// Ensure followed by 32+ character
					bool bValidComment=false;
					if(strnicmp(pPtr, "`", 1)==NULL && *(pPtr+1)>=32) bValidComment=true;
					if(strnicmp(pPtr, "'", 1)==NULL && *(pPtr+1)>=32) bValidComment=true;
					if(strnicmp(pPtr, "//", 2)==NULL && *(pPtr+2)>=32) bValidComment=true;
					if(strnicmp(pPtr, "rem ", 4)==NULL && *(pPtr+4)>=32)
					{
						// lee - 230306 - u6b4 - additionally, REM only valid (if previous character is a sub-32 char)
						if ( *(unsigned char*)(pPtr-1)<=32 )
							bValidComment=true;
					}
					if(bValidComment)
					{
						// Only skip line
						while(pPtr<pPtrEnd)
						{
							if(*(pPtr-1)==13 && *(pPtr-0)==10) break;
							pPtr++;
						}
						dwFillWithMarks = (pPtr-2) - pFillStart;
					}
				}
				iSpeechMark=0;
			}

			// lee - 200306 - u6b4 - blank out remark areas now
			if ( pFillStart && dwFillWithMarks>0 )
				for ( LPSTR pN=pFillStart; pN<pFillStart+dwFillWithMarks; pN++ )
					*(pN)=(unsigned char)96; // ` symbol
		}

		// REPLACE ALL CONSTANTS : deal with speech marks, and constants outside them
		if(*(pPtr)=='"') iSpeechMark=1-iSpeechMark;
		if(iSpeechMark==0)
		{
			// Replace all constants with const-values
			CDataTable* pCurrentConst = g_pConstantsTable->GetNext();
			while(pCurrentConst)
			{
				// This Token Name
				LPSTR pToken = pCurrentConst->GetString()->GetStr();
				DWORD dwTokenLength = strlen(pToken);

				// Try to match program text with this token
				if(strnicmp(pPtr, pToken, dwTokenLength)==NULL)
				{
					// Check both
					bool bValidConst=true;
					for(int side=0; side<2; side++)
					{
						int iOffset;
						if(side==0) iOffset=-1;
						if(side==1) iOffset=(int)dwTokenLength;
						if(	(*((unsigned char*)pPtr+iOffset)=='_') || (side==1 && ( *((unsigned char*)pPtr+iOffset)=='(') )
						||	(*((unsigned char*)pPtr+iOffset)>='a' && *((unsigned char*)pPtr+iOffset)<='z') 
						||	(*((unsigned char*)pPtr+iOffset)>='A' && *((unsigned char*)pPtr+iOffset)<='Z')
						||	(*((unsigned char*)pPtr+iOffset)>='0' && *((unsigned char*)pPtr+iOffset)<='9') )
						{
							// Valid const char either side, so must be something else..
							bValidConst=false;
						}
					}
		
					// Must also match non-constant characters either side
					if(bValidConst==true)
					{
						// This Token Value
						LPSTR pTokenValue = pCurrentConst->GetString2()->GetStr();
						DWORD dwTokenValueLength = strlen(pTokenValue);

						// Calculate predicted near end of data space
						DWORD dwPredictSize = (pPtr-pLastReadPtr) + dwTokenValueLength + 32;

						// Ensure new data size is big enough for addition
						EnsureDataMemBugEnough(pPtr, dwPredictSize, &pNewData, &dwNewDataSize, &pWritePtr);

						// Copy upto this point
						DWORD dwWriteSize = pPtr-pLastReadPtr;
						memcpy(pWritePtr, pLastReadPtr, dwWriteSize);
						pWritePtr+=dwWriteSize;

						// Copy constant value to new data
						memcpy(pWritePtr, pTokenValue, dwTokenValueLength);
						pWritePtr+=dwTokenValueLength;

						// Advance past token
						pPtr+=dwTokenLength;
						pLastReadPtr=pPtr;
					}
				}

				// Get Next Constant
				pCurrentConst=pCurrentConst->GetNext();
			}
		}

		// next byte
		pPtr++;
	}

	// Calculate predicted near end of data space
	DWORD dwPredictSize = (pPtr-pLastReadPtr) + 32;

	// Ensure new data size is big enough for addition
	EnsureDataMemBugEnough(pPtr, dwPredictSize, &pNewData, &dwNewDataSize, &pWritePtr);

	// Copy upto this point
	DWORD dwWriteSize = pPtr-pLastReadPtr;
	memcpy(pWritePtr, pLastReadPtr, dwWriteSize);
	pWritePtr+=dwWriteSize;

	// Assign new data as latest file data for next pass (or final task)
	DWORD dwDataSizeOfNewData = pWritePtr-pNewData;
	SAFE_FREE(m_pFileData);
	m_pFileData=pNewData;
	m_FileDataSize=dwDataSizeOfNewData;

	// Finished with Constants Table
	g_pConstantsTable->Free();
	g_pConstantsTable=NULL;

	// Piggyback replacement of semicolons to colons except where speech marks preceed
	int iSpeechMarks=0;
	pPtr = m_pFileData;
	pPtrEnd = m_pFileData + m_FileDataSize;
	while(pPtr<pPtrEnd)
	{
		if(*pPtr=='"') iSpeechMarks=1-iSpeechMarks;
		if(iSpeechMarks==0)
		{
			// replaces ; with : to reduce compiler errors (c programmer accidents)
			if(*pPtr==';')
			{
				bool bValid=false;
				int iCount=1024;
				LPSTR pAtPtr = pPtr-1;
				while(iCount>0 && *pAtPtr!=10 && pAtPtr>=m_pFileData)
				{
					if(strnicmp(pAtPtr,"print",5)==NULL
					|| strnicmp(pAtPtr,"input",5)==NULL)
					{
						bValid=true;
						break;
					}
					pAtPtr--; iCount--;
				}
				if(bValid==false)
				{
					// Replace is
					*pPtr = ':';
				}
			}

			// allows the c programmers to document their lines easily
			if(*pPtr=='`' && *(unsigned char*)(pPtr+1)>=32)
			{
				// Replace then skip so we dont recurse the replacement!
				*(pPtr+0) = ':';
				*(pPtr+1) = '`';

				// leefix - 130306 - u60b3 - ensure speech marks are erased from comment lines
				LPSTR pQuickScanPtr = pPtr + 1;
				while ( pQuickScanPtr<pPtrEnd && *(unsigned char*)(pQuickScanPtr)!=13 && *(unsigned char*)(pQuickScanPtr)!=10 && *(unsigned char*)(pQuickScanPtr)!=0 )
				{
					if ( *(pQuickScanPtr)=='"') *(pQuickScanPtr) = ' ';
					pQuickScanPtr++;
				}

				// continue;
				pPtr++;
			}
		}
		pPtr++;
	}

	// Complete
	return true;
}

bool CDBPCompiler::CopyData(LPSTR* ppData, DWORD* pdwDataSize, LPSTR pAdd, DWORD dwAddSize)
{
	// Local vars
	DWORD dwNewSize = 0;

	// New Size of Data
	dwNewSize = (*pdwDataSize) + dwAddSize + 2;

	// Create New Data Memory
	LPSTR pNewData = (LPSTR)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, dwNewSize);
	if(pNewData)
	{
		// Copy Current
		if(*ppData && *pdwDataSize>0)
		{
			CopyMemory(pNewData, *ppData, *pdwDataSize);
		}

		// Copy Add Data
		if(pAdd && dwAddSize>0)
		{
			if(*pdwDataSize==0)
			{
				// Initial data block
				CopyMemory(pNewData, pAdd, dwAddSize);
			}
			else
			{
				// Scan back if zeros
				while(*pdwDataSize>0)
				{
					if(*(pNewData+(*pdwDataSize-2))!=0) break;
					*pdwDataSize = *pdwDataSize - 1;
				}

				// Add carriage return
				*(pNewData+(*pdwDataSize)-1)=13;
				*(pNewData+(*pdwDataSize)+0)=10;

				// Add data block
				CopyMemory(pNewData+(*pdwDataSize)+1, pAdd, dwAddSize);
			}
		}

		// Release current data memory
		if(*ppData)
		{
			GlobalFree(*ppData);
			*ppData=NULL;
		}

		// Reassign pointers
		*ppData = pNewData;
		*pdwDataSize = dwNewSize;
	}
	else
	{
		g_pErrorReport->AddErrorString("Failed to 'GlobalAlloc' in 'CopyData'");
		return false;
	}

	// Complete
	return true;
}

bool CDBPCompiler::SeekIncludeToken(LPSTR* ppPtr, LPSTR pPtrEnd, DWORD* pdwAdvance, LPSTR* ppIncludeFilename)
{
	int iSpeechMark=0;
	LPSTR pStartPtr = (*ppPtr);
	LPSTR pPtr = pStartPtr;
	while(pPtr<pPtrEnd)
	{
		// Search for #INCLUDE token
		if(strnicmp(pPtr, "#include ", 9)==NULL)
		{
			// First char of include token
			LPSTR pFirstChar=pPtr;

			// Skip past include token
			pPtr+=9;

			// Read in include name
			CStr* pName = new CStr("");
			while(pPtr<pPtrEnd)
			{
				// Build name string
				pName->AddChar(*(pPtr));

				// Leave at end of line
				if(*(pPtr-1)==13 && *(pPtr-0)==10) break;
				if(*(pPtr)=='"') iSpeechMark=1-iSpeechMark;
				if(iSpeechMark==0) if(*(pPtr)==':') break;

				// Next character
				pPtr++;
			}

			// Blank out reference to INCLUDE token
			if(*(pPtr)==10) pPtr--;
			for(LPSTR pP=pFirstChar; pP<pPtr; pP++) *(pP)=32;

			// Strip spaces and stuff from name
			pName->EatEdgeSpacesandTabs(NULL);

			// Strip speech marks from name
			pName->EatSpeechMarks();

			// Convert any forwards slashes to backwards slashes
			for(DWORD d=0; d<pName->Length(); d++)
				if(pName->GetChar(d)=='/')
					pName->SetChar(d,'\\');

			// Copy name and return 
			*ppIncludeFilename = new char[pName->Length()+1];
			strcpy(*ppIncludeFilename, pName->GetStr());
			break;
		}

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
						if(*(pPtr-1)==13 && *(pPtr-0)==10) break;
						pPtr++;
					}
				}
			}
		}

		// Advance pointer and counter
		if(pPtr<pPtrEnd) pPtr++;
	}

	// Calculate advance count
	(*ppPtr)=pPtr;
	(*pdwAdvance)=pPtr-pStartPtr;

	// Complete
	return true;
}

bool CDBPCompiler::MakeProgram(void)
{
	db3::CProfile<> prof1("CDBPCompiler::MakeProgram");

	// Make result
	bool bResult=true;

	// Create New Program
	g_pStructTable = new CStructTable;
	g_pASMWriter = new CASMWriter;
	g_pDBMWriter = new CDBMWriter;
	g_pLabelTable = new CLabelTable("*");
	g_pDataTable = new CDataTable;
	g_pStringTable = new CDataTable("*");
	g_pDLLTable = new CDataTable("*");
	g_pCommandTable = new CDataTable("*");
	g_pVarTable = new CVarTable("$_ESP_");
	g_pStatementList = new CStatementList;

	// Set Compile Defaults
	g_pASMWriter->SetDefaultCompileFlags ( m_bSafeArrays );

	// Set struct default
	g_pStructTable->SetStructDefaults();

	// Clear last DBM file
	DeleteFile(GetInternalFile(PATH_TEMPDBMFILE));
	DeleteFile(GetInternalFile(PATH_TEMPEXBFILE));

	// Create CEXE Object
	g_pEXE = new CEXEBlock;

	// Settings for Executable
	g_pEXE->m_dwInitialDisplayMode=1;
	if(g_pDBPCompiler->GetFullScreenMode()) g_pEXE->m_dwInitialDisplayMode=3;
	if(g_pDBPCompiler->GetDesktopMode()) g_pEXE->m_dwInitialDisplayMode=2;
	if(g_pDBPCompiler->GetFullDesktopMode()) g_pEXE->m_dwInitialDisplayMode=4;
	if(g_pDBPCompiler->GetHiddenMode()) g_pEXE->m_dwInitialDisplayMode=0;

	// Start Display Dimensions
	g_pEXE->m_dwInitialDisplayWidth=g_pDBPCompiler->GetStartDisplayWidth();
	g_pEXE->m_dwInitialDisplayHeight=g_pDBPCompiler->GetStartDisplayHeight();
	g_pEXE->m_dwInitialDisplayDepth=g_pDBPCompiler->GetStartDisplayDepth();

	// Create Appname String
	g_pEXE->m_pInitialAppName=g_pDBPCompiler->GetProjectField("app title");
	if ( g_pEXE->m_pInitialAppName==NULL )
	{
		// 280203 - default app name required or exe will not have a classname
		DWORD dwLength = strlen ( m_pFinalDBASource );
		g_pEXE->m_pInitialAppName = new char[dwLength+1];
		strcpy ( g_pEXE->m_pInitialAppName, m_pFinalDBASource );
		if ( dwLength>4 ) g_pEXE->m_pInitialAppName [ dwLength-4 ] = 0;
		if ( strlen ( g_pEXE->m_pInitialAppName )==0 )
		{
			// leefix - 230105 - proj with no appname would corrupt
			SAFE_DELETE(g_pEXE->m_pInitialAppName);
			g_pEXE->m_pInitialAppName = new char[strlen("DB3 Application")+1];
			strcpy ( g_pEXE->m_pInitialAppName, "DB3 Application" );
		}
	}
	
	// IN Debug Mode, cannot have non-desktop mode
	if(g_DebugInfo.DebugModeOn())
	{
		// Switch fullscreen to desktop mode
		if(g_pEXE->m_dwInitialDisplayMode==3 || g_pEXE->m_dwInitialDisplayMode==4)
		{
			// Debug Fullscreen has taskbar to help with debugger switching
			g_pEXE->m_dwInitialDisplayMode=2;
		}
	}

	// Deposit Runtime Strings Database if flagged
	if(1)
	{
		// Entire Runtime Database Part of EXE
		g_pEXE->m_dwNumberOfRuntimeErrorStrings=g_pErrorReport->GetRuntimeErrorStringMax();
		g_pEXE->m_pRuntimeErrorStringsArray = g_pEXE->CreateArray(g_pEXE->m_dwNumberOfRuntimeErrorStrings);
		for(DWORD err=0; err<g_pEXE->m_dwNumberOfRuntimeErrorStrings; err++)
		{
			// Get Data from error runtime string database
			LPSTR pStringData=g_pErrorReport->GetRuntimeErrorString(err);
			
			// Create Dynamic String
			LPSTR pDynamicString=NULL;
			if(pStringData)
			{
				pDynamicString = new char[strlen(pStringData)+1];
				strcpy(pDynamicString, pStringData);
			}

			// Copy to EXEData
			g_pEXE->m_pRuntimeErrorStringsArray[err]=(DWORD)pDynamicString;
		}
	}
	else
	{
		// No Runtime Database Used
		g_pEXE->m_dwNumberOfRuntimeErrorStrings=0;
		g_pEXE->m_pRuntimeErrorStringsArray=NULL;
	}

	// LEE: (a) Can Parse Main Program
	// LEE: (b) Can Parse CLI Mini-programs
	// LEE: (c) fot future, can recompile sections of changed code only..

	// Start With Main Program
	bool bParsingMainProgram = true;

	// CLI can make some temp memory used for mini-program parsing
	DWORD dwMiniSize = 0;
	LPSTR pMiniData = NULL;

	// Skips DBM Data production
	m_bProduceDBMFileOn=true;

	// Parser Loop (does main program, and then any CLI mini-programs)
	bool bBeenInCLI=false;
	bool bContinueParsing=true;
	while(bContinueParsing)
	{
		db3::CProfile<> prof2("CDBPCompiler::MakeProgram() -> Main Loop");

		// Start with parsing in mind
		g_pDBMWriter->SetNewCodeFlag(true);

		// Gather Debug Information on Main Program Only
		g_pStatementList->SetWriteStarted(false);
		if(bParsingMainProgram==true)
		{
			// Load Up Debug Info For Main Program
// leefix - 2450604 - u54 - original program is different size due to CONSTANTS
//			g_DebugInfo.SetProgramSize(m_dwOriginalFileDataSize);
//			g_DebugInfo.SetProgramPtr(m_pOriginalFileData);
			g_DebugInfo.SetProgramSize(m_FileDataSize);
			g_DebugInfo.SetProgramPtr(m_pFileData);
			g_DebugInfo.SetParsingMain(true);

			// Parse Main Program
			db3::CProfile<> prof3("CDBPCompiler::MakeProgram() -> g_pStatementList->MakeStatements()");
			if(!g_pStatementList->MakeStatements(m_pFileData, m_FileDataSize))
			{
				g_pErrorReport->AddErrorString("Failed to 'MakeStatements'");
				bResult=false;
			}
		}
		else
		{
			// Useful flag to some code
			g_DebugInfo.SetParsingMain(false);

			// Load MiniCLI Text from debugger
			SAFE_DELETE(pMiniData);
			g_pASMWriter->GetDataFromDebugger(51, &pMiniData, &dwMiniSize);

			// Parse Mini Program
			if(!g_pStatementList->AddMiniStatements(pMiniData, dwMiniSize))
			{
				// Report Error to Debugger
				LPSTR pData = g_pErrorReport->GetParserErrorString();
				DWORD dwSize = 0;
				if(pData) dwSize = strlen(pData);
				g_pASMWriter->SendDataToDebugger(31, pData, dwSize);

				// Clear miniprogram for empty parse
				g_pDBMWriter->SetNewCodeFlag(false);
				SAFE_DELETE(pMiniData);
				bResult=true;
			}

			// So we dont carry error back to editor
			bBeenInCLI=true;
		}

		// If Parse Successful
		if(bResult==true)
		{
			// Verify all variables use valid types
			if(g_pVarTable->VerifyVariableStructures())
			{
				// Produce EXE or DEBUGRUN from Statements
				db3::CProfile<> prof4("CDBPCompiler::MakeProgram() -> g_pDBMWriter->WriteProgramAsEXEOrDEBUG()");
				if(g_pDBMWriter->WriteProgramAsEXEOrDEBUG(m_pEXEFilename, bParsingMainProgram))
				{
					bResult=true;
				}
				else
				{
					g_pErrorReport->AddErrorString("Failed to 'WriteProgramAs EXEOrDEBUG'");
					bResult=false;
				}
			}
			else
			{
				g_pErrorReport->AddErrorString("Failed to 'VerifyVariableStructures'");
				bResult=false;
			}
		}

		// If Debug Mode Only
		if(bResult==true && g_DebugInfo.DebugModeOn())
		{
			// If BreakForCLI..
			if(g_dwEscapeValueMem!=2)
			{
				// Trigger Mini-Parsing from now on
				bParsingMainProgram=false;
			}
			else
			{
				// Exit Parser-loop
				bContinueParsing=false;
			}
		}
		else
		{
			// Compile only
			bContinueParsing=false;
		}
	}

	// If been in CLI (so editor does not report old errors)
	if(bBeenInCLI==true)
	{
		// Delete error report
		SAFE_DELETE(g_pErrorReport);
		g_pErrorReport = new CError;
	}

	// Free MiniFileData Mem
	SAFE_DELETE(pMiniData);

	// Free The EXE when no more to run
	g_pEXE->FreeUptoDisplay();
	g_pEXE->Free();

	// Delete CEXE Object Here
	SAFE_DELETE(g_pEXE);

	// Free Objects from Statement List First and List itself
	g_pStatementList->GetPreScanStatements()->FreeObjects();
	g_pStatementList->GetProgramStatements()->FreeObjects();
	SAFE_DELETE(g_pStatementList);

	// Free ASMWriter Database
	g_pASMWriter->FreeAll();

	// Free Large Lists
	if(g_pLabelTable)
	{
		g_pLabelTable->Free();
		g_pLabelTable=NULL;
	}
	if(g_pStructTable)
	{
		g_pStructTable->Free();
		g_pStructTable=NULL;
	}
	if(g_pDataTable)
	{
		g_pDataTable->Free();
		g_pDataTable=NULL;
	}
	if(g_pStringTable)
	{
		g_pStringTable->Free();
		g_pStringTable=NULL;
	}
	if(g_pDLLTable)
	{
		g_pDLLTable->Free();
		g_pDLLTable=NULL;
	}
	if(g_pCommandTable)
	{
		g_pCommandTable->Free();
		g_pCommandTable=NULL;
	}
	if(g_pVarTable)
	{
		g_pVarTable->Free();
		g_pVarTable=NULL;
	}

	// Delete All Lists
	SAFE_DELETE(g_pASMWriter);
	SAFE_DELETE(g_pDBMWriter);

	// Complete
	return bResult;
}

bool CDBPCompiler::LoadProjectFile(LPSTR pFilename)
{
	// Release any previous usage
	SAFE_FREE(m_pProjectFileData);

	// Get last six chars of filename
	DWORD length=strlen(pFilename);
	//LPSTR pStrExt = new char[7]; //<-- LOL no
	//strcpy(pStrExt,"");
	char pStrExt[7] = { '\0', };
	if(length>6)
	{
		DWORD readn=length-6;
		for(DWORD n=0; n<6; n++)
		{
			pStrExt[n]=pFilename[readn++];
		}
		pStrExt[n]=0;
	}

	// Check for .DBPRO Extension
	bool bFilenameIsProjectFile=false;
	if(strcmp(pStrExt, ".dbpro")==NULL) bFilenameIsProjectFile=true;
	//SAFE_DELETE(pStrExt);

	// Extract relative path
	m_pRelativePathToProjectFile = new CStr();
	m_pRelativePathToProjectFile->SetText(pFilename);
	m_pRelativePathToProjectFile->TrimToPathOnly();

	// Construct absolute path to project (for media root)
	char pCurrentDir[_MAX_PATH];
	getcwd(pCurrentDir, _MAX_PATH);
	m_pAbsolutePathToProjectFile = new CStr();
	m_pAbsolutePathToProjectFile->SetText(pCurrentDir);
	m_pAbsolutePathToProjectFile->AddText("\\");
	m_pAbsolutePathToProjectFile->AddText(m_pRelativePathToProjectFile->GetStr());

	// Read in project file if it is one
	if(bFilenameIsProjectFile==true)
	{
		// Load into memory
		if(LoadRaw(pFilename, &m_pProjectFileData, &m_ProjectFileDataSize))
		{
			// Eat EdgeSpaces
			CStr* pNewStr = new CStr(m_pProjectFileData);
			pNewStr->EatEdgeSpacesandTabs(NULL);
			ZeroMemory(m_pProjectFileData, m_ProjectFileDataSize);
			strcpy(m_pProjectFileData, pNewStr->GetStr());
			SAFE_DELETE(pNewStr);

			// Project File loaded ok
			m_bProjectExists=true;
		}
		else
		{
			g_pErrorReport->AddErrorString("Failed to 'LoadProjectFile'");
			return false;
		}
	}
	else
	{
		// Returns with no project loaded - still valid
		m_pProjectFileData=NULL;
		m_bProjectExists=false;
		return true;
	}

	// Complete
	return true;
}

bool CDBPCompiler::FreeProjectFile(void)
{
	// Delete DBPCompiler Strings
	SAFE_DELETE(m_pCompilerFilename);
	SAFE_DELETE(m_pCompilerPathOnly);
	SAFE_DELETE(m_pAbsolutePathToProjectFile);
	SAFE_DELETE(m_pRelativePathToProjectFile);

	// Delete POroject Setting Strings
	SAFE_DELETE(m_pFinalDBASource);
	SAFE_DELETE(m_pEXEFilename);

	// Free Project Settings File Data
	SAFE_FREE(m_pProjectFileData);

	return true;
}

LPSTR CDBPCompiler::ReplaceTokens(LPSTR pFilename)
{
	CStr* pNewStr = new CStr("");
	DWORD dwLen=strlen(pFilename);
	for(DWORD n=0; n<dwLen; n++)
	{
		if(strnicmp(pFilename+n, "%temp", 5)==NULL)
		{
			pNewStr->AddText(GetInternalFile(PATH_TEMPFOLDER));
			n+=5;
		}
		pNewStr->AddChar(*(pFilename+n));
	}
	if(pNewStr->Length()>0)
	{
		delete pFilename;
		pFilename = new char[pNewStr->Length()+1];
		strcpy(pFilename, pNewStr->GetStr());
		pFilename[pNewStr->Length()]=0;
	}
	SAFE_DELETE(pNewStr);
	return pFilename;
}

bool CDBPCompiler::GetAllProjectFields(LPSTR pFilename)
{
	// Project or direct file
	if(ProjectExists()==true)
	{
		// Read In Project Settings
		m_pFinalDBASource = GetProjectFile("final source");
		m_bSourceIsMMF = GetProjectState("source as mmf");
		m_pEXEFilename = GetProjectFile("executable");
		m_bDebugModeOn = GetProjectState("CLI");

		// Replace any tokens
		m_pEXEFilename = ReplaceTokens(m_pEXEFilename);

		// Ensure its either absolute path or clean fileonly
		if(m_pEXEFilename[1]!=':')
		{
			// Not absolute, make sire its just the exe
			CStr* pExeOnly = new CStr("");
			pExeOnly->SetText(m_pEXEFilename);
			pExeOnly->Reverse();
			DWORD dwPos = pExeOnly->FindFirstChar('\\');
			if(dwPos>0) pExeOnly->SetChar(dwPos, 0);
			dwPos = pExeOnly->FindFirstChar('/');
			if(dwPos>0) pExeOnly->SetChar(dwPos, 0);
			pExeOnly->Reverse();
			SAFE_DELETE(m_pEXEFilename);
			m_pEXEFilename = new char[pExeOnly->Length()+1];
			strcpy(m_pEXEFilename, pExeOnly->GetStr());
			SAFE_DELETE(pExeOnly);
		}

		// Compiler Display Settings
		m_bFullScreenModeOn = GetProjectStateMatch("graphics mode", "fullscreen");
		m_bDesktopModeOn = GetProjectStateMatch("graphics mode", "desktop");
		m_bFullDesktopModeOn = GetProjectStateMatch("graphics mode", "fulldesktop");
		m_bHiddenModeOn = GetProjectStateMatch("graphics mode", "hidden");
		if ( m_bFullScreenModeOn )
		{
			m_dwStartDisplayWidth = GetProjectDisplayInfo("fullscreen resolution", 1);
			m_dwStartDisplayHeight = GetProjectDisplayInfo("fullscreen resolution", 2);
			m_dwStartDisplayDepth = GetProjectDisplayInfo("fullscreen resolution", 3);
		}
		else
		{
			m_dwStartDisplayWidth = GetProjectDisplayInfo("window resolution", 1);
			m_dwStartDisplayHeight = GetProjectDisplayInfo("window resolution", 2);
			m_dwStartDisplayDepth = GetProjectDisplayInfo("window resolution", 3);
		}

		// Compiler settings that canot be changed externally
		m_bGenerateHelpTxtOn = GetProjectState("generatehelptxt");
		#ifdef MAKEHELPTXT
		// lee - 210406 - can switch this to true to generate new HELPTXT in plugins folder
		m_bGenerateHelpTxtOn = true;
		#endif

		// Compiler General Settings
		m_bRuntimeErrorsOn = GetProjectState("runtimeerrors", true);
		m_bInternalMediaState = GetProjectStateMatch("build type", "media");
		m_bEXEInstallerState = GetProjectStateMatch("build type", "installer");
		m_bEXEAloneState = GetProjectStateMatch("build type", "alone");
		m_bCompressPCKState = GetProjectState("compression"); 
		if(m_bEXEInstallerState)
		{
			// Installer assumes media addition and no safecast pure-exe-stub
			// and also cannot compress installer executables too
			m_bInternalMediaState=true;
			m_bCompressPCKState=false;
			m_bEXEAloneState=false;
		}
		m_bEncryptionState = GetProjectState("encryption"); 
		if(m_bInternalMediaState==false) m_bEncryptionState=false;
		m_bSpeedOverStabilityState = GetProjectState("speed over stability"); 
	}
	else
	{
		// Use filename as DBA Source
		m_bSourceIsMMF=false;
		DWORD length=strlen(pFilename);
		m_pFinalDBASource = new char[length+1];
		strcpy(m_pFinalDBASource, pFilename);
		m_pEXEFilename = new char[_MAX_PATH];
		strcpy(m_pEXEFilename, "default.exe");
		m_bDebugModeOn=false;
		m_bRuntimeErrorsOn=true;
		m_bProduceDBMFileOn=true;
		m_bFullScreenModeOn=false;
		m_bFullDesktopModeOn=true;
		m_bDesktopModeOn=false;
		m_dwStartDisplayWidth=640;
		m_dwStartDisplayHeight=480;
		m_dwStartDisplayDepth=32;
		m_bHiddenModeOn=false;
		m_bEXEAloneState=false;
		m_bEXEInstallerState=false; 
		m_bCompressPCKState=false;
		m_bInternalMediaState=false;
		m_bEncryptionState=false;
		m_bSpeedOverStabilityState=false;
		m_bGenerateHelpTxtOn=false;
		#ifdef MAKEHELPTXT
		// lee - 210406 - can switch this to true to generate new HELPTXT in plugins folder
		m_bGenerateHelpTxtOn = true;
		#endif
	}
	return true;
}

bool CDBPCompiler::GetProjectState(LPSTR pFieldName, bool bDefault)
{
	bool bState=bDefault;
	LPSTR pState = GetProjectField(pFieldName);
	if(pState)
	{
		if(stricmp(pState,"yes")==NULL)
			bState=true;
		else
			bState=false;

		SAFE_DELETE(pState);
	}
	return bState;
}

bool CDBPCompiler::GetProjectState(LPSTR pFieldName)
{
	return GetProjectState(pFieldName, false);
}

bool CDBPCompiler::GetProjectStateMatch(LPSTR pFieldName, LPSTR pCompareStr)
{
	bool bState=false;
	LPSTR pState = GetProjectField(pFieldName);
	if(pState)
	{
		if(stricmp(pState,pCompareStr)==NULL)
			bState=true;
		else
			bState=false;

		SAFE_DELETE(pState);
	}
	return bState;
}

DWORD CDBPCompiler::GetProjectDisplayInfo(LPSTR pFieldName, DWORD dwDisplayItem)
{
	DWORD dwDisplayData=0;
	LPSTR pState = GetProjectField(pFieldName);
	if(pState)
	{
		// U75 - 260210 - old editor used 0,0 for window resolution
		// whereas new editor uses 0x0, so handle both
		bool bOldStyleResolution = false;
		for ( int n=0; n<(int)strlen(pState); n++)
			if ( pState[n]==',' )
				bOldStyleResolution = true;

		if(dwDisplayItem==1 || dwDisplayItem==2)
		{
			// U75 - 260210 - old editor or new editor
			DWORD w, h;
			if ( bOldStyleResolution==true )
				sscanf(pState, "%d,%d", &w, &h);
			else
				sscanf(pState, "%dx%d", &w, &h);
			if(dwDisplayItem==1) dwDisplayData=w;
			if(dwDisplayItem==2) dwDisplayData=h;
		}
		else
		{
			// U75 - 260210 - old editor or new editor
			DWORD w, h;
			char depth[32];
			if ( bOldStyleResolution==true )
				sscanf(pState, "%d,%d,%s", &w, &h, depth);
			else
				sscanf(pState, "%dx%dx%s", &w, &h, depth);
			if(stricmp(depth, "16")==NULL) dwDisplayData=16;
			if(stricmp(depth, "16M")==NULL) dwDisplayData=32;
		}
		SAFE_DELETE(pState);
	}
	if(dwDisplayData==0)
	{
		if(dwDisplayItem==1) dwDisplayData=640;
		if(dwDisplayItem==2) dwDisplayData=480;
		if(dwDisplayItem==3) dwDisplayData=32;
	}
	return dwDisplayData;
}

LPSTR CDBPCompiler::GetProjectFile(LPSTR pFieldName)
{
	// Find filename
	LPSTR pFileOnly = GetProjectField(pFieldName);

	// Create Full Path and File
	CStr TempStr;
	TempStr.SetText(m_pRelativePathToProjectFile->GetStr());
	TempStr.AddText(pFileOnly);

	// Create new string
	LPSTR pFullFile = new char[TempStr.Length()+1];
	strcpy(pFullFile, TempStr.GetStr());

	// Free usages
	SAFE_DELETE(pFileOnly);

	// Return new string
	return pFullFile;
}

LPSTR CDBPCompiler::GetProjectField(LPSTR pFieldName)
{
	LPSTR pPointer=m_pProjectFileData;
	LPSTR pPointerEnd=m_pProjectFileData+m_ProjectFileDataSize;
	while(pPointer<pPointerEnd)
	{
		// Eat CRs, tabs and spaces
		while(*(pPointer)==13 || *(pPointer)==10 || *(pPointer)==9 || *(pPointer)==32)
		{
			while(*(pPointer)==13) pPointer++;
			while(*(pPointer)==10) pPointer++;
			while(*(pPointer)==9) pPointer++;
			while(*(pPointer)==32) pPointer++;
		}

		// If comment, skip all line
		if(*(pPointer)==';')
		{
			// Skip comment text
			pPointer++;
			while(pPointer<pPointerEnd)
			{
				if(*(pPointer-1)==13 && *(pPointer-0)==10) { pPointer++; break; }
				pPointer++;
			}
			if(pPointer>=pPointerEnd) break;
		}
		else
		{
			bool bLineIsValid=true;

			// Mark start of field
			LPSTR pStartOfField = pPointer;

			// Scan to equals
			if(bLineIsValid)
			{
				while(pPointer<pPointerEnd)
				{
					if(*(pPointer)=='=') break;
					pPointer++;
				}
				if(pPointer>=pPointerEnd) break;
			}

			// Mark end of field
			LPSTR pEndOfField = pPointer;
			pPointer++;
			LPSTR pStartOfDataItem = pPointer;

			// Scan to end of line
			while(pPointer<pPointerEnd)
			{
				if(*(pPointer-1)==13 && *(pPointer+0)==10) { pPointer++; break; }
				pPointer++;
			}

			// Mark end of dataitem
			LPSTR pEndOfDataItem = pPointer-2;

			if(bLineIsValid)
			{
				// Copy out dataitem if it matches seatch criteria
				DWORD length = pEndOfField-pStartOfField;
				LPSTR pFound = new char[length+1];
				memcpy(pFound, pStartOfField, length);
				pFound[length]=0;
				if(stricmp(pFound, pFieldName)==NULL)
				{
					// Field has been found, make data string and exit
					DWORD datalength = pEndOfDataItem-pStartOfDataItem;
					LPSTR pDataItemString = new char[datalength+1];
					memcpy(pDataItemString, pStartOfDataItem, datalength);
					pDataItemString[datalength]=0;
					SAFE_DELETE(pFound);
					if(pDataItemString)
						if(strcmp(pDataItemString,"")==NULL)
							SAFE_DELETE(pDataItemString);

					return pDataItemString;
				}
				SAFE_DELETE(pFound);
			}

			// if no more data, leave
			if(pPointer>=pPointerEnd) break;
		}
	}
	return NULL;
}

void CDBPCompiler::SetInternalFile(DWORD dwFileID, LPSTR pFilename)
{
	if(m_pInternalFile[dwFileID]==NULL) m_pInternalFile[dwFileID] = new CStr("");
	m_pInternalFile[dwFileID]->SetText(pFilename);
}

LPSTR CDBPCompiler::GetInternalFile(DWORD dwFileID)
{
	if(m_pInternalFile[dwFileID])
		return m_pInternalFile[dwFileID]->GetStr();
	else
		return NULL;
}

bool CDBPCompiler::FileExists(LPSTR pFilename)
{
	HANDLE hFile = CreateFile(pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile!=INVALID_HANDLE_VALUE)
	{
		// Close File
		SAFE_CLOSE(hFile);
		return true;
	}
	// soft fail
	return false;
}

bool CDBPCompiler::PathExists(LPSTR pPath)
{
	// State var
	bool bExists=false;

	// Get current folder
	char old[_MAX_PATH];
	_getcwd(old, _MAX_PATH);

	// Try to switch to pPath
	if(_chdir(pPath)==0) bExists=true;

	// Restore dir
	_chdir(old);

	return bExists;
}

void CDBPCompiler::GatherAllExternalWords(LPSTR pWordsFile)
{
	// Read All External Words From Words File
	int id=0;
	while(id<EXTWORDSMAX)
	{
		// create id
		char idstr[8];
		itoa(id, idstr, 10);

		// get word from id
		GetPrivateProfileString("WORDS", idstr, "", m_pWord[id], _MAX_PATH, pWordsFile);

		// next word
		id++;
	}
}

LPSTR CDBPCompiler::GetWord ( int iID )
{
	return m_pWord [ iID ];
}

bool CDBPCompiler::EstablishRequiredBaseFiles(void)
{
	// Temp Strings
	static char missing[8192];
	char path[_MAX_PATH];
	char textfiles[_MAX_PATH];
	bool filesAreMissing = false;
#define CHECK_MISSING_FILEPATH(which)\
	if ((which==0 && !FileExists(path)) || (which==1 && !PathExists(path)))\
	{\
		if(filesAreMissing)\
		{\
			filesAreMissing=true;\
			strcat_s(missing, ", ");\
		}\
		strcat_s(missing, path);\
	}
#define CHECK_MISSING_FILE() CHECK_MISSING_FILEPATH(0)
#define CHECK_MISSING_PATH() CHECK_MISSING_FILEPATH(1)

	strcpy_s(missing, "Some files appear to be missing from your installation: ");

	// Root Folder to Compiler (not necessarily current dir)
	SetInternalFile(PATH_ROOTPATH, m_pCompilerPathOnly->GetStr());

	// Path to Test Files
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "SETUP.INI");
	SetInternalFile(PATH_SETUPFILE, path);
	CHECK_MISSING_FILE();

	// Get Path to Language Folder
	GetPrivateProfileString("SETTINGS", "TEXTLANGUAGE", "ENGLISH", textfiles, 256, path);
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "LANG");
	strcat(path, "\\");
	strupr(textfiles);
	strcat(path, textfiles);
	strcat(path, "\\ERRORS.TXT");
	SetInternalFile(PATH_ERRORSFILE, path);
	CHECK_MISSING_FILE();

	// Get Path to WORDS File
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "SETUP.INI");
	GetPrivateProfileString("SETTINGS", "TEXTLANGUAGE", "ENGLISH", textfiles, 256, path);
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "LANG");
	strcat(path, "\\");
	strupr(textfiles);
	strcat(path, textfiles);
	strcat(path, "\\WORDS.TXT");

	// GATHER ALL EXTERNAL WORDS
	GatherAllExternalWords(path);

	// Get All Exclusion Names (DLLs we shall not include)
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "SETUP.INI");
	g_dwExcludeFilesCount=0;
	memset ( g_pExcludeFiles, 0, MAX_EXCLUSIONS * sizeof(LPSTR) );
	for ( int i=1; i<MAX_EXCLUSIONS; i++)
	{
		char pExDLL[256];
		wsprintf ( pExDLL, "exdll%d", i );
		GetPrivateProfileString("EXCLUSIONS", pExDLL, "", textfiles, 256, path);
		if ( strcmp ( textfiles, "" )!=NULL )
		{
			DWORD dwSize = strlen ( textfiles )+1;
			g_pExcludeFiles [ i ] = new char [ dwSize ];
			strcpy ( g_pExcludeFiles [ i ], textfiles );
			g_dwExcludeFilesCount = 1+i;
		}
		else
			break;
	}

	//
	//	TODO: Move this part to somewhere more appropriate...
	//

	// Get multi-threading options
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "SETUP.INI");
	GetPrivateProfileString("MULTITHREADING", "ThreadCount", "0", textfiles, sizeof(textfiles), path);
	if (!g_WorkQueue.Init( static_cast<db3::uint>(atoi(textfiles)) ))
	{
		MessageBox( GetActiveWindow(), "Failed to initialize work queue", "Error", MB_ICONERROR|MB_OK );
		return false;
	}

	// === TESTING ===
#if 0
	// indicate the number of threads used
	sprintf_s(textfiles, "%u", static_cast<unsigned int>(m_WorkQueue.GetThreadCount()));
	MessageBoxA(0, textfiles, "Thread Count", 64);

	// provide some work
	db3::CSignal sig;
	const char *items[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P" };
	void(*func)(const char *) = [](const char *text)->void{MessageBoxA(0, text, "Worker", 64);};
	for(db3::uint i=0; i<sizeof(items)/sizeof(items[0]); i++)
	{
		m_WorkQueue.Enqueue(func, items[i], &sig);
	}
	sig.Sync();
	//m_WorkQueue.Sync();
	//m_WorkQueue.Enqueue([&](char *text)->void{MessageBoxA(0, text, "Worker", 64);},textfiles);
#endif

	// Get all directives from the global directives SETUP.INI
	m_bRemoveSafetyCode = false;
	m_bSafeArrays = false;
	g_bLocalTempFolder = false;
	g_bExternaliseDLLS = false;
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "SETUP.INI");
	GetPrivateProfileString("DIRECTIVES", "RemoveSafetyCode", "no", textfiles, 256, path);
	if ( stricmp ( textfiles, "yes" )==NULL ) m_bRemoveSafetyCode = true;
	GetPrivateProfileString("DIRECTIVES", "SafeArrays", "yes", textfiles, 256, path);
	if ( stricmp ( textfiles, "yes" )==NULL ) m_bSafeArrays = true;

	// lee - 050406 - u6rc6 - new diretive
	GetPrivateProfileString("DIRECTIVES", "LocalTempFolder", "no", textfiles, 256, path);
	if ( stricmp ( textfiles, "yes" )==NULL ) g_bLocalTempFolder = true;
	
	// lee - 270308 - u67 - can externalise all DLLs to make exe smaller and rely on outside DLLs being dropped in
	GetPrivateProfileString("DIRECTIVES", "ExternaliseDLLS", "no", textfiles, 256, path);
	if ( stricmp ( textfiles, "yes" )==NULL ) g_bExternaliseDLLS = true;

	// Get Path to Debugger Program
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "\\DBPDebugger.exe");
	SetInternalFile(PATH_DEBUGGERFILE, path);
	CHECK_MISSING_FILE();
	
	// Get Path to PLUGINS Folder
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "plugins\\");
	SetInternalFile(PATH_PLUGINSFOLDER, path);
	CHECK_MISSING_PATH();

	// Get Path to PLUGINS-USER Folder
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "plugins-user\\");
	SetInternalFile(PATH_PLUGINSUSERFOLDER, path);
	CHECK_MISSING_PATH();

	// Get Path to PLUGINS-LICENSED Folder
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "plugins-licensed\\");
	SetInternalFile(PATH_PLUGINSLICENSEDFOLDER, path);
	CHECK_MISSING_PATH();

	// Get Path to TEMP Folder
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "..\\temp\\");
	SetInternalFile(PATH_TEMPFOLDER, path);

	// U69 - 290508 - if TEMP folder removed, try the USER temp location
	char storethisone [ 260 ];
	_getcwd ( storethisone, 260 );
	bool bUseUserFolderForTemp = false;
	if(PathExists(GetInternalFile(PATH_TEMPFOLDER))==false) bUseUserFolderForTemp = true;
	if(PathExists(GetInternalFile(PATH_TEMPFOLDER))==true)
	{
		// if DBPRO\TEMP exists, make sure we can WRITE to it, otherwise use USER temp
		_chdir ( GetInternalFile(PATH_TEMPFOLDER) );
		HANDLE hFile = CreateFile( "_temp.temp", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile==INVALID_HANDLE_VALUE )
		{
			// no create access to temp - do not use
			bUseUserFolderForTemp = true;
		}
		else
		{
			// remove - successful - temp file valid to use
			CloseHandle( hFile );
			DeleteFile ( "_temp.temp" );
		}
	}
	if ( bUseUserFolderForTemp==true )
	{
		// U69 - create new USER TEMP folder
		_chdir ( storethisone );
		SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path );
		LPSTR pDBProTEMP = "Dark Basic Professional TEMP";
		if ( _chdir ( pDBProTEMP )==-1 )
		{
			// make it if not present
			_mkdir ( pDBProTEMP );
			_chdir ( pDBProTEMP );
		}
		_getcwd ( path, 260 );
		strcat ( path, "\\" );
		SetInternalFile(PATH_TEMPFOLDER, path);
	}
	_chdir ( storethisone );
	CHECK_MISSING_PATH();

	// Get Path to DBM File
	strcpy(path, GetInternalFile(PATH_TEMPFOLDER));
	strcat(path, "_Temp.dbm");
	SetInternalFile(PATH_TEMPDBMFILE, path);

	// Get Path to DBM File
	strcpy(path, GetInternalFile(PATH_TEMPFOLDER));
	strcat(path, "_Temp.exb");
	SetInternalFile(PATH_TEMPEXBFILE, path);

	// Get Path to DBM File
	strcpy(path, GetInternalFile(PATH_TEMPFOLDER));
	strcat(path, "ErrorReport.txt");
	SetInternalFile(PATH_TEMPERRORFILE, path);

	// Get Path to DBM File
	strcpy(path, GetInternalFile(PATH_ROOTPATH));
	strcat(path, "plugins\\");
	strcat(path, "compress.dll");
	SetInternalFile(PATH_COMPRESSDLLFILE, path);
	CHECK_MISSING_FILE();

	// Verify Importanr Files Exist
#if 0
	bool bAllFilesAvailable=true;
	if(FileExists(GetInternalFile(PATH_SETUPFILE))==false) bAllFilesAvailable=false;
	if(FileExists(GetInternalFile(PATH_ERRORSFILE))==false) bAllFilesAvailable=false;
	if(FileExists(GetInternalFile(PATH_DEBUGGERFILE))==false) bAllFilesAvailable=false;
	if(PathExists(GetInternalFile(PATH_PLUGINSFOLDER))==false) bAllFilesAvailable=false;
	if(PathExists(GetInternalFile(PATH_PLUGINSUSERFOLDER))==false) bAllFilesAvailable=false;
	if(PathExists(GetInternalFile(PATH_PLUGINSLICENSEDFOLDER))==false) bAllFilesAvailable=false;
	if(PathExists(GetInternalFile(PATH_TEMPFOLDER))==false) bAllFilesAvailable=false;
	if(FileExists(GetInternalFile(PATH_COMPRESSDLLFILE))==false) bAllFilesAvailable=false;
#else
	bool bAllFilesAvailable = !filesAreMissing;
#endif
	if(bAllFilesAvailable==false)
	{
		//
		//	TODO: Make this error more descriptive.
		//

		// Not all files could be found
#if 0
		char missing[32];
		wsprintf ( missing, "%s\nStatus:%d%d%d%d%d%d%d%d",	GetInternalFile(PATH_TEMPFOLDER),//g_pDBPCompiler->GetWordString(8),
															(int)FileExists(GetInternalFile(PATH_SETUPFILE)),
															(int)FileExists(GetInternalFile(PATH_ERRORSFILE)),
															(int)FileExists(GetInternalFile(PATH_DEBUGGERFILE)),
															(int)PathExists(GetInternalFile(PATH_PLUGINSFOLDER)),
															(int)PathExists(GetInternalFile(PATH_PLUGINSUSERFOLDER)),
															(int)PathExists(GetInternalFile(PATH_PLUGINSLICENSEDFOLDER)),
															(int)PathExists(GetInternalFile(PATH_TEMPFOLDER)),
															(int)FileExists(GetInternalFile(PATH_COMPRESSDLLFILE))
															);
#endif
		// Not all internal files exist for compiler
		MessageBoxA(NULL, missing, g_pDBPCompiler->GetWordString(9), MB_OK|MB_ICONERROR);
		return false;
	}

	// Complete
	return true;
}

bool CDBPCompiler::RemoveAndRecordBreakpoints(void)
{
	db3::CProfile<> prof("CDBPCompiler::RemoveAndRecordBreakpoints");

	// Temp Vars
	DWORD dwLine=1;
	DWORD dwSpeechMarks=0;

	// Clear BP list
	ClearBreakPointList();

	// Scan entire filedata, and record/remove all ! symbols
	LPSTR pPtr = m_pFileData;
	LPSTR pEnd = m_pFileData+m_FileDataSize;
	while(pPtr<pEnd)
	{
		if(*pPtr=='"') dwSpeechMarks=1-dwSpeechMarks;
		if(dwSpeechMarks==0)
		{
			if(*pPtr=='!')
			{
				// leefix-060803-before remove, check if left-hand neighbor is carriage return or program start
				bool bMustBeAtStartOfLine=false;
				if ( pPtr==m_pFileData )
				{
					// leefix-090903-step-through-bug - ! at first char no longer causes bug
					bMustBeAtStartOfLine=true;
				}
				else
				{
					for ( LPSTR pCheckPtr=pPtr-1; pCheckPtr>=m_pFileData; pCheckPtr-- )
					{
						if ( *pCheckPtr==10 || *pCheckPtr==13 || pCheckPtr==m_pFileData )
						{
							bMustBeAtStartOfLine=true;
							break;
						}
						if ( *pCheckPtr!=9 && *pCheckPtr!=32 )
							break;
					}
				}

				// only if start of line
				if ( bMustBeAtStartOfLine )
				{
					// remove symbol
					*pPtr=32;

					// record line number in breakpint list
					AddToBreakPointList(dwLine);
				}
			}
		}
		if(*(pPtr-1)==13 && *pPtr==10) dwLine++;
		pPtr++;
	}

	// Complete List 
	FinishBreakPointList();

	// Complete
	return true;
}

bool CDBPCompiler::ClearBreakPointList(void)
{
	SAFE_DELETE(m_pBreakpointList);
	m_dwBreakpointSize = m_FileDataSize;
	m_pBreakpointList = new DWORD[m_dwBreakpointSize+1];
	ZeroMemory(m_pBreakpointList, sizeof(DWORD)*(m_dwBreakpointSize+1));
	m_dwBreakpointIndex = 0;
	return true;
}

bool CDBPCompiler::AddToBreakPointList(DWORD dwLine)
{
	if(m_dwBreakpointIndex<m_dwBreakpointSize)
	{
		bool bLineIsUnique=true;
		if(m_dwBreakpointIndex>0)
			if(m_pBreakpointList[m_dwBreakpointIndex-1]==dwLine) bLineIsUnique=false;

		if(bLineIsUnique)
		{
			m_pBreakpointList[m_dwBreakpointIndex]=dwLine;
			m_dwBreakpointIndex++;
		}
		return true;
	}
	else
		return false;
}

bool CDBPCompiler::FinishBreakPointList(void)
{
	m_dwBreakpointMax = m_dwBreakpointIndex;
	m_dwBreakpointIndex = 0;
	return true;
}
