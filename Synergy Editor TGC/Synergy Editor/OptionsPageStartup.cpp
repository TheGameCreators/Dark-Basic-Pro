// OptionsPageStartup.cpp : implementation file

#include "stdafx.h"
#include "Synergy Editor.h"
#include "OptionsPageStartup.h"
#include "Settings.h"

// OptionsPageStartup dialog
IMPLEMENT_DYNAMIC(OptionsPageStartup, CBCGPPropertyPage)

OptionsPageStartup::OptionsPageStartup(CWnd* pParent /*=NULL*/)
	: CBCGPPropertyPage(OptionsPageStartup::IDD)
{}

OptionsPageStartup::~OptionsPageStartup()
{}

BOOL OptionsPageStartup::OnInitDialog()
{
	CBCGPPropertyPage::OnInitDialog();

	cFile.SetWindowTextW(Settings::LastProject);

	cAction.InsertString(-1, _T("Show empty environment"));
	cAction.InsertString(-1, _T("Create new project"));
	cAction.InsertString(-1, _T("Show open project dialog"));
	cAction.InsertString(-1, _T("Load last loaded project"));
	cAction.InsertString(-1, _T("Load specified project"));

	if(Settings::OnStartup == 0)
	{
		cAction.SelectString(-1, _T("Show empty environment"));
	}
	else if(Settings::OnStartup == 1)
	{
		cAction.SelectString(-1, _T("Create new project"));
	}
	else if(Settings::OnStartup == 2)
	{
		cAction.SelectString(-1, _T("Show open project dialog"));
	}
	else if(Settings::OnStartup == 3)
	{
		cAction.SelectString(-1, _T("Load last loaded project"));
	}
	else
	{
		cAction.SelectString(-1, _T("Load specified project"));
	}

	bool visible = (Settings::OnStartup == 4);
	cFile.EnableWindow(visible);
	cBrowse.EnableWindow(visible);

	if( !m_ToolTip.Create(this))
	{
	   TRACE0("Unable to create the ToolTip!");
	}
	else
	{
		m_ToolTip.AddTool( &cAction, _T("Choose how DarkBASIC Professional Editor behaves on startup"));
		m_ToolTip.AddTool( &cFile, _T("Specify the project to load on startup"));
		m_ToolTip.AddTool( &cBrowse, _T("Click to locate the project to load on startup"));
		m_ToolTip.Activate(TRUE);
	}

	CBCGPPropertyPage::SetModified(FALSE);

	return TRUE;
}

BOOL OptionsPageStartup::PreTranslateMessage(MSG* pMsg)
{
     m_ToolTip.RelayEvent(pMsg);
     return CDialog::PreTranslateMessage(pMsg);
}

void OptionsPageStartup::OnOK()
{
	cFile.GetWindowTextW(Settings::LastProject);

	CString startup;
	cAction.GetWindowText(startup);

	if(startup == _T("Show empty environment"))
	{
		Settings::OnStartup = 0;
	}
	else if(startup == _T("Create new project"))
	{
		Settings::OnStartup = 1;
	}
	else if(startup == _T("Show open project dialog"))
	{
		Settings::OnStartup = 2;
	}
	else if(startup == _T("Load last loaded project"))
	{
		Settings::OnStartup = 3;
	}
	else
	{
		Settings::OnStartup = 4;
	}

	CBCGPPropertyPage::OnOK();	
}

void OptionsPageStartup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STARTUP, cAction);
	DDX_Control(pDX, IDC_PROJECT_LOAD, cFile);
	DDX_Control(pDX, IDC_BROWSE_PROJECT, cBrowse);
}

BEGIN_MESSAGE_MAP(OptionsPageStartup, CDialog)
	ON_BN_CLICKED(IDC_BROWSE_PROJECT, OnBnClickedBrowseProject)
	ON_CBN_SELCHANGE(IDC_STARTUP, OnCbnSelchangeStartup)
	ON_EN_CHANGE(IDC_PROJECT_LOAD, &OptionsPageStartup::OnEnChangeProjectLoad)
END_MESSAGE_MAP()

// OptionsPageStartup message handlers

void OptionsPageStartup::OnBnClickedBrowseProject()
{
	CFileDialog FileDlg(TRUE, _T(".exe"), NULL, 0, _T("DarkBASIC Professional Project (*.dbpro)|*.dbpro||"));

	if( FileDlg.DoModal() == IDOK )
	{
		cFile.SetWindowText(FileDlg.GetPathName());		
	}
}

void OptionsPageStartup::OnCbnSelchangeStartup()
{
	CString startup;
	cAction.GetWindowText(startup);
	bool visible = (startup == _T("Load specified project"));
	cFile.EnableWindow(visible);
	cBrowse.EnableWindow(visible);

	CBCGPPropertyPage::SetModified(TRUE);
}

void OptionsPageStartup::OnEnChangeProjectLoad()
{
	CBCGPPropertyPage::SetModified(TRUE);
}
