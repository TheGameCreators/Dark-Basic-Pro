#include "StdAfx.h"
#include "Utilities.h"
#include "Variables.h"
#include "Settings.h"

// For GoToLine
#include "View.h"
#include "Doc.h"
#include "Synergy Editor.h"

WORD Utilities::r;
WORD Utilities::c1;
WORD Utilities::c2;
DWORD Utilities::sum;

// Extracts the filename from the fully qualified path
CString Utilities::ExtractFilename(CString item)
{
	UINT stringLength = item.GetLength();
	for(UINT i=stringLength; i > 0; i--)
	{
		if(item.GetAt(i) == '\\')
		{
			return item.Right(item.GetLength() - (i + 1));
		}
	}
	return item;
}

// Extracts the path from the fully qualified path
CString Utilities::ExtractPath(CString item)
{
	UINT stringLength = item.GetLength();
	for(UINT i=stringLength; i > 0; i--)
	{
		if(item.GetAt(i) == '\\')
		{
			return item.Left(i + 1);
		}
	}
	return item;
}

// Returns the relative path of an item
CString Utilities::GetRelativePath(CString item)
{
	if(Utilities::ExtractPath(item) == Variables::m_ProjectPath)
	{
		return Utilities::ExtractFilename(item);
	}
	else
	{
		CString extracted(Utilities::ExtractPath(item));
		if(extracted.Mid(0, Variables::m_ProjectPath.GetLength()) == Variables::m_ProjectPath)
		{
			return item.Mid(Variables::m_ProjectPath.GetLength());
		}
		else
		{
			return item;
		}
	}
}								   

// Removes the modified flag if it exists from the title
CString Utilities::RemoveModifiedFlag(CString title)
{
	if(title.Right(1) == "*")
	{
		return title.Left(title.GetLength() - 2);
	}
	else
	{
		return title;
	}
}

// Removes the file extension
CString Utilities::RemoveExtension(CString item)
{
	UINT stringLength = item.GetLength();
	for(UINT i=stringLength; i > 0; i--)
	{
		if(item.GetAt(i) == '.')
		{
			return item.Left(i);
		}
	}
	return item;
}

// Returns whether the path exists
bool Utilities::CheckPathExists(CString szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// Returns whether the file exists
bool Utilities::CheckFileExists(CString szFile)
{
	CFileStatus status;
	return (CFile::GetStatus(szFile,status) == TRUE);
}

CTime Utilities::GetFileWriteTime(CString szFile)
{
	CFileStatus status;
	if(!CFile::GetStatus(szFile, status))
	{
		return NULL;
	}
	return status.m_mtime;
}

// Creates a path if it doesn't exist
bool Utilities::EnsurePathExists(CString path)
{
	if(Utilities::CheckPathExists(path))
	{
		return true;
	}
	if(!CreateDirectory(path, NULL))
	{		
		return false;
	}
	return true;
}

// Returns whether the string is a number
bool Utilities::isNumber(CString &item)
{
	UINT stringLength = item.GetLength();
	for(UINT i=0; i<stringLength; i++)
	{
		if(!isdigit(item.GetAt(i)))
		{
			return false;
		}
	}
	return true;
}

// Returns the integer as a string
CString Utilities::GetNumber(int num)
{
	char pszNum [10] = {0};
	_itoa_s(num,pszNum,10, 10);
	CString res(pszNum);
	return res;
}

CString Utilities::GetSpecialFolderLocation(int nFolder)
{
	CString Result;
	
	LPITEMIDLIST pidl;
	HRESULT hr = SHGetSpecialFolderLocation(NULL, nFolder, &pidl);
	
	if (SUCCEEDED(hr))
	{
		// Convert the item ID list's binary
		// representation into a file system path
		TCHAR szPath[_MAX_PATH];

		if (SHGetPathFromIDList(pidl, szPath))
		{
			Result = szPath;
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	else
	{
		ASSERT(FALSE);
	}
	
	return Result;
}

int CALLBACK Utilities::BrowseCallbackProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
  if (uMsg == BFFM_INITIALIZED)
  {
	SendMessage(hWnd, BFFM_SETSELECTION,TRUE, lpData);
  }
  return 0;
}

// Shows the browse for folder window
bool Utilities::ShowBrowseWindow(CString &folder, CString text, CString defaultPath)
{
	// Create a pointer to a MALLOC (memory allocation object) then get the Shell Malloc.
	IMalloc* pMalloc = 0;
	if(::SHGetMalloc(&pMalloc) != NOERROR)
	return false;

	// Now create BROWSEINFO structure, to tell the shell how to display the dialog.
	BROWSEINFO bi;
	memset(&bi, 0, sizeof(bi));

	bi.hwndOwner = NULL;;
	bi.lpszTitle = text;
	bi.ulFlags = BIF_USENEWUI;

	LPITEMIDLIST pidl;
	SHGetSpecialFolderLocation(NULL,CSIDL_DRIVES , &pidl);// My Computer
	bi.pidlRoot = pidl;

	// LPITEMIDLIST pItemIDList = new ITEMIDLIST;
	//// the path you want to convert
	//LPCWSTR pszName =  L"C:\\Windows";
	//SHParseDisplayName(pszName, 0, &pItemIDList, 0, 0);
	//// then set the pidlroot value of your BROWSEINFO var
	//bi.pidlRoot = pItemIDList; 

	if(defaultPath != L"")
	{
		TCHAR path[MAX_PATH];
		wcscpy_s(path, MAX_PATH, defaultPath);
		bi.lParam = (long)(LPCTSTR)path;
		bi.lpfn = BrowseCallbackProc;
	}

	// Now show the dialog and get the itemIDList for the selected folder.
	LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);
	if(pIDL == NULL)
	return false;

	// Now create a buffer to store the path, thne get it.
	TCHAR buffer[_MAX_PATH];
	if(::SHGetPathFromIDList(pIDL, buffer) == 0)
	return false;

	// Finally, set the string to the path, and return true.
	folder = CString(buffer);
	if(folder.Right(1) != _T("\\"))
	{
		folder += _T("\\");
	}

	return true;
}

// Highlights the line in the editor control
void Utilities::GoToLine(CString file)
{
	int pos = Variables::IncludeExists(file);
	if(pos == -1)
	{
		return;
	}
	int line = Variables::m_IncludeFiles.at(pos)->line;
	if(line == -1)
	{
		return;
	}

	App* pApp = (App*)AfxGetApp();
	ASSERT_VALID(pApp);
	CDocTemplate* pTemplate = pApp->pDocTemplate;
	ASSERT_VALID(pTemplate);
	POSITION posDocument = pTemplate->GetFirstDocPosition();

	while(posDocument != NULL)
	{
		CDocument* pDoc = pTemplate->GetNextDoc(posDocument);
		ASSERT_VALID(pDoc);
		if(pDoc->GetPathName() == file)
		{
			POSITION posView = pDoc->GetFirstViewPosition ();
			View* pView = DYNAMIC_DOWNCAST (View, pDoc->GetNextView (posView));
			ASSERT_VALID(pView);
			if (pView != NULL)
			{
				pView->GoToLine(line);
				pView->SetSourceFile(Variables::m_IncludeFiles.at(pos));
			}
			break;					
		}
	}
}

// Returns the location of the db installation from the directory
CString Utilities::GetDBPLocation()
{
	// Get DBP location
	CString DBPLoc = _T("");
	CBCGPRegistry* reg = new CBCGPRegistry(TRUE, KEY_ALL_ACCESS);
	reg->Open(_T("SOFTWARE\\Dark Basic\\Dark Basic Pro"));
	reg->Read(_T("INSTALL-PATH"), DBPLoc);
	reg->Close();
	delete reg;

	if(DBPLoc != _T(""))
	{
		DBPLoc = DBPLoc + _T("\\");
	}
	
	return DBPLoc;
}

// Returns the processor count
int Utilities::GetProcessorCount()
{
	SYSTEM_INFO si;
	memset( &si, 0, sizeof(si));
	GetSystemInfo( &si);

	return si.dwNumberOfProcessors;
}

int Utilities::GetWindowsMajor()
{
	OSVERSIONINFO osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	return osvi.dwMajorVersion;
}

// Returns the temporary path
CString Utilities::GetTemporaryPath()
{
	if(Settings::TempLocation != L"" && Utilities::CheckAccess(Settings::TempLocation, O_RDWR))
	{
		return Settings::TempLocation;
	}
	else
	{
		TCHAR lpPathBuffer[MAX_PATH];
		TCHAR lpLongPathBuffer[MAX_PATH];
		GetTempPath(MAX_PATH, lpPathBuffer);
		GetLongPathName(lpPathBuffer, lpLongPathBuffer, MAX_PATH);
		return CString(lpLongPathBuffer);
	}
}

// Returns the application path + name
CString Utilities::GetApplicationPath()
{
	//	Get the command line.
	CString commandLine = GetCommandLine();

	if(commandLine.GetLength() == 0)
		return CString("");

	//	The first parameter is the application path. If this begins with quotation marks, then we 
	//	get everything between that and the next set of quotation marks.
	if(commandLine[0] == '\"')
	{
		int nPos = commandLine.Find(_T("\""), 1);

		if(nPos != -1)
			return commandLine.Mid(1, nPos - 1);
		else
			return commandLine.Mid(1, commandLine.GetLength()-1);
	}
	else
	{
		//	The path is not in quotation marks, so it ends with a space.
		int nPos = commandLine.Find(_T(" "), 1);

		if(nPos != -1)
			return commandLine.Mid(0, nPos - 1);
		else
			return commandLine;
	}
}

// Checks to see whether a path has the permission requested
bool Utilities::CheckAccess(CString path, int accessType)
{
	bool res = false;

	if(Utilities::GetWindowsMajor() > 4)
	{
		// desired access flags can be set to whatever you define
		int flags = accessType;
		DWORD sdLen, dwAccessDesired = 0, dwPrivSetSize, dwAccessGranted;
		PSECURITY_DESCRIPTOR fileSD;
		GENERIC_MAPPING GenericMapping;
		PRIVILEGE_SET PrivilegeSet;
		BOOL fAccessGranted = FALSE;
		HANDLE hToken, hTokenOne;		

		// Get the users token
		if(OpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE, &hTokenOne))
		{
			DuplicateTokenEx(hTokenOne, TOKEN_IMPERSONATE | TOKEN_READ | TOKEN_DUPLICATE, NULL, SecurityImpersonation, TokenImpersonation, &hToken);
	 
			if(ImpersonateLoggedOnUser(hToken))
			{	 
				// get file security descriptor - use null value first to get size
				if (!GetFileSecurity(path, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, NULL, 0, &sdLen))
				{
					fileSD = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sdLen);
					GetFileSecurity(path, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, fileSD, sdLen, &sdLen);
				}

				// initialize a GenericMapping structure based on required access
				memset(&GenericMapping, 0x00, sizeof(GENERIC_MAPPING));
				switch (flags & (O_RDONLY | O_RDWR | O_WRONLY))
				{
					case O_RDONLY:
					   dwAccessDesired = FILE_READ_DATA;
					   GenericMapping.GenericRead = FILE_GENERIC_READ;
					   break;
					case O_RDWR:
					   dwAccessDesired = FILE_READ_DATA | FILE_WRITE_DATA;
					   GenericMapping.GenericAll = GENERIC_READ | GENERIC_WRITE;
					   break;
					case O_WRONLY:
					   dwAccessDesired = FILE_WRITE_DATA;
					   GenericMapping.GenericWrite = FILE_GENERIC_WRITE;
					   break;
				}
				 
				// Create a desired access mask
				MapGenericMask(&dwAccessDesired, &GenericMapping);

				// check the access 
				dwPrivSetSize = sizeof(PRIVILEGE_SET);
				AccessCheck(fileSD, hToken, dwAccessDesired, &GenericMapping, &PrivilegeSet, &dwPrivSetSize, &dwAccessGranted, &fAccessGranted);

				// Free heap
				HeapFree(GetProcessHeap(), 0, fileSD);

				if (fAccessGranted) 
				{
					res = true;
				}
				else 
				{
					CString error;
					error.Format(L"Access denied, error: %d, requested: %lx\n", GetLastError(), dwAccessDesired);
				}
			}
			else
			{
				CString error;
				error.Format(L"Cannot impersonate, error: %d, requested: %lx\n", GetLastError(), dwAccessDesired);
				AfxMessageBox(error);
			}

			// clean up resources
			CloseHandle(hToken);		
		}
	}
	else
	{
		// Windows 95, 98, ME Failover
		int test;
		if(accessType == O_RDONLY)
		{
			test = 4;
		}
		else if(accessType == O_RDWR)
		{
			test = 6;
		}
		else
		{
			test = 2;
		}
		res = (_waccess_s(path, test) == 0);
	}

	return res;
}

// Capitalises the first letter of each word
CString Utilities::MixedCase(CString item)
{
	CString loweritem = item.MakeLower();
	item.MakeUpper();
	CString newItem = _T("");
	bool nextUpper = true;
	for(int i=0; i<item.GetLength(); i++)
	{
		TCHAR ch = loweritem.GetAt(i);
		if(nextUpper == true)
		{
			newItem += item.GetAt(i);
			nextUpper = false;
		}
		else
		{
			newItem += ch;
		}
		if(ch == ' ')
		{
			nextUpper = true;
		}
	}	
	return newItem;
}

// Adds a byte to the checksum
void Utilities::add(BYTE value)
{
	BYTE cipher = (value ^ (r >> 8));
	r = (cipher + r) * c1 + c2;
	sum += cipher;
}

// Adds a string to the checksum
void Utilities::AddToChecksum(const CString & s)
{	
	for(int i = 0; i < s.GetLength(); i++)
	{
		add((BYTE)s.GetAt(i));
	}
}

// Initialises the checksum
void Utilities::InitChecksum()
{
	sum = 0; r = 55665; c1 = 52845; c2 = 22719;
}

// Variables used by methods below
CString returnCode;

// Used by format method below
void AddText(CString line, int tabCount)
{
	for(int b = 0; b < tabCount; b++)
	{
		returnCode += _T("\t");
	}
	returnCode += line + _T("\n");
}

// Formats the code nicely
CString Utilities::FormatCode(CString file)
{
	int tabCount = 0;	
	bool delayed = false;
	bool oneOff = false;
	bool inCommentBlock = false;
	
	CString code = _T("");

	CStdioFile read;
	read.Open(file, CFile::modeRead);
	{
		CString line;
		while(read.ReadString(line))
		{
			if(delayed)
			{
				delayed = false;
				tabCount++;
			}
			if(oneOff)
			{
				oneOff = false;
				tabCount++;
			}

			line = line.Trim();
			CString codeUpper = line;
			codeUpper.MakeUpper();

			if(codeUpper.Left(8) == _T("REMSTART"))
			{
				AddText(line, tabCount);
				inCommentBlock = true;
				continue;
			}
			else if(codeUpper.Left(6) == _T("REMEND"))
			{
				AddText(line, tabCount);
				inCommentBlock = false;
				continue;
			}

			if(inCommentBlock)
			{
				AddText(line, tabCount);
				continue;
			}

			if(codeUpper.Left(3) == _T("REM"))
			{
				AddText(line, tabCount);
				continue;
			}

			for(int i = 0; i <line.GetLength(); i++)
			{
				if(codeUpper.Mid(i, 5) == "ENDIF")
				{
					tabCount--;
					i=i+5;				
				}
				else if(codeUpper.Mid(i, 4) == "ELSE")
				{
					tabCount--;
					oneOff = true;
					i=i+4;		
				}
				else if(codeUpper.Mid(i, 4) == "THEN")
				{
					delayed = false;
					i=i+4;		
				}
				else if(codeUpper.Mid(i, 5) == "NEXT ")
				{
					tabCount--;
					i=i+5;				
				}
				else if(codeUpper.Mid(i, 11) == "ENDFUNCTION")
				{
					tabCount--;
					i=i+11;				
				}

				if(codeUpper.Mid(i, 3) == "IF ")
				{			
					i=i+3;
					delayed = true;
				} 
				else if(codeUpper.Mid(i, 4) == "FOR ")
				{
					i=i+4;
					delayed = true;
				}
				else if(codeUpper.Mid(i, 8) == "FUNCTION ")
				{
					i=i+8;
					delayed = true;
				}
			}

			AddText(line, tabCount);
		}
	}
	read.Close();

	return returnCode;
}
