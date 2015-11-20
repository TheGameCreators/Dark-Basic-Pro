
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include "cbasic2Dc.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

typedef IDirect3DDevice9* ( *GFX_GetDirect3DDevicePFN ) ( void );

DBPRO_GLOBAL GlobStruct*					g_pGlob					 = NULL;
DBPRO_GLOBAL HINSTANCE						g_GFX;
DBPRO_GLOBAL GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;
DBPRO_GLOBAL LPDIRECT3DDEVICE9				m_pD3D;
DBPRO_GLOBAL bool							m_bIsLocked;
DBPRO_GLOBAL D3DLOCKED_RECT					m_LockedRect;
DBPRO_GLOBAL LPDIRECT3DSURFACE9				m_pSurface;
DBPRO_GLOBAL DWORD*							m_pData;
DBPRO_GLOBAL int							m_SurfaceWidth;
DBPRO_GLOBAL int							m_SurfaceHeight;
DBPRO_GLOBAL LPDIRECT3DVERTEXBUFFER9		m_pLineVB;
DBPRO_GLOBAL DWORD							m_dwBPP;
DBPRO_GLOBAL int							m_iBRed;
DBPRO_GLOBAL int							m_iBGreen;
DBPRO_GLOBAL int							m_iBBlue;
DBPRO_GLOBAL int							m_iFRed;
DBPRO_GLOBAL int							m_iFGreen;
DBPRO_GLOBAL int							m_iFBlue;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK int GetBitDepthFromFormat(D3DFORMAT Format)
{
	switch(Format)
	{
		case D3DFMT_R8G8B8 :		return 24;	break;
		case D3DFMT_A8R8G8B8 :		return 32;	break;
		case D3DFMT_X8R8G8B8 :		return 32;	break;
		case D3DFMT_R5G6B5 :		return 16;	break;
		case D3DFMT_X1R5G5B5 :		return 16;	break;
		case D3DFMT_A1R5G5B5 :		return 16;	break;
		case D3DFMT_A4R4G4B4 :		return 16;	break;
		case D3DFMT_A8	:			return 8;	break;
		case D3DFMT_R3G3B2 :		return 8;	break;
		case D3DFMT_A8R3G3B2 :		return 16;	break;
		case D3DFMT_X4R4G4B4 :		return 16;	break;
		case D3DFMT_A2B10G10R10 :	return 32;	break;
		case D3DFMT_G16R16 :		return 32;	break;
		case D3DFMT_A8P8 :			return 8;	break;
		case D3DFMT_P8 :			return 8;	break;
		case D3DFMT_L8 :			return 8;	break;
		case D3DFMT_A8L8 :			return 16;	break;
		case D3DFMT_A4L4 :			return 8;	break;
	}
	return 0;
}

DARKSDK void UpdateBPP(void)
{
	// Update BPP based on draw depth
	D3DSURFACE_DESC ddsd;
	g_pGlob->pCurrentBitmapSurface->GetDesc(&ddsd);
	m_dwBPP=GetBitDepthFromFormat(ddsd.Format)/8;
}

DARKSDK void Constructor ( HINSTANCE hSetup )
{
	// if we dont have any valid handles,
	if ( !hSetup )
	{
		// attempt to load the DLLs manually
		#ifndef DARKSDK_COMPILE
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );
		
		
		// create glob data locally
		g_pGlob = new GlobStruct;

        // u74b7 - Fixed the overwriting of stack data.
		//ZeroMemory(&g_pGlob, sizeof(GlobStruct));
		ZeroMemory(g_pGlob, sizeof(GlobStruct));
		#endif
	}

	#ifndef DARKSDK_COMPILE
	// Assign Setup DLL Handle
	g_GFX = hSetup;

	// check library was loaded
	if ( !g_GFX )
		Error ( "Cannot load setup library" );
	#endif

	// now setup function pointer and get interface to D3D
	//g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( g_GFX, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );

	#ifndef DARKSDK_COMPILE
	{
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( g_GFX, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	}
	#else
	{
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;
	}
	#endif

	m_pD3D                  = g_GFX_GetDirect3DDevice ( );

	m_bIsLocked = false;
	m_pSurface  = NULL;
	m_pData     = NULL;

    // 20091128 v75 - IRM - Expand the LineVB so that it can also be used for the BOX command
	m_pLineVB = NULL;
	m_pD3D->CreateVertexBuffer(	6*sizeof(tagLinesVertexDesc), D3DUSAGE_WRITEONLY, D3DFVF_LINES, D3DPOOL_DEFAULT, &m_pLineVB, NULL);

	memset ( &m_LockedRect, 0, sizeof ( m_LockedRect ) );
}

DARKSDK void Destructor ( void )
{
	SAFE_RELEASE(m_pLineVB);
}

DARKSDK void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK void PassCoreData( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;

	// Get BPP
	UpdateBPP();
}

DARKSDK void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		Destructor();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		Constructor ( g_pGlob->g_GFX );
		PassCoreData ( g_pGlob );
	}
}

DARKSDK void Clear ( int iRed, int iGreen, int iBlue )
{
	// Assign Colours
	m_iBRed=iRed;
	m_iBGreen=iGreen;
	m_iBBlue=iBlue;
	m_pD3D->Clear ( 1, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB( 255, m_iBRed, m_iBGreen, m_iBBlue ), 0.0f, 0 );
}

DARKSDK void Ink ( int iRedF, int iGreenF, int iBlueF, int iRedB, int iGreenB, int iBlueB )
{
	// Assign Colours
	m_iBRed=iRedB;
	m_iBGreen=iGreenB;
	m_iBBlue=iBlueB;
	m_iFRed=iRedF;
	m_iFGreen=iGreenF;
	m_iFBlue=iBlueF;

	// Assign COLORREF
	g_pGlob->dwForeColor = D3DCOLOR_ARGB( 255, m_iFRed, m_iFGreen, m_iFBlue );
	g_pGlob->dwBackColor = D3DCOLOR_ARGB( 255, m_iBRed, m_iBGreen, m_iBBlue );
}

#define _RGB16BIT565(r,g,b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))

DARKSDK USHORT RGB16Bit565(int r, int g, int b)
{
	r>>=3; g>>=2; b>>=3;
	return(_RGB16BIT565((r),(g),(b)));
} 

DARKSDK void PlotCore( int iX, int iY, DWORD Color )
{
	// mike - 230604 - modified core so that 16 bit colours are supported
	DWORD dwAlpha = Color >> 24;
	DWORD dwRed   = ((( Color ) >> 16 ) & 0xff );
	DWORD dwGreen = ((( Color ) >>  8 ) & 0xff );
	DWORD dwBlue  = ((( Color ) )       & 0xff );

	// mike - 280305 - allow to write over screen boundaries
	if(iX<0) return;
	if(iY<0) return;

	// leefix - 310305 - return code with additional bitmap specific check
	if ( g_pGlob->iCurrentBitmapNumber==0 )
	{
		if ( iX >= g_pGlob->iScreenWidth ) return;
		if ( iY >= g_pGlob->iScreenHeight ) return;
	}

	// mike - 021005 - ensure we're not plotting outside of surce			
	if ( iX >= m_SurfaceWidth )
		return;

	if ( iY >= m_SurfaceHeight )
		return;

	switch(m_dwBPP)
	{
		// mike - 250604
		case 2 : *((WORD*)m_pData + iX + iY * m_LockedRect.Pitch / 2 ) = RGB16Bit565 ( dwRed, dwGreen, dwBlue );	break;
		//case 2 : *((WORD*)m_pData + iX + iY * m_LockedRect.Pitch / 2 ) =WORD(Color);	break;
		case 4 : m_pData [ iX + iY * m_LockedRect.Pitch / 4 ] = Color;	break;
	}
}

DARKSDK DWORD GetPlotCore( int iX, int iY )
{
	// lee - 170206 - u60 - performance hit sure, but safest method
	if(iX<0) return 0;
	if(iY<0) return 0;
	if ( g_pGlob->iCurrentBitmapNumber==0 )
	{
		if ( iX >= g_pGlob->iScreenWidth ) return 0;
		if ( iY >= g_pGlob->iScreenHeight ) return 0;
	}
	if ( iX >= m_SurfaceWidth ) return 0;
	if ( iY >= m_SurfaceHeight ) return 0;

	switch(m_dwBPP)
	{
		case 2: return *((WORD*)m_pData + iX + iY * m_LockedRect.Pitch / 2 );
		case 4: return m_pData [ iX + iY*m_LockedRect.Pitch / 4 ];
	}
	return 0;
}

DARKSDK void Write ( int iX, int iY, DWORD Color )
{
	if(iX>=0 && iY>=0 && iX<=m_SurfaceWidth && iY<=m_SurfaceHeight)
		PlotCore(iX,iY,Color);
}

DARKSDK void WriteCirclePoints ( int iCX, int iCY, int x, int y, DWORD Color )
{
	Write ( iCX+x, iCY+y, Color );
	Write ( iCX+y, iCY+x, Color );

	Write ( iCX+y, iCY-x, Color );
	Write ( iCX+x, iCY-y, Color );

	Write ( iCX-x, iCY-y, Color );
	Write ( iCX-y, iCY-x, Color );

	Write ( iCX-y, iCY+x, Color );
	Write ( iCX-x, iCY+y, Color );
}

DARKSDK void WriteEllipsePoints ( int iCX, int iCY, int x, int y, DWORD Color )
{
	Write ( iCX+x, iCY+y, Color );
	Write ( iCX+x, iCY-y, Color );
	Write ( iCX-x, iCY-y, Color );
	Write ( iCX-x, iCY+y, Color );
}

DARKSDK void Clear ( DWORD RGBColor )
{
	m_pD3D->Clear ( 1, NULL, D3DCLEAR_TARGET, RGBColor, 0.0f, 0 );
}

DARKSDK bool GetLockable ( void )
{
	return false;
}

DARKSDK void CopyArea(int iDestX, int iDestY, int iWidth, int iHeight, int iSourceX, int iSourceY)
{
	// get surface desc
	D3DSURFACE_DESC surfacedesc;
	IDirect3DSurface9* pSurface = g_pGlob->pCurrentBitmapSurface;
	pSurface->GetDesc ( &surfacedesc );

	// Define areas to copy
	RECT  rcSource = {  iSourceX,  iSourceY, iSourceX+iWidth,  iSourceY+iHeight };

	// Clip to actual surface source size
	int iScrWidth=surfacedesc.Width;
	int iScrHeight=surfacedesc.Height;
	if(rcSource.right>iScrWidth) rcSource.right=iScrWidth;
	if(rcSource.bottom>iScrHeight) rcSource.bottom=iScrHeight;

	// Define destination rect
	RECT rcDestRect;
	rcDestRect.left=iDestX;
	rcDestRect.top=iDestY;
	rcDestRect.right=iDestX+(rcSource.right-rcSource.left);
	rcDestRect.bottom=iDestX+(rcSource.bottom-rcSource.top);

	// Perform copy
	if(m_pD3D)
	{
		// source and dest surface
		if ( pSurface )
		{
			// create temp surface
			IDirect3DSurface9* pTempSurface = NULL;
			m_pD3D->CreateRenderTarget ( surfacedesc.Width, surfacedesc.Height, surfacedesc.Format, D3DMULTISAMPLE_NONE, 0, TRUE, &pTempSurface, NULL );
			if ( pTempSurface )
			{
				// copy over
				m_pD3D->StretchRect(pSurface, NULL, pTempSurface, NULL, D3DTEXF_NONE);
				m_pD3D->StretchRect(pTempSurface, &rcSource, pSurface, &rcDestRect, D3DTEXF_NONE);

				// free surface
				SAFE_RELEASE ( pTempSurface );
			}
		}
	}
}

DARKSDK void Clear ( void )
{
	m_pD3D->Clear ( 0, NULL, D3DCLEAR_TARGET, g_pGlob->dwBackColor, 0.0f, 0 );
}

DARKSDK void ClearRgb ( DWORD RGBBackColor )
{
	g_pGlob->iCursorX=0;
	g_pGlob->iCursorY=0;
	m_pD3D->Clear ( 0, NULL, D3DCLEAR_TARGET, RGBBackColor, 0.0f, 0 );
}

DARKSDK void Ink ( DWORD RGBForeColor, DWORD RGBBackColor )
{
	g_pGlob->dwForeColor = RGBForeColor;
	g_pGlob->dwBackColor = RGBBackColor;
}

DARKSDK DWORD CorrectDotColor ( DWORD dwCol )
{
	// mike - 260604 - return original colour
	return dwCol;

	if(m_dwBPP==2)
	{
		int red =	(int)(((dwCol & (255<<16)) >> 16) / 8.3);
		int green = (int)(((dwCol & (255<<8)) >> 8) / 4.1);
		int blue =	(int)((dwCol & 255) / 8.3);
		if(red>31) red=31;
		if(green>63) green=63;
		if(blue>31) blue=31;
		dwCol = (red<<11)+(green<<5)+(blue);
	}
	return dwCol;
}

DARKSDK void Dot ( int iX, int iY )
{
	DWORD dwCol = CorrectDotColor ( g_pGlob->dwForeColor );

	// mike - 280305 - allow to write over screen boundaries
	if(iX<0) return;
	if(iY<0) return;

	// leefix - 310305 - return code with additional bitmap specific check
	if ( g_pGlob->iCurrentBitmapNumber==0 )
	{
	 	if(iX>g_pGlob->iScreenWidth) return; 
		if(iY>g_pGlob->iScreenHeight) return;
	}

	if ( m_bIsLocked )
	{
		PlotCore(iX,iY,dwCol);
	}
	else
	{
		Lock ( );
		PlotCore(iX,iY,dwCol);
		Unlock ( );
	}
}

DARKSDK void Box ( int iLeft, int iTop, int iRight, int iBottom )
{
	/* This does not work for all NVidia systems, so replace with a render.
	D3DRECT	rect = { iLeft, iTop, iRight, iBottom };
	m_pD3D->Clear ( 1, &rect, D3DCLEAR_TARGET, g_pGlob->dwForeColor, 0.0f, 0 );
	*/

	tagLinesVertexDesc * v = 0;

    if (m_pLineVB)
    	m_pLineVB->Lock( 0, 0, (void**)&v, 0 );

    if (v)
	{
		v[0].x      = (float)iLeft;
        v[0].y      = (float)iBottom;
		v[0].z      = 10.0f;
        v[0].rhw    = 1.0f;
		v[0].dwColour  = g_pGlob->dwForeColor;

		v[1].x      = (float)iLeft;
        v[1].y      = (float)iTop;
		v[1].z      = 10.0f;
        v[1].rhw    = 1.0f;
		v[1].dwColour  = g_pGlob->dwForeColor;

		v[2].x      = (float)iRight;
        v[2].y      = (float)iTop;
		v[2].z      = 10.0f;
        v[2].rhw    = 1.0f;
		v[2].dwColour  = g_pGlob->dwForeColor;

		v[3].x      = (float)iRight;
        v[3].y      = (float)iBottom;
		v[3].z      = 10.0f;
        v[3].rhw    = 1.0f;
		v[3].dwColour  = g_pGlob->dwForeColor;

		m_pLineVB->Unlock();

		m_pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pD3D->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );		

		m_pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pD3D->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		m_pD3D->SetRenderState( D3DRS_ZENABLE, FALSE );

		m_pD3D->SetRenderState( D3DRS_DITHERENABLE, FALSE );
		m_pD3D->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pD3D->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		m_pD3D->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		m_pD3D->SetRenderState( D3DRS_SPECULARENABLE, FALSE );

		m_pD3D->SetRenderState( D3DRS_FOGENABLE, FALSE );
		m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

        DWORD AAEnabled = FALSE;
        m_pD3D->GetRenderState  ( D3DRS_MULTISAMPLEANTIALIAS, &AAEnabled );
        if (AAEnabled)
		    m_pD3D->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
		
		m_pD3D->SetVertexShader( NULL  );
		m_pD3D->SetFVF( D3DFVF_LINES  );
		m_pD3D->SetStreamSource( 0, m_pLineVB, 0, sizeof(tagLinesVertexDesc) );

		m_pD3D->SetTexture( 0, NULL );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

		m_pD3D->DrawPrimitive( D3DPT_TRIANGLEFAN, 0 ,2 );

        if (AAEnabled)
		    m_pD3D->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
	}
}

DARKSDK void Line ( int iXa, int iYa, int iXb, int iYb )
{
	if(m_pLineVB)
	{
		// fill data for line
		tagLinesVertexDesc * lines;
		m_pLineVB->Lock( 0, 0, (void**)&lines, 0 );
		lines [ 0 ].x        = (float)iXa;										// start x point
		lines [ 0 ].y        = (float)iYa;										// start y point
		lines [ 0 ].z        = 0;												// keep this as 0 because we want 2D only
		lines [ 0 ].rhw		 = 1.0f;
		lines [ 0 ].dwColour = g_pGlob->dwForeColor;
		lines [ 1 ].x        = (float)iXb;										// end x point
		lines [ 1 ].y        = (float)iYb;										// end y point
		lines [ 1 ].z        = 0;												// keep this as 0 because we want 2D only
		lines [ 1 ].rhw		 = 1.0f;
		lines [ 1 ].dwColour = g_pGlob->dwForeColor;
		m_pLineVB->Unlock();

		// draw the line
		m_pD3D->SetTexture( 0, NULL  );
		m_pD3D->SetTexture( 1, NULL  );

		// DX8->DX9
		m_pD3D->SetVertexShader( NULL  );
		m_pD3D->SetFVF( D3DFVF_LINES  );
		m_pD3D->SetStreamSource( 0, m_pLineVB, 0, sizeof(tagLinesVertexDesc) );

		m_pD3D->SetRenderState( D3DRS_FOGENABLE, FALSE );
		m_pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		m_pD3D->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	

		m_pD3D->DrawPrimitive( D3DPT_LINELIST, 0 , 1);
	}
}

DARKSDK void Circle ( int iX, int iY, int iRadius )
{
	Lock ( );
	if ( m_bIsLocked )
	{
		int x = 0;
		int y = iRadius;
		int d = 1 - iRadius;

		WriteCirclePoints ( iX, iY, x, y, g_pGlob->dwForeColor );

		while ( y > x )
		{
			if ( d < 0 )
				d += 2 * x;// + 3;
			else
			{
				d += 2 * ( x - y );// + 5;
				y--;
			}

			x++;

			WriteCirclePoints ( iX, iY, x, y, g_pGlob->dwForeColor );
		}
	}
	Unlock ( );
}

DARKSDK void Elipse ( int iX, int iY, int iXRadius, int iYRadius )
{
	Lock ( );
	if ( m_bIsLocked )
	{
		int x = 0;
		int y = iYRadius;
		double a = 0;
		while(a<2.0)
		{
			double ix = sin(a)*iXRadius;
			double iy = cos(a)*iYRadius;
			int newx=(int)ix;
			int newy=(int)iy;
			while(newx!=x || newy!=y)
			{
				if(newx<x) x--;
				if(newx>x) x++;
				if(newy<y) y--;
				if(newy>y) y++;
				WriteEllipsePoints ( iX, iY, x, y, g_pGlob->dwForeColor );
			}
			//a+=0.01;
			
			// mike - 230604 - stop missing pixels
			a+=0.001;
		}
	}
	Unlock ( );
}

DARKSDK DWORD Rgb ( int iRed, int iGreen, int iBlue )
{
	return D3DCOLOR_XRGB(iRed, iGreen, iBlue);
}

DARKSDK int RgbR ( DWORD iRGB )
{
	return (int)((iRGB & 0x00FF0000) >> 16);
}

DARKSDK int RgbG ( DWORD iRGB )
{
	return (int)((iRGB & 0x0000FF00) >> 8);
}

DARKSDK int RgbB ( DWORD iRGB )
{
	return (int)((iRGB & 0x000000FF) );
}

DARKSDK DWORD GetPoint ( int iX, int iY )
{
	DWORD dwColor=0;
	if ( m_bIsLocked )
	{
// LEEFIX - 141102 - 16 bit has no such alpha bites so this is very wrong
//		dwColor=GetPlotCore(iX,iY) & 0x000000FF;
		dwColor=GetPlotCore(iX,iY);
	}
	else
	{
// LEEFIX - 141102 - 16 bit has no such alpha bites so this is very wrong
//		dwColor=GetPlotCore(iX,iY) & 0x000000FF;
		Lock ( );
		dwColor=GetPlotCore(iX,iY);
		Unlock ( );
	}

	int red, green, blue;
	switch(m_dwBPP)
	{
		case 2 :	// Split 16bit(5-6-5) components
					red =		(int)((	((dwColor>>11)	& 31) ) * 8.3);
					green =		(int)((	((dwColor>>5)	& 63) ) * 4.1);
					blue =		(int)((	((dwColor)		& 31) ) * 8.3);
					if(red>255) red=255;
					if(green>255) green=255;
					if(blue>255) blue=255;
					dwColor = D3DCOLOR_XRGB( red, green, blue );
					break;
	}

	// Combine to create final colour value
	return dwColor;
}

DARKSDK void BoxGradient( int iLeft, int iTop, int iRight, int iBottom, DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4 )
{
	// Create a box with a gradient fade from one colour to another
	// using the per vertex colour rendering technique :)

    /*
        20091128 v75 - IRM - http://forum.thegamecreators.com/?m=forum_view&t=161543&b=15
        Using 2 polys for rendering the box does not give correctly shaded output,
        ie the central points colour should be an average of all 4 corners.

        Have switched to using 4 polys with each poly sharing a centre with a calculated colour.
        This can be done as a fan, with 6 vertices and no index buffer.

        Also, lots of code removed from this function, as I have expanded the vertex buffer
        used by the LINE command so that it can also be used for this command, rather than
        building a new vertex buffer each time.
        Because of this, it should be a little faster than it was before.
    */

    // 20091128 v75 - IRM - Calculate the centre vertex position and colour
    float CentreX = (iLeft + iRight) / 2.0f;
    float CentreY = (iTop + iBottom) / 2.0f;

    #define BG_GetAlpha(c) (((c) >> 24) & 0xff)
    #define BG_GetRed(c)   (((c) >> 16) & 0xff)
    #define BG_GetGreen(c) (((c) >> 8)  & 0xff)
    #define BG_GetBlue(c)  ( (c)        & 0xff)

    // Note: The +2 allows colour to be rounded to the nearest match.
    //       If not used, the central colour would tend to be a little too dark.
    DWORD CentreA = (BG_GetAlpha(dw1) + BG_GetAlpha(dw2) + BG_GetAlpha(dw3) + BG_GetAlpha(dw4) + 2) / 4;
    DWORD CentreR = (BG_GetRed(dw1)   + BG_GetRed(dw2)   + BG_GetRed(dw3)   + BG_GetRed(dw4)   + 2) / 4;
    DWORD CentreG = (BG_GetGreen(dw1) + BG_GetGreen(dw2) + BG_GetGreen(dw3) + BG_GetGreen(dw4) + 2) / 4;
    DWORD CentreB = (BG_GetBlue(dw1)  + BG_GetBlue(dw2)  + BG_GetBlue(dw3)  + BG_GetBlue(dw4)  + 2) / 4;
    DWORD CentreCol = D3DCOLOR_ARGB(CentreA, CentreR, CentreG, CentreB);

	tagLinesVertexDesc * v = 0;

    if (m_pLineVB)
    	m_pLineVB->Lock( 0, 0, (void**)&v, 0 );

    if (v)
	{
        // 20091128 v75 - IRM - Fill out the 6 vertices
		v[0].x      = CentreX;
        v[0].y      = CentreY;
		v[0].z      = 10.0f;
        v[0].rhw    = 1.0f;
		v[0].dwColour  = CentreCol;

		v[1].x      = (float)iLeft;
        v[1].y      = (float)iBottom;
		v[1].z      = 10.0f;
        v[1].rhw    = 1.0f;
		v[1].dwColour  = dw1;

		v[2].x      = (float)iLeft;
        v[2].y      = (float)iTop;
		v[2].z      = 10.0f;
        v[2].rhw    = 1.0f;
		v[2].dwColour  = dw2;

		v[3].x      = (float)iRight;
        v[3].y      = (float)iTop;
		v[3].z      = 10.0f;
        v[3].rhw    = 1.0f;
		v[3].dwColour  = dw4;

		v[4].x      = (float)iRight;
        v[4].y      = (float)iBottom;
		v[4].z      = 10.0f;
        v[4].rhw    = 1.0f;
		v[4].dwColour  = dw3;

        v[5].x      = (float)iLeft;
        v[5].y      = (float)iBottom;
		v[5].z      = 10.0f;
        v[5].rhw    = 1.0f;
		v[5].dwColour  = dw1;

		m_pLineVB->Unlock();

		m_pD3D->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		m_pD3D->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );		

		m_pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pD3D->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		m_pD3D->SetRenderState( D3DRS_ZENABLE, FALSE );

		m_pD3D->SetRenderState( D3DRS_DITHERENABLE, FALSE );
		m_pD3D->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pD3D->SetRenderState( D3DRS_COLORVERTEX, TRUE );
		m_pD3D->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
		m_pD3D->SetRenderState( D3DRS_SPECULARENABLE, FALSE );

		m_pD3D->SetRenderState( D3DRS_FOGENABLE, FALSE );
		m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

        // Save AA setting, and disable if active
        DWORD AAEnabled = FALSE;
        m_pD3D->GetRenderState  ( D3DRS_MULTISAMPLEANTIALIAS, &AAEnabled );
        if (AAEnabled)
		    m_pD3D->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
		
		m_pD3D->SetVertexShader( NULL  );
		m_pD3D->SetFVF( D3DFVF_LINES  );
		m_pD3D->SetStreamSource( 0, m_pLineVB, 0, sizeof(tagLinesVertexDesc) );

		m_pD3D->SetTexture( 0, NULL );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

        // 20091128 v75 - IRM - Render as a fan with 4 polys, not a strip with 2 polys
		m_pD3D->DrawPrimitive( D3DPT_TRIANGLEFAN, 0 ,4);

        // Re-enable AA if it was previously enabled
        if (AAEnabled)
		    m_pD3D->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
	}
}

DARKSDK void Dot ( int iX, int iY, DWORD dwColor )
{
	// LEEFIX - 141102 - Added this conevrter function from above DOT function
	dwColor = CorrectDotColor ( dwColor );
	if ( m_bIsLocked )
	{
		PlotCore(iX,iY,dwColor);
	}
	else
	{
		Lock ( );
		PlotCore(iX,iY,dwColor);
		Unlock ( );
	}
}

DARKSDK DWORD GetPixelPtr ( void )
{
	return (DWORD)m_pData;
}

DARKSDK DWORD GetPixelPitch ( void )
{
	return (DWORD)m_LockedRect.Pitch;
}

DARKSDK void Lock ( void )
{
	if ( m_bIsLocked==false )
	{
		// Get current render target surface
		m_pSurface = g_pGlob->pCurrentBitmapSurface;
		if(m_pSurface==NULL)
			return;

		if ( FAILED ( m_pSurface->LockRect ( &m_LockedRect, NULL, 0 ) ) )
			return;

		m_pData = ( DWORD* ) ( m_LockedRect.pBits );

		D3DSURFACE_DESC ddsd;
		m_pSurface->GetDesc(&ddsd);
		m_SurfaceWidth=(int)ddsd.Width;
		m_SurfaceHeight=(int)ddsd.Height;
		m_bIsLocked = true;
	}
}

DARKSDK void Unlock ( void )
{
	if ( m_bIsLocked==true )
	{
		m_pSurface->UnlockRect ( );
		m_bIsLocked = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorBasic2D ( HINSTANCE hInstance )
{
	Constructor( hInstance );
}

void DestructorBasic2D  ( void )
{
	Destructor ( );
}

void SetErrorHandlerBasic2D ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataBasic2D ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DBasic2D  ( int iMode )
{
	RefreshD3D ( iMode );
}

void UpdateBPPBasic2D (void)
{
	UpdateBPP ( );
}

void dbCLS ( DWORD RGBBackColor )
{
	ClearRgb ( RGBBackColor );
}

void dbInk ( DWORD RGBForeColor, DWORD RGBBackColor )
{
	Ink ( RGBForeColor, RGBBackColor );
}

void dbDot ( int iX, int iY )
{
	Dot ( iX, iY );
}

void dbBox ( int iLeft, int iTop, int iRight, int iBottom )
{
	Box ( iLeft, iTop, iRight, iBottom );
}

void dbLine ( int iXa, int iYa, int iXb, int iYb )
{
	Line ( iXa, iYa, iXb, iYb );
}

void dbCircle ( int iX, int iY, int iRadius )
{
	Circle ( iX, iY, iRadius );
}

void dbElipse ( int iX, int iY, int iXRadius, int iYRadius )
{
	Elipse ( iX, iY, iXRadius, iYRadius );
}

DWORD dbRgb ( int iRed, int iGreen, int iBlue )
{
	return Rgb ( iRed, iGreen, iBlue );
}

int dbRgbR ( DWORD iRGB )
{
	return RgbR ( iRGB );
}

int dbRgbG ( DWORD iRGB )
{
	return RgbG ( iRGB );
}

int dbRgbB ( DWORD iRGB )
{
	return RgbB ( iRGB );
}

DWORD dbPoint ( int iX, int iY )
{
	return GetPoint ( iX, iY );
}

void dbDot ( int iX, int iY, DWORD dwColor )
{
	Dot ( iX, iY, dwColor );
}

void dbBox ( int iLeft, int iTop, int iRight, int iBottom, DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4 )
{
	BoxGradient ( iLeft, iTop, iRight, iBottom, dw1, dw2, dw3, dw4 );
}

void dbLockPixels ( void )
{
	Lock ( );
}

void dbUnLockPixels ( void )
{
	Unlock ( );
}

DWORD dbGetPixelsPointer ( void )
{
	return GetPixelPtr ( );
}

DWORD dbGetPixelsPitch ( void )
{
	return GetPixelPitch ( );
}

void dbCopyArea ( int iDestX, int iDestY, int iWidth, int iHeight, int iSourceX, int iSourceY )
{
	CopyArea ( iDestX, iDestY, iWidth, iHeight, iSourceX, iSourceY );
}

// lee - 300706 - GDK fixes
DWORD dbRGB ( int iRed, int iGreen, int iBlue ) { return dbRgb ( iRed, iGreen, iBlue ); }
int	dbRGBR ( DWORD iRGB ) { return dbRgbR ( iRGB ); }
int	dbRGBG ( DWORD iRGB ) { return dbRgbG ( iRGB ); }
int	dbRGBB ( DWORD iRGB ) { return dbRgbB ( iRGB ); }
void dbEllipse ( int iX, int iY, int iXRadius, int iYRadius ) { dbElipse ( iX, iY, iXRadius, iYRadius ); }
void dbUnlockPixels	( void ) { dbUnLockPixels(); }

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////