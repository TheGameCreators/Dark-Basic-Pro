#include "StdAfx.h"
#include "ImageChild.h"
#include "winuser.h"
#include "Resource.h"

BEGIN_MESSAGE_MAP(ImageChild, CWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void ImageChild::SetPicture(CString m_Filename)
{	
	m_Bitmap.SetFilename(m_Filename, false);	

	CRect myRect;
	GetClientRect(&myRect);
	CPoint imageSize = m_Bitmap.GetSize();

	ClientToScreen(myRect);

	GetParentFrame()->MoveWindow(myRect.left, myRect.top, imageSize.x + 20, imageSize.y + 40);
}

int ImageChild::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	GetClientRect (rectDummy);

	const DWORD bitmapViewStyle= WS_CHILD | WS_VISIBLE;
	if(!m_Bitmap.Create(_T(""), bitmapViewStyle, rectDummy, this, 2))
		return -1;

	return 0;
}

// ChildView message handlers
BOOL ImageChild::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void ImageChild::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect (rectClient);

	m_Bitmap.SetWindowPos (NULL, 0, 0, rectClient.Width (), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void ImageChild::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);

	m_Bitmap.Invalidate(TRUE);
}
