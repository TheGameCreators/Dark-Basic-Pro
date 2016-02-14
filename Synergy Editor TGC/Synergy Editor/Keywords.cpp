#include "StdAfx.h"
#include "Keywords.h"
#include "Utilities.h"
#include "Settings.h"
#include <algorithm>

std::vector <Keyword*> Keywords::m_AKeywords;
std::vector <Keyword*> Keywords::m_BKeywords;
std::vector <Keyword*> Keywords::m_CKeywords;
std::vector <Keyword*> Keywords::m_DKeywords;
std::vector <Keyword*> Keywords::m_EKeywords;
std::vector <Keyword*> Keywords::m_FKeywords;
std::vector <Keyword*> Keywords::m_GKeywords;
std::vector <Keyword*> Keywords::m_HKeywords;
std::vector <Keyword*> Keywords::m_IKeywords;
std::vector <Keyword*> Keywords::m_JKeywords;
std::vector <Keyword*> Keywords::m_KKeywords;
std::vector <Keyword*> Keywords::m_LKeywords;
std::vector <Keyword*> Keywords::m_MKeywords;
std::vector <Keyword*> Keywords::m_NKeywords;
std::vector <Keyword*> Keywords::m_OKeywords;
std::vector <Keyword*> Keywords::m_PKeywords;
std::vector <Keyword*> Keywords::m_QKeywords;
std::vector <Keyword*> Keywords::m_RKeywords;
std::vector <Keyword*> Keywords::m_SKeywords;
std::vector <Keyword*> Keywords::m_TKeywords;
std::vector <Keyword*> Keywords::m_UKeywords;
std::vector <Keyword*> Keywords::m_VKeywords;
std::vector <Keyword*> Keywords::m_WKeywords;
std::vector <Keyword*> Keywords::m_XKeywords;
std::vector <Keyword*> Keywords::m_YKeywords;
std::vector <Keyword*> Keywords::m_ZKeywords;
std::vector <Keyword*> Keywords::m_MiscKeywords;

KeywordDialog Keywords::dlg;

int g_iFailKeywordStepIfFailToWriteVista7 = 0;

int Keywords::LongestKeyword;

int Keywords::a,Keywords::b,Keywords::c,Keywords::d,Keywords::e,Keywords::f,Keywords::g,Keywords::h,Keywords::i,Keywords::j,Keywords::k,Keywords::l,Keywords::m,Keywords::n,Keywords::o,Keywords::p,Keywords::q,Keywords::r,Keywords::s,Keywords::t,Keywords::u,Keywords::v,Keywords::w,Keywords::x,Keywords::y,Keywords::z;

int SortByVal(const Keyword* t1, const Keyword* t2 )
{
	return (t1->keyword > t2->keyword);
}

void Keywords::Kill(std::vector <Keyword*> &vec)
{	
	std::vector<Keyword*>::iterator myKVectorIterator;
	for(myKVectorIterator = vec.begin(); myKVectorIterator != vec.end(); myKVectorIterator++)
	{
		delete *myKVectorIterator;
	}
}

void Keywords::RebuildKeywords()
{	
	AfxBeginThread(Keywords::RebuildKeywordsThread, 0);
	dlg.DoModal();
}

UINT Keywords::RebuildKeywordsThread(LPVOID pParam)
{
	while(dlg.GetSafeHwnd() == NULL)
	{
		Sleep(1);
	}
	TCHAR flushMessage[] = _T("Current Process: Flushing existing cache");
	dlg.SendMessage(WM_USER + 1, (WPARAM)&flushMessage, 0);

	CString output = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Syntax\\Dark Basic Professional.cache");

	DeleteFile(output);

	TidyUp();

	LongestKeyword = -1;

	m_AKeywords.clear();
	m_BKeywords.clear();
	m_CKeywords.clear();
	m_DKeywords.clear();
	m_EKeywords.clear();
	m_FKeywords.clear();
	m_GKeywords.clear();
	m_HKeywords.clear();
	m_IKeywords.clear();
	m_JKeywords.clear();
	m_KKeywords.clear();
	m_LKeywords.clear();
	m_MKeywords.clear();
	m_NKeywords.clear();
	m_OKeywords.clear();
	m_PKeywords.clear();
	m_QKeywords.clear();
	m_RKeywords.clear();
	m_SKeywords.clear();
	m_TKeywords.clear();
	m_UKeywords.clear();
	m_VKeywords.clear();
	m_WKeywords.clear();
	m_XKeywords.clear();
	m_YKeywords.clear();
	m_ZKeywords.clear();
	m_MiscKeywords.clear();

	TCHAR createMessage[] = _T("Current Process: Creating keywords");
	dlg.SendMessage(WM_USER + 1, (WPARAM)&createMessage, 0);

	CreateKeywords();

	TCHAR readMessage[] = _T("Current Process: Reading keywords");
	
	dlg.SendMessage(WM_USER + 1, (WPARAM)&readMessage, 0);

	if ( g_iFailKeywordStepIfFailToWriteVista7==0 )
		ReadKeywords();
	
	dlg.SendMessage(WM_USER + 4, 0, 0);

	return 0;
}

void Keywords::CreateKeywords()
{
	// "C:\Program Files\The Game Creators\Dark Basic Professional Online\Compiler\DBPCompiler.exe"
	CString path = Settings::DBPLocation + _T("Editor\\Keywords\\");

	CString output = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Syntax\\");

	CreateDirectory(output, NULL);

	output += _T("Dark Basic Professional.cache");

	TCHAR temp[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, temp);
	CString currentDir(temp);
	SetCurrentDirectory(path);

	CFileFind finder;

	std::vector <Keyword*> Temp;

	Temp.push_back(new Keyword(_T("AS"), _T("AS"), _T("main.htm")));	
	Temp.push_back(new Keyword(_T("THEN"), _T("THEN"), _T("main.htm")));	
	Temp.push_back(new Keyword(_T("TO"), _T("TO"), _T("main.htm")));	

	int openError = 0;
	int lineCount = 0;

	BOOL bWorking = finder.FindFile(_T("*.ini"));
	while (bWorking)
	{
		bWorking = finder.FindNextFileW();
		CString filename(finder.GetFileName());
		filename.MakeUpper();
		if(filename.Right(3) != _T("INI"))
			continue;

		CStdioFile read;
		CString line(_T(""));

		try {
			BOOL open = read.Open(filename, CFile::modeRead);	
			if(open == FALSE)
			{
				openError++;
				continue;
			}
			{		
				CString fileMessageStr = _T("Current File: ") + filename;
				int size = fileMessageStr.GetLength() + 1;
				TCHAR* fileMessage = new TCHAR[size];
				wcscpy_s(fileMessage, size, fileMessageStr);
				dlg.SendMessage(WM_USER + 2, (WPARAM)fileMessage, 0);

				while(read.ReadString(line))
				{
					if(line == _T("") || line.GetAt(0) == '[')
						continue;
					
					lineCount++;
					dlg.SendMessage(WM_USER + 3, (WPARAM)lineCount, 0);					

					CString keyword, help, url;
					SplitLine(line, keyword, url, help);

					bool ignore = false;
					std::vector <Keyword*>::iterator Iter;
					for (Iter = Temp.begin(); Iter != Temp.end(); Iter++ )
					{
						Keyword* toCheck = *Iter;
						if(toCheck->keyword == keyword)
						{
							ignore = true;
						}
					}

					if(!ignore)
					{
						Temp.push_back(new Keyword(keyword, help, url));				
					}
				}
			}
		}
		catch(CFileException *e)
		{
			AfxMessageBox(_T("A keyword file could not be accessed, ensure that no other applications are using these files"), MB_OK | MB_ICONEXCLAMATION);
			e->Delete();
		}
		try
		{
			read.Close();
		} 
		catch(char * str) { delete str; }
	}

	if(openError > 0)
	{
		CString number;
		number.Format(_T("%d"), openError);
		AfxMessageBox(_T("There was a problem opening ") + number + _T(" keyword file(s), ensure that the keyword files are not open by another process"), MB_OK | MB_ICONEXCLAMATION);
	}

	std::sort(Temp.begin(), Temp.end(), SortByVal);

	CStdioFile wfile;
	BOOL openWrite = wfile.Open(output,CFile::modeWrite | CFile::modeCreate|CFile::modeNoTruncate);
	if(openWrite)
	{
		// Write checksum
		CString checkSum;
		checkSum.Format(_T("%d"),GetChecksumName()); 
		wfile.WriteString(checkSum + _T("\n"));
		// Write keywords
		std::vector <Keyword*>::iterator Iter;
		for (Iter = Temp.begin(); Iter != Temp.end(); Iter++ )
		{
			Keyword* toWrite = *Iter;		
			wfile.WriteString(toWrite->keyword + _T("\n") + toWrite->help +_T("\n") + toWrite->url +_T("\n"));
		}
		wfile.Flush();
		wfile.Close();
	}

	Kill(Temp);
	Temp.clear();

	SetCurrentDirectory(currentDir);

	if(!openWrite)
	{
		// U74 - 291009 - message and quit or editor stays in loop!
		AfxMessageBox(_T("There was a problem writing the keyword cache file, ensure that the directory that DarkBASIC Professional Editor resides has write permissions.\n\nIf you are using Windows Vista/7 then you should either:\n1). Run DarkBASIC Professional Editor with administrative settings\n2). Install DarkBASIC Professional Editor into a directory that is not locked down by Vista/7 security"), MB_OK | MB_ICONEXCLAMATION);
		g_iFailKeywordStepIfFailToWriteVista7 = 1;
	}
}

void Keywords::ReadKeywords()
{
	CString path = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Syntax\\Dark Basic Professional.cache");

	CFileStatus status;
	if(!CFile::GetStatus(path,status))
	{
		if(!Utilities::CheckPathExists(Settings::DBPLocation + _T("Editor\\Keywords\\")))
		{
			return;
		}
		AfxMessageBox(_T("The required syntax highlighting file is missing. It will now be built."), MB_OK);
		RebuildKeywords();
		return;
	}

	CStdioFile read;

	CString checkSum; // Checksum
	CString keyword;
	CString help;
	CString url;

	// Reserve standard DBP installation amount
	Keywords::m_AKeywords.reserve(45);
	Keywords::m_BKeywords.reserve(30);
	Keywords::m_CKeywords.reserve(78);
	Keywords::m_DKeywords.reserve(54);
	Keywords::m_EKeywords.reserve(23);
	Keywords::m_FKeywords.reserve(51);
	Keywords::m_GKeywords.reserve(63);
	Keywords::m_HKeywords.reserve(16);
	Keywords::m_IKeywords.reserve(15);
	Keywords::m_JKeywords.reserve(21);
	Keywords::m_KKeywords.reserve(1);
	Keywords::m_LKeywords.reserve(71);
	Keywords::m_MKeywords.reserve(90);
	Keywords::m_NKeywords.reserve(28);
	Keywords::m_OKeywords.reserve(31);
	Keywords::m_PKeywords.reserve(58);
	Keywords::m_QKeywords.reserve(1);
	Keywords::m_RKeywords.reserve(53);
	Keywords::m_SKeywords.reserve(274);
	Keywords::m_TKeywords.reserve(36);
	Keywords::m_UKeywords.reserve(8);
	Keywords::m_VKeywords.reserve(4);
	Keywords::m_WKeywords.reserve(27);
	Keywords::m_XKeywords.reserve(5);
	Keywords::m_YKeywords.reserve(5);
	Keywords::m_ZKeywords.reserve(6);
	Keywords::m_MiscKeywords.reserve(2);

	// Reset mid values
	Keywords::a = -1;
	Keywords::b = -1;
	Keywords::c = -1;
	Keywords::d = -1;
	Keywords::e = -1;
	Keywords::f = -1;
	Keywords::g = -1;
	Keywords::h = -1;
	Keywords::i = -1;
	Keywords::j = -1;
	Keywords::k = -1;
	Keywords::l = -1;
	Keywords::m = -1;
	Keywords::n = -1;
	Keywords::o = -1;
	Keywords::p = -1;
	Keywords::q = -1;
	Keywords::r = -1;
	Keywords::s = -1;
	Keywords::t = -1;
	Keywords::u = -1;
	Keywords::v = -1;
	Keywords::w = -1;
	Keywords::x = -1;
	Keywords::y = -1;
	Keywords::z = -1;

	BOOL res = read.Open(path, CFile::modeRead);	
	if(!res)
	{
		AfxMessageBox(_T("The required syntax highlighting could not be 'read'. Check the other processes are not accessing this file. Syntax highlighting will not be available"), MB_OK);
		return;
	}
	{				
		read.ReadString(checkSum);
		if(GetChecksumName() != _ttoi(checkSum))
		{
			read.Close();
			RebuildKeywords();
			return;
		}

		while(read.ReadString(keyword))
		{
			read.ReadString(help);
			read.ReadString(url);
			keyword.MakeUpper();
			if(LongestKeyword < keyword.GetLength())
			{
				LongestKeyword = keyword.GetLength();
			}
			TCHAR ch = keyword.GetAt(0);
			switch(ch)
			{
			case _T('A'):
				if(Keywords::a == -1 && keyword.GetAt(1) <= 'N')
				{
					Keywords::a = (int)m_AKeywords.size();
				}
				m_AKeywords.push_back(new Keyword(keyword, help, url));				
				break;
			case _T('B'):
				if(Keywords::b == -1 && keyword.GetAt(1) <= 'I')
				{
					Keywords::b = (int)m_BKeywords.size();
				}
				m_BKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('C'):
				if(Keywords::c == -1 && keyword.GetAt(1) <= 'L')
				{
					Keywords::c = (int)m_CKeywords.size();
				}
				m_CKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('D'):
				if(Keywords::d == -1 && keyword.GetAt(1) <= 'E')
				{
					Keywords::d = (int)m_DKeywords.size();
				}
				m_DKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('E'):
				if(Keywords::e == -1 && keyword.GetAt(1) <= 'N')
				{
					Keywords::e = (int)m_EKeywords.size();
				}
				m_EKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('F'):
				if(Keywords::f == -1 && keyword.GetAt(1) <= 'I')
				{
					Keywords::f = (int)m_FKeywords.size();
				}
				m_FKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('G'):
				if(Keywords::g == -1 && keyword.GetAt(1) <= 'E')
				{
					Keywords::g = (int)m_GKeywords.size();
				}
				m_GKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('H'):
				if(Keywords::h == -1 && keyword.GetAt(1) <= 'I')
				{
					Keywords::h = (int)m_HKeywords.size();
				}
				m_HKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('I'):
				if(Keywords::i == -1 && keyword.GetAt(1) <= 'N')
				{
					Keywords::i = (int)m_IKeywords.size();
				}
				m_IKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('J'):
				if(Keywords::j == -1 && keyword.GetAt(1) <= 'O')
				{
					Keywords::j = (int)m_JKeywords.size();
				}
				m_JKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('K'):
				if(Keywords::k == -1 && keyword.GetAt(1) <= 'E')
				{
					Keywords::k = (int)m_KKeywords.size();
				}
				m_KKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('L'):
				if(Keywords::l == -1 && keyword.GetAt(1) <= 'M')
				{
					Keywords::l = (int)m_LKeywords.size();
				}
				m_LKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('M'):
				if(Keywords::m == -1 && keyword.GetAt(1) <= 'A')
				{
					Keywords::m = (int)m_MKeywords.size();
				}
				m_MKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('N'):
				if(Keywords::n == -1 && keyword.GetAt(1) <= 'E')
				{
					Keywords::n = (int)m_NKeywords.size();
				}
				m_NKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('O'):
				if(Keywords::o == -1 && keyword.GetAt(1) <= 'B')
				{
					Keywords::o = (int)m_OKeywords.size();
				}
				m_OKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('P'):
				if(Keywords::p == -1 && keyword.GetAt(1) <= 'I')
				{
					Keywords::p = (int)m_PKeywords.size();
				}
				m_PKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('Q'):
				if(Keywords::q == -1 && keyword.GetAt(1) <= 'E')
				{
					Keywords::q = (int)m_QKeywords.size();
				}
				m_QKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('R'):
				if(Keywords::r == -1 && keyword.GetAt(1) <= 'E')
				{
					Keywords::r = (int)m_RKeywords.size();
				}
				m_RKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('S'):
				if(Keywords::s == -1 && keyword.GetAt(1) <= 'E')
				{
					Keywords::s = (int)m_SKeywords.size();
				}
				m_SKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('T'):
				if(Keywords::t == -1 && keyword.GetAt(1) <= 'I')
				{
					Keywords::t = (int)m_TKeywords.size();
				}
				m_TKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('U'):
				if(Keywords::u == -1 && keyword.GetAt(1) <= 'N')
				{
					Keywords::u = (int)m_UKeywords.size();
				}
				m_UKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('V'):
				if(Keywords::v == -1 && keyword.GetAt(1) <= 'E')
				{
					Keywords::v = (int)m_VKeywords.size();
				}
				m_VKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('W'):
				if(Keywords::w == -1 && keyword.GetAt(1) <= 'R')
				{
					Keywords::w = (int)m_WKeywords.size();
				}
				m_WKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('X'):
				if(Keywords::x == -1 && keyword.GetAt(1) <= ' ')
				{
					Keywords::x = (int)m_XKeywords.size();
				}
				m_XKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('Y'):
				if(Keywords::y == -1 && keyword.GetAt(1) <= ' ')
				{
					Keywords::y = (int)m_YKeywords.size();
				}
				m_YKeywords.push_back(new Keyword(keyword, help, url));
				break;
			case _T('Z'):
				if(Keywords::z == -1 && keyword.GetAt(1) <= 'O')
				{
					Keywords::z = (int)m_ZKeywords.size();
				}
				m_ZKeywords.push_back(new Keyword(keyword, help, url));
				break;
			default:
				m_MiscKeywords.push_back(new Keyword(keyword, help, url));
				break;
			}
		}
	}
	read.Close();
}

// Scans the directory for all of the ini files and then returns the value
int Keywords::GetChecksumName()
{
	Utilities::InitChecksum();
	CString path = Settings::DBPLocation + _T("Editor\\Keywords\\");

	TCHAR temp[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, temp);
	CString currentDir(temp);
	SetCurrentDirectory(path);

	CFileFind finder;

	BOOL bWorking = finder.FindFile(_T("*.ini"));
	while (bWorking)
	{
		bWorking = finder.FindNextFileW();
		CString filename(finder.GetFileName());
		filename.MakeUpper();
		Utilities::AddToChecksum(filename);
	}
	
	SetCurrentDirectory(currentDir);

	return Utilities::getChecksum();
}

// Provides the keyword, url and help file from the line
void Keywords::SplitLine(CString line, CString &keyword, CString &url, CString &help)
{
	int curPos = 0;
	int length = line.GetLength();
	int mode = 0;
	while(curPos < length)
	{
		TCHAR chr = line.GetAt(curPos);
		if(chr == '=')
		{
			mode++;
		}
		else
		{
			if(mode == 0)
			{
				keyword += chr;
			}
			else if(mode == 1)
			{
				url += chr;
			}
			else
			{
				help += chr;
			}
		}
		curPos++;
	}

	keyword.MakeUpper();
	if(url == _T(""))
	{
		url = "main.htm";
	}
}

void Keywords::TidyUp()
{
	Kill(m_AKeywords);
	Kill(m_BKeywords);
	Kill(m_CKeywords);
	Kill(m_DKeywords);
	Kill(m_EKeywords);
	Kill(m_FKeywords);
	Kill(m_GKeywords);
	Kill(m_HKeywords);
	Kill(m_IKeywords);
	Kill(m_JKeywords);
	Kill(m_KKeywords);
	Kill(m_LKeywords);
	Kill(m_MKeywords);
	Kill(m_NKeywords);
	Kill(m_OKeywords);
	Kill(m_PKeywords);
	Kill(m_QKeywords);
	Kill(m_RKeywords);
	Kill(m_SKeywords);
	Kill(m_TKeywords);
	Kill(m_UKeywords);
	Kill(m_VKeywords);
	Kill(m_WKeywords);
	Kill(m_XKeywords);
	Kill(m_YKeywords);
	Kill(m_ZKeywords);
	Kill(m_MiscKeywords);
}
