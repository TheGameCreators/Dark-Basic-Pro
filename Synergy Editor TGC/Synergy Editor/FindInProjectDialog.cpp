#include "stdafx.h"
#include "Synergy Editor.h"
#include "FindInProjectDialog.h"
#include "SourceFile.h"
#include "Variables.h"
#include "Utilities.h"
#include "MainFrm.h"
#include <vector>

// FindInProjectDialog dialog

IMPLEMENT_DYNAMIC(FindInProjectDialog, CDialog)

FindInProjectDialog::FindInProjectDialog(CWnd* pParent /*=NULL*/)
	: CDialog(FindInProjectDialog::IDD, pParent)
{
	currentFile = _T("");
}

void FindInProjectDialog::PostNcDestroy()
{	
	delete this;
}

BOOL FindInProjectDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	cmbLookIn.AddString(_T("Project file list (ignore open files)"));
	cmbLookIn.AddString(_T("Project file list (prefer open file contents)"));
	cmbLookIn.AddString(_T("Project directory"));
	cmbLookIn.SetCurSel(0);	

	txtFind.SetFocus();

	rdOpenFile.SetCheck(BST_CHECKED);

	btnFind.EnableWindow(FALSE);

	if( !m_ToolTip.Create(this))
	{
	   TRACE0("Unable to create the ToolTip!");
	}
	else
	{
	  m_ToolTip.AddTool( &cmbLookIn, _T("Choose the location of the files to be searched"));
	  m_ToolTip.AddTool( &chkMatchCase, _T("Toggle whether the case of the search string should be matched"));	  
	  m_ToolTip.AddTool( &lstResults, _T("The list of files and line numbers containing the search string"));	
	  m_ToolTip.AddTool( &btnFind, _T("The search string to use"));	
	  m_ToolTip.AddTool( &rdOpenFile, _T("Toggle whether each match should be highlighted in the editor"));	
	  m_ToolTip.AddTool( &rdListFiles, _T("Toggle whether the search results should be listed"));	
	  m_ToolTip.AddTool( &txtFind, _T("Enter the search string here"));	

	  m_ToolTip.Activate(TRUE);
	}

	CenterWindow();

	return FALSE;
}

void FindInProjectDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOOK_IN, cmbLookIn);
	DDX_Control(pDX, IDC_MATCH_CASE, chkMatchCase);
	DDX_Control(pDX, IDC_RESULTS_LIST, lstResults);
	DDX_Control(pDX, IDC_FIND_BUTTON, btnFind);
	DDX_Control(pDX, IDC_HIGHLIGHT_TEXT, rdOpenFile);
	DDX_Control(pDX, IDC_LIST_FILES, rdListFiles);
	DDX_Control(pDX, IDC_FIND_WHAT, txtFind);
}

BEGIN_MESSAGE_MAP(FindInProjectDialog, CDialog)
	ON_BN_CLICKED(IDC_FIND_BUTTON, OnBnClickedFindButton)
	ON_BN_CLICKED(IDC_HIGHLIGHT_TEXT, OnBnClickedHighlightText)
	ON_BN_CLICKED(IDC_LIST_FILES, OnBnClickedHighlightText)
	ON_EN_CHANGE(IDC_FIND_WHAT, OnEnChangeFindWhat)
	ON_LBN_DBLCLK(IDC_RESULTS_LIST, &FindInProjectDialog::OnLbnDblclkResultsList)
END_MESSAGE_MAP()

// FindInProjectDialog message handlers

void FindInProjectDialog::OnBnClickedFindButton()
{
	int selected = cmbLookIn.GetCurSel();
	lstResults.ResetContent();
	if(selected == 0)
	{		
		if(rdOpenFile.GetCheck() == BST_CHECKED)
		{
			if(currentFile == _T(""))
			{
				currentFile = GetNextProjectFile(currentFile);
				firstFile = currentFile;
			}
			while(!GoToTextIn(currentFile, false))
			{
				currentFile = GetNextProjectFile(currentFile);
				if(currentFile == firstFile)
				{
					AfxMessageBox(_T("No more matches"), MB_ICONEXCLAMATION | MB_OK);
					break;
				}
			}
		}
		else
		{
			std::vector <SourceFile*>::iterator Iter;
			for ( Iter = Variables::m_IncludeFiles.begin( ) ; Iter != Variables::m_IncludeFiles.end( ) ; Iter++ )
			{
				FindTextIn((*Iter)->filename, false);
			}
		}
	}
	else if(selected == 1)
	{
		if(rdOpenFile.GetCheck() == BST_CHECKED)
		{
			if(currentFile == _T(""))
			{
				currentFile = GetNextProjectFile(currentFile);
				firstFile = currentFile;
			}
			while(!GoToTextIn(currentFile, true))
			{
				currentFile = GetNextProjectFile(currentFile);
				if(currentFile == firstFile)
				{
					AfxMessageBox(_T("No more matches"), MB_ICONEXCLAMATION | MB_OK);
					break;
				}
			}
		}
		else
		{
			std::vector <SourceFile*>::iterator Iter;
			for ( Iter = Variables::m_IncludeFiles.begin( ) ; Iter != Variables::m_IncludeFiles.end( ) ; Iter++ )
			{
				FindTextIn((*Iter)->filename, true);
			}
		}
	}
	else
	{
		if(rdOpenFile.GetCheck() == BST_CHECKED)
		{
			if(currentFile == _T(""))
			{
				currentFile = GetNextFolderFile(currentFile);
				firstFile = currentFile;
			}
			while(!GoToTextIn(currentFile, false))
			{
				currentFile = GetNextFolderFile(currentFile);
				if(currentFile == _T(""))
				{
					AfxMessageBox(_T("No more matches"), MB_ICONEXCLAMATION | MB_OK);
					break;
				}
			}
		}
		else
		{
			FindTextInFolder();
		}
	}
}

CString FindInProjectDialog::GetNextFolderFile(CString oldfilename)
{
	CString ret = _T("");

	currentLine = 0;

	TCHAR temp[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, temp);
	CString currentDir(temp);
	SetCurrentDirectory(Variables::m_ProjectPath);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(_T("*.dba"));

	bool next = false;
	while (bWorking)
	{		
		bWorking = finder.FindNextFileW();
		CString filename(finder.GetFileName());
		if(oldfilename == _T("") || next)
		{	
			ret = filename;
			break;
		}
		else if(filename == oldfilename)
		{
			next = true;
		}
	}

	SetCurrentDirectory(currentDir); // Reset current directory

	return ret;
}

void FindInProjectDialog::FindTextInFolder()
{
	TCHAR temp[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, temp);
	CString currentDir(temp);
	SetCurrentDirectory(Variables::m_ProjectPath);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(_T("*.dba"));

	while (bWorking)
	{		
		bWorking = finder.FindNextFileW();
		CString filename(finder.GetFileName());
		FindTextIn(filename, false);
	}

	SetCurrentDirectory(currentDir); // Reset current directory
}

CString FindInProjectDialog::GetNextProjectFile(CString oldfilename)
{
	CString ret = _T("");

	currentLine = 0;

	bool next = false;
	std::vector <SourceFile*>::iterator Iter;
	for ( Iter = Variables::m_IncludeFiles.begin( ) ; Iter != Variables::m_IncludeFiles.end( ) ; Iter++ )
	{
		if(oldfilename == _T("") || next)
		{	
			ret = (*Iter)->filename;
			break;
		}
		else if((*Iter)->filename == oldfilename)
		{
			next = true;
		}
	}

	return ret;
}

void FindInProjectDialog::FindTextIn(CString filename, bool tryView)
{
	CString toFind;
	txtFind.GetWindowText(toFind);

	if(chkMatchCase.GetCheck() == BST_UNCHECKED)
	{
		toFind.MakeUpper();
	}

	CString line;
	CString fileToRead = filename;

	if(tryView)
	{
		App* app = (App*)AfxGetApp();

		POSITION posDocument = app->pDocTemplate->GetFirstDocPosition();

		while(posDocument != NULL)
		{
			Doc* pDoc = (Doc*)app->pDocTemplate->GetNextDoc(posDocument);
			CString title = Utilities::RemoveModifiedFlag(pDoc->GetTitle());
			CString upTitle = title;
			upTitle.MakeUpper();
			if(upTitle.Right(3) != _T("DBA"))
			{
				title.Append(_T(".dba"));
			}
			if(title == Utilities::ExtractFilename(filename))
			{
				POSITION posView = pDoc->GetFirstViewPosition ();
				View* pView = DYNAMIC_DOWNCAST (View, pDoc->GetNextView (posView));
				if (pView != NULL)
				{
					CBCGPEditCtrl *cntrl = pView->GetEditCtrl();
					CStdioFile file;
					TCHAR tszBuf[MAX_PATH];
					int written = ::GetTempPathW(MAX_PATH, tszBuf);
					fileToRead = CString(tszBuf, written) + _T("Synergy.tmp");
					BOOL open = file.Open(fileToRead, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
					if(!open)
					{
						return;
					}
					file.WriteString(cntrl->GetText());
					file.Flush();
					file.Close();
				}
			}
		}
	}

	CStdioFile read;
	int lineNum = 0;
	if(!read.Open(fileToRead, CFile::modeRead))
	{
		return;
	}
	while(read.ReadString(line))
	{
		lineNum++;
		if(chkMatchCase.GetCheck() == BST_UNCHECKED)
		{
			line.MakeUpper();
		}
		if(line.Find(toFind) > -1)
		{
			lstResults.AddString(Utilities::GetRelativePath(filename) + _T(" at line ") + Utilities::GetNumber(lineNum));
		}		
	}
	read.Close();
}

bool FindInProjectDialog::GoToTextIn(CString filename, bool tryView)
{
	CString toFind;
	txtFind.GetWindowText(toFind);

	if(chkMatchCase.GetCheck() == BST_UNCHECKED)
	{
		toFind.MakeUpper();
	}

	CString line;
	CString fileToRead = filename;

	if(tryView)
	{
		App* app = (App*)AfxGetApp();

		POSITION posDocument = app->pDocTemplate->GetFirstDocPosition();

		while(posDocument != NULL)
		{
			Doc* pDoc = (Doc*)app->pDocTemplate->GetNextDoc(posDocument);
			CString title = Utilities::RemoveModifiedFlag(pDoc->GetTitle());
			CString upTitle = title;
			upTitle.MakeUpper();
			if(upTitle.Right(3) != _T("DBA"))
			{
				title.Append(_T(".dba"));
			}
			if(title == Utilities::ExtractFilename(filename))
			{
				POSITION posView = pDoc->GetFirstViewPosition ();
				View* pView = DYNAMIC_DOWNCAST (View, pDoc->GetNextView (posView));
				if (pView != NULL)
				{
					CBCGPEditCtrl *cntrl = pView->GetEditCtrl();
					CStdioFile file;
					TCHAR tszBuf[MAX_PATH];
					int written = ::GetTempPathW(MAX_PATH, tszBuf);
					fileToRead = CString(tszBuf, written) + _T("Synergy.tmp");
					BOOL open = file.Open(fileToRead, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
					if(!open)
					{
						return false;
					}
					file.WriteString(cntrl->GetText());
					file.Flush();
					file.Close();
				}
			}
		}
	}

	CStdioFile read;
	int lineNum = 0;
	if(!read.Open(fileToRead, CFile::modeRead))
	{
		return false;
	}
	bool found = false;
	while(read.ReadString(line))
	{
		lineNum++;
		if(lineNum <= currentLine)
		{
			continue;
		}
		if(chkMatchCase.GetCheck() == BST_UNCHECKED)
		{
			line.MakeUpper();
		}
		if(line.Find(toFind) > -1)
		{
			lstResults.AddString(Utilities::GetRelativePath(filename) + _T(" at line ") + Utilities::GetNumber(lineNum));
			currentLine = lineNum;
			found = true;
			break;
		}		
	}
	read.Close();

	if(found)
	{
		MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
		pMainWnd->GoToViewLine(filename, lineNum - 1);
	}

	return found;
}

void FindInProjectDialog::OnBnClickedHighlightText()
{
	if(rdOpenFile.GetCheck() == BST_CHECKED)
	{
		btnFind.SetWindowTextW(_T("Find Next"));
		currentFile = _T("");
	}
	else
	{
		btnFind.SetWindowTextW(_T("Find All"));
	}
}

void FindInProjectDialog::OnEnChangeFindWhat()
{
	btnFind.EnableWindow(txtFind.GetWindowTextLengthW() > 0);
}

void FindInProjectDialog::OnLbnDblclkResultsList()
{
	int item = lstResults.GetCurSel();
	int n = lstResults.GetTextLen( item );
	CString text;
	text.LockBuffer();
	lstResults.GetText(item, text.GetBuffer(n));
	text.ReleaseBuffer();

	int start = text.Find(_T(" at"));
	CString filename = text.Mid(0, start);
	int end = text.Find(_T("line "));
	CString line = text.Mid(end + 5);
	int lineNum = _ttoi(line);

	MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
	pMainWnd->GoToViewLine(filename, lineNum - 1);
}
