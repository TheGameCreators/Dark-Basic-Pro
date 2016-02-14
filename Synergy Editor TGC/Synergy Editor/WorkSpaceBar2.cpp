// workspace2.cpp : implementation of the CWorkSpaceBar2 class

#include "stdafx.h"
#include "Synergy Editor.h"
#include "WorkSpaceBar2.h"
#include "Variables.h"

const int nBorderSize = 1;

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar2

BEGIN_MESSAGE_MAP(CWorkSpaceBar2, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CWorkSpaceBar2)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SORTINGPROP, OnSortingprop)
	ON_UPDATE_COMMAND_UI(ID_SORTINGPROP, OnUpdateSortingprop)
	ON_COMMAND(ID_EXPAND, OnExpand)
	ON_UPDATE_COMMAND_UI(ID_EXPAND, OnUpdateExpand)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED,OnPropertyChanged)   
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWorkSpaceBar2::OnContextMenu(CWnd* pWnd, CPoint point){} // Supress

bool updatingProperties;

void CWorkSpaceBar2::OnSortingprop() 
{
	m_wndList.SetAlphabeticMode ();
}

void CWorkSpaceBar2::OnUpdateSortingprop(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndList.IsAlphabeticMode ());
}

void CWorkSpaceBar2::OnExpand() 
{
	m_wndList.SetAlphabeticMode (FALSE);
}

void CWorkSpaceBar2::OnUpdateExpand(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (!m_wndList.IsAlphabeticMode ());
}

void CWorkSpaceBar2::SetBuildTypeToMedia()
{
	Variables::m_DBPropertyBuildType = _T("media");
	pType->SetValue((_variant_t) Variables::m_DBPropertyBuildType);
}

LRESULT CWorkSpaceBar2::OnPropertyChanged (WPARAM,LPARAM lParam)
{
	if(updatingProperties)
		return 0;

	CBCGPProp* pProp = (CBCGPProp*) lParam;

	if(pProp == m_Title)
	{
		Variables::m_DBPropertyTitle = pProp->GetValue();
		if(Variables::m_DBPropertyTitle == _T(""))
		{
			pProp->SetValue(_T("My Application"));
		}
	}
	else if(pProp == pType)
	{
		Variables::m_DBPropertyBuildType = pProp->GetValue();
		if(Variables::m_DBPropertyBuildType == _T("media"))
		{
			pCompress->Enable(TRUE);
			pEncrypt->Enable(TRUE);
		}
		else
		{
			pCompress->Enable(FALSE);
			pEncrypt->Enable(FALSE);
		}
	}
	else if(pProp == pIcon)
	{
		Variables::m_DBPropertyIcon = pProp->GetValue();
	}
	else if(pProp == pFilename)
	{
		Variables::m_DBPropertyExe = pProp->GetValue();
		if(Variables::m_DBPropertyExe == _T(""))
		{
			pProp->SetValue(_T("Temp.exe"));
		}
	}
	else if(pProp == pScreenType)
	{
		Variables::m_DBPropertyScreenType = pProp->GetValue();
		if(Variables::m_DBPropertyScreenType == _T("fullscreen"))
		{
			pFullResolution->Enable(TRUE);
		}
		else
		{
			pFullResolution->Enable(FALSE);
		}			
		if(Variables::m_DBPropertyScreenType == _T("window"))
		{
			pWindowResolution->Enable(TRUE);
		}
		else
		{
			pWindowResolution->Enable(FALSE);
		}	
	}
	else if(pProp == pFullResolution)
	{
		Variables::m_DBPropertyFullResolution = pProp->GetValue();
	}
	else if(pProp == pWindowResolution)
	{
		Variables::m_DBPropertyWindowResolution = pProp->GetValue();
	}
	else if(pProp == pCompress)
	{
		CString cStr = pProp->GetValue().bstrVal;
		Variables::m_DBPropertyCompressMedia = (cStr == _T("Yes"));
	}
	else if(pProp == pEncrypt)
	{
		CString cStr = pProp->GetValue().bstrVal;
		Variables::m_DBPropertyEncryptMedia = (cStr == _T("Yes"));
	}
	else if(pProp == pComments)
	{
		Variables::m_DBPropertyFileComments = pProp->GetValue();
	}
	else if(pProp == pCompany)
	{
		Variables::m_DBPropertyFileCompany = pProp->GetValue();
	}
	else if(pProp == pCopyright)
	{
		Variables::m_DBPropertyFileCopyright = pProp->GetValue();
	}
	else if(pProp == pDescription)
	{
		Variables::m_DBPropertyFileDescription = pProp->GetValue();
	}
	else if(pProp == pVersion)
	{
		Variables::m_DBPropertyFileVersion = pProp->GetValue();
	}

	Variables::m_ProjectChanged = true;

	return 0;
}

void CWorkSpaceBar2::UpdateFromLibrary()
{
	updatingProperties = true;
	m_Title->SetValue((_variant_t) Variables::m_DBPropertyTitle);
	pType->SetValue((_variant_t) Variables::m_DBPropertyBuildType);
	pIcon->SetValue((_variant_t) Variables::m_DBPropertyIcon);
	pFilename->SetValue((_variant_t) Variables::m_DBPropertyExe);
	pScreenType->SetValue((_variant_t) Variables::m_DBPropertyScreenType);
	pFullResolution->SetValue((_variant_t) Variables::m_DBPropertyFullResolution);
	pWindowResolution->SetValue((_variant_t) Variables::m_DBPropertyWindowResolution);
	pCompress->SetValue((_variant_t) Variables::m_DBPropertyCompressMedia ? _T("Yes") : _T("No"));
	pEncrypt->SetValue((_variant_t) Variables::m_DBPropertyEncryptMedia ? _T("Yes") : _T("No"));
	pComments->SetValue((_variant_t) Variables::m_DBPropertyFileComments);
	pCompany->SetValue((_variant_t) Variables::m_DBPropertyFileCompany);
	pCopyright->SetValue((_variant_t) Variables::m_DBPropertyFileCopyright);
	pDescription->SetValue((_variant_t) Variables::m_DBPropertyFileDescription);
	pVersion->SetValue((_variant_t) Variables::m_DBPropertyFileVersion);
	updatingProperties = false;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar2 message handlers

int CWorkSpaceBar2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	updatingProperties = true;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create combo box
	m_wndToolBar.Create (this, dwDefaultToolbarStyle, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar (IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle ();

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
		
	m_wndToolBar.SetBarStyle (m_wndToolBar.GetBarStyle () & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner (this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame (FALSE);

	// Create tree windows.
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
		TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndList.Create (dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	m_wndList.EnableHeaderCtrl (FALSE);
	m_wndList.EnableDesciptionArea ();
	m_wndList.SetVSDotNetLook ();
	m_wndList.MarkModifiedProperties ();

	CBCGPProp* pGroup1 = new CBCGPProp (_T("Application"));
	m_Title = new CBCGPProp (_T("Title"), (_variant_t) Variables::m_DBPropertyTitle,
		_T("Enter the text that you would like to be shown in your applications title bar"));
	pGroup1->AddSubItem (m_Title);

	/*
	exe
	media
	installer
	alone
	*/
	pType = new CBCGPProp (_T("Application Type"), (_variant_t) Variables::m_DBPropertyBuildType,
		_T("The type of application produced by the compiler"));
	pType->AddOption (_T("exe"));
	pType->AddOption (_T("media"));
	pType->AddOption (_T("installer"));
	pType->AddOption (_T("alone"));
	pType->AllowEdit (FALSE);

	pGroup1->AddSubItem (pType);

	static TCHAR BASED_CODE szIconFilter[] = _T("Icon Files (*.ico)|*.ico|Bitmap Files (*.bmp)|*.bmp||");
	pIcon = new CBCGPFileProp (_T("Icon"), TRUE, _T(""), _T("ico"), 0, szIconFilter, 
		_T("Specifies the icon used by your application"));
	pGroup1->AddSubItem (pIcon);

	static TCHAR BASED_CODE szApplicationFilter[] = _T("Exe Files (*.exe)|*.exe|Screensaver Files (*.scr)|*.scr||");
	pFilename = new CBCGPFileProp (_T("Filename"), TRUE, Variables::m_DBPropertyExe, _T("exe"), 0, szApplicationFilter, 
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
	pScreenType = new CBCGPProp (_T("Screen Type"), _T("window"),
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
	pWindowResolution = new CBCGPProp (_T("Window Resolution"), _T("800x600"),
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
	pFullResolution = new CBCGPProp (_T("Fullscreen Resolution"), _T("800x600x16"),
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
	pCompress = new CBCGPProp (_T("Compress"), _T("No"),
		_T("Option to compress media that is part of the project"));
	pCompress->AddOption (_T("Yes"));
	pCompress->AddOption (_T("No"));
	pCompress->AllowEdit (FALSE);

	pGroup3->AddSubItem (pCompress);

	/*
	Yes
	No
	*/
	pEncrypt = new CBCGPProp (_T("Encrypt"), _T("No"),
		_T("Option to encrypt media that is part of the project"));
	pEncrypt->AddOption (_T("Yes"));
	pEncrypt->AddOption (_T("No"));
	pEncrypt->AllowEdit (FALSE);

	pGroup3->AddSubItem (pEncrypt);

	m_wndList.AddProperty(pGroup3);

	CBCGPProp* pGroup4 = new CBCGPProp (_T("Version Information"));
	pComments = new CBCGPProp (_T("Comments"), (_variant_t) _T(""),
		_T("Enter any comments regarding the application"));
	pGroup4->AddSubItem (pComments);
	pCompany = new CBCGPProp (_T("Company"), (_variant_t) _T(""),
		_T("Enter the company details regarding the application"));
	pGroup4->AddSubItem (pCompany);
	pCopyright = new CBCGPProp (_T("Copyright"), (_variant_t) _T(""),
		_T("Enter the applications copyright"));
	pGroup4->AddSubItem (pCopyright);
	pDescription = new CBCGPProp (_T("Description"), (_variant_t) _T(""),
		_T("Enter the applications description"));
	pGroup4->AddSubItem (pDescription);
	pVersion = new CBCGPProp (_T("Version"), (_variant_t) _T("1.0.0.0"),
		_T("Enter the version number of your application in the format a.b.c.d"));
	pGroup4->AddSubItem (pVersion);
	m_wndList.AddProperty(pGroup4);

	updatingProperties = false;
	return 0;
}

void CWorkSpaceBar2::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	CRect rectClient;
	GetClientRect (rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout (FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos (NULL,
							   rectClient.left, 
							   rectClient.top, 
							   rectClient.Width (),
							   cyTlb,
							   SWP_NOACTIVATE | SWP_NOZORDER);


	m_wndList.SetWindowPos (NULL,
									  rectClient.left, 
									  rectClient.top + cyTlb, 
									  rectClient.Width (),
									  rectClient.Height () - cyTlb,
									  SWP_NOACTIVATE | SWP_NOZORDER);
}

void CWorkSpaceBar2::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndList.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
		::GetSysColor (COLOR_3DSHADOW));
}

void CWorkSpaceBar2::OnChangeVisualStyle ()
{
	m_wndToolBar.CleanUpLockedImages ();
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;
	m_wndToolBar.LoadBitmap (bIsHighColor ? 
		IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);
}
