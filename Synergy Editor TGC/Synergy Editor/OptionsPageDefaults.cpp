#include "StdAfx.h"
#include "OptionsPageDefaults.h"
#include "Settings.h"

IMPLEMENT_DYNCREATE(OptionsPageDefaults, CBCGPPropertyPage)

OptionsPageDefaults::OptionsPageDefaults() : CBCGPPropertyPage(OptionsPageDefaults::IDD){}

BOOL OptionsPageDefaults::PreTranslateMessage(MSG* pMsg)
{
     m_ToolTip.RelayEvent(pMsg);
     return CDialog::PreTranslateMessage(pMsg);
}

BOOL OptionsPageDefaults::OnInitDialog()
{
	CBCGPPropertyPage::OnInitDialog();

	updatingProperties = true;

	CRect rectPropList;
	m_wndPropListLocation.GetClientRect (&rectPropList);
	m_wndPropListLocation.MapWindowPoints (this, &rectPropList);

	if (!m_wndList.Create (WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rectPropList, this, (UINT)-1))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	m_wndList.EnableHeaderCtrl (FALSE);
	m_wndList.EnableDesciptionArea ();
	m_wndList.SetVSDotNetLook ();
	m_wndList.MarkModifiedProperties ();

	CBCGPProp* pGroup1 = new CBCGPProp (_T("Application"));
	m_Title = new CBCGPProp (_T("Title"), (_variant_t)Settings::m_DBPropertyTitle,
		_T("Enter the text that you would like to be shown in your applications title bar"));
	pGroup1->AddSubItem (m_Title);

	/*
	exe
	media
	installer
	alone
	*/
	pType = new CBCGPProp (_T("Application Type"), (_variant_t)Settings::m_DBPropertyBuildType,
		_T("The type of application produced by the compiler"));
	pType->AddOption (_T("exe"));
	pType->AddOption (_T("media"));
	pType->AddOption (_T("installer"));
	pType->AddOption (_T("alone"));
	pType->AllowEdit (FALSE);

	pGroup1->AddSubItem (pType);

	static TCHAR BASED_CODE szIconFilter[] = _T("Icon Files (*.ico)|*.ico|Bitmap Files (*.bmp)|*.bmp||");
	pIcon = new CBCGPFileProp (_T("Icon"), TRUE, Settings::m_DBPropertyIcon, _T("ico"), 0, szIconFilter, 
		_T("Specifies the icon used by your application"));
	pGroup1->AddSubItem (pIcon);

	static TCHAR BASED_CODE szApplicationFilter[] = _T("Exe Files (*.exe)|*.exe|Screensaver Files (*.scr)|*.scr||");
	pFilename = new CBCGPFileProp (_T("Filename"), TRUE, Settings::m_DBPropertyExe, _T("exe"), 0, szApplicationFilter, 
		_T("Specifies the filename created by the compiler"));
	pGroup1->AddSubItem (pFilename);

	m_wndList.AddProperty(pGroup1);
	CBCGPProp* pGroup2 = new CBCGPProp (_T("Screen"));

	/*
	fullscreen
	window
	desktop
	fulldesktop
	hidden
	*/
	pScreenType = new CBCGPProp (_T("Screen Type"), (_variant_t)Settings::m_DBPropertyScreenType,
		_T("The screen type your application uses on loading"));
	pScreenType->AddOption (_T("fullscreen"));
	pScreenType->AddOption (_T("window"));
	pScreenType->AddOption (_T("desktop"));
	pScreenType->AddOption (_T("fulldesktop"));
	pScreenType->AddOption (_T("hidden"));
	pScreenType->AllowEdit (FALSE);

	pGroup2->AddSubItem (pScreenType);

	/*
	640x480
	800x600
	1024x768
	1280x1024
	*/
	pWindowResolution = new CBCGPProp (_T("Window Resolution"), (_variant_t)Settings::m_DBPropertyWindowResolution,
		_T("The screen resolution your application uses on loading"));
	pWindowResolution->AddOption (_T("320x200"));
	pWindowResolution->AddOption (_T("320x240"));
	pWindowResolution->AddOption (_T("400x300"));
	pWindowResolution->AddOption (_T("480x360"));
	pWindowResolution->AddOption (_T("512x384"));
	pWindowResolution->AddOption (_T("640x400"));
	pWindowResolution->AddOption (_T("640x480"));
	pWindowResolution->AddOption (_T("800x600"));
	pWindowResolution->AddOption (_T("960x720"));
	pWindowResolution->AddOption (_T("1024x768"));
	pWindowResolution->AddOption (_T("1152x864"));
	pWindowResolution->AddOption (_T("1280x960"));
	pWindowResolution->AddOption (_T("1280x1024"));
	pWindowResolution->AddOption (_T("1600x900"));
	pWindowResolution->AddOption (_T("1600x1200"));
	pWindowResolution->AddOption (_T("1920x1080"));
	pWindowResolution->AddOption (_T("1920x1200"));
	pWindowResolution->AllowEdit (FALSE);

	pGroup2->AddSubItem (pWindowResolution);

	/*
	640x480x16
	800x600
	1024x768
	1280x1024
	*/
	pFullResolution = new CBCGPProp (_T("Fullscreen Resolution"), (_variant_t)Settings::m_DBPropertyFullResolution,
		_T("The screen resolution your application uses on loading"));
	pFullResolution->AddOption (_T("320x200x16"));
	pFullResolution->AddOption (_T("320x240x16"));
	pFullResolution->AddOption (_T("400x300x16"));
	pFullResolution->AddOption (_T("480x360x16"));
	pFullResolution->AddOption (_T("512x384x16"));
	pFullResolution->AddOption (_T("640x400x16"));
	pFullResolution->AddOption (_T("640x480x16"));
	pFullResolution->AddOption (_T("800x600x16"));
	pFullResolution->AddOption (_T("960x720x16"));
	pFullResolution->AddOption (_T("1024x768x16"));
	pFullResolution->AddOption (_T("1152x864x16"));
	pFullResolution->AddOption (_T("1280x960x16"));
	pFullResolution->AddOption (_T("1280x1024x16"));
	pFullResolution->AddOption (_T("1600x900x16"));
	pFullResolution->AddOption (_T("1600x1200x16"));
	pFullResolution->AddOption (_T("1920x1080x16"));
	pFullResolution->AddOption (_T("1920x1200x16"));
	pFullResolution->AddOption (_T("320x200x32"));
	pFullResolution->AddOption (_T("320x240x32"));
	pFullResolution->AddOption (_T("400x300x32"));
	pFullResolution->AddOption (_T("480x360x32"));
	pFullResolution->AddOption (_T("512x384x32"));
	pFullResolution->AddOption (_T("640x400x32"));
	pFullResolution->AddOption (_T("640x480x32"));
	pFullResolution->AddOption (_T("800x600x32"));
	pFullResolution->AddOption (_T("960x720x32"));
	pFullResolution->AddOption (_T("1024x768x32"));
	pFullResolution->AddOption (_T("1152x864x32"));
	pFullResolution->AddOption (_T("1280x960x32"));
	pFullResolution->AddOption (_T("1280x1024x32"));
	pFullResolution->AddOption (_T("1600x900x32"));
	pFullResolution->AddOption (_T("1600x1200x32"));
	pFullResolution->AddOption (_T("1920x1080x32"));
	pFullResolution->AddOption (_T("1920x1200x32"));
	pFullResolution->AllowEdit (FALSE);

	pFullResolution->Enable(FALSE);
	pGroup2->AddSubItem (pFullResolution);

	m_wndList.AddProperty(pGroup2);

	CBCGPProp* pGroup3 = new CBCGPProp (_T("Media"));

	/*
	Yes
	No
	*/
	pCompress = new CBCGPProp (_T("Compress"), (_variant_t)Settings::m_DBPropertyCompressMedia ? _T("Yes") : _T("No"),
		_T("Option to compress media that is part of the project"));
	pCompress->AddOption (_T("Yes"));
	pCompress->AddOption (_T("No"));
	pCompress->AllowEdit (FALSE);

	pGroup3->AddSubItem (pCompress);

	/*
	Yes
	No
	*/
	pEncrypt = new CBCGPProp (_T("Encyrpt"), (_variant_t)Settings::m_DBPropertyEncryptMedia ? _T("Yes") : _T("No"),
		_T("Option to encytpt media that is part of the project"));
	pEncrypt->AddOption (_T("Yes"));
	pEncrypt->AddOption (_T("No"));
	pEncrypt->AllowEdit (FALSE);

	pGroup3->AddSubItem (pEncrypt);

	m_wndList.AddProperty(pGroup3);

	CBCGPProp* pGroup4 = new CBCGPProp (_T("Version Information"));
	pComments = new CBCGPProp (_T("Comments"), (_variant_t)Settings::m_DBPropertyFileComments,
		_T("Enter any comments regarding the application"));
	pGroup4->AddSubItem (pComments);
	pCompany = new CBCGPProp (_T("Company"), (_variant_t)Settings::m_DBPropertyFileCompany,
		_T("Enter the company details regarding the application"));
	pGroup4->AddSubItem (pCompany);
	pCopyright = new CBCGPProp (_T("Copyright"), (_variant_t)Settings::m_DBPropertyFileCopyright,
		_T("Enter the applications copyright"));
	pGroup4->AddSubItem (pCopyright);
	pDescription = new CBCGPProp (_T("Description"), (_variant_t)Settings::m_DBPropertyFileDescription,
		_T("Enter the applications description"));
	pGroup4->AddSubItem (pDescription);
	pVersion = new CBCGPProp (_T("Version"), (_variant_t)Settings::m_DBPropertyFileVersion,
		_T("Enter the version number of your application in the format a.b.c.d"));
	pGroup4->AddSubItem (pVersion);
	m_wndList.AddProperty(pGroup4);

	updatingProperties = false;

	//Create the ToolTip control
	if( m_ToolTip.Create(this))
	{
		m_ToolTip.AddTool( &m_wndList, _T("Choose the default settings for new projects"));
		m_ToolTip.Activate(TRUE);
	}

	OptionsPageDefaults::SetModified(FALSE);

	return TRUE;
}

void OptionsPageDefaults::DoDataExchange(CDataExchange* pDX)
{
	CBCGPPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROPERTIES, m_wndPropListLocation);
}

void OptionsPageDefaults::OnChange()
{
	OptionsPageDefaults::SetModified(TRUE);
}

void OptionsPageDefaults::OnOK()
{
	CBCGPPropertyPage::OnOK();

	Settings::m_DBPropertyTitle = m_Title->GetValue();
	Settings::m_DBPropertyBuildType = pType->GetValue();

	Settings::m_DBPropertyIcon = pIcon->GetValue();
	Settings::m_DBPropertyExe = pFilename->GetValue();

	Settings::m_DBPropertyScreenType = pScreenType->GetValue();
	Settings::m_DBPropertyFullResolution = pFullResolution->GetValue();
	Settings::m_DBPropertyWindowResolution = pWindowResolution->GetValue();
	Settings::m_DBPropertyCompressMedia = ((CString)pCompress->GetValue() == _T("YES")) ? true : false;
	Settings::m_DBPropertyEncryptMedia = ((CString)pEncrypt->GetValue() == _T("YES")) ? true : false;

	Settings::m_DBPropertyFileComments = pComments->GetValue();
	Settings::m_DBPropertyFileCompany = pCompany->GetValue();
	Settings::m_DBPropertyFileCopyright = pCopyright->GetValue();
	Settings::m_DBPropertyFileDescription = pDescription->GetValue();
	Settings::m_DBPropertyFileVersion = pVersion->GetValue();
}

// The default MFC implementation of OnApply() would call OnOK().
BOOL OptionsPageDefaults::OnApply()
{
	return CBCGPPropertyPage::OnApply();
}

LRESULT OptionsPageDefaults::OnPropertyChanged (WPARAM,LPARAM lParam)
{
	OnChange();

	CBCGPProp* pProp = (CBCGPProp*) lParam;

	if(pProp == pScreenType)
	{
		CString tempValue(pProp->GetValue());
		if(tempValue == _T("fullscreen"))
		{
			pFullResolution->Enable(TRUE);
		}
		else
		{
			pFullResolution->Enable(FALSE);
		}			
		if(tempValue == _T("window"))
		{
			pWindowResolution->Enable(TRUE);
		}
		else
		{
			pWindowResolution->Enable(FALSE);
		}	
	}

	OptionsPageDefaults::SetModified(TRUE);

	return 0;
}

BEGIN_MESSAGE_MAP(OptionsPageDefaults, CBCGPPropertyPage)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED,OnPropertyChanged)   
END_MESSAGE_MAP()
