#include "StdAfx.h"
#include "CompileSupport.h"

#include "WriteProject.h"
#include "WriteFiles.h"
#include "Variables.h"
#include "Utilities.h"
#include "ReadError.h"
#include "MainFrm.h"
#include "Settings.h"

bool CompileSupport::m_run;
bool CompileSupport::m_IsCompiling;
CString CompileSupport::m_strHelpFile;

void CompileSupport::Compile(bool run)
{
	m_IsCompiling = true;
	m_run = run;

	if(Variables::m_IncludeFiles.size() == 0)
	{
		AfxMessageBox(_T("There are no files in your project, add a file to your project before attempting to compile"));
		m_IsCompiling = false;
		return;
	}

	MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
	if(Settings::ClearOutputOnCompile)
	{
		pMainWnd->ClearOutputText();
	}

    // IRM 20090910 - Give feedback for compilation start.
    CString str(_T("Compiling project..."));
    int size = str.GetLength() + 1;
    TCHAR* msg = new TCHAR[size];
    wcscpy_s(msg, size, str);
    pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msg);

    pMainWnd->ShowOutput();
	#ifndef NONVERBOSEOUTPUTMODE
	pMainWnd->AddOutputText(_T("------------"));
	#endif

	if(!WriteProject::Write(true))
	{
		m_IsCompiling = false;
		return;
	}

	if(!WriteFiles::Write())
	{
		m_IsCompiling = false;
		return;
	}

	if(!Utilities::CheckFileExists(Settings::DBPLocation + L"Compiler\\DBPCompiler.exe"))
	{
		AfxMessageBox(_T("Could not find DBPCompiler.exe, the path stored in your registry or settings file is invalid. Please locate the DarkBASIC Professional compiler in the options and ensure DarkBASIC Professional is correctly installed."));
		m_IsCompiling = false;
		return;
	}

	// Clean up compiler report file
	ReadError::CleanUp();

	// Construct project execution path
	CString output = L"Executing " + Settings::DBPLocation + L"Compiler\\DBPCompiler.exe " + Utilities::GetTemporaryPath() + L"Temp.dbpro";
	#ifndef NONVERBOSEOUTPUTMODE
	pMainWnd->AddOutputText(output);
	#endif

	AfxBeginThread(CompileThread, pMainWnd);	
}

void CompileSupport::CompileHelp(bool run)
{
	m_IsCompiling = true;
	m_run = run;

	MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
	if(Settings::ClearOutputOnCompile)
	{
		pMainWnd->ClearOutputText();
	}
	pMainWnd->ShowOutput();
	#ifndef NONVERBOSEOUTPUTMODE
	pMainWnd->AddOutputText(_T("------------"));
	#endif

	if(!WriteProject::WriteHelp())
	{
		m_IsCompiling = false;
		return;
	}

	if(!WriteFiles::WriteHelp())
	{
		m_IsCompiling = false;
		return;
	}

	if(!Utilities::CheckFileExists(Settings::DBPLocation + L"Compiler\\DBPCompiler.exe"))
	{
		AfxMessageBox(_T("Could not find DBPCompiler.exe, the path stored in your registry or settings file is invalid. Please locate the DarkBASIC Professional compiler in the options and ensure DarkBASIC Professional is correctly installed."));
		m_IsCompiling = false;
		return;
	}

	// Clean up compiler report file
	ReadError::CleanUp();

	// Construct project execution path
	CString output = L"Executing " + Settings::DBPLocation + L"Compiler\\DBPCompiler.exe " + Utilities::GetTemporaryPath() + L"Temp.dbpro";
	#ifndef NONVERBOSEOUTPUTMODE
	pMainWnd->AddOutputText(output);
	#endif

	AfxBeginThread(CompileThread, pMainWnd);	
}

UINT CompileSupport::CompileThread(LPVOID pParam)
{	
	MainFrame* pMainWnd = (MainFrame*)pParam;

	// Tell status bar the number of lines in the project (max value)
	pMainWnd->SendMessage(WM_USER + 1, Variables::m_NumberOfLines, 0);

	// Execute structures
	STARTUPINFO         siStartupInfo;
    PROCESS_INFORMATION piProcessInfo;

	// File to run at each stage
	CString runExe;
	TCHAR exeAr[MAX_PATH];
    
	// Accelerator flags
	DWORD dwCreationFlags = CREATE_DEFAULT_ERROR_MODE;
	if(Settings::AllowCompilerAccelerator)
	{
		dwCreationFlags = dwCreationFlags | ABOVE_NORMAL_PRIORITY_CLASS;
	}
	
	
	// Clear memory
	ZeroMemory( &siStartupInfo, sizeof(siStartupInfo) );
	siStartupInfo.cb = sizeof(siStartupInfo);
	ZeroMemory( &piProcessInfo, sizeof(piProcessInfo) );
	ZeroMemory( &exeAr, sizeof(exeAr) );

	// Prepare path
	runExe = Settings::DBPLocation + L"Compiler\\DBPCompiler.exe Temp.dbpro";	
	wcscpy_s(exeAr, MAX_PATH, runExe);

	// leefix - 300908 - ensure compiler does not wipe out HelpFile string
	CString StoreHelpFileString = m_strHelpFile;

	// U75 - 231009 - delete EXE if exists before compile
	// Run if asked and no error (copied from below)
	if(1)
	{
		CString pathRun;
		if(m_strHelpFile == "")
		{
			if(Utilities::ExtractFilename(Variables::m_DBPropertyExe) == Variables::m_DBPropertyExe)
			{
				pathRun = Variables::m_ProjectPath; // Exe is in the same location as the project
				runExe = Variables::m_ProjectPath + Variables::m_DBPropertyExe;
			}
			else
			{
				if(Utilities::ExtractPath(Variables::m_DBPropertyExe) == Utilities::GetTemporaryPath() )
				{
					char pCurrDir[512];
					GetCurrentDirectoryA(512,pCurrDir);
					pathRun = CString(pCurrDir);
					runExe = Variables::m_DBPropertyExe;
				}
				else
				{
					pathRun = Utilities::ExtractPath(Variables::m_DBPropertyExe);
					runExe = Variables::m_DBPropertyExe;
				}
			}
		}
		else
		{
			pathRun = Utilities::ExtractPath(m_strHelpFile);
			runExe = Utilities::GetTemporaryPath() + _T("Temp.exe");
		}
		::DeleteFile ( runExe );
	}

	// Run compiler
	CreateProcess(NULL, exeAr, 0, 0, FALSE, dwCreationFlags, 0, Utilities::GetTemporaryPath(), &siStartupInfo, &piProcessInfo);
	if(Settings::AllowReadCompilerStatus)
	{
		HANDLE fileMap = NULL;
		int* fileView = NULL;
		int lastR = 0;
		DWORD retVal;
		while(GetExitCodeProcess(piProcessInfo.hProcess, &retVal) && retVal == STILL_ACTIVE)
		{
			if(fileMap == NULL)
			{
				fileMap = OpenFileMapping(0x0004, true, _T("DBPROEDITORMESSAGE"));
			}
			if(fileMap != NULL)
			{		
				if(fileView == NULL)
				{
					fileView = (int*)MapViewOfFile(fileMap, SECTION_MAP_READ, 0, 0, 16);
				}
				if(fileView != NULL)
				{
					if(fileView[0] != lastR)
					{
						pMainWnd->SendMessage(WM_USER + 2, fileView[0], 0);
						lastR = fileView[0];
					}
				}
			}
			Sleep(10); // Don't overwhelm the system
		}
		if(fileView != NULL)
		{
			UnmapViewOfFile(fileView);
		}
		if(fileMap != NULL)
		{			
			CloseHandle(fileMap);
		}
	}
	else
	{
		WaitForSingleObject(piProcessInfo.hProcess, INFINITE); 
	}
	pMainWnd->SendMessage(WM_USER + 2, -1, 0); // Reset status bar

	// Close process and thread handles
	CloseHandle(piProcessInfo.hThread);
	CloseHandle(piProcessInfo.hProcess);

	m_IsCompiling = false;

	// leefix - 300908 - ensure compiler does not wipe out HelpFile string (stored from above)
	m_strHelpFile = StoreHelpFileString;

	// Clean up
	#ifndef NONVERBOSEOUTPUTMODE
	CString strCleanUp = _T("Cleaning up");	
	TCHAR* msgCleanUp = new TCHAR[strCleanUp.GetLength() + 1];
	wcscpy_s(msgCleanUp, strCleanUp.GetLength() + 1, strCleanUp);
	pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msgCleanUp);
	#endif

#ifndef _DEBUG
	::DeleteFile(Utilities::GetTemporaryPath() + _T("Temp.dbpro"));
	::DeleteFile(Utilities::GetTemporaryPath() + _T("_Temp.dbsource"));
#endif

	// Get error
	bool result = ReadError::Read();

	// Run if asked and no error
	if(result && m_run)
	{
		CString pathRun;
		if(m_strHelpFile == "")
		{			
			if(Utilities::ExtractFilename(Variables::m_DBPropertyExe) == Variables::m_DBPropertyExe)
			{
				pathRun = Variables::m_ProjectPath; // Exe is in the same location as the project
				runExe = Variables::m_ProjectPath + Variables::m_DBPropertyExe;
			}
			else
			{
				// leefix - 300908 - when run TEMP.EXE in TEMP folder, should use CURRENT FOLDER not TEMP FOLDER as CWD
				if(Utilities::ExtractPath(Variables::m_DBPropertyExe) == Utilities::GetTemporaryPath() )
				{
					char pCurrDir[512];
					GetCurrentDirectoryA(512,pCurrDir);
					pathRun = CString(pCurrDir);
					runExe = Variables::m_DBPropertyExe;
				}
				else
				{
					// leenote - not sure what scenario for this might be?
					pathRun = Utilities::ExtractPath(Variables::m_DBPropertyExe); // Exe path contains the path
					runExe = Variables::m_DBPropertyExe;
				}
			}
		}
		else
		{
			pathRun = Utilities::ExtractPath(m_strHelpFile);
			runExe = Utilities::GetTemporaryPath() + _T("Temp.exe");
		}
		if(Variables::m_CommandLineArgs != L"")
		{
			runExe += _T(" ") + Variables::m_CommandLineArgs;
		}

		ZeroMemory( &siStartupInfo, sizeof(siStartupInfo) );
		siStartupInfo.cb = sizeof(siStartupInfo);
		ZeroMemory( &piProcessInfo, sizeof(piProcessInfo) );
		ZeroMemory( &exeAr, sizeof(exeAr) );
		wcscpy_s(exeAr, MAX_PATH, runExe);

		CreateProcess(NULL, exeAr, 0, 0, FALSE, CREATE_DEFAULT_ERROR_MODE, 0, pathRun, &siStartupInfo, &piProcessInfo);

		WaitForSingleObject(piProcessInfo.hProcess, INFINITE);

		// Close process and thread handles
		CloseHandle(piProcessInfo.hThread);
		CloseHandle(piProcessInfo.hProcess);
	}

	#ifndef NONVERBOSEOUTPUTMODE
 	CString strDots = _T("------------");
	TCHAR* msgDots = new TCHAR[strDots.GetLength() + 1];
	wcscpy_s(msgDots, strDots.GetLength() + 1, strDots);
	pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msgDots);	
	#endif

	return 0;
}
