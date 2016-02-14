// MainFrm.h : interface of the MainFrame class
#pragma once
#include "WorkSpaceBar.h"
#include "WorkSpaceBar2.h"
#include "MediaBar.h"
#include "CursorBar.h"
#include "OutputBar.h"
#include "ToDoWindow.h"
#include "ClassView.h"
#include "WelcomeDialog.h"
#include "UpdateWindow.h"
#include "GotoLineDlg.h"
#include "SemanticParserManager.h"

class MainFrame : public CBCGPMDIFrameWnd
{
	DECLARE_DYNAMIC(MainFrame)
public:
	MainFrame();

// Attributes
public:
	void AddOutputText(CString text);
	void ClearOutputText();

	void AddNewFile(CString text);
	void DeleteOldFile(CString text);
	void ClearInclude();

	void AddToDo(CString text, BOOL checked);
	void ClearToDo();

	void SetStatus(int status);
	void SetStatusMax(int max);
	void ShowOutput();
	void HideOutput();

	void SetStatusText(CString text);
	
	void UpdateFromLibrary();
	
	void AddNewMediaFile(CString text);
	void ClearMedia();

	void AddNewCursorFile(CString text);
	void ClearCursors();

	void AddComment(CString line);
	void ClearComments();
	void DumpComments();

	void OnOpenProject(); // Called by DarkBASIC Professional Editor if the default action is to show this dialog
	void OnOpenProject(CString filename); // Called by DarkBASIC Professional Editor on MRU

	void SetBuildTypeToMedia();

	void GoToViewLine(CString file, int line); // Used by search in files

	void MakeToolTip(); // Show the run example tooltip
	
	// Public because they are used by the options dialog
	afx_msg void OnAppLook(UINT id);

	CString m_DefaultPath; // Set when processing command line arguments, so make public

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~MainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CBCGPStatusBar			m_wndStatusBar;
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPToolBar			m_wndToolBar;
	CBCGPToolBar			m_wndHelpToolBar;
	CBCGPToolBar			m_wndEditToolBar;
	CWorkSpaceBar			m_SolutionExplorer;
	CWorkSpaceBar2			m_Properties;
	MediaBar				m_Media;
	CursorBar				m_Cursor;
	COutputBar				m_wndOutput;
	ToDoWindow				m_ToDo;
	ClassView				m_ClassView;	

	void CleanUp();

	void OnNewProject();
	void OnNewFile();
	void OnSaveProject();
	void OnSaveProjectAs();
	void OnSaveAll();
	void OnOpenAny();

	void OnFindInFiles();
	void OnGotoLine();
	void GotoLine(int badLine);

	void OnUpdateCompile(CCmdUI *pCmdUI);
	void OnCompile();
	void OnCompileRun();
	void OnDebug();
	void OnStep();
	void OnRunPrevious();
	void OnUpdateRunPrevious(CCmdUI* pCmdUI);

	void OnCommandArgs();
	void OnOptions();
	void OnCustomise();

	void OnRebuild();

	void OnFullScreen();

	void OnTutorial();
	void OnTutorialUpdate(CCmdUI* pCmdUI);
	void OnUpdate();
	void OnCheckModules();
	void OnDBPAbout();
	void OnDGS();
	void OnKeyboard();

	void OnOpenGameCreatorStore();

	void OnKeyHelper();

	void GoURL(CString url);

	void AssociateFiles();

	bool outputNeedsCollapse;

	LRESULT OnUser(WPARAM, LPARAM);
	static LPCWSTR s_winClassName;

	LRESULT SetStatusMaxMessage(WPARAM a, LPARAM b);
	LRESULT SetStatusMessage(WPARAM a, LPARAM b);
	LRESULT SetOutputMessage(WPARAM a, LPARAM b);
	CString message;

	void CreateToolTip (HWND hWndParent, CPoint pos, long icon_type, const TCHAR *title, const TCHAR *text, bool is_balloon);
	HWND hWndToolTip;	

	CBCGPToolBarImages	m_UserImages;

	WelcomeDialog* wd;
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	void OnToolsViewUserToolbar (UINT id);
	void OnUpdateToolsViewUserToolbar (CCmdUI* pCmdUI);
	afx_msg void OnWindowManager();
	afx_msg void OnUpdateAppLook(CCmdUI* pCmdUI);
	afx_msg void OnMdiMoveToNextGroup();
	afx_msg void OnMdiMoveToPrevGroup();
	afx_msg void OnMdiNewHorzTabGroup();
	afx_msg void OnMdiNewVertGroup();
	afx_msg void OnMdiCancel();
	afx_msg void OnClose();
	afx_msg void OnTimer( UINT );
	void OnActivateApp(BOOL bActive, DWORD dwThreadID);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnShowMDITabContextMenu (CPoint point, DWORD dwAllowedItems, BOOL bDrop);
	virtual CBCGPMDIChildWnd* CreateDocumentWindow (LPCTSTR lpcszDocName, CObject* /*pObj*/);

	UINT	m_nAppLook;
};


