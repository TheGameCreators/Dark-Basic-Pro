// KeywordDialog.cpp : implementation file
#include "stdafx.h"
#include "Synergy Editor.h"
#include "KeywordDialog.h"

#include "Keywords.h"
#include "Utilities.h"

// KeywordDialog dialog
IMPLEMENT_DYNAMIC(KeywordDialog, CDialog)

KeywordDialog::KeywordDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KeywordDialog::IDD, pParent)
{}

KeywordDialog::~KeywordDialog()
{}

BOOL KeywordDialog::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;	

	return TRUE;
}

void KeywordDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CURRENT_PROCESS, cProcess);
	DDX_Control(pDX, IDC_CURRENT_FILE, cFile);
	DDX_Control(pDX, IDC_CURRENT_LINE, cLine);
}

BEGIN_MESSAGE_MAP(KeywordDialog, CDialog)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER + 1, SetProcessMessage)
	ON_MESSAGE(WM_USER + 2, SetFile)
	ON_MESSAGE(WM_USER + 3, SetLine)
	ON_MESSAGE(WM_USER + 4, CloseDialog)
END_MESSAGE_MAP()

// KeywordDialog message handlers
LRESULT KeywordDialog::CloseDialog(WPARAM a, LPARAM b)
{
	this->EndDialog(0);
	return TRUE;
}

LRESULT KeywordDialog::SetProcessMessage(WPARAM a, LPARAM b)
{
	TCHAR* message = (TCHAR*)a;
	cProcess.SetWindowText(message);
	return TRUE;
}

LRESULT KeywordDialog::SetFile(WPARAM a, LPARAM b)
{
	TCHAR* message = (TCHAR*)a;
	cFile.SetWindowText((TCHAR*)message);
	delete [] message;
	return TRUE;
}

LRESULT KeywordDialog::SetLine(WPARAM line, LPARAM b)
{
	CString msg = _T("Current Line: ") + Utilities::GetNumber((int)line);
	cLine.SetWindowText(msg);
	return TRUE;
}
