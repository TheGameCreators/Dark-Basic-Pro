// TGCOnline.cpp : Defines the class behaviors for the application.
//

// Includes
#include "stdafx.h"
#include "TGCOnline.h"
#include "TGCOnlineDlg.h"
#include "resource.h"
#include "direct.h"
#include "ole2.h"

/////////////////////////////////////////////////////////////////////////////
// CTGCOnlineApp

BEGIN_MESSAGE_MAP(CTGCOnlineApp, CWinApp)
	//{{AFX_MSG_MAP(CTGCOnlineApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTGCOnlineApp construction

CTGCOnlineApp::CTGCOnlineApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTGCOnlineApp object

CTGCOnlineApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTGCOnlineApp initialization

BOOL CTGCOnlineApp::InitInstance()
{
	// Run Main Dialog
	AfxEnableControlContainer();
	//Enable3dControls(); // .NET 2003

	// Ensure we are in the correct current working folder of this executable
	// so can find the text files needed to populate the text buttons, etc
	char ActualEXEFilename[_MAX_PATH];
	GetModuleFileName(this->m_hInstance, ActualEXEFilename, _MAX_PATH);
	char ActualEXEFolder[_MAX_PATH];
	strcpy ( ActualEXEFolder, ActualEXEFilename );
	for ( int n=strlen(ActualEXEFolder); n>0; n-- )
	{
		if ( ActualEXEFolder[n]=='\\' || ActualEXEFolder[n]=='/' )
		{
			ActualEXEFolder[n]=0;
			break;
		}
	}
	if ( strlen ( ActualEXEFolder ) > 0 ) _chdir ( ActualEXEFolder );

	// Try skin version first
	int m_nModalResult = -1;
	if ( m_nModalResult==-1 )
	{
		// fall back to no skin version if fail
		CTGCOnlineDlg dlg ( IDD_TGCONLINE_DIALOGNOSKIN );
		int iSecondAttempt = dlg.DoModal();
	}

	// complete
	return FALSE;
}
