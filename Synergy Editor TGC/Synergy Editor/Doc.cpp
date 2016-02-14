#include "stdafx.h"
#include "Synergy Editor.h"
#include "Variables.h"
#include "MainFrm.h"
#include <vector>
#include "Utilities.h"
#include "View.h"
#include "Settings.h"
#include "SourceFile.h"

#include "Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Doc
IMPLEMENT_DYNCREATE(Doc, CDocument)

BEGIN_MESSAGE_MAP(Doc, CDocument)
END_MESSAGE_MAP()

// Doc construction/destruction
Doc::Doc()
{
	saved = false;
	m_IgnoreChangeCheck = false;
	m_OldName = "";
}

Doc::~Doc()
{
	if(!saved)
	{
		CString fileName = GetPathName();
		CString fileNameUpper(fileName);
		fileNameUpper.MakeUpper();
			
		if(fileNameUpper.Right(3) != _T("DBA"))
		{
			fileName.Append(_T(".dba"));
		}
		Variables::RemoveInclude(fileName);
		MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
		ASSERT_VALID(pMainWnd);
		pMainWnd->DeleteOldFile(fileName);
	}
}

BOOL Doc::OnNewDocument()
{
	if(CDocument::OnNewDocument() == FALSE)
		return FALSE;

	UpdateTitle(GetTitle());

	return TRUE;
}

BOOL Doc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if(CDocument::OnOpenDocument(lpszPathName) == FALSE)
		return FALSE;

	saved = true;

	UpdateTitle(lpszPathName);

	m_LastWrite = Utilities::GetFileWriteTime(lpszPathName);

	return TRUE;
}

BOOL Doc::OnSaveDocument(LPCTSTR lpszPathName)
{
	if(CDocument::OnSaveDocument(lpszPathName) == FALSE)
		return FALSE;

	saved = true;

	UpdateTitle(lpszPathName);

	m_LastWrite = Utilities::GetFileWriteTime(lpszPathName);
	m_IgnoreChangeCheck = false; // Turn off the ignorance checking

	return TRUE;
}

void Doc::SetModifiedFlag(BOOL bModified)
{
	CString strTitle = GetTitle();

	CString strIndicator = _T(" *");

	if(!IsModified() && bModified)
	{
		SetTitle(strTitle + strIndicator);
	}
	else if(IsModified() && !bModified)
	{
		if(strTitle.Right(3) == strIndicator)
		{
			SetTitle(strTitle.Left (strTitle.GetLength() - strIndicator.GetLength()));
		}
	}

	UpdateFrameCounts();

	CDocument::SetModifiedFlag(bModified);
}

void Doc::UpdateTitle(CString name)
{
	MainFrame* pMainWnd = (MainFrame*)AfxGetMainWnd();
	ASSERT_VALID(pMainWnd);
		
	CString path = name;

	// Fix the missing path
	if(Utilities::ExtractFilename(name) == name)
	{
		path = Variables::m_ProjectPath + name;
		SetPathName(path, TRUE);
	}

	// Fix this missing dba (include and solution view only)
	CString upperName = name;
	upperName.MakeUpper();
	if(upperName.Right(3) != _T("DBA"))
	{
		path.Append(_T(".dba"));
	}

	// If the user has renamed then we want to change the old path
	if(m_OldName != "")
	{
		if(m_OldName == path) return; // Don't bother updating if there is no change
		int pos = Variables::IncludeExists(m_OldName);
		if(pos > -1)
		{
			Variables::m_IncludeFiles.at(pos)->filename = path;
		}
		pMainWnd->DeleteOldFile(m_OldName);
		pMainWnd->AddNewFile(path);

		POSITION posView = GetFirstViewPosition ();
		View* pView = DYNAMIC_DOWNCAST (View, GetNextView (posView));
		if (pView != NULL)
		{
			pView->SetSourceFile(Variables::m_IncludeFiles.at(Variables::IncludeExists(path)));
		}
	}

	// If the user closes the document and reopens it we don't want to re-add it
	if(Variables::IncludeExists(path) == -1)
	{
		Variables::m_IncludeFiles.push_back(new SourceFile(path, Variables::m_OpenFileLine));
		Variables::m_OpenFileLine = -1;
		pMainWnd->AddNewFile(path);
	}

	m_OldName = path;
}

// Used by the backup system to get the text held in the control
CString Doc::GetText()
{
	POSITION pos = GetFirstViewPosition ();
	View* pView = DYNAMIC_DOWNCAST (View, GetNextView (pos));
	if (pView != NULL)
	{
		return pView->GetText();
	}
	else
	{
		return _T("");
	}
}

// Checks to see if the line is a breakpoint
bool Doc::IsABreakpoint(int line)
{
	POSITION pos = GetFirstViewPosition ();
	View* pView = DYNAMIC_DOWNCAST (View, GetNextView (pos));
	if (pView != NULL)
	{
		std::vector <int>::iterator Iter;
		for ( Iter = pView->m_Breakpoints.begin(); Iter != pView->m_Breakpoints.end(); Iter++ )
		{
			if(*Iter == line)
			{
				return true;
			}
			else if(*Iter > line) // Optimization: Return false directly if the line number in the list is greater
			{
				return false;
			}
		}
	}
	return false;
}

// Used when the DBP Compiler sends its message to highlight a line
void Doc::HighlightLine(int line)
{
	POSITION pos = GetFirstViewPosition ();
	View* pView = DYNAMIC_DOWNCAST (View, GetNextView (pos));
	if (pView != NULL)
	{
		pView->HighlightLine(line);
	}
}

// Used to ensure that the file opened is the latest
void Doc::CheckFileStatus()
{
	if(!saved || m_IgnoreChangeCheck) // If we haven't saved then don't check a non-existant files date, also don't continue if user says don't
	{
		return;
	}
	CTime time = Utilities::GetFileWriteTime(GetPathName());
	if(time == NULL) // Counldn't get the time from the file
	{
		return;
	}
	if(time != m_LastWrite)
	{
		if(AfxMessageBox(_T("The file contents have changed on disk, do you wish to reload the file?"), MB_ICONWARNING | MB_YESNO) == IDYES)
		{
			POSITION pos = GetFirstViewPosition ();
			View* pView = DYNAMIC_DOWNCAST (View, GetNextView (pos));
			pView->OpenFile(GetPathName());
			m_LastWrite = time;
		}
		else
		{
			m_IgnoreChangeCheck = true;
		}
	}
}

// Doc serialization
void Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		POSITION pos = GetFirstViewPosition ();
		View* pView = DYNAMIC_DOWNCAST (View, GetNextView (pos));
		if (pView != NULL)
		{
			pView->SaveFile(ar);
		}
		SetModifiedFlag(FALSE);
	}
	else
	{
		POSITION pos = GetFirstViewPosition ();
		View* pView = DYNAMIC_DOWNCAST (View, GetNextView (pos));
		if (pView != NULL)
		{
			pView->SerializeRaw(ar);
		}
	}
}

// Doc diagnostics
#ifdef _DEBUG
void Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
