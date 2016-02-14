#include "StdAfx.h"
#include "ImageWindow.h"

// MainFrame

IMPLEMENT_DYNAMIC(ImageWindow, CFrameWnd)

BEGIN_MESSAGE_MAP(ImageWindow, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// MainFrame construction/destruction

ImageWindow::ImageWindow()
{}

ImageWindow::~ImageWindow()
{}

void ImageWindow::SetPicture(CString m_Filename)
{
	m_wndView.SetPicture(m_Filename);
}

int ImageWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

BOOL ImageWindow::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_SYSMENU | WS_SIZEBOX;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// MainFrame diagnostics

#ifdef _DEBUG
void ImageWindow::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void ImageWindow::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// MainFrame message handlers

void ImageWindow::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL ImageWindow::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
