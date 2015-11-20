
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include ".\..\DBOFormat\DBOFormat.h"
#include ".\..\error\cerror.h"
#include ".\..\core\globstruct.h"
#include "ccamerac.h"
#include "Stereoscopics.h"
#include "TextureBackdrop.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKImage.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDK3DMaths.h"
#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBasic3D.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

typedef IDirect3DDevice9*		( *GFX_GetDirect3DDevicePFN )		( void );
typedef void*					( *BASIC3D_GetInternalDataPFN )			( int );
typedef void					( *IMAGE_RetVoidParamIntPFN )			( int );
typedef int  					( *IMAGE_RetIntParamIntPFN )			( int );
typedef bool  					( *IMAGE_RetBoolParamIntPFN )			( int );
typedef LPDIRECT3DTEXTURE9		( *IMAGE_RetLPD3DTEX9ParamIntPFN )		( int );
typedef LPDIRECT3DTEXTURE9		( *IMAGE_RetVoidParamInt3D3DFPFN )		( int, int, int, D3DFORMAT );
typedef void					( *IMAGE_RetVoidParamGetICUBEIPFN )		( int, LPDIRECT3DCUBETEXTURE9*, int* );
typedef LPDIRECT3DTEXTURE9	    ( *IMAGE_RetLPD3DTEX9ParamIntPFN )		( int );
typedef D3DXVECTOR3				( *GetVectorPFN )						( int );
typedef void					( *SetVectorPFN )						( int, float, float, float );
typedef int						( *GetExistPFN )						( int );

DBPRO_GLOBAL GlobStruct*					g_pGlob							= NULL;
DBPRO_GLOBAL int							m_iRenderCamera					= 0;
DBPRO_GLOBAL int							m_iCurrentCamera				= 0;
DBPRO_GLOBAL bool							m_bAutoCamState					= true;
DBPRO_GLOBAL bool							m_bActivateBackdrop				= true;
DBPRO_GLOBAL float							lastmovetargetx					= 0.0f;
DBPRO_GLOBAL float							lastmovetargety					= 0.0f;
DBPRO_GLOBAL float							lastmovetargetz					= 0.0f;
DBPRO_GLOBAL HINSTANCE						g_GFX;
DBPRO_GLOBAL GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;
DBPRO_GLOBAL BASIC3D_GetInternalDataPFN		g_Basic3D_GetInternalData;
DBPRO_GLOBAL sObject*						m_Object_Ptr;
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN		g_Image_Delete;	
DBPRO_GLOBAL IMAGE_RetVoidParamInt3D3DFPFN	g_Image_MakeRenderTarget;
DBPRO_GLOBAL IMAGE_RetVoidParamInt3D3DFPFN	g_Image_MakeJustFormat;
DBPRO_GLOBAL IMAGE_RetBoolParamIntPFN		g_Image_GetExist;
DBPRO_GLOBAL IMAGE_RetVoidParamGetICUBEIPFN	g_Image_GetCubeFace;
DBPRO_GLOBAL IMAGE_RetLPD3DTEX9ParamIntPFN	g_Image_GetPointer;
DBPRO_GLOBAL GetVectorPFN					g_Types_GetVector;
DBPRO_GLOBAL SetVectorPFN					g_Types_SetVector;
DBPRO_GLOBAL GetExistPFN					g_Types_GetExist;
DBPRO_GLOBAL CCameraManager					m_CameraManager;
DBPRO_GLOBAL tagCameraData*					m_ptr;
DBPRO_GLOBAL LPDIRECT3DDEVICE9				m_pD3D;
DBPRO_GLOBAL LPDIRECT3D9					m_pDX;
DBPRO_GLOBAL bool							g_bCameraOutputToImage			= false;
DBPRO_GLOBAL LPDIRECT3DSURFACE9				m_pImageDepthSurface;
DBPRO_GLOBAL DWORD							m_dwImageDepthWidth;
DBPRO_GLOBAL DWORD							m_dwImageDepthHeight;
DBPRO_GLOBAL int							g_iCameraHasClipPlane			= 0;		// added 310506 - u62
DBPRO_GLOBAL DWORD							g_dwMaxUserClipPlanes			= 0;		// added 310506 - u62
DBPRO_GLOBAL tagCameraData*					g_pStereoscopicCameraUpdated	= NULL;		// added 180508 - u69
DBPRO_GLOBAL IDirect3DTexture9*				g_pStereoscopicFinalTexture		= NULL;		// U7.0 - 180608 - WOW autostereo

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void* GetInternalData ( int iID )
{
	// get the data for the camera, this can be useful if
	// you want to access the structure directly

	// update internal pointer
	if ( !UpdatePtr ( iID ) )
		return NULL;

	// return a pointer to the data
	return m_ptr;
}

DARKSDK void ConstructorD3D ( HINSTANCE hSetup, HINSTANCE hImage )
{
	// setup the DLL
	bool bFromCore = false;
	if(hSetup) bFromCore=true;

	// clear the data pointer to null
	m_ptr = NULL;

	// see if the handles to the DLLs are ok
	#ifndef DARKSDK_COMPILE
	if ( !hSetup || !hImage )
	{
		// if not try and load them manually
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );		// load the setup library
		hImage = LoadLibrary ( "DBProImageDebug.dll" );		// load the image library
	}
	#endif

	// link in the functions, for this DLL we only need to be able to access
	// the direct 3D device so we only link into 1 function
	#ifndef DARKSDK_COMPILE
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	#else
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;
		bFromCore = true;
	#endif

	
	m_pD3D                  = g_GFX_GetDirect3DDevice ( );
	m_pD3D->GetDirect3D(&m_pDX);

	// check the device is valid
	if ( !m_pD3D )
		Error ( "Failed to access D3D for camera DLL" );

	// link up image functions
	#ifndef DARKSDK_COMPILE
		g_Image_Delete	             = ( IMAGE_RetVoidParamIntPFN )			GetProcAddress ( hImage, "?Delete@@YAXH@Z" );
		g_Image_MakeRenderTarget     = ( IMAGE_RetVoidParamInt3D3DFPFN )	GetProcAddress ( hImage, "?MakeRenderTarget@@YAPAUIDirect3DTexture9@@HHHW4_D3DFORMAT@@@Z" );
		g_Image_MakeJustFormat       = ( IMAGE_RetVoidParamInt3D3DFPFN )	GetProcAddress ( hImage, "?MakeJustFormat@@YAPAUIDirect3DTexture9@@HHHW4_D3DFORMAT@@@Z" );
		g_Image_GetExist             = ( IMAGE_RetBoolParamIntPFN )			GetProcAddress ( hImage, "?GetExist@@YA_NH@Z" );
		g_Image_GetCubeFace          = ( IMAGE_RetVoidParamGetICUBEIPFN )	GetProcAddress ( hImage, "?GetCubeFace@@YAXHPAPAUIDirect3DCubeTexture9@@PAH@Z" );
		g_Image_GetPointer           = ( IMAGE_RetLPD3DTEX9ParamIntPFN )	GetProcAddress ( hImage, "?GetPointer@@YAPAUIDirect3DTexture9@@H@Z" );
	#else
		g_Image_Delete	             = dbDeleteImage;
		g_Image_MakeRenderTarget     = dbMakeImageRenderTarget;
		g_Image_MakeJustFormat       = dbMakeImageJustFormat;
		g_Image_GetExist             = ( IMAGE_RetBoolParamIntPFN )	dbGetImageExist;
		g_Image_GetCubeFace          = dbGetImageCubeFace;
	#endif

	// create a default camera
	if(bFromCore)
	{
		// Core uses a cameraID of zero
		m_iCurrentCamera=0;
		Create ( m_iCurrentCamera );
	}
	else
	{
		// DarkSDK uses a cameraID of 1
		m_iCurrentCamera=1;
		Create ( m_iCurrentCamera );
	}

	// set the glob struct camera ID to zero
	if ( g_pGlob ) g_pGlob->dwCurrentSetCameraID = 0;

	// lee - 290306 - u6rc3 - reset key values
	m_bActivateBackdrop = true; // ensure backdrop activates when use autocam a fresh
}

DARKSDK void Constructor ( HINSTANCE hSetup, HINSTANCE hImage )
{
	ConstructorD3D ( hSetup, hImage );
}

DARKSDK void FreeCameraSurfaces ( void )
{
	// free image surface from camera first - using m_ptr
	if(m_ptr->pCameraToImageSurface)
	{
		m_ptr->pCameraToImageSurface->Release();
		m_ptr->pCameraToImageSurface=NULL;
	}
	if ( m_ptr->pCameraToImageAlphaTexture )
	{
		if(m_ptr->pCameraToImageTexture)
		{
			m_ptr->pCameraToImageTexture->Release();
			m_ptr->pCameraToImageTexture=NULL;
		}
		if(m_ptr->pCameraToImageAlphaSurface)
		{
			m_ptr->pCameraToImageAlphaSurface->Release();
			m_ptr->pCameraToImageAlphaSurface=NULL;
		}
	}

	// U69 - 180508 - free textures and surfaces of any stereoscopics
	if(m_ptr->pCameraToStereoImageBackSurface)
	{
		m_ptr->pCameraToStereoImageBackSurface->Release();
		m_ptr->pCameraToStereoImageBackSurface=NULL;
	}
	if(m_ptr->pCameraToStereoImageFrontSurface)
	{
		m_ptr->pCameraToStereoImageFrontSurface->Release();
		m_ptr->pCameraToStereoImageFrontSurface=NULL;
	}
	/* V111 - 110608 - these deleted by Image DLL that created them
	if(m_ptr->pCameraToStereoImageBackTexture)
	{
		m_ptr->pCameraToStereoImageBackTexture->Release();
		m_ptr->pCameraToStereoImageBackTexture=NULL;
	}
	if(m_ptr->pCameraToStereoImageFrontTexture)
	{
		m_ptr->pCameraToStereoImageFrontTexture->Release();
		m_ptr->pCameraToStereoImageFrontTexture=NULL;
	}
	*/
}

DARKSDK void DestructorD3D ( void )
{
	for (int iID = m_CameraManager.GetNextID( -1 ); iID != -1; iID = m_CameraManager.GetNextID( iID ))
	{
		tagCameraData* ptr = m_CameraManager.GetData( iID );

		if (ptr)
		{
			m_ptr = ptr;
			FreeCameraSurfaces();
		}
	}

	// delete any items in the list
	if ( m_pImageDepthSurface )
	{
		m_pImageDepthSurface->Release();
		m_pImageDepthSurface=NULL;
	}

	// U69 - 180508 - free stereosopics
	FreeStereoscopicResources();

	// U75 - 040609 - free resources
	FreeTextureBackdrop();

	// release ref
	SAFE_RELEASE(m_pDX);
}

DARKSDK void Destructor ( void )
{
	DestructorD3D();
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

	#ifndef DARKSDK_COMPILE
		// Assign function ptrs for vector handling
		g_Types_GetVector					= ( GetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetVector3@@YA?AUD3DXVECTOR3@@H@Z" );
		g_Types_SetVector					= ( SetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?SetVector3@@YAXHMMM@Z" );
		g_Types_GetExist					= ( GetExistPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetExist@@YAHH@Z" );

		// link up object ptr function
		g_Basic3D_GetInternalData = ( BASIC3D_GetInternalDataPFN )	GetProcAddress ( g_pGlob->g_Basic3D, "?GetInternalData@@YAPAXH@Z" );
	#else

		

		g_Types_GetVector = dbGetVector3;
		g_Types_SetVector = dbSetVector3;
		g_Types_GetExist = dbGet3DMathsExist;

		g_Basic3D_GetInternalData = dbObjectGetInternalData;
	#endif
}

DARKSDK void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		DestructorD3D();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		ConstructorD3D ( g_pGlob->g_GFX, g_pGlob->g_Image );
		PassCoreData ( g_pGlob );
	}
}

DARKSDK IDirect3DTexture9* GetStereoscopicFinalTexture ( void )
{
	// U70 - 180608 - WOW autostereo
	return g_pStereoscopicFinalTexture;
}

DARKSDK void SetViewportForSurface ( D3DVIEWPORT9* pvp, LPDIRECT3DSURFACE9 pSurface )
{
	// viewport should never be better than target surface
	D3DSURFACE_DESC ddsd;
	pSurface->GetDesc ( &ddsd );
	if ( pvp->Width > ddsd.Width ) pvp->Width=ddsd.Width;
	if ( pvp->Height > ddsd.Height ) pvp->Height=ddsd.Height;

	// set viewport for view
	m_pD3D->SetViewport ( pvp );
}

DARKSDK bool CameraToImage ( void )
{
	if ( m_ptr->iCameraToImage > 0 )
	{
		// determine if camera goes straight to cubemap face
		int iCubeMapFace=-1;
		LPDIRECT3DCUBETEXTURE9 pCubeMapRef = NULL;
		if ( g_Image_GetCubeFace )
		{
			g_Image_GetCubeFace ( m_ptr->iCameraToImage, &pCubeMapRef, &iCubeMapFace );
			if ( pCubeMapRef )
			{
				// camera output is direct to cube map
				LPDIRECT3DSURFACE9 pFace;
				pCubeMapRef->GetCubeMapSurface( (D3DCUBEMAP_FACES)iCubeMapFace, 0, &pFace );
				if ( pFace )
				{
					// set redner targets
					m_pD3D->SetRenderTarget ( 0, pFace );
					m_pD3D->SetDepthStencilSurface ( m_pImageDepthSurface );
				}
			}
			else
			{
				// camera output is to image(texture surface)
				m_pD3D->SetRenderTarget ( 0, m_ptr->pCameraToImageSurface );
				m_pD3D->SetDepthStencilSurface ( m_pImageDepthSurface );

				// if present, copy alpha surface 
				if ( m_ptr->pCameraToImageAlphaSurface )
				{
					// copy surface to alpha surface
					HRESULT hRes = D3DXLoadSurfaceFromSurface(m_ptr->pCameraToImageAlphaSurface, NULL, NULL, m_ptr->pCameraToImageSurface, NULL, NULL, D3DX_FILTER_NONE, 0xFF000000 );

					/* commenetd out for safe keeping (direct alpha manipulation of render target)
					// now lock the surface
					D3DLOCKED_RECT d3dlr;
					if ( SUCCEEDED ( m_ptr->pCameraToImageAlphaSurface->LockRect ( &d3dlr, 0, 0 ) ) )
					{
						// get a pointer to surface data
						DWORD* pPix = ( DWORD* ) d3dlr.pBits;
						DWORD  pGet;

						for ( DWORD y = 0; y < (DWORD)256; y++ )
						{
							for ( DWORD x = 0; x < (DWORD)256; x++ )
							{
								pGet = *pPix;

								DWORD dwAlpha = pGet >> 24;
								DWORD dwRed   = ((( pGet ) >> 16 ) & 0xff );
								DWORD dwGreen = ((( pGet ) >>  8 ) & 0xff );
								DWORD dwBlue  = ((( pGet ) )       & 0xff );
								
//								if ( *pPix != 0 )
//									*pPix++ = D3DCOLOR_ARGB ( 128, dwRed, dwGreen, dwBlue );
//								else
//									*pPix++;

								*pPix++ = D3DCOLOR_ARGB ( 128, dwRed, dwGreen, dwBlue );
							}
						}
					}
					m_ptr->pCameraToImageAlphaSurface->UnlockRect();
					*/
				}
			}
		}
		g_bCameraOutputToImage=true;

		// U69 - 180508 - if this camera is the right camera of a stereoscopic render, trigger the convert after this camera render
		if ( m_ptr->iStereoscopicMode > 0 )
			g_pStereoscopicCameraUpdated = m_ptr;

		// camera is image
		return false;
	}

	// camera is not image
	return true;
}

void HandleClippingPlane ( void )
{
	// leeadd - 310506 - u62 - optional camera clipping operation
	// called before camera (m_ptr) renders
	if ( m_ptr->iClipPlaneOn!=0 )
	{
		if ( g_iCameraHasClipPlane!=m_ptr->iClipPlaneOn )
		{
			// leeadd - 161008 - u70 - when using clippping with shaders (HLSL)
			// they should be in clip space, not world space
			bool bClipSpaceRequired = false;
			int iActualClipPlaneOn = m_ptr->iClipPlaneOn;
			if ( m_ptr->iClipPlaneOn==3 ) { bClipSpaceRequired=true; iActualClipPlaneOn=1; }
			if ( m_ptr->iClipPlaneOn==4 ) { bClipSpaceRequired=true; iActualClipPlaneOn=2; }

			// transform world to clip space
			D3DXPLANE planeUse = m_ptr->planeClip;
			if ( bClipSpaceRequired )
			{
				// temp planes for conversion
				D3DXPLANE tempPlane = planeUse;
				D3DXPLANE viewSpacePlane;

				// normalize the plane which is required for the transforms
				D3DXPlaneNormalize(&tempPlane, &tempPlane);

				// transform the plane into view space
				D3DXMATRIX tempMatrix = m_ptr->matView;
				D3DXMatrixInverse(&tempMatrix, NULL, &tempMatrix);
				D3DXMatrixTranspose(&tempMatrix, &tempMatrix);
				D3DXPlaneTransform(&viewSpacePlane, &tempPlane, &tempMatrix);

				// transform the plane into clip space, or post projection space
				tempMatrix = m_ptr->matProjection;
				D3DXMatrixInverse(&tempMatrix, NULL, &tempMatrix);
				D3DXMatrixTranspose(&tempMatrix, &tempMatrix);

				// place resulting clip space plane ready for setclipplane
				D3DXPlaneTransform(&planeUse, &viewSpacePlane, &tempMatrix);
			}

			// clipping plane enabled
			m_pD3D->SetClipPlane ( 0, (float*)planeUse );
			m_pD3D->SetRenderState ( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );

			// optional feature to auto-reflect the camera around the plane
			// this was overridden by code in UpdateInitOnce (so moved to that function)
			/*
			if ( iActualClipPlaneOn==2 )
			{
				// Reflect camera view in clip plane (mirror)
				D3DXMATRIX matView, matReflect;
				D3DXMatrixReflect ( &matReflect, &m_ptr->planeClip );
				D3DXMatrixMultiply ( &matView, &matReflect, &m_ptr->matView );
				m_pD3D->SetTransform ( D3DTS_VIEW, &matView );
			}
			*/

			// flag controls low-state hits
			g_iCameraHasClipPlane=m_ptr->iClipPlaneOn;
		}
	}
	else
	{
		if ( g_iCameraHasClipPlane>0 )
		{
			m_pD3D->SetRenderState ( D3DRS_CLIPPLANEENABLE, 0x00 );
			g_iCameraHasClipPlane=0;
		}
	}
}

DARKSDK void StartSceneEx ( int iMode )
{
	// iMode : 0-regular, 1-no camera backdrop (when disable camera rendering)

	// start render chain for cameras
	m_iRenderCamera=0;

	// update the pointer
	if ( UpdatePtr ( m_iRenderCamera ) )
	{
		// camera to image handling
		CameraToImage ();

		// Update Camera
		InternalUpdate( m_iRenderCamera );

		// set camera to view
		D3DVIEWPORT9 vp = m_ptr->viewPort3D;
		LPDIRECT3DSURFACE9 pDestSurface = NULL;
		if ( m_ptr->iCameraToImage > 0 )
			pDestSurface = m_ptr->pCameraToImageSurface;
		else
			pDestSurface = g_pGlob->pCurrentBitmapSurface;
		SetViewportForSurface ( &vp, pDestSurface );

		// leeadd - 310506 - u62 - optional camera clipping operation
		HandleClippingPlane();

		// clear the screen
		if ( iMode==0 )
		{
			if(m_ptr->iBackdropState==1)
				m_pD3D->Clear ( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ptr->dwBackdropColor, 1.0f, 0 );
			else
				m_pD3D->Clear ( 0, NULL, D3DCLEAR_ZBUFFER, m_ptr->dwBackdropColor, 1.0f, 0 );
		}

		// U74- 040709 - draw backdrop texture if required
		if ( m_ptr->iBackdropTextureMode!=0 ) DrawTextureBackdrop ( m_ptr->iBackdropTextureMode, pDestSurface, m_ptr->pBackdropTexture );
	}
	else
	{
		// no camera as yet
		m_iRenderCamera=-1;
	}

	// allow globstruct to keep track of which camera doing the rendering
	if(g_pGlob) g_pGlob->dwRenderCameraID=m_iRenderCamera;
}

DARKSDK void StartScene ( void )
{
	// see above
	StartSceneEx ( 0 );

	// U69 - 180508 - start a new camera render loop, reset stereoscopic update flag
	g_pStereoscopicCameraUpdated = NULL;
}

DARKSDK int FinishSceneEx ( bool bKnowInAdvanceCameraIsUsed )
{
	// leeadd - 130906 - u63 - bKnowInAdvanceCameraIsUsed set from SYNC MASK logic in core
	m_iRenderCamera = m_CameraManager.GetNextID ( m_iRenderCamera );
	if(g_pGlob) g_pGlob->dwRenderCameraID=m_iRenderCamera;
	if(m_iRenderCamera != -1 )
	{
		// update the pointer for Next Camera
		UpdatePtr ( m_iRenderCamera );

		// 20120313 IanM - all code from here moved into the Update function

		// another camera to render
		return 0;
	}
	else
	{
		// U69 - 180508 - if processed a 'stereosopic right camera', need to convert left/right to back/front before leave camera render loop
		if ( g_pStereoscopicCameraUpdated )
		{
			// WOW - must use a shader to create a greyscale version of scene depth, then do side by side stretchrect
			if ( g_pStereoscopicCameraUpdated->iStereoscopicMode==3 )
			{
				// take left surface, convert to WOW textures (back=grey and front=sidebyside)
				tagCameraData* pPtrL = g_pStereoscopicCameraUpdated->pStereoscopicFirstCamera;
				tagCameraData* pPtrR = g_pStereoscopicCameraUpdated;
				StereoscopicConvertLeftToGreySideBySide (	pPtrR->iStereoscopicMode,
															pPtrL->pCameraToImageTexture,
															pPtrR->pCameraToStereoImageBackSurface, pPtrR->pCameraToStereoImageFrontSurface );

				// this texture ptr is read by SETUPDLL when seting the WOW final texture
				g_pStereoscopicFinalTexture = pPtrR->pCameraToStereoImageFrontTexture;
			}
			else
			{
				// take left/right surfaces, convert to S3D surfaces (images passed in and created)
				tagCameraData* pPtrL = g_pStereoscopicCameraUpdated->pStereoscopicFirstCamera;
				tagCameraData* pPtrR = g_pStereoscopicCameraUpdated;
				StereoscopicConvertLeftRightToBackFront (	pPtrR->iStereoscopicMode,
															pPtrL->pCameraToImageTexture, pPtrR->pCameraToImageTexture,
															pPtrR->pCameraToStereoImageBackSurface, pPtrR->pCameraToStereoImageFrontSurface );
			}

			// done for this cycle
			g_pStereoscopicCameraUpdated = NULL;
		}

		// restore camera output to current bitmap (if changed)
		if ( g_bCameraOutputToImage==true )
		{
			m_pD3D->SetRenderTarget ( 0, g_pGlob->pCurrentBitmapSurface );
			m_pD3D->SetDepthStencilSurface ( g_pGlob->pHoldDepthBufferPtr );
			g_bCameraOutputToImage=false;
		}

		// update the pointer for first camera
		UpdatePtr ( 0 );

		// restore camera for 2D operations
		D3DVIEWPORT9 vp = m_ptr->viewPort2D;
		SetViewportForSurface ( &vp, g_pGlob->pCurrentBitmapSurface );

		// leeadd - 310506 - u62 ensure clip is removed before 2D operations commence
		if ( g_iCameraHasClipPlane>0 )
		{
			m_pD3D->SetRenderState ( D3DRS_CLIPPLANEENABLE, 0x00 );
			g_iCameraHasClipPlane=0;
		}

		// NOTE: Render target can be changed after this using DRAW TO CAMERA 
		// which runs the code below RunCode(1)..

		// end of render camera chain
		return 1;
	}
}

DARKSDK int FinishScene ( void )
{
	// assume next camera is valid, so we set it up and clear render target
	// leenote - 130906 - u63 - discovered this would wipe out previous camera is using multiple SYNC MASK calls (post-process tricks)
	return FinishSceneEx ( true );
}


DARKSDK void Update ( void )
{
	// 20120313 IanM - This code taken from FinishSceneEx.
	// Core may determine that eg camera 1 is not in the mask and instruct FinishSceneEx
	// that the 'next' camera will not be rendered too, except that if camera 1 doesn't
	// exist then the next camera will then mistakenly not be rendered.

	// m_iRenderCamera and mptr have been updated by the FinishSceneEx function,
	// so no need to do so again.
	// This function is now called as the first item in the renderlist in core, so
	// will prep everything ready for rendering.
	if ( CameraToImage () )
	{
		// restore camera output to current bitmap
		m_pD3D->SetRenderTarget ( 0, g_pGlob->pCurrentBitmapSurface );
		m_pD3D->SetDepthStencilSurface ( g_pGlob->pHoldDepthBufferPtr );
		g_bCameraOutputToImage=false;
	}

	// Update Camera
	InternalUpdate( m_iRenderCamera );

	// set viewport for new camera
	D3DVIEWPORT9 vp = m_ptr->viewPort3D;
	if ( m_ptr->iCameraToImage > 0 )
	{
		if ( m_ptr->pCameraToImageSurface ) SetViewportForSurface ( &vp, m_ptr->pCameraToImageSurface );
	}
	else
		SetViewportForSurface ( &vp, g_pGlob->pCurrentBitmapSurface );

	// leeadd - 310506 - u62 - optional camera clipping operation
	HandleClippingPlane();

	// clear the screen
	if(m_ptr->iBackdropState==1)
		m_pD3D->Clear ( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_ptr->dwBackdropColor, 1.0f, 0 );
	else
		m_pD3D->Clear ( 0, NULL, D3DCLEAR_ZBUFFER, m_ptr->dwBackdropColor, 1.0f, 0 );
	 // this is a redundant function
}

DARKSDK int GetRenderCamera ( void )
{
	return m_iRenderCamera;
}

DARKSDK void RunCode ( int iCodeMode )
{
	// used to run code from within Camera DLL from elsewhere
	switch ( iCodeMode )
	{
		case 0 : // leeadd - 071108 - U71 - restore to normal behaviour (output to main bitmap surface)
		{
			m_pD3D->SetRenderTarget ( 0, g_pGlob->pCurrentBitmapSurface );
			m_pD3D->SetDepthStencilSurface ( g_pGlob->pHoldDepthBufferPtr );
			UpdatePtr ( 0 );
			if ( m_ptr )
			{
				D3DVIEWPORT9 vp = m_ptr->viewPort2D;
				SetViewportForSurface ( &vp, g_pGlob->pCurrentBitmapSurface );
			}
		}
		break;

		case 1 : // leeadd - 071108 - U71 - redirect SPRITE/2D/IMAGE rendering to non-Bitmap Render Target, specified with DRAW TO CAMERA (camera zero)
		UpdatePtr ( 0 );
		if ( m_ptr )
		{
			// only where camera is being rendered to an image
			D3DVIEWPORT9 vp = m_ptr->viewPort2D;
			if ( m_ptr->pCameraToImageSurface )
			{
				m_pD3D->SetRenderTarget ( 0, m_ptr->pCameraToImageSurface );
				m_pD3D->SetDepthStencilSurface ( g_pGlob->pHoldDepthBufferPtr );
				SetViewportForSurface ( &vp, m_ptr->pCameraToImageSurface );
			}
		}
		break;
	}
}

DARKSDK int GetPowerSquareOfSize( int Size )
{
	if ( Size <= 1 ) Size = 1;
	else if ( Size <= 2 ) Size = 2;
	else if ( Size <= 4 ) Size = 4;
	else if ( Size <= 8 ) Size = 8;
	else if ( Size <= 16 ) Size = 16;
	else if ( Size <= 32 ) Size = 32;
	else if ( Size <= 64 ) Size = 64;
	else if ( Size <= 128 ) Size = 128;
	else if ( Size <= 256 ) Size = 256;
	else if ( Size <= 512 ) Size = 512;
	else if ( Size <= 1024 ) Size = 1024;
	else if ( Size <= 2048 ) Size = 2048;
	else if ( Size <= 4096 ) Size = 4096;
	return Size;
}

DARKSDK float wrapangleoffset(float da)
{
	int breakout=100;
	while(da<0.0f || da>=360.0f)
	{
		if(da<0.0f) da=da+360.0f;
		if(da>=360.0f) da=da-360.0f;
		breakout--;
		if(breakout==0) break;
	}
	if(breakout==0) da=0.0f;
	return da;
}

DARKSDK void GetAngleFromPoint(float x1, float y1, float z1, float x2, float y2, float z2, float* ax, float* ay, float* az)
{
	D3DXVECTOR3 Vector;
	Vector.x = x2-x1;
	Vector.y = y2-y1;
	Vector.z = z2-z1;

	// Find Y and then X axis rotation
	double yangle=atan2(Vector.x, Vector.z);
	if(yangle<0.0) yangle+=D3DXToRadian(360.0);
	if(yangle>=D3DXToRadian(360.0)) yangle-=D3DXToRadian(360.0);

	D3DXMATRIX yrotate;
	D3DXMatrixRotationY ( &yrotate, (float)-yangle );
	D3DXVec3TransformCoord ( &Vector, &Vector, &yrotate );

	double xangle=-atan2(Vector.y, Vector.z);
	if(xangle<0.0) xangle+=D3DXToRadian(360.0);
	if(xangle>=D3DXToRadian(360.0)) xangle-=D3DXToRadian(360.0);

	*ax = wrapangleoffset(D3DXToDegree((float)xangle));
	*ay = wrapangleoffset(D3DXToDegree((float)yangle));
	*az = 0.0f;
}

DARKSDK void SetAutoCam ( float fX, float fY, float fZ, float fRadius )
{
	// Initial backdrop activator
	if(m_bActivateBackdrop==true)
	{
		m_bActivateBackdrop=false;
		BackdropOn();
	}

	if(m_bAutoCamState==true)
	{
		// update the pointer
		if ( !UpdatePtr ( m_iCurrentCamera ) )
			return;

		// Position of target
		D3DXVECTOR3 FullvPos;
		FullvPos.x = fX;
		FullvPos.y = fY;
		FullvPos.z = fZ;

		// Ensure object is in front of default camera
		m_ptr->vecPosition.x = FullvPos.x;
		m_ptr->vecPosition.y = FullvPos.y + (fRadius/2.0f);
		m_ptr->vecPosition.z = FullvPos.z - (fRadius*3.0f);
		if(m_ptr->vecPosition.z>-2.0f) m_ptr->vecPosition.z=-2.0f;

		// Point towards object
		GetAngleFromPoint ( m_ptr->vecPosition.x, m_ptr->vecPosition.y, m_ptr->vecPosition.z,
							fX, fY, fZ, &m_ptr->fXRotate, &m_ptr->fYRotate, &m_ptr->fZRotate);

		// update the camera
		InternalUpdate ( m_iCurrentCamera );
	}
}

DARKSDK _inline bool UpdatePtr ( int iID )
{
	// use this to get the camera data, every function needs to call this
	// it's made as a separate function so if we need to make any changes
	// we only need to do it in 1 place

	// set the pointer to null
	m_ptr = NULL;
	
	// see if the camera is valid and we can get the data, if this fails
	// return false and let the caller know we couldn't get the data
	if ( !( m_ptr = m_CameraManager.GetData ( iID ) ) )
		return false;
	
	// return true if everything is ok - we are able to update the pointer
	return true;
}

DARKSDK float GetLookX ( int iID )
{
	// get the look x value

	// update the pointer
	if ( !UpdatePtr ( iID ) )
		return 0.0;

	// return the value
	return m_ptr->vecLook.x;
}

DARKSDK float GetLookY ( int iID )
{
	// get the look y value

	// update the pointer
	if ( !UpdatePtr ( iID ) )
		return 0.0;

	// return the value
	return m_ptr->vecLook.y;
}

DARKSDK float GetLookZ ( int iID )
{
	// get the look z value

	// update the pointer
	if ( !UpdatePtr ( iID ) )
		return 0.0;

	// return the value
	return m_ptr->vecLook.z;
}

DARKSDK D3DXMATRIX GetMatrixCamera ( int iID )
{
	return GetMatrix ( iID );
}

DARKSDK D3DXMATRIX GetMatrix ( int iID )
{
	// return the matrix used by the camera, useful for
	// DarkSDK apps

	// update the pointer
	if ( !UpdatePtr ( iID ) )
	{
		D3DXMATRIX d3dNull;
		D3DXMatrixIdentity ( &d3dNull );
		return d3dNull;
	}

	// return the matrix
	return m_ptr->matView;
}

DARKSDK void AnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles )
{
	// Thanks to Andrew for finding this gem!
	// from http://www.martinb.com/maths/geometry/rotations/conversions/matrixToEuler/index.htm
	float m00 = pmatMatrix->_11;
	float m01 = pmatMatrix->_12;
	float m02 = pmatMatrix->_13;
	float m12 = pmatMatrix->_23;
	float m22 = pmatMatrix->_33;
	float heading = (float)atan2(m01,m00);
	float attitude = (float)atan2(m12,m22);
	float bank = (float)asin(-m02);

	// check for gimbal lock
	if ( fabs ( m02 ) > 1.0f )
	{
		// looking straight up or down
		float PI = D3DX_PI / 2.0f;
		pVecAngles->x = 0.0f;
		pVecAngles->y = D3DXToDegree ( PI * m02 );
		pVecAngles->z = 0.0f;
	}
	else
	{
		pVecAngles->x = D3DXToDegree ( attitude );
		pVecAngles->y = D3DXToDegree ( bank );
		pVecAngles->z = D3DXToDegree ( heading );
	}
}

DARKSDK void InternalUpdate ( int iID )
{
	// mike - ???when - update any properties of the camera and set the transform
	if ( m_ptr->bOverride )
	{
		m_pD3D->SetTransform ( D3DTS_VIEW, &m_ptr->matOverride );
		m_pD3D->SetTransform ( D3DTS_PROJECTION, &m_ptr->matProjection );
		return;
	}

	// variable declarations
	D3DXMATRIX		matRot;		// we use this for rotation
	D3DXMATRIX		matTempRot;

	// setup an identity matrix
	D3DXMatrixIdentity ( &m_ptr->matView );
	
	// Create a rotation matrix
	if(m_ptr->bUseFreeFlightRotation==false)
	{
		// STANDARD EULER ANGLE ROTATION HANDLING
		if ( m_ptr->bRotate )
			D3DXMatrixRotationX ( &matRot, D3DXToRadian ( m_ptr->fXRotate ) );
		else
			D3DXMatrixRotationZ ( &matRot, D3DXToRadian ( m_ptr->fZRotate ) );

		D3DXMatrixRotationY ( &matTempRot, D3DXToRadian ( m_ptr->fYRotate ) );
		D3DXMatrixMultiply ( &matRot, &matRot, &matTempRot );

		if ( m_ptr->bRotate )
			D3DXMatrixRotationZ ( &matTempRot, D3DXToRadian ( m_ptr->fZRotate ) );
		else
			D3DXMatrixRotationX ( &matTempRot, D3DXToRadian ( m_ptr->fXRotate ) );

		D3DXMatrixMultiply ( &matRot, &matRot, &matTempRot );

		// rotation recreated each time for accuracy
		m_ptr->vecRight.x=1;
		m_ptr->vecRight.y=0;
		m_ptr->vecRight.z=0;
		m_ptr->vecUp.x=0;
		m_ptr->vecUp.y=1;
		m_ptr->vecUp.z=0;
		m_ptr->vecLook.x=0;
		m_ptr->vecLook.y=0;
		m_ptr->vecLook.z=1;
		
		D3DXVec3TransformCoord ( &m_ptr->vecRight,	&m_ptr->vecRight,	&matRot );
		D3DXVec3TransformCoord ( &m_ptr->vecUp,		&m_ptr->vecUp,		&matRot );
		D3DXVec3TransformCoord ( &m_ptr->vecLook,	&m_ptr->vecLook,	&matRot );

		// store the new values in the view matrix
		m_ptr->matView._11 = m_ptr->vecRight.x; 
		m_ptr->matView._12 = m_ptr->vecUp.x; 
		m_ptr->matView._13 = m_ptr->vecLook.x;

		m_ptr->matView._21 = m_ptr->vecRight.y; 
		m_ptr->matView._22 = m_ptr->vecUp.y; 
		m_ptr->matView._23 = m_ptr->vecLook.y;

		m_ptr->matView._31 = m_ptr->vecRight.z;
		m_ptr->matView._32 = m_ptr->vecUp.z; 
		m_ptr->matView._33 = m_ptr->vecLook.z;

		m_ptr->matView._41 =- D3DXVec3Dot ( &m_ptr->vecPosition, &m_ptr->vecRight );
		m_ptr->matView._42 =- D3DXVec3Dot ( &m_ptr->vecPosition, &m_ptr->vecUp    );
		m_ptr->matView._43 =- D3DXVec3Dot ( &m_ptr->vecPosition, &m_ptr->vecLook  );
	}
	else
	{
		// FREE FLIGHT ROTATION HANDLING
		matRot = m_ptr->matFreeFlightRotate;
		D3DXMatrixInverse(&matRot, NULL, &matRot);

		// rotation recreated each time for accuracy
		m_ptr->vecRight.x=1;
		m_ptr->vecRight.y=0;
		m_ptr->vecRight.z=0;
		m_ptr->vecUp.x=0;
		m_ptr->vecUp.y=1;
		m_ptr->vecUp.z=0;
		m_ptr->vecLook.x=0;
		m_ptr->vecLook.y=0;
		m_ptr->vecLook.z=1;
		
		D3DXVec3TransformCoord ( &m_ptr->vecRight,	&m_ptr->vecRight,	&matRot );
		D3DXVec3TransformCoord ( &m_ptr->vecUp,		&m_ptr->vecUp,		&matRot );
		D3DXVec3TransformCoord ( &m_ptr->vecLook,	&m_ptr->vecLook,	&matRot );

//		// leefix - 170203 - update rotate vars (for angle return)
//		GetAngleFromPoint ( 0.0f, 0.0f, 0.0f,
//							m_ptr->vecLook.x, m_ptr->vecLook.y, m_ptr->vecLook.z,
//							&m_ptr->fXRotate, &m_ptr->fYRotate, &m_ptr->fZRotate);

		// leefix - 040803 - New Extract Angles Code - from free flight camera matrix
		// leefix - 280305 - remembering of course that to remain compatible
		// with objectDLL we need to use the inverse matrix to get the angles
		// AnglesFromMatrix ( &m_ptr->matFreeFlightRotate, &vecRot );
		D3DXVECTOR3 vecRot;
		AnglesFromMatrix ( &matRot, &vecRot );
		m_ptr->fXRotate=vecRot.x;
		m_ptr->fYRotate=vecRot.y;
		m_ptr->fZRotate=vecRot.z;

		// Use new current matrix
		m_ptr->matView = m_ptr->matFreeFlightRotate;

		// Apply position of camera to view
		D3DXMATRIX matTemp;
		D3DXMatrixTranslation(&matTemp, -m_ptr->vecPosition.x,-m_ptr->vecPosition.y,-m_ptr->vecPosition.z);
		D3DXMatrixMultiply(&m_ptr->matView, &matTemp, &m_ptr->matView);
	}
	
	// finally apply our new matrix to the view
	m_pD3D->SetTransform ( D3DTS_VIEW, &m_ptr->matView );
	m_pD3D->SetTransform ( D3DTS_PROJECTION, &m_ptr->matProjection );
}

DARKSDK void Rotate ( int iID, float fX, float fY, float fZ )
{
	/*
	// rotate the camera around all 3 axes
	// check the camera exists and get it's data
	if ( !UpdatePtr ( iID ) )
		return;
	// Rotate forcee euler rotation
	m_ptr->bUseFreeFlightRotation=true;
	// update the camera
	InternalUpdate ( iID );
	*/
}

DARKSDK void XRotate ( int iID, float fX )
{
	/*
	// rotate the camera on it's x axis
	// check the camera exists and get it's data
	if ( !UpdatePtr ( iID ) )
		return;
	// save pitch
	m_ptr->fPitch += fX;
	// update the camera
	InternalUpdate ( iID );
	*/
}

DARKSDK void YRotate ( int iID, float fY )
{
	/*
	// rotate the camera on it's y axis
	// check the camera exists and get it's data
	if ( !UpdatePtr ( iID ) )
		return;
	// save yaw
	m_ptr->fYaw += fY;
	// update the camera
	InternalUpdate ( iID );
	*/
}

DARKSDK void ZRotate ( int iID, float fZ )
{
	/*
	// rotate the camera on it's z axis
	// check the camera exists and get it's data
	if ( !UpdatePtr ( iID ) )
		return;
	// save roll
	m_ptr->fRoll += fZ;
	// update the camera
	InternalUpdate ( iID );
	*/
}

DARKSDK int GetExistEx ( int iID )
{
	if ( UpdatePtr ( iID ) )
		return true;
	else
		return false;
}

DARKSDK float GetXPosition ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0;

	return m_ptr->vecPosition.x;
}

DARKSDK float GetYPosition ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0;

	return m_ptr->vecPosition.y;
}

DARKSDK float GetZPosition ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0;

	return m_ptr->vecPosition.z;
}

DARKSDK float GetXAngle ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0;

	return 0.0;
}

DARKSDK float GetYAngle ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0;

	return 0.0;
}

DARKSDK float GetZAngle ( int iID )
{
	if ( !UpdatePtr ( iID ) )
		return 0.0;

	return 0.0;
}

//
// DBV1 Command Functions
//

DARKSDK void Position ( float fX, float fY, float fZ )
{
	Position ( m_iCurrentCamera, fX, fY, fZ );
}

DARKSDK void SetRotate ( float fX, float fY, float fZ )
{
	SetRotate ( m_iCurrentCamera, fX, fY, fZ );
}

DARKSDK void SetXRotate ( float fX )
{
	SetXRotate ( m_iCurrentCamera, fX );
}

DARKSDK void SetYRotate ( float fY )
{
	SetYRotate ( m_iCurrentCamera, fY );
}

DARKSDK void SetZRotate ( float fZ )
{
	SetZRotate ( m_iCurrentCamera, fZ );
}

DARKSDK void Point ( float fX, float fY, float fZ )
{
	Point ( m_iCurrentCamera, fX, fY, fZ );
}

DARKSDK void Move ( float fStep )
{
	Move ( m_iCurrentCamera, fStep );
}

DARKSDK void SetRange ( float fFront, float fBack )
{
	SetRange ( m_iCurrentCamera, fFront, fBack );
}

DARKSDK void SetView ( int iLeft, int iTop, int iRight, int iBottom )
{
	SetView ( m_iCurrentCamera, iLeft, iTop, iRight, iBottom );
}

DARKSDK void ClearView ( DWORD dwColorValue )
{
	int iRed = (dwColorValue >> 16) & 255;
	int iGreen = (dwColorValue >> 8) & 255;
	int iBlue = dwColorValue & 255;
	ClearView ( m_iCurrentCamera, iRed, iGreen, iBlue );
}

DARKSDK void ClearViewEx ( int iID, DWORD dwColorValue )
{
	int iRed = (dwColorValue >> 16) & 255;
	int iGreen = (dwColorValue >> 8) & 255;
	int iBlue = dwColorValue & 255;
	ClearView ( iID, iRed, iGreen, iBlue );
}

DARKSDK void SetRotationXYZ ( void )
{
	SetRotationXYZ ( 0 );
}

DARKSDK void SetRotationZYX ( void )
{
	SetRotationZYX ( 0 );
}

DARKSDK void SetFOV ( float fAngle )
{
	SetFOV ( m_iCurrentCamera, fAngle );
}

DARKSDK void SetAspect ( float fAspect )
{
	SetAspect ( m_iCurrentCamera, fAspect );
}

DARKSDK void Follow ( float fX, float fY, float fZ, float fAngle, float fDistance, float fHeight, float fSmooth, int iCollision )
{
	Follow ( m_iCurrentCamera, fX, fY, fZ, fAngle, fDistance, fHeight, fSmooth, iCollision );
}

DARKSDK void SetAutoCamOn ( void )
{
	m_bAutoCamState = true;
}

DARKSDK void SetAutoCamOff ( void )
{
	m_bAutoCamState = false;
}

DARKSDK void TurnLeft ( float fAngle )
{
	TurnLeft ( m_iCurrentCamera, fAngle );
}

DARKSDK void TurnRight ( float fAngle )
{
	TurnRight ( m_iCurrentCamera, fAngle );
}

DARKSDK void PitchUp ( float fAngle )
{
	PitchUp ( m_iCurrentCamera, fAngle );
}

DARKSDK void PitchDown ( float fAngle )
{
	PitchDown ( m_iCurrentCamera, fAngle );
}

DARKSDK void RollLeft ( float fAngle )
{
	RollLeft ( m_iCurrentCamera, fAngle );
}

DARKSDK void RollRight ( float fAngle )
{
	RollRight ( m_iCurrentCamera, fAngle );
}

DARKSDK void SetToObjectOrientation ( int iObjectID )
{
	SetToObjectOrientation ( m_iCurrentCamera, iObjectID );
}

DARKSDK void BackdropOn ( void )
{
	BackdropOn ( m_iCurrentCamera );
}

DARKSDK void BackdropOff ( void )
{
	BackdropOff ( m_iCurrentCamera );
}

DARKSDK void BackdropColor ( DWORD dwColor )
{
	BackdropColor ( m_iCurrentCamera, dwColor );
}

DARKSDK void BackdropTexture ( int iImage )
{
	BackdropTexture ( m_iCurrentCamera, iImage );
}

DARKSDK void BackdropScroll ( int iU, int iV )
{
	BackdropScroll ( m_iCurrentCamera, iU, iV );
}

DARKSDK void		SetCameraDepth					( int iID, int iSourceID )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void		BackdropOff						( int iID, int iNoDepthClear )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void		CopyCameraToImage				( int iID, int iImage )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void SetCamerasToStereoscopic ( int iStereoscopicMode, int iCameraL, int iCameraR, int iBackGrey, int iFrontSideBySide )
{
	// iStereoscopicMode:
	// 1-anaglyph (back)
	// 2-IZ3d (back/front)
	// 3-Philips WOWVX (grey/side-by-side)

	// no action if zero specified
	if ( iStereoscopicMode<=0 )
		return;

	// set-up new render targets to hold back/front surfaces for new functionality of camera image writing
	HRESULT hRes = S_OK;

	// image values must be correct
	if(iBackGrey<1 || iBackGrey>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return;
	}
	if(iFrontSideBySide<1 || iFrontSideBySide>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return;
	}

	// cameras must have images initially
	// left
	if ( iCameraL < 0 || iCameraL > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iCameraL ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}
	tagCameraData* m_ptrL = m_ptr;
	if(m_ptrL->pCameraToImageTexture==NULL)
	{
		RunTimeError ( RUNTIMEERROR_IMAGENOTEXIST );
		return;
	}
	// right (not WOW)
	tagCameraData* m_ptrR = NULL;
	if ( iStereoscopicMode == 3 )
	{
		// copy left camera ptr (as right camera ptr used to trigger the event of creating the greyscale/side by side texture)
		m_ptrR = m_ptrL;
	}
	else
	{
		if ( iCameraR < 0 || iCameraR > MAXIMUMVALUE )
		{
			RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
			return;
		}
		if ( !UpdatePtr ( iCameraR ) )
		{
			RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
			return;
		}
		m_ptrR = m_ptr;
		if(m_ptrR->pCameraToImageTexture==NULL)
		{
			RunTimeError ( RUNTIMEERROR_IMAGENOTEXIST );
			return;
		}
	}

	// delete images
	if ( g_Image_GetExist ( iBackGrey )) g_Image_Delete ( iBackGrey );
	if ( g_Image_GetExist ( iFrontSideBySide )) g_Image_Delete ( iFrontSideBySide );

	// set the second camera (right camera) to trigger the generation of the stereoscopic images
	m_ptrL->iStereoscopicMode = 0;
	m_ptrL->pStereoscopicFirstCamera = NULL;
	m_ptrR->iStereoscopicMode = iStereoscopicMode;
	m_ptrR->pStereoscopicFirstCamera = m_ptrL;

	// Set camera to render to stereoscopic image from regular camera image
	m_ptrR->iCameraToStereoImageBack = iBackGrey;
	m_ptrR->iCameraToStereoImageFront = iFrontSideBySide;

	// get width and height from existing camera image surface
	D3DSURFACE_DESC surfacedesc;
	m_ptrR->pCameraToImageSurface->GetDesc(&surfacedesc);
	D3DFORMAT CommonFormat = surfacedesc.Format;
	int iWidth = surfacedesc.Width;
	int iHeight = surfacedesc.Height;

	// make render target an internal texture
	m_ptrR->pCameraToStereoImageBackTexture = g_Image_MakeRenderTarget ( iBackGrey, iWidth, iHeight, CommonFormat );
	if ( m_ptrR->pCameraToStereoImageBackTexture==NULL )
	{
		m_ptrR->iCameraToStereoImageBack = 0;
		m_ptrR->iCameraToStereoImageFront = 0;
		return;
	}
	m_ptrR->pCameraToStereoImageFrontTexture = g_Image_MakeRenderTarget ( iFrontSideBySide, iWidth, iHeight, CommonFormat );
	if ( m_ptrR->pCameraToStereoImageFrontTexture==NULL )
	{
		m_ptrR->iCameraToStereoImageBack = 0;
		m_ptrR->iCameraToStereoImageFront = 0;
		return;
	}

	// Open image surface for continual usage
	m_ptrR->pCameraToStereoImageBackTexture->GetSurfaceLevel( 0, &m_ptrR->pCameraToStereoImageBackSurface );
	if ( m_ptrR->pCameraToStereoImageBackSurface==NULL )
	{
		SAFE_RELEASE ( m_ptrR->pCameraToStereoImageBackTexture );
		SAFE_RELEASE ( m_ptrR->pCameraToStereoImageFrontTexture );
		return;
	}
	m_ptrR->pCameraToStereoImageFrontTexture->GetSurfaceLevel( 0, &m_ptrR->pCameraToStereoImageFrontSurface );
	if ( m_ptrR->pCameraToStereoImageFrontSurface==NULL )
	{
		SAFE_RELEASE ( m_ptrR->pCameraToStereoImageBackTexture );
		SAFE_RELEASE ( m_ptrR->pCameraToStereoImageFrontTexture );
		return;
	}

	// success
	return;
}


//
// DBV1 Expression Functions
//

DARKSDK DWORD GetXPositionEx ( void )
{
	// leefix - 020308 - should use current camera value
	// return GetXPositionEx ( 0 );
	return GetXPositionEx ( m_iCurrentCamera );
}

DARKSDK DWORD GetYPositionEx ( void )
{
	// leefix - 020308 - should use current camera value
	// return GetYPositionEx ( 0 );
	return GetYPositionEx ( m_iCurrentCamera );
}

DARKSDK DWORD GetZPositionEx ( void )
{
	// leefix - 020308 - should use current camera value
	// return GetZPositionEx ( 0 );
	return GetZPositionEx ( m_iCurrentCamera );
}

DARKSDK DWORD GetXAngleEx ( void )
{
	// leefix - 020308 - should use current camera value
	// return GetXAngleEx ( 0 );
	return GetXAngleEx ( m_iCurrentCamera );
}

DARKSDK DWORD GetYAngleEx ( void )
{
	// leefix - 020308 - should use current camera value
	// return GetYAngleEx ( 0 );
	return GetYAngleEx ( m_iCurrentCamera );
}

DARKSDK DWORD GetZAngleEx ( void )
{
	// leefix - 020308 - should use current camera value
	// return GetZAngleEx ( 0 );
	return GetZAngleEx ( m_iCurrentCamera );
}

DARKSDK void Create ( int iID )
{
	// internal camera data
	tagCameraData	cData;
	cData.pCameraToImageTexture=NULL;

	// created internally so much be released internally (unlike above which was done with makeformat)
	cData.pCameraToImageAlphaTexture=NULL;

	// add data to list
	m_CameraManager.Add ( &cData, iID );

	// leefix - 140906 - u63 - sort cameras in list, coreDLL needs cameras in index-order (predictive sync mask)
	// 20120311 IanM - No need for sort any more - m_CameraManager is kept sorted automatically
	// m_CameraManager.Sort ( );

	// setup default values
	Reset ( iID );
}

DARKSDK void CreateEx ( int iID )
{
	// create a camera
	if ( iID==0 )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOZERO );
		return;
	}
	if ( UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERAALREADYEXISTS );
		return;
	}

	Create ( iID );
}

DARKSDK void Destroy ( int iID )
{
	// update internal pointer
	if ( !UpdatePtr ( iID ) )
		return;

	// free image surface from camera first
	FreeCameraSurfaces();

	// attempt to delete the camera from the list
	m_CameraManager.Delete ( iID );
}

DARKSDK void DestroyEx ( int iID )
{
	// destroy a camera
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}
	if ( iID==0 )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOZERO );
		return;
	}

	Destroy ( iID );
}

DARKSDK void Reset ( int iID )
{
	// variable declarations
	D3DVIEWPORT9	viewport;	// current viewport

	// leefix-040803-use camera zero as the only viewport to collect initial viewport
	if ( iID==0 )
	{
		// from device
		memset ( &viewport, 0, sizeof ( viewport ) );
		m_pD3D->GetViewport ( &viewport );
	}
	else
	{
		// update internal pointer to camera zero
		if ( !UpdatePtr ( 0 ) )
			return;

		// if non-zero camera, from camera zero we take the viewport defaults
		memcpy ( &viewport, &m_ptr->viewPort3D, sizeof ( m_ptr->viewPort3D ) );
	}

	// update internal pointer
	if ( !UpdatePtr ( iID ) )
		return;

	int iWidth = viewport.Width;
	int iHeight = viewport.Height;
	float fMinZ = viewport.MinZ;
	float fMaxZ = viewport.MaxZ;

	if(g_pGlob)
	{
		// From actual surface desc
		iWidth = g_pGlob->iScreenWidth;
		iHeight = g_pGlob->iScreenHeight;
		fMinZ = 0.0f;
		fMaxZ = 1.0f;
	}

	// Default FOV (for DBV1 legacy)
	m_ptr->fFOV = D3DXToDegree(3.14159265358979323846f/2.905f);

	// fill viewport2D and copy to viewport3D
	m_ptr->viewPort2D.X = 0;
	m_ptr->viewPort2D.Y = 0;
	m_ptr->viewPort2D.Width = iWidth;
	m_ptr->viewPort2D.Height = iHeight;
	m_ptr->viewPort2D.MinZ = fMinZ;
	m_ptr->viewPort2D.MaxZ = fMaxZ;
	memcpy(&m_ptr->viewPort3D, &m_ptr->viewPort2D, sizeof ( m_ptr->viewPort2D ) );

	// work out values for perspective, fov, clipping and positions
	m_ptr->fAspect = ( float ) viewport.Width / ( float ) viewport.Height;
	if ( m_ptr->fAspectMod != 0.0f ) m_ptr->fAspect = m_ptr->fAspectMod;

	SetRange       ( iID, m_ptr->fZNear, m_ptr->fZFar );								// default range
	SetView        ( iID, (int)0, 0, (int)iWidth, (int)iHeight );	// default view
	Position       ( iID, 0.0, 0.0, 0.0 );												// default position

	// Default rotation and rotation-order
	m_ptr->bUseFreeFlightRotation = false;
	m_ptr->bRotate = true;

	// LEEFIX - 041002 - Default non zero camera has green backdrop to help users
	if(iID>0)
	{
		m_ptr->iBackdropState = 1;
		m_ptr->dwBackdropColor = D3DCOLOR_XRGB(0,128,0);
	}

	// set the glob struct camera ID
	if ( g_pGlob ) g_pGlob->dwCurrentSetCameraID = iID;

	// leeadd - 310506 - u62 - additional resets
	m_ptr->bOverride = false;
	m_ptr->iClipPlaneOn = 0;
	memset ( m_ptr->planeClip, 0, sizeof(m_ptr->planeClip) );
}

DARKSDK void SetCurrentCamera ( int iID )
{
	// used to set the current camera, this needs to be called when you
	// end up using multiple camers
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if(iID>0)
	{
		if ( !UpdatePtr ( iID ) )
		{
			RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
			return;
		}
	}

	// Set the current camera
	m_iCurrentCamera = iID;

	// set the glob struct camera ID
	if ( g_pGlob ) g_pGlob->dwCurrentSetCameraID = iID;
}

DARKSDK D3DFORMAT GetValidStencilBufferFormat ( D3DFORMAT BackBufferFormat )
{
	// create the list in order of precedence
	D3DFORMAT DepthFormat;
	D3DFORMAT	list [ ] =
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
		if ( SUCCEEDED ( m_pDX->CheckDeviceFormat( D3DADAPTER_DEFAULT,
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

DARKSDK void SetCameraToImageEx ( int iID, int iImage, int iWidth, int iHeight, int iGenerateCameraAlpha, DWORD dwOwnD3DFMTValue )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// new mode which can detatch camera from image
	if ( iImage==-1 )
	{
		// free internal texture used
		if(m_ptr->pCameraToImageSurface)
		{
			// 240714 - release this surface before delete underlying texture
			m_ptr->pCameraToImageSurface->Release();
			m_ptr->pCameraToImageSurface = NULL;
		}
		if(m_ptr->pCameraToImageTexture)
		{
			// 070714 - free image ptr if about to release
			bool bReleased = false;
			if ( m_ptr->iCameraToImage > 0 )
			{
				if ( g_Image_GetExist ( m_ptr->iCameraToImage ) ) 
				{
					g_Image_Delete ( m_ptr->iCameraToImage );
					bReleased = true;
				}
			}
			if ( bReleased==false )
			{
				m_ptr->pCameraToImageTexture->Release(); 
			}
			m_ptr->pCameraToImageTexture=NULL;
		}
		// free any alpha texture created
		if(m_ptr->pCameraToImageAlphaTexture)
		{
			//m_ptr->pCameraToImageAlphaTexture->Release();
			m_ptr->pCameraToImageAlphaTexture=NULL;
		}
		// switch back to camera rendering
		m_ptr->iCameraToImage = 0;
		// done
		return;
	}

	// valid image
	if(iImage<1 || iImage>MAXIMUMVALUE)
	{
		RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
		return;
	}

	// image size must be a power of two
	// leechange - 170906 - u63 - powe of two no longer a limit on some HW
	/*
	int iPowW = GetPowerSquareOfSize ( iWidth );
	int iPowH = GetPowerSquareOfSize ( iHeight );
	if(iPowW!=iWidth || iPowH!=iHeight)
	{
		RunTimeError(RUNTIMEERROR_IMAGEAREAILLEGAL);
		return;
	}
	*/

	// only if camera not assigned to image
	if(m_ptr->pCameraToImageTexture==NULL)
	{
		// delete image
		if ( g_Image_GetExist ( iImage )) g_Image_Delete ( iImage );

		// Set camera to render to image (texture)
		m_ptr->iCameraToImage = iImage;

		// use backbuffer to determine common D3DFORMAT
		D3DSURFACE_DESC backbufferdesc;
		g_pGlob->pHoldBackBufferPtr->GetDesc(&backbufferdesc);
		D3DFORMAT CommonFormat = backbufferdesc.Format;

		// leeadd - 210604 - u54 - Alpha Camera Image or not
		if ( iGenerateCameraAlpha==1 )
		{
			// make render target an internal texture
			HRESULT hRes = D3DXCreateTexture (m_pD3D,
									  iWidth,
									  iHeight,
									  D3DX_DEFAULT,
									  D3DUSAGE_RENDERTARGET,
									  CommonFormat,
									  D3DPOOL_DEFAULT,
									  &m_ptr->pCameraToImageTexture );
		}
		else
		{
			// leeadd - 140906 - u63 - Just want a camera image that retains the alpha data (for things like post process depth of field)
			if ( iGenerateCameraAlpha==2 )
			{
				// lee - 060207 - fix - allow camera alpha mode 2 to be cleared
				D3DFORMAT dwStoreFormat = CommonFormat;
				CommonFormat=D3DFMT_A8R8G8B8;
				m_ptr->pCameraToImageTexture = g_Image_MakeRenderTarget ( iImage, iWidth, iHeight, CommonFormat );
				CommonFormat=dwStoreFormat;
			}
			else
			{
				// leeadd - 170708 - u70 - can now choose own D3DFMT value from extra param
				if ( iGenerateCameraAlpha==3 )
				{
					D3DFORMAT dwStoreFormat = CommonFormat;
					CommonFormat=(D3DFORMAT)dwOwnD3DFMTValue;
					m_ptr->pCameraToImageTexture = g_Image_MakeRenderTarget ( iImage, iWidth, iHeight, CommonFormat );
					CommonFormat=dwStoreFormat;
				}
				else
				{
					// no alpha so use render target as direct texture
					m_ptr->pCameraToImageTexture = g_Image_MakeRenderTarget ( iImage, iWidth, iHeight, CommonFormat );
				}
			}
		}

		// Open image surface for continual usage
		m_ptr->pCameraToImageTexture->GetSurfaceLevel( 0, &m_ptr->pCameraToImageSurface );

		// leeadd - 210604 - u54 - add alpha surface
		if ( iGenerateCameraAlpha==1 )
		{
			// get surface to texture alpha deposit area
			m_ptr->pCameraToImageAlphaTexture = g_Image_MakeJustFormat ( iImage, iWidth, iHeight, D3DFMT_A8R8G8B8 );
			m_ptr->pCameraToImageAlphaTexture->GetSurfaceLevel( 0, &m_ptr->pCameraToImageAlphaSurface );
		}

		// Work out size for depth buffer
		D3DSURFACE_DESC imagerenderdesc;
		m_ptr->pCameraToImageSurface->GetDesc(&imagerenderdesc);
		DWORD dwDepthWidth = imagerenderdesc.Width;
		DWORD dwDepthHeight = imagerenderdesc.Height;

		// delete old image depth buffer if too small for this one
		if( m_pImageDepthSurface && (dwDepthWidth>m_dwImageDepthWidth || dwDepthHeight>m_dwImageDepthHeight) )
		{
			// delete any items in the list
			if ( m_pImageDepthSurface )
			{
				m_pImageDepthSurface->Release();
				m_pImageDepthSurface=NULL;
			}
		}

		// create image depth surface
		if ( m_pImageDepthSurface==NULL )
		{
			m_dwImageDepthWidth = dwDepthWidth;
			m_dwImageDepthHeight = dwDepthHeight;
			HRESULT hRes = m_pD3D->CreateDepthStencilSurface(	dwDepthWidth, dwDepthHeight,
																GetValidStencilBufferFormat(CommonFormat), D3DMULTISAMPLE_NONE, 0, TRUE,
																&m_pImageDepthSurface, NULL );
		}

		// Set View to entire image surface
		SetView ( iID, 0, 0, iWidth, iHeight );
	}
}

DARKSDK void SetCameraToImageEx ( int iID, int iImage, int iWidth, int iHeight, int iGenerateCameraAlpha )
{
	// Regular camera image alpha specified
	SetCameraToImageEx ( iID, iImage, iWidth, iHeight, iGenerateCameraAlpha, 0 );
}

DARKSDK void SetCameraToImage ( int iID, int iImage, int iWidth, int iHeight )
{
	// Regular camera image no alpha
	SetCameraToImageEx ( iID, iImage, iWidth, iHeight, 0, 0 );
}

DARKSDK void MoveUp ( int iID, float fStep )
{
	// move the camera up
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// move upwards, up is defined from the up vector so it will move
	// up dependant on which way is up for the camera
	m_ptr->vecPosition += ( fStep * m_ptr->vecUp );

	// put this back in and comment out the last line if you want
	// to have up always the same direction
	// m_ptr->vecPosition.y += fStep;

	// apply the change
	InternalUpdate ( iID );
}

DARKSDK void MoveDown ( int iID, float fStep )
{
	// move the camera down
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// move downwards, down is defined from the down vector so it will move
	// down dependant on which way is down for the camera
	m_ptr->vecPosition -= ( fStep * m_ptr->vecUp );

	// put this back in and comment out the last line if you want
	// to have down always the same direction
	// m_ptr->vecPosition.y -= fStep;

	// apply the change
	InternalUpdate ( iID );
}

DARKSDK void MoveLeft ( int iID, float fStep )
{
	// move the camera left
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// move left, left is defined from the right vector so it will move
	// left dependant on which way is left for the camera
	m_ptr->vecPosition -= ( fStep * m_ptr->vecRight );

	// put this back in and comment out the last line if you want
	// to have left always the same direction
	// m_ptr->vecPosition -= fStep * m_ptr->vecRight;

	// update internal data
	InternalUpdate ( iID );
}

DARKSDK void MoveRight ( int iID, float fStep )
{
	// move the camera right
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// move right, right is defined from the right vector so it will move
	// right dependant on which way is right for the camera
	m_ptr->vecPosition += ( fStep * m_ptr->vecRight );

	// put this back in and comment out the last line if you want
	// to have right always the same direction
	// m_ptr->vecPosition += fStep * m_ptr->vecRight;

	// update internal data
	InternalUpdate ( iID );
}

DARKSDK void ControlWithArrowKeys ( int iID, float fVelocity, float fTurnSpeed )
{
	// move thewith the arrow keys
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// move camera based on direct arrow presses
	if ( GetAsyncKeyState ( VK_UP ) ) m_ptr->vecPosition += ( fVelocity * m_ptr->vecLook );
	if ( GetAsyncKeyState ( VK_DOWN ) ) m_ptr->vecPosition -= ( fVelocity * m_ptr->vecLook );
	if ( GetAsyncKeyState ( VK_LEFT ) ) m_ptr->fYRotate -= fTurnSpeed;
	if ( GetAsyncKeyState ( VK_RIGHT ) ) m_ptr->fYRotate += fTurnSpeed;

	// ensure camera is simplified
	m_ptr->bUseFreeFlightRotation=false;
	m_ptr->fXRotate = 0.0f;
	m_ptr->fZRotate = 0.0f;

	// update internal data
	InternalUpdate ( iID );
}

DARKSDK void SetPositionVector3 ( int iID, int iVector )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	m_ptr->vecPosition = g_Types_GetVector ( iVector );

	// apply the change
	InternalUpdate ( iID );
}

DARKSDK void GetPositionVector3 ( int iVector, int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	g_Types_SetVector ( iVector, m_ptr->vecPosition.x, m_ptr->vecPosition.y, m_ptr->vecPosition.z );
}

DARKSDK void SetRotationVector3 ( int iID, int iVector )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	D3DXVECTOR3 vec = g_Types_GetVector ( iVector );
	m_ptr->fXRotate = vec.x;
	m_ptr->fYRotate = vec.y;
	m_ptr->fZRotate = vec.z;

	// apply the change
	InternalUpdate ( iID );
}

DARKSDK void GetRotationVector3 ( int iVector, int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	g_Types_SetVector ( iVector, m_ptr->fXRotate, m_ptr->fYRotate, m_ptr->fZRotate );
}

DARKSDK void Position ( int iID, float fX, float fY, float fZ )
{
	// sets the position of the camera
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// now set the position
	m_ptr->vecPosition = D3DXVECTOR3 ( fX, fY, fZ );

	// apply the change
	InternalUpdate ( iID );
}

DARKSDK void SetRotate ( int iID, float fX, float fY, float fZ )
{
	// rotate the camera around all 3 axes
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// Rotate forcee euler rotation
	m_ptr->bUseFreeFlightRotation=false;
	m_ptr->fXRotate = fX;
	m_ptr->fYRotate = fY;
	m_ptr->fZRotate = fZ;
	
	// update the camera
	InternalUpdate ( iID );
}

DARKSDK void SetXRotate ( int iID, float fX )
{
	// rotate the camera on it's x axis
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// Rotate forcee euler rotation
	m_ptr->bUseFreeFlightRotation=false;
	m_ptr->fXRotate = fX;

	// update the camera
	InternalUpdate ( iID );
}

DARKSDK void SetYRotate ( int iID, float fY )
{
	// rotate the camera on it's y axis
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// Rotate forcee euler rotation
	m_ptr->bUseFreeFlightRotation=false;
	m_ptr->fYRotate = fY;

	// update the camera
	InternalUpdate ( iID );
}

DARKSDK void SetZRotate ( int iID, float fZ )
{
	// rotate the camera on it's z axis
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// Rotate forcee euler rotation
	m_ptr->bUseFreeFlightRotation=false;
	m_ptr->fZRotate = fZ;

	// update the camera
	InternalUpdate ( iID );
}

DARKSDK void Point ( int iID, float fX, float fY, float fZ )
{
	// point the camera in the location
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// rotation from xyz diff
	GetAngleFromPoint ( m_ptr->vecPosition.x, m_ptr->vecPosition.y, m_ptr->vecPosition.z,
						fX, fY, fZ, &m_ptr->fXRotate, &m_ptr->fYRotate, &m_ptr->fZRotate);

	// mike - 011005 - change to using euler rotation
	m_ptr->bUseFreeFlightRotation = false;

	// update internal camera data to reflect changes
	InternalUpdate ( iID );
}

DARKSDK void Move ( int iID, float fStep )
{
	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// move the camera forward
	m_ptr->vecPosition += ( fStep * m_ptr->vecLook );

	// update internal data
	InternalUpdate ( iID );
}

DARKSDK void SetRange ( int iID, float fFront, float fBack )
{
	// sets the near and far z ranges for the camera
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// save properties
	m_ptr->fZNear = fFront;	// front
	m_ptr->fZFar  = fBack;	// far
	
	// setup the perspective matrix
	D3DXMatrixPerspectiveFovLH ( &m_ptr->matProjection, D3DXToRadian(m_ptr->fFOV), m_ptr->fAspect, m_ptr->fZNear, m_ptr->fZFar );
	
	/* wrong, backwards!"
	// New HFOV and VFOV approach
	float fov_horiz = D3DXToRadian(m_ptr->fFOV);
	float fov_vert = 30.0f;
    float w = (float)1/tan(fov_horiz*0.5);  // 1/tan(x) == cot(x)
    float h = (float)1/tan(fov_vert*0.5);   // 1/tan(x) == cot(x)
    float Q = m_ptr->fZFar/(m_ptr->fZFar - m_ptr->fZNear);

    D3DXMATRIX ret;
    ZeroMemory(&ret, sizeof(ret));
    ret(0, 0) = w;
    ret(1, 1) = h;
    ret(2, 2) = Q;
    ret(3, 2) = -Q*m_ptr->fZNear;
    ret(2, 3) = 1;
	m_ptr->matProjection = ret;
	*/

	// and set the projection transform if current camera
	if(iID==m_iCurrentCamera)
	{
		m_pD3D->SetTransform ( D3DTS_PROJECTION, &m_ptr->matProjection );
	}
}

DARKSDK void SetView ( int iID, int iLeft, int iTop, int iRight, int iBottom )
{
	// set up the viewing area for the 3D scene
	int iWidth = iRight - iLeft;
	int iHeight = iBottom - iTop;

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// clear the current viewport
	memset ( &m_ptr->viewPort3D, 0, sizeof ( m_ptr->viewPort3D ) );
	
	// setup new viewport
	m_ptr->viewPort3D.X      = iLeft;		// top left
	m_ptr->viewPort3D.Y      = iTop;		// top
	m_ptr->viewPort3D.Width  = iWidth;	// far right
	m_ptr->viewPort3D.Height = iHeight;	// and bottom
	m_ptr->viewPort3D.MinZ   = 0.0;		// default as 0.0
	m_ptr->viewPort3D.MaxZ   = 1.0;		// default as 1.0

// leefix-050803-wrong place to set viewport (should be in camera start/finish functions)
//	// we can now set the new viewport
//	m_pD3D->SetViewport ( &m_ptr->viewPort3D );

	// leeadd - U69 - 250508 - if viewport larger than 2D (adjust 2D to match ) - double wide buffers
	if ( m_ptr->viewPort3D.Width > m_ptr->viewPort2D.Width ) m_ptr->viewPort2D.Width = m_ptr->viewPort3D.Width;
}

DARKSDK void ClearView ( int iID, int iRed, int iGreen, int iBlue )
{
	// clears the camera view to the specified colour

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// Custom colour
	DWORD dwCustomBackCol = D3DCOLOR_XRGB(iRed, iGreen, iBlue);

	// leefix - 050308 - only clear specified view - was NULL before
	D3DRECT areatoclear;
	areatoclear.x1 = m_ptr->viewPort3D.X;
	areatoclear.y1 = m_ptr->viewPort3D.Y;
	areatoclear.x2 = m_ptr->viewPort3D.X + m_ptr->viewPort3D.Width;
	areatoclear.y2 = m_ptr->viewPort3D.Y + m_ptr->viewPort3D.Height;

	// clear the screen
	m_pD3D->Clear ( 1, &areatoclear, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, dwCustomBackCol, 1.0f, 0 );
}

DARKSDK void SetRotationXYZ ( int iID )
{
	// set the rotation mode to x, y and then z

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// set normal rotation on
	m_ptr->bRotate = true;
}

DARKSDK void SetRotationZYX ( int iID )
{
	// set the rotation mode to z, y and then x

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// set different rotation on
	m_ptr->bRotate = false;
}

DARKSDK void SetFOV ( int iID, float fAngle )
{
	// set the cameras field of view

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// store the new angle
	m_ptr->fFOV = fAngle;

	// setup the projection matrix to apply the change
	SetRange ( iID, m_ptr->fZNear, m_ptr->fZFar );
}

//Dave 4th August 2014
DARKSDK void SetCameraSwitchBank ( int iID, int flags )
{
	// set the cameras field of view

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// store the new angle
	m_ptr->dwCameraSwitchBank = flags;

}

DARKSDK void SetAspect ( int iID, float fAspect )
{
	// set the cameras field of view

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// store the new angle
	m_ptr->fAspectMod = fAspect;
	m_ptr->fAspect = fAspect;

	// setup the projection matrix to apply the change
	SetRange ( iID, m_ptr->fZNear, m_ptr->fZFar );
}

DARKSDK void SetClip ( int iID, int iOnOff, float fX, float fY, float fZ, float fNX, float fNY, float fNZ )
{
	// leeadd - 310506 - u62 - set camera clip plane

	// validate inputs
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// update if available
    D3DCAPS9 d3dCaps;
    m_pD3D->GetDeviceCaps( &d3dCaps );
    g_dwMaxUserClipPlanes = d3dCaps.MaxUserClipPlanes;

	// set the clip settings
	if ( g_dwMaxUserClipPlanes>0 )
	{
		m_ptr->iClipPlaneOn = iOnOff;
		D3DXVECTOR3 vecPos = D3DXVECTOR3(fX,fY,fZ);
		D3DXVECTOR3 vecNormal = D3DXVECTOR3(fNX,fNY,fNZ);
		if ( iOnOff!=0 ) D3DXPlaneFromPointNormal ( &m_ptr->planeClip, &vecPos, &vecNormal );
	}
	else
		m_ptr->iClipPlaneOn = 0;
}

DARKSDK void Follow ( int iID, float targetx, float targety, float targetz, float targetangle, float camerarange, float cameraheight, float cameraspeed, int usestaticcollision )
{
	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// Angle to radians
	targetangle = D3DXToRadian( targetangle );

	// Create move target (where camera actually goes)
	bool bUntouched=true;
	float movetargetx = targetx - (float)(sin(targetangle)*camerarange);
	float movetargety = targety + cameraheight; // leefix - 200703 - added targety
	float movetargetz = targetz - (float)(cos(targetangle)*camerarange);

	if(usestaticcollision>0)
	{
		int once=usestaticcollision;
		int twice=usestaticcollision*2;

		/* mike : need two functions to check collision with the static objects

		// Put line at top of camera
		targety+=once;
		movetargety+=once;

		// Adjust move target so it has direct line of sight with real target
		float bosdistx1=targetx;
		float bosdisty1=targety;
		float bosdistz1=targetz;
		float bosdistx2=movetargetx;
		float bosdisty2=movetargety;
		float bosdistz2=movetargetz;
		if(DB_CheckCollisionPoolWithLine(	bosdistx1, bosdisty1, bosdistz1,
											bosdistx2, bosdisty2, bosdistz2, (float)once, 1.0f,
											&gvLatestStaticCollisionResult )>0)
		{
			movetargetx=gvLatestStaticCollisionResult.x;
			movetargety=gvLatestStaticCollisionResult.y;
			movetargetz=gvLatestStaticCollisionResult.z;
			lastmovetargetx=movetargetx;
			lastmovetargety=movetargety;
			lastmovetargetz=movetargetz;
			bUntouched=false;
		}	

		// Keep camera out of walls
		if(DB_CheckCollisionPool(	lastmovetargetx-twice, lastmovetargety-once, lastmovetargetz-twice,
									lastmovetargetx+twice, lastmovetargety+once, lastmovetargetz+twice,
									movetargetx-twice, movetargety-once, movetargetz-twice,
									movetargetx+twice, movetargety+once, movetargetz+twice,
									&gvLatestStaticCollisionResult )>0)
		{
			movetargetx -= gvLatestStaticCollisionResult.x;
			movetargety -= gvLatestStaticCollisionResult.y;
			movetargetz -= gvLatestStaticCollisionResult.z;
			bUntouched=false;
		}
		*/
	}

	// Calculate distance between camera and move position
	float dx = movetargetx - m_ptr->vecPosition.x;
	float dy = movetargety - m_ptr->vecPosition.y;
	float dz = movetargetz - m_ptr->vecPosition.z;

	// Have camera follow target at speed
	float moveangle = (float)atan2( (double)dx, (double)dz );
	float movedist = (float)sqrt(fabs(dx*dx)+fabs(dz*dz));
	float speed = movedist/cameraspeed;

	float newx, newy, newz;
	newx = m_ptr->vecPosition.x + (float)(sin(moveangle)*speed);
	newy = m_ptr->vecPosition.y + (dy/cameraspeed);
	newz = m_ptr->vecPosition.z + (float)(cos(moveangle)*speed);

	// Calculate look angle
	dx = targetx - newx;
	dz = targetz - newz;
	float lookangle = (float)atan2( (double)dx, (double)dz );

	// Calculate smoothing of camera look angle
	float a = D3DXToDegree ( lookangle );
	float da = m_ptr->fYRotate;
	float diff = a-da;
	if(diff<-180.0f) diff=(a+360.0f)-da;
	if(diff>180.0f) diff=a-(da+360.0f);
	da=da+(diff/1.5f);

	m_ptr->fYRotate = wrapangleoffset(da);
	m_ptr->vecPosition.x = newx;
	m_ptr->vecPosition.y = newy;
	m_ptr->vecPosition.z = newz;

	// apply changes
	InternalUpdate ( iID );

	if(bUntouched==true)
	{
		lastmovetargetx=movetargetx;
		lastmovetargety=movetargety;
		lastmovetargetz=movetargetz;
	}
}

DARKSDK void CheckRotationConversion ( tagCameraData* m_ptr, bool bUseFreeFlightMode )
{
	// has there been a change?
	if ( bUseFreeFlightMode != m_ptr->bUseFreeFlightRotation )
	{
		// Caluclates equivilant rotation data if switch rotation-modes
		if( bUseFreeFlightMode==true )
		{
			// Euler to Freeflight
			m_ptr->matFreeFlightRotate = m_ptr->matView;
			m_ptr->matFreeFlightRotate._41=0.0f;
			m_ptr->matFreeFlightRotate._42=0.0f;
			m_ptr->matFreeFlightRotate._43=0.0f;
		}
	}

	// always calculate freeflight to euler (for angle feedback)
	if( bUseFreeFlightMode==true )
	{
		// Freeflight to Euler
		D3DXVECTOR3 vecRotate;
		AnglesFromMatrix ( &m_ptr->matFreeFlightRotate, &vecRotate );
		m_ptr->fXRotate = vecRotate.x;
		m_ptr->fYRotate = vecRotate.y;
		m_ptr->fZRotate = vecRotate.z;
	}

	// new rotation mode
	m_ptr->bUseFreeFlightRotation = bUseFreeFlightMode;
}

DARKSDK void TurnLeft ( int iID, float fAngle )
{
	// turns the camera left

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// execute possible conversion
	CheckRotationConversion ( m_ptr, true );

	// rotation adjustment (leefix-210703-big change to freeflightcameracommands)
	D3DXMATRIX matRotation;
	D3DXMatrixRotationY ( &matRotation, D3DXToRadian ( fAngle ) );
	m_ptr->matFreeFlightRotate = m_ptr->matFreeFlightRotate * matRotation;
	m_ptr->bUseFreeFlightRotation=true;

	// apply changes
	InternalUpdate ( iID );
}

DARKSDK void TurnRight ( int iID, float fAngle )
{
	// turns the camera right

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// execute possible conversion
	CheckRotationConversion ( m_ptr, true );

	// rotation adjustment
	D3DXMATRIX matRotation;
	D3DXMatrixRotationY ( &matRotation, D3DXToRadian ( -fAngle ) );
	m_ptr->matFreeFlightRotate = m_ptr->matFreeFlightRotate * matRotation;
	m_ptr->bUseFreeFlightRotation=true;

	// apply changes
	InternalUpdate ( iID );
}

DARKSDK void PitchUp ( int iID, float fAngle )
{
	// pitch the camera up

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// execute possible conversion
	CheckRotationConversion ( m_ptr, true );

	// rotation adjustment
	D3DXMATRIX matRotation;
	D3DXMatrixRotationX ( &matRotation, D3DXToRadian ( fAngle ) );
	m_ptr->matFreeFlightRotate = m_ptr->matFreeFlightRotate * matRotation;
	m_ptr->bUseFreeFlightRotation=true;

	// apply changes
	InternalUpdate ( iID );
}

DARKSDK void PitchDown ( int iID, float fAngle )
{
	// pitch the camera down

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// execute possible conversion
	CheckRotationConversion ( m_ptr, true );

	// rotation adjustment
	D3DXMATRIX matRotation;
	D3DXMatrixRotationX ( &matRotation, D3DXToRadian ( -fAngle ) );
	m_ptr->matFreeFlightRotate = m_ptr->matFreeFlightRotate * matRotation;
	m_ptr->bUseFreeFlightRotation=true;

	// apply changes
	InternalUpdate ( iID );
}

DARKSDK void RollLeft ( int iID, float fAngle )
{
	// roll the camera left

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// execute possible conversion
	CheckRotationConversion ( m_ptr, true );

	// rotation adjustment
	D3DXMATRIX matRotation;
	D3DXMatrixRotationZ ( &matRotation, D3DXToRadian ( -fAngle ) );
	m_ptr->matFreeFlightRotate = m_ptr->matFreeFlightRotate * matRotation;
	m_ptr->bUseFreeFlightRotation=true;

	// apply changes
	InternalUpdate ( iID );
}

DARKSDK void RollRight ( int iID, float fAngle )
{
	// roll the camera right

	// update internal pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// execute possible conversion
	CheckRotationConversion ( m_ptr, true );

	// rotation adjustment
	D3DXMATRIX matRotation;
	D3DXMatrixRotationZ ( &matRotation, D3DXToRadian ( fAngle ) );
	m_ptr->matFreeFlightRotate = m_ptr->matFreeFlightRotate * matRotation;
	m_ptr->bUseFreeFlightRotation=true;

	// apply changes
	InternalUpdate ( iID );
}

DARKSDK void SetToObjectOrientation ( int iID, int iObjectID )
{
	// Get Camera Ptr
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// Get Object Ptr
	m_Object_Ptr = (sObject*)g_Basic3D_GetInternalData ( iObjectID );
	if ( m_Object_Ptr==NULL)
		return;

	// Assign Rotation Details to Camera
	m_ptr->vecLook					= m_Object_Ptr->position.vecLook;
	m_ptr->vecUp					= m_Object_Ptr->position.vecUp;
	m_ptr->vecRight					= m_Object_Ptr->position.vecRight;
	m_ptr->fXRotate					= m_Object_Ptr->position.vecRotate.x;
	m_ptr->fYRotate					= m_Object_Ptr->position.vecRotate.y;
	m_ptr->fZRotate					= m_Object_Ptr->position.vecRotate.z;
	m_ptr->bUseFreeFlightRotation	= m_Object_Ptr->position.bFreeFlightRotation;


// leefix - 220604 - u54 - camera and object free flights are inverse of each other
//	m_ptr->matFreeFlightRotate		= m_Object_Ptr->position.matFreeFlightRotate;
	FLOAT fDeterminant;
	D3DXMatrixInverse ( &m_ptr->matFreeFlightRotate, &fDeterminant, &m_Object_Ptr->position.matFreeFlightRotate );

	if ( m_Object_Ptr->position.dwRotationOrder == ROTORDER_XYZ )
		m_ptr->bRotate					= true;
	else
		m_ptr->bRotate					= false;

	// update camera
	InternalUpdate ( iID );
}

//
// New Expression Functions
//

DARKSDK DWORD GetXPositionEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->vecPosition.x;
}

DARKSDK DWORD GetYPositionEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->vecPosition.y;
}

DARKSDK DWORD GetZPositionEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->vecPosition.z;
}

DARKSDK DWORD GetXAngleEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->fXRotate;
}

DARKSDK DWORD GetYAngleEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->fYRotate;
}

DARKSDK DWORD GetZAngleEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return 0;
	}
	return *(DWORD*)&m_ptr->fZRotate;
}

DARKSDK DWORD GetLookXEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return 0;
	}

	// return the value
	return *(DWORD*)&m_ptr->vecLook.x;
}

DARKSDK DWORD GetLookYEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return 0;
	}

	// return the value
	return *(DWORD*)&m_ptr->vecLook.y;
}

DARKSDK DWORD GetLookZEx ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return 0;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return 0;
	}

	// return the value
	return *(DWORD*)&m_ptr->vecLook.z;
}

DARKSDK void BackdropOn ( int iID ) 
{
	// update the pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// set state
	m_ptr->iBackdropState = 1;
}

DARKSDK void BackdropOff ( int iID ) 
{
	// update the pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// deactivate backdrop activation
	m_bActivateBackdrop=false;

	// set state
	m_ptr->iBackdropState = 0;

}

DARKSDK void BackdropColor ( int iID, DWORD dwColor, DWORD dwForeColor ) 
{
	// update the pointer
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	// set color
	m_ptr->dwBackdropColor = dwColor;
	m_ptr->dwForegroundColor = dwForeColor;

	// lee - 310306 - u6rc4 - if camera zero, community colour for sprite backdrop
	if ( g_pGlob && iID==0 ) g_pGlob->dw3DBackColor = dwColor;
}

DARKSDK void BackdropColor ( int iID, DWORD dwColor ) 
{
	// passed to core function
	BackdropColor ( iID, dwColor, 0 );
}

DARKSDK void BackdropTexture ( int iID, int iImage ) 
{
	// obsolete function - prior to 060409
	// RunTimeError ( RUNTIMEERROR_COMMANDNOWOBSOLETE );
	// re-used command for U74 - 070409
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}
	if ( iImage==0 )
	{
		// stop backdrop texture
		m_ptr->iBackdropTextureMode = 0;
		m_ptr->pBackdropTexture = NULL;
	}
	else
	{
		// use backdrop texture
		if(iImage<1 || iImage>MAXIMUMVALUE)
		{
			RunTimeError(RUNTIMEERROR_IMAGEILLEGALNUMBER);
			return;
		}
		if ( !g_Image_GetExist ( iImage ))
		{
			RunTimeError(RUNTIMEERROR_IMAGENOTEXIST);
			return;
		}

		// set backdrop texture
		m_ptr->iBackdropTextureMode = 1;
		m_ptr->pBackdropTexture = g_Image_GetPointer ( iImage );
	}
}

DARKSDK void BackdropScroll ( int iID, int iU, int iV )
{
	// obsolete function
	RunTimeError ( RUNTIMEERROR_COMMANDNOWOBSOLETE );
}

DARKSDK void SetCameraMatrix ( int iID, D3DXMATRIX* pMatrix )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	m_ptr->bOverride = true;
	m_ptr->matOverride = *pMatrix;
	m_ptr->matView = m_ptr->matOverride;
	D3DXMATRIX matCurrentTrans;
	float f;
	D3DXMatrixInverse ( &matCurrentTrans, &f, &m_ptr->matView );
	m_ptr->vecPosition.x = matCurrentTrans._41;
	m_ptr->vecPosition.y = matCurrentTrans._42;
	m_ptr->vecPosition.z = matCurrentTrans._43;

	// get euler angles from view matrix
	D3DXVECTOR3 sCamright, sCamup, sCamlook;
	sCamright.x = m_ptr->matView._11;
	sCamright.y = m_ptr->matView._21;
	sCamright.z = m_ptr->matView._31;
	sCamup.x    = m_ptr->matView._12;
	sCamup.y    = m_ptr->matView._22;
	sCamup.z    = m_ptr->matView._32;
	sCamlook.x  = m_ptr->matView._13;
	sCamlook.y  = m_ptr->matView._23;
	sCamlook.z  = m_ptr->matView._33;

	// copy vector values into camera (for frustum culling)
	m_ptr->vecUp = sCamlook;
	m_ptr->vecLook = sCamup;
	m_ptr->vecRight = sCamright;

	// Calculate yaw and pitch and roll
	float lookLengthOnXZ = sqrtf( (sCamlook.z*sCamlook.z) + (sCamlook.x*sCamlook.x) );
	float fPitch = atan2f( sCamlook.y, lookLengthOnXZ );
	float fYaw   = atan2f( sCamlook.x, sCamlook.z );
	float fRoll  = atan2f( sCamup.y, sCamright.y ) - D3DX_PI/2;
	m_ptr->fXRotate = D3DXToDegree(-fPitch);
	m_ptr->fYRotate = D3DXToDegree(fYaw);
	m_ptr->fZRotate = D3DXToDegree(fRoll);
}

DARKSDK void ReleaseCameraMatrix ( int iID )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	m_ptr->bOverride = false;
}

D3DXMATRIX GetViewMatrix ( int iID )
{
	D3DXMATRIX matReturn;
	D3DXMatrixIdentity ( &matReturn );
	if ( !UpdatePtr ( iID ) )
		return matReturn;

	return m_ptr->matView;
}

D3DXMATRIX GetProjectionMatrix ( int iID )
{
	D3DXMATRIX matReturn;
	D3DXMatrixIdentity ( &matReturn );
	if ( !UpdatePtr ( iID ) )
		return matReturn;

	return m_ptr->matProjection;
}

void SetProjectionMatrix ( int iID, D3DXMATRIX* pMatrix )
{
	if ( iID < 0 || iID > MAXIMUMVALUE )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANUMBERILLEGAL );
		return;
	}
	if ( !UpdatePtr ( iID ) )
	{
		RunTimeError ( RUNTIMEERROR_CAMERANOTEXIST );
		return;
	}

	m_ptr->bOverride = true;
	m_ptr->matProjection = *pMatrix;
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void UpdateCamera ( void )
{
	Update ( );
}

void ConstructorCamera ( HINSTANCE hSetup, HINSTANCE hImage )
{
	Constructor ( hSetup, hImage );
}

void DestructorCamera  ( void )
{
	Destructor ( );
}

void SetErrorHandlerCamera ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataCamera( LPVOID pGlobPtr )
{
	PassCoreData( pGlobPtr );
}

void RefreshD3DCamera ( int iMode )
{
	RefreshD3D ( iMode );
}

void dbPositionCamera ( float fX, float fY, float fZ )
{
	Position ( fX, fY, fZ );
}

void dbRotateCamera ( float fX, float fY, float fZ )
{
	SetRotate ( fX, fY, fZ );
}

void dbXRotateCamera ( float fX )
{
	SetXRotate ( fX );
}

void dbYRotateCamera ( float fY )
{
	SetYRotate ( fY );
}

void dbZRotateCamera ( float fZ )
{
	SetZRotate ( fZ );
}

void dbPointCamera ( float fX, float fY, float fZ )
{
	Point ( fX, fY, fZ );
}

void dbMoveCamera ( float fStep )
{
	Move ( fStep );
}

void dbSetCameraRange ( float fFront, float fBack )
{
	SetRange ( fFront, fBack );
}

void dbSetCameraView ( int iLeft, int iTop, int iRight, int iBottom )
{
	SetView ( iLeft, iTop, iRight, iBottom );
}

void dbClearCameraView ( DWORD dwColorValue )
{
	ClearView ( dwColorValue );
}

void dbSetCameraRotationXYZ ( void )
{
	SetRotationXYZ ( );
}

void dbSetCameraRotationZYX ( void )
{
	SetRotationZYX ( );
}

void dbSetCameraFOV ( float fAngle )
{
	SetFOV ( fAngle );
}

void dbSetCameraAspect ( float fAspect )
{
	SetAspect ( fAspect );
}

void dbSetCameraToFollow ( float fX, float fY, float fZ, float fAngle, float fDistance, float fHeight, float fSmooth, int iCollision )
{
	Follow ( fX, fY, fZ, fAngle, fDistance, fHeight, fSmooth, iCollision );
}

void dbAutoCamOn ( void )
{
	SetAutoCamOn ( );
}

void dbAutoCamOff ( void )
{
	SetAutoCamOff ( );
}

void dbTurnCameraLeft ( float fAngle )
{
	TurnLeft ( fAngle );
}

void dbTurnCameraRight ( float fAngle )
{
	TurnRight ( fAngle );
}

void dbPitchCameraUp ( float fAngle )
{
	PitchUp ( fAngle );
}

void dbPitchCameraDown ( float fAngle )
{
	PitchDown ( fAngle );
}

void dbRollCameraLeft ( float fAngle )
{
	RollLeft ( fAngle );
}

void dbRollCameraRight ( float fAngle )
{
	RollRight ( fAngle );
}

void dbSetCameraToObjectOrientation ( int iObjectID )
{
	SetToObjectOrientation ( iObjectID );
}

void dbBackdropOn ( void )
{
	BackdropOn ( );
}

void dbBackdropOff ( void )
{
	BackdropOff ( );
}

void dbBackdropColor ( DWORD dwColor )
{
	BackdropColor ( dwColor );
}

void dbBackdropTexture ( int iImage )
{
	BackdropTexture ( iImage );
}

void dbBackdropScroll ( int iU, int iV )
{
	BackdropScroll ( iU, iV );
}

float dbCameraPositionX ( void )
{
	DWORD dwReturn = GetXPositionEx ( );
	
	return *( float* ) &dwReturn;
}

float dbCameraPositionY ( void )
{
	DWORD dwReturn = GetYPositionEx ( );
	
	return *( float* ) &dwReturn;
}

float dbCameraPositionZ ( void )
{
	DWORD dwReturn = GetZPositionEx ( );
	
	return *( float* ) &dwReturn;
}

float dbCameraAngleX ( void )
{
	DWORD dwReturn = GetXAngleEx ( );
	
	return *( float* ) &dwReturn;
}

float dbCameraAngleY ( void )
{
	DWORD dwReturn = GetYAngleEx ( );
	
	return *( float* ) &dwReturn;
}

float dbCameraAngleZ ( void )
{
	DWORD dwReturn = GetZAngleEx ( );
	
	return *( float* ) &dwReturn;
}

void dbMakeCamera ( int iID )
{
	CreateEx ( iID );
}

void dbDeleteCamera ( int iID )
{
	DestroyEx ( iID );
}

void dbSetCurrentCamera ( int iID )
{
	SetCurrentCamera ( iID );
}

void dbSetCameraToImage ( int iID, int iImage, int iWidth, int iHeight )
{
	SetCameraToImage ( iID, iImage, iWidth, iHeight );
}

void dbSetCameraToImage ( int iID, int iImage, int iWidth, int iHeight, int iGenAlpha )
{
	SetCameraToImageEx ( iID, iImage, iWidth, iHeight, iGenAlpha );
}

void dbResetCamera ( int iID )
{
	Reset ( iID );
}

void dbMoveCameraLeft ( int iID, float fStep )
{
	MoveLeft ( iID, fStep );
}

void dbMoveCameraRight ( int iID, float fStep )
{
	MoveRight ( iID, fStep );
}

void dbMoveCameraUp ( int iID, float fStep )
{
	MoveUp ( iID, fStep );
}

void dbMoveCameraDown ( int iID, float fStep )
{
	MoveDown ( iID, fStep );
}

void dbControlCameraUsingArrowKeys ( int iID, float fVelocity, float fTurnSpeed )
{
	ControlWithArrowKeys ( iID, fVelocity, fTurnSpeed );
}

void dbPositionCamera ( int iID, int iVector )
{
	SetPositionVector3 ( iID, iVector );
}

void dbSetVector3ToCameraPosition ( int iVector, int iID )
{
	GetPositionVector3 ( iVector, iID );
}

void dbRotateCamera ( int iID, int iVector )
{
	SetRotationVector3 ( iID, iVector );
}

void dbSetVector3ToCameraRotation ( int iVector, int iID )
{
	GetRotationVector3 ( iVector, iID );
}

void dbPositionCamera ( int iID, float fX, float fY, float fZ )
{
	Position ( iID, fX, fY, fZ );
}

void dbRotateCamera ( int iID, float fX, float fY, float fZ )
{
	SetRotate ( iID, fX, fY, fZ );
}

void dbXRotateCamera ( int iID, float fX )
{
	SetXRotate ( iID, fX );
}

void dbYRotateCamera ( int iID, float fY )
{
	SetYRotate ( iID, fY );
}

void dbZRotateCamera ( int iID, float fZ )
{
	SetZRotate ( iID, fZ );
}

void dbPointCamera ( int iID, float fX, float fY, float fZ )
{
	Point ( iID, fX, fY, fZ );
}

void dbMoveCamera ( int iID, float fStep )
{
	Move ( iID, fStep );
}

void dbSetCameraRange ( int iID, float fFront, float fBack )
{
	SetRange ( iID, fFront, fBack );
}

void dbSetCameraView ( int iID, int iLeft, int iTop, int iRight, int iBottom )
{
	SetView ( iID, iLeft, iTop, iRight, iBottom );
}

void dbClearCameraView ( int iID, DWORD dwColorValue )
{
	ClearViewEx ( iID, dwColorValue );
}

void dbClearCameraView ( int iID, int iRed, int iGreen, int iBlue )
{
	ClearView ( iID, iRed, iGreen, iBlue );
}

void dbSetCameraRotationXYZ ( int iID )
{
	SetRotationXYZ ( iID );
}

void dbSetCameraRotationZYX ( int iID )
{
	SetRotationZYX ( iID );
}

void dbSetCameraFOV ( int iID, float fAngle )
{
	SetFOV ( iID, fAngle );
}

void dbSetCameraAspect ( int iID, float fAspect )
{
	SetAspect ( iID, fAspect );
}

void dbSetCameraToFollow ( int iID, float fX, float fY, float fZ, float fAngle, float fDistance, float fHeight, float fSmooth, int iCollision )
{
	Follow ( iID, fX, fY, fZ, fAngle, fDistance, fHeight, fSmooth, iCollision );
}

void dbTurnCameraLeft ( int iID, float fAngle )
{
	TurnLeft ( iID, fAngle );
}

void dbTurnCameraRight ( int iID, float fAngle )
{
	TurnRight ( iID, fAngle );
}

void dbPitchCameraUp ( int iID, float fAngle )
{
	PitchUp ( iID, fAngle );
}

void dbPitchCameraDown ( int iID, float fAngle )
{
	PitchDown ( iID, fAngle );
}

void dbRollCameraLeft ( int iID, float fAngle )
{
	RollLeft ( iID, fAngle );
}

void dbRollCameraRight ( int iID, float fAngle )
{
	RollRight ( iID, fAngle );
}

void dbSetCameraToObjectOrientation ( int iID, int iObjectID )
{
	SetToObjectOrientation ( iID, iObjectID );
}

void dbBackdropOn ( int iID )
{
	BackdropOn ( iID );
}

void dbBackdropOff ( int iID )
{
	BackdropOff ( iID );
}

void dbColorBackdrop ( DWORD dwColor )
{
	BackdropColor ( 0, dwColor );
}

void dbColorBackdrop ( int iID, DWORD dwColor )
{
	BackdropColor ( iID, dwColor );
}

void dbBackdropTexture ( int iID, int iImage )
{
	BackdropTexture ( iID, iImage );
}

void dbBackdropScroll ( int iID, int iU, int iV )
{
	BackdropScroll ( iID, iU, iV );
}

float dbCameraPositionX ( int iID )
{
	DWORD dwReturn = GetXPositionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbCameraPositionY ( int iID )
{
	DWORD dwReturn = GetYPositionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbCameraPositionZ ( int iID )
{
	DWORD dwReturn = GetZPositionEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbCameraAngleX ( int iID )
{
	DWORD dwReturn = GetXAngleEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbCameraAngleY ( int iID )
{
	DWORD dwReturn = GetYAngleEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbCameraAngleZ ( int iID )
{
	DWORD dwReturn = GetZAngleEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbCameraLookX ( int iID )
{
	DWORD dwReturn = GetLookXEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbCameraLookY ( int iID )
{
	DWORD dwReturn = GetLookYEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbCameraLookZ ( int iID )
{
	DWORD dwReturn = GetLookZEx ( iID );
	
	return *( float* ) &dwReturn;
}

void* dbGetCameraInternalData ( int iID )
{
	return GetInternalData ( iID );
}

void dbSetAutoCam ( float fX, float fY, float fZ, float fRadius )
{
	SetAutoCam ( fX, fY, fZ, fRadius );
}

void dbCameraInternalUpdate	( int iID )
{
	InternalUpdate ( iID );
}

void dbStartScene ( void )
{
	StartScene ( );
}

void dbStartSceneEx ( int iMode )
{
	StartSceneEx ( iMode );
}

int dbFinishScene ( void )
{
	return FinishScene ( );
}

int dbGetRenderCamera ( void )
{
	return GetRenderCamera();
}

void dbRunCode ( int iMode )
{
	RunCode ( iMode );
}

D3DXMATRIX dbGetViewMatrix ( int iID )
{
	return GetViewMatrix ( iID );
}

D3DXMATRIX dbGetProjectionMatrix ( int iID )
{
	return GetProjectionMatrix ( iID );
}

// lee - 300706 - GDK fixes
void dbTextureBackdrop ( int iImage ) { dbBackdropTexture (iImage); }
void dbScrollBackdrop ( int iU, int iV ) { dbBackdropScroll (iU, iV); }
void dbTextureBackdrop ( int iID, int iImage ) { dbBackdropTexture (iID,iImage); }
void dbScrollBackdrop ( int iID, int iU, int iV ) { dbBackdropScroll (iID,iU, iV); }

// mike - 131106
int	dbFinishSceneEx ( bool bKnowInAdvanceCameraIsUsed )
{
	return FinishSceneEx ( bKnowInAdvanceCameraIsUsed );
}

void dbSetCameraClip ( int iID, int iOnOff, float fX, float fY, float fZ, float fNX, float fNY, float fNZ )
{
	// mike - 220107 - support camera clip
	SetClip ( iID, iOnOff, fX, fY, fZ, fNX, fNY, fNZ );
}

int dbCameraExist ( int iID )
{
	return GetExistEx ( iID );
}


#endif

