#include "stdafx.h"
#include "ImagePreviewStatic.h"

// CImagePrieviewStatic
IMPLEMENT_DYNAMIC(CImagePreviewStatic, CStatic)

CImagePreviewStatic::CImagePreviewStatic() : CStatic()
{
	m_img = (Image *) NULL;
	m_graphics = (Graphics *) NULL;
	stretch = false;
	deleted = true; // Don't need to delete now
	sFilename = _T("");
}

CImagePreviewStatic::~CImagePreviewStatic()
{
	if(!deleted)
	{
		delete m_img;
	}
	delete m_graphics;
}

int CImagePreviewStatic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CStatic::OnCreate(lpCreateStruct);
	return 0;
}

void CImagePreviewStatic::SetFilename(CString szFilename, bool newStretch)
{
	if(szFilename == _T(""))
	{
		if(!deleted)
		{
			delete m_img;
			m_img = NULL;
			deleted = true;
			sFilename = _T("");
		}
	}
	else
	{
		if(szFilename == sFilename)
		{
			return;
		}
		if(!deleted)
		{
			delete m_img;			
		}
		sFilename = szFilename;
		m_img =	new	Image(sFilename, FALSE);
		m_BitmapSize.x = m_img->GetWidth();
		m_BitmapSize.y = m_img->GetHeight();
		stretch = newStretch;
		deleted = false;
	}
	Invalidate(TRUE);
}

CPoint CImagePreviewStatic::GetSize()
{
	return m_BitmapSize;
}

void CImagePreviewStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	m_graphics = new Graphics(dc);

	Unit  units;
	CRect rect;

	GetClientRect(&rect);

	CBrush brush(RGB(255, 255, 255));
	dc.FillRect(rect, &brush);

	if (m_img != NULL)
	{
		RectF srcRect;
		m_img->GetBounds(&srcRect, &units);
		if(rect.Width() > srcRect.Width && rect.Height() > srcRect.Height)
		{
			REAL fLeft = (rect.Width() - srcRect.Width) / 2;
			REAL fTop = (rect.Height() - srcRect.Height) / 2;
			m_graphics->DrawImage(m_img, fLeft, fTop, srcRect.Width, srcRect.Height);
		}
		else if(stretch)
		{
			RectF destRect(REAL(rect.left),	REAL(rect.top),	REAL(rect.Width()),	REAL(rect.Height()));
			m_graphics->DrawImage(m_img, destRect, srcRect.X, srcRect.Y, srcRect.Width,	srcRect.Height,	UnitPixel, NULL);
		}
		else
		{	
			RectF destRect(REAL(rect.left),	REAL(rect.top),	REAL(srcRect.Width),	REAL(srcRect.Height));		
			m_graphics->DrawImage(m_img, destRect, srcRect.X, srcRect.Y, srcRect.Width,	srcRect.Height,	UnitPixel, NULL);
		}
	}
}

BEGIN_MESSAGE_MAP(CImagePreviewStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()
