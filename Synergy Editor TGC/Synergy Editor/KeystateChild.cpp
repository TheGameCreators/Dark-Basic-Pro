#include "StdAfx.h"
#include "KeystateChild.h"
#include "winuser.h"
#include "Resource.h"

#include "Synergy Editor.h"
#include "Doc.h"
#include "View.h"
#include "Settings.h"

KeystateChild::KeystateChild()
{
	key = 0;
	ascii = 0;
}

KeystateChild::~KeystateChild()
{}

BEGIN_MESSAGE_MAP(KeystateChild, CWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_BN_CLICKED(333, OnClick)
END_MESSAGE_MAP()

BOOL KeystateChild::PreTranslateMessage(MSG* pMsg)
{
	HACCEL accel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	if(TranslateAccelerator(this->GetSafeHwnd(), accel, pMsg))
		 return TRUE;
	return FALSE;
}

int KeystateChild::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_Button.Create(_T("Insert"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
	CRect(5,20,100,40), this, 333);

	return 0;
}

void KeystateChild::OnClick()
{
	CWnd* pMainWnd = AfxGetMainWnd();
	ASSERT_VALID(pMainWnd);

	if(!pMainWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
	{
		return;
	}

	CView* pView = static_cast<CFrameWnd*>(pMainWnd)->GetActiveFrame()->GetActiveView();
	ASSERT_VALID(pView);

	if(pView->IsKindOf(RUNTIME_CLASS(View)))
	{
		View* view = (View*)pView;

		CString read;
		if(Settings::IntellisenseCase == 0)
		{
			read.Format(_T("KEYSTATE(%d)"),key);
		}
		else if(Settings::IntellisenseCase == 1)
		{
			read.Format(_T("keystate(%d)"),key);
		}
		else
		{
			read.Format(_T("Keystate(%d)"),key);
		}

		view->InsertText(read);
	}

	SetFocus();
}

// ChildView message handlers
BOOL KeystateChild::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void KeystateChild::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	key = MapVirtualKey(nChar, 0);	
	this->Invalidate();
}

void KeystateChild::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ascii = nChar;
}

void KeystateChild::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);

	dc.DrawText(_T("Press a key within this window to view its value"), &rect, DT_CENTER | DT_WORDBREAK);

	rect.top = rect.bottom - 15;

	CString read;
	read.Format(_T("Keystate: %d; ASCII: %d"),key, ascii);
	dc.DrawText(read, &rect, DT_CENTER | DT_WORDBREAK);
}
