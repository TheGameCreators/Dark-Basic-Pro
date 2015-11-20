#include "cparticlec.h"
#include ".\..\error\cerror.h"
#include "CSnowC.h"

	#include ".\..\core\globstruct.h"

	// Global Shared Data Pointer (passed in from core)
	DBPRO_GLOBAL GlobStruct*				g_pGlob							= NULL;

#ifdef DARKSDK_COMPILE
	
	//#include ".\..\position\cpositionc.cpp"

	//tagObjectPos* m_pParticlePos;
#endif

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKImage.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKCamera.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDK3DMaths.h"
#endif

//////////////////////////////////////////////////////////////////////////
// vectors /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef D3DXVECTOR3				( *GetVectorPFN )       ( int );
typedef void					( *SetVectorPFN )       ( int, float, float, float );
typedef int						( *GetExistPFN )       ( int );
DBPRO_GLOBAL GetVectorPFN					g_Types_GetVector;
DBPRO_GLOBAL SetVectorPFN					g_Types_SetVector;
DBPRO_GLOBAL GetExistPFN						g_Types_GetExist;

//////////////////////////////////////////////////////////////////////////
// image /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef void				( *IMAGE_RetVoidParamVoidPFN )      ( void );
typedef bool				( *IMAGE_RetBoolParamIntPCharPFN )  ( int, char* );
typedef void				( *IMAGE_RetVoidParamIntPFN )       ( int );
typedef LPDIRECT3DTEXTURE9	( *IMAGE_RetLPD3DTEX9ParamIntPFN )  ( int );
typedef int  				( *IMAGE_RetIntParamIntPFN )        ( int );
typedef bool  				( *IMAGE_RetBoolParamIntPFN )       ( int );
typedef void  				( *IMAGE_RetVoidParamBoolPFN )      ( bool );
typedef void  				( *IMAGE_RetVoidParamInt7PFN )      ( int, int, int, int, int, int, int );
typedef void  				( *IMAGE_RetVoidParamFloatPFN )     ( float );
typedef void  				( *IMAGE_RetVoidParamInt4PFN )      ( int, int, int, int );
typedef void  				( *IMAGE_RetVoidParamInt2PFN )      ( int, int );

DBPRO_GLOBAL HINSTANCE						g_Image;

DBPRO_GLOBAL IMAGE_RetVoidParamVoidPFN		g_Image_Constructor;				// constructor
DBPRO_GLOBAL IMAGE_RetVoidParamVoidPFN		g_Image_Destructor;					// destructor

DBPRO_GLOBAL IMAGE_RetBoolParamIntPCharPFN	g_Image_Load;						// load an image from disk
DBPRO_GLOBAL IMAGE_RetBoolParamIntPCharPFN	g_Image_Save;						// save an image to disk
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN		g_Image_Delete;						// delete an image from memory

DBPRO_GLOBAL IMAGE_RetLPD3DTEX9ParamIntPFN	g_Image_GetPointer;					// get pointer to image data, useful to external apps
DBPRO_GLOBAL IMAGE_RetIntParamIntPFN			g_Image_GetWidth;					// get width of an image
DBPRO_GLOBAL IMAGE_RetIntParamIntPFN			g_Image_GetHeight;					// get height of an image
DBPRO_GLOBAL IMAGE_RetBoolParamIntPFN		g_Image_GetExist;					// does image exist

DBPRO_GLOBAL IMAGE_RetVoidParamBoolPFN		g_Image_SetSharing;					// set sharing mode on / off
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN		g_Image_SetMemory;					// set memory pool

DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN		g_Image_Lock;						// lock an image so you can write data to it
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN		g_Image_Unlock;						// unlock an image
DBPRO_GLOBAL IMAGE_RetVoidParamInt7PFN		g_Image_Write;						// write data to an image

DBPRO_GLOBAL IMAGE_RetVoidParamBoolPFN		g_Image_SetMipmapMode;				// set mipmapping on / off
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN		g_Image_SetMipmapType;				// set type of mipmapping - none, point and linear
DBPRO_GLOBAL IMAGE_RetVoidParamFloatPFN		g_Image_SetMipmapBias;				// set mipmap bias
DBPRO_GLOBAL IMAGE_RetVoidParamIntPFN		g_Image_SetMipmapNum;				// set number of mipmaps for when an image is created

DBPRO_GLOBAL IMAGE_RetVoidParamInt4PFN		g_Image_SetColorKey;				// set image color key
DBPRO_GLOBAL IMAGE_RetVoidParamInt2PFN		g_Image_SetTranslucency;			// set image translucency, specify a percentage for it
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// setup /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef IDirect3DDevice9* ( *GFX_GetDirect3DDevicePFN ) ( void );
DBPRO_GLOBAL HINSTANCE				g_GFX;							// for dll loading
DBPRO_GLOBAL GFX_GetDirect3DDevicePFN	g_GFX_GetDirect3DDevice;	// get pointer to D3D device
//////////////////////////////////////////////////////////////////////////

///
// Camera - autocam function
///
	typedef void					( *CAMERA3D_SetAutoCam ) ( float, float, float, float );
	DBPRO_GLOBAL CAMERA3D_SetAutoCam				g_Camera3D_SetAutoCam = NULL;


DBPRO_GLOBAL CParticleManager			m_ParticleManager;
DBPRO_GLOBAL tagParticleData*			m_ptr;
		
DBPRO_GLOBAL LPDIRECT3DDEVICE9			m_pD3D;
DBPRO_GLOBAL D3DMATERIAL9				gWhiteMaterial;

extern tagObjectPos* m_pParticlePos;

DARKSDK void ConstructorD3D ( HINSTANCE hSetup, HINSTANCE hImage )
{
	m_ptr = NULL;

	#ifndef DARKSDK_COMPILE
	if ( !hSetup || !hImage )
	{
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );		// load the setup library
		hImage = LoadLibrary ( "DBProImageDebug.dll" );		// load the image library
	}
	#endif

	#ifndef DARKSDK_COMPILE
		g_Image_Constructor          = ( IMAGE_RetVoidParamVoidPFN )		GetProcAddress ( hImage, "?Constructor@@YAXXZ" );
		g_Image_Destructor           = ( IMAGE_RetVoidParamVoidPFN )		GetProcAddress ( hImage, "?Destructor@@YAXXZ" );

		g_Image_Load	             = ( IMAGE_RetBoolParamIntPCharPFN )	GetProcAddress ( hImage, "?Load@@YA_NHPAD@Z" );
		g_Image_Save	             = ( IMAGE_RetBoolParamIntPCharPFN )	GetProcAddress ( hImage, "?Save@@YA_NHPAD@Z" );
		g_Image_Delete	             = ( IMAGE_RetVoidParamIntPFN )			GetProcAddress ( hImage, "?Delete@@YAXH@Z" );

		g_Image_GetPointer           = ( IMAGE_RetLPD3DTEX9ParamIntPFN )	GetProcAddress ( hImage, "?GetPointer@@YAPAUIDirect3DTexture9@@H@Z" );
		g_Image_GetWidth             = ( IMAGE_RetIntParamIntPFN )			GetProcAddress ( hImage, "?GetWidth@@YAHH@Z" );
		g_Image_GetHeight            = ( IMAGE_RetIntParamIntPFN )			GetProcAddress ( hImage, "?GetHeight@@YAHH@Z" );
		g_Image_GetExist             = ( IMAGE_RetBoolParamIntPFN )			GetProcAddress ( hImage, "?GetExist@@YA_NH@Z" );

		g_Image_SetSharing           = ( IMAGE_RetVoidParamBoolPFN )		GetProcAddress ( hImage, "?SetSharing@@YAX_N@Z" );
		g_Image_SetMemory            = ( IMAGE_RetVoidParamIntPFN )			GetProcAddress ( hImage, "?SetMemory@@YAXH@Z" );

		g_Image_Lock	             = ( IMAGE_RetVoidParamIntPFN )			GetProcAddress ( hImage, "?Lock@@YAXH@Z" );
		g_Image_Unlock	             = ( IMAGE_RetVoidParamIntPFN )			GetProcAddress ( hImage, "?Unlock@@YAXH@Z" );
		g_Image_Write	             = ( IMAGE_RetVoidParamInt7PFN )		GetProcAddress ( hImage, "?Write@@YAXHHHHHHH@Z" );

		g_Image_SetMipmapMode        = ( IMAGE_RetVoidParamBoolPFN )		GetProcAddress ( hImage, "?SetMipmapMode@@YAX_N@Z" );
		g_Image_SetMipmapType        = ( IMAGE_RetVoidParamIntPFN )			GetProcAddress ( hImage, "?SetMipmapType@@YAXH@Z" );
		g_Image_SetMipmapBias        = ( IMAGE_RetVoidParamFloatPFN )		GetProcAddress ( hImage, "?SetMipmapBias@@YAXM@Z" );
		g_Image_SetMipmapNum         = ( IMAGE_RetVoidParamIntPFN )			GetProcAddress ( hImage, "?SetMipmapNum@@YAXH@Z" );

		g_Image_SetColorKey          = ( IMAGE_RetVoidParamInt4PFN )		GetProcAddress ( hImage, "?SetColorKey@@YAXHHH@Z" );
		g_Image_SetTranslucency      = ( IMAGE_RetVoidParamInt2PFN )		GetProcAddress ( hImage, "?SetTranslucency@@YAXHH@Z" );

		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	#else
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;

		g_Image_GetPointer = dbGetImagePointer;
		
	#endif

	m_pD3D                  = g_GFX_GetDirect3DDevice ( );

	// Prepare a pure white material for texture usage
	memset ( &gWhiteMaterial, 0, sizeof ( gWhiteMaterial ) );
	gWhiteMaterial.Diffuse.r = 1.0f;
	gWhiteMaterial.Diffuse.g = 1.0f;
	gWhiteMaterial.Diffuse.b = 1.0f;
	gWhiteMaterial.Diffuse.a = 1.0f;	
	gWhiteMaterial.Ambient.r = 0.0f;
	gWhiteMaterial.Ambient.g = 0.0f;
	gWhiteMaterial.Ambient.b = 0.0f;
	gWhiteMaterial.Ambient.a = 0.0f;
	gWhiteMaterial.Specular = gWhiteMaterial.Ambient;
	gWhiteMaterial.Emissive = gWhiteMaterial.Ambient;
	gWhiteMaterial.Power = 1.0f;
}

DARKSDK void Constructor ( HINSTANCE hSetup, HINSTANCE hImage )
{
	ConstructorD3D ( hSetup, hImage );
}

DARKSDK void FreeParticle( tagParticleData* ptr )
{
	// Delete particles
	PARTICLE* pParticle = ptr->m_pParticles;
	while ( pParticle )
	{
		PARTICLE* pNextParticle = pParticle->m_pNext;
		SAFE_DELETE(pParticle);
		pParticle = pNextParticle;
	}
	pParticle = ptr->m_pParticlesFree;
	while ( pParticle )
	{
		PARTICLE* pNextParticle = pParticle->m_pNext;
		SAFE_DELETE(pParticle);
		pParticle = pNextParticle;
	}

	// Delete vertx buffer and obj
	SAFE_RELEASE(ptr->m_pVB);
	SAFE_DELETE(ptr);
}

DARKSDK void DestructorD3D ( void )
{
	link* check = m_ParticleManager.GetList()->m_start;
	while(check)
	{
		tagParticleData* ptr = NULL;
		ptr = m_ParticleManager.GetData ( check->id );
		if ( ptr == NULL ) return;

		// Free any effects
		if ( ptr->m_pSnow )
		{
			ptr->m_pSnow->KillSystem ( );
			delete ptr->m_pSnow;
			ptr->m_pSnow=NULL;
		}
		FreeParticle ( ptr );

		check = check->next;
	}
	m_ParticleManager.DeleteAll();

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
	// Global Data passed in
	g_pGlob = (GlobStruct*)pGlobPtr;

	/*
	// Acquire Camera AutoCam Function
	g_Camera3D_SetAutoCam = ( CAMERA3D_SetAutoCam ) GetProcAddress ( g_pGlob->g_Camera3D, "?SetAutoCam@@YAXMMMM@Z" );

	// Assign function ptrs for vector handling
	g_Types_GetVector					= ( GetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetVector3@@YA?AUD3DXVECTOR3@@H@Z" );
	g_Types_SetVector					= ( SetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?SetVector3@@YAXHMMM@Z" );
	g_Types_GetExist					= ( GetExistPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetExist@@YAHH@Z" );
	*/

	#ifndef DARKSDK_COMPILE
		g_Camera3D_SetAutoCam = ( CAMERA3D_SetAutoCam ) GetProcAddress ( g_pGlob->g_Camera3D, "?SetAutoCam@@YAXMMMM@Z" );

		g_Types_GetVector					= ( GetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetVector3@@YA?AUD3DXVECTOR3@@H@Z" );
		g_Types_SetVector					= ( SetVectorPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?SetVector3@@YAXHMMM@Z" );
		g_Types_GetExist					= ( GetExistPFN )					GetProcAddress ( g_pGlob->g_Vectors, "?GetExist@@YAHH@Z" );
	#else
		g_Camera3D_SetAutoCam = dbSetAutoCam;

		g_Types_GetVector = dbGetVector3;
		g_Types_SetVector = dbSetVector3;
		g_Types_GetExist = dbGet3DMathsExist;
	#endif
}
//#endif

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

DARKSDK void Update ( void )
{
	m_ParticleManager.Update ( );
}

bool UpdateParticlePtr ( int iID )
{
	m_ptr  = NULL;
	m_ptr  = m_ParticleManager.GetData ( iID );

	if ( m_ptr == NULL )
		return false;

	
	m_pParticlePos = ( tagObjectPos* ) m_ptr;

	return true;
}

DARKSDK void Delete ( int iID )
{
	if ( !UpdateParticlePtr ( iID ) )
		return;

	// Free any effects
	if ( m_ptr->m_pSnow )
	{
		m_ptr->m_pSnow->KillSystem ( );
		delete m_ptr->m_pSnow;
		m_ptr->m_pSnow=NULL;
	}

	FreeParticle ( m_ptr );
	m_ParticleManager.Delete ( iID );
}

DARKSDK void GetCullDataFromModel(int)
{
}

DARKSDK bool Create ( int iID, DWORD dwFlush, DWORD dwDiscard, float fRadius, int iImage )
{
	HRESULT				hr;
	tagParticleData		m_Data;

	memset ( &m_Data, 0, sizeof ( m_Data ) );

	m_Data.pD3D             = m_pD3D;
	m_Data.m_fRadius        = fRadius;
	m_Data.m_dwBase         = dwDiscard;
	m_Data.m_dwDiscard      = dwDiscard;
	m_Data.m_dwFlush        = dwFlush;
	m_Data.m_dwParticles    = 0;
	m_Data.m_dwParticlesLim = 2048;
	m_Data.m_pParticles     = NULL;
	m_Data.m_pParticlesFree = NULL;
	m_Data.m_pVB            = NULL;

	m_Data.fSecsPerFrame        = 0.005f;
	m_Data.dwNumParticlesToEmit = 4;
	m_Data.fEmitVel             = 4.0;
	m_Data.vPosition			= D3DXVECTOR3 ( 0.0, 0.0, 0.0 );
	m_Data.EmitColor  		    = D3DXCOLOR   ( 1.0, 1.0, 1.0, 1.0 );
	m_Data.fTime = 0.0f;

	m_Data.m_bEffect			= false;
	m_Data.m_pSnow				= NULL;

	m_Data.m_bVisible			= true;
	m_Data.m_fGravity			= 5.0f;
	m_Data.m_iLife				= 100;
	m_Data.m_iFloorActive		= 1;
	m_Data.m_fChaos				= 0.0f;

	m_Data.m_bGhost				= true;
	m_Data.m_iGhostMode			= -1;
	m_Data.m_fGhostPercentage	= -1.0f;
	
	m_Data.m_pTexture = g_Image_GetPointer ( iImage );

	if ( FAILED ( hr = m_pD3D->CreateVertexBuffer ( 
														dwDiscard * sizeof ( POINTVERTEX ), 
														D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
														D3DFVF_POINTVERTEX,
														D3DPOOL_DEFAULT, 
														&m_Data.m_pVB,
														NULL
												  ) ) )
	{
		RunTimeError ( RUNTIMEERROR_PARTICLESCOULDNOTBECREATED );
		return false;
	}

	m_ParticleManager.Add ( &m_Data, iID );

	// Scale controlled by radius
	float fScale = fRadius*100.0f;
	ParticleScale  ( iID, fScale,fScale,fScale );

	ParticlePosition ( iID, 0.0f, 0.0f, 0.0f );
	ParticleRotate ( iID, 0.0f, 0.0f, 0.0f );

	return true;
}

DARKSDK void SetColor ( int iID, int iRed, int iGreen, int iBlue )
{
	if ( !UpdateParticlePtr ( iID ) )
		return;

	m_ptr->EmitColor = D3DXCOLOR ( ( float ) iRed / 100.0f, ( float ) iGreen / 100.0f, ( float ) iBlue / 100.0f, 1.0f );
}

DARKSDK void SetTime ( int iID, float fTime )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	m_ptr->fTime = fTime;
}

// DBPro commands

DARKSDK void CreateEx ( int iID, int iImageIndex, int maxParticles, float fRadius )
{
	if ( UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESALREADYEXISTS);
		return;
	}

	DWORD dwFlush = maxParticles;
	DWORD dwDiscard = dwFlush*5;
	Create ( iID, dwFlush, dwDiscard, fRadius, iImageIndex );

	// Set autocam
	#if DB_PRO
	g_Camera3D_SetAutoCam ( 0.0f, 0.0f, 0.0f, fRadius );
	#endif
}

DARKSDK void DeleteEx ( int iID )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	Delete ( iID );
}

DARKSDK void SetSecondsPerFrame ( int iID, float fTime )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	m_ptr->fSecsPerFrame = fTime;
}

DARKSDK void SetNumberOfEmmissions ( int iID, int iNumber )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	m_ptr->dwNumParticlesToEmit = iNumber;
}

DARKSDK void SetVelocity ( int iID, float fVelocity )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	m_ptr->fEmitVel = fVelocity;
}

DARKSDK void SetPosition ( int iID, float fX, float fY, float fZ )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}
	ParticlePosition ( iID,  fX, fY, fZ );
}

DARKSDK void SetEmitPosition ( int iID, float fX, float fY, float fZ )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	fX /= m_ptr->m_fRadius;
	fY /= m_ptr->m_fRadius;
	fZ /= m_ptr->m_fRadius;

	m_ptr->vPosition = D3DXVECTOR3 ( fX, fY, fZ );
}

DARKSDK void SetColorEx ( int iID, int iRed, int iGreen, int iBlue )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	m_ptr->EmitColor = D3DXCOLOR ( ( float ) iRed / 255.0f, ( float ) iGreen / 255.0f, ( float ) iBlue / 255.0f, 1.0f );
}

DARKSDK void SetRotation ( int iID, float fX, float fY, float fZ )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}
	ParticleRotate(iID, fX, fY, fZ);
}

DARKSDK void SetGravity	( int iID, float fGravity )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}	
	m_ptr->m_fGravity=fGravity;
}

DARKSDK void SetLife ( int iID, int iLifeValue )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}	
	m_ptr->m_iLife=iLifeValue;
}

DARKSDK void SetMask				( int iID, int iCameraMask )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}
DARKSDK void EmitSingleParticle		( int iID, float fX, float fY, float fZ )
{
	// MessageBox ( NULL, "DX10", "", MB_OK );
}

DARKSDK void SetFloor ( int iID, int iFlag )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}	
	m_ptr->m_iFloorActive=iFlag;
}

DARKSDK void SetChaos ( int iID, float fChaos )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}
	m_ptr->m_fChaos=fChaos/1000.0f;
}

DARKSDK void GhostOn ( int iID, int iMode )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	m_ptr->m_bGhost = true;
	m_ptr->m_iGhostMode = iMode;
}

DARKSDK void GhostOff ( int iID )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	m_ptr->m_bGhost = false;
	m_ptr->m_iGhostMode = -1;
	m_ptr->m_fGhostPercentage = -1.0f;
}

DARKSDK void Ghost ( int iID, int iMode, float fPercentage )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	// clip value
	if ( fPercentage >= 0.0f && fPercentage <= 100.0f )
	{
		m_ptr->m_bGhost = true;
		m_ptr->m_iGhostMode = iMode;
		m_ptr->m_fGhostPercentage = fPercentage;
	}
}

DARKSDK void Hide ( int iID )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	m_ptr->m_bVisible=false;
}

DARKSDK void Show ( int iID )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}

	m_ptr->m_bVisible=true;
}

DARKSDK void CreateDriftFallEffect ( int iID, int ImageIndex, int maxParticles, float fX, float fY, float fZ, float fWidth, float fHeight, float fDepth, bool bRevEffect )
{
	if ( UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESALREADYEXISTS);
		return;
	}

	// DBPro - adjust so uses same area dimension as a centered box (MAKE BOX)
	fWidth/=2;
	fHeight/=2;
	fDepth/=2;
	fY-=(fHeight/2);

	tagParticleData		m_Data;
	memset ( &m_Data, 0, sizeof ( m_Data ) );
	m_Data.pD3D					= m_pD3D;
	m_Data.m_fRadius			= 0;
	m_Data.m_dwBase				= 0;
	m_Data.m_dwDiscard			= 0;
	m_Data.m_dwFlush			= 0;
	m_Data.m_dwParticles		= 0;
	m_Data.m_dwParticlesLim		= 2048;
	m_Data.m_pParticles			= NULL;
	m_Data.m_pParticlesFree		= NULL;
	m_Data.m_pVB				= NULL;

	// leeadd - 070306 - u60 -used for drift now
	m_Data.dwNumParticlesToEmit = (DWORD)SNOWFLAKES_PER_SEC;

	// This particle object is an effect
	m_Data.m_bEffect			= true;
	m_Data.m_pSnow				= new CSnowstorm ( maxParticles, D3DXVECTOR3 ( 0, 0, 0 ), fHeight, fWidth, fDepth, bRevEffect );
	m_Data.m_pSnow->InitializeSystem ( ImageIndex );

	m_Data.m_bVisible			= true;
	m_Data.m_fGravity			= 5.0f;
	m_Data.m_iLife				= 100;
	m_Data.m_iFloorActive		= 1;
	m_Data.m_fChaos				= 0.0f;

	m_Data.m_bGhost				= true;
	m_Data.m_iGhostMode			= -1;

	// Add to list
	m_ParticleManager.Add ( &m_Data, iID );

	// Default particle position
	ParticleScale  ( iID, 100, 100, 100 );
	ParticlePosition ( iID, fX, fY, fZ );
	ParticleRotate ( iID, 0.0f, 0.0f, 0.0f );

	// Initial creation of particles
	for (int i = 0; i < maxParticles; i++)
		m_Data.m_pSnow->Update ( 0.1f, m_Data.dwNumParticlesToEmit  );

	// Set autocam
	#if DB_PRO
	g_Camera3D_SetAutoCam ( 0.0f, 0.0f, 0.0f, fY+fZ );
	#endif
}

DARKSDK void CreateSnowEffect ( int iID, int ImageIndex, int maxParticles, float fX, float fY, float fZ, float fWidth, float fHeight, float fDepth )
{
	// Snow drifts down
	CreateDriftFallEffect ( iID, ImageIndex, maxParticles, fX, fY, fZ, fWidth, fHeight, fDepth, false );
}

DARKSDK void CreateFireEffect ( int iID, int ImageIndex, int maxParticles, float fX, float fY, float fZ, float fWidth, float fHeight, float fDepth )
{
	// Fire drifts up
	CreateDriftFallEffect ( iID, ImageIndex, maxParticles, fX, fY, fZ, fWidth, fHeight, fDepth, true );
}

DARKSDK void  SetPositionVector3 ( int iID, int iVector )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	D3DXVECTOR3 vec = g_Types_GetVector ( iVector );
	ParticlePosition ( iID,  vec.x, vec.y, vec.z );
}

DARKSDK void GetPositionVector3 ( int iVector, int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	g_Types_SetVector ( iVector, m_ptr->vecPosition.x, m_ptr->vecPosition.y, m_ptr->vecPosition.z );
}

DARKSDK void  SetRotationVector3 ( int iID, int iVector )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	D3DXVECTOR3 vec = g_Types_GetVector ( iVector );
	ParticleRotate ( iID,  vec.x, vec.y, vec.z );
}

DARKSDK void GetRotationVector3 ( int iVector, int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNUMBERILLEGAL);
		return;
	}
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return;
	}
	if ( !g_Types_GetExist ( iVector ) )
	{
		RunTimeError ( RUNTIMEERROR_VECTORNOTEXIST );
		return;
	}
	g_Types_SetVector ( iVector, m_ptr->vecRotate.x, m_ptr->vecRotate.y, m_ptr->vecRotate.z );
}

// 
// expressions
//

DARKSDK SDK_BOOL GetExist ( int iID )
{
	if ( UpdateParticlePtr ( iID ) )
		return true;
	else
		return false;
}

DARKSDK SDK_FLOAT GetPositionXEx ( int iID )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return 0;
	}

	float fResult = m_ptr->vecPosition.x;
	return SDK_RETFLOAT(fResult);
}

DARKSDK SDK_FLOAT GetPositionYEx ( int iID )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return 0;
	}

	float fResult = m_ptr->vecPosition.y;
	return SDK_RETFLOAT(fResult);
}

DARKSDK SDK_FLOAT GetPositionZEx ( int iID )
{
	if ( !UpdateParticlePtr ( iID ) )
	{
		RunTimeError(RUNTIMEERROR_PARTICLESNOTEXIST);
		return 0;
	}

	float fResult = m_ptr->vecPosition.z;
	return SDK_RETFLOAT(fResult);
}


//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE


void ConstructorParticles ( HINSTANCE hSetup, HINSTANCE hImage )
{
	Constructor ( hSetup, hImage );
}

void DestructorParticles ( void )
{
	Destructor ( );
}

void SetErrorHandlerParticles ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataParticles ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DParticles ( int iMode )
{
	RefreshD3D ( iMode );
}

void UpdateParticles ( void )
{
	Update ( );
}

bool dbMake ( int iID, DWORD dwFlush, DWORD dwDiscard, float fRadius, int iImage )
{
	return Create ( iID, dwFlush, dwDiscard, fRadius, iImage );
}

void dbDelete ( int iID )
{
	Delete ( iID );
}

void dbSetColor ( int iID, int iRed, int iGreen, int iBlue )
{
	SetColor ( iID, iRed, iGreen, iBlue );
}

void dbSetTime ( int iID, float fTime )
{
	SetTime ( iID, fTime );
}

void dbMakeParticles ( int iID, int iImageIndex, int maxParticles, float fRadius )
{
	CreateEx ( iID, iImageIndex, maxParticles, fRadius );
}

void dbDeleteParticles ( int iID )
{
	DeleteEx ( iID );
}

void dbSetParticleSpeed ( int iID, float fTime )
{
	SetSecondsPerFrame ( iID, fTime );
}

void dbSetParticleEmmissions ( int iID, int iNumber )
{
	SetNumberOfEmmissions ( iID, iNumber );
}

void dbSetParticleVelocity ( int iID, float fVelocity )
{
	SetVelocity ( iID, fVelocity );
}

void dbPositionParticles ( int iID, float fX, float fY, float fZ )
{
	SetPosition ( iID, fX, fY, fZ );
}

void dbPositionParticleEmmissions ( int iID, float fX, float fY, float fZ )
{
	SetEmitPosition (  iID,  fX,  fY,  fZ );
}

void dbColorParticles ( int iID, int iRed, int iGreen, int iBlue )
{
	SetColorEx (  iID,  iRed,  iGreen, iBlue );
}

void dbRotateParticles ( int iID, float fX, float fY, float fZ )
{
	SetRotation (  iID,  fX,  fY,  fZ );
}

void dbSetParticleGravity ( int iID, float fGravity )
{
	SetGravity (  iID,  fGravity );
}

void dbSetParticleChaos ( int iID, float fChaos )
{
	SetChaos (  iID,  fChaos );
}

void dbSetParticleLife ( int iID, int iLifeValue )
{
	SetLife (  iID,  iLifeValue );
}

void dbSetParticleFloor ( int iID, int iFlag )
{
	SetFloor (  iID,  iFlag );
}

void dbGhostParticlesOn ( int iID, int iMode )
{
	GhostOn (  iID,  iMode );
}

void dbGhostParticlesOff ( int iID )
{
	GhostOff (  iID );
}

void dbGhostParticlesOn ( int iID, int iMode, float fPercentage )
{
	Ghost (  iID,  iMode,  fPercentage );
}

void dbHideParticles ( int iID )
{
	Hide (  iID );
}

void dbShowParticles ( int iID )
{
	Show ( iID );
}

void dbMakeSnowParticles ( int iID, int iImageIndex, int maxParticles, float fX, float fY, float fZ, float fWidth, float fHeight, float fDepth )
{
	CreateSnowEffect (  iID,  iImageIndex,  maxParticles,  fX,  fY,  fZ,  fWidth,  fHeight,  fDepth );
}

void dbMakeFireParticles ( int iID, int iImageIndex, int maxParticles, float fX, float fY, float fZ, float fWidth, float fHeight, float fDepth )
{
	CreateFireEffect (  iID,  iImageIndex,  maxParticles,  fX,  fY,  fZ,  fWidth,  fHeight,  fDepth );
}

void dbPositionParticles ( int iID, int iVector )
{
	SetPositionVector3 (  iID,  iVector );
}

void dbSetVector3ToParticlesPosition ( int iVector, int iID )
{
	GetPositionVector3 ( iVector,  iID );
}

void dbRotateParticles ( int iID, int iVector )
{
	SetRotationVector3 (  iID,  iVector );
}

void dbSetVector3ToParticlesRotation ( int iVector, int iID )
{
	GetRotationVector3 (  iVector,  iID );
}

bool dbParticlesExist ( int iID )
{
	if ( GetExist ( iID )==0 )
		return false;
	else
		return true;
}

float dbParticlesPositionX	( int iID )
{
	DWORD dwReturn = GetPositionXEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbParticlesPositionY	( int iID )
{
	DWORD dwReturn = GetPositionYEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbParticlesPositionZ	( int iID )
{
	DWORD dwReturn = GetPositionZEx ( iID );
	
	return *( float* ) &dwReturn;
}

// lee - 300706 - GDK fixes
void dbSetParticleEmissions	( int iID, int iNumber ) { dbSetParticleEmmissions ( iID, iNumber ); }
void dbPositionParticleEmissions ( int iID, float fX, float fY, float fZ ) { SetEmitPosition ( iID, fX, fY, fZ ); }

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////