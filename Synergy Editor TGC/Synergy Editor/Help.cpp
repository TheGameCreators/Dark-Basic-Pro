#include "StdAfx.h"
#include "Help.h"
#include "Resource.h"
#include "Utilities.h"
#include "rpcdce.h"
#include "CompileSupport.h"
#include "Settings.h"
#include "MainFrm.h"

IMPLEMENT_DYNCREATE(CHelp, CHtmlView)

BEGIN_MESSAGE_MAP(CHelp, CHtmlView)
	//{{AFX_MSG_MAP(CMfcieView)
	ON_COMMAND(ID_BROWSER_GOBACK, OnGoBack)
	ON_COMMAND(ID_BROWSER_GOFORWARD, OnGoForward)
	ON_COMMAND(ID_BROWSER_STOP, OnViewStop)
	ON_COMMAND(ID_BROWSER_REFRESH, OnViewRefresh)
	ON_COMMAND(ID_FONT_LARGEST, OnViewFontsLargest)
	ON_COMMAND(ID_FONT_LARGE, OnViewFontsLarge)
	ON_COMMAND(ID_FONT_MEDIUM, OnViewFontsMedium)
	ON_COMMAND(ID_FONT_SMALL, OnViewFontsSmall)
	ON_COMMAND(ID_FONT_SMALLEST, OnViewFontsSmallest)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMfcieView construction/destruction

BOOL CHelp::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.lpszClass = AfxRegisterWndClass(0);
	return CView::PreCreateWindow(cs);
}

CHelp::CHelp()
{
	m_bHelpShown = false;
}

/////////////////////////////////////////////////////////////////////////////
// CMfcieView drawing

void CHelp::OnDraw(CDC* /*pDC*/)
{
	HTMLDocument* pDoc = (HTMLDocument*)GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CMfcieView diagnostics

#ifdef _DEBUG
void CHelp::AssertValid() const
{
	CView::AssertValid();
}

void CHelp::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

HTMLDocument* CHelp::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(HTMLDocument)));
	return (HTMLDocument*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMfcieView message handlers

void CHelp::OnEditFind()
{
	static const GUID cmdGUID = 
		{ 0xED016940, 0xBD5B, 0x11CF, { 0xBA, 0x4E, 0x00, 0xC0, 0x4F, 0xD7, 0x08, 0x16 } };

	//1 : FIND, 2: VIEWSOURCE, 3 : OPTIONS
	DWORD nCmdID = 1; 

	LPDISPATCH lpDispatch = NULL;
	LPOLECOMMANDTARGET lpOleCommandTarget = NULL;

	// pBrowserApp is member of CHtmlView 
	HRESULT hr = m_pBrowserApp->get_Document( &lpDispatch );
	if( FAILED(hr) || lpDispatch == NULL )
	{
		ASSERT(FALSE);
		return;
	}

	// Get an IDispatch pointer for the IOleCommandTarget interface.
	lpDispatch->QueryInterface( IID_IOleCommandTarget, (void**)&lpOleCommandTarget);
	ASSERT(lpOleCommandTarget);

	lpDispatch->Release();

	// Invoke the given command id for the WebBrowser control
	lpOleCommandTarget->Exec(&cmdGUID, nCmdID, 0, NULL, NULL);

	// Release the command target
	lpOleCommandTarget->Release();
} 

void CHelp::OnTitleChange(LPCTSTR lpszText)
{
	// this will change the main frame's title bar
	if (m_pDocument != NULL)
		m_pDocument->SetTitle(Utilities::RemoveExtension(Utilities::ExtractFilename(lpszText)));
}

void CHelp::OnDocumentComplete(LPCTSTR lpszUrl)
{
	// make sure the main frame has the new URL.  This call also stops the animation
	//((CMainFrame*)GetParentFrame())->SetAddress(lpszUrl);
}

void CHelp::OnInitialUpdate()
{
	// Go to the home page initially
	CHtmlView::OnInitialUpdate();
}

void CHelp::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
	*pbCancel = TRUE;
	//Navigate2(Settings::DBPLocation + _T("Help\\main.htm"));
}

void CHelp::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD /*nFlags*/,
	LPCTSTR /*lpszTargetFrameName*/, CByteArray& /*baPostedData*/,
	LPCTSTR /*lpszHeaders*/, BOOL* /*pbCancel*/)
{
	CString link = CString(lpszURL);
	// leefix - 300908 - does not have to be case sensitive!
	CString lowerlink = link.MakeLower();
	if(lowerlink.Find(_T("dba.htm")) > -1)
	{
		CompileSupport::m_strHelpFile = link.Mid(0, link.GetLength() - 4);
		if(!m_bHelpShown)
		{
			MainFrame* pMainFrm = (MainFrame*)AfxGetMainWnd();
			pMainFrm->MakeToolTip();
			m_bHelpShown = true;
		}
	}
	else
	{
		CompileSupport::m_strHelpFile = _T("");
	}	
	url = CompileSupport::m_strHelpFile;
}

CHelp::~CHelp()
{
	CompileSupport::m_strHelpFile = _T("");
}

void CHelp::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{	
	if(!bActivate)
	{
		CompileSupport::m_strHelpFile = _T("");	
	}
	else
	{
		CompileSupport::m_strHelpFile = url;
	}
	CHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

// these are all simple one-liners to do simple controlling of the browser
void CHelp::OnGoBack()
{
	GoBack();
}

void CHelp::OnGoForward()
{
	GoForward();
}

void CHelp::OnGoStartPage()
{
	GoHome();
}

void CHelp::OnViewStop()
{
	Stop();
}

void CHelp::OnViewRefresh()
{
	Refresh();
}

void CHelp::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == VK_ESCAPE)
	{
		CFrameWnd* pFrame = this->GetParentFrame(); 
		pFrame->DestroyWindow();
		return;
	}
	CHtmlView::OnKeyDown(nChar, nRepCnt, nFlags);
}

// these functions control the font size.  There is no explicit command in the
// CHtmlView class to do this, but we can do it by using the ExecWB() function.
void CHelp::OnViewFontsLargest()
{
	COleVariant vaZoomFactor(4l);

	ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
		   &vaZoomFactor, NULL);
}

void CHelp::OnViewFontsLarge()
{
	COleVariant vaZoomFactor(3l);

	ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
		   &vaZoomFactor, NULL);
}

void CHelp::OnViewFontsMedium()
{
	COleVariant vaZoomFactor(2l);

	ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
		   &vaZoomFactor, NULL);
}

void CHelp::OnViewFontsSmall()
{
	COleVariant vaZoomFactor(1l);

	ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
		   &vaZoomFactor, NULL);
}

void CHelp::OnViewFontsSmallest()
{
	COleVariant vaZoomFactor(0l);

	ExecWB(OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
		   &vaZoomFactor, NULL);
}

// This demonstrates how we can use the Navigate2() function to load local files
// including local HTML pages, GIFs, AIFF files, etc.
void CHelp::OnFileOpen()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("HTML Files|*.htm;*.html|Text Files|*.txt|GIF Files|*.gif|JPEG Files|*.jpg;*.jpeg|AU Files|*.au|AIFF Files|*.aif;*.aiff|XBM Files|*.xbm|All Files|*.*||"));

	if(fileDlg.DoModal() == IDOK)
		Navigate2(fileDlg.GetPathName(), 0, NULL);
}
