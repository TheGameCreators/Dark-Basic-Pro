
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include "cbitmapc.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"
#include ".\..\camera\ccameradatac.h"
#include ".\..\Core\SteamCheckForWorkshop.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include <map>

DBPRO_GLOBAL GlobStruct*			g_pGlob						= NULL;

namespace
{
    typedef std::map<int, tagData*>	BitmapList_t;
    typedef BitmapList_t::iterator	BitmapPtr;


    typedef IDirect3DDevice9*		( *GFX_GetDirect3DDevicePFN ) ( void );
    typedef void*					( *CAMERA3D_GetInternalDataPFN ) ( int );

    BitmapList_t					m_List;
    LPDIRECT3DDEVICE9				m_pD3D						= NULL;
    LPDIRECT3D9						m_pDX						= NULL;
    int								m_iWidth					= 0;
    int								m_iHeight					= 0;
    int								m_iMipMapNum				= 1;
    int								m_iMemory					= D3DPOOL_MANAGED;
    bool							m_bSharing					= true;
    bool							m_bMipMap					= true;
    D3DCOLOR						m_Color						= D3DCOLOR_ARGB ( 255, 0, 0, 0 );
    int                             m_CurrentId                 = -1;
    tagData*						m_ptr						= NULL;
    D3DFORMAT						g_CommonSurfaceFormat;
    DWORD							g_CommonSurfaceDepth;
    DWORD							g_CSBPP;
    bool							g_bOffscreenBitmap			= false;
    bool							g_bSupressErrorMessage		= false;

    GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;
    CAMERA3D_GetInternalDataPFN		g_Camera3D_GetInternalData;



    bool RemoveBitmap( int iID )
    {
        // Clear the cached value if the bitmap being deleted is the current cached bitmap.
        if (m_CurrentId == iID)
        {
            m_CurrentId = -1;
            m_ptr = NULL;
        }

        // Locate the bitmap, and if found, release all of it's resources.
        BitmapPtr pBitmap = m_List.find( iID );
        if (pBitmap != m_List.end())
        {
            SAFE_RELEASE( pBitmap->second->lpSurface );
            // SAFE_DELETE( pBitmap->second->lpDepth ); 240111 - crashes XP infrequently
            SAFE_RELEASE( pBitmap->second->lpDepth );
            delete pBitmap->second;

            m_List.erase(pBitmap);

            return true;
        }

        return false;
    }

    bool UpdatePtr ( int iID )
    {
        // If the bitmap required is not already cached, refresh the cached value
        if (!m_ptr || iID != m_CurrentId)
        {
            m_CurrentId = iID;

            BitmapPtr p = m_List.find( iID );
            if (p == m_List.end())
            {
                m_ptr = NULL;
            }
            else
            {
                m_ptr = p->second;
            }
        }

        return m_ptr != NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void Constructor ( HINSTANCE hSetup )
{
	// Handle now passed in core

}

DARKSDK void Destructor ( void )
{
    m_CurrentId = -1;
    m_ptr = NULL;

    for (BitmapPtr pCheck = m_List.begin(); pCheck != m_List.end(); ++pCheck)
    {
        // Release the texture and texture name
        tagData* ptr = pCheck->second;
		SAFE_RELEASE ( ptr->lpSurface );
		SAFE_RELEASE ( ptr->lpDepth );

        // Release the rest of the image storage
        delete ptr;

        // NOTE: Not removing from m_List at this point:
        // 1 - it makes moving to the next item harder
        // 2 - it's less efficient - we'll clear the entire list at the end
    }

    // Now clear the list
    m_List.clear();

	// Free reference to D3D we took
	SAFE_RELEASE(m_pDX);
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

	// setup function pointers
	//g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( g_pGlob->g_GFX, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );

	#ifndef DARKSDK_COMPILE
	{
		// D3D
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( g_pGlob->g_GFX, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );

		// Camera Ptr
		g_Camera3D_GetInternalData = ( CAMERA3D_GetInternalDataPFN )	GetProcAddress ( g_pGlob->g_Camera3D, "?GetInternalData@@YAPAXH@Z" );
	}
	#else
	{
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;
	}
	#endif

	m_pD3D                  = g_GFX_GetDirect3DDevice ( );
	m_pD3D->GetDirect3D(&m_pDX);

	// Use refresh to recreate bitmap zero
	UpdateBitmapZeroOfNewBackbuffer();

	// Start with bitmap zero target
	SetCurrentBitmap(0);
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
		PassCoreData ( g_pGlob );
	}
}

DARKSDK int GetHeight ( int iID )
{
	// get the height of an image

	// update internal data
	if ( !UpdatePtr ( iID ) )
		return -1;

	// return the height
	return m_ptr->iHeight;
}

DARKSDK int GetWidth ( int iID )
{
	// get the width of an image

	// update internal data
	if ( !UpdatePtr ( iID ) )
		return -1;

	// return the width
	return m_ptr->iWidth;
}

DARKSDK void Write ( int iID, int iX, int iY, int iA, int iR, int iG, int iB )
{
	if ( !UpdatePtr ( iID ) )
		return;

	if ( !m_ptr->bLocked )
		Error ( "Unable to modify texture data for image library as it isn't locked" );
	
	DWORD* pPix = ( DWORD* ) m_ptr->d3dlr.pBits;
	
	pPix [ ( ( m_ptr->d3dlr.Pitch >> 4 ) * iX ) + iY ] = D3DCOLOR_ARGB ( iA, iR, iG, iB );
}

DARKSDK void Get ( int iID, int iX, int iY, int* piR, int* piG, int* piB )
{
	// get a pixel at x, y

	// check pointers are valid
	if ( !piR || !piG || !piB )
		return;

	// update the internal data
	if ( !UpdatePtr ( iID ) )
		return;

	// check the image is locked
	if ( !m_ptr->bLocked )
		Error ( "Unable to get texture data for image library as it isn't locked" );

	// get a pointer to the data
	DWORD* pPix = ( DWORD* ) m_ptr->d3dlr.pBits;
	DWORD  pGet;

	// get offset in file
	pGet = pPix [ ( ( m_ptr->d3dlr.Pitch >> 4 ) * iX ) + iY ];

	// break value down
	DWORD dwAlpha = pGet >> 24;						// get alpha
	DWORD dwRed   = ((( pGet ) >> 16 ) & 0xff );	// get red
	DWORD dwGreen = ((( pGet ) >>  8 ) & 0xff );	// get green
	DWORD dwBlue  = ((( pGet ) )       & 0xff );	// get blue

	// save values
	*piR = dwRed;
	*piG = dwGreen;
	*piB = dwBlue;
}

DARKSDK D3DFORMAT GetValidStencilBufferFormat ( D3DFORMAT BackBufferFormat )
{
	// create the list in order of precedence
	D3DFORMAT DepthFormat;
	D3DFORMAT list [ ] =
							{
								D3DFMT_D24S8, //GeForce4 top choice
								D3DFMT_R8G8B8,
								D3DFMT_A8R8G8B8,
								D3DFMT_X8R8G8B8,
								D3DFMT_R5G6B5,
								D3DFMT_X1R5G5B5,
								D3DFMT_A1R5G5B5,
								D3DFMT_A4R4G4B4,
								D3DFMT_R3G3B2,
								D3DFMT_A8,
								D3DFMT_A8R3G3B2,
								D3DFMT_X4R4G4B4,
								D3DFMT_A8P8,
								D3DFMT_P8,
								D3DFMT_L8,
								D3DFMT_A8L8,
								D3DFMT_A4L4,
								D3DFMT_V8U8,
								D3DFMT_L6V5U5,
								D3DFMT_X8L8V8U8,
								D3DFMT_Q8W8V8U8,
								D3DFMT_V16U16,
//								D3DFMT_W11V11U10,
								D3DFMT_D16_LOCKABLE,
								D3DFMT_D32,
								D3DFMT_D15S1,
								D3DFMT_D16,
								D3DFMT_D24X8,
								D3DFMT_D24X4S4,
							};

	for ( int iTemp = 0; iTemp < 29; iTemp++ )
	{
		// Verify that the depth format exists first
		if ( SUCCEEDED ( m_pDX->CheckDeviceFormat(  D3DADAPTER_DEFAULT,
													D3DDEVTYPE_HAL,
													BackBufferFormat,
													D3DUSAGE_DEPTHSTENCIL,
													D3DRTYPE_SURFACE,
													list [ iTemp ]						) ) )
		{
			if ( SUCCEEDED ( m_pDX->CheckDepthStencilMatch	(	D3DADAPTER_DEFAULT,
																D3DDEVTYPE_HAL,
																BackBufferFormat,
																BackBufferFormat,
																list [ iTemp ]				) ) )
			{
				DepthFormat = list [ iTemp ];
				break;
			}
		}
	}

	return DepthFormat;
}

DARKSDK LPDIRECT3DSURFACE9 MakeSurface ( int iWidth, int iHeight, D3DFORMAT Format )
{
	// create surface for bitmap
	LPDIRECT3DSURFACE9 lpSurface = NULL;
	HRESULT hr = m_pD3D->CreateRenderTarget( iWidth, iHeight, Format,
									D3DMULTISAMPLE_NONE, 0, TRUE,
									&lpSurface, NULL);

	// return ptr to it
	return lpSurface;
}

DARKSDK LPDIRECT3DSURFACE9 MakeSurface ( int iWidth, int iHeight )
{
	return MakeSurface ( iWidth, iHeight, g_CommonSurfaceFormat );
}

DARKSDK LPDIRECT3DSURFACE9 MakeFormat ( int iID, int iWidth, int iHeight, D3DFORMAT d3dformat )
{
	// make a new image

	// variable declarations
	tagData*	test = NULL;		// pointer to data structure
	HRESULT		hr;					// used for error checking
	
	// create a new block of memory
	test = new tagData;
	memset(test, 0, sizeof(tagData));

	// check the memory was created
	if ( test == NULL )
		return NULL;

	// clear out the memory
	memset ( test, 0, sizeof ( test ) );

	// video or system bitmap
	if ( g_bOffscreenBitmap )
	{
		// create system surface for bitmap
		// U69 - 010508 - cannot draw to system bitmaps (not a render target, and vid bitmaps are SLOW)
		// but we cannot stard messing with existing functionality! hr = m_pD3D->CreateRenderTarget( iWidth, iHeight, g_CommonSurfaceFormat, D3DMULTISAMPLE_NONE, 0, FALSE, &test->lpSurface, NULL);
		hr = m_pD3D->CreateOffscreenPlainSurface( iWidth, iHeight, g_CommonSurfaceFormat, D3DPOOL_SYSTEMMEM, &test->lpSurface, NULL);
		if ( FAILED ( hr ) )
		{
			if ( g_bSupressErrorMessage==false )
				Error ( "Failed to create new bitmap" );
			SAFE_DELETE(test);
			return NULL;
		}

		// no depth buffer for system bitmaps
		test->lpDepth = NULL;
	}
	else
	{
		// create video surface for bitmap
		hr = m_pD3D->CreateRenderTarget( iWidth, iHeight, g_CommonSurfaceFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &test->lpSurface, NULL);
		if ( FAILED ( hr ) )
		{
			if ( g_bSupressErrorMessage==false )
				Error ( "Failed to create new bitmap" );
			SAFE_DELETE(test);
			return NULL;
		}

		// use backbuffer to determine common D3DFORMAT
		D3DSURFACE_DESC backbufferdesc;
		g_pGlob->pHoldBackBufferPtr->GetDesc(&backbufferdesc);
		D3DFORMAT CommonFormat = backbufferdesc.Format;

		// create depth surface
		if ( test->lpDepth==NULL )
		{
			HRESULT hRes = m_pD3D->CreateDepthStencilSurface(	iWidth, iHeight, GetValidStencilBufferFormat(CommonFormat), D3DMULTISAMPLE_NONE, 0, TRUE,
																&test->lpDepth, NULL );
		}
	}

	// setup properties
	test->iWidth  = iWidth;		// store the height
	test->iHeight = iHeight;		// store the width
	test->iDepth = g_CommonSurfaceDepth;		// store the width
	test->bLocked = false;			// set locked to false
	test->iMirrored  = 0;		
	test->iFlipped  = 0;	

    m_List.insert( std::make_pair(iID, test) );

	return test->lpSurface;
}


DARKSDK void SetBitmapFormat ( int iFormat )
{
	// mike - 061005 - new function to change bitmap format for alpha etc.

	if ( iFormat == - 1 )
	{
		if ( g_pGlob )
		{
			D3DSURFACE_DESC backbufferdesc;
			g_pGlob->pHoldBackBufferPtr->GetDesc ( &backbufferdesc );
			g_CommonSurfaceFormat = backbufferdesc.Format;
		}
	}
	else
	{
		g_CommonSurfaceFormat = ( D3DFORMAT ) iFormat;
	}
}

DARKSDK LPDIRECT3DSURFACE9 Make ( int iID, int iWidth, int iHeight )
{
	if(UpdatePtr ( iID ))
	{
        RemoveBitmap( iID );
	}

	return MakeFormat ( iID, iWidth, iHeight, g_CommonSurfaceFormat );
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

DARKSDK void UpdateBitmapZeroOfNewBackbuffer( void )
{
	int iWidth = g_pGlob->iScreenWidth;
	int iHeight = g_pGlob->iScreenHeight;

	// remove old bitmap zero object
	if ( UpdatePtr ( 0 ) )
	{
        RemoveBitmap(0);
	}

	// use backbuffer to determine common D3DFORMAT
	int iDepthValue=0;
	D3DSURFACE_DESC backbufferdesc;
	g_pGlob->pHoldBackBufferPtr->GetDesc(&backbufferdesc);
	iDepthValue = GetBitDepthFromFormat(backbufferdesc.Format);
	g_CommonSurfaceFormat = backbufferdesc.Format;
	g_CommonSurfaceDepth = iDepthValue;
	g_CSBPP = iDepthValue/8;

	// make new bitmap zero
	g_bOffscreenBitmap=false;
	Make ( 0, iWidth, iHeight );

	// remove surface from it (specified on the fly)
	UpdatePtr ( 0 );
	if(m_ptr)
	{
		// Not surface created
		SAFE_RELEASE(m_ptr->lpSurface);
		SAFE_RELEASE(m_ptr->lpDepth );

		// Actually backbuffer
		int iDepthValue=0;
		D3DSURFACE_DESC imageddsd;
		g_pGlob->pCurrentBitmapSurface->GetDesc(&imageddsd);
		iDepthValue = GetBitDepthFromFormat(imageddsd.Format);
		m_ptr->iDepth = iDepthValue;
	}
}

DARKSDK void DirectMirror(LPDIRECT3DSURFACE9 pFromSurface, DWORD Width, DWORD Height)
{
	// get data from surface
	D3DSURFACE_DESC pFromDesc;
	HRESULT hRes = pFromSurface->GetDesc(&pFromDesc);

	// reverse contents of texture (mirror)
	RECT FromRect = { 0, 0, Width, Height };
	D3DLOCKED_RECT d3dlrFrom;
	if(SUCCEEDED(hRes=pFromSurface->LockRect ( &d3dlrFrom, &FromRect, 0 ) ) )
	{
		LPSTR pToPtr = (LPSTR)d3dlrFrom.pBits;
		LPSTR pFromPtr = (LPSTR)d3dlrFrom.pBits;
		for(DWORD y=0; y<Height; y++)
		{
			LPSTR pStartOfNext=pFromPtr+d3dlrFrom.Pitch;
			pFromPtr += (Width*g_CSBPP);
			for(DWORD x=0; x<Width/2; x++)
			{
				pFromPtr -= g_CSBPP;
				if(g_CSBPP==2)
				{
					WORD wStore = *((WORD*)pToPtr);
					*(WORD*)pToPtr = *((WORD*)pFromPtr);
					*(WORD*)pFromPtr = wStore;
				}
				if(g_CSBPP==4)
				{
					DWORD dwStore = *((DWORD*)pToPtr);
					*(DWORD*)pToPtr = *((DWORD*)pFromPtr);
					*(DWORD*)pFromPtr = dwStore;
				}				
				pToPtr += g_CSBPP;
			}
			pFromPtr=pStartOfNext;
			pToPtr+=d3dlrFrom.Pitch - ((Width/2)*g_CSBPP);
		}
		pFromSurface->UnlockRect();
	}
}

DARKSDK void DirectFlip(LPDIRECT3DSURFACE9 pFromSurface, DWORD Width, DWORD Height)
{
	// get data from surface
	D3DSURFACE_DESC pFromDesc;
	HRESULT hRes = pFromSurface->GetDesc(&pFromDesc);

	// reverse contents of texture (flip)
	RECT FromRect = { 0, 0, Width, Height };
	D3DLOCKED_RECT d3dlrFrom;
	if(SUCCEEDED(hRes=pFromSurface->LockRect ( &d3dlrFrom, &FromRect, 0 ) ) )
	{
		DWORD* pBuffer = new DWORD[Width];
		LPSTR pToPtr = (LPSTR)d3dlrFrom.pBits;
		LPSTR pFromPtr = (LPSTR)d3dlrFrom.pBits + (d3dlrFrom.Pitch*Height);
		for(DWORD y=0; y<Height/2; y++)
		{
			pFromPtr -= d3dlrFrom.Pitch;
			memcpy(pBuffer, pToPtr, Width*g_CSBPP);
			memcpy(pToPtr, pFromPtr, Width*g_CSBPP);
			memcpy(pFromPtr, pBuffer, Width*g_CSBPP);
			pToPtr += d3dlrFrom.Pitch;
		}
		pFromSurface->UnlockRect();
		SAFE_DELETE(pBuffer);
	}
}

DARKSDK DWORD GetAverageFromRange(int addbyte, void* surface, int pitch, int width, int height, int x, int y, int rangea, int rangeb)
{
	// constants set for 32bit only
	int gABitShift = 24;
	int gRBitShift = 16;
	int gGBitShift = 8;
	int gABitFill = 255;
	int gRBitFill = 255;
	int gGBitFill = 255;
	int gBBitFill = 255;

	// if 16bit, set for 565
	if(g_CSBPP==2)
	{
		gABitShift = 15;
		gRBitShift = 11;
		gGBitShift = 5;
		gABitFill = 1;
		gRBitFill = 31;
		gGBitFill = 63;
		gBBitFill = 31;
	}

	// Prepare blank RGB averaging-data
	int aa[10*10];
	int ar[10*10];
	int ag[10*10];
	int ab[10*10];

	// Gather pixel RGB data from range
	char* start = (char*)surface;
	char* pixel;

	int aindex=0;
	for(int ty=y+rangea; ty<=y+rangeb; ty++)
	{
		pixel = start + (ty*pitch);
		pixel += (x+rangea) * addbyte;
		for(int tx=x+rangea; tx<=x+rangeb; tx++)
		{
			if(tx>=0 && ty>=0 && tx<width && ty<height)
			{
				DWORD value = *(DWORD*)pixel;
				DWORD bpp = (addbyte*8);
				if(bpp<32) value &= (1<<bpp)-1;

				int valpha	= ((value >> gABitShift	) & gABitFill );
				int vred	= ((value >> gRBitShift	) & gRBitFill );
				int vgreen	= ((value >> gGBitShift	) & gGBitFill );
				int vblue	= ((value				) & gBBitFill );

				aa[aindex] = valpha;
				ar[aindex] = vred;
				ag[aindex] = vgreen;
				ab[aindex] = vblue;

				aindex++;
				if(aindex>99) break;
			}
			pixel+=addbyte;
		}
	}
	
	// Calculate average from data
	int averagea=0;
	int averager=0;
	int averageg=0;
	int averageb=0;
	for(int a=0; a<aindex; a++)
	{
		averagea += aa[a];
		averager += ar[a];
		averageg += ag[a];
		averageb += ab[a];
	}
	averagea/=aindex;
	averager/=aindex;
	averageg/=aindex;
	averageb/=aindex;
	
	// return new RGB
	return (averagea << gABitShift) + (averager << gRBitShift) + (averageg << gGBitShift) + (averageb);
}

DARKSDK bool DirectBlur(LPDIRECT3DSURFACE9 pFromSurface, int quantity, DWORD width, DWORD height)
{
	// Blur Buffer is 10x10 = 100 storage units only
	if(quantity>9)
	{
		return false;
	}

	// get data from surface
	D3DSURFACE_DESC pFromDesc;
	HRESULT hRes = pFromSurface->GetDesc(&pFromDesc);

	// Define range
	int rangea;
	int rangeb;
	if(quantity<=1)
	{
		rangea=0;
		rangeb=0;
	}
	else
	{
		rangea=(quantity/2)*-1;
		rangeb=(quantity+rangea)-1;
	}

	// create temp to hold source for blur code
	LPDIRECT3DSURFACE9 pToSurface;
	m_pD3D->CreateOffscreenPlainSurface( width, height, pFromDesc.Format, D3DPOOL_SCRATCH, &pToSurface, NULL);

	// reverse contents of texture (flip)
	RECT ToRect = { 0, 0, width, height };
	D3DLOCKED_RECT d3dlrTo;
	if(SUCCEEDED(hRes=pToSurface->LockRect ( &d3dlrTo, &ToRect, 0 ) ) )
	{
		RECT FromRect = { 0, 0, width, height };
		D3DLOCKED_RECT d3dlrFrom;
		if(SUCCEEDED(hRes=pFromSurface->LockRect ( &d3dlrFrom, &FromRect, 0 ) ) )
		{
			// blur phase
			LPVOID destsurface = (LPVOID)d3dlrTo.pBits;
			LPVOID sourcesurface = (LPVOID)d3dlrFrom.pBits;
			DWORD addbyte = g_CSBPP;
			DWORD tpitch = d3dlrTo.Pitch;
			DWORD fpitch = d3dlrFrom.Pitch;
			char* start = (char*)destsurface;
			for(DWORD y=0; y<height; y++)
			{
				char* pixel = start;
				for(DWORD x=0; x<width; x++)
				{
					// Read RGB component on pixel
					DWORD newvalue = GetAverageFromRange(addbyte, sourcesurface, fpitch, width, height, x, y, rangea, rangeb);

					// Write result
					memcpy(pixel, &newvalue, addbyte);

					// next pixel
					pixel+=addbyte;
				}
				start+=tpitch;
			}

			// copy phase
			destsurface = (LPVOID)d3dlrTo.pBits;
			sourcesurface = (LPVOID)d3dlrFrom.pBits;
			start = (char*)destsurface;
			for(DWORD y=0; y<height; y++)
			{
				char* pixel = start;
				int sourceaddy = (y*d3dlrFrom.Pitch);
				for(DWORD x=0; x<width; x++)
				{
					// copy from temp to actual
					if(g_CSBPP==2) *(WORD*)((char*)sourcesurface + (x*g_CSBPP) + sourceaddy) = *(WORD*)pixel;
					if(g_CSBPP==4) *(DWORD*)((char*)sourcesurface + (x*g_CSBPP) + sourceaddy) = *(DWORD*)pixel;

					// next pixel
					pixel+=addbyte;
				}
				start+=tpitch;
			}

			pFromSurface->UnlockRect();
		}
		pToSurface->UnlockRect();
	}

	// release surface when done
	SAFE_RELEASE(pToSurface);

	// Complete
	return true;
}

DARKSDK bool DirectFade(LPDIRECT3DSURFACE9 pFromSurface, int percentage, DWORD width, DWORD height)
{
	// constants set for 32bit only
	int gABitShift = 24;
	int gRBitShift = 16;
	int gGBitShift = 8;
	int gABitFill = 255;
	int gRBitFill = 255;
	int gGBitFill = 255;
	int gBBitFill = 255;

	// if 16bit, set for 565
	if(g_CSBPP==2)
	{
		gABitShift = 15;
		gRBitShift = 11;
		gGBitShift = 5;
		gABitFill = 1;
		gRBitFill = 31;
		gGBitFill = 63;
		gBBitFill = 31;
	}

	// Can only be 0-100
	if(percentage<0 || percentage>100)
		return false;

	// get data from surface
	D3DSURFACE_DESC pFromDesc;
	HRESULT hRes = pFromSurface->GetDesc(&pFromDesc);

	// lock surface and go to work
	RECT FromRect = { 0, 0, width, height };
	D3DLOCKED_RECT d3dlrFrom;
	if(SUCCEEDED(hRes=pFromSurface->LockRect ( &d3dlrFrom, &FromRect, 0 ) ) )
	{
		// Fade Code
		LPVOID	surface = (LPVOID)d3dlrFrom.pBits;
		DWORD	addbyte = g_CSBPP;
		DWORD	pitch = d3dlrFrom.Pitch;
		float pf = percentage/100.0f;
		char* start = (char*)surface;
		char* pixel;
		for(DWORD y=0; y<height; y++)
		{
			pixel = start;
			for(DWORD x=0; x<width; x++)
			{
				// Read RGB component on pixel
				DWORD value;
				if(g_CSBPP==2) value=*(WORD*)pixel;
				if(g_CSBPP==4) value=*(DWORD*)pixel;

				DWORD bpp = (addbyte*8);
				if(bpp<32) value &= (1<<bpp)-1;

				int valpha	= ((value >> gABitShift	) & gABitFill );
				int vred	= ((value >> gRBitShift	) & gRBitFill );
				int vgreen	= ((value >> gGBitShift	) & gGBitFill );
				int vblue	= ((value				) & gBBitFill );

				// Perform fade
//				valpha = (int)(valpha * pf); 140206 - u60 - decided not to fade alpha
				vred = (int)(vred * pf);
				vgreen = (int)(vgreen * pf);
				vblue = (int)(vblue * pf);

				// Write result
				DWORD newvalue = (valpha<<gABitShift) + (vred<<gRBitShift) + (vgreen<<gGBitShift) + (vblue);
				memcpy(pixel, &newvalue, addbyte);

				// next pixel
				pixel+=addbyte;
			}
			start+=pitch;
		}
		pFromSurface->UnlockRect();
	}

	// Complete
	return true;
}

//
// Command Functions
//

DARKSDK void CreateBitmapEx ( int iID, int iWidth, int iHeight, int iSystemMemoryMode, int iSilentError )
{
	// 200112 - silent error mode
	if ( iSilentError==1 ) g_bSupressErrorMessage = true;

	if(iID<1 || iID>MAXIMUMVALUE)
	{
		if ( g_bSupressErrorMessage==false ) RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}

	// make new bitmap
	if ( iSystemMemoryMode==1 )
		g_bOffscreenBitmap=true;
	else
		g_bOffscreenBitmap=false;

	// create surface
	Make ( iID, iWidth, iHeight );

	// auto set to new bitmap
	SetCurrentBitmap ( iID );

	// now clear the bitmap
	m_pD3D->Clear(0,NULL,D3DCLEAR_TARGET,0,1,0);

	// 200112 - silent error mode
	g_bSupressErrorMessage = false;
}

DARKSDK void CreateBitmapEx ( int iID, int iWidth, int iHeight, int iSystemMemoryMode )
{
	CreateBitmapEx ( iID, iWidth, iHeight, iSystemMemoryMode, 0 );
}

DARKSDK void CreateBitmap ( int iID, int iWidth, int iHeight )
{
	CreateBitmapEx ( iID, iWidth, iHeight, 0 );
}

DARKSDK void LoadBitmapCore ( char* szFilename, int iID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}

	// get any required format
	D3DFORMAT RequiredFormat = g_CommonSurfaceFormat;
	if ( UpdatePtr ( iID ) )
	{
		// Backbuffer or Bitmap To/Dest Surface
		LPDIRECT3DSURFACE9 pExistingSurface = m_ptr->lpSurface;
		if(iID==0) pExistingSurface=g_pGlob->pHoldBackBufferPtr;

		// Get format of existing surface
		D3DSURFACE_DESC desc;
		pExistingSurface->GetDesc ( &desc );
		RequiredFormat = desc.Format;
	}

	// load the bitmap into space surface
	D3DXIMAGE_INFO finfo;
	HRESULT hRes = D3DXGetImageInfoFromFile( szFilename, &finfo );
	LPDIRECT3DSURFACE9 pLoadedSurface = MakeSurface ( finfo.Width, finfo.Height, RequiredFormat );

	RECT rc = { 0, 0, finfo.Width, finfo.Height };
	// LEEFIX - 081102 - Prevent dithering of the loaded bitmap - causes rgb(128,128,128) to look weird
	hRes = D3DXLoadSurfaceFromFile(pLoadedSurface, NULL, &rc, szFilename, NULL, D3DX_FILTER_NONE, 0, NULL);
	if ( FAILED ( hRes ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPLOADFAILED);
		return;
	}

	// check if bitmap exists
	if ( UpdatePtr ( iID ) )
	{
		// Backbuffer or Bitmap To/Dest Surface
		LPDIRECT3DSURFACE9 pToSurface = m_ptr->lpSurface;
		if(iID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

		// if exists, copy clipped bitmap into it
		POINT pt = { 0,0 };
		RECT destrc = { 0, 0, finfo.Width, finfo.Height };
		if(destrc.right>m_ptr->iWidth) destrc.right=m_ptr->iWidth;
		if(destrc.bottom>m_ptr->iHeight) destrc.bottom=m_ptr->iHeight;
		m_pD3D->StretchRect(pLoadedSurface, &destrc, pToSurface, &destrc, D3DTEXF_NONE);
	}
	else
	{
		// if new, create actual size
		g_bOffscreenBitmap=false;
		LPDIRECT3DSURFACE9 pThisSurface = Make ( iID, finfo.Width, finfo.Height );
		m_pD3D->StretchRect(pLoadedSurface, NULL, pThisSurface, NULL, D3DTEXF_NONE);

		// find ptr to bitmap
		UpdatePtr ( iID );

		// get actual dimensions of surface
		int iDepthValue=0;
		D3DSURFACE_DESC imageddsd;
		pThisSurface->GetDesc(&imageddsd);
		iDepthValue = GetBitDepthFromFormat(imageddsd.Format);

		// fill out rest of structure
		m_ptr->iWidth  = finfo.Width;					// store the height
		m_ptr->iHeight = finfo.Height;					// store the width
		m_ptr->iDepth  = iDepthValue;				// store the depth
		m_ptr->bLocked = false;
		m_ptr->iMirrored  = 0;		
		m_ptr->iFlipped  = 0;
	}

	// free loaded surface
	SAFE_RELEASE(pLoadedSurface);

	// auto set to new bitmap
	SetCurrentBitmap ( iID );
}

DARKSDK void LoadBitmap ( char* szFilename, int iID )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	LoadBitmapCore ( VirtualFilename, iID );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

DARKSDK void LoadBitmapEx ( char* szFilename )
{
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	CheckForWorkshopFile ( VirtualFilename );

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	LoadBitmapCore ( VirtualFilename, 0 );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );
}

DARKSDK void SaveBitmap ( char* szFilename, int iID )
{
	// Not Implemented in DBPRO V1 RELEASE
	RunTimeError(RUNTIMEERROR_COMMANDNOWOBSOLETE);
}

DARKSDK void SaveBitmapEx ( char* szFilename )
{
	SaveBitmap ( szFilename, 0 );
}

DARKSDK void CopyBitmap ( int iID, int iToID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return;
	}

	// source
	LPDIRECT3DSURFACE9 pFromSurface = m_ptr->lpSurface;
	if(iID==0) pFromSurface=g_pGlob->pHoldBackBufferPtr;

	if(iToID<0 || iToID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtr ( iToID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return;
	}

	// cannot copy a to a
	if(iID==iToID)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}

	// Backbuffer or Bitmap To/Dest Surface
	LPDIRECT3DSURFACE9 pToSurface = m_ptr->lpSurface;
	if(iToID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

	// src
	D3DSURFACE_DESC src;
	pFromSurface->GetDesc(&src);
	int iWidth = src.Width;
	int iHeight = src.Height;

	// dest
	D3DSURFACE_DESC dest;
	pToSurface->GetDesc(&dest);
	if(iWidth>(int)dest.Width) iWidth=(int)dest.Width;
	if(iHeight>(int)dest.Height) iHeight=(int)dest.Height;

	// Ensure the rectangle is within the destination area for copying
	RECT FromRect = { 0, 0, iWidth, iHeight };
	POINT ToRect = { 0, 0 };

	// copy one surface to the other
	m_pD3D->StretchRect(pFromSurface, &FromRect, pToSurface, &FromRect, D3DTEXF_NONE);
}

DARKSDK void CopyBitmapEx ( int iID, int iX1, int iY1, int iX2, int iY2, int iToID, int iToX1, int iToY1, int iToX2, int iToY2 )
{
	HRESULT hRes;

	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return;
	}

	// source
	LPDIRECT3DSURFACE9 pFromSurface = m_ptr->lpSurface;
	if(iID==0) pFromSurface=g_pGlob->pHoldBackBufferPtr;

	if(iToID<0 || iToID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtr ( iToID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return;
	}

	// Backbuffer or Bitmap To/Dest Surface
	LPDIRECT3DSURFACE9 pToSurface = m_ptr->lpSurface;
	if(iToID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

	// if src and dest surfaces the same, create temp copy
	IDirect3DSurface9* pTempSrcSurface = NULL;
	if ( pFromSurface==pToSurface )
	{
		// get surface desc
		D3DSURFACE_DESC surfacedesc;
		pFromSurface->GetDesc ( &surfacedesc );

		// create temp surface
		m_pD3D->CreateRenderTarget ( surfacedesc.Width, surfacedesc.Height, surfacedesc.Format, D3DMULTISAMPLE_NONE, 0, TRUE, &pTempSrcSurface, NULL );
		if ( pTempSrcSurface )
			m_pD3D->StretchRect(pFromSurface, NULL, pTempSrcSurface, NULL, D3DTEXF_NONE);

		// from is now temp
		pFromSurface = pTempSrcSurface;
	}

	
	// Check if same size areas
	if(iX2-iX1==iToX2-iToX1)
	{
		// Same size copy region

		// get surfaces from textures
		RECT FromRect = { iX1, iY1, iX2, iY2 };
		RECT ToRect = { iToX1, iToY1, iToX1+(iX2-iX1), iToY1+(iY2-iY1) };

		// copy one surface to the other
		m_pD3D->StretchRect(pFromSurface, &FromRect, pToSurface, &ToRect, D3DTEXF_NONE);
	}
	else
	{
		// Different size copy (need to stretch)
		int iDestWidth = iToX2-iToX1;
		int iDestHeight = iToY2-iToY1;

		// Different size copy (need to stretch)
		double iSrcIncX = (double)(iX2-iX1)/(double)iDestWidth;
		double iSrcIncY = (double)(iY2-iY1)/(double)iDestHeight;
		double iSrcX = 0;
		double iSrcY = 0;

		// Define Rects
		RECT FromRect = { iX1, iY1, iX2, iY2 };
		RECT ToRect = { iToX1, iToY1, iToX2, iToY2 };

		// lock textures and perform stretched bit copy
		D3DLOCKED_RECT d3dlrFrom;
		D3DLOCKED_RECT d3dlrTo;
		if(SUCCEEDED(hRes=pFromSurface->LockRect ( &d3dlrFrom, &FromRect, 0 ) ) )
		{
			if(SUCCEEDED(hRes=pToSurface->LockRect ( &d3dlrTo, &ToRect, 0 ) ) )
			{
				LPSTR pStartFromPtr = (LPSTR)d3dlrFrom.pBits;
				LPSTR pToPtr = (LPSTR)d3dlrTo.pBits;
				LPSTR pFromPtr = pStartFromPtr;
				for(int y=0; y<iDestHeight; y++)
				{
					iSrcX = 0;
					for(int x=0; x<iDestWidth; x++)
					{
						// need to add slerpish code to vector source pixel..
						if(g_CSBPP==2) *(WORD*)pToPtr = *((WORD*)pFromPtr+(DWORD)(iSrcX));
						if(g_CSBPP==4) *(DWORD*)pToPtr = *((DWORD*)pFromPtr+(DWORD)(iSrcX));
						iSrcX += iSrcIncX;
						pToPtr += g_CSBPP;
					}
					iSrcY+=iSrcIncY;
					pFromPtr=pStartFromPtr+((DWORD)iSrcY*d3dlrFrom.Pitch);
					pToPtr+=d3dlrTo.Pitch - (iDestWidth*g_CSBPP);
				}
				pToSurface->UnlockRect();
			}
			pFromSurface->UnlockRect();
		}
	}

	// free temp surface
	SAFE_RELEASE ( pTempSrcSurface );
}

DARKSDK void DeleteBitmapEx ( int iID )
{
	if(iID<1 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return;
	}

    RemoveBitmap( iID );

	// restore current bitmap
	SetCurrentBitmap ( 0 );
}

DARKSDK void FlipBitmap ( int iID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return;
	}

	// Backbuffer or Bitmap To/Dest Surface
	LPDIRECT3DSURFACE9 pToSurface = m_ptr->lpSurface;
	if(iID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

	// flip bitmap
	DirectFlip( pToSurface, m_ptr->iWidth, m_ptr->iHeight );

	// update state value
	m_ptr->iFlipped = 1-m_ptr->iFlipped;
}

DARKSDK void MirrorBitmap ( int iID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return;
	}

	// Backbuffer or Bitmap To/Dest Surface
	LPDIRECT3DSURFACE9 pToSurface = m_ptr->lpSurface;
	if(iID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

	// mirror bitmap
	DirectMirror(pToSurface, m_ptr->iWidth, m_ptr->iHeight);

	// update state value
	m_ptr->iMirrored = 1-m_ptr->iMirrored;
}

DARKSDK void FadeBitmap ( int iID, int iFade )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return;
	}

	// Backbuffer or Bitmap To/Dest Surface
	LPDIRECT3DSURFACE9 pToSurface = m_ptr->lpSurface;
	if(iID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

	// fade bitmap
	DirectFade ( pToSurface, iFade, m_ptr->iWidth, m_ptr->iHeight );

	// update state value
	m_ptr->iFadeValue = iFade;
}

DARKSDK void BlurBitmap ( int iID, int iBlurLevel )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return;
	}

	// Backbuffer or Bitmap To/Dest Surface
	LPDIRECT3DSURFACE9 pToSurface = m_ptr->lpSurface;
	if(iID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

	// blur bitmap
	DirectBlur ( pToSurface, iBlurLevel, m_ptr->iWidth, m_ptr->iHeight );
}

DARKSDK void SetCurrentBitmap ( int iID )
{
	if ( iID>=0 )
	{
		// specifies bitmap number
		if(iID<0 || iID>MAXIMUMVALUE)
		{
			RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
			return;
		}
		if ( !UpdatePtr ( iID ) )
		{
			RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
			return;
		}
	}
	else
	{
		// specifies inverse camera number
	}

	// Bitmap Zero is default backbuffer
	if ( iID==0 )
	{
		// render target is backbuffer
		m_pD3D->SetRenderTarget ( 0, g_pGlob->pHoldBackBufferPtr );
		m_pD3D->SetDepthStencilSurface ( g_pGlob->pHoldDepthBufferPtr );
		g_pGlob->pCurrentBitmapSurface=g_pGlob->pHoldBackBufferPtr;
		g_pGlob->iCurrentBitmapNumber=iID;
	}
	else
	{
		if ( iID > 0 )
		{
			// render target is bitmap
			m_pD3D->SetRenderTarget ( 0, m_ptr->lpSurface );
			m_pD3D->SetDepthStencilSurface ( m_ptr->lpDepth );
			g_pGlob->pCurrentBitmapSurface=m_ptr->lpSurface;
			g_pGlob->iCurrentBitmapNumber=iID;
		}
		else
		{
			// U70 - 190908 - render target is camera surface
			int iRenderBitmapActivityToCameraID = abs(iID);
			if ( iRenderBitmapActivityToCameraID > 0 )
			{
				tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( iRenderBitmapActivityToCameraID );
				if ( m_Camera_Ptr )
				{
					m_pD3D->SetRenderTarget ( 0, m_Camera_Ptr->pCameraToImageSurface );
					m_pD3D->SetDepthStencilSurface ( NULL );
					g_pGlob->pCurrentBitmapSurface=m_Camera_Ptr->pCameraToImageSurface;
				}
				else
					iRenderBitmapActivityToCameraID = 0;
			}
			if ( iRenderBitmapActivityToCameraID==0 )
			{
				// DEFAULT back to render target is default backbuffer
				m_pD3D->SetRenderTarget ( 0, g_pGlob->pHoldBackBufferPtr );
				m_pD3D->SetDepthStencilSurface ( g_pGlob->pHoldDepthBufferPtr );
				g_pGlob->pCurrentBitmapSurface=g_pGlob->pHoldBackBufferPtr;
			}
			g_pGlob->iCurrentBitmapNumber=0;
		}
	}
}


//
// Command Expression Functions
//

DARKSDK int CurrentBitmap ( void )
{
	// Current birmap number
	return g_pGlob->iCurrentBitmapNumber;
}

DARKSDK int BitmapExist ( int iID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return 0;
	}

	// returns true if the bitmap exists
	if ( !UpdatePtr ( iID ) )
		return 0;

	// return true
	return 1;
}

DARKSDK int BitmapWidth ( int iID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return 0;
	}
	return m_ptr->iWidth;
}

DARKSDK int BitmapHeight ( int iID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return 0;
	}
	return m_ptr->iHeight;
}

DARKSDK int BitmapDepth ( int iID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return 0;
	}
	return m_ptr->iDepth;
}

DARKSDK int BitmapMirrored ( int iID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return 0;
	}
	return m_ptr->iMirrored;
}

DARKSDK int BitmapFlipped ( int iID )
{
	if(iID<0 || iID>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_BITMAPILLEGALNUMBER);
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_BITMAPNOTEXIST);
		return 0;
	}
	return m_ptr->iFlipped;
}

DARKSDK int BitmapExist ( void )
{
	return BitmapExist ( g_pGlob->iCurrentBitmapNumber );
}

DARKSDK int BitmapWidth ( void )
{
	return BitmapWidth ( g_pGlob->iCurrentBitmapNumber );
}

DARKSDK int BitmapHeight ( void )
{
	return BitmapHeight ( g_pGlob->iCurrentBitmapNumber );
}

DARKSDK int BitmapDepth ( void )
{
	return BitmapDepth ( g_pGlob->iCurrentBitmapNumber );
}

DARKSDK int BitmapMirrored ( void )
{
	return BitmapMirrored ( g_pGlob->iCurrentBitmapNumber );
}

DARKSDK int BitmapFlipped ( void )
{
	return BitmapFlipped ( g_pGlob->iCurrentBitmapNumber );
}

// Data Access Functions

DARKSDK void GetBitmapData( int iID, DWORD* dwWidth, DWORD* dwHeight, DWORD* dwDepth, LPSTR* pData, DWORD* dwDataSize, bool bLockData )
{
	// Read Data
	if(bLockData==true)
	{
		if ( !UpdatePtr ( iID ) )
			return;

		// Backbuffer or Bitmap To/Dest Surface
		LPDIRECT3DSURFACE9 pToSurface = m_ptr->lpSurface;
		if(iID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

		// bitmap data
		*dwWidth = m_ptr->iWidth;
		*dwHeight = m_ptr->iHeight;
		*dwDepth = m_ptr->iDepth;

		// lock
		D3DLOCKED_RECT d3dlock;
		if(SUCCEEDED(pToSurface->LockRect ( &d3dlock, NULL, 0 ) ) )
		{
			// create memory
			DWORD bpp = m_ptr->iDepth/8;
			DWORD dwSizeOfBitmapData = m_ptr->iWidth*m_ptr->iHeight*bpp;
			*pData = new char[dwSizeOfBitmapData];
			*dwDataSize = dwSizeOfBitmapData;

			// copy from surface
			LPSTR pSrc = (LPSTR)d3dlock.pBits;
			LPSTR pPtr = *pData;
			for(int y=0; y<m_ptr->iHeight; y++)
			{
				memcpy(pPtr, pSrc, m_ptr->iWidth*bpp);
				pPtr+=m_ptr->iWidth*bpp;
				pSrc+=d3dlock.Pitch;
			}
			pToSurface->UnlockRect();
		}
	}
	else
	{
		// free memory
		delete *pData;
	}
}

DARKSDK void SetBitmapData( int iID, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, LPSTR pData, DWORD dwDataSize )
{
	LPDIRECT3DSURFACE9 pToSurface=NULL;
	if ( iID>0 )
	{
		if( UpdatePtr ( iID ) )
		{
			// Backbuffer or Bitmap To/Dest Surface
			pToSurface = m_ptr->lpSurface;
			if(iID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

			// Check new bitmap specs with existing one
			if(dwWidth==(DWORD)m_ptr->iWidth && dwHeight==(DWORD)m_ptr->iHeight && dwDepth==(DWORD)m_ptr->iDepth)
			{
				// Same bitmap size
			}
			else
			{
				// Recreate Bitmap
				DeleteBitmapEx ( iID );
				m_ptr=NULL;
			}
		}
		if(m_ptr==NULL)
		{
			D3DFORMAT dFormat;
			g_bOffscreenBitmap=false;
			if ( dwDepth==16 ) dFormat=D3DFMT_R5G6B5;
			if ( dwDepth==24 ) dFormat=D3DFMT_R8G8B8;
			if ( dwDepth==32 ) dFormat=D3DFMT_X8R8G8B8;
			MakeFormat ( iID, dwWidth, dwHeight, dFormat );
		}
	}

	// Failed to create bitmap?
	if ( !UpdatePtr ( iID ) ) return;

	// Bitmap may have changed
	pToSurface = m_ptr->lpSurface;
	if(iID==0) pToSurface=g_pGlob->pHoldBackBufferPtr;

	// Write Data
	D3DLOCKED_RECT d3dlock;
	if(SUCCEEDED(pToSurface->LockRect ( &d3dlock, NULL, 0 ) ) )
	{
		// copy from surface
		DWORD databpp = dwDepth/8;
		LPSTR pSrc = (LPSTR)d3dlock.pBits;
		LPSTR pPtr = pData;
		for(DWORD y=0; y<dwHeight; y++)
		{
			memcpy(pSrc, pPtr, dwWidth*databpp);
			pPtr+=dwWidth*databpp;
			pSrc+=d3dlock.Pitch;
		}
		pToSurface->UnlockRect();
	}

}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorBitmap ( HINSTANCE hInstance )
{
	Constructor ( hInstance );
}

void DestructorBitmap  ( void )
{
	Destructor ( );
}

void SetErrorHandlerBitmap ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataBitmap( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DBitmap ( int iMode )
{
	RefreshD3D ( iMode );
}

void UpdateBitmapZeroOfNewBackbufferBitmap( void )
{
	UpdateBitmapZeroOfNewBackbuffer ( );
}

void dbCreateBitmap ( int iID, int iWidth, int iHeight )
{
	CreateBitmap ( iID, iWidth, iHeight );
}

void dbLoadBitmap ( char* szFilename, int iID )
{
	LoadBitmap ( szFilename, iID );
}

void dbSaveBitmap ( char* szFilename, int iID )
{
	SaveBitmap ( szFilename, iID );
}

void dbLoadBitmap ( char* szFilename )
{
	LoadBitmapEx ( szFilename );
}

void dbSaveBitmap ( char* szFilename )
{
	SaveBitmapEx ( szFilename );
}

void dbCopyBitmap ( int iID, int iToID )
{
	CopyBitmap ( iID, iToID );
}

void dbCopyBitmap ( int iID, int iX1, int iY1, int iX2, int iY2, int iToID, int iToX1, int iToY1, int iToX2, int iToY2 )
{
	CopyBitmapEx ( iID, iX1, iY1, iX2, iY2, iToID, iToX1, iToY1, iToX2, iToY2 );
}

void dbDeleteBitmap ( int iID )
{
	DeleteBitmapEx ( iID );
}

void dbFlipBitmap ( int iID )
{
	FlipBitmap ( iID );
}

void dbMirrorBitmap ( int iID )
{
	MirrorBitmap ( iID );
}

void dbFadeBitmap ( int iID, int iFade )
{
	FadeBitmap ( iID, iFade );
}

void dbBlurBitmap ( int iID, int iBlurLevel )
{
	BlurBitmap ( iID, iBlurLevel );
}

void dbSetCurrentBitmap ( int iID )
{
	SetCurrentBitmap ( iID );
}

int dbCurrentBitmap ( void )
{
	return CurrentBitmap ( );
}

int dbBitmapExist ( int iID )
{
	return BitmapExist ( iID );
}

int dbBitmapWidth ( int iID )
{
	return BitmapWidth ( iID );
}

int dbBitmapHeight ( int iID )
{
	return BitmapHeight ( iID );
}

int dbBitmapDepth ( int iID )
{
	return BitmapDepth ( iID );
}

int dbBitmapMirrored ( int iID )
{
	return BitmapMirrored ( iID );
}

int dbBitmapFlipped ( int iID )
{
	return BitmapFlipped ( iID );
}

int dbBitmapExist ( void )
{
	return BitmapExist ( );
}

int dbBitmapWidth ( void )
{
	return BitmapWidth ( );
}

int dbBitmapHeight ( void )
{
	return BitmapHeight ( );
}

int dbBitmapDepth ( void )
{
	return BitmapDepth ( );
}

int dbBitmapMirrored ( void )
{
	return BitmapMirrored ( );
}

int dbBitmapFlipped ( void )
{
	return BitmapFlipped ( );
}

void dbGetBitmapData ( int iID, DWORD* dwWidth, DWORD* dwHeight, DWORD* dwDepth, LPSTR* pData, DWORD* dwDataSize, bool bLockData )
{
	GetBitmapData ( iID, dwWidth, dwHeight, dwDepth, pData, dwDataSize, bLockData );
}

void dbSetBitmapData ( int bitmapindex, DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, LPSTR pData, DWORD dwDataSize )
{
	SetBitmapData ( bitmapindex, dwWidth, dwHeight, dwDepth, pData, dwDataSize );
}

void				dbSetBitmapFormat							( int iFormat )
{
	SetBitmapFormat ( iFormat );
}

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
