#include "StdAfx.h"
#include "SnippetWnd.h"

IMPLEMENT_DYNAMIC(SnippetWnd, CMiniFrameWnd)

BEGIN_MESSAGE_MAP(SnippetWnd, CMiniFrameWnd)
	ON_LBN_KILLFOCUS(1, OnLbnKillfocusList1)
	ON_LBN_DBLCLK(1, OnLbnDblclkList1)
	ON_WM_VKEYTOITEM()
	ON_WM_DESTROY()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void SnippetWnd::OnLbnKillfocusList1()
{
	PostMessage(WM_CLOSE);
}

int SnippetWnd::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	switch(nKey)
	{
		case VK_ESCAPE:
			PostMessage(WM_CLOSE);
			return -2;
		case VK_RETURN: 
		case VK_TAB:
		case VK_SPACE:
			OnLbnDblclkList1();
			break;
	}
	return -1;
}

void SnippetWnd::OnLbnDblclkList1()
{
	CString start, end;
	int startL, endL;

	int item = m_pLstBoxData->GetCurSel();
	int n = m_pLstBoxData->GetTextLen( item );
	CString text;
	text.LockBuffer();
	m_pLstBoxData->GetText(item, text.GetBuffer(n));
	text.ReleaseBuffer();

	if(text == _T("do"))
	{
		start = _T("Do");
		end = _T("Loop");
	} 
	else if(text ==  _T("for"))
	{
		start = _T("For variable = value To value");
		end = _T("Next variable");
	}
	else if(text ==  _T("for..step"))
	{
		start = _T("For variable = value To value Step value");
		end = _T("Next variable");
	}
	else if(text == _T("function"))
	{
		start = _T("Function name (arguments)");
		end = _T("EndFunction");
	}
	else if(text == _T("if"))
	{
		start = _T("If");
		end = _T("EndIf");
	}
	else if(text == _T("repeat"))
	{
		start = _T("Repeat");
		end = _T("Until");
	}
	else if(text == _T("while"))
	{
		start = _T("While");
		end = _T("EndWhile");		
	}
	else if(text == "Standard Init")
	{
		start = _T("rem Standard Setup Code\nsync on : sync rate 0 : color backdrop rgb(0,0,128) : hide mouse\nset text font \"arial\" : set text size 12 : set text transparent");
		end = _T("");
	}
	
	if(m_Surround)
	{
		start.Append(_T("\n"));
		startL = start.GetLength() + 1;
		TCHAR* a = new TCHAR[startL];
		wcscpy_s(a, startL, start);
		end.Insert(0, _T("\n"));
		endL = end.GetLength() + 1;
		TCHAR* b = new TCHAR[endL];
		wcscpy_s(b, endL, end);
		m_pParentWnd->SendMessage(WM_USER, (WPARAM)a, (LPARAM)b);
	}
	else
	{
		start = start + _T("\n\n") + end;
		startL = start.GetLength() + 1;
		TCHAR* a = new TCHAR[startL];
		wcscpy_s(a, startL, start);
		m_pParentWnd->SendMessage(WM_USER, (WPARAM)a, 0);
	}
	PostMessage(WM_CLOSE);
}

BOOL SnippetWnd::Create(LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	m_pParentWnd = pParentWnd;

	if (!CMiniFrameWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID))
	{
		return FALSE;
	}

	m_pLstBoxData = new CListBox();

	CRect myRect;
	GetClientRect(myRect);

	if (!m_pLstBoxData->Create (WS_CHILD | WS_VSCROLL | LBS_NOTIFY | LBS_WANTKEYBOARDINPUT | LBS_HASSTRINGS | LBS_SORT, 
		CRect (0, 0, myRect.Width(), myRect.Height()), this, 1))
	{
		return FALSE;
	}
	 
	m_pLstBoxData->AddString(_T("do"));
	m_pLstBoxData->AddString(_T("for"));
	m_pLstBoxData->AddString(_T("for..step"));
	m_pLstBoxData->AddString(_T("function"));
	m_pLstBoxData->AddString(_T("if"));
	m_pLstBoxData->AddString(_T("repeat"));
	m_pLstBoxData->AddString(_T("while"));
	if(!m_Surround)
	{
		m_pLstBoxData->AddString(_T("Standard Init"));
	}

	LOGFONT logfont;
	memset (&logfont, 0, sizeof (LOGFONT));
	logfont.lfHeight = 8;
	lstrcpy (logfont.lfFaceName, _T ("MS Sans Serif"));

	m_lbFont.CreateFontIndirect (&logfont);
	m_pLstBoxData->SetFont(&m_lbFont);

	m_pLstBoxData->ShowWindow (SW_SHOW);
	m_pLstBoxData->SetSel(0);
	m_pLstBoxData->SetFocus ();
	ShowWindow (SW_SHOW);
	m_pLstBoxData->RedrawWindow ();

	return TRUE;
}

void SnippetWnd::OnDestroy() 
{
	CMiniFrameWnd::OnDestroy();
}

void SnippetWnd::PostNcDestroy() 
{
	delete m_pLstBoxData;
	
	CMiniFrameWnd::PostNcDestroy();
}

BCGNcHitTestType SnippetWnd::OnNcHitTest(CPoint /*point*/) 
{
	return HTCLIENT;
}


SnippetWnd::SnippetWnd(bool surround)
{
	m_Surround = surround;
}

SnippetWnd::~SnippetWnd(void){}
