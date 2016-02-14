#include "StdAfx.h"
#include "OptionsPageSemantic.h"
#include "Settings.h"

IMPLEMENT_DYNCREATE(OptionsPageSemantic, CBCGPPropertyPage)

OptionsPageSemantic::OptionsPageSemantic() : CBCGPPropertyPage(OptionsPageSemantic::IDD) {}

BOOL OptionsPageSemantic::PreTranslateMessage(MSG* pMsg)
{
     m_ToolTip.RelayEvent(pMsg);
     return CDialog::PreTranslateMessage(pMsg);
}

BOOL OptionsPageSemantic::OnInitDialog()
{
	CBCGPPropertyPage::OnInitDialog();
	
	cListOrder.InsertString(-1, _T("Creation order"));
	cListOrder.InsertString(-1, _T("Alphabetical order"));
	if(Settings::CodeViewOrder == 0)
	{
		cListOrder.SelectString(-1, _T("Creation order"));
	}
	else
	{
		cListOrder.SelectString(-1, _T("Alphabetical order"));
	}

	cEnableCodeView.SetCheck(Settings::AllowCodeView ? BST_CHECKED : BST_UNCHECKED);
	cListOrder.EnableWindow(cEnableCodeView.GetCheck() == BST_CHECKED);
	cPromptError.SetCheck(Settings::AllowCheckLine ? BST_CHECKED : BST_UNCHECKED);
	cChangeCase.SetCheck(Settings::AllowVariableFix ? BST_CHECKED : BST_UNCHECKED);

	if(m_ToolTip.Create(this))
	{
		m_ToolTip.AddTool( &cEnableCodeView, _T("Toggle whether the code view should be kept updated"));
		m_ToolTip.AddTool( &cListOrder, _T("Specify the order in which items appear in the code view tree"));
		m_ToolTip.AddTool( &cPromptError, _T("Toggle whether a message box prompts the user when a syntax error is detected"));
		m_ToolTip.AddTool( &cChangeCase, _T("Toggle whether used variables are automatically changed to the same case as when defined"));
		m_ToolTip.Activate(TRUE);
	}

	OptionsPageSemantic::SetModified(FALSE);

	return TRUE;
}

void OptionsPageSemantic::DoDataExchange(CDataExchange* pDX)
{
	CBCGPPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENABLE_CODE_VIEW, cEnableCodeView);
	DDX_Control(pDX, IDC_LIST_ORDER, cListOrder);
	DDX_Control(pDX, IDC_PROMPT_ERROR, cPromptError);
	DDX_Control(pDX, IDC_CHANGE_CASE, cChangeCase);
}

void OptionsPageSemantic::OnOK()
{
	Settings::AllowCodeView = (cEnableCodeView.GetCheck() == BST_CHECKED);

	CString Sort;
	cListOrder.GetWindowText(Sort);
	if(Sort == _T("Creation order"))
	{
		Settings::CodeViewOrder = 0;
	}
	else
	{
		Settings::CodeViewOrder = 1;
	}

	Settings::AllowCheckLine = (cPromptError.GetCheck() == BST_CHECKED);
	Settings::AllowVariableFix = (cChangeCase.GetCheck() == BST_CHECKED);

	CBCGPPropertyPage::OnOK();	
}

// The default MFC implementation of OnApply() would call OnOK().
BOOL OptionsPageSemantic::OnApply()
{
	return CBCGPPropertyPage::OnApply();
}

BEGIN_MESSAGE_MAP(OptionsPageSemantic, CBCGPPropertyPage)
	ON_BN_CLICKED(IDC_ENABLE_CODE_VIEW, OnBnClickedEnableCodeView)
	ON_CBN_SELCHANGE(IDC_LIST_ORDER, OnCbnSelchangeListOrder)
	ON_BN_CLICKED(IDC_PROMPT_ERROR, OnBnClickedPromptError)
	ON_BN_CLICKED(IDC_CHANGE_CASE, OnChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionsPageSemantic message handlers

void OptionsPageSemantic::OnBnClickedEnableCodeView()
{
	OptionsPageSemantic::SetModified(TRUE);
	cListOrder.EnableWindow(cEnableCodeView.GetCheck() == BST_CHECKED);
}

void OptionsPageSemantic::OnCbnSelchangeListOrder()
{
	OptionsPageSemantic::SetModified(TRUE);
}

void OptionsPageSemantic::OnBnClickedPromptError()
{
	OptionsPageSemantic::SetModified(TRUE);
}

void OptionsPageSemantic::OnChange()
{
	OptionsPageSemantic::SetModified(TRUE);
}
