#include "stdafx.h"
#include "ReadError.h"
#include "Settings.h"
#include "MainFrm.h"
#include "Variables.h"
#include "Utilities.h"
#include "CompileSupport.h"

void ReadError::CleanUp()
{
      CString Loc = Settings::DBPLocation + _T("TEMP\\ErrorReport.txt");
      TCHAR szPath[MAX_PATH];
     
      // Delete standard file
      if(Utilities::CheckFileExists(Loc))
      {
            DeleteFileW(Loc);
      }
 
      // Delete virtualized file
      if(Utilities::GetWindowsMajor() > 5 && SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath)))
      {
            PathAppend(szPath, L"VirtualStore\\");
            PathAppend(szPath, Settings::DBPLocation.Mid(3));
            PathAppend(szPath, L"TEMP\\ErrorReport.txt");
            Loc = szPath;
            if(Utilities::CheckFileExists(Loc))
            {
                  DeleteFileW(Loc);
            }
      }
 
      // Delete temp file
      if(Utilities::CheckFileExists(Utilities::GetTemporaryPath() + L"Dark Basic Professional TEMP\\ErrorReport.txt"))
      {
            Loc = Utilities::GetTemporaryPath() + L"Dark Basic Professional TEMP\\ErrorReport.txt";
            if(Utilities::CheckFileExists(Loc))
            {
                  DeleteFileW(Loc);
            }
      }
}
 
bool ReadError::Read()
{
      MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
 
      CString Loc = Settings::DBPLocation + _T("TEMP\\ErrorReport.txt");
 
      TCHAR szPath[MAX_PATH];
      CString temp;
 
      CString virtualised = L"";
      if(Utilities::GetWindowsMajor() > 5 && SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath)))
      {
            PathAppend(szPath, L"VirtualStore\\");
            PathAppend(szPath, Settings::DBPLocation.Mid(3));
            PathAppend(szPath, L"TEMP\\ErrorReport.txt");
            temp = szPath;
            if(Utilities::CheckFileExists(temp))
            {
                  virtualised = temp;
            }
      }
 
      if(!virtualised.IsEmpty())
      {
            Loc = virtualised;
      }
      else if(Utilities::CheckFileExists(Utilities::GetTemporaryPath() + L"Dark Basic Professional TEMP\\ErrorReport.txt"))
      {
            Loc = Utilities::GetTemporaryPath() + L"Dark Basic Professional TEMP\\ErrorReport.txt";
      }
 
      // Send error lines back to main window
      CStdioFile read;
      if(read.Open(Loc, CFile::modeRead))
      {
            int iUseOnlyThirdLine = 0;
            CString line;
            while(read.ReadString(line))
            {
				iUseOnlyThirdLine++;
				  /* My BCG does not support this
                  if(Settings::AllowComprehensiveReporting || iUseOnlyThirdLine == 3) {
                        int size = line.GetLength() + 1;
                        TCHAR* msg = new TCHAR[size];
                        wcscpy_s(msg, size, line);
                        pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msg);
                  }
				  */
				if ( iUseOnlyThirdLine == 3)
				{
					int size = line.GetLength() + 1;
					TCHAR* msg = new TCHAR[size];
					wcscpy_s(msg, size, line);
					pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msg);
				}
            }
            CString str(_T("Compilation Failed (Syntax Errors)"));
            int size = str.GetLength() + 1;
            TCHAR* msg = new TCHAR[size];
            wcscpy_s(msg, size, str);
            pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msg);
            read.Close();
 
            DeleteFileW(Loc); // Delete it
            return false;
      }
 
      // Check to see if the exe file exists
      CString runFile;
      if(CompileSupport::m_strHelpFile == "")
      {                
            if(Utilities::ExtractFilename(Variables::m_DBPropertyExe) == Variables::m_DBPropertyExe)
            {
                  runFile = Variables::m_ProjectPath + Variables::m_DBPropertyExe;
            }
            else
            {
                  runFile = Variables::m_DBPropertyExe;
            }
      }
      else
      {
            runFile = Utilities::GetTemporaryPath() + _T("Temp.exe");
      }
      CFileStatus exe;
      if(!CFile::GetStatus(runFile,exe))
      {
            CString str(_T("Compilation Failed (No exe created)"));
            int size = str.GetLength() + 1;
            TCHAR* msg = new TCHAR[size];
            wcscpy_s(msg, size, str);
            pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msg);
            return false;
      }
 
      CString str(_T("Compilation Successful"));
      int size = str.GetLength() + 1;
      TCHAR* msg = new TCHAR[size];
      wcscpy_s(msg, size, str);
      pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msg);
      return true;
}

/*
// pre 240309 - John changes to support Vista/7 visualisations

#include "StdAfx.h"
#include "ReadError.h"
#include "Settings.h"
#include "MainFrm.h"
#include "Variables.h"
#include "Utilities.h"
#include "CompileSupport.h"

void ReadError::CleanUp()
{
	CString Loc = Settings::DBPLocation + _T("TEMP\\ErrorReport.txt");
	
	CFileStatus file1;
	if(CFile::GetStatus(Loc,file1))
	{
		DeleteFileW(Loc);
	}
}

bool ReadError::Read()
{
	MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();

	// 230908 - this line is wrong for (a) Vista compatibility and (b) the file is not created
	// here but in the Documents And Settings\Lee\Local Settings\Temp\
	//CString Loc = Settings::DBPLocation + _T("TEMP\\ErrorReport.txt");
	CString Loc = Utilities::GetTemporaryPath() + _T("Dark Basic Professional TEMP\\ErrorReport.txt");

	// Send error lines back to main window
	CStdioFile read;
	if(read.Open(Loc, CFile::modeRead))
	{
		int iUseOnlyThirdLine = 0;
		CString line;
		while(read.ReadString(line))
		{
			iUseOnlyThirdLine++;
			if ( iUseOnlyThirdLine==3 )
			{
				line = _T("Compilation Failed. ") + line;
				int size = line.GetLength() + 1;
				TCHAR* msg = new TCHAR[size];
				wcscpy_s(msg, size, line);
				pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msg);
			}
		}
		read.Close();

		DeleteFileW(Loc); // Delete it
		return false;
	}

	// Check to see if the exe file exists
	CString runFile;
	if(CompileSupport::m_strHelpFile == "")
	{			
		if(Utilities::ExtractFilename(Variables::m_DBPropertyExe) == Variables::m_DBPropertyExe)
		{
			runFile = Variables::m_ProjectPath + Variables::m_DBPropertyExe;
		}
		else
		{
			runFile = Variables::m_DBPropertyExe;
		}
	}
	else
	{
		runFile = Utilities::GetTemporaryPath() + _T("Temp.exe");
	}
	CFileStatus exe;
	if(!CFile::GetStatus(runFile,exe))
	{
		CString str(_T("Compilation Failed (No exe created)"));
		int size = str.GetLength() + 1;
		TCHAR* msg = new TCHAR[size];
		wcscpy_s(msg, size, str);
		pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msg);
		return false;
	}

	CString str(_T("Compilation Successful"));
	int size = str.GetLength() + 1;
	TCHAR* msg = new TCHAR[size];
	wcscpy_s(msg, size, str);
	pMainWnd->SendMessage(WM_USER + 3, (WPARAM)msg);
	return true;
}
*/