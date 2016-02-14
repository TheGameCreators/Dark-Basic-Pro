// OptionsPageHelp.cpp : implementation file
#include "stdafx.h"
#include "Synergy Editor.h"
#include "OptionsPageHelp.h"
#include "Settings.h"

// OptionsPageHelp dialog
IMPLEMENT_DYNAMIC(OptionsPageHelp, CBCGPPropertyPage)

OptionsPageHelp::OptionsPageHelp(CWnd* pParent /*=NULL*/)
	: CBCGPPropertyPage(OptionsPageHelp::IDD)
{}

OptionsPageHelp::~OptionsPageHelp()
{}

BOOL OptionsPageHelp::PreTranslateMessage(MSG* pMsg)
{
     m_ToolTip.RelayEvent(pMsg);

     return CBCGPPropertyPage::PreTranslateMessage(pMsg);
}

BOOL OptionsPageHelp::OnInitDialog()
{
	CBCGPPropertyPage::OnInitDialog();

	cHelpPage.SetWindowTextW(Settings::MainHelpURL);
	cNewTab.SetCheck(Settings::OpenHelpInNewTab ? BST_CHECKED : BST_UNCHECKED);

	if( !m_ToolTip.Create(this))
	{
	   TRACE0("Unable to create the ToolTip!");
	}
	else
	{
	  m_ToolTip.AddTool( &cHelpPage, _T("Choose the default html file used by the help system"));
	  m_ToolTip.AddTool( &cNewTab, _T("Toggle whether each help file is loaded in a new tab"));	  

	  m_ToolTip.Activate(TRUE);
	}

	OptionsPageHelp::SetModified(FALSE);

	return TRUE;
}

void OptionsPageHelp::OnOK()
{
	cHelpPage.GetWindowTextW(Settings::MainHelpURL);
	Settings::OpenHelpInNewTab = (cNewTab.GetCheck() == BST_CHECKED);

	CBCGPPropertyPage::OnOK();
}

void OptionsPageHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HELP_PAGE, cHelpPage);
	DDX_Control(pDX, IDC_OPEN_NEW_TAB, cNewTab);
}

BEGIN_MESSAGE_MAP(OptionsPageHelp, CDialog)
	ON_EN_CHANGE(IDC_HELP_PAGE, OnEnChangeHelp)
	ON_BN_CLICKED(IDC_OPEN_NEW_TAB, OnBnClickedOpenNewTab)
END_MESSAGE_MAP()

// OptionsPageHelp message handlers
void OptionsPageHelp::OnEnChangeHelp()
{
	OptionsPageHelp::SetModified(TRUE);
}

void OptionsPageHelp::OnBnClickedOpenNewTab()
{
	OptionsPageHelp::SetModified(TRUE);
}
