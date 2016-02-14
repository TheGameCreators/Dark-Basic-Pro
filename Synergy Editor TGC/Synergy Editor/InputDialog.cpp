// InputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Synergy Editor.h"
#include "InputDialog.h"


// InputDialog dialog

IMPLEMENT_DYNAMIC(InputDialog, CDialog)

InputDialog::InputDialog(CString prompt, CString def, CWnd* pParent /*=NULL*/)
	: CDialog(InputDialog::IDD, pParent)
{
	retText = def;
	promptText = prompt;	
}

InputDialog::~InputDialog()
{}

CString InputDialog::GetText()
{
	return retText;
}

BOOL InputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	cPromptText.SetWindowTextW(promptText);

	if(retText != _T(""))
	{
		cText.SetWindowText(retText);
		cText.SetSel(0, retText.GetLength(), TRUE);
	}

	cText.SetFocus();

	return FALSE;
}

void InputDialog::OnOK()
{
	cText.GetWindowTextW(retText);

	CDialog::OnOK();
}

void InputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, cText);
	DDX_Control(pDX, IDC_PROMPT, cPromptText);
}


BEGIN_MESSAGE_MAP(InputDialog, CDialog)
END_MESSAGE_MAP()


// InputDialog message handlers
