#include "StdAfx.h"
#include "WriteProject.h"
#include "Variables.h"
#include "MainFrm.h" // For writing the output text
#include "Utilities.h"
#include "Settings.h"

bool WriteProject::Write(bool compile)
{
	CWinApp* pApp = AfxGetApp();
	MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
	int id = 0;

	if(Variables::m_ProjectName == _T(""))
	{
		Variables::m_ProjectName = _T("Test");
	}

	CString filename;
	if(compile)
	{
		filename = Utilities::GetTemporaryPath() + _T("Temp.dbpro");
	}
	else
	{
		filename = Variables::m_ProjectFile;
	}

	CString create = _T("Writing project file ") + filename;
	#ifndef NONVERBOSEOUTPUTMODE
	pMainWnd->AddOutputText(create);
	pMainWnd->DumpComments(); // Write the comments block to the vector so that we can use it
	#endif

	::DeleteFileW(filename);

	DWORD dwAttrs = GetFileAttributes(filename);
	if (dwAttrs != INVALID_FILE_ATTRIBUTES)
	{
         if (dwAttrs & FILE_ATTRIBUTE_READONLY) 
         { 
			if(AfxMessageBox(L"The file " + filename + L" is readonly. To save the file the readonly attribute must be changed.\n\nDo you wish to make this file writeable?", MB_ICONEXCLAMATION | MB_YESNO) == IDNO)
			{
				return false;
			}
			if(!SetFileAttributes(filename, dwAttrs & ~FILE_ATTRIBUTE_READONLY))
			{
				AfxMessageBox(L"Could not change the file attributes, check that you have permissions to access the project file", MB_ICONERROR | MB_OK);
				return false;
			}
		 }
	}

	CStdioFile file;
    BOOL open = file.Open(filename, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if(!open)
	{
		AfxMessageBox(_T("DarkBASIC Professional Editor could not open the project file ") + filename + _T(" for writing.\n\nIf you are using Windows Vista/7 then either:\n1). Choose a temporary path with write permissions in the options or ensure your project is in a path with write permissions\n2). Run DarkBASIC Professional Editor with administrative permissions"), MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	file.WriteString(_T("; **** Dark BASIC Professional Project File ****\n"));
	file.WriteString(_T("; **** Written by DarkBASIC Professional Editor ****\n"));
	file.WriteString(_T("version=DBP1.00\n"));
	file.WriteString(_T("project name=") + Variables::m_ProjectName + _T("\n"));
	file.WriteString(_T("\n"));
	file.WriteString(_T("; **** source file information ****\n"));
	
	// Write the files
	std::vector <SourceFile *>::iterator SFIter;
	for ( SFIter = Variables::m_IncludeFiles.begin( ) ; SFIter != Variables::m_IncludeFiles.end( ) ; SFIter++ )
	{
		CString fileWrite;
		CString lineWrite;
		if(id == 0)
		{		
			fileWrite = _T("main=");	
			lineWrite = _T("LineMain=");
		}
		else
		{
			fileWrite = _T("include") + Utilities::GetNumber(id) + _T("=");
			lineWrite = _T("LineInclude") + Utilities::GetNumber(id) +_T("=");
		}
		fileWrite += Utilities::GetRelativePath((*SFIter)->filename) + _T("\n");	
		
		file.WriteString(fileWrite);

		int lineNum = (*SFIter)->line;
		if(lineNum > -1)
		{
			lineWrite += Utilities::GetNumber(lineNum) + _T("\n");
			file.WriteString(lineWrite);
		}
		id++;
	}

	file.WriteString(_T("final source=_Temp.dbsource\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** Executable Information  ***\n"));
    file.WriteString(_T("; build types: exe, media, installer, alone\n"));
	if(compile && Utilities::ExtractFilename(Variables::m_DBPropertyExe) == Variables::m_DBPropertyExe)
	{
		file.WriteString(_T("executable=") + Variables::m_ProjectPath + Variables::m_DBPropertyExe + _T("\n"));
	}
	else
	{
		file.WriteString(_T("executable=") + Variables::m_DBPropertyExe + _T("\n"));
	}

    // Build Options
    file.WriteString(_T("build type=") + Variables::m_DBPropertyBuildType + _T("\n"));
	if(Variables::m_DBPropertyBuildType != _T("media") && Variables::m_DBPropertyMedia.size() > 0)
	{
		#ifndef NONVERBOSEOUTPUTMODE
		pMainWnd->AddOutputText(_T("Warning: Media files will not be included in the final exe; change the Application Type to 'media'"));
		#endif
	}

	// Media Options
    file.WriteString(_T("; ** Media file compression **\n")); 
    if (Variables::m_DBPropertyCompressMedia == TRUE)
    {
        file.WriteString(_T("compression=YES\n"));
    }
    else
    {
        file.WriteString(_T("compression=NO\n"));
    }
    file.WriteString(_T("\n"));

    file.WriteString(_T("; ** Media file encryption **\n"));
    // Media Options
    if (Variables::m_DBPropertyEncryptMedia == TRUE)
    {
        file.WriteString(_T("encryption=YES"));
    }
    else
    {
        file.WriteString(_T("encryption=NO"));
    }
    file.WriteString(_T("\n"));

    file.WriteString(_T("; ** Display the card options screen window? **\n"));
    // Display Card Window?
    file.WriteString(_T("card options window=NO\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** debugger information ****\n"));
    file.WriteString(_T("; If the editor sets this to yes, it is running in debug mode\n"));
    // Debugging?
	if (Variables::m_SavingBecauseDebug == true)
    {
        file.WriteString(_T("CLI=YES\n"));
    }
    else
    {
        file.WriteString(_T("CLI=NO\n"));
    }
	file.WriteString(L"CommandLineArguments=" + Variables::m_CommandLineArgs + L"\n");
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** display mode information ****\n"));

    file.WriteString(_T("app title=") + Variables::m_DBPropertyTitle +_T("\n"));

    file.WriteString(_T("\n"));
    file.WriteString(_T("; graphics mode options: fullscreen, window, desktop, fulldesktop, hidden\n"));
    file.WriteString(_T("graphics mode=") + Variables::m_DBPropertyScreenType + _T("\n"));
    file.WriteString(_T("fullscreen resolution=") + Variables::m_DBPropertyFullResolution + _T("\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; arbitrary sizes are valid for windowed mode\n"));
    file.WriteString(_T("window resolution=") + Variables::m_DBPropertyWindowResolution + _T("\n"));

    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** External Files Information ****\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** Media ****\n"));
    file.WriteString(_T("; Example entries: media1=graphics\\*.jpg\n"));
	file.WriteString(_T("media root path=") + Variables::m_ProjectPath + _T("\n"));

	std::vector <CString>::iterator Iter;
	id=1; // lee - 240309 - reset counter
	int projectPathLength = Variables::m_ProjectPath.GetLength();
	for ( Iter = Variables::m_DBPropertyMedia.begin( ) ; Iter != Variables::m_DBPropertyMedia.end( ) ; Iter++ )
	{
		if((*Iter).GetLength() < projectPathLength || _wcsnicmp(Variables::m_ProjectPath, (*Iter), projectPathLength) != 0)
		{
			#ifndef NONVERBOSEOUTPUTMODE
			pMainWnd->AddOutputText(_T("Warning: Media file ") + (*Iter) + _T(" will not be added to the final exe; it needs to be under the project path"));
			#endif
		}
		file.WriteString(_T("media") + Utilities::GetNumber(id++) + _T("=") + Utilities::GetRelativePath(*Iter) + _T("\n"));
	}			

    file.WriteString(_T("\n"));

	file.WriteString(_T("; **** Icons ****\n"));
	if(Variables::m_DBPropertyIcon != _T(""))
	{
		file.WriteString(_T("icon1=" + Variables::m_DBPropertyIcon + _T("\n")));
	}
	else
	{
		#ifndef NONVERBOSEOUTPUTMODE
		pMainWnd->AddOutputText(_T("Warning: No icon is specified for the final exe"));
		#endif
	}
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** Cursors ****\n"));
	id = 0;
	for ( Iter = Variables::m_DBPropertyCursors.begin( ) ; Iter != Variables::m_DBPropertyCursors.end( ) ; Iter++ )
	{
		if(id == 0)
		{
			file.WriteString(_T("cursorarrow=" + (*Iter) + _T("\n")));
		}
		else if(id == 1)
		{
			file.WriteString(_T("cursorwait=" + (*Iter) + _T("\n")));
		}
		else
		{
			file.WriteString(_T("pointer") + Utilities::GetNumber(id) + _T("=") + Utilities::GetRelativePath(*Iter) + _T("\n"));
		}
		id++;
	}
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** Version Info ****\n"));
    file.WriteString(_T("VerComments=") + Variables::m_DBPropertyFileComments + _T("\n"));
    file.WriteString(_T("VerCompany=") + Variables::m_DBPropertyFileCompany + _T("\n"));
    file.WriteString(_T("VerFileDesc=") + Variables::m_DBPropertyFileDescription + _T("\n"));
    file.WriteString(_T("VerFileNumber=") + Variables::m_DBPropertyFileVersion + _T("\n"));
    file.WriteString(_T("VerInternal=") + Variables::m_DBPropertyFileVersion + _T("\n"));
    file.WriteString(_T("VerCopyright=") + Variables::m_DBPropertyFileCopyright + _T("\n"));
    file.WriteString(_T("VerTrademark=\n"));
    file.WriteString(_T("VerFilename=\n"));
    file.WriteString(_T("VerProduct=\n"));
    file.WriteString(_T("VerProductNumber=v1.0\n"));

	file.WriteString(_T("\n"));
	file.WriteString(_T("; **** To Do ****\n"));
	id = 1;
	for ( Iter = Variables::m_ToDo.begin( ) ; Iter != Variables::m_ToDo.end( ) ; Iter++ )
	{
		file.WriteString(_T("ToDo") + Utilities::GetNumber(id++) + _T("=") + (*Iter) + _T("\n"));
	}	

	file.WriteString(_T("\n"));
	file.WriteString(_T("; **** Comments ****\n"));
	id = 1;
	for ( Iter = Variables::m_Comments.begin( ) ; Iter != Variables::m_Comments.end( ) ; Iter++ )
	{
		file.WriteString(_T("comments") + Utilities::GetNumber(id++) + _T("=") + (*Iter) + _T("\n"));
	}
	file.Flush();
    file.Close();

	return true;
}

bool WriteProject::WriteHelp()
{
	CWinApp* pApp = AfxGetApp();
	MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
	int id = 0;

	CString path = Utilities::GetTemporaryPath();

	CString filename = path + _T("Temp.dbpro");

	CString create = _T("Writing project file ") + filename;
	#ifndef NONVERBOSEOUTPUTMODE
	pMainWnd->AddOutputText(create);
	#endif

	::DeleteFileW(filename);

	CStdioFile file;
    BOOL open = file.Open(filename, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
	if(!open)
	{
		AfxMessageBox(_T("DarkBASIC Professional Editor could not open the project file ") + filename + _T(" for writing."), MB_ICONEXCLAMATION | MB_OK);
		return false;
	}
	file.WriteString(_T("; **** Dark BASIC Professional Project File ****\n"));
	file.WriteString(_T("; **** Written by DarkBASIC Professional Editor ****\n"));
	file.WriteString(_T("version=DBP1.00\n"));
	file.WriteString(_T("project name=Temp Help File\n"));
	file.WriteString(_T("\n"));
	file.WriteString(_T("; **** source file information ****\n"));
	
	// Write the files
	file.WriteString(_T("main=Temp.dba\n"));
	file.WriteString(_T("final source=_Temp.dbsource\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** Executable Information  ***\n"));
    file.WriteString(_T("; build types: exe, media, installer, alone\n"));
	file.WriteString(_T("executable=Temp.exe\n"));

    // Build Options
    file.WriteString(_T("build type=exe\n"));

	// Media Options
    file.WriteString(_T("; ** Media file compression **\n")); 
    file.WriteString(_T("compression=NO\n"));
    file.WriteString(_T("\n"));

	// Media Options
    file.WriteString(_T("; ** Media file encryption **\n"));    
    file.WriteString(_T("encryption=NO"));
	file.WriteString(_T("\n"));

	// Display Card Window?
    file.WriteString(_T("; ** Display the card options screen window? **\n"));    
    file.WriteString(_T("card options window=NO\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** debugger information ****\n"));
    file.WriteString(_T("; If the editor sets this to yes, it is running in debug mode\n"));
    // Debugging?
	file.WriteString(_T("CLI=NO\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** display mode information ****\n"));

    file.WriteString(_T("app title=Help Example\n"));

    file.WriteString(_T("\n"));
    file.WriteString(_T("; graphics mode options: fullscreen, window, desktop, fulldesktop, hidden\n"));
	file.WriteString(_T("graphics mode=") + Settings::m_DBPropertyScreenType + _T("\n"));
    file.WriteString(_T("fullscreen resolution=") + Settings::m_DBPropertyFullResolution + _T("\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; arbitrary sizes are valid for windowed mode\n"));
    file.WriteString(_T("window resolution=") + Settings::m_DBPropertyWindowResolution + _T("\n"));

    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** External Files Information ****\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** Media ****\n"));
    file.WriteString(_T("; Example entries: media1=graphics\\*.jpg\n"));
	file.WriteString(_T("media root path=\n"));
	file.WriteString(_T("\n"));

	file.WriteString(_T("; **** Icons ****\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** Cursors ****\n"));
    file.WriteString(_T("\n"));

    file.WriteString(_T("; **** Version Info ****\n"));
    file.WriteString(_T("VerComments=") + Settings::m_DBPropertyFileComments + _T("\n"));
    file.WriteString(_T("VerCompany=") + Settings::m_DBPropertyFileCompany + _T("\n"));
    file.WriteString(_T("VerFileDesc=") + Settings::m_DBPropertyFileDescription + _T("\n"));
    file.WriteString(_T("VerFileNumber=") + Settings::m_DBPropertyFileVersion + _T("\n"));
    file.WriteString(_T("VerInternal=") + Settings::m_DBPropertyFileVersion + _T("\n"));
    file.WriteString(_T("VerCopyright=") + Settings::m_DBPropertyFileCopyright + _T("\n"));
    file.WriteString(_T("VerTrademark=\n"));
    file.WriteString(_T("VerFilename=\n"));
    file.WriteString(_T("VerProduct=\n"));
    file.WriteString(_T("VerProductNumber=v1.0\n"));
	file.WriteString(_T("\n"));

	file.WriteString(_T("; **** To Do ****\n"));
	file.WriteString(_T("\n"));

	file.WriteString(_T("; **** Comments ****\n"));
	file.Flush();
    file.Close();

	return true;
}
