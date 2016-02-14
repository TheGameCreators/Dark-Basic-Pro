// MainFrm.cpp : implementation of the MainFrame class

#include "stdafx.h"
#include "Synergy Editor.h"
#include "MainFrm.h"
#include "EditorControl.h"
#include "CompileSupport.h"
#include "Variables.h"
#include "WriteProject.h"
#include "WriteFiles.h"
#include "Utilities.h"
#include "ReadProject.h"
#include "Keywords.h"
#include "OptionsDlg.h"
#include "KeystateWindow.h"
#include "Settings.h"
#include "HTMLDocument.h"
#include "NewProjectDialog.h"
#include "SemanticParser.h"
#include "Doc.h"
#include "InputDialog.h"
#include "FindInProjectDialog.h"
#include "AdvertThread.h"
#include "HtmlViewCustom.h"

// Defines
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// External global - set in AdvertThread.cpp
extern bool g_bUsingAdvertising;
extern bool g_bUsingAdvertisingLoaded;
extern LONGLONG g_lUsingAdvertisingTimeStamp;
extern bool g_bIsNETBOOKVersion;

// MainFrame
IMPLEMENT_DYNAMIC(MainFrame, CBCGPMDIFrameWnd)

const int  iMaxUserToolbars		= 10;
const UINT uiFirstUserToolBarId	= AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId	= uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(MainFrame, CBCGPMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_ACTIVATEAPP()

	ON_MESSAGE(WM_USER, OnUser)
	ON_MESSAGE(WM_USER + 1, SetStatusMaxMessage)
	ON_MESSAGE(WM_USER + 2, SetStatusMessage)
	ON_MESSAGE(WM_USER + 3, SetOutputMessage)

	ON_COMMAND(ID_WINDOW_MANAGER, OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_2007_3, OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_2007_3, OnUpdateAppLook)
	ON_COMMAND(ID_MDI_MOVE_TO_NEXT_GROUP, OnMdiMoveToNextGroup)
	ON_COMMAND(ID_MDI_MOVE_TO_PREV_GROUP, OnMdiMoveToPrevGroup)
	ON_COMMAND(ID_MDI_NEW_HORZ_TAB_GROUP, OnMdiNewHorzTabGroup)
	ON_COMMAND(ID_MDI_NEW_VERT_GROUP, OnMdiNewVertGroup)
	ON_COMMAND(ID_MDI_CANCEL, OnMdiCancel)

	// John...
	ON_COMMAND(ID_FILE_NEWPROJECT, OnNewProject)
	ON_COMMAND(ID_FILE_NEWDBA, OnNewFile)
	ON_COMMAND(ID_FILE_OPENPROJECT, OnOpenProject)
	ON_COMMAND(ID_FILE_SAVEPROJECT, OnSaveProject)
	ON_COMMAND(ID_FILE_SAVEPROJECTAS, OnSaveProjectAs)
	ON_COMMAND(ID_FILE_SAVEALL, OnSaveAll)
	ON_COMMAND(ID_FILE_OPEN_ANY, OnOpenAny)

	ON_COMMAND(ID_EDIT_FINDINFILES, OnFindInFiles)
	ON_COMMAND(ID_ADVANCED_GOTOPROJECTLINE, OnGotoLine)

	ON_COMMAND(ID_COMPILE_COMPILE, OnCompile)
	ON_UPDATE_COMMAND_UI_RANGE(ID_COMPILE_COMPILE, ID_COMPILE_COMPILERUNINSTEP, OnUpdateCompile)

	ON_COMMAND(ID_COMPILE_COMPILERUN, OnCompileRun)

	ON_COMMAND(ID_OPENSTORE, OnOpenGameCreatorStore)
	ON_COMMAND(ID_CHECKMODULES, OnCheckModules)

	ON_COMMAND(ID_COMPILE_COMPILERUNINDEBUGMODE, OnDebug)
	ON_COMMAND(ID_COMPILE_COMPILERUNINSTEP, OnStep)
	ON_COMMAND(ID_COMPILE_EXECUTEPREVIOUSAPPLICATION, OnRunPrevious)
	ON_UPDATE_COMMAND_UI(ID_COMPILE_EXECUTEPREVIOUSAPPLICATION, OnUpdateRunPrevious)

	ON_COMMAND(ID_TOOLS_COMMANDARGUMENTS, OnCommandArgs)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnCustomise)
	ON_COMMAND(ID_VIEW_OPTIONS, OnOptions)

	ON_COMMAND(ID_TOOLS_KEYSTATEHELPER, OnKeyHelper)
	ON_COMMAND(ID_TOOLS_REBUILDKEYWORDS, OnRebuild)
	ON_COMMAND(ID_TOOLS_ASSOCIATEPROJECTANDSOURCEFILES, AssociateFiles)

	ON_COMMAND(ID_VIEW_FULLSCREEN, OnFullScreen)

	ON_COMMAND(ID_HELP_DARKBASICPROFESSIONALVIDEOS32997, OnTutorial)
	ON_UPDATE_COMMAND_UI(ID_HELP_DARKBASICPROFESSIONALVIDEOS32997, OnTutorialUpdate)
	ON_COMMAND(ID_HELP_CHECKFORUPDATES, OnUpdate)
	ON_COMMAND(ID_HELP_CHECKFORMODULES, OnCheckModules)
	ON_COMMAND(ID_HELP_KEYBOARDMAP, OnKeyboard)
	ON_COMMAND(ID_HELP_ABOUTDARKBASICPROFESSIONAL, OnDBPAbout)
	ON_COMMAND(ID_HELP_DARKGAMESTUDIOWEBSITE, OnDGS)

	// ...John
	// Lee 310808...
	ON_COMMAND(ID_HELP_OPENGAMECREATORSTORE, OnOpenGameCreatorStore)
	// ...Lee
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// MainFrame construction/destruction
MainFrame::MainFrame()
{	
	m_nAppLook = theApp.GetInt (_T("ApplicationLook"), ID_VIEW_APPLOOK_VS2008);	
	wd = NULL;
	hWndToolTip = NULL;
}

MainFrame::~MainFrame()
{
	if(wd)
	{
		delete wd;
	}	
}

int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	OnAppLook (m_nAppLook);

	CBCGPToolBar::EnableQuickCustomization ();

	CList<UINT, UINT>	lstBasicCommands;

	lstBasicCommands.AddTail (ID_VIEW_TOOLBARS);
	lstBasicCommands.AddTail (ID_FILE_NEW);
	lstBasicCommands.AddTail (ID_FILE_NEWDBA);
	lstBasicCommands.AddTail (ID_FILE_OPEN);
	lstBasicCommands.AddTail (ID_FILE_SAVE);
	lstBasicCommands.AddTail (ID_FILE_PRINT);
	lstBasicCommands.AddTail (ID_FILE_NEWPROJECT);
	lstBasicCommands.AddTail (ID_FILE_OPENPROJECT);
	lstBasicCommands.AddTail (ID_FILE_SAVEPROJECT);
	lstBasicCommands.AddTail (ID_FILE_SAVEALL);
	lstBasicCommands.AddTail (ID_APP_EXIT);
	lstBasicCommands.AddTail (ID_EDIT_CUT);
	lstBasicCommands.AddTail (ID_EDIT_COPY);
	lstBasicCommands.AddTail (ID_EDIT_SELECT_ALL);
	lstBasicCommands.AddTail (ID_EDIT_PASTE);
	lstBasicCommands.AddTail (ID_EDIT_FIND);
	lstBasicCommands.AddTail (ID_EDIT_FINDINFILES);
	lstBasicCommands.AddTail (ID_EDIT_REPLACE);
	lstBasicCommands.AddTail (ID_EDIT_UNDO);
	lstBasicCommands.AddTail (ID_EDIT_REDO);
	lstBasicCommands.AddTail (ID_EDIT_TOGGLEBOOKMARK);
	lstBasicCommands.AddTail (ID_EDIT_NEXTBOOKMARK);
	lstBasicCommands.AddTail (ID_EDIT_PREVIOUSBOOKMARK);
	lstBasicCommands.AddTail (ID_EDIT_REMOVEALLBOOKMARKS);
	lstBasicCommands.AddTail (ID_EDIT_TOGGLEBREAKPOINT);
	lstBasicCommands.AddTail (ID_EDIT_REMOVEALLBREAKPOINTS);
	lstBasicCommands.AddTail (ID_EDIT_GOTOLINE);
	lstBasicCommands.AddTail (ID_ADVANCED_GOTOPROJECTLINE);
	lstBasicCommands.AddTail (ID_EDIT_STOPHIDINGCURRENT);
	lstBasicCommands.AddTail (ID_EDIT_TOGGLEOUTLINING);
	lstBasicCommands.AddTail (ID_EDIT_TOGGLEALLOUTLINING);
	lstBasicCommands.AddTail (ID_EDIT_COLLAPSETODEFINITIONS);
	lstBasicCommands.AddTail (ID_EDIT_STOPOUTLINING);
	lstBasicCommands.AddTail (ID_EDIT_AUTOOUTLINING);
	lstBasicCommands.AddTail (ID_EDIT_ENABLEOUTLINING);
	lstBasicCommands.AddTail (ID_EDIT_LINENUMBERS);
	lstBasicCommands.AddTail (ID_ADVANCED_COMMENTSELECTION);
	lstBasicCommands.AddTail (ID_ADVANCED_UNCOMMENTSELECTION);
	lstBasicCommands.AddTail (ID_ADVANCED_INCREASELINEINDENT);
	lstBasicCommands.AddTail (ID_ADVANCED_DECREASELINEINDENT);
	lstBasicCommands.AddTail (ID_ADVANCED_GOTODEFINITION);
	lstBasicCommands.AddTail (ID_ADVANCED_GOTOLASTLINE);
	lstBasicCommands.AddTail (ID_COMPILE_COMPILE);
	lstBasicCommands.AddTail (ID_COMPILE_COMPILERUN);
	lstBasicCommands.AddTail (ID_COMPILE_COMPILERUNINDEBUGMODE);
	lstBasicCommands.AddTail (ID_COMPILE_COMPILERUNINSTEP);
	lstBasicCommands.AddTail (ID_COMPILE_EXECUTEPREVIOUSAPPLICATION);
	lstBasicCommands.AddTail (ID_RECORD_NEXT);
	lstBasicCommands.AddTail (ID_RECORD_LAST);
	lstBasicCommands.AddTail (ID_TOOLS_KEYSTATEHELPER);
	lstBasicCommands.AddTail (ID_TOOLS_COMMANDARGUMENTS);
	lstBasicCommands.AddTail (ID_TOOLS_REBUILDKEYWORDS);
	lstBasicCommands.AddTail (ID_TOOLS_ASSOCIATEPROJECTANDSOURCEFILES);
	lstBasicCommands.AddTail (ID_APP_ABOUT);
	lstBasicCommands.AddTail (ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail (ID_VIEW_CUSTOMIZE);
	lstBasicCommands.AddTail (ID_VIEW_FULLSCREEN);
	lstBasicCommands.AddTail (ID_VIEW_OPTIONS);
	lstBasicCommands.AddTail (ID_WINDOW_TILE_HORZ);
	lstBasicCommands.AddTail (ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2000);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_XP);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2003);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2005);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_WIN_XP);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_1);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_2);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_3);
	lstBasicCommands.AddTail (ID_HELP_DARKBASICPROFESSIONALVIDEOS32997);
	lstBasicCommands.AddTail (ID_HELP_DARKBASICPROFESSIONALHELP);
	lstBasicCommands.AddTail (ID_HELP_DIGITALZENITHLTDONTHEWEB);
	lstBasicCommands.AddTail (ID_HELP_CHECKFORUPDATES);
	lstBasicCommands.AddTail (ID_HELP_CHECKFORMODULES);
	lstBasicCommands.AddTail (ID_HELP_KEYBOARDMAP);
	lstBasicCommands.AddTail (ID_HELP_ABOUTDARKBASICPROFESSIONAL);
	lstBasicCommands.AddTail (ID_HELP_DARKGAMESTUDIOWEBSITE);

	// Lee 310808
	lstBasicCommands.AddTail (ID_HELP_OPENGAMECREATORSTORE);

	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_DARKBASICPROFESSIONAL);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_DARKPHYSICSANDDARKA);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_NEWCOMERSCORNER);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_CODESNIPPETS);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_BUGREPORTS);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_DLLTALK);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_3DIMENSIONALCHAT);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_2DALLTHEWAY);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_GAMEDESIGNTHEORY);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_SYNERGYIDESYNERGYEDITOR);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_PROGRAMANNOUNCEMENTS);
	lstBasicCommands.AddTail (ID_THEGAMECREATORSFORUMS_WORKINPROGRESS);
	lstBasicCommands.AddTail (ID_EDIT_HIDESELECTION);

	// Browser
	lstBasicCommands.AddTail (ID_BROWSER_GOBACK);
	lstBasicCommands.AddTail (ID_BROWSER_GOFORWARD);
	lstBasicCommands.AddTail (ID_BROWSER_STOP);
	lstBasicCommands.AddTail (ID_BROWSER_REFRESH);
	lstBasicCommands.AddTail (ID_FONT_LARGEST);
	lstBasicCommands.AddTail (ID_FONT_LARGE);
	lstBasicCommands.AddTail (ID_FONT_MEDIUM);
	lstBasicCommands.AddTail (ID_FONT_SMALL);
	lstBasicCommands.AddTail (ID_FONT_SMALLEST);

	CBCGPToolBar::SetBasicCommands (lstBasicCommands);

	// U70 - By default we want recently used OFF (first time only, registry overwrites this later)
	m_wndMenuBar.SetRecentlyUsedMenus(0);

	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	// Detect color depth. 256 color toolbars can be used in the high or true color modes only (bits per pixel is > 8):
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;

	DWORD dwToolbarLayout = IDR_MAINFRAME;
	if ( g_bIsNETBOOKVersion==true ) dwToolbarLayout = IDR_MAINFRAME_NETBOOK;
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1),ID_VIEW_TOOLBAR) ||
		!m_wndToolBar.LoadToolBar(dwToolbarLayout, 0, 0, FALSE, 0, 0, bIsHighColor ? IDB_TOOLBAR256 : 0))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strMainToolbarTitle;
	strMainToolbarTitle.LoadString (IDS_MAIN_TOOLBAR);
	m_wndToolBar.SetWindowText (strMainToolbarTitle);

	if (!m_wndHelpToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1), ID__BROWSERTOOLBAR) ||
		!m_wndHelpToolBar.LoadToolBar(IDR_HelpA, 0, 0, FALSE, 0, 0, bIsHighColor ? IDB_BROWSER : 0))
	{
		TRACE0("Failed to create help toolbar\n");
		return -1;      // fail to create
	}

	CString strBrowserToolbarTitle;
	strBrowserToolbarTitle.LoadString (IDS_BROWSER_TOOLBAR);
	m_wndHelpToolBar.SetWindowText (strBrowserToolbarTitle);

	if (!m_wndEditToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(1,1,1,1), ID_EDITTOOLBAR) ||
		!m_wndEditToolBar.LoadToolBar(IDR_EDIT, 0, 0, FALSE, 0, 0, bIsHighColor ? IDB_EDIT : 0))
	{
		TRACE0("Failed to create edit toolbar\n");
		return -1;      // fail to create
	}

	CString strEditToolbarTitle;
	strEditToolbarTitle.LoadString (IDS_EDIT_TOOLBAR);
	m_wndEditToolBar.SetWindowText (strEditToolbarTitle);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Load control bar icons:
	CBCGPToolBarImages imagesWorkspace;
	imagesWorkspace.SetImageSize (CSize (16, 16));
	imagesWorkspace.SetTransparentColor (RGB (255, 0, 255));
	imagesWorkspace.Load (IDB_WORKSPACE);
	
	if (!m_SolutionExplorer.Create (_T("Solution Explorer"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_WORKSPACE,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create solution explorer\n");
		return -1;      // fail to create
	}

	m_SolutionExplorer.SetIcon (imagesWorkspace.ExtractIcon (0), FALSE);

	if (!m_Media.Create (_T("Media"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_MEDIA,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create media bar\n");
		return -1;      // fail to create
	}

	m_Media.SetIcon (imagesWorkspace.ExtractIcon (2), FALSE);

	if (!m_Cursor.Create (_T("Cursors"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_CURSORS,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create cursor bar\n");
		return -1;      // fail to create
	}

	m_Cursor.SetIcon (imagesWorkspace.ExtractIcon (5), FALSE);

	if (!m_Properties.Create (_T("Properties"), this, CRect (0, 0, 200, 200),
	TRUE, ID_VIEW_WORKSPACE2,
	WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create properties bar\n");
		return -1;      // fail to create
	}

	m_Properties.SetIcon (imagesWorkspace.ExtractIcon (1), FALSE);


	if (!m_ToDo.Create (_T("To Do and Comments"), this, CSize (200, 200),
		TRUE, ID_VIEW_TODO,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create To Do List\n");
		return -1;      // fail to create
	}

	m_ToDo.SetIcon (imagesWorkspace.ExtractIcon (3), FALSE);

	if (!m_wndOutput.Create (_T("Output"), this, CSize (730, 170),
		TRUE, ID_VIEW_OUTPUT,
		CBRS_SIZE_FIXED | WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
	{
		TRACE0("Failed to create output bar\n");
		return -1;      // fail to create
	}

	if (!m_ClassView.Create (_T("Code View"), this, CSize (200, 200),
		TRUE, ID_VIEW_CLASSVIEW,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Code View\n");
		return -1;      // fail to create
	}

	m_ClassView.SetIcon (imagesWorkspace.ExtractIcon (4), FALSE);

	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndHelpToolBar.EnableDocking(CBRS_ALIGN_ANY); // Help
	m_wndEditToolBar.EnableDocking(CBRS_ALIGN_ANY); // Edit
	m_SolutionExplorer.EnableDocking(CBRS_ALIGN_ANY);
	m_Properties.EnableDocking(CBRS_ALIGN_ANY);
	m_Media.EnableDocking(CBRS_ALIGN_ANY);
	m_Cursor.EnableDocking(CBRS_ALIGN_ANY);
	m_ToDo.EnableDocking(CBRS_ALIGN_ANY);
	m_ClassView.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndMenuBar);
	DockControlBar(&m_wndHelpToolBar);
	DockControlBarLeftOf(&m_wndEditToolBar, &m_wndHelpToolBar); // Help
	DockControlBarLeftOf(&m_wndToolBar, &m_wndEditToolBar); // Edit
	DockControlBar(&m_SolutionExplorer);

	// U75 - 211009 - fix size of output (min size)
	DockControlBar(&m_wndOutput);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	CSize size = CSize ( 730, 170 );
	m_wndOutput.SetMinSize(size);
	m_wndOutput.SetResizeMode(FALSE);

	m_Media.AttachToTabWnd (&m_SolutionExplorer, DM_SHOW, FALSE, NULL);
	m_Cursor.AttachToTabWnd (&m_SolutionExplorer, DM_SHOW, FALSE, NULL);
	m_Properties.AttachToTabWnd (&m_SolutionExplorer, DM_SHOW, FALSE, NULL);
	m_ToDo.AttachToTabWnd (&m_SolutionExplorer, DM_SHOW, FALSE, NULL);	
	m_ClassView.AttachToTabWnd (&m_SolutionExplorer, DM_SHOW, FALSE, NULL);	

	m_wndToolBar.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
	m_wndHelpToolBar.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize...")); // Help
	m_wndEditToolBar.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize...")); // Edit

	// Allow user-defined toolbars operations:
	InitUserToobars (NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	// Enable control bar context menu (list of bars + customize command):
	EnableControlBarMenu (TRUE,	// Enable
							0, 	// Customize command ID
							_T("Customize..."),	// Customize command text
							ID_VIEW_TOOLBARS);	// Menu items with this ID will be replaced by toolbars menu

	EnableFullScreenMode (ID_VIEW_FULLSCREEN);

	m_DefaultPath = Settings::DBPLocation + _T("Projects\\");
	SemanticParser::SetClassView(m_ClassView.GetSafeHwnd());
	SemanticParserManager::Init(this);

	App* app = (App*)AfxGetApp();
	CString dir = Utilities::ExtractPath(Utilities::GetApplicationPath()) + _T("Videos\\");
	if(Utilities::CheckPathExists(dir) && (app->createdSettings || Settings::ShowWelcomeScreen))
	{
		if(wd)
		{
			delete wd;
		}
		wd = new WelcomeDialog(_T("Welcome to DarkBASIC Professional"), this);			
		wd->Create(this);
	}

	// U75 - 171109 - only reveal viewer if NOT a NETBOOK version
	if ( g_bIsNETBOOKVersion==false && !Settings::BlockCertificateViewer)
	{
		CString certificate(Settings::DBPLocation + _T("Compiler\\"));
		if(Utilities::CheckFileExists(certificate + _T("TGCCertificateViewer.exe")))
		{
			ShellExecuteW(this->GetSafeHwnd(), _T("open"), certificate + _T("TGCCertificateViewer.exe"), NULL, certificate, 1);
		}
	}

	// File change checker
	SetTimer(Variables::FileChangeChecker, 10000, 0);

	// U75 - 271009 - insert web advert on main interface (made visible later by g_bUsingAdvertising=true)
	// obtain size of advertisement
	RECT rc; rc.left=0; rc.right=0; rc.top=0; rc.bottom=0;
	// create a frame window to hold our web view
	app->pAdvertWnd = (CFrameWnd*) RUNTIME_CLASS(CFrameWnd)->CreateObject();
	app->pAdvertWnd->Create(NULL, NULL, WS_POPUP, rc, this );
	// create an html runtime view
	CCreateContext pNewContext;
	pNewContext.m_pNewViewClass = RUNTIME_CLASS(CHtmlViewCustom);
	app->pAdvertView = (CHtmlViewCustom *) app->pAdvertWnd->CreateView(&pNewContext);
	if ( app->pAdvertView )
	{
		// launch thread to detect connection (and if found, runs code below)
		app->pAdvertThread = new CAdvertThread;
		app->pAdvertThread->Start();

		// called later to navigate and place the htmlview
		//app->pAdvertView->Navigate(_T("http://advert.thegamecreators.com/adCampaigns.php?width=728&height=90"));
		//app->pAdvertWnd->SetWindowPos ( 0, 140, 450, 728, 90, SWP_SHOWWINDOW );
	}

	// refresh entire window
	Invalidate(1);
	UpdateWindow();

	// finished main window creation
	return 0;
}

LPCWSTR MainFrame::s_winClassName = NULL; 

static LPCWSTR RegisterSimilarClass(LPCWSTR lpszNewClassName, LPCWSTR lpszOldClassName, UINT nIDResource)
{
	// Get class info for old class.
	HINSTANCE hInst = AfxGetInstanceHandle();
	WNDCLASS wc;
	if (!::GetClassInfo(hInst, lpszOldClassName, &wc)) {
		TRACE("Can't find window class %s\n", lpszOldClassName);
		return NULL;
	}

	// Register new class with same info, but different name and icon.
	wc.lpszClassName = lpszNewClassName;
	wc.hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(nIDResource));
	if (!AfxRegisterClass(&wc)) {
		TRACE("Unable to register window class%s\n", lpszNewClassName);
		return NULL;
	}
	return lpszNewClassName;
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{	
	if( !CBCGPMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	if (s_winClassName == NULL) 
	{
		// One-time initialization: register the class
		s_winClassName = RegisterSimilarClass(_T("TDBPROEDITOR"), cs.lpszClass, IDR_MAINFRAME);
		if (!s_winClassName)
		{
		  return FALSE;
		}
	}

	cs.lpszClass = s_winClassName;

	return TRUE;
}

// MainFrame diagnostics
#ifdef _DEBUG
void MainFrame::AssertValid() const
{
	CBCGPMDIFrameWnd::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

// MainFrame message handlers
void MainFrame::OnViewCustomize()
{
	CBCGPToolbarCustomize* pDlgCust = new CBCGPToolbarCustomize (this, TRUE, (BCGCUSTOMIZE_MENU_SHADOWS | BCGCUSTOMIZE_TEXT_LABELS | BCGCUSTOMIZE_LOOK_2000 | BCGCUSTOMIZE_MENU_ANIMATIONS | BCGCUSTOMIZE_NOHELP));
	pDlgCust->EnableUserDefinedToolbars ();
	pDlgCust->Create ();
}

afx_msg LRESULT MainFrame::OnToolbarReset(WPARAM wp,LPARAM)
{
	UINT uiToolBarId = (UINT) wp;
	if (uiToolBarId == IDR_MAINFRAME)
	{
		// Replace new button by the menu
		CMenu menu;
		VERIFY (menu.LoadMenu (IDR_MAINFRAME));

		CMenu* pMenuFile = menu.GetSubMenu (0);
		VERIFY (pMenuFile != NULL);

		CMenu* pMenuNew = pMenuFile->GetSubMenu (0);
		VERIFY (pMenuNew != NULL);

		CBCGPToolbarMenuButton btnNew (ID_FILE_NEW, pMenuNew->GetSafeHmenu (), CImageHash::GetImageOfCommand (ID_FILE_NEW));
		m_wndToolBar.ReplaceButton (ID_FILE_NEW, btnNew);		
	}
	return 0;
}

void MainFrame::OnWindowManager() 
{
	ShowWindowsDialog ();
}

void MainFrame::OnAppLook(UINT id)
{
	CBCGPDockManager::SetDockMode (DT_SMART);

	m_nAppLook = id;

	CBCGPVisualManager2007::SetCustomColor ((COLORREF)-1);

	CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D;

	CBCGPMDITabParams mdiTabParams;
	mdiTabParams.m_bTabIcons = FALSE;
	mdiTabParams.m_bEnableTabSwap = TRUE;

	switch (m_nAppLook)
	{
	case ID_VIEW_APPLOOK_2000:
		// enable Office 2000 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager));
		break;

	case ID_VIEW_APPLOOK_XP:
		// enable Office XP look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		// enable Windows XP look (in other OS Office XP look will be used):
		CBCGPWinXPVisualManager::m_b3DTabsXPTheme = TRUE;
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPWinXPVisualManager));
		break;

	case ID_VIEW_APPLOOK_2003:
		// enable Office 2003 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));

		mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
		mdiTabParams.m_bAutoColor = TRUE;
		CBCGPDockManager::SetDockMode (DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2005:
		// enable VS.NET 2005 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2005));

		mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
		mdiTabParams.m_bDocumentMenu = TRUE;
		CBCGPDockManager::SetDockMode (DT_SMART);
		break;
	case ID_VIEW_APPLOOK_2007:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Aqua);

		mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
		mdiTabParams.m_bDocumentMenu = TRUE;
		CBCGPDockManager::SetDockMode (DT_SMART);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		break;	
	case ID_VIEW_APPLOOK_2007_1:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);

		mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
		mdiTabParams.m_bDocumentMenu = TRUE;
		CBCGPDockManager::SetDockMode (DT_SMART);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		break;	
	case ID_VIEW_APPLOOK_2007_2:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);

		mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
		mdiTabParams.m_bDocumentMenu = TRUE;
		CBCGPDockManager::SetDockMode (DT_SMART);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		break;	
	case ID_VIEW_APPLOOK_2007_3:
		// enable Office 2007 look:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);

		mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
		mdiTabParams.m_bDocumentMenu = TRUE;
		CBCGPDockManager::SetDockMode (DT_SMART);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		break;	
	case ID_VIEW_APPLOOK_2007_4:
		CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);
		CBCGPVisualManager2007::SetCustomColor ((COLORREF)theApp.GetInt (_T("LookColor"), (COLORREF)-1));
		
		mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
		mdiTabParams.m_bDocumentMenu = TRUE;
		CBCGPDockManager::SetDockMode (DT_SMART);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		break;
	case ID_VIEW_APPLOOK_VS2008:
		mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_VS2005;
		mdiTabParams.m_bDocumentMenu = TRUE;
		CBCGPDockManager::SetDockMode (DT_SMART);
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2008));
		break;
	}	

	mdiTabParams.m_bActiveTabCloseButton = TRUE; // John

	EnableMDITabbedGroups (TRUE, mdiTabParams);

	CBCGPDockManager* pDockManager = GetDockManager ();
	if (pDockManager != NULL)
	{
		ASSERT_VALID (pDockManager);
		pDockManager->AdjustBarFrames ();
	}

	CBCGPTabbedControlBar::ResetTabs ();

	RecalcLayout ();
	RedrawWindow (NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

	theApp.WriteInt (_T("ApplicationLook"), m_nAppLook);
}

void MainFrame::OnUpdateAppLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio (m_nAppLook == pCmdUI->m_nID);
}

BOOL MainFrame::OnShowMDITabContextMenu (CPoint point, DWORD dwAllowedItems, BOOL bDrop)
{
	CMenu menu;
	VERIFY(menu.LoadMenu (bDrop ? IDR_POPUP_DROP_MDITABS : IDR_POPUP_MDITABS));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	if ((dwAllowedItems & BCGP_MDI_CREATE_HORZ_GROUP) == 0)
	{
		pPopup->DeleteMenu (ID_MDI_NEW_HORZ_TAB_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & BCGP_MDI_CREATE_VERT_GROUP) == 0)
	{
		pPopup->DeleteMenu (ID_MDI_NEW_VERT_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & BCGP_MDI_CAN_MOVE_NEXT) == 0)
	{
		pPopup->DeleteMenu (ID_MDI_MOVE_TO_NEXT_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & BCGP_MDI_CAN_MOVE_PREV) == 0)
	{
		pPopup->DeleteMenu (ID_MDI_MOVE_TO_PREV_GROUP, MF_BYCOMMAND);
	}

	CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;
	pPopupMenu->SetAutoDestroy (FALSE);
	pPopupMenu->Create (this, point.x, point.y, pPopup->GetSafeHmenu ());

	return TRUE;
}

void MainFrame::OnMdiMoveToNextGroup() 
{
	MDITabMoveToNextGroup ();
}

void MainFrame::OnMdiMoveToPrevGroup() 
{
	MDITabMoveToNextGroup (FALSE);
}

void MainFrame::OnMdiNewHorzTabGroup() 
{
	MDITabNewGroup (FALSE);
}

void MainFrame::OnMdiNewVertGroup() 
{
	MDITabNewGroup ();
}

void MainFrame::OnMdiCancel() 
{}

CBCGPMDIChildWnd* MainFrame::CreateDocumentWindow (LPCTSTR lpcszDocName, CObject* /*pObj*/)
{
	if (lpcszDocName != NULL && lpcszDocName [0] != '\0')
	{
		CDocument* pDoc = AfxGetApp()->OpenDocumentFile (lpcszDocName);

		if (pDoc != NULL)
		{
			POSITION pos = pDoc->GetFirstViewPosition();

			if (pos != NULL)
			{
				CView* pView = pDoc->GetNextView (pos);
				if (pView == NULL)
				{
					return NULL;
				}

				return DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, pView->GetParent ());
			}   
		}
	}

	return NULL;
}

void MainFrame::OnClose() 
{
	if(Variables::m_ProjectChanged)
	{
		int res = AfxMessageBox(_T("The project has been modified. Do you wish to save changes?"), MB_YESNOCANCEL);
		if(res == IDYES)
		{
			OnSaveProject();    
		}
		else if(res == IDCANCEL)
		{
			return;    
		}
	}	
	KillTimer(Variables::RunBalloon); // Force end of bubble timer if enabled
	KillTimer(Variables::FileChangeChecker); // Turn off the file checker
	SemanticParser::kill = true; // Kill the thread
	SemanticParser::CleanUp(false); // Empty the semantic vectors
	SaveMDIState (theApp.GetRegSectionPath ());
	CBCGPMDIFrameWnd::OnClose();
}

// ********************************************************************************************
// John
// ********************************************************************************************

void MainFrame::CleanUp()
{
	ClearInclude(); // Clear the include tree
	ClearMedia(); // Clear the media tree
	ClearToDo(); // Clear the todo list
	ClearCursors(); // Clear the cursor list
	ClearComments(); // Clear the comments list
	Variables::Init(); // Clear the variables
	m_Properties.UpdateFromLibrary(); // Reset the property window from cleared variables
}

void MainFrame::OnNewFile()
{
	CFileDialog FileDlg(FALSE, _T(".dba"), NULL, 0, _T("DarkBASIC Professional Source Files (*.dba)|*.dba||"));
	FileDlg.GetOFN().lpstrInitialDir = m_DefaultPath;

	if( FileDlg.DoModal() != IDOK )
	{
		return;
	}	

	CString file = FileDlg.GetPathName();

	CStdioFile wfile;
	BOOL openWrite = wfile.Open(file,CFile::modeWrite | CFile::modeCreate |CFile::modeNoTruncate);
	if(openWrite)
	{
		wfile.WriteString(_T("Rem ***** Included Source File *****\n"));
		wfile.Flush();
		wfile.Close();
		AfxGetApp()->OpenDocumentFile(file);
	}
	else
	{
		AfxMessageBox(_T("DarkBASIC Professional Editor could not create the file ") + file + _T(" for writing.\n\nIf you are using Windows Vista/7 then you may need to run DarkBASIC Professional Editor with administrative permissions"), MB_ICONEXCLAMATION | MB_OK);
	}
}

void MainFrame::OnNewProject()
{
	NewProjectDialog newDlg;
	App* app = (App*)AfxGetApp();

	bool useNewProject = !app->stopNewDialog;
	
	if(app->stopNewDialog)
	{
		app->stopNewDialog = false;
	}
	else
	{
		newDlg.DoModal();
		if(newDlg.projectName == _T("Cancel"))
		{
			return;
		}
	}

	if(Variables::m_ProjectChanged)
	{
		if(AfxMessageBox(_T("Do you wish to save the current project?"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			OnSaveProject();
		}
	}
	
	app->CloseAllDocuments(FALSE); // Close all of the documents
	CleanUp(); // Do this after the closing the documents, as the documents might want to access some data

	if(useNewProject && newDlg.projectName != _T(""))
	{
		Variables::m_ProjectName = newDlg.projectName;
		Variables::m_ProjectPath = newDlg.projectPath;
		m_DefaultPath = newDlg.projectPath;
		Variables::m_ProjectFile = newDlg.projectPath + newDlg.projectName + _T(".dbpro");
		Variables::m_DBPropertyExe = newDlg.projectName + _T(".exe");
		Variables::m_DBPropertyTitle = newDlg.projectName;
		CString newFile;
		if(newDlg.templateName != _T("Empty Project"))
		{
			newFile = Variables::m_ProjectPath + newDlg.templateName;
			CopyFile(newDlg.templateName, newFile, FALSE);			
		}
		else
		{
			newFile = Variables::m_ProjectPath + Variables::m_ProjectName + _T(".dba");
			CStdioFile wfile;
			BOOL openWrite = wfile.Open(newFile,CFile::modeWrite | CFile::modeCreate|CFile::modeNoTruncate);
			if(openWrite)
			{
				wfile.WriteString(_T("Rem Project: ") + Variables::m_ProjectName + _T("\n"));
				CTime today = CTime::GetCurrentTime(); 
				wfile.WriteString(_T("Rem Created: ") + today.Format(_T("%A, %B %d, %Y")) + _T("\n"));
				wfile.WriteString(_T("\nRem ***** Main Source File *****\n"));
				wfile.Flush();
				wfile.Close();
			}
		}	
		// Add Sticky Includes
		if(newDlg.stickyIncludes)
		{
			int curPos = 0;
			CString resToken= Settings::StickyIncludes.Tokenize(_T("#"),curPos);
			while (resToken != "")
			{
				m_SolutionExplorer.AddNewFile(resToken);
				Variables::m_IncludeFiles.push_back(new SourceFile(resToken, 0));
				resToken= Settings::StickyIncludes.Tokenize(_T("#"),curPos);
			}
		}
		app->OpenDocumentFile(newFile);
		WriteProject::Write(false);
		m_Media.UpdatePath(Variables::m_ProjectPath);
		m_SolutionExplorer.UpdatePath(Variables::m_ProjectPath);
	}
	else
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
	}
}

void MainFrame::OnOpenAny()
{
	CFileDialog FileDlg(TRUE, _T(".dbpro"), NULL, 0, _T("Supported Files|*.dbpro;*.dba|DarkBASIC Professional Editor Project Files (*.dbpro)|*.dbpro|DarkBASIC Professional Editor Source Files (*.dba)|*.dba||"));
	FileDlg.GetOFN().lpstrInitialDir = m_DefaultPath;

	if( FileDlg.DoModal() == IDOK )
	{
		CString filename = FileDlg.GetPathName();
		filename.MakeUpper();
		if(filename.Right(5) == _T("DBPRO"))
		{
			OnOpenProject(FileDlg.GetPathName());
		}
		else
		{
			App* app = (App*)AfxGetApp();
			app->OpenDocumentFile(FileDlg.GetPathName());
		}
	}
}

void MainFrame::OnOpenProject(CString filename)
{
	App* pApp = (App*)AfxGetApp();
	ASSERT_VALID(pApp);
	// Clean up and load
	m_DefaultPath = Utilities::ExtractPath(filename);
	CleanUp();	
	// UAC Check
	if(!Utilities::CheckAccess(filename, Utilities::O_RDWR))
	{
		int res = AfxMessageBox(L"The project you want to open is located in a directory that DarkBASIC Professional Editor does not have permission to work in. You will not be able to save work or compile unless you run DarkBASIC Professional Editor under the Administrator account by using the User Account Control feature of your operating system (you may be prompted for the Administrator password).\n\nDo you wish to restart DarkBASIC Professional Editor under the Administrator account?", MB_ICONEXCLAMATION | MB_YESNOCANCEL);
		if(res == IDYES)
		{
			CString appPath = Utilities::GetApplicationPath();
			CString newFile = L"\"" + filename + L"\"";

			AfxGetMainWnd()->SendMessage(WM_CLOSE);	
			::ShellExecute(0, // owner window
				   L"runas",
				   appPath,
				   newFile, // params
				   0, // directory
				   SW_SHOWNORMAL);
			return;
		}
		else if(res == IDCANCEL)
		{
			return;
		}
	}
	// End
	if(Settings::OnStartup != 4)
	{
		Settings::LastProject = filename;
	}
	ReadProject::Read(filename);
	m_Media.UpdatePath(m_DefaultPath);
	m_SolutionExplorer.UpdatePath(m_DefaultPath);
	Variables::m_ProjectChanged = false;
	pApp->AddToRecentFileList(filename);
}

void MainFrame::OnOpenProject()
{
	CFileDialog FileDlg(TRUE, _T(".dbpro"), NULL, 0, _T("DarkBASIC Professional Editor Project Files (*.dbpro)|*.dbpro||"));
	FileDlg.GetOFN().lpstrInitialDir = m_DefaultPath;

	if( FileDlg.DoModal() == IDOK )
	{
		OnOpenProject(FileDlg.GetPathName());
	}
}

void MainFrame::OnSaveProject()
{
	if(Variables::m_ProjectFile == _T(""))
	{
		OnSaveProjectAs();
		return;
	}

	WriteProject::Write(false);
	Variables::m_ProjectChanged = false;
}

void MainFrame::OnSaveProjectAs()
{
	CFileDialog FileDlg(FALSE, _T(".dbpro"), NULL, 0, _T("DarkBASIC Professional Editor Project Files (*.dbpro)|*.dbpro||"));
	FileDlg.GetOFN().lpstrInitialDir = m_DefaultPath;

	if( FileDlg.DoModal() != IDOK )
	{
		return;
	}

	if(Settings::OnStartup != 4)
	{
		Settings::LastProject = FileDlg.GetPathName();
	}

	Variables::m_ProjectName = Utilities::RemoveExtension(Utilities::ExtractFilename(FileDlg.GetPathName()));
	Variables::m_ProjectPath = Utilities::ExtractPath(FileDlg.GetPathName());
	Variables::m_ProjectFile = FileDlg.GetPathName(); 

	m_DefaultPath = Utilities::ExtractPath(FileDlg.GetPathName());
	m_Media.UpdatePath(m_DefaultPath);
	m_SolutionExplorer.UpdatePath(m_DefaultPath); // Requires projectpath to be set

	WriteProject::Write(false);
	Variables::m_ProjectChanged = false;
}

void MainFrame::OnSaveAll()
{
	WriteFiles::Write();
	MainFrame::OnSaveProject();
}

void MainFrame::OnUpdateCompile(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!CompileSupport::m_IsCompiling && Variables::m_IncludeFiles.size() > 0);
}

void MainFrame::OnCompile()
{	
	if(CompileSupport::m_strHelpFile != "")
	{
		CompileSupport::CompileHelp(false);
	}
	else
	{
		CompileSupport::Compile(false);
	}
}

void MainFrame::OnCompileRun()
{
	if(CompileSupport::m_strHelpFile != "")
	{
		CompileSupport::CompileHelp(true);
	}
	else
	{
		CompileSupport::Compile(true);
	}
}

void MainFrame::OnDebug()
{
	Variables::m_SavingBecauseDebug = true;
	CompileSupport::Compile(false);
	Variables::m_SavingBecauseDebug = false;
}

void MainFrame::OnStep()
{
	Variables::m_SavingBecauseDebug = true;
	Variables::m_SavingBecauseStep = true;
	CompileSupport::Compile(false);
	Variables::m_SavingBecauseStep = false;
	Variables::m_SavingBecauseDebug = false;
}

LRESULT MainFrame::OnUser(WPARAM a, LPARAM b)
{	
	HANDLE fileMap = OpenFileMapping(0x0004, true, _T("DBPROEDITORMESSAGE"));
	if(fileMap != NULL)
	{	
		char* fileView = (char*)MapViewOfFile(fileMap, SECTION_MAP_READ, 0, 0, 256);
		if(fileView != NULL)
		{
			CString line(fileView);		
			UnmapViewOfFile(fileView);			

			int pos = -1;
			line = line.Left(line.GetLength() -1);
			for(int i=line.GetLength(); i > 0; i--)
			{
				if(line.GetAt(i) == ' ')
				{
					pos = i+1;
					break;
				}	
			}
			if(pos > -1)
			{
				int badLine = _ttoi(line.Mid(pos));		

				GotoLine(badLine);
			}
		}
		CloseHandle(fileMap);
	}
	return TRUE;
}

void MainFrame::OnRunPrevious()
{
	if(Utilities::CheckFileExists(Variables::m_DBPropertyExe))
	{
		::ShellExecute (NULL, NULL, Variables::m_DBPropertyExe, NULL, NULL, NULL);
	} 
}

void MainFrame::OnUpdateRunPrevious(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Utilities::CheckFileExists(Variables::m_DBPropertyExe) && !CompileSupport::m_IsCompiling);
}

LRESULT MainFrame::SetOutputMessage(WPARAM a, LPARAM b)
{
	TCHAR* chr = (TCHAR*)a;
	CString text(chr);
	AddOutputText(text);
	delete [] chr;
	return TRUE;
}

void MainFrame::AddOutputText(CString text)
{
	if(text == _T("Compile Successful"))
	{
		HideOutput();
	}
	m_wndOutput.AddOutputText(text);
}

void MainFrame::ClearOutputText()
{
	m_wndOutput.OnClear();
}

void MainFrame::AddNewFile(CString text)
{
	m_SolutionExplorer.AddNewFile(text);
}

void MainFrame::DeleteOldFile(CString text)
{
	m_SolutionExplorer.DeleteOldFile(text);
}

void MainFrame::ClearInclude()
{
	m_SolutionExplorer.Clear();
}

void MainFrame::AddNewMediaFile(CString text)
{
	m_Media.AddNewFile(text);
}

void MainFrame::ClearMedia()
{
	m_Media.Clear();
}

void MainFrame::AddNewCursorFile(CString text)
{
	m_Cursor.AddNewFile(text);
}

void MainFrame::ClearCursors()
{
	m_Cursor.Clear();
}

void MainFrame::AddToDo(CString text, BOOL checked)
{
	m_ToDo.AddItem(text, checked);
}

void MainFrame::ClearToDo()
{
	m_ToDo.Clear();
}

LRESULT MainFrame::SetStatusMessage(WPARAM a, LPARAM b)
{
	SetStatus((int)a);
	return TRUE;
}

void MainFrame::SetStatus(int status)
{
	m_wndOutput.SetStatus(status);
}

LRESULT MainFrame::SetStatusMaxMessage(WPARAM a, LPARAM b)
{
	SetStatusMax((int)a);
	return TRUE;
}

void MainFrame::SetStatusMax(int max)
{
	m_wndOutput.SetStatusMax(max);
}

void MainFrame::ShowOutput()
{
	if(Settings::ShowOutputWindow)
	{
		if(!m_wndOutput.IsVisible())
		{
			outputNeedsCollapse=true;
			m_wndOutput.ShowControlBar(TRUE, FALSE, TRUE);
		}
	}
}

void MainFrame::HideOutput()
{
	if(Settings::ShowOutputWindow && outputNeedsCollapse)
	{
		outputNeedsCollapse=false;
		m_wndOutput.Slide(FALSE, TRUE);
	}
}

void MainFrame::SetStatusText(CString text)
{
	m_wndStatusBar.SetPaneText(0, text, TRUE);
	m_wndStatusBar.InvalidatePaneContent(0);
}

void MainFrame::UpdateFromLibrary()
{
	m_Properties.UpdateFromLibrary();
	m_SolutionExplorer.Expand();
}

void MainFrame::OnRebuild()
{
	Keywords::RebuildKeywords();
}

void MainFrame::OnFullScreen()
{
	ShowFullScreen();
}

void MainFrame::OnKeyboard()
{
	CBCGPKeyMapDlg dlg (this, TRUE /* Enable Print */);
	dlg.DoModal ();
}

void MainFrame::OnCustomise()
{
	CBCGPToolbarCustomize* pDlgCust = new CBCGPToolbarCustomize (this,
		TRUE /* Automatic menus scaning */,
		BCGCUSTOMIZE_MENU_SHADOWS | BCGCUSTOMIZE_TEXT_LABELS | 
		BCGCUSTOMIZE_LOOK_2000 | BCGCUSTOMIZE_MENU_ANIMATIONS);

	pDlgCust->EnableUserDefinedToolbars ();
	pDlgCust->Create ();
}

void MainFrame::OnOptions()
{
	COptionsDlg dlgOptions (_T("Options"), this);
	dlgOptions.DoModal ();
}

void MainFrame::OnKeyHelper()
{
	CRect rect(0, 0,500,75);
	KeystateWindow* ks = new KeystateWindow();
	ks->Create(_T("Keystate_Tool"), _T("Keystate Tool"), WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, rect);
	ks->ShowWindow(SW_SHOW);
	ks->UpdateWindow();
}

void MainFrame::OnDBPAbout()
{
	CString location = Settings::DBPLocation + _T("\\Editor\\dbp_config.dll");

	CFileStatus fileStat;
	if(!CFile::GetStatus(location,fileStat))
	{
		AfxMessageBox(_T("Could not locate DBP status dll"));
		return;
	} 

   /* get handle to dll */
   HINSTANCE hGetProcIDDLL = LoadLibrary(location);

   /* get pointer to the function in the dll*/
   FARPROC lpfnGetProcessID = GetProcAddress(HMODULE(hGetProcIDDLL), "on_about");

   typedef void (__stdcall * pICFUNC)(void);

   pICFUNC MyFunction = pICFUNC(lpfnGetProcessID);

   /* The actual call to the function contained in the dll */
   MyFunction();

   /* Release the Dll */
   FreeLibrary(hGetProcIDDLL);
}

void MainFrame::OnDGS()
{
	// Launch DGS website
	CString link(_T("http://www.darkgamestudio.com"));
	ShellExecuteW(NULL, NULL, link, NULL, NULL, SW_NORMAL);
}

void MainFrame::GoURL(CString url)
{
	App* app = (App*)AfxGetApp();

	if(Settings::OpenHelpInNewTab)
	{
		app->pHelpTemplate->OpenDocumentFile(url);
	}
	else
	{
		if(app->pHelpTemplate->GetFirstDocPosition() == NULL)
		{
			app->pHelpTemplate->OpenDocumentFile(url);
		}
		else
		{
			POSITION pos =  app->pHelpTemplate->GetFirstDocPosition();
			HTMLDocument* doc = DYNAMIC_DOWNCAST (HTMLDocument, app->pHelpTemplate->GetNextDoc(pos));
			doc->OnOpenDocument(url);
		}
	}	
}

void MainFrame::OnTimer( UINT id )
{
	App* app = (App*)AfxGetApp();
	POSITION posDocument = app->pDocTemplate->GetFirstDocPosition();

	// U75 - 271009 - trigger a refresh if advertising enabled for first time (thread can't do it)
	if ( g_bUsingAdvertising==true && g_lUsingAdvertisingTimeStamp==0 )
	{
		Invalidate();
		UpdateWindow();
	}

	if(id == Variables::SemanticParser && Settings::AllowCodeView)
	{		
		SemanticParserManager::Start();
	}
	else if(id == Variables::FileChangeChecker)
	{
		// Need to check each document to ensure they have the latest file version open
		while(posDocument != NULL)
		{
			Doc* pDoc = (Doc*)app->pDocTemplate->GetNextDoc(posDocument);
			pDoc->CheckFileStatus();
		}
	}
	else if(id == Variables::RunBalloon)
	{
		KillTimer(Variables::RunBalloon);
		::SendMessage (hWndToolTip, TTM_TRACKACTIVATE, FALSE, 0);
	}
}

void MainFrame::OnTutorial()
{
	// Launch link to video tutorial
	CString link(_T("http://developer.thegamecreators.com/?f=dbpro_tutorials"));
	ShellExecuteW(NULL, NULL, link, NULL, NULL, SW_NORMAL);
}

void MainFrame::OnTutorialUpdate(CCmdUI* pCmdUI)
{
	// tutorial available?
	pCmdUI->Enable(1);
}

void MainFrame::OnUpdate()
{
	UpdateTool up(this);
	up.DoModal();
}

void MainFrame::OnCheckModules()
{
	// LEE - U75 - 051009 - using DBPLocation instead of extacting from apppath
	CString appPath = Utilities::GetApplicationPath();
	char pANSI[512];
	BOOL b = FALSE;
	WideCharToMultiByte ( CP_ACP, WC_NO_BEST_FIT_CHARS, Settings::DBPLocation, -1, pANSI, 512, "_", &b);
	strcat ( pANSI, "Compiler\\" );
	ShellExecuteA ( NULL, "open", "TGCCertificateViewerKEEPACTIVE.exe", "", pANSI, SW_SHOWDEFAULT );
}

void MainFrame::OnOpenGameCreatorStore()
{
	HINSTANCE StoreHandle = LoadLibrary(_T("GameCreatorStore.dll"));
	if ( !StoreHandle ) 
	{
		MessageBox( _T("Failed to load Store DLL"), _T("Error") );
		return;
	}

	void (*SetDownloadMode)(int) = ( void (*)(int) ) GetProcAddress( StoreHandle, "?SetDownloadMode@@YAXH@Z" );
	if ( !SetDownloadMode )
	{
		MessageBox( _T("Invalid Function Pointer: SetDownloadMode"), _T("Error") );
		return;
	}

	// 3=extract for DBPro
	SetDownloadMode( 3 );

    void (*OpenStore)(char*,int,int,int,int) = ( void (*)(char*,int,int,int,int) ) GetProcAddress( StoreHandle, "?OpenStore@@YAXPADHHHH@Z" );
	if ( !OpenStore )
	{
		MessageBox( _T("Invalid Function Pointer: OpenStore"), _T("Error") );
		return;
	}

	//Parameters: Download Folder, posX, posY, sizeX, sizeY
	//folder must exist, highly recommend an absolute path
	//window size defaults to 1100x800 centered if the resolution supports it
	//otherwise the store will be maximised.

	CString appPath = Utilities::GetApplicationPath();
	// returns Editor\Editor.exe - need to trim it down to create the Media folder we need for Game Creator Store
	int nPos = appPath.GetLength()-17;
	CString rootPath = appPath.Left(nPos) + _T("Media Store");
	if ( Utilities::CheckPathExists(rootPath)==false ) CreateDirectory ( rootPath, NULL );

	char pANSI[512];
	BOOL b = FALSE;
	WideCharToMultiByte ( CP_ACP, WC_NO_BEST_FIT_CHARS, rootPath.GetString(), -1, pANSI, 512, "_", &b);
	OpenStore( pANSI, -1,-1, -1,-1 );

    FreeLibrary( StoreHandle );
}

void MainFrame::OnCommandArgs()
{
	InputDialog dlg(_T("Enter the application command line arguments"), Variables::m_CommandLineArgs);
	dlg.DoModal();
	Variables::m_CommandLineArgs = dlg.GetText();
}

void MainFrame::OnFindInFiles()
{
	FindInProjectDialog* fpd = new FindInProjectDialog(this);
	fpd->Create(IDD_FIND_IN_FILES, this);
	fpd->ShowWindow(SW_SHOW);
}

void MainFrame::OnGotoLine()
{
	if(Variables::Lines.size() == 0)
	{
		AfxMessageBox(_T("The line count information is not available, please compile your project first"), MB_OK | MB_ICONINFORMATION);
		return;
	}
	CGotoLineDlg dlg (this);
	if (dlg.DoModal () != IDOK)
	{
		return;
	}
	GotoLine (dlg.m_nLineNumber);
}

void MainFrame::GotoLine(int badLine)
{
	int total = 0;
	int previousTotal = 0;
	bool found = false;
	for(UINT a=0; a < Variables::Lines.size(); a++)
	{							
		total += Variables::Lines.at(a);
		if(badLine <= total)
		{
			int probLine = badLine - previousTotal;
			CString file = Variables::m_IncludeFiles.at(a)->filename;
			App* app = (App*)AfxGetApp();

			POSITION posDocument = app->pDocTemplate->GetFirstDocPosition();

			while(posDocument != NULL)
			{
				Doc* pDoc = (Doc*)app->pDocTemplate->GetNextDoc(posDocument);
				CString title = Utilities::RemoveModifiedFlag(pDoc->GetTitle());
				CString upTitle = title;
				upTitle.MakeUpper();
				if(upTitle.Right(3) != _T("DBA"))
				{
					title.Append(_T(".dba"));
				}
				if(title == Utilities::ExtractFilename(file))
				{
					POSITION posView = pDoc->GetFirstViewPosition ();
					View* pView = DYNAMIC_DOWNCAST (View, pDoc->GetNextView (posView));
					if (pView != NULL)
					{
						this->MDIActivate(pView->GetParent());
					}
					found = true;
					pDoc->HighlightLine(probLine - 1);
					break;
				}
			}
			if(!found)
			{						
				Doc* pDoc = (Doc*)AfxGetApp()->OpenDocumentFile(file);
				
				CString title = Utilities::RemoveModifiedFlag(pDoc->GetTitle());
				CString upTitle = title;
				upTitle.MakeUpper();
				if(upTitle.Right(3) != _T("DBA"))
				{
					title.Append(_T(".dba"));
				}
				if(title == Utilities::ExtractFilename(file))
				{
					pDoc->HighlightLine(probLine - 1);
				}
			}
			break;
		}
		previousTotal += Variables::Lines.at(a);
	}
}

void MainFrame::GoToViewLine(CString file, int line)
{
	App* app = (App*)AfxGetApp();
	bool found = false;

	POSITION posDocument = app->pDocTemplate->GetFirstDocPosition();

	while(posDocument != NULL)
	{
		Doc* pDoc = (Doc*)app->pDocTemplate->GetNextDoc(posDocument);
		CString title = Utilities::RemoveModifiedFlag(pDoc->GetTitle());
		CString upTitle = title;
		upTitle.MakeUpper();
		if(upTitle.Right(3) != _T("DBA"))
		{
			title.Append(_T(".dba"));
		}
		if(title == Utilities::ExtractFilename(file))
		{
			POSITION posView = pDoc->GetFirstViewPosition ();
			View* pView = DYNAMIC_DOWNCAST (View, pDoc->GetNextView (posView));
			if (pView != NULL)
			{
				this->MDIActivate(pView->GetParent());
			}
			found = true;
			pDoc->HighlightLine(line);
			break;
		}
	}
	if(!found)
	{				
		if(file.Mid(1,1) != _T(":")) // Full path
		{
			file = Variables::m_ProjectPath + file;
		}
		Doc* pDoc = (Doc*)app->OpenDocumentFile(file);

		CString title = Utilities::RemoveModifiedFlag(pDoc->GetTitle());
		CString upTitle = title;
		upTitle.MakeUpper();
		if(upTitle.Right(3) != _T("DBA"))
		{
			title.Append(_T(".dba"));
		}
		if(title == Utilities::ExtractFilename(file))
		{
			pDoc->HighlightLine(line);
		}
	}
}

/*
* Changes the build type to media
* Used by the media window to ensure the user doesn't miss this step out
*/
void MainFrame::SetBuildTypeToMedia()
{
	m_Properties.SetBuildTypeToMedia();
}

/*
* Removes the comments from the to do window
*/
void MainFrame::ClearComments()
{
	m_ToDo.ClearComments();
}

/*
* Adds a new comment line to the to do window
*/
void MainFrame::AddComment(CString line)
{
	m_ToDo.AddComment(line);
}

/*
* Writes the comments from the to do window in to vectors for saving
*/
void MainFrame::DumpComments()
{
	m_ToDo.DumpComments();
}

/*
* Associates DBP files with DarkBASIC Professional Editor
*/
void MainFrame::AssociateFiles()
{
	HKEY   hkey;
	DWORD  dwDisposition;

	// HKEY_CLASSES_ROOT\.dba = dba_auto_file
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, _T(".dba"), 0, NULL, 0, 0, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS)
	{
		RegSetValueEx(hkey, NULL, 0, REG_SZ, (BYTE*)_T("dba_auto_file"), 14);
		RegCloseKey(hkey);
	}

	// HKEY_CLASSES_ROOT\.dbpro = dba_auto_file
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, _T(".dbpro"), 0, NULL, 0, 0, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS)
	{
		RegSetValueEx(hkey, NULL, 0, REG_SZ, (BYTE*)_T("dba_auto_file"), 14);
		RegCloseKey(hkey);
	}

	// HKEY_CLASSES_ROOT\dba_auto_file\shell\open\command = "E:\Source\DarkBASIC Professional Editor\debug\DarkBASIC Professional Editor.exe" "%1"
	if(RegCreateKeyEx(HKEY_CLASSES_ROOT, _T("dba_auto_file\\shell\\open\\command"), 0, NULL, 0, 0, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS)
	{
		CString val = _T("\"") + Utilities::GetApplicationPath() + _T("\" \"%1\"");
		RegSetValueEx(hkey, NULL, 0, REG_SZ, (BYTE*)val.GetBuffer(), val.GetLength());
		RegCloseKey(hkey);
	}
}

void MainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	if(bActive)
	{
		SemanticParserManager::ThreadResume();
	}
	else
	{
		SemanticParserManager::ThreadPause();
	}
}

void MainFrame::MakeToolTip()
{
	CBCGPToolbarButton *pBut = m_wndToolBar.GetButton(13);
	if(pBut == NULL)
	{
		return;
	}
	WINDOWPLACEMENT place;
	memset (&place, 0, sizeof (place)); // mama says: reset your structure before touching it.
	place.length = sizeof (place);
	::GetWindowPlacement(m_wndToolBar.GetSafeHwnd(), &place);
	
	RECT rt = place.rcNormalPosition;
	CRect  pt;
	pt.left = rt.left;
	pt.top = rt.top;
	ClientToScreen(pt);
	
	CPoint pos;
	pos.x = pt.left + pBut->Rect().left;
	pos.y = pt.top + pBut->Rect().Height();

	CreateToolTip(NULL, pos, 1, L"Run Example", L"You can run this source code and any other by clicking here when an example is open", true);
	SetTimer(Variables::RunBalloon, 4000, 0);
}

void MainFrame::CreateToolTip (HWND hWndParent, CPoint pos, long icon_type, const TCHAR *title, const TCHAR *text, bool is_balloon)
{
	// this function creates a tooltip	
	TOOLINFO ti;
	memset (&ti, 0, sizeof (ti)); // mama says: reset your structure before touching it.
	ti.cbSize = sizeof (ti);
	ti.uFlags = TTF_TRACK;
	ti.lpszText = (TCHAR *) text;
	ti.hwnd = hWndParent;
	
	// is a tooltip already displayed ?
	if (hWndToolTip != NULL)
	{		
		::SendMessage (hWndToolTip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &ti);
		::SendMessage (hWndToolTip, TTM_TRACKPOSITION, 0, (LPARAM) MAKELONG (pos.x, pos.y));
	}
	else
	{
		// prepare the tooltip window style
		long window_style = WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_CLOSE;

		if (is_balloon)
		{
			window_style |= TTS_BALLOON; // add the balloon flag if we want a balloon tooltip
		}

		// create a tooltip window
		hWndToolTip = CreateWindow (TOOLTIPS_CLASS, // common dialog style tooltip
			NULL, // title bar
			window_style, // window style flags
			CW_USEDEFAULT, CW_USEDEFAULT, // left/top positions
			CW_USEDEFAULT, CW_USEDEFAULT, // width/height of window
			NULL, // the parent window
			NULL, // no menu
			NULL, // window handle
			NULL); // no extra parameters

		if (hWndToolTip != NULL)
		{
			//ti.uFlags = TTF_SUBCLASS|TTF_IDISHWND|TTF_TRACK;
			::GetClientRect (hWndParent, &ti.rect);

			// send an addtool message to the tooltip control window
			::SendMessage (hWndToolTip, TTM_ADDTOOL, 0, (LPARAM) &ti);

			// set the icon and the title for the tooltip window
			::SendMessage (hWndToolTip, TTM_SETTITLE, icon_type, (LPARAM) title);

			// sets a maximum width for the tool tip window (else it won't wrap lines at all)
			::SendMessage (hWndToolTip, TTM_SETMAXTIPWIDTH, 0, 500);

			// I dunno why, but I must call this to display the window (it wont work without)
			::SendMessage (hWndToolTip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &ti);

			// is this tooltip NOT associated with another window ?
			if (hWndParent == NULL)
			{
				::SendMessage (hWndToolTip, TTM_TRACKPOSITION, 0, (LPARAM) MAKELONG (pos.x, pos.y));
			}
		}
	}
}
