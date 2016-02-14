// Includes
#include "stdafx.h"
#include "Synergy Editor.h"
#include "ChildFrm.h"
#include "Doc.h"
#include "View.h"
#include "Help.h"
#include "HTMLDocument.h"
#include "Keywords.h"
#include "Utilities.h"
#include "Settings.h"
#include "ReadProject.h"
#include "afxwin.h"
#include "Variables.h"
#include "direct.h"
#include "io.h"
//#include "adpcore.h"

// Globals
int g_uTriggerIntroOn = 0;
bool g_bIsNETBOOKVersion = false;

// Prototype for associated GETDXVER.CPP (or empty true in MAIN.CPP if compiler)
DWORD g_dwDirectXVersion = 0;
TCHAR g_strDirectXVersion[10] = { 0 };
HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );

// Message Pump
BEGIN_MESSAGE_MAP(App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_HELP_DARKBASICPROFESSIONALHELP, OnHelp)
END_MESSAGE_MAP()

// App construction
App::App() :
	CBCGPWorkspace (TRUE /* m_bResourceSmartUpdate */)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only App object
App theApp;

// App initialization
BOOL App::InitInstance()
{
	/*
	// U75 - 171109 - check if special NETBOOK version
	g_bIsNETBOOKVersion = false;
	char pOldDir [ _MAX_PATH ];
	_getcwd ( pOldDir, _MAX_PATH );
	createdSettings = Settings::LoadSettings();
	CString pCertFolder = Settings::DBPLocation + L"Compiler\\certificates\\";
	SetCurrentDirectory ( pCertFolder );
	_finddata_t filedata;
	long hInternalFile = _findfirst("*.*", &filedata);
	if(hInternalFile!=-1L)
	{
		int FileReturnValue = 0;
		bool bContainsDBPString = false;
		while ( FileReturnValue==0 && g_bIsNETBOOKVersion==false )
		{
			if( !(filedata.attrib & _A_SUBDIR) )
			{
				char pFilename[256];
				strcpy ( pFilename, filedata.name );
				if ( pFilename )
					if ( stricmp ( pFilename, "a3330fee1fd9f540f18ee37c331c46ea" )==NULL )
						g_bIsNETBOOKVersion = true;
			}
			FileReturnValue = _findnext(hInternalFile, &filedata);
		}
		_findclose(hInternalFile);
		hInternalFile=NULL;
	}
	_chdir ( pOldDir );
	if ( g_bIsNETBOOKVersion==true )
	{
		// U75 - NETBOOK - 111109 - initial authorization
		ADP_RET_CODE ret_code;
		ADP_APPLICATIONID myApplicationID = {{ 0x10e618f4,0xf8f1465c,0x9ea22267,0x016b47ee}};
		//ADP_APPLICATIONID myApplicationID = ADP_DEBUG_APPLICATIONID;
		if ((ret_code = ADP_Initialize()) != ADP_SUCCESS )
		{ 
			switch( ret_code ) 
			{ 
				case ADP_INCOMPATIBLE_VERSION: 
					MessageBox ( NULL, _T( "ERROR: Incompatible version" ), _T("Dark Basic Pro Authorization"), MB_OK ); 
					break; 

				case ADP_NOT_AVAILABLE: 
					MessageBox ( NULL, _T( "ERROR: Client Agent not running" ), _T("Dark Basic Pro Authorization"), MB_OK ); 
					break; 

				case ADP_FAILURE: 
				default: 
					MessageBox ( NULL, _T( "ERROR: Unknown error" ), _T("Dark Basic Pro Authorization"), MB_OK ); 
					break; 
			} 
			ADP_Close();
			return FALSE;
		}
		if (( ret_code = ADP_IsAuthorized( myApplicationID )) != ADP_AUTHORIZED )
		{ 
			if ( ret_code == ADP_NOT_AUTHORIZED ) 
			{ 
				MessageBox ( NULL, _T( "ERROR: This application is not authorized to run" ), _T("Dark Basic Pro Authorization"), MB_OK ); 
				ADP_Close();
				return FALSE;
			}
		}
	}
	*/

	// U75 - 291009 - does user have sufficient DirectX installed?
	/* 130110 - to avoid further validation delays, remove DX check
	LPSTR pDXRequired = "DirectX 9.0c (October 2006) or later";
	DWORD dwRequiresVersion = 0x00090003;
    char strResult[128];
    HRESULT hr = GetDXVersion( &g_dwDirectXVersion, g_strDirectXVersion, 10 );
    if( SUCCEEDED(hr) )
    {
        if( g_dwDirectXVersion > 0 )
			wsprintfA ( strResult, "DirectX %s installed (%d). Requires %s.", g_strDirectXVersion, g_dwDirectXVersion, pDXRequired );
        else
            wsprintfA ( strResult, "DirectX not installed" );
    }
    else
	{
        wsprintfA ( strResult, "Unknown version of DirectX installed" );
	}
	if ( g_dwDirectXVersion < dwRequiresVersion )
	{
		MessageBoxA(NULL, strResult, "DirectX Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	*/

	// _CrtSetBreakAlloc( 61762 );
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	CString date(__DATE__);
	SetRegistryKey(_T("DarkBASIC Professional Editor - " ) + date);
	LoadStdProfileSettings(6);  // Load standard INI file options (including MRU)

	SetRegistryBase (_T("Settings"));

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	InitContextMenuManager();
	InitKeyboardManager();
	InitShellManager ();

	// Enable user-defined tools. If you want allow more than 10 tools,
	// add tools entry to resources (ID_USER_TOOL11, ID_USER_TOOL12,...)
	EnableUserTools (ID_TOOLS_ENTRY, ID_USER_TOOL1, ID_USER_TOOL10, 
					RUNTIME_CLASS (CBCGPUserTool));

	// TODO: Remove this if you don't want extended tooltips:
	InitTooltipManager();

	CBCGPToolTipParams params;
	params.m_bVislManagerTheme = TRUE;

	theApp.GetTooltipManager ()->SetTooltipParams (
		BCGP_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS (CBCGPToolTipCtrl),
		&params);

	// U75 - 171109 - moved further up to get DBP location for netbook check
	//createdSettings = Settings::LoadSettings();
	Keywords::ReadKeywords();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views	
	DWORD dwMainMenuResource = IDR_UntitledTYPE;
	if ( g_bIsNETBOOKVersion==true ) dwMainMenuResource = IDR_UntitledTYPE_NETBOOK;
	pDocTemplate = new CMultiDocTemplate(dwMainMenuResource,
		RUNTIME_CLASS(Doc),
		RUNTIME_CLASS(CBCGPMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pHelpTemplate = new CMultiDocTemplate(IDR_Help,
		RUNTIME_CLASS(HTMLDocument),
		RUNTIME_CLASS(CBCGPMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CHelp));
	if (!pHelpTemplate)
		return FALSE;

	// create main MDI Frame window
	pMainFrame = new MainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		DoSelectedAction();
	}
	else
	{
		// Dispatch commands specified on the command line
		if(cmdInfo.m_strFileName != _T(""))
		{
			CString FileNameUpper(cmdInfo.m_strFileName);
			FileNameUpper.MakeUpper();
			if(FileNameUpper.Right(5) == _T("DBPRO"))
			{
				AddToRecentFileList(cmdInfo.m_strFileName);
				ReadProject::Read(cmdInfo.m_strFileName);
				pMainFrame->m_DefaultPath = Utilities::ExtractPath(cmdInfo.m_strFileName);
			}
			else if(FileNameUpper.Right(3) == _T("DBA"))
			{
				Variables::Init(); // Clear the variables
				pDocTemplate->OpenDocumentFile(cmdInfo.m_strFileName);
				pMainFrame->UpdateFromLibrary();
				pMainFrame->m_DefaultPath = Utilities::ExtractPath(cmdInfo.m_strFileName);
			}
		}
		else
		{
			DoSelectedAction();
		}
	}
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	Variables::m_ProjectChanged = false; // Reset flag after a new file has been created

	// U75 - 281009 - read if intro should be displayed on start-up
	g_uTriggerIntroOn = 1;
	TCHAR tszBuf[MAX_PATH];
	CString appPath = Utilities::ExtractPath(Utilities::GetApplicationPath());
	CString LaunchCFGFile = appPath + _T("..\\launch.cfg");
	GetPrivateProfileString(_T("LAUNCH FILES"),_T("IntroOn"),_T("Yes"),tszBuf,MAX_PATH,LaunchCFGFile);
	if ( tszBuf[0]=='n' ) g_uTriggerIntroOn = 0;
	if ( tszBuf[0]=='N' ) g_uTriggerIntroOn = 0;
	if ( g_uTriggerIntroOn==1 )
	{
		CString entry = Settings::DBPLocation + _T("Help\\") + Settings::MainHelpURL;
		App* app = (App*)AfxGetApp();
		app->pHelpTemplate->OpenDocumentFile(entry);
	}

	return TRUE;
}

// Performs a startup operation chosen by the user
void App::DoSelectedAction()
{
	if(Settings::OnStartup == 0)
	{
		// Do nothing
		Variables::Init(); // Clear the variables
	}
	else if(Settings::OnStartup == 1)
	{
		stopNewDialog = true;
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEWPROJECT);
	}
	else if(Settings::OnStartup == 2)
	{
		pMainFrame->OnOpenProject();
	}
	else
	{
		if(Settings::LastProject != _T("") && Utilities::CheckFileExists(Settings::LastProject))
		{
			ReadProject::Read(Settings::LastProject);
			pMainFrame->m_DefaultPath = Utilities::ExtractPath(Settings::LastProject);
		}
		else
		{
			stopNewDialog = true;
			AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEWPROJECT);
		}
	}
}

// Overide to prevent help files being added
void App::AddToRecentFileList(LPCTSTR lpszPathName)
{
	CString item(lpszPathName);
	item.MakeUpper();
	if(item.Right(5) == _T("DBPRO"))
	{
		CWinApp::AddToRecentFileList(lpszPathName);
	}
}

BOOL App::OnOpenRecentFile(UINT nID)
{
	int nIndex = nID - ID_FILE_MRU_FILE1;
	CString strMRU = (*m_pRecentFileList)[nIndex];
	CString strMRUUpper = strMRU;
	strMRUUpper.MakeUpper();
	if(strMRUUpper.Right(5) == _T("DBPRO"))
	{
		if(!Utilities::CheckFileExists(strMRU))
		{
			AfxMessageBox(_T("The project file does not exist, cannot continue loading"), MB_ICONERROR | MB_OK);
			m_pRecentFileList->Remove(nIndex);
		}
		else
		{
			pMainFrame->OnOpenProject(strMRU);
		}
	}
	else
	{
		if(OpenDocumentFile(strMRU) == NULL)
		{
			m_pRecentFileList->Remove(nIndex);
		}
	}

	return TRUE;
}

// App message handlers

int App::ExitInstance() 
{
	Variables::ClearIncludes();
	Keywords::TidyUp();

	delete pHelpTemplate;
//	delete pAdvertTemplate;

	GdiplusShutdown(m_gdiplusToken);

	BCGCBProCleanUp();

	return CWinApp::ExitInstance();
}

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
	CBCGPURLLinkButton m_btnURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CStatic cVersion;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPANY_URL, m_btnURL);
	DDX_Control(pDX, IDC_VERSION, cVersion);
}

BOOL CAboutDlg::OnInitDialog()
{
	if(!CDialog::OnInitDialog())
		return FALSE;	

	CString date(__DATE__);
	cVersion.SetWindowTextW(_T("Build: ") + date);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// App command to run the dialog
void App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void App::OnHelp()
{
	pHelpTemplate->OpenDocumentFile(Settings::DBPLocation + _T("Help\\main.htm"));
}

// App message handlers
void App::PreLoadState ()
{
	GetContextMenuManager()->AddMenu (_T("My menu"), IDR_CONTEXT_MENU);
}

