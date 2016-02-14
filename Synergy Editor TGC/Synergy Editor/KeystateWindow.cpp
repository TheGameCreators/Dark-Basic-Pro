#include "StdAfx.h"
#include "KeystateWindow.h"

// MainFrame

IMPLEMENT_DYNAMIC(KeystateWindow, CFrameWnd)

BEGIN_MESSAGE_MAP(KeystateWindow, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// MainFrame construction/destruction

KeystateWindow::KeystateWindow()
{
	// TODO: add member initialization code here
}

KeystateWindow::~KeystateWindow()
{
}

void KeystateWindow::PostNcDestroy()
{	
	delete this;
}

int KeystateWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CenterWindow(CWnd::GetDesktopWindow());

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	return 0;
}

BOOL KeystateWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// MainFrame diagnostics

#ifdef _DEBUG
void KeystateWindow::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void KeystateWindow::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// MainFrame message handlers

void KeystateWindow::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL KeystateWindow::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
