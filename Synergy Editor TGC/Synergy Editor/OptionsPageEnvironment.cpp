#include "StdAfx.h"
#include "OptionsPageEnvironment.h"
#include "Settings.h"
#include "Utilities.h"
#include "Synergy Editor.h"

IMPLEMENT_DYNCREATE(OptionsPageEnvironment, CBCGPPropertyPage)

OptionsPageEnvironment::OptionsPageEnvironment() : CBCGPPropertyPage(OptionsPageEnvironment::IDD){}

BOOL OptionsPageEnvironment::PreTranslateMessage(MSG* pMsg)
{
     m_ToolTip.RelayEvent(pMsg);
     return CDialog::PreTranslateMessage(pMsg);
}

BOOL OptionsPageEnvironment::OnInitDialog()
{
	CBCGPPropertyPage::OnInitDialog();

	cCompiler.SetWindowTextW(Settings::DBPLocation + _T("Compiler\\DBPCompiler.exe"));	
	cTempPath.SetWindowTextW(Settings::TempLocation);

	cStatus.SetCheck(Settings::AllowReadCompilerStatus ? BST_CHECKED : BST_UNCHECKED);
	cAccelerate.SetCheck(Settings::AllowCompilerAccelerator && Utilities::GetProcessorCount() > 1 ? BST_CHECKED : BST_UNCHECKED);
	cAccelerate.EnableWindow(Utilities::GetProcessorCount() > 1);
	cShowOutput.SetCheck(Settings::ShowOutputWindow ? BST_CHECKED : BST_UNCHECKED);
	cClearOutput.SetCheck(Settings::ClearOutputOnCompile ? BST_CHECKED : BST_UNCHECKED);
	cOpenAllIncludes.SetCheck(Settings::OpenAllIncludes ? BST_CHECKED : BST_UNCHECKED);

	cAppearance.InsertString(-1, _T("Office 2000"));
	cAppearance.InsertString(-1, _T("Office XP"));
	cAppearance.InsertString(-1, _T("Office 2003"));
	cAppearance.InsertString(-1, _T("Visual Studio 2005"));
	cAppearance.InsertString(-1, _T("Windows XP"));
	cAppearance.InsertString(-1, _T("Office 2007 (Aqua)"));
	cAppearance.InsertString(-1, _T("Office 2007 (Luna)"));
	cAppearance.InsertString(-1, _T("Office 2007 (Obsidian)"));
	cAppearance.InsertString(-1, _T("Office 2007 (Silver)"));
	cAppearance.InsertString(-1, _T("Office 2007 (Custom)"));
	cAppearance.InsertString(-1, _T("Visual Studio 2008"));

	int m_nAppLook = theApp.GetInt (_T("ApplicationLook"), ID_VIEW_APPLOOK_VS2005);	
	if(m_nAppLook == ID_VIEW_APPLOOK_2000)
	{
		cAppearance.SelectString(-1, _T("Office 2000"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_XP)
	{
		cAppearance.SelectString(-1, _T("Office XP"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_2003)
	{
		cAppearance.SelectString(-1, _T("Office 2003"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_VS2005)
	{
		cAppearance.SelectString(-1, _T("Visual Studio 2005"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_WIN_XP)
	{
		cAppearance.SelectString(-1, _T("Windows XP"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_2007)
	{
		cAppearance.SelectString(-1, _T("Office 2007 (Aqua)"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_2007_1)
	{
		cAppearance.SelectString(-1, _T("Office 2007 (Luna)"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_2007_2)
	{
		cAppearance.SelectString(-1, _T("Office 2007 (Obsidian)"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_2007_3)
	{
		cAppearance.SelectString(-1, _T("Office 2007 (Silver)"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_2007_4)
	{
		cAppearance.SelectString(-1, _T("Office 2007 (Custom)"));
	}
	else if(m_nAppLook == ID_VIEW_APPLOOK_VS2008)
	{
		cAppearance.SelectString(-1, _T("Visual Studio 2008"));
	}

	cStyleHue.EnableWindow(m_nAppLook == ID_VIEW_APPLOOK_2007_4);
	cStyleHue.EnableOtherButton (_T("Other"));
	cStyleHue.SetColor ((COLORREF)theApp.GetInt (_T("LookColor"), (COLORREF)-1));
	cStyleHue.SetColumnsNumber (10);

	if(m_ToolTip.Create(this))
	{
		m_ToolTip.AddTool( &cCompiler, _T("Choose the location of the DarkBasic Professional compiler"));
		m_ToolTip.AddTool( &cStatus, _T("Report the progress of the compilation process"));
		m_ToolTip.AddTool( &cAccelerate, _T("Accelerate the compiler process"));
		m_ToolTip.AddTool( &cHelp, _T("Choose the default html file used by the help system"));
		m_ToolTip.AddTool( &cNewHelpTab, _T("Toggle whether each help file is loaded in a new tab"));	  
		m_ToolTip.AddTool( &cShowOutput, _T("Choose whether the output bar is opened on compile"));
		m_ToolTip.AddTool( &cAppearance, _T("Choose the appearance to use"));
		m_ToolTip.AddTool( &cStyleHue, _T("Choose the style hue to use with the selected skin"));
		m_ToolTip.AddTool( &cClearOutput, _T("Toggle whether the output panel should be cleared on each compile"));
		m_ToolTip.AddTool( &cBrowse, _T("Click to locate the DarkBASIC Professional compiler"));
		m_ToolTip.AddTool( &cBrowseTemp, _T("Click to locate a temporary path"));
		m_ToolTip.AddTool( &cTempPath, _T("Specify the location where temporary files should be stored"));
		m_ToolTip.AddTool( &cOpenAllIncludes, _T("Choose whether all include files in a project are also opened when a project is loaded"));
		m_ToolTip.Activate(TRUE);
	}

	OptionsPageEnvironment::SetModified(FALSE);

	return TRUE;
}

void OptionsPageEnvironment::DoDataExchange(CDataExchange* pDX)
{
	CBCGPPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, cCompiler);
	DDX_Control(pDX, IDC_CHECK2, cStatus);
	DDX_Control(pDX, IDC_CHECK3, cAccelerate);
	DDX_Control(pDX, IDC_BROWSE, cBrowse);
	DDX_Control(pDX, IDC_BROWSE_TEMP, cBrowseTemp);
	DDX_Control(pDX, IDC_TEMP_PATH, cTempPath);
	DDX_Control(pDX, IDC_SHOW_OUTPUT, cShowOutput);
	DDX_Control(pDX, IDC_APPEARANCE, cAppearance);
	DDX_Control(pDX, IDC_CLEAR_OUTPUT, cClearOutput);
	DDX_Control(pDX, IDC_OPEN_INCLUDES, cOpenAllIncludes);
	DDX_Control(pDX, IDC_STYLE_HUE, cStyleHue);
}

void OptionsPageEnvironment::OnBrowse()
{
	CFileDialog FileDlg(TRUE, _T(".exe"), NULL, 0, _T("DarkBASIC Professional Compiler (*.exe)|*.exe||"));

	if( FileDlg.DoModal() == IDOK )
	{
		cCompiler.SetWindowText(FileDlg.GetPathName());
	}
}

void OptionsPageEnvironment::OnBrowseTemp()
{
	CString path;
	if(!Utilities::ShowBrowseWindow(path, _T("Please select the folder to use for temporary files and press 'OK'. This current user must have read and write permissions."), Settings::DBPLocation))
	{
		return;
	}
	
	cTempPath.SetWindowTextW(path);
}

void OptionsPageEnvironment::OnChange()
{
	OptionsPageEnvironment::SetModified(TRUE);
}

void OptionsPageEnvironment::OnChangeStyle()
{
	CString appearance;
	cAppearance.GetWindowText(appearance);
	cStyleHue.EnableWindow(appearance == _T("Office 2007 (Custom)"));
	OnChange();
}

void OptionsPageEnvironment::OnOK()
{
	CBCGPPropertyPage::OnOK();

	CString Temp;	
	cCompiler.GetWindowTextW(Temp);
	Temp = Temp.Mid(0, Temp.GetLength() - 24);
	Settings::DBPLocation = Temp;
	cTempPath.GetWindowTextW(Settings::TempLocation);
	if(!Utilities::CheckPathExists(Settings::TempLocation))
	{
		Settings::TempLocation = L""; // Reset so that we get the windows temp path
		Settings::TempLocation = Utilities::GetTemporaryPath();
	}
	if(Settings::TempLocation.Right(1) != _T("\\"))
	{
		Settings::TempLocation += _T("\\");
	}
	
	Settings::AllowReadCompilerStatus = (cStatus.GetCheck() == BST_CHECKED);
	Settings::AllowCompilerAccelerator = (cAccelerate.GetCheck() == BST_CHECKED);
	Settings::ShowOutputWindow = (cShowOutput.GetCheck() == BST_CHECKED);
	Settings::ClearOutputOnCompile = (cClearOutput.GetCheck() == BST_CHECKED);
	Settings::OpenAllIncludes = (cOpenAllIncludes.GetCheck() == BST_CHECKED);
	
	CString appearance;
	cAppearance.GetWindowText(appearance);

	theApp.WriteInt(_T("LookColor"), cStyleHue.GetColor());

	MainFrame * pMainFrame = (MainFrame*)AfxGetMainWnd();
	if(appearance == _T("Office 2000"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_2000);
	}
	else if(appearance == _T("Office XP"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_XP);
	}
	else if(appearance == _T("Office 2003"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_2003);
	}
	else if(appearance == _T("Visual Studio 2005"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_VS2005);
	}
	else if(appearance == _T("Windows XP"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_WIN_XP);
	}
	else if(appearance == _T("Office 2007 (Aqua)"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_2007);
	}
	else if(appearance == _T("Office 2007 (Luna)"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_2007_1);
	}
	else if(appearance == _T("Office 2007 (Obsidian)"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_2007_2);
	}
	else if(appearance == _T("Office 2007 (Silver)"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_2007_3);
	}
	else if(appearance == _T("Office 2007 (Custom)"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_2007_4);
	}
	else if(appearance == _T("Visual Studio 2008"))
	{
		pMainFrame->OnAppLook(ID_VIEW_APPLOOK_VS2008);
	}	
}

// The default MFC implementation of OnApply() would call OnOK().
BOOL OptionsPageEnvironment::OnApply()
{
	return CBCGPPropertyPage::OnApply();
}

BEGIN_MESSAGE_MAP(OptionsPageEnvironment, CBCGPPropertyPage)
	ON_EN_CHANGE(IDC_EDIT1, OnChange)
	ON_CONTROL(BN_CLICKED, IDC_CHECK2, OnChange)
	ON_CONTROL(BN_CLICKED, IDC_CHECK3, OnChange)
	ON_CONTROL(BN_CLICKED, IDC_BROWSE, OnBrowse)	
	ON_CONTROL(BN_CLICKED, IDC_BROWSE_TEMP, OnBrowseTemp)
	ON_EN_CHANGE(IDC_TEMP_PATH, OnChange)
	ON_BN_CLICKED(IDC_CHECK2, OnChange)
	ON_BN_CLICKED(IDC_CHECK3, OnChange)
	ON_BN_CLICKED(IDC_SHOW_OUTPUT, OnChange)
	ON_EN_CHANGE(IDC_PROJECT_LOAD, OnChange)
	ON_BN_CLICKED(IDC_OPEN_NEW_TAB, OnChange)
	ON_BN_CLICKED(IDC_CLEAR_OUTPUT, OnChange)
	ON_CBN_SELCHANGE(IDC_APPEARANCE, OnChangeStyle)
	ON_BN_CLICKED(IDC_STYLE_HUE, OnChange)
	ON_BN_CLICKED(IDC_OPEN_INCLUDES, OnBnClickedOpenIncludes)
END_MESSAGE_MAP()

void OptionsPageEnvironment::OnBnClickedOpenIncludes()
{
	OnChange();
}
