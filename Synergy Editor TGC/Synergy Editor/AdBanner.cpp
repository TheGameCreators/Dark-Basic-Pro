// AdBanner.cpp : implementation file
//

#include "stdafx.h"
#include "AdBanner.h"
#include "HTTPComm.h"
#include "GameCreatorStore.h"
#include <atlimage.h>


// CAdBanner

IMPLEMENT_DYNAMIC(CAdBanner, CButton)

CAdBanner::CAdBanner()
{
	m_bBitmapLoaded = false;
	m_bLoading = false;
}

CAdBanner::~CAdBanner()
{
}


BEGIN_MESSAGE_MAP(CAdBanner, CButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

unsigned CAdBanner::Run( )
{
	m_bBitmapLoaded = false;
	m_bLoading = true;

	CString sPath = "TGCStore\\TEMP\\AdBanner.bmp";
	HTTPComm server;

	//HTTP call here
	bool result = server.GetPreviewImage( "HTMLImages/AdBanner.bmp", sPath );
	//if ( !result ) MessageBox( server.GetLastError( ), "TGC Store Error" );
	if ( !result ) return 0;

	//try and load the downloaded image
	SetCurrentDirectory( theApp.m_szDirectory );
	CImage image;
	HRESULT hr = image.Load( sPath );
	if ( FAILED(hr) ) return 0;

	CWindowDC wndDC( this );
	CDC dc;
	dc.CreateCompatibleDC( &wndDC );

	//copy the image to the local bitmap
	m_bmpBanner.DeleteObject( );
	m_bmpBanner.CreateBitmap( image.GetWidth(),image.GetHeight(), 1, 32, NULL );
	CBitmap *oldBmp = dc.SelectObject( &m_bmpBanner );

	::SetStretchBltMode( image.GetDC(), HALFTONE );
	::SetBrushOrgEx( image.GetDC(), 0,0, NULL );
	image.BitBlt( dc.GetSafeHdc( ), 0,0 );
	dc.SelectObject( oldBmp );

	image.ReleaseDC();
	image.ReleaseDC();

	//let the control draw the bitmap now
	m_bLoading = false;
	m_bBitmapLoaded = true;

	Invalidate( );

	return 0;
}



// CAdBanner message handlers

BOOL CAdBanner::OnEraseBkgnd( CDC *pDC )
{
	return FALSE;
}

void CAdBanner::OnPaint()
{
	CPaintDC dc( this );

	CRect rectClient;
	GetClientRect( &rectClient );

    //CBrush brush; brush.CreateSolidBrush( GetSysColor( COLOR_3DFACE ) );
	CPen pen; pen.CreatePen( PS_SOLID, 1, RGB(0,0,0) );
	dc.SelectObject( &pen );
	//dc.SelectObject( &brush );
	dc.SelectStockObject( NULL_BRUSH );
	dc.Rectangle( &rectClient );

	if ( m_bBitmapLoaded )
	{
		//if the bitmap is loaded
		CDC bmpDC;
		bmpDC.CreateCompatibleDC( &dc );
		bmpDC.SelectObject( &m_bmpBanner );

		BITMAP details;
		m_bmpBanner.GetBitmap( &details );
		
		int iY = 0;
		if ( rectClient.Height() > details.bmHeight ) iY = (rectClient.Height() - details.bmHeight) / 2;

		dc.BitBlt( 0,iY, rectClient.Width(), rectClient.Height(), &bmpDC, 0,0, SRCCOPY );
	}
    
	if ( m_bLoading )
	{
		//optional loading screen
		dc.SelectObject( GetFont( ) );
		dc.SetBkMode( OPAQUE );
		dc.SetBkColor( GetSysColor( COLOR_3DFACE ) );
		dc.SetTextColor( RGB(0,0,0) );
	
		//CString sText;
		//GetWindowText( sText );
		//dc.DrawText( GetLanguageData( _T("TGC Store"), _T("PleaseWait") ), -1, &rectClient, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
		//sText.ReleaseBuffer( );
	}
}