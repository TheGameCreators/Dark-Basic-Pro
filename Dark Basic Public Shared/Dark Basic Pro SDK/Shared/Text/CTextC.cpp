#include "ctextc.h"
#include "stdio.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	//#include "cpositionc.cpp"
#endif

//////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

typedef IDirect3DDevice9* ( *GFX_GetDirect3DDevicePFN ) ( void );

// main globals
DBPRO_GLOBAL LPDIRECT3D9				m_pDX								= NULL;					// pointer to dx
DBPRO_GLOBAL LPDIRECT3DDEVICE9			m_pD3D								= NULL;					// pointer to direct3d interface
DBPRO_GLOBAL D3DFORMAT					m_FontFormat						= D3DFMT_UNKNOWN;		// default format
DBPRO_GLOBAL TCHAR						m_strFontName [ 80 ]				= "";					// font name
DBPRO_GLOBAL DWORD						m_dwFontHeight						= 0;					// font height
DBPRO_GLOBAL DWORD						m_dwFontFlags						= 0;					// font flags
DBPRO_GLOBAL LPDIRECT3DTEXTURE9			m_pTexture							= NULL;					// texture
DBPRO_GLOBAL LPDIRECT3DVERTEXBUFFER9	m_pVB								= NULL;					// vertex buffer
DBPRO_GLOBAL DWORD						m_dwTexWidth						= 0;					// texture width
DBPRO_GLOBAL DWORD						m_dwTexHeight						= 0;					// texture height
DBPRO_GLOBAL FLOAT						m_fTextScale						= 0.0f;					// text scale
DBPRO_GLOBAL FLOAT						m_fTexCoords [ 256 - 32 ] [ 4 ];							// texture coods
DBPRO_GLOBAL int						m_szTexWidth [ 256 - 32 ];									// letter sizes
DBPRO_GLOBAL int						m_szTexHeight [ 256 - 32 ];									// letter sizes
DBPRO_GLOBAL IDirect3DStateBlock9*		m_pSavedStateBlock					= NULL;					// dx8->dx9
DBPRO_GLOBAL IDirect3DStateBlock9*		m_pDrawTextStateBlock				= NULL;					// state block
DBPRO_GLOBAL tagObjectPos*				m_pPosText								= NULL;					// position pointer
DBPRO_GLOBAL bool						GDI_TEXT							= 0;					// text type
DBPRO_GLOBAL PTR_FuncCreateStr			g_pCreateDeleteStringFunction		= NULL;					// delete string
DBPRO_GLOBAL DWORD						dwDEFAULTCHARSET					= ANSI_CHARSET;			// character set
DBPRO_GLOBAL HFONT						g_hRetainRawTextWriteFont			= NULL;					// raw font
DBPRO_GLOBAL bool						g_bWideCharacterSet					= false;				// unicode

// font properties
DBPRO_GLOBAL DWORD						m_dwColor							= 0;					// colour
DBPRO_GLOBAL DWORD						m_dwBKColor							= 0;					// bk colour
DBPRO_GLOBAL bool						m_bTextBold							= false;				// bold flag
DBPRO_GLOBAL bool						m_bTextItalic						= false;				// italic flag
DBPRO_GLOBAL bool						m_bTextOpaque						= false;				// bold flag
DBPRO_GLOBAL int						m_iTextCharSet						= 0;					// text char set
DBPRO_GLOBAL int						m_iX								= 0;					// x pos
DBPRO_GLOBAL int						m_iY								= 0;					// y pos

// local checklist work vars
DBPRO_GLOBAL bool						g_bCreateChecklistNow				= false;				// create checklist
DBPRO_GLOBAL DWORD						g_dwMaxStringSizeInEnum				= 0;					// maximum string size
DBPRO_GLOBAL DWORD						m_dwWorkStringSize					= 0;					// string size
DBPRO_GLOBAL LPSTR						m_pWorkString						= NULL;					// work string
DBPRO_GLOBAL LPSTR                      m_szTokenString                     = NULL;                 // splitting by token
DBPRO_GLOBAL DWORD                      m_dwTokenStringSize                 = 0;

// function pointers
DBPRO_GLOBAL HINSTANCE					g_GFX								= 0;					// for dll loading
DBPRO_GLOBAL GFX_GetDirect3DDevicePFN	g_GFX_GetDirect3DDevice				= NULL;					// get pointer to D3D device

DBPRO_GLOBAL GlobStruct*				g_pGlob								= NULL;					// glob struct

//////////////////////////////////////////////////////////////////////////


DARKSDK bool UpdatePtr ( int iID )
{
	return true;
}

DARKSDK void ValidateWorkStringBySize ( DWORD dwSize )
{
    // u74b7 - delete m_pWorkString as an array
	// free string that is too small
	if ( m_pWorkString )
		if ( m_dwWorkStringSize<dwSize )
			SAFE_DELETE_ARRAY(m_pWorkString);

	// create new work string of good size
	if ( m_pWorkString==NULL )
	{
		m_dwWorkStringSize = dwSize+1;
		m_pWorkString = new char[m_dwWorkStringSize];
		memset ( m_pWorkString, 0, m_dwWorkStringSize );
	}
}

DARKSDK void ValidateWorkString(LPSTR pString)
{
	// Size from string
	if ( pString ) ValidateWorkStringBySize ( strlen(pString)+1 );
}

DARKSDK void ValidateDefaultTextureForFont(void)
{
	// Would be a good idea to have a format-collector function for what surface, depth, texture formats we can use from the card and put into glob..
	// (taken form image DLL - perhaps merge them at some point in setup DLL)

	// Get default d3dformat from backbuffer
	D3DSURFACE_DESC backdesc;
	LPDIRECT3DSURFACE9 pBackBuffer = g_pGlob->pCurrentBitmapSurface;
	if(pBackBuffer) pBackBuffer->GetDesc(&backdesc);
	m_FontFormat = D3DFMT_A4R4G4B4;
	
	// Ensure textureformat is valid, else choose next valid..
	HRESULT hRes = m_pDX->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
												D3DDEVTYPE_HAL,
												backdesc.Format,
												0, D3DRTYPE_TEXTURE,
												m_FontFormat);
	if ( FAILED( hRes ) )
	{
		// Need another texture format with an alpha
		for(DWORD t=0; t<12; t++)
		{
			switch(t)
			{
				case 0  : m_FontFormat = D3DFMT_A4R4G4B4;		break;
				case 1  : m_FontFormat = D3DFMT_A8R3G3B2;		break;
				case 2 : m_FontFormat = D3DFMT_A1R5G5B5;		break;
				case 3 : m_FontFormat = D3DFMT_X1R5G5B5;		break;
				case 4 : m_FontFormat = D3DFMT_A8R8G8B8;		break;
				case 5 : m_FontFormat = D3DFMT_A2B10G10R10;		break;
				case 6 : m_FontFormat = D3DFMT_X8R8G8B8;		break;
				case 7 : m_FontFormat = D3DFMT_R8G8B8;			break;
				case 8 : m_FontFormat = D3DFMT_R5G6B5;			break;
				case 9 : m_FontFormat = D3DFMT_R3G3B2;			break;
				case 10 : m_FontFormat = D3DFMT_X4R4G4B4;		break;
				case 11 : m_FontFormat = D3DFMT_G16R16;			break;
			}
			HRESULT hRes = m_pDX->CheckDeviceFormat(	D3DADAPTER_DEFAULT,
														D3DDEVTYPE_HAL,
														backdesc.Format,
														0, D3DRTYPE_TEXTURE,
														m_FontFormat);
			if ( SUCCEEDED( hRes ) )
			{
				// Found a texture we can use
				return;
			}
		}
	}
}

DARKSDK void TextConstructor ( HINSTANCE hSetup )
{
	#ifndef DARKSDK_COMPILE
	if ( !hSetup )
	{
		// attempt to load the DLLs manually
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );
	}
	#endif

	#ifndef DARKSDK_COMPILE
	{
		// Assign Setup DLL Handle
		g_GFX = hSetup;

		// check the dll was loaded
		if ( !g_GFX )
			Error ( "Cannot load setup library for text.dll" );
	}
	#endif

	// setup the function pointer and then call it to get a pointer to the direct3d interface
	#ifndef DARKSDK_COMPILE
	{
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( g_GFX, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	}
	#else
	{
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;
	}
	#endif

	m_pD3D = g_GFX_GetDirect3DDevice ( );


	// get reference to DX
	m_pD3D->GetDirect3D(&m_pDX);

	// now setup font construction using preset defaults
	//strcpy ( m_strFontName, "arial" );			// attempt to use arial font
	// mike - 250604 - default to fixedsys font
	strcpy ( m_strFontName, "fixedsys" );			// attempt to use arial font
    m_dwFontHeight         = 12;				// use point size 12
	m_dwFontFlags          = 0;					// no flags initially
	m_fTextScale		   = 1.0f;				// normal scale
	m_iTextCharSet		   = 0;

    m_pTexture             = NULL;				// set texture pointer to null
    m_pVB                  = NULL;				// set vertex buffer to null

	// create state blocks
	m_pD3D->CreateStateBlock  ( D3DSBT_ALL, &m_pSavedStateBlock );
	m_pD3D->CreateStateBlock  ( D3DSBT_ALL, &m_pDrawTextStateBlock );

	m_bTextBold			   = false;									// turn bold off
	m_bTextItalic		   = false;									// turn italic off
   	m_bTextOpaque		   = false;									// text opaque
	m_dwColor			   = D3DCOLOR_ARGB ( 255, 255, 255, 255 );	// set colour to white
	m_dwBKColor			   = D3DCOLOR_ARGB ( 255, 0, 0, 0 );		// set colour to black
	m_iX			       = 0;										// initial x pos
	m_iY			       = 0;										// initial y pos

	// setup position
	SAFE_DELETE(m_pPosText);
	m_pPosText = new tagObjectPos;
	memset ( m_pPosText, 0, sizeof ( tagObjectPos ) );

	if ( !m_pPosText )
		Error ( "Unable to allocate memory for positional data in text library" );

	// now setup the font to be rendered - dbpro does it in passcore
	// leefix - 300305 - moved to passcore when we have the glob (do not do in DLL)
//	SetupFont   ( );
//	SetupStates ( );

	// prepare work string
	ValidateWorkStringBySize ( 256 );

    // u74b7 - if already have a token temp area, leave it alone.
    if (! m_szTokenString )
    {
        m_dwTokenStringSize = 100;
        m_szTokenString = new char[ 100 ];
		memset ( m_szTokenString, 0, sizeof(m_szTokenString) );
    }
}

DARKSDK int RgbR ( DWORD iRGB )
{
	return (int)((iRGB & 0x00FF0000) >> 16);
}
DARKSDK  int RgbG ( DWORD iRGB )
{
	return (int)((iRGB & 0x0000FF00) >> 8);
}
DARKSDK  int RgbB ( DWORD iRGB )
{
	return (int)((iRGB & 0x000000FF) );
}

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

// Realtext Functions
DARKSDK HFONT DB_SetRealTextFont(HDC hdc, DWORD textstyle, DWORD bItalicFlag, int fontsize, char* fontname, int inter)
{
	HFONT hFont = NULL;
	if(inter==0)
	{
		// Get Real(Available) Font Height Used
		if(dwDEFAULTCHARSET==ANSI_CHARSET)
		{
			hFont = CreateFont( (int)(fontsize*m_fTextScale), 0, 0, 0, textstyle, bItalicFlag, FALSE, FALSE, dwDEFAULTCHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY,VARIABLE_PITCH, fontname);
		}
		else
		{
			int FontHeight=0;
			FontHeight=-MulDiv(fontsize, (int)(GetDeviceCaps(hdc, LOGPIXELSY) * m_fTextScale), 72); 
			hFont = CreateFont( FontHeight, 0, 0, 0, textstyle, bItalicFlag, FALSE, FALSE, dwDEFAULTCHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY,VARIABLE_PITCH, fontname);
			
		}
	}
	else
	{
		hFont = CreateFont(	fontsize, 0, 0, 0,
									textstyle, bItalicFlag, FALSE, FALSE,
									inter,
									OUT_DEFAULT_PRECIS,
									CLIP_DEFAULT_PRECIS,
									NONANTIALIASED_QUALITY,
									VARIABLE_PITCH,
									fontname);

		if(dwDEFAULTCHARSET!=(DWORD)inter)
		{
			dwDEFAULTCHARSET=inter;
		}
	}

	// work out if wide character text (japanese, korean, chinese/trad)
	if ( dwDEFAULTCHARSET==128 || dwDEFAULTCHARSET==129 || dwDEFAULTCHARSET==134 || dwDEFAULTCHARSET==136 ) 
		g_bWideCharacterSet = true;
	else
		g_bWideCharacterSet = false;

	// return font handle
	return hFont;
}

DARKSDK void SetupFont ( void )
{
	if ( GDI_TEXT == false )
	{
		// variable definitions
		HRESULT		hr;				// used for error checking
		D3DCAPS9	d3dCaps;		// device capabilities structure
		
		DWORD*      pBitmapBits;	// pointer to bitmap data
		BITMAPINFO	bmi;			// bitmap info structure
		HDC			hDC;			// handle to device context
		HBITMAP		hbmBitmap;		// handle to bitmap
		HFONT		hFont;			// handle to font

		DWORD		x = 0;
		DWORD		y = 0;
		TCHAR		str [ 2 ] = ( "x" );
		SIZE		size;

		D3DLOCKED_RECT	d3dlr;
		WORD*			pDst16;
		BYTE			bAlpha;

		// leefix - 300205 - texture plate size isdependant on the size of the font
		hDC					= CreateCompatibleDC ( NULL );	
		SetMapMode ( hDC, MM_TEXT );																		// set mapping mode x, y
		DWORD textstyle		= ( m_bTextBold   ) ? FW_BOLD : FW_NORMAL;		// set bold flag on / off
		DWORD bItalicFlag	= ( m_bTextItalic ) ? TRUE    : FALSE;			// set italic flag on / off
		hFont = DB_SetRealTextFont ( hDC, textstyle, bItalicFlag, m_dwFontHeight, m_strFontName, m_iTextCharSet );
		HFONT hbmOldFont = (HFONT)SelectObject ( hDC, hFont );
		SetTextAlign ( hDC, TA_TOP );
		GetTextExtentPoint32 ( hDC, "g", 1, &size );

		// leefix - 300305 40, 20, X was not right (extended chars left 256 texture plate)
		DWORD dwActualUsageHeight = size.cy;
		if ( dwActualUsageHeight > 40 )
			m_dwTexWidth = m_dwTexHeight = 1024;
		else 
			if ( dwActualUsageHeight > 15 )
				m_dwTexWidth = m_dwTexHeight = 512;
			else
				m_dwTexWidth = m_dwTexHeight = 256;

		// now check that it's ok for us to create
		// a texture at the height and width, get
		// the device caps to do this
		m_pD3D->GetDeviceCaps ( &d3dCaps );

		// if the texture is too big we simply
		// create a smaller texture and scale
		// the final font
		m_fTextScale = 1.0f;
		if ( m_dwTexWidth > d3dCaps.MaxTextureWidth )
		{
			m_fTextScale = ( float ) d3dCaps.MaxTextureWidth / ( float ) m_dwTexWidth;
			m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
		}

		// create a blank texture for the font
		if ( FAILED ( hr = m_pD3D->CreateTexture ( m_dwTexWidth, m_dwTexHeight, 1, 0, m_FontFormat, D3DPOOL_MANAGED, &m_pTexture, NULL ) ) )
			Error ( "Unable to create font texture in text library" );
    
		// and now create a bitmap, what we do is draw
		// a font onto the bitmap and then later on
		// copy it onto the texture. by doing this
		// we draw loads of 3d objects at runtime 
		// instead of having to lock buffers and copy
		// text which is faster and enables us to have
		// 3d text
		memset ( &bmi.bmiHeader, 0, sizeof ( BITMAPINFOHEADER ) );		// clear out the header
		bmi.bmiHeader.biSize        = sizeof ( BITMAPINFOHEADER );		// set the size
		bmi.bmiHeader.biWidth       =  ( int ) m_dwTexWidth;			// set the bmp width
		bmi.bmiHeader.biHeight      = -( int ) m_dwTexHeight;			// set the bmp height
		bmi.bmiHeader.biPlanes      = 1;								// set planes ( always 1 )
		bmi.bmiHeader.biCompression = BI_RGB;							// no compression
		bmi.bmiHeader.biBitCount    = 32;								// bit depth

		// create a device context and a bitmap for the font
//		hDC       = CreateCompatibleDC ( NULL );															// create dc
		hbmBitmap = CreateDIBSection   ( hDC, &bmi, DIB_RGB_COLORS, ( VOID** ) &pBitmapBits, NULL, 0 );		// create bmp
//		SetMapMode ( hDC, MM_TEXT );																		// set mapping mode x, y


//		// at this point we can now create a font
//		DWORD textstyle   = ( m_bTextBold   ) ? FW_BOLD : FW_NORMAL;		// set bold flag on / off
//		DWORD bItalicFlag = ( m_bTextItalic ) ? TRUE    : FALSE;			// set italic flag on / off
//		hFont = DB_SetRealTextFont ( hDC, textstyle, bItalicFlag, m_dwFontHeight, m_strFontName, m_iTextCharSet );

		/*
		nHeight = -MulDiv ( m_dwFontHeight, ( int ) ( GetDeviceCaps ( hDC, LOGPIXELSY ) * m_fTextScale ), 72 );	// get font height
		dwBold   = ( m_bTextBold   ) ? FW_BOLD : FW_NORMAL;		// set bold flag on / off
		dwItalic = ( m_bTextItalic ) ? true    : false;			// set italic flag on / off

		// call create font
		hFont    = CreateFont 
							( 
								nHeight,					// logical height of font
								0,							// logical average width
								0,							// angle of escapment
								0,							// base line orientation
								dwBold,						// font weight bold / normal
								dwItalic,					// italic flag
								false,						// underline flag
								false,						// strikeout flag
								DEFAULT_CHARSET,			// character set
								OUT_DEFAULT_PRECIS,			// default output precision
								CLIP_DEFAULT_PRECIS,		// default clipping precision
								ANTIALIASED_QUALITY,		// ask for antialiased quality ( not always possible )
								VARIABLE_PITCH,				// variable pitch
								m_strFontName				// font name
							);

		// check font was created
		if ( hFont == NULL )
			Error ( "Unable to create font in text library" );
		*/

		// select objects into dc
		HBITMAP hbmOldBitmap = (HBITMAP)SelectObject ( hDC, hbmBitmap );

		// Colour is merely a mask maker, actual colour is later in code
		SetTextColor ( hDC, RGB ( 255, 255, 255 ) );
		SetBkColor   ( hDC, 0x00000000 );				// background colour, black by default

		// set the text properties
//		SetTextAlign ( hDC, TA_TOP );					// align to top by default
		
		// loop through all printable character and output them to the bitmap
		// also keep track of the corresponding tex coords for each character
		// LEEFIX - 141102 - Increased font chars to 256 (to include special ascii chars)
		for ( unsigned short c = 32; c < 256; c++ )
		{
			str [ 0 ] = (unsigned char)c;

			GetTextExtentPoint32 ( hDC, str, 1, &size );

			DWORD dwPush = 0;
			DWORD dwAdditional = 0;
			if(m_bTextItalic) 
			{
				dwPush = (size.cy/8);
				dwAdditional = (size.cy/4);//little edge of italic text

				//size.cx+=dwAdditional;

				// mike - 250604 - adjust for italics
				size.cx+=m_dwFontHeight / 3 + 5;
			}

			if ( ( DWORD ) ( x + size.cx + 1 ) > m_dwTexWidth )
			{
				x  = 0;
				y += size.cy + 2;
			}

			ExtTextOut ( hDC, x + dwPush + 0, y + 0, ETO_OPAQUE, NULL, str, 1, NULL );

			m_fTexCoords [ c - 32 ] [ 0 ] = ( ( float ) ( x + 0           ) ) / m_dwTexWidth;
			m_fTexCoords [ c - 32 ] [ 1 ] = ( ( float ) ( y + 0           ) ) / m_dwTexHeight;
			m_fTexCoords [ c - 32 ] [ 2 ] = ( ( float ) ( x + 0 + size.cx + 1 ) ) / m_dwTexWidth;
			m_fTexCoords [ c - 32 ] [ 3 ] = ( ( float ) ( y + 0 + size.cy + 1 ) ) / m_dwTexHeight;
			m_szTexWidth [ c - 32 ] = (int)((float)(size.cx - dwAdditional) / m_fTextScale);
			m_szTexHeight [ c - 32 ] = size.cy;

			// mike - 250704 - use this for foreign language DB Pro text
			if ( m_iTextCharSet != 0 )
				x += size.cx + 2;
			else
				x += size.cx + 16;
		}

		// lock the surface (for 16bit format - 4444)
		DWORD dwDepth=GetBitDepthFromFormat(m_FontFormat);
		if(dwDepth==16)
		{
			DWORD dwUseBack = m_dwBKColor;
			if(m_bTextOpaque==false) dwUseBack=0;

			m_pTexture->LockRect ( 0, &d3dlr, 0, 0 );
			pDst16 = ( WORD* ) d3dlr.pBits;
			WORD wFore = 0x0fff;
			WORD wBack = 0x0000;
			if(m_dwBKColor>0)
			{
				BYTE bFR = RgbR(m_dwColor)/16; if(bFR>15) bFR=15;
				BYTE bFG = RgbG(m_dwColor)/16; if(bFG>15) bFG=15;
				BYTE bFB = RgbB(m_dwColor)/16; if(bFB>15) bFB=15;
				BYTE bBR = RgbR(dwUseBack)/16; if(bFR>15) bFR=15;
				BYTE bBG = RgbG(dwUseBack)/16; if(bBG>15) bBG=15;
				BYTE bBB = RgbB(dwUseBack)/16; if(bBB>15) bBB=15;
				wFore = (bFR<<8) + (bFG<<4) + (bFB);
				wBack = (bBR<<8) + (bBG<<4) + (bBB);
			}
			for ( y = 0; y < m_dwTexHeight; y++ )
			{
				for ( x = 0; x < m_dwTexWidth; x++ )
				{
					bAlpha = ( BYTE ) ( ( pBitmapBits [ m_dwTexWidth * y + x ] & 0xff ) >> 4 );
					
					if ( bAlpha > 0 )
						*pDst16++ = ( bAlpha << 12 ) | wFore;
					else
						*pDst16++ = wBack;
				}
			}
			// unlock the texture
			m_pTexture->UnlockRect ( 0 );
		}

		// select out objects
		SelectObject ( hDC, hbmOldBitmap );
		SelectObject ( hDC, hbmOldFont );

		// store font, deleting any old one
		if ( g_hRetainRawTextWriteFont ) DeleteObject ( g_hRetainRawTextWriteFont );
		g_hRetainRawTextWriteFont = hFont;

		// now clean up objects
		DeleteObject ( hbmBitmap );
		DeleteDC     ( hDC );
	}

	if ( GDI_TEXT == true )
	{
		HDC			hDC;			// handle to device context
		HFONT		hFont;			// handle to font
		SIZE		size;

		LPDIRECT3DSURFACE9 pBackBuffer = g_pGlob->pCurrentBitmapSurface;
		pBackBuffer->GetDC ( &hDC );

		SetMapMode ( hDC, MM_TEXT );

		DWORD textstyle   = ( m_bTextBold   ) ? FW_BOLD : FW_NORMAL;		// set bold flag on / off
		DWORD bItalicFlag = ( m_bTextItalic ) ? TRUE    : FALSE;			// set italic flag on / off
		hFont = DB_SetRealTextFont ( hDC, textstyle, bItalicFlag, m_dwFontHeight, m_strFontName, m_iTextCharSet );

		SelectObject ( hDC, hFont );
		
		// store font, deleting any old one

		if ( g_hRetainRawTextWriteFont ) DeleteObject ( g_hRetainRawTextWriteFont );
		g_hRetainRawTextWriteFont = hFont;

		TCHAR str [ 2 ] = ( "x" );

		m_fTextScale = 1.0f;

		for ( unsigned short c = 32; c < 256; c++ )
		{
			str [ 0 ] = (unsigned char)c;

			GetTextExtentPoint32 ( hDC, str, 1, &size );

			m_szTexWidth  [ c - 32 ] = (int)((float)(size.cx) / m_fTextScale);
			m_szTexHeight [ c - 32 ] = m_dwFontHeight;
		}
		
		pBackBuffer->ReleaseDC ( hDC );
	}
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
	g_pCreateDeleteStringFunction = g_pGlob->CreateDeleteString;

	// Choose bext texture and create font and state now
	ValidateDefaultTextureForFont();
	SetupFont   ( );
	SetupStates ( );
}

DARKSDK void DeleteFonts ( void )
{
	if ( m_pTexture )
	{
		m_pTexture->Release ( );
		m_pTexture = NULL;
	}
}

DARKSDK void DeleteStates ( void )
{
	if ( m_pVB )
	{
		m_pVB->Release ( );
		m_pVB = NULL;
	}
}

DARKSDK void TextDestructor ( void )
{
    // u74b7 - delete token temp string
	SAFE_DELETE_ARRAY( m_szTokenString );
    // u74b7 - delete m_pWorkString as an array
	SAFE_DELETE_ARRAY( m_pWorkString );

	SAFE_DELETE(m_pPosText);
	DeleteFonts();
	DeleteStates();

	// delete widecharacter font
	if ( g_hRetainRawTextWriteFont )
	{
		DeleteObject ( g_hRetainRawTextWriteFont );
		g_hRetainRawTextWriteFont=NULL;
	}

	// Release state blocks
	if ( m_pSavedStateBlock )
	{
		m_pSavedStateBlock->Release();
		m_pSavedStateBlock=NULL;
	}
	if ( m_pDrawTextStateBlock )
	{
		m_pDrawTextStateBlock->Release();
		m_pDrawTextStateBlock=NULL;
	}

	// Release references
	if(m_pDX)
	{
		m_pDX->Release();
		m_pDX=NULL;
	}
}

DARKSDK void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		TextDestructor();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		TextConstructor ( g_pGlob->g_GFX );
		PassCoreData ( g_pGlob );
	}
}

DARKSDK void SetTextColor ( int iAlpha, int iRed, int iGreen, int iBlue )
{
	m_dwColor = D3DCOLOR_ARGB ( iAlpha, iRed, iGreen, iBlue );
}

DARKSDK void SetupStates ( void )
{
	HRESULT hr;

    // create vertex buffer
    if ( FAILED ( hr = m_pD3D->CreateVertexBuffer ( MAX_NUM_VERTICES * sizeof ( FONT2DVERTEX ), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
        Error ( "Unable to create vertex buffer for text library" );
    
	// leefix-060803-release stateblocks
	SAFE_RELEASE ( m_pSavedStateBlock );
	SAFE_RELEASE ( m_pDrawTextStateBlock );

	// create the state blocks for rendering text
    for ( UINT which = 0; which < 2; which++ )
    {
        m_pD3D->BeginStateBlock ( );

        m_pD3D->SetTexture ( 0, m_pTexture );

		// Text can be transparent
		if(m_bTextOpaque==true)
		{
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,			FALSE );
		}
		else
		{
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,			TRUE );
		}
        m_pD3D->SetRenderState ( D3DRS_SRCBLEND,					D3DBLEND_SRCALPHA );
        m_pD3D->SetRenderState ( D3DRS_DESTBLEND,					D3DBLEND_INVSRCALPHA );
        m_pD3D->SetRenderState ( D3DRS_FILLMODE,					D3DFILL_SOLID );
        m_pD3D->SetRenderState ( D3DRS_CULLMODE,					D3DCULL_CCW );
        m_pD3D->SetRenderState ( D3DRS_ZENABLE,						FALSE );
        m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,				FALSE );
        m_pD3D->SetRenderState ( D3DRS_CLIPPING,					TRUE );
        m_pD3D->SetRenderState ( D3DRS_VERTEXBLEND,					FALSE );
        m_pD3D->SetRenderState ( D3DRS_INDEXEDVERTEXBLENDENABLE,	FALSE );
        m_pD3D->SetRenderState ( D3DRS_FOGENABLE,					FALSE );

        m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,				D3DTOP_MODULATE );
        m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1,				D3DTA_TEXTURE );
        m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2,				D3DTA_DIFFUSE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,				D3DTOP_MODULATE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,				D3DTA_TEXTURE );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,				D3DTA_DIFFUSE );

		m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER,					D3DTEXF_LINEAR );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER,					D3DTEXF_LINEAR );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER,					D3DTEXF_POINT );//D3DTEXF_NONE );

        m_pD3D->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX,			0 );
        m_pD3D->SetTextureStageState ( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
        m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,				D3DTOP_DISABLE );

		// leefix-060803-recreate stateblocks
        if ( which == 0 )
            m_pD3D->EndStateBlock ( &m_pSavedStateBlock );
        else
            m_pD3D->EndStateBlock ( &m_pDrawTextStateBlock );
    }
}

DARKSDK void Recreate ( void )
{
	if ( GDI_TEXT == false )
	{
		DeleteFonts();
		DeleteStates();
		SetupFont ( );
		SetupStates ( );
	}
	

	if ( GDI_TEXT == true )
	{
		DeleteFonts();
		SetupFont ( );
	}

	/*
	DeleteFonts();
	DeleteStates();
	SetupFont ( );
	SetupStates ( );
	*/
}

DARKSDK void GetCullDataFromModel(int)
{
}

DARKSDK int CALLBACK EnumFontFamProc(ENUMLOGFONT FAR *lpelf, NEWTEXTMETRIC FAR *lpntm, int FontType, LPARAM lParam )
{
	LPSTR pFontName=lpelf->elfLogFont.lfFaceName;
	if(pFontName)
	{
		DWORD dwSize=strlen(pFontName)+1;
		if(dwSize>g_dwMaxStringSizeInEnum) g_dwMaxStringSizeInEnum=dwSize;
		if(g_bCreateChecklistNow)
		{
			// New checklist item
			strcpy(g_pGlob->checklist[g_pGlob->checklistqty].string, pFontName);
		}
		g_pGlob->checklistqty++;
	}
	return 1;
}

DARKSDK void Text ( int iX, int iY, char* szText )
{
	// only if string given
	if(szText==NULL)
		return;

	if ( GDI_TEXT == true )
	{
		HDC hDC;
		LPDIRECT3DSURFACE9 pBackBuffer = g_pGlob->pCurrentBitmapSurface;
		pBackBuffer->GetDC ( &hDC );
		HFONT hbmOldFont = (HFONT)SelectObject ( hDC, g_hRetainRawTextWriteFont );
		SetTextColor ( hDC, RGB ( RgbR(m_dwColor),RgbG(m_dwColor),RgbB(m_dwColor) ) );
		SetBkColor   ( hDC, RGB ( RgbR(m_dwBKColor),RgbG(m_dwBKColor),RgbB(m_dwBKColor) ) );
		SetTextAlign ( hDC, TA_LEFT );
		DWORD dwOpaqueMode = ETO_OPAQUE;
		if ( m_bTextOpaque==false )
		{
			SetBkMode ( hDC, TRANSPARENT );
			dwOpaqueMode=0;
		}
		else
			SetBkMode ( hDC, OPAQUE );

        ExtTextOut ( hDC, iX, iY, dwOpaqueMode, NULL, szText, strlen(szText), NULL );
		SelectObject ( hDC, hbmOldFont );
		pBackBuffer->ReleaseDC ( hDC );

		// complete
		return;
	}

	// leeadd - 160204 - use simple GDI textwrite if using wide character (for now)
	// if ( g_bWideCharacterSet && g_hRetainRawTextWriteFont )
	// U73 - 240309 - widecharacter text not drawing to camera render targets, so only do code below for direct
	bool bRenderingToCamera = false;
	if ( g_pGlob->iCurrentBitmapNumber==0 && g_pGlob->pCurrentBitmapSurface!=g_pGlob->pHoldBackBufferPtr ) bRenderingToCamera = true;
	if ( g_bWideCharacterSet && g_hRetainRawTextWriteFont && bRenderingToCamera==false )
	{
		// direct write of wide character text
		HDC hDC;
		LPDIRECT3DSURFACE9 pBackBuffer = g_pGlob->pCurrentBitmapSurface;
		pBackBuffer->GetDC ( &hDC );
	    HFONT hbmOldFont = (HFONT)SelectObject ( hDC, g_hRetainRawTextWriteFont );
		SetTextColor ( hDC, RGB ( RgbR(m_dwColor),RgbG(m_dwColor),RgbB(m_dwColor) ) );
		SetBkColor   ( hDC, RGB ( RgbR(m_dwBKColor),RgbG(m_dwBKColor),RgbB(m_dwBKColor) ) );
		SetTextAlign ( hDC, TA_TOP );
		DWORD dwOpaqueMode = ETO_OPAQUE;
		if ( m_bTextOpaque==false )
		{
			SetBkMode ( hDC, TRANSPARENT );
			dwOpaqueMode=0;
		}
		else
			SetBkMode ( hDC, OPAQUE );

        ExtTextOut ( hDC, iX, iY, dwOpaqueMode, NULL, szText, strlen(szText), NULL );
		SelectObject ( hDC, hbmOldFont );
		pBackBuffer->ReleaseDC ( hDC );

		// complete
		return;
	}

	float			fStartX			= (float)iX;
	float			fX				= (float)iX;
	float			fY				= (float)iY;

	FONT2DVERTEX*	pVertices		= NULL;
    DWORD			dwNumTriangles	= 0;

	// setup renderstates
//    m_pD3D->CaptureStateBlock ( m_dwSavedStateBlock );
//    m_pD3D->ApplyStateBlock   ( m_dwDrawTextStateBlock );
	m_pSavedStateBlock->Capture();
	m_pDrawTextStateBlock->Apply();

    m_pD3D->SetVertexShader   ( NULL );
    m_pD3D->SetFVF   ( D3DFVF_FONT2DVERTEX );

    m_pD3D->SetStreamSource   ( 0, m_pVB, 0, sizeof ( FONT2DVERTEX ) );

	// fill vertex buffer
    m_pVB->Lock ( 0, 0, ( VOID** )&pVertices, D3DLOCK_DISCARD );

    while ( *(LPSTR)szText )
    {
        unsigned char c = *(LPSTR)szText++;

        if ( c == ('\n') )
        {
            fX  = fStartX;
            fY += ( m_fTexCoords [ 0 ] [ 3 ] - m_fTexCoords [ 0 ] [ 1 ] ) * m_dwTexHeight;
        }
        
		if ( c < (' ') )
            continue;

        FLOAT tx1 = m_fTexCoords [ c - 32 ] [ 0 ];
        FLOAT ty1 = m_fTexCoords [ c - 32 ] [ 1 ];
        FLOAT tx2 = m_fTexCoords [ c - 32 ] [ 2 ];
        FLOAT ty2 = m_fTexCoords [ c - 32 ] [ 3 ];

        FLOAT w = ( tx2 - tx1 ) * m_dwTexWidth  / m_fTextScale;
        FLOAT h = ( ty2 - ty1 ) * m_dwTexHeight / m_fTextScale;

		DWORD dwUseActualVertexColour=m_dwColor;
		if(m_dwBKColor>0) dwUseActualVertexColour=D3DCOLOR_ARGB ( 255, 255, 255, 255 );
		if(m_dwBKColor==0 && m_dwColor==0) dwUseActualVertexColour=D3DCOLOR_ARGB ( 255, 1, 1, 1 );
        *pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( fX + 0 - 0.5f, fY + h -0.5f, 0.9f, 1.0f ), dwUseActualVertexColour, tx1, ty2 );
        *pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( fX + 0 - 0.5f, fY + 0 -0.5f, 0.9f, 1.0f ), dwUseActualVertexColour, tx1, ty1 );
        *pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( fX + w - 0.5f, fY + h -0.5f, 0.9f, 1.0f ), dwUseActualVertexColour, tx2, ty2 );
        *pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( fX + w - 0.5f, fY + 0 -0.5f, 0.9f, 1.0f ), dwUseActualVertexColour, tx2, ty1 );
        *pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( fX + w - 0.5f, fY + h -0.5f, 0.9f, 1.0f ), dwUseActualVertexColour, tx2, ty2 );
        *pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( fX + 0 - 0.5f, fY + 0 -0.5f, 0.9f, 1.0f ), dwUseActualVertexColour, tx1, ty1 );

        dwNumTriangles += 2;

        if ( dwNumTriangles * 3 > ( MAX_NUM_VERTICES - 6 ) )
        {
            // unlock, render, and relock the vertex buffer
            m_pVB->Unlock ( );

            m_pD3D->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

            pVertices = NULL;

            m_pVB->Lock ( 0, 0, ( VOID** ) &pVertices, D3DLOCK_DISCARD );
            dwNumTriangles = 0L;
        }

        fX += m_szTexWidth [ c - 32 ];
    }

	// unlock and render the vertex buffer
    m_pVB->Unlock();

    if ( dwNumTriangles > 0 )
        m_pD3D->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // restore the modified renderstates
//    m_pD3D->ApplyStateBlock ( m_dwSavedStateBlock );
	m_pSavedStateBlock->Apply();
}

DARKSDK void SetTextFont ( char* szTypeface, int iCharacterSet )
{
	// If not setup, exit
	if ( GDI_TEXT == false )
	{
		if(m_pTexture==NULL) return;
	}


	memset ( m_strFontName, 0, sizeof ( m_strFontName ) );
	memcpy ( m_strFontName, (LPSTR)szTypeface, sizeof ( char ) * strlen ( (LPSTR)szTypeface ) );
	m_iTextCharSet = iCharacterSet;
	Recreate ( );
}

/*
void SetCursor ( int iX, int iY )
{
	m_iX = iX;
	m_iY = iY;
}

void Print ( char* szText )
{
	if(szText)
	{
		Text ( m_iX, m_iY, szText );
	}
}
*/

DARKSDK  LPSTR GetReturnStringFromWorkString(void)
{
	LPSTR pReturnString=NULL;
	if(m_pWorkString)
	{
		DWORD dwSize=strlen(m_pWorkString);
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, dwSize+1);
		strcpy(pReturnString, m_pWorkString);
	}
	return pReturnString;
}

DARKSDK int GetTextWidth ( char* szString )
{
	int iWidth=0;
	if(szString)
	{
		while ( *(LPSTR)szString )
		{
			unsigned char c = *(LPSTR)szString++;
			if ( c>=32 ) iWidth+=m_szTexWidth [ c - 32 ];
		}
	}
	return iWidth;
}

DARKSDK int GetTextHeight ( char* szString )
{
	int iHeight=0;
	if(szString)
	{
		while ( *(LPSTR)szString )
		{
			unsigned char c = *(LPSTR)szString++;
			if ( c>=32 )
			{
				int iThisH = m_szTexHeight [ c - 32 ];
				if(iThisH>iHeight) iHeight=iThisH;
			}
		}
	}
	return iHeight;
}

//
// General String Command Functions
//

DARKSDK int	  Asc	( DWORD dwSrcStr )
{
	if(dwSrcStr)
		return (int)*(unsigned char*)dwSrcStr;
	else
		return 0;
}

DARKSDK DWORD Bin	( DWORD pDestStr, int iValue )
{
	// Work string
	LPSTR text=m_pWorkString;
	int t=0;
	text[t++]='%';
	for(int bit=31; bit>=0; bit--)
	{
		unsigned int mask = 1 << bit;
		if(iValue & mask)
			text[t++]='1';
		else
			text[t++]='0';
	}
	text[t]=0;

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD	  Chr	( DWORD pDestStr, int iValue )
{
	// Work string
	m_pWorkString[0]=iValue;
	m_pWorkString[1]=0;

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD Hex	( DWORD pDestStr, int iValue )
{
	// Work string
	wsprintf(m_pWorkString, "%X", iValue);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD Left	( DWORD pDestStr, DWORD szText, int iValue )
{
	// lee - 290306 - u6rc3 - index must be greater than zero
	// leeremove - 290506 - u62 - unpopular negative-value runtime error removed
//	if ( iValue < 0 )
//	{
//		RunTimeError ( RUNTIMEERROR_MUSTBEPOSITIVEVALUE );
//		return pDestStr;
//	}

	// Work string
	ValidateWorkString ( (LPSTR)szText );
	LPSTR text=m_pWorkString;
	if(szText)
		strcpy(text, (char*)szText);
	else
		strcpy(text, "");

	if(iValue>0 && iValue<=(int)strlen(text))
		text[iValue]=0;
	else
		if(iValue<=0)
			strcpy(text, "");

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK int	  Len	( DWORD dwSrcStr )
{
	if(dwSrcStr)
		return strlen((LPSTR)dwSrcStr);
	else
		return 0;
}

DARKSDK DWORD Lower ( DWORD pDestStr, DWORD szText )
{
	// Work string
	ValidateWorkString ( (LPSTR)szText );
	if(szText)
		strcpy(m_pWorkString, (LPSTR)szText);
	else
		strcpy(m_pWorkString, "");

	_strlwr(m_pWorkString);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD Mid	( DWORD pDestStr, DWORD szText, int iValue )
{
	// lee - 290306 - u6rc3 - index must be greater than zero
	// leeremove - 290506 - u62 - unpopular negative-value runtime error removed
//	if ( iValue < 0 )
//	{
//		RunTimeError ( RUNTIMEERROR_MUSTBEPOSITIVEVALUE );
//		return pDestStr;
//	}

	// Work string
	ValidateWorkString ( (LPSTR)szText );
	char character;
	LPSTR text=m_pWorkString;
	if(szText)
		strcpy(text, (char*)szText);
	else
		strcpy(text, "");

	unsigned int index = iValue;
	if(index>0 && index<=strlen(text))
	{
		character=text[index-1];	
		text[0]=character;
		text[1]=0;
	}
	else
		text[0]=0;

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD Right ( DWORD pDestStr, DWORD szText, int iValue )
{
	// lee - 290306 - u6rc3 - index must be greater than zero
	// leeremove - 290506 - u62 - unpopular negative-value runtime error removed
//	if ( iValue < 0 )
//	{
//		RunTimeError ( RUNTIMEERROR_MUSTBEPOSITIVEVALUE );
//		return pDestStr;
//	}

	// Work string
	ValidateWorkString ( (LPSTR)szText );
	LPSTR text=m_pWorkString;
	if(szText)
		strcpy(text, (char*)szText);
	else
		strcpy(text, "");

	int w = 0;
	int length = strlen(text);
	int rightmost = length-iValue;
	if(rightmost>=0 && rightmost<=length)
	{
		for(int n=rightmost; n<length; n++)
			text[w++]=text[n];
		text[w]=0;
	}
	else
		if(rightmost>length)
			strcpy(text, "");
		
	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD Str	( DWORD pDestStr, float fValue )
{
	// Work string
	sprintf(m_pWorkString, "%.12g", fValue);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD StrEx	( DWORD pDestStr, float fValue, int iDecPlaces )
{
	// Work string
	char format[32];
	sprintf(format, "%%.%df", iDecPlaces );
	sprintf(m_pWorkString, format, fValue);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD Str	( DWORD pDestStr, int iValue )
{
	// Work string
	sprintf(m_pWorkString, "%d", iValue);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD Upper ( DWORD pDestStr, DWORD szText )
{
	// Work string
	ValidateWorkString ( (LPSTR)szText );
	if(szText)
		strcpy(m_pWorkString, (LPSTR)szText);
	else
		strcpy(m_pWorkString, "");

	_strupr(m_pWorkString);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

/*
int	  ValL	( DWORD dwSrcStr )
{
	if(dwSrcStr)
		return atoi((LPSTR)dwSrcStr);
	else
		return 0;
}
*/

DARKSDK DWORD ValF	( DWORD dwSrcStr )
{
	float fValue = 0.0f;
	if(dwSrcStr) fValue = (float)atof((LPSTR)dwSrcStr);
	return *(DWORD*)&fValue;
}

//LEEFIX - 191102 - Added DOUBLE INTEGER Return for bigger numbers
/*LEEFIX - 060303 - Removed until compiler can handle expression selection
//					from multiple output types ( I dont want to start adding commands!)
*/
// mike - 220107 - add this back in for gdk
LONGLONG ValR	( DWORD dwSrcStr )
{
	LONGLONG lValue = 0;
	if(dwSrcStr) lValue = _atoi64((LPSTR)dwSrcStr);
	return lValue;
}


DARKSDK DWORD StrDouble	( DWORD pDestStr, double dValue )
{
	// Work string
	sprintf(m_pWorkString, "%.16g", dValue);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK DWORD StrDoubleInt	( DWORD pDestStr, LONGLONG lValue )
{
	// Work string
	sprintf(m_pWorkString, "%I64d", lValue);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

//
// Text Command Functions
//

DARKSDK void PerformChecklistForFonts ( void )
{
	// Generate Checklist
	g_pGlob->checklisthasvalues=false;
	g_pGlob->checklisthasstrings=true;

	g_dwMaxStringSizeInEnum=0;
	g_bCreateChecklistNow=false;
	for(int pass=0; pass<2; pass++)
	{
		if(pass==1)
		{
			// Ensure checklist is large enough
			g_bCreateChecklistNow=true;
			for(int c=0; c<g_pGlob->checklistqty; c++)
				GlobExpandChecklist(c, g_dwMaxStringSizeInEnum);
		}

		// Run through total list of fonts
		g_pGlob->checklistqty=0;
		HDC hdc = CreateCompatibleDC ( NULL );															// create dc
		DWORD SearchData=0;
		EnumFontFamilies(hdc, NULL, (FONTENUMPROC)EnumFontFamProc, SearchData); 
		DeleteDC ( hdc );
	}
 
	// Determine if checklist has any contents
	if(g_pGlob->checklistqty>0)
		g_pGlob->checklistexists=true;
	else
		g_pGlob->checklistexists=false;
}

DARKSDK void BasicText ( int iX, int iY, DWORD szText )
{
	// External Ink Color Control
	if(m_dwColor!=g_pGlob->dwForeColor)
	{
		m_dwColor=g_pGlob->dwForeColor;
		if(m_dwBKColor>0) Recreate();
	}
	
	if(m_dwBKColor!=g_pGlob->dwBackColor) { m_dwBKColor=g_pGlob->dwBackColor; Recreate(); }
	if(szText) Text ( iX, iY, (LPSTR)szText );
}

DARKSDK void CenterText ( int iX, int iY, DWORD szText )
{
	// External Ink Color Control
	if(m_dwColor!=g_pGlob->dwForeColor)
	{
		m_dwColor=g_pGlob->dwForeColor;
		if(m_dwBKColor>0) Recreate();
	}
	if(m_dwBKColor!=g_pGlob->dwBackColor) { m_dwBKColor=g_pGlob->dwBackColor; Recreate(); }
	int iHalfWidth=GetTextWidth((LPSTR)szText)/2;
	if(szText) Text ( iX-iHalfWidth, iY, (LPSTR)szText );
}

DARKSDK void SetBasicTextFont ( DWORD szTypeface )
{
	if(szTypeface)
	{
		memset ( m_strFontName, 0, sizeof ( m_strFontName ) );
		if(szTypeface) memcpy ( m_strFontName, (LPSTR)szTypeface, sizeof ( char ) * strlen ( (LPSTR)szTypeface ) );
		Recreate ( );
	}
}

DARKSDK void SetBasicTextFont ( DWORD szTypeface, int iCharacterSet )
{
	if(szTypeface)
	{
		memset ( m_strFontName, 0, sizeof ( m_strFontName ) );
		if(szTypeface) memcpy ( m_strFontName, (LPSTR)szTypeface, sizeof ( char ) * strlen ( (LPSTR)szTypeface ) );
		m_iTextCharSet = iCharacterSet;
		Recreate ( );
	}
}

DARKSDK void SetTextSize ( int iSize )
{
	if ( (DWORD)iSize != m_dwFontHeight )
	{
		m_dwFontHeight = iSize;

		// mike - 250604 - size cannot be greater than 100

		if ( m_dwFontHeight > 100 )
			m_dwFontHeight = 100;

		Recreate ( );
	}
}

DARKSDK void SetTextToNormal ( void )
{
	m_bTextBold   = false;
	m_bTextItalic = false;
	Recreate ( );
}

DARKSDK void SetTextToItalic ( void )
{
	m_bTextBold   = false;
	m_bTextItalic = true;
	Recreate ( );
}

DARKSDK void SetTextToBold ( void )
{
	m_bTextBold = true;
	m_bTextItalic = false;
	Recreate ( );
}

DARKSDK void SetTextToBoldItalic ( void )
{
	m_bTextBold   = true;
	m_bTextItalic = true;
	Recreate ( );
}

DARKSDK void SetTextToOpaque ( void )
{
	m_bTextOpaque = true;
	Recreate ( );
}

DARKSDK void SetTextToTransparent ( void )
{
	m_bTextOpaque = false;
	Recreate ( );
}

//
// Command Expression Functions
//

DARKSDK DWORD TextFont ( DWORD pDestStr )
{
	// Work string
	strcpy(m_pWorkString, m_strFontName);

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=GetReturnStringFromWorkString();
	return (DWORD)pReturnString;
}

DARKSDK int TextSize ( void )
{
	return m_dwFontHeight;
}

DARKSDK int TextStyle ( void )
{
	int iStyle=0;
	if(m_bTextItalic) iStyle+=1;
	if(m_bTextBold) iStyle+=2;
	return iStyle;
}

DARKSDK int TextBackgroundType ( void )
{
	if(m_bTextOpaque)
		return 1;
	else
		return 0;
}

DARKSDK int TextWidth ( DWORD szString )
{
	int iWidth=0;
	if(szString)
	{
		while ( *(LPSTR)szString )
		{
			unsigned char c = *(LPSTR)szString++;
			if ( c>=32 ) iWidth+=m_szTexWidth [ c - 32 ];
		}
	}
	return iWidth;
}

DARKSDK int TextHeight ( DWORD szString )
{
	int iHeight=0;
	if(szString)
	{
		while ( *(LPSTR)szString )
		{
			unsigned char c = *(LPSTR)szString++;
			if ( c>=32 )
			{
				int iThisH = m_szTexHeight [ c - 32 ];
				if(iThisH>iHeight) iHeight=iThisH;
			}
		}
	}
	return iHeight;
}

//
// New Command Functions
//

DARKSDK void Text3D ( char* szText )
{
	if(szText==NULL)
		return;

	float			x = 0.0f;
    float			y = 0.0f;
	FONT3DVERTEX*	pVertices;
    DWORD			dwVertex       = 0L;
    DWORD			dwNumTriangles = 0L;
	FLOAT			fStartX = x;
	unsigned char	c;

	D3DXMATRIX	matTrans;

	m_pD3D->GetTransform ( D3DTS_WORLD, &matTrans );
	m_pD3D->SetTransform ( D3DTS_WORLD, &m_pPosText->matObject );

	// setup renderstates
    m_pSavedStateBlock->Capture();
    m_pDrawTextStateBlock->Apply();

    m_pD3D->SetVertexShader   ( NULL );
    m_pD3D->SetFVF   ( D3DFVF_FONT3DVERTEX );

    m_pD3D->SetStreamSource   ( 0, m_pVB, 0, sizeof ( FONT3DVERTEX) );

    // set filter states
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );

	// fill vertex buffer
    m_pVB->Lock ( 0, 0, ( VOID** )&pVertices, D3DLOCK_DISCARD );

	while ( c = *szText++ )
    {
        if ( c == '\n' )
        {
            x  = fStartX;
            y -= ( m_fTexCoords [ 0 ] [ 3 ] - m_fTexCoords [ 0 ] [ 1 ] ) * m_dwTexHeight / 10.0f;
        }

        if ( c < 32 )
            continue;

        FLOAT tx1 = m_fTexCoords [ c - 32 ] [ 0 ];
        FLOAT ty1 = m_fTexCoords [ c - 32 ] [ 1 ];
        FLOAT tx2 = m_fTexCoords [ c - 32 ] [ 2 ];
        FLOAT ty2 = m_fTexCoords [ c - 32 ] [ 3 ];

        FLOAT w = ( tx2 - tx1 ) * m_dwTexWidth  / ( 10.0f * m_fTextScale );
        FLOAT h = ( ty2 - ty1 ) * m_dwTexHeight / ( 10.0f * m_fTextScale );

        *pVertices++ = InitFont3DVertex ( D3DXVECTOR3 ( x + 0, y + 0, 0 ), D3DXVECTOR3 ( 0, 0, -1 ), tx1, ty2 );
        *pVertices++ = InitFont3DVertex ( D3DXVECTOR3 ( x + 0, y + h, 0 ), D3DXVECTOR3 ( 0, 0, -1 ), tx1, ty1 );
        *pVertices++ = InitFont3DVertex ( D3DXVECTOR3 ( x + w, y + 0, 0 ), D3DXVECTOR3 ( 0, 0, -1 ), tx2, ty2 );
        *pVertices++ = InitFont3DVertex ( D3DXVECTOR3 ( x + w, y + h, 0 ), D3DXVECTOR3 ( 0, 0, -1 ), tx2, ty1 );
        *pVertices++ = InitFont3DVertex ( D3DXVECTOR3 ( x + w, y + 0, 0 ), D3DXVECTOR3 ( 0, 0, -1 ), tx2, ty2 );
        *pVertices++ = InitFont3DVertex ( D3DXVECTOR3 ( x + 0, y + h, 0 ), D3DXVECTOR3 ( 0, 0, -1 ), tx1, ty1 );
        dwNumTriangles += 2;

        if ( dwNumTriangles * 3 > ( MAX_NUM_VERTICES - 6 ) )
        {
            m_pVB->Unlock ( );
            m_pD3D->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
            m_pVB->Lock ( 0, 0, ( VOID** ) &pVertices, D3DLOCK_DISCARD );
            dwNumTriangles = 0L;
        }

        x += w;
    }

	// Unlock and render the vertex buffer
    m_pVB->Unlock ( );

    if ( dwNumTriangles > 0 )
		m_pD3D->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

    // restore the modified renderstates
//    m_pD3D->ApplyStateBlock ( m_dwSavedStateBlock );
    m_pSavedStateBlock->Apply();

	//m_pD3D->GetTransform ( D3DTS_WORLD, &matTrans );
	m_pD3D->SetTransform ( D3DTS_WORLD, &matTrans );
}

//
// Extra String Expressions
//

DARKSDK DWORD Spaces ( DWORD pDestStr, int iSpaces )
{
	// mike - 250604 - addition for negative input
	if ( iSpaces < 0 )
	{
		if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
		LPSTR pReturnString=NULL;
		g_pCreateDeleteStringFunction((DWORD*)&pReturnString, 2 );

		memset((LPSTR)pReturnString, 32, 2);

		pReturnString [ 0 ] = 0;
		pReturnString [ 1 ] = 0;

		return (DWORD)pReturnString;	
	}

	// Create and return string
	if(pDestStr) g_pCreateDeleteStringFunction((DWORD*)&pDestStr, 0);
	LPSTR pReturnString=NULL;
	g_pCreateDeleteStringFunction((DWORD*)&pReturnString, iSpaces+1 );
	memset((LPSTR)pReturnString, 32, iSpaces);
	pReturnString[iSpaces]=0;
	return (DWORD)pReturnString;
}

// MIKE - 100204 - new text based commands

/*
	APPEND$%SS%?Append@@YAXKK@Z%string A, string B
	REVERSE$%S%?Reverse@@YAXK@Z%string
	FIND FIRST CHAR$[%LSS%?FindFirstChar@@YAHKK@Z%source, char
	FIND LAST CHAR$[%LSS%?FindLastChar@@YAHKK@Z%source, char
	FIND SUB STRING$[%LSS%?FindSubString@@YAHKK@Z%source, string
	COMPARE CASE$[%LSS%?CompareCase@@YAHKK@Z%string A, string B
	FIRST TOKEN$[%SSS%?FirstToken@@YAKKKK@Z%source, delim
	NEXT TOKEN$[%SS%?NextToken@@YAKKK@Z%delim
*/

DARKSDK char* SetupString ( char* szInput )
{
	char* pReturn = NULL;
	DWORD dwSize  = strlen ( szInput );

	g_pGlob->CreateDeleteString ( ( DWORD* ) &pReturn, dwSize + 1 );

	// error
	if ( !pReturn )
		RunTimeError ( RUNTIMEERROR_NOTENOUGHMEMORY );
		
	memcpy ( pReturn, szInput, dwSize );

	pReturn [ dwSize ] = 0;

	return pReturn;
}

// u74b7 - removed append statement as not fixable
// For information: The string resource was ... APPEND$%SS%?Append@@YAXKK@Z%string A, string B
//DARKSDK void Append ( DWORD dwA, DWORD dwB )
//{
//	strcat ( ( char* ) dwA, ( char* ) dwB );
//}

DARKSDK void Reverse ( DWORD dwA )
{
	strrev ( ( char* ) dwA );
}

DARKSDK int FindFirstChar ( DWORD dwSource, DWORD dwChar )
{
	char* pInt    = ( char* ) dwChar;
	char  pIntA   = *pInt;
	char* pFirst  = strchr ( ( char* ) dwSource, ( int ) pIntA );
	int   iResult = pFirst - ( char* ) dwSource + 1;

	return iResult;
}

DARKSDK int FindLastChar ( DWORD dwSource, DWORD dwChar )
{
	char* pInt    = ( char* ) dwChar;
	char  pIntA   = *pInt;
	char* pFirst  = strrchr ( ( char* ) dwSource, ( int ) pIntA );
	int   iResult = pFirst - ( char* ) dwSource + 1;

	return iResult;
}

DARKSDK int FindSubString ( DWORD dwSource, DWORD dwString )
{
	char* pFirst  = strstr ( ( char* ) dwSource, ( char* ) dwString );
	int   iResult = pFirst - ( char* ) dwSource + 1;

	return iResult;
}

DARKSDK int CompareCase ( DWORD dwA, DWORD dwB )
{
	if ( strcmp ( ( char* ) dwA, ( char* ) dwB ) == 0 )
		return 1;

	return 0;
}

// u74b7 - rewrite so that tokens are non-destructive to the source string.
//         Work is carried out on a copy instead of the original.
DARKSDK DWORD FirstToken ( DWORD dwReturn, DWORD dwSource, DWORD dwDelim )
{
    LPSTR szSource = (LPSTR) dwSource;
    LPSTR szDelim  = (LPSTR) dwDelim;

    // If the delimiter an empty string, use a space
    if (szDelim == 0 || szDelim[0] == 0)
    {
        szDelim = " ";
    }

    // If there's a string, copy it to the temp area
    if ( szSource && szSource[0] )
    {
        // Get length, including null terminator
        DWORD dwLength = strlen( szSource ) + 1;

        // If the temp area isn't large enough, make it so
        if ( dwLength > m_dwTokenStringSize )
        {
            // U74 - 060709 - double the allocation is not enough, must use dwLength
            m_dwTokenStringSize = dwLength + 1;

			// free old string and create larger one
            delete[] m_szTokenString;
            m_szTokenString = new char[ m_dwTokenStringSize ];
			memset ( m_szTokenString, 0, sizeof(m_szTokenString) );
        }

        // Copy the source string into the temp area
        memcpy( m_szTokenString, szSource, dwLength );
    }
    else
    {
        m_szTokenString[ 0 ] = 0;
    }

	// U74 BETA9 - 060709 - free old string and create new one
	g_pGlob->CreateDeleteString ( ( DWORD* ) &dwReturn, 0 );
    char* szToken = strtok ( m_szTokenString, szDelim );
    if ( szToken )
    {
	    return ( DWORD ) SetupString ( szToken );
    }
    return ( DWORD ) SetupString ( "" );
}

DARKSDK DWORD NextToken ( DWORD dwReturn, DWORD dwDelim )
{
    LPSTR szDelim  = (LPSTR) dwDelim;

    // If the delimiter an empty string, use a space
    if (szDelim == 0 || szDelim[0] == 0)
    {
        szDelim = " ";
    }

	// U74 BETA9 - 060709 - free old string and create new one
	g_pGlob->CreateDeleteString ( ( DWORD* ) &dwReturn, 0 );
	char* szToken = strtok ( NULL, szDelim );
    if ( szToken )
    {
	    return ( DWORD ) SetupString ( szToken );
    }
    return ( DWORD ) SetupString ( "" );
}

//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorText ( HINSTANCE hSetup  )
{
	TextConstructor ( hSetup );
}

void DestructorText ( void )
{
	TextDestructor ( );
}

void SetErrorHandlerText ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataText ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DText ( int iMode )
{
	RefreshD3D ( iMode );
}

int dbAsc ( char* dwSrcStr)
{
	return Asc ( ( DWORD ) dwSrcStr);
}

char* dbBin	( int iValue )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Bin ( NULL, iValue );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbChr	(  int iValue )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Chr ( NULL, iValue );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbHex	(  int iValue )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Hex ( NULL, iValue );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbLeft ( char* szText, int iValue )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Left ( NULL, ( DWORD ) szText, iValue );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int dbLen ( char* dwSrcStr )
{
	return Len	(  ( DWORD ) dwSrcStr );
}

char* dbLower ( char* szText )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Lower ( NULL, ( DWORD ) szText );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbMid	(  char* szText, int iValue )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Mid ( NULL, ( DWORD )szText,  iValue );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbRight (  char* szText, int iValue )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Right ( NULL, ( DWORD )szText,  iValue );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbStr	(  float fValue )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Str ( NULL,  fValue );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbStr	(  int iValue )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Str ( NULL, iValue );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

// leefix - 2103060 u6b4 - changed from DWORD to char* - GDK could not resolve external linkage
char* dbUpper (  char* szText )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Upper ( NULL, (DWORD)szText );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

float dbValF ( char* dwSrcStr )
{
	DWORD dwReturn = ValF ( ( DWORD ) dwSrcStr );
	
	return *( float* ) &dwReturn;
}

double dbStrDouble (  double dValue )
{
	return StrDouble (  NULL,  dValue );
}

LONGLONG dbValR ( char* dwSrcStr )
{
	// mike - 220107 - modified this function for correct param and casting
	return ValR ( ( DWORD ) dwSrcStr );
}

void dbPerformChecklistForFonts ( void )
{
	PerformChecklistForFonts ( );
}

void dbText ( int iX, int iY, char* szText )
{
	BasicText (  iX,  iY, ( DWORD ) szText );
}

void dbCenterText ( int iX, int iY, char* szText )
{
	CenterText (  iX,  iY,  ( DWORD ) szText );
}

void dbSetTextFont ( char* szTypeface )
{
	SetBasicTextFont (  ( DWORD ) szTypeface );
}

void dbSetTextFont ( char* szTypeface, int iCharacterSet )
{
	SetBasicTextFont (  ( DWORD ) szTypeface,  iCharacterSet );
}
	
void dbSetTextSize ( int iSize )
{
	SetTextSize ( iSize );
}
 
void dbSetTextToNormal ( void )
{
	SetTextToNormal ( );
}
 
void dbSetTextToItalic ( void )
{
	SetTextToItalic ( );
}
 
void dbSetTextToBold ( void )
{
	SetTextToBold ( );
}
 
void dbSetTextToBoldItalic ( void )
{
	SetTextToBoldItalic ( );
}
 
void dbSetTextToOpaque ( void )
{
	SetTextToOpaque ( );
}
 
void dbSetTextToTransparent ( void )
{
	SetTextToTransparent ( );
}

int dbTextBackgroundType ( void )
{
	return TextBackgroundType ( );
}

char* dbTextFont ( void )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = TextFont ( NULL );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

int dbTextSize ( void )
{
	return TextSize ( );
}

int dbTextStyle ( void )
{
	return TextStyle ( );
}

int dbTextWidth ( char* szString )
{
	return TextWidth (  ( DWORD ) szString );
}

int dbTextHeight ( char* szString )
{
	return TextHeight ( ( DWORD ) szString );
}

void dbText3D ( char* szText )
{
	Text3D (  szText );
}

char* dbSpaces	( int iSpaces )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = Spaces ( NULL, iSpaces );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

void dbAppend ( char* dwA, char* dwB )
{
//  U74 discontinued command
//	Append ( ( DWORD ) dwA, ( DWORD ) dwB );
}

void dbReverse ( char* dwA )
{
	Reverse ( ( DWORD ) dwA );
}

int dbFindFirstChar ( char* dwSource, char* dwChar )
{
	return FindFirstChar ( ( DWORD ) dwSource, ( DWORD ) dwChar );
}

int dbFindLastChar ( char* dwSource, char* dwChar )
{
	return FindLastChar ( ( DWORD ) dwSource, ( DWORD ) dwChar );
}

int dbFindSubString ( char* dwSource, char* dwString )
{
	return FindSubString ( ( DWORD ) dwSource, ( DWORD ) dwString );
}

int dbCompareCase ( char* dwA, char* dwB )
{
	return CompareCase ( ( DWORD ) dwA, ( DWORD ) dwB );
}

char* dbFirstToken ( char* dwSource, char* dwDelim )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = FirstToken (  NULL, ( DWORD ) dwSource,  ( DWORD ) dwDelim );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

char* dbNextToken ( char* dwDelim )
{
	static char* szReturn = NULL;
	DWORD		 dwReturn = NextToken (  NULL, ( DWORD ) dwDelim );

	szReturn = ( char* ) dwReturn;

	return szReturn;
}

void dbSetTextColor	( int iAlpha, int iRed, int iGreen, int iBlue )
{
	SetTextColor ( iAlpha, iRed, iGreen, iBlue );
}

int	dbGetTextWidth ( char* szString )
{
	return GetTextWidth ( szString );
}

int	dbGetTextHeight	( char* szString )
{
	return GetTextHeight ( szString );
}

// lee - 300706 - GDK fixes
void dbSetTextOpaque ( void ) { dbSetTextToOpaque (); }							
void dbSetTextTransparent ( void ) { dbSetTextToTransparent (); }					
float dbVal	( char* dwSrcStr ) { return dbValF ( dwSrcStr ); }

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////