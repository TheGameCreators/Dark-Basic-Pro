// outputbar.cpp : implementation of the COutputBar class

// Includes
#include "stdafx.h"
#include "Synergy Editor.h"
#include "outputbar.h"
#include "Utilities.h"
#include "Settings.h"
#include "direct.h"
#include "io.h"
#include "httpcomm.h"
#include "Mmsystem.h"

// External global - set in AdvertThread.cpp
extern bool g_bUsingAdvertising;
extern bool g_bUsingAdvertisingLoaded;
extern LONGLONG g_lUsingAdvertisingTimeStamp;

// Defines
#define _CRT_SECURE_NO_WARNINGS
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

BEGIN_MESSAGE_MAP(COutputBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(COutputBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()

	ON_BN_CLICKED(ID_OUTPUT_COPY, OnCopy)
	ON_BN_CLICKED(ID_OUTPUT_WRITE, OnWrite)
	ON_BN_CLICKED(ID_OUTPUT_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void COutputBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint pt = point;	
	m_wndList.ScreenToClient(&pt);

	UINT unFlags = 0 ;
	int hItem = m_wndList.HitTest(pt, &unFlags) ;
	if(hItem > -1)
	{
		m_wndList.SetItemState(hItem, LVIS_SELECTED, LVIS_SELECTED);
	}	

	if(point.x == -1 && point.x == -1)
	{
		CRect rect;
		GetClientRect(&rect);
		point = rect.TopLeft();
		point.Offset(5,5);
		ClientToScreen(&point);
	}

	CMenu mnuTop;
	if(hItem > -1)
	{
		mnuTop.LoadMenu(IDR_OUTPUT_POPUP);
	}
	else
	{
		mnuTop.LoadMenu(IDR_OUTPUT_POPUP_2);
	}

	CMenu* pPopup = mnuTop.GetSubMenu(0);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this, NULL);
}

void COutputBar::AddOutputText(CString text)
{	
	m_wndList.InsertItem(m_wndList.GetItemCount(), text);
	m_wndList.EnsureVisible(m_wndList.GetItemCount()-1, TRUE);
	if ( g_bUsingAdvertising==true )
	{
		// after X minutes or initially
		LONGLONG lTimeNow = timeGetTime();
		if ( lTimeNow > g_lUsingAdvertisingTimeStamp )
		{
			// call refresh on advert view
			App* app = (App*)AfxGetApp();
			if ( app->pAdvertView ) 
			{
				app->pAdvertView->Navigate(_T("http://advert.thegamecreators.com/adCampaigns.php?width=728&height=90"));
				app->pAdvertView->Refresh();
			}

			// set next refresh for two minutes
			g_lUsingAdvertisingTimeStamp = lTimeNow + (1000*60*2);
		}
	}
}

void COutputBar::SetStatus(int status)
{
	if(status == -1)
	{
		m_wndProg.SetPos(0);
		return;
	}

	int min, max;
	m_wndProg.GetRange( min, max );

	if(status < lastnumber)
	{
		base += max / 4;
	}
	lastnumber = status;

	if(base + status < max)
	{
		m_wndProg.SetPos(base + status);
	}
}
	
void COutputBar::SetStatusMax(int max)
{
	lastnumber = -1;
	base = 0;
	m_wndProg.SetRange(0, max * 4);
	m_wndProg.SetPos(0);
}

void COutputBar::OnCopy()
{
	POSITION pos = m_wndList.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		int nItem = m_wndList.GetNextSelectedItem(pos);
		CString text = m_wndList.GetItemText(nItem, 0);
		int size = text.GetLength()+1;

		if ( !OpenClipboard() )
		{
			AfxMessageBox( _T("Cannot open the Clipboard") );
			return;
		}
		if( !EmptyClipboard() )
		{
			AfxMessageBox( _T("Cannot empty the Clipboard") );
			return;
		}
		HGLOBAL hClipboardData;
		hClipboardData = GlobalAlloc(GMEM_DDESHARE, size);

		char * pchData = (char*)GlobalLock(hClipboardData);

		size_t   converted;
		wcstombs_s(&converted, pchData, size, LPCWSTR(text), size);	

		GlobalUnlock(hClipboardData);

		if ( ::SetClipboardData( CF_TEXT, hClipboardData ) == NULL )
		{
			AfxMessageBox( _T("Unable to set Clipboard data") );
			CloseClipboard();
			return;
		}
		CloseClipboard();
	}
}

void COutputBar::OnWrite()
{
	CFileDialog FileDlg(FALSE, _T(".txt"), NULL, 0, _T("Text Files (*.txt)|*.txt||"));
	FileDlg.GetOFN().lpstrInitialDir = Utilities::ExtractPath(Utilities::GetApplicationPath());

	if( FileDlg.DoModal() != IDOK )
	{
		return;
	}

	CStdioFile wfile;
	BOOL openWrite = wfile.Open(FileDlg.GetPathName(),CFile::modeWrite | CFile::modeCreate|CFile::modeNoTruncate);
	if(openWrite)
	{
		for(int i=0; i < m_wndList.GetItemCount(); i++)
		{
			wfile.WriteString(m_wndList.GetItemText(i,0));
			wfile.WriteString(_T("\n"));
		}
		wfile.Flush();
		wfile.Close();
	}
}

void COutputBar::OnClear()
{
	m_wndList.DeleteAllItems();
}

/////////////////////////////////////////////////////////////////////////////
// COutputBar construction/destruction

COutputBar::COutputBar()
{
//	m_pwndAdvert = NULL;
}

COutputBar::~COutputBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// COutputBar message handlers

int COutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	const DWORD dwListViewStyle =	
		LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_NOCOLUMNHEADER;

	const DWORD dwProgViewStyle =	
		LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE;
	
	if (!m_wndList.Create (dwListViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create list view\n");
		return -1;      // fail to create
	}
	m_wndList.SetView(LV_VIEW_DETAILS);
	m_wndList.InsertColumn(0, _T("Details"));
	m_wndList.InsertItem(0, _T("Welcome to DarkBASIC Professional Editor"));

	if (!m_wndProg.Create(dwProgViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create progress bar\n");
		return -1;      // fail to create
	}
	m_wndProg.SetRange(0, 100);
	m_wndProg.SetPos(0);

	// place views within outputbar layout
	AdjustLayout ();

	// done
	return 0;
}
void COutputBar::OnDestroy ( void )
{
}
//************************************************************************************
void COutputBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL) return;
	CRect rectClient;
	GetClientRect (rectClient);

	// AdSense Dimensions
	int advertHeight = 0;
	if ( g_bUsingAdvertising==true )
	{
		// navigate to required web page if first time
		App* app = (App*)AfxGetApp();
		if ( g_bUsingAdvertisingLoaded==false )
		{
			app->pAdvertView->Navigate(_T("http://advert.thegamecreators.com/adCampaigns.php?width=728&height=90"));
			g_bUsingAdvertisingLoaded=true;
		}

		// move the web page view
		advertHeight = 98; // 728x90 = Banner ADVERT (with border)
		if ( app->pAdvertWnd )
		{
			CRect rectWindow;
			GetWindowRect ( rectWindow ); rectWindow.top += 18;
			app->pAdvertWnd->SetWindowPos ( NULL, rectWindow.left + rectClient.left, rectWindow.top + (rectClient.top + (rectClient.Height() - advertHeight)), rectClient.Width (), advertHeight, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW );
		}
	}

	// Output list area
	int barHeight = 15;
	m_wndList.SetWindowPos (NULL, rectClient.left, rectClient.top, 
								rectClient.Width (), rectClient.Height () - barHeight - advertHeight,
								SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndList.SetColumnWidth(0, rectClient.Width () - 20);

	// Progress bar
	m_wndProg.SetWindowPos (NULL, rectClient.left, rectClient.top + rectClient.Height () - barHeight - advertHeight,
								rectClient.Width (), barHeight,
								SWP_NOACTIVATE | SWP_NOZORDER);	
}
//************************************************************************************
void COutputBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);
	AdjustLayout();
}
void COutputBar::OnMoving(UINT nSide, LPRECT lpRect)
{
	CBCGPDockingControlBar::OnMoving(nSide, lpRect);
	AdjustLayout();
}
//************************************************************************************
void COutputBar::OnPaint() 
{
	// must keep updating output layout to reposition window as output bar moves
	if ( g_bUsingAdvertising==true ) AdjustLayout();

	// regular draw code
	CPaintDC dc(this); // device context for painting
	CRect rectList;
	m_wndList.GetWindowRect (rectList);
	ScreenToClient (rectList);
	rectList.InflateRect (1, 1);
	dc.Draw3dRect (rectList,	::GetSysColor (COLOR_3DSHADOW), 
								::GetSysColor (COLOR_3DSHADOW));
}
