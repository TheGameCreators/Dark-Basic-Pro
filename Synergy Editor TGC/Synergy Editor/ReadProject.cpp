#include "StdAfx.h"
#include "ReadProject.h"
#include "Variables.h"
#include "Utilities.h"
#include "Settings.h"

// Get temp path
CString temp;

MainFrame* ReadProject::pMainWnd;

void ReadProject::Read(CString filename)
{
	// leefix - U70 - 300908 - it was assumed the filename had a path, but when you
	// load a project by double clicking a DBPRO in its own folder, there is no folder, so..
	if ( filename.Find(_T("\\"))!=-1 || filename.Find(_T("/"))!=-1 )
	{
		// path in filename
		Variables::m_ProjectName = Utilities::ExtractFilename(filename);
		Variables::m_ProjectPath = Utilities::ExtractPath(filename);
		Variables::m_ProjectFile = filename;
	}
	else
	{
		// no path in filename - use current working directory
		Variables::m_ProjectName = Utilities::ExtractFilename(filename);
		TCHAR temp[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, temp);
		CString currentDir(temp);
		Variables::m_ProjectPath = currentDir + _T("\\");
		Variables::m_ProjectFile = filename;
	}

	temp = _T("");

	CWinApp* pApp = AfxGetApp();
	pMainWnd = (MainFrame*)AfxGetMainWnd();

	pApp->CloseAllDocuments(FALSE); // Close all documents
	
	TCHAR tszBuf[MAX_PATH];
	int written = ::GetTempPathW(MAX_PATH, tszBuf);

	temp = CString(tszBuf, written) + _T("Synergy.ini");

	CStdioFile write;
	CStdioFile read;
	write.Open(temp, CFile::modeWrite | CFile::modeCreate);
	BOOL res = read.Open(filename, CFile::modeRead);
	if(res == FALSE)
	{
		AfxMessageBox(_T("The project file does not exist, cannot continue loading"), MB_ICONERROR | MB_OK);
		return;
	}
	{
		write.WriteString(_T("[Synergy]\n"));
		CString line;
		while(read.ReadString(line))
		{
			line.Append(_T("\n"));
			write.WriteString(line);
		}
	}
	read.Close();
	write.Close();

	handleStr(_T("Project name"), _T("My Project") , Variables::m_ProjectName);
	handleStr(_T("Executable"), _T("Project.exe") , Variables::m_DBPropertyExe);
	if(Variables::m_DBPropertyExe.Left(5) == L"%temp") // Fix temp macro
	{
		Variables::m_DBPropertyExe = Utilities::GetTemporaryPath() + Variables::m_DBPropertyExe.Mid(6);
	}
	handleStr(_T("Build type"), _T("exe") , Variables::m_DBPropertyBuildType);
	handleStr(_T("app title"), _T("My Project") , Variables::m_DBPropertyTitle);
	handleStr(_T("graphics mode"), _T("fulldesktop") , Variables::m_DBPropertyScreenType);
	handleStr(_T("fullscreen resolution"), _T("800x600x16") , Variables::m_DBPropertyFullResolution);
	handleStr(_T("window resolution"), _T("800x600") , Variables::m_DBPropertyWindowResolution);
	handleStr(_T("icon1"), _T("") , Variables::m_DBPropertyIcon);
	handleStr(L"CommandLineArguments", L"", Variables::m_CommandLineArgs);

	handleStr(_T("VerComments"), _T("") , Variables::m_DBPropertyFileComments);
	handleStr(_T("VerCompany"), _T("") , Variables::m_DBPropertyFileCompany);
	handleStr(_T("VerFileDesc"), _T("") , Variables::m_DBPropertyFileDescription);
	//handleStr(_T("VerFileNumber"), _T("") , Variables::);
	handleStr(_T("VerInternal"), _T("") , Variables::m_DBPropertyInternalVersion);
	handleStr(_T("VerCopyright"), _T("") , Variables::m_DBPropertyFileCopyright);
	//handleStr(_T("VerTrademark"), _T("") , Variables::);
	//handleStr(_T("VerFilename"), _T("") , Variables::);
	//handleStr(_T("VerProduct"), _T("") , Variables::);
	handleStr(_T("VerProductNumber"), _T("") , Variables::m_DBPropertyFileVersion);

	handleBool(_T("compression"), _T("NO") , Variables::m_DBPropertyCompressMedia);
	handleBool(_T("encryption"), _T("NO") , Variables::m_DBPropertyEncryptMedia);

	readFiles();
	readMedia();
	readToDo();
	readComments();
	readCursors();

	// Update grid
	pMainWnd->UpdateFromLibrary();

	// Delete ini file
	::DeleteFileW(temp);
}

void ReadProject::readFiles()
{
	pMainWnd->ClearInclude();
	Variables::ClearIncludes();

	TCHAR tszBuf[MAX_PATH];
	TCHAR tszLineBuf[MAX_PATH];
	GetPrivateProfileString(_T("Synergy"),_T("main"),_T("Main.dba"),tszBuf,MAX_PATH,temp);
	GetPrivateProfileString(_T("Synergy"),_T("LineMain"),_T("-1"),tszLineBuf,MAX_PATH,temp);

	CString field(tszBuf);
	CString mainLine(tszLineBuf);

    if(field.Mid(1,1) != _T(":")) // Full path
	{
		field = Variables::m_ProjectPath + field;
	}

	// Check that we can access it
	CFileStatus file2;
	if(!CFile::GetStatus(field,file2))
	{
		AfxMessageBox(_T("Could not find main include file, the path stored in the include file is invalid or missing. You will have to add the file manually."));
	} 
	else
	{
		Variables::m_OpenFileLine = _ttoi(mainLine);
		AfxGetApp()->OpenDocumentFile(field);
		if(Settings::GoToRememberedLine) Utilities::GoToLine(field);		
	}

	// Now read the includes
	int i = 1;
	while(true)
	{
		CString read;
		read.Format(_T("include%d"),i);

		CString lineread;
		lineread.Format(_T("LineInclude%d"),i);

		GetPrivateProfileString(_T("Synergy"),read,_T("ERROR"),tszBuf,MAX_PATH,temp);
		GetPrivateProfileString(_T("Synergy"),lineread,_T("-1"),tszLineBuf,MAX_PATH,temp);

		CString include(tszBuf);
		CString includeLine(tszLineBuf);

		if(include == _T("ERROR"))
			break;

		if(include.Mid(1,1) != _T(":")) // Full path?
		{
			include = Variables::m_ProjectPath + include;
		}

		pMainWnd->AddNewFile(include); // Add to the tree

		CFileStatus status;
		if(CFile::GetStatus(include,status)) // Only add to the list if it exists
		{
			Variables::m_IncludeFiles.push_back(new SourceFile(include, _ttoi(includeLine))); // Add first so that opening doesn't try and add it's own version
			if(Settings::OpenAllIncludes)
			{
				AfxGetApp()->OpenDocumentFile(include);
			}						
		}

		i++;
	}
}

// Reads the media from the project file
void ReadProject::readMedia()
{
	pMainWnd->ClearMedia(); // Clear the media

	CString field;
	int i = 1;
	while(true)
	{
		field.Empty();
		CString read;
		read.Format(_T("media%d"),i);

		handleStr(read, _T("") , field);

		if(field == _T(""))
			break;

		if(field.Mid(1,1) != _T(":")) // Full path
		{
			field = Variables::m_ProjectPath + field;
		}

		i++;
		pMainWnd->AddNewMediaFile(field);
	}
}

// Reads the cursors from the project file
void ReadProject::readCursors()
{
	pMainWnd->ClearCursors(); // Clear the cursors

	CString field;

	// Arrow
	handleStr(_T("cursorarrow"), _T("") , field);
	if(field == _T(""))
	{
		return;
	}
	if(field.Mid(1,1) != _T(":")) // Full path
	{
		field = Variables::m_ProjectPath + field;
	}
	pMainWnd->AddNewCursorFile(field);

	// Wait
	handleStr(_T("cursorwait"), _T("") , field);
	if(field == _T(""))
	{
		return;
	}
	if(field.Mid(1,1) != _T(":")) // Full path
	{
		field = Variables::m_ProjectPath + field;
	}
	pMainWnd->AddNewCursorFile(field);

	// Others
	int i = 2;
	while(true)
	{
		field.Empty();
		CString read;
		read.Format(_T("pointer%d"),i);

		handleStr(read, _T("") , field);

		if(field == _T(""))
			break;

		if(field.Mid(1,1) != _T(":")) // Full path
		{
			field = Variables::m_ProjectPath + field;
		}

		i++;
		pMainWnd->AddNewCursorFile(field);
	}
}

// Reads the to do list from the project file
void ReadProject::readToDo()
{
	pMainWnd->ClearToDo(); // Clear the to to list

	CString field;
	int i = 1;
	while(true)
	{
		field.Empty();
		CString read;
		read.Format(_T("ToDo%d"),i);

		handleStr(read, _T("") , field);

		if(field == _T(""))
			break;

		i++;
		pMainWnd->AddToDo(field.Mid(1), field.GetAt(0) == '1');
	}
}

// Reads the comments from the project file
void ReadProject::readComments()
{
	pMainWnd->ClearComments(); // Clear the comments

	CString field, buffer = _T("");
	int i = 1;
	while(true)
	{
		field.Empty();
		CString read;
		read.Format(_T("comments%d"),i);

		handleStr(read, _T("") , field);

		if(field == _T(""))
			break;

		field += _T("\r\n");
		buffer += field;

		i++;
	}	
	pMainWnd->AddComment(buffer);
}

// Reads a char array and converts it into a CString
void ReadProject::handleStr(CString name, CString def, CString& field)
{
	TCHAR tszBuf[MAX_PATH];
	int size = GetPrivateProfileString(_T("Synergy"),name,def,tszBuf,MAX_PATH,temp);

	if(wcslen(tszBuf) == 0)
	{
		field = def;
	}
	else
	{
		field = CString(tszBuf);
	}
}

// Reads a char array and converts it into a bool (YES/NO)
void ReadProject::handleBool(CString name, CString def, bool& field)
{
	TCHAR tszBuf[MAX_PATH];
	int size = GetPrivateProfileString(_T("Synergy"),name,def,tszBuf,MAX_PATH,temp);

	if(wcslen(tszBuf) == 0)
	{
		field = (def != _T("NO"));
	}
	else
	{
		field = (wcscmp(tszBuf, _T("NO")) != 0);
	}
	
}
