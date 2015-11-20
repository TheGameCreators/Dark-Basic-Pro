#include "loader.h"
#include <direct.h>
#include "textures.h"
#include "collision.h"
#include ".\..\error\cerror.h"
#include "..\..\darksdk\q3bsp\resource.h"

// Include Global Structure
#include ".\..\core\globstruct.h"


#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKCamera.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKBasic3D.h"
#endif

#include "cBSPFile.h"

// Global Shared Data Pointer (passed in from core)
DBPRO_GLOBAL GlobStruct*						g_pGlob							= NULL;

extern files_found Q3A_Resources;
extern files_found Q2_Resources;

#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 )

DBPRO_GLOBAL LPDIRECT3DDEVICE9		m_pD3D = NULL;

#define Q3A_MODE_LIGHTMAP		0
#define Q3A_MODE_VERTEXLIGHTING 1

void gamma_rgb ( byte* p, int size, float factor );
void swap      ( float*  x, float*  y );
void swap      ( double* x, double* y );
void swap      ( int*    x, int*    y );
void Q3ARender ( void );

#include < vector >
using namespace std;
typedef vector < int > int_vec;

// Setup Functions
DBPRO_GLOBAL RetD3DFunctionPointerPFN	g_Setup_GetDirect3DDevice;

// Camera3D Functions
DBPRO_GLOBAL CAMERA_Int1Float3PFN		g_Camera_Position;
DBPRO_GLOBAL CAMERA_GetFloatPFN			g_Camera_GetXPosition;
DBPRO_GLOBAL CAMERA_GetFloatPFN			g_Camera_GetYPosition;
DBPRO_GLOBAL CAMERA_GetFloatPFN			g_Camera_GetZPosition;

DBPRO_GLOBAL CAMERA_GetFloatPFN			g_Camera_GetXLook;
DBPRO_GLOBAL CAMERA_GetFloatPFN			g_Camera_GetYLook;
DBPRO_GLOBAL CAMERA_GetFloatPFN			g_Camera_GetZLook;

// Object Functions
DBPRO_GLOBAL CAMERA_Int1Float3PFN		g_Object_Position;
DBPRO_GLOBAL CAMERA_GetFloatPFN			g_Object_GetXPosition;
DBPRO_GLOBAL CAMERA_GetFloatPFN			g_Object_GetYPosition;
DBPRO_GLOBAL CAMERA_GetFloatPFN			g_Object_GetZPosition;

// Quake1and2BSP Functions
DBPRO_GLOBAL  BSP_Constructor				g_Q2BSP_Constructor;
DBPRO_GLOBAL  BSP_Destructor				g_Q2BSP_Destructor;
DBPRO_GLOBAL  RetVoidFCoreDataPFN			g_Q2BSP_PassCoreData;
DBPRO_GLOBAL  BSP_Load					g_Q2BSP_Load;
DBPRO_GLOBAL  BSP_Update					g_Q2BSP_Update;
DBPRO_GLOBAL  BSP_Delete					g_Q2BSP_Delete;
DBPRO_GLOBAL  BSP_SetupCameraCollision	g_Q2BSP_SetupCameraCollision;
DBPRO_GLOBAL  BSP_SetupCameraCollisionR	g_Q2BSP_SetCameraCollisionRadius;
DBPRO_GLOBAL  BSP_SetupCameraCollisionR	g_Q2BSP_SetObjectCollisionRadius;
DBPRO_GLOBAL  BSP_SetupObjectCollision	g_Q2BSP_SetupObjectCollision;
DBPRO_GLOBAL  BSP_SetupCullingCamera		g_Q2BSP_SetupCullingCamera;
DBPRO_GLOBAL  BSP_SetupCollisionOff		g_Q2BSP_SetupCollisionOff;
DBPRO_GLOBAL  BSP_SetCollisionThreshhold	g_Q2BSP_SetCollisionThreshhold;
DBPRO_GLOBAL  BSP_SetCollisionHeightAdjustment g_Q2BSP_SetCollisionHeightAdjustment;
DBPRO_GLOBAL  BSP_Start					g_Q2BSP_Start;
DBPRO_GLOBAL  BSP_End						g_Q2BSP_End;

DBPRO_GLOBAL  BSP_Start					g_Q2BSP_SetHardwareMultiTexturingOff;
DBPRO_GLOBAL  BSP_Start					g_Q2BSP_SetHardwareMultiTexturingOn;


// OwnBSP Functions
DBPRO_GLOBAL  BSP_Constructor				g_OwnBSP_Constructor;
DBPRO_GLOBAL  BSP_Destructor				g_OwnBSP_Destructor;
DBPRO_GLOBAL  RetVoidFCoreDataPFN			g_OwnBSP_PassCoreData;
DBPRO_GLOBAL  BSPOwn_Load					g_OwnBSP_Load;
DBPRO_GLOBAL  BSP_Update					g_OwnBSP_Update;
DBPRO_GLOBAL  BSP_Delete					g_OwnBSP_Delete;
DBPRO_GLOBAL  BSP_SetupCameraCollision	g_OwnBSP_SetupCameraCollision;
DBPRO_GLOBAL  BSP_SetupObjectCollision	g_OwnBSP_SetupObjectCollision;
DBPRO_GLOBAL  BSP_SetupCameraCollisionR	g_OwnBSP_SetObjectCollisionRadius;
DBPRO_GLOBAL  BSP_SetupCameraCollisionR	g_OwnBSP_SetCameraCollisionRadius;
DBPRO_GLOBAL  BSP_SetupCullingCamera		g_OwnBSP_SetupCullingCamera;
DBPRO_GLOBAL  BSP_SetupCollisionOff		g_OwnBSP_SetupCollisionOff;
DBPRO_GLOBAL  BSP_SetCollisionThreshhold	g_OwnBSP_SetCollisionThreshhold;
DBPRO_GLOBAL  BSP_SetCollisionHeightAdjustment g_OwnBSP_SetCollisionHeightAdjustment;

DBPRO_GLOBAL  BSP_GetCollisionResponse	g_OwnBSP_GetCollisionResponse;
DBPRO_GLOBAL  BSP_GetCollisionX			g_OwnBSP_GetCollisionX;
DBPRO_GLOBAL  BSP_GetCollisionY			g_OwnBSP_GetCollisionY;
DBPRO_GLOBAL  BSP_GetCollisionZ			g_OwnBSP_GetCollisionZ;

// mike - 240604
DBPRO_GLOBAL  BSP_GetCollisionResponse	g_Q2BSP_GetCollisionResponse;
DBPRO_GLOBAL  BSP_GetCollisionX			g_Q2BSP_GetCollisionX;
DBPRO_GLOBAL  BSP_GetCollisionY			g_Q2BSP_GetCollisionY;
DBPRO_GLOBAL  BSP_GetCollisionZ			g_Q2BSP_GetCollisionZ;

DBPRO_GLOBAL  BSP_Start					g_OwnBSP_Start;
DBPRO_GLOBAL  BSP_ProcessCollision		g_OwnBSP_ProcessCollision;
DBPRO_GLOBAL  BSP_End						g_OwnBSP_End;

// BSPCompiler Functions
//BSPC_Constructor			g_BSPCompiler_Constructor;
//BSPC_Destructor				g_BSPCompiler_Destructor;
//BSPC_CompileBSP				g_BSPCompiler_CompileBSP;

// Internal Data Globals
DWORD						g_dwCameraCullMode;

//D3DXVECTOR3 GetPosition(D3DXVECTOR3 position, D3DXVECTOR3 velocity);

void DeleteMap ( void );

// 
// Internal Data Structures
//

enum eBSPCollisionType
{
	OBJECT_COLLISION,
	CAMERA_COLLISION,
};

struct sCollision
{
	D3DXVECTOR3			vecOldPosition;
	D3DXVECTOR3			vecNewPosition;
	float				fRadius;
	eBSPCollisionType	eType;
	int					iID;
	D3DXVECTOR3			vecRadius;
	int					iResponse;
	bool				bRespImpact;
	D3DXVECTOR3			vecResp;

	float			fAdjustY;

	// mike - 240604 - new additions
	float			fThreshold;
	bool			bStart;
};

sCollision			gCollision       [ 25 ];
bool				gCollisionSwitch [ 25 ];
bool				bActivate					= true;
int					gCurrentCamera				= 1;
float				gCameraHeightAdjustment		= 0.0f;

DWORD				g_dwBSPType					= 0;

//
// Internal Functions
//



#ifdef DARKSDK_COMPILE
	
	//IDirect3DDevice9* GetDirect3DDevice ( void );
	void  Position  ( int iID, float fX, float fY, float fZ );
	
	DWORD GetXPositionEx ( int iID );											// get x pos
	DWORD GetYPositionEx ( int iID );											// get y pos
	DWORD GetZPositionEx ( int iID );											// get z pos

	DWORD GetLookXEx ( int iID );
	DWORD GetLookYEx ( int iID );
	DWORD GetLookZEx ( int iID );

	DWORD dbtGetLookXExCamera ( int iID );
	DWORD dbtGetLookYExCamera ( int iID );
	DWORD dbtGetLookZExCamera ( int iID );
#endif

void Constructor ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject )
{
	// get function pointers
	#ifndef DARKSDK_COMPILE
		g_Setup_GetDirect3DDevice		= ( RetD3DFunctionPointerPFN )  GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
		g_Camera_Position               = ( CAMERA_Int1Float3PFN )		GetProcAddress ( hCamera, "?Position@@YAXHMMM@Z" );
		
		g_Camera_GetXPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetXPosition@@YAMH@Z" );
		g_Camera_GetYPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetYPosition@@YAMH@Z" );
		g_Camera_GetZPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetZPosition@@YAMH@Z" );

		g_Camera_GetXLook			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetLookX@@YAMH@Z" );
		g_Camera_GetYLook			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetLookY@@YAMH@Z" );
		g_Camera_GetZLook			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hCamera, "?GetLookZ@@YAMH@Z" );

		g_Object_Position				= ( CAMERA_Int1Float3PFN )		GetProcAddress ( hObject, "?Position@@YAXHMMM@Z" );
		g_Object_GetXPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hObject, "?GetXPosition@@YAMH@Z" );
		g_Object_GetYPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hObject, "?GetYPosition@@YAMH@Z" );
		g_Object_GetZPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( hObject, "?GetZPosition@@YAMH@Z" );
	#else
		g_Setup_GetDirect3DDevice =( RetD3DFunctionPointerPFN  ) dbGetDirect3DDevice;

		g_Camera_Position               = dbPositionCamera;
		
		g_Camera_GetXPosition			= ( CAMERA_GetFloatPFN )dbCameraPositionX;
		g_Camera_GetYPosition			= ( CAMERA_GetFloatPFN )dbCameraPositionY;
		g_Camera_GetZPosition			= ( CAMERA_GetFloatPFN )dbCameraPositionZ;

		//g_Camera_GetXLook				= ( CAMERA_GetFloatPFN )dbtGetLookXExCamera;
		//g_Camera_GetYLook				= ( CAMERA_GetFloatPFN )dbtGetLookYExCamera;
		//g_Camera_GetZLook				= ( CAMERA_GetFloatPFN ) dbtGetLookZExCamera;

		g_Object_Position				= dbPositionObject;
		g_Object_GetXPosition			= dbObjectPositionX;
		g_Object_GetYPosition			= dbObjectPositionY;
		g_Object_GetZPosition			= dbObjectPositionZ;
	#endif

	// get direct3d
	m_pD3D = g_Setup_GetDirect3DDevice ( );

	// mike - 240604 - clear structure
	for ( int i = 0; i < 25; i++ )
	{
		gCollision [ i ].fThreshold = 0.003f;
		gCollision [ i ].bStart = false;
	}
}

void Destructor ( void )
{
	// Free other BSP Formats
//	if(g_BSPCompiler_Destructor) g_BSPCompiler_Destructor();
	if(g_Q2BSP_Destructor) g_Q2BSP_Destructor();
	if(g_OwnBSP_Destructor) g_OwnBSP_Destructor();

	files_cache.Release ( );
	
	shaders.Release ( );
	Textures.Reset ( );

	Q3MAP.Release ( );
	//FreeColOpt();

	DeleteMap ( );
}

void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

void PassCoreData( LPVOID pGlobPtr )
{

	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;

	#ifndef DARKSDK_COMPILE
		// get function pointers
		g_Setup_GetDirect3DDevice		= ( RetD3DFunctionPointerPFN )  GetProcAddress ( g_pGlob->g_GFX, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
		g_Camera_Position               = ( CAMERA_Int1Float3PFN )		GetProcAddress ( g_pGlob->g_Camera3D, "?Position@@YAXHMMM@Z" );
		g_Camera_GetXPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( g_pGlob->g_Camera3D, "?GetXPosition@@YAMH@Z" );
		g_Camera_GetYPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( g_pGlob->g_Camera3D, "?GetYPosition@@YAMH@Z" );
		g_Camera_GetZPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( g_pGlob->g_Camera3D, "?GetZPosition@@YAMH@Z" );

		g_Camera_GetXLook			= ( CAMERA_GetFloatPFN )		GetProcAddress ( g_pGlob->g_Camera3D, "?GetLookX@@YAMH@Z" );
		g_Camera_GetYLook			= ( CAMERA_GetFloatPFN )		GetProcAddress ( g_pGlob->g_Camera3D, "?GetLookY@@YAMH@Z" );
		g_Camera_GetZLook			= ( CAMERA_GetFloatPFN )		GetProcAddress ( g_pGlob->g_Camera3D, "?GetLookZ@@YAMH@Z" );

		g_Object_Position				= ( CAMERA_Int1Float3PFN )		GetProcAddress ( g_pGlob->g_Basic3D, "?Position@@YAXHMMM@Z" );
		g_Object_GetXPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( g_pGlob->g_Basic3D, "?GetXPosition@@YAMH@Z" );
		g_Object_GetYPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( g_pGlob->g_Basic3D, "?GetYPosition@@YAMH@Z" );
		g_Object_GetZPosition			= ( CAMERA_GetFloatPFN )		GetProcAddress ( g_pGlob->g_Basic3D, "?GetZPosition@@YAMH@Z" );

		// get Q1and2BSP function pointers
		g_Q2BSP_Constructor					 = ( BSP_Constructor )				GetProcAddress ( g_pGlob->g_Q2BSP, "?Constructor@@YAXPAUHINSTANCE__@@000@Z" );
		g_Q2BSP_Destructor					 = ( BSP_Destructor )				GetProcAddress ( g_pGlob->g_Q2BSP, "?Destructor@@YAXXZ" );
		g_Q2BSP_PassCoreData				 = ( RetVoidFCoreDataPFN )			GetProcAddress ( g_pGlob->g_Q2BSP, "?PassCoreData@@YAXPAX@Z" );

		g_Q2BSP_Load						 = ( BSP_Load )						GetProcAddress ( g_pGlob->g_Q2BSP, "?Load@@YA_NPAD0@Z" );
		g_Q2BSP_Update						 = ( BSP_Update )					GetProcAddress ( g_pGlob->g_Q2BSP, "?Update@@YAXXZ" );
		g_Q2BSP_Delete						 = ( BSP_Delete )					GetProcAddress ( g_pGlob->g_Q2BSP, "?DeleteEx@@YAXXZ" );
		g_Q2BSP_SetupCameraCollision		 = ( BSP_SetupCameraCollision )		GetProcAddress ( g_pGlob->g_Q2BSP, "?SetupCameraCollision@@YAXHHMH@Z" );
		g_Q2BSP_SetupObjectCollision		 = ( BSP_SetupObjectCollision )		GetProcAddress ( g_pGlob->g_Q2BSP, "?SetupObjectCollision@@YAXHHMH@Z" );
		g_Q2BSP_SetupCullingCamera			 = ( BSP_SetupCullingCamera )		GetProcAddress ( g_pGlob->g_Q2BSP, "?SetupCullingCamera@@YAXH@Z" );
		g_Q2BSP_SetupCollisionOff			 = ( BSP_SetupCollisionOff )		GetProcAddress ( g_pGlob->g_Q2BSP, "?SetupCollisionOff@@YAXH@Z" );
		g_Q2BSP_SetCollisionThreshhold		 = ( BSP_SetCollisionThreshhold )	GetProcAddress ( g_pGlob->g_Q2BSP, "?SetCollisionThreshhold@@YAXHM@Z" );
		g_Q2BSP_SetCollisionHeightAdjustment = ( BSP_SetCollisionHeightAdjustment )	GetProcAddress ( g_pGlob->g_Q2BSP, "?SetCollisionHeightAdjustment@@YAXHM@Z" );

		g_Q2BSP_Start						 = ( BSP_Start )					GetProcAddress ( g_pGlob->g_Q2BSP, "?Start@@YAXXZ" );
		g_Q2BSP_End							 = ( BSP_End )						GetProcAddress ( g_pGlob->g_Q2BSP, "?End@@YAXXZ" );
		g_Q2BSP_SetCameraCollisionRadius	 = ( BSP_SetupCameraCollisionR )	GetProcAddress ( g_pGlob->g_Q2BSP, "?SetCameraCollisionRadius@@YAXHHMMM@Z" );
		g_Q2BSP_SetObjectCollisionRadius     = ( BSP_SetupCameraCollisionR )	GetProcAddress ( g_pGlob->g_Q2BSP, "?SetObjectCollisionRadius@@YAXHHMMM@Z" );
		g_Q2BSP_SetHardwareMultiTexturingOff = ( BSP_Start )					GetProcAddress ( g_pGlob->g_Q2BSP, "?SetHardwareMultiTexturingOff@@YAXXZ" );
		g_Q2BSP_SetHardwareMultiTexturingOn  = ( BSP_Start )					GetProcAddress ( g_pGlob->g_Q2BSP, "?SetHardwareMultiTexturingOn@@YAXXZ" );

		// get OwnBSP function pointers
		g_OwnBSP_Constructor			  = ( BSP_Constructor )				GetProcAddress ( g_pGlob->g_OwnBSP, "?Constructor@@YAXPAUHINSTANCE__@@000@Z" );
		g_OwnBSP_Destructor				  = ( BSP_Destructor )				GetProcAddress ( g_pGlob->g_OwnBSP, "?Destructor@@YAXXZ" );
		g_OwnBSP_PassCoreData			  = ( RetVoidFCoreDataPFN )			GetProcAddress ( g_pGlob->g_OwnBSP, "?PassCoreData@@YAXPAX@Z" );
		g_OwnBSP_Load					  = ( BSPOwn_Load )					GetProcAddress ( g_pGlob->g_OwnBSP, "?Load@@YA_NPAD@Z" );
		g_OwnBSP_Update					  = ( BSP_Update )					GetProcAddress ( g_pGlob->g_OwnBSP, "?Update@@YAXXZ" );
		g_OwnBSP_Delete					  = ( BSP_Delete )					GetProcAddress ( g_pGlob->g_OwnBSP, "?DeleteEx@@YAXXZ" );
		g_OwnBSP_SetupCameraCollision	  = ( BSP_SetupCameraCollision )	GetProcAddress ( g_pGlob->g_OwnBSP, "?SetupCameraCollision@@YAXHHMH@Z" );
		g_OwnBSP_SetupObjectCollision	  = ( BSP_SetupObjectCollision )	GetProcAddress ( g_pGlob->g_OwnBSP, "?SetupObjectCollision@@YAXHHMH@Z" );
		g_OwnBSP_SetupCullingCamera		  = ( BSP_SetupCullingCamera )		GetProcAddress ( g_pGlob->g_OwnBSP, "?SetupCullingCamera@@YAXH@Z" );
		g_OwnBSP_SetupCollisionOff		  = ( BSP_SetupCollisionOff )		GetProcAddress ( g_pGlob->g_OwnBSP, "?SetupCollisionOff@@YAXH@Z" );
		g_OwnBSP_SetCollisionThreshhold	  = ( BSP_SetCollisionThreshhold )	GetProcAddress ( g_pGlob->g_OwnBSP, "?SetCollisionThreshhold@@YAXHM@Z" );
		g_OwnBSP_SetCollisionHeightAdjustment = ( BSP_SetCollisionHeightAdjustment )	GetProcAddress ( g_pGlob->g_OwnBSP, "?SetCollisionHeightAdjustment@@YAXHM@Z" );
		g_OwnBSP_Start					  = ( BSP_Start )					GetProcAddress ( g_pGlob->g_OwnBSP, "?Start@@YAXXZ" );
		g_OwnBSP_ProcessCollision		  = ( BSP_ProcessCollision )		GetProcAddress ( g_pGlob->g_OwnBSP, "?ProcessCollision@@YAXH@Z" );
		g_OwnBSP_End					  = ( BSP_End )						GetProcAddress ( g_pGlob->g_OwnBSP, "?End@@YAXXZ" );
		g_OwnBSP_SetCameraCollisionRadius = ( BSP_SetupCameraCollisionR )	GetProcAddress ( g_pGlob->g_OwnBSP, "?SetCameraCollisionRadius@@YAXHHMMM@Z" );
		g_OwnBSP_SetObjectCollisionRadius = ( BSP_SetupCameraCollisionR )	GetProcAddress ( g_pGlob->g_OwnBSP, "?SetObjectCollisionRadius@@YAXHHMMM@Z" );

		g_OwnBSP_GetCollisionResponse	  = ( BSP_GetCollisionResponse )	GetProcAddress ( g_pGlob->g_OwnBSP, "?GetCollisionResponse@@YAHH@Z" );
		g_OwnBSP_GetCollisionX			  = ( BSP_GetCollisionX )			GetProcAddress ( g_pGlob->g_OwnBSP, "?GetCollisionX@@YAMH@Z" );
		g_OwnBSP_GetCollisionY			  = ( BSP_GetCollisionY )			GetProcAddress ( g_pGlob->g_OwnBSP, "?GetCollisionY@@YAMH@Z" );
		g_OwnBSP_GetCollisionZ			  = ( BSP_GetCollisionZ )			GetProcAddress ( g_pGlob->g_OwnBSP, "?GetCollisionZ@@YAMH@Z" );

		g_Q2BSP_GetCollisionResponse	  = ( BSP_GetCollisionResponse )	GetProcAddress ( g_pGlob->g_Q2BSP, "?GetCollisionResponse@@YAHH@Z" );
		g_Q2BSP_GetCollisionX			  = ( BSP_GetCollisionX )			GetProcAddress ( g_pGlob->g_Q2BSP, "?GetCollisionX@@YAMH@Z" );
		g_Q2BSP_GetCollisionY			  = ( BSP_GetCollisionY )			GetProcAddress ( g_pGlob->g_Q2BSP, "?GetCollisionY@@YAMH@Z" );
		g_Q2BSP_GetCollisionZ			  = ( BSP_GetCollisionZ )			GetProcAddress ( g_pGlob->g_Q2BSP, "?GetCollisionZ@@YAMH@Z" );

		// get BSPCompiler function pointers
	//	g_BSPCompiler_Constructor			= ( BSPC_Constructor )			GetProcAddress ( g_pGlob->g_BSPCompiler, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
	//	g_BSPCompiler_Destructor			= ( BSPC_Destructor )			GetProcAddress ( g_pGlob->g_BSPCompiler, "?Destructor@@YAXXZ" );
	//	g_BSPCompiler_CompileBSP			= ( BSPC_CompileBSP )			GetProcAddress ( g_pGlob->g_BSPCompiler, "?CompileBSPEx@@YAXPAD0HMH@Z" );
	#else
		g_Setup_GetDirect3DDevice =( RetD3DFunctionPointerPFN  ) dbGetDirect3DDevice;
		g_Camera_Position               = dbPositionCamera;
		
		g_Camera_GetXPosition			= ( CAMERA_GetFloatPFN )dbCameraPositionX;
		g_Camera_GetYPosition			= ( CAMERA_GetFloatPFN )dbCameraPositionY;
		g_Camera_GetZPosition			= ( CAMERA_GetFloatPFN )dbCameraPositionZ;

		//g_Camera_GetXLook				= ( CAMERA_GetFloatPFN )dbtGetLookXExCamera;
		//g_Camera_GetYLook				= ( CAMERA_GetFloatPFN )dbtGetLookYExCamera;
		//g_Camera_GetZLook				= ( CAMERA_GetFloatPFN ) dbtGetLookZExCamera;

		g_Object_Position				= dbPositionObject;
		g_Object_GetXPosition			= dbObjectPositionX;
		g_Object_GetYPosition			= dbObjectPositionY;
		g_Object_GetZPosition			= dbObjectPositionZ;
	#endif

	// get direct3d
	if ( !m_pD3D )
	m_pD3D = g_Setup_GetDirect3DDevice ( );

	// Construct all other BSP modes
	if(g_OwnBSP_Constructor) g_OwnBSP_Constructor( g_pGlob->g_GFX, g_pGlob->g_Image, g_pGlob->g_Camera3D, g_pGlob->g_Basic3D );
	if(g_Q2BSP_Constructor) g_Q2BSP_Constructor( g_pGlob->g_GFX, g_pGlob->g_Image, g_pGlob->g_Camera3D, g_pGlob->g_Basic3D );
//	if(g_BSPCompiler_Constructor) g_BSPCompiler_Constructor( g_pGlob->g_GFX );

	// default settings for dbpro
	gCurrentCamera = 0;

	// Pass coreptr to other BSPs
	if(g_Q2BSP_PassCoreData) g_Q2BSP_PassCoreData((LPVOID)g_pGlob);
	if(g_OwnBSP_PassCoreData) g_OwnBSP_PassCoreData((LPVOID)g_pGlob);
}

void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		Destructor();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		Constructor ( g_pGlob->g_GFX, g_pGlob->g_Image, g_pGlob->g_Camera3D, g_pGlob->g_Basic3D );
		PassCoreData ( (LPVOID)g_pGlob );
	}
}

void StartSingleCollisionIndex ( int iTemp )
{
	if ( gCollision [ iTemp ].eType == CAMERA_COLLISION )
	{
		gCollision [ iTemp ].vecOldPosition = D3DXVECTOR3 ( 
																g_Camera_GetXPosition ( gCollision [ iTemp ].iID ),
																g_Camera_GetYPosition ( gCollision [ iTemp ].iID )-gCollision [ iTemp ].fAdjustY,
																g_Camera_GetZPosition ( gCollision [ iTemp ].iID )
														  );
	}
	else
	{
		gCollision [ iTemp ].vecOldPosition = D3DXVECTOR3 ( 
																g_Object_GetXPosition ( gCollision [ iTemp ].iID ),
																g_Object_GetYPosition ( gCollision [ iTemp ].iID )-gCollision [ iTemp ].fAdjustY,
																g_Object_GetZPosition ( gCollision [ iTemp ].iID )
														  );
	}
}

void Start ( void )
{
	for ( int iTemp = 0; iTemp < 25; iTemp++ )
	{
		if ( gCollisionSwitch [ iTemp ] )
		{
			StartSingleCollisionIndex ( iTemp );
		}
	}
}

void ProcessCollision ( int iTemp )
{
	if ( Q3MAP.vert_count <= 0 )
		return;

	if ( gCollisionSwitch [ iTemp ] )
	{
		if ( gCollision [ iTemp ].eType == CAMERA_COLLISION )
		{
			gCollision [ iTemp ].vecNewPosition = D3DXVECTOR3 ( 
																	g_Camera_GetXPosition ( gCollision [ iTemp ].iID ),
																	g_Camera_GetYPosition ( gCollision [ iTemp ].iID )-gCollision [ iTemp ].fAdjustY,
																	g_Camera_GetZPosition ( gCollision [ iTemp ].iID )
															  );

			
			if ( gCollision [ iTemp ].bStart == false )
			{
				gCollision [ iTemp ].vecOldPosition = gCollision [ iTemp ].vecNewPosition;
				gCollision [ iTemp ].bStart = true;
			}
		}
		else
		{
			gCollision [ iTemp ].vecNewPosition = D3DXVECTOR3 ( 
																	g_Object_GetXPosition ( gCollision [ iTemp ].iID ),
																	g_Object_GetYPosition ( gCollision [ iTemp ].iID )-gCollision [ iTemp ].fAdjustY,
																	g_Object_GetZPosition ( gCollision [ iTemp ].iID )
															  );

			if ( gCollision [ iTemp ].bStart == false )
			{
				gCollision [ iTemp ].vecOldPosition = gCollision [ iTemp ].vecNewPosition;
				gCollision [ iTemp ].bStart = true;
			}
		}

		// Snapshot where entity would have been
		D3DXVECTOR3 vecLatestPos;
		vecLatestPos = gCollision [ iTemp ].vecNewPosition;

		BOOL bClipped;

		if ( gCollision [ iTemp ].fRadius == -1.0f )
		{
		
			
				bClipped = Collision::World ( 
					gCollision [ iTemp ].vecOldPosition, 
					gCollision [ iTemp ].vecNewPosition, 
					gCollision [ iTemp ].vecRadius,
					&gCollision [ iTemp ].vecNewPosition, 
					5,
					gCollision [ iTemp ].fThreshold		// mike - 240604 - pass in threshold
				 );
			
		}
		else
		{
			//gCollision [ iTemp ].vecNewPosition = GetPosition ( gCollision [ iTemp ].vecOldPosition, gCollision [ iTemp ].vecNewPosition );

			
			
			bClipped = Collision::World
							( 
								gCollision [ iTemp ].vecOldPosition, 
								gCollision [ iTemp ].vecNewPosition, 
								D3DXVECTOR3 ( gCollision [ iTemp ].fRadius, gCollision [ iTemp ].fRadius, gCollision [ iTemp ].fRadius ),
								&gCollision [ iTemp ].vecNewPosition, 
								5,
								gCollision [ iTemp ].fThreshold
						   );	
			
		}

		// Return response to collision impact based on where it ended up
		gCollision [ iTemp ].vecResp = gCollision [ iTemp ].vecNewPosition - vecLatestPos;
		if ( bClipped==TRUE )
		{
			// Set for response detector
			gCollision [ iTemp ].bRespImpact = true;

			// Adjust position if mode correctly set
			if ( gCollision [ iTemp ].eType == CAMERA_COLLISION )
			{
				
				if ( gCollision [ iTemp ].iResponse == 0 )
				{
					g_Camera_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecNewPosition.x,
											gCollision [ iTemp ].vecNewPosition.y+gCollision [ iTemp ].fAdjustY, 
											gCollision [ iTemp ].vecNewPosition.z
									  );
				}
				if ( gCollision [ iTemp ].iResponse == 1 )
				{
					g_Camera_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecOldPosition.x,
											gCollision [ iTemp ].vecOldPosition.y+gCollision [ iTemp ].fAdjustY, 
											gCollision [ iTemp ].vecOldPosition.z
									  );
				}
			}
			else
			{
				if ( gCollision [ iTemp ].iResponse == 0 )
				{
					g_Object_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecNewPosition.x,
											gCollision [ iTemp ].vecNewPosition.y+gCollision [ iTemp ].fAdjustY, 
											gCollision [ iTemp ].vecNewPosition.z
									  );
				}
				if ( gCollision [ iTemp ].iResponse == 1 )
				{
					g_Object_Position ( 
											gCollision [ iTemp ].iID,
											gCollision [ iTemp ].vecOldPosition.x,
											gCollision [ iTemp ].vecOldPosition.y+gCollision [ iTemp ].fAdjustY, 
											gCollision [ iTemp ].vecOldPosition.z
									  );
				}
			}
		}
	}
}

static void End ( void )
{
	for ( int iTemp = 0; iTemp < 25; iTemp++ )
	{
		ProcessCollision ( iTemp );
	}
}

void BeginRenderStates ( void )
{
	//////////////////
	
	// set material
	D3DMATERIAL9 mtrl;
	memset ( &mtrl, 0, sizeof ( mtrl ) );
	mtrl.Ambient.r = 1.0f;
	mtrl.Ambient.g = 1.0f;
	mtrl.Ambient.b = 1.0f;
	mtrl.Ambient.a = 1.0f;
	mtrl.Diffuse.r = 1.0f;
	mtrl.Diffuse.g = 1.0f;
	mtrl.Diffuse.b = 1.0f;
	mtrl.Diffuse.a = 1.0f;
	m_pD3D->SetMaterial ( &mtrl );
	m_pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );

	// wireframe
	m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );

	// lighting
	m_pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );
	

	// fogenable
	//m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );

	// mike - 240604
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, TRUE );

	// transparency
	m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE,		FALSE );
	m_pD3D->SetRenderState ( D3DRS_ALPHAFUNC,			D3DCMP_ALWAYS );

	// ghost
	m_pD3D->SetRenderState ( D3DRS_ZENABLE,				TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

	// default material render states
	m_pD3D->SetRenderState ( D3DRS_COLORVERTEX,					TRUE );
	m_pD3D->SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE,		D3DMCS_COLOR1 );
	m_pD3D->SetRenderState ( D3DRS_SPECULARMATERIALSOURCE,		D3DMCS_MATERIAL );
	m_pD3D->SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE,		D3DMCS_MATERIAL );
	m_pD3D->SetRenderState ( D3DRS_EMISSIVEMATERIALSOURCE,		D3DMCS_MATERIAL );
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE,				FALSE );

	// texture coordinate data
	m_pD3D->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pD3D->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
	
	//////////////////


	/* from CustomBSP - reflection of BSPs
	float fX = g_Camera_GetXPosition ( gCurrentCamera );
	float fY = g_Camera_GetYPosition ( gCurrentCamera );
	float fZ = g_Camera_GetZPosition ( gCurrentCamera );
	g_dwCameraCullMode = D3DCULL_CCW;
	if(g_pGlob)
	{
		if(g_pGlob->dwStencilMode==2)
		{
			if(g_pGlob->dwRedrawPhase==1)
			{
				// View is a reflection
				fX = g_pGlob->fReflectionPlaneX;
				fY = g_pGlob->fReflectionPlaneY;
				fZ = g_pGlob->fReflectionPlaneZ;				
				g_dwCameraCullMode = D3DCULL_CW;
			}
		}
	}
	*/
}

void EndRenderStates ( void )
{
	// alpha blending
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,			FALSE );
	m_pD3D->SetRenderState ( D3DRS_SRCBLEND,					D3DBLEND_ONE );
	m_pD3D->SetRenderState ( D3DRS_DESTBLEND,					D3DBLEND_ZERO );
	m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE,				FALSE );

	// lighting
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE,				TRUE );

	// render misc
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,				TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZENABLE,						TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZFUNC,						D3DCMP_LESSEQUAL );
	m_pD3D->SetRenderState ( D3DRS_CULLMODE,					D3DCULL_CCW );

	// filter and mipmapping
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX,		0 );

	// mike - 240604
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
}

float fX = 0.0f;



void Update ( void )
{
	m_pD3D = g_Setup_GetDirect3DDevice ( );

	/*
		D3DXMATRIX	matTranslation;
		D3DXMATRIX	matRotation, matRotateX, matRotateY, matRotateZ;
		D3DXMATRIX	matScale;
		D3DXMATRIX	matObject;

		D3DXMatrixScaling ( &matScale, 1.0f, 1.0f, 1.0f );

		D3DXMatrixTranslation ( &matTranslation, 0.0f, 0.0f, 0.0f );

		D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( -90.0f ) );
		D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( 0.0f ) );
		D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( 0.0f ) );

		matRotation = matRotateX * matRotateY * matRotateZ;
		matObject = matRotation * matScale * matTranslation;
		*/



	//					fX = fX + 0.01f;
	

	D3DXMATRIX	matTranslation,
				matScale,
				matObject;

	// World Transform
	D3DXMatrixTranslation ( &matTranslation, 0.0f, 0.0f, 0.0f );
	D3DXMatrixScaling     ( &matScale,       1.0f, 1.0f, 1.0f );
	matObject = matScale * matTranslation;
	

	m_pD3D->SetTransform ( D3DTS_WORLD, &matObject );

	// Setup renderstates
	BeginRenderStates();

	// Render
	Q3ARender ( );

	// Restore renderstates
	EndRenderStates();
}

void Delete ( void )
{
	// MIKE FIX 190303 - call destructor to delete BSP data
	Destructor ( );
}

//
// Command Functions
//

bool LoadCore ( char* szFilename, char* szMap )
{
	// Attempt to load as BSP type direct from zip or local bsp file
	// Attempt to load BSP by using path to zip and then BSP filename
	for(DWORD pass=0; pass<2; pass++)
	{
		// Construct map filename
		char zipAttempt[_MAX_PATH];
		char mapAttempt[_MAX_PATH];
		strcpy(zipAttempt, szFilename);
		strcpy(mapAttempt, szMap);
		if(pass==1)
		{
			// get path from zipfilename and add it to map filename
			char path[_MAX_PATH];
			strcpy(path, "");
			for(DWORD p=strlen(zipAttempt); p>0; p--)
			{
				if(zipAttempt[p]=='\\' || zipAttempt[p]=='/')
				{
					zipAttempt[p+1]=0;
					strcpy(path, zipAttempt);
					strcpy(zipAttempt,"");
					break;
				}
			}
			strcpy(mapAttempt, path);
			strcat(mapAttempt, szMap);
		}

		InitShaders ( );
		if ( LoadQ3AMap ( zipAttempt, mapAttempt, TRUE )==TRUE )
		{
			// BSP Quake 3
			g_dwBSPType=3;
			return true;
		}
		else if(g_Q2BSP_Load ( zipAttempt, mapAttempt )==true)
		{
			// Load Quake1-2 BSP World
			g_dwBSPType=2;
			return true;
		}
		else if(g_OwnBSP_Load ( mapAttempt )==true)
		{
			// Load OwnBSP World
			g_dwBSPType=1;
			return true;
		}
	}
					
	// Load Failed
	g_dwBSPType=0;
	return false;
}

bool Load ( char* szFilename, char* szMap )
{
	
	// Uses actual or virtual file..
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, szFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	// Decrypt and use media, re-encrypt
	g_pGlob->Decrypt( (DWORD)VirtualFilename );
	bool bRes = LoadCore ( VirtualFilename, szMap );
	g_pGlob->Encrypt( (DWORD)VirtualFilename );

	return bRes;
	
	
	//bool bRes = LoadCore ( szFilename, szMap );
	//return true;
	
}

D3DXVECTOR3 ConvertVectorFromQ3 ( D3DXVECTOR3 src )
{
	//return src;

	src *= 0.029f;

	swap ( src.y, src.z );

	return src;
}

D3DXVECTOR3 ConvertVectorFromQ3 ( float src [ 3 ] )
{
	//return D3DXVECTOR3 ( src [ 0 ], src [ 1 ], src [ 2 ] );

	return ConvertVectorFromQ3 ( D3DXVECTOR3 ( src [ 0 ], src [ 1 ], src [ 2 ] ) );
}

D3DXVECTOR3 ConvertVectorFromQ3 ( int src [ 3 ] )
{
	//return D3DXVECTOR3 ( src [ 0 ], src [ 1 ], src [ 2 ] );

	return ConvertVectorFromQ3 ( D3DXVECTOR3 ( (float) src [ 0 ], (float) src [ 1 ], (float) src [ 2 ] ) );
}

D3DXVECTOR3 ConvertVectorFromQ3 ( short src [ 3 ] )
{
	//D3DXVECTOR3 ( src [ 0 ], src [ 1 ], src [ 2 ] );

	return ConvertVectorFromQ3 ( D3DXVECTOR3 ( src [ 0 ], src [ 1 ], src [ 2 ] ) );
}

D3DXVECTOR3 ConvertVectorToQ3 ( D3DXVECTOR3 src )
{
	//return src;

	src /= 0.029f;

	swap ( src.y, src.z );

	return src;
}

float ConvertVectorToQ3 ( float src )
{
	//return src;

	return src / 0.029f;
}

void swap ( float* x, float* y )
{
	float temp = *x;
	
	*x = *y;
	*y = temp;
}

void swap ( double* x, double* y )
{
	double temp = *x;
	
	*x = *y;
	*y = temp;
}

void swap ( int* x, int* y )
{
	int temp = *x;
	
	*x = *y;
	*y = temp;
}

void gamma_rgb ( byte* p, int size, float factor )
{
	for ( int i = 0; i < size / 3; i++, p += 3 )
	{
		float	r,
				g,
				b;
		float	scale = 1.0f;
		float	tmp;

        r = ( float ) p [ 0 ];
        g = ( float ) p [ 1 ];
        b = ( float ) p [ 2 ];

        r = r * factor / 255.0f;
        g = g * factor / 255.0f;
        b = b * factor / 255.0f;

        if ( r > 1.0f && ( tmp = ( 1.0f / r ) ) < scale )
			scale = tmp;

        if ( g > 1.0f && ( tmp = ( 1.0f / g ) ) < scale ) 
			scale = tmp;

        if ( b > 1.0f && ( tmp = ( 1.0f / b ) ) < scale ) 
			scale = tmp;

        scale *= 255.0f;
		r     *= scale;
        g     *= scale;
        b     *= scale;

        p [ 0 ] = ( byte ) r;
        p [ 1 ] = ( byte ) g;
        p [ 2 ] = ( byte ) b;
	}
}

BOOL LoadQ3AMap ( LPSTR filename )
{
	return LoadQ3AMap ( filename, "", TRUE );
}

BOOL LoadQ3AMap ( LPSTR filename, char* szMap, BOOL reset )
{
	byte* data;
	int length;
	if ( strnicmp ( filename + strlen ( filename ) - 4, ".pk3", 4 ) == 0 )
	{
		char pk3 [ _MAX_PATH ];
		strcpy ( pk3, filename );

		if ( !file_loader::Load::From_PK3 ( pk3, szMap, &data, &length ) )
			return FALSE;

		files_cache.AddFile ( szMap, szMap, pk3 );
	}
	else
	{
		// Not PK3, not Quake3..
		return FALSE;
	}
	/* confuses me - so can only load BSP through PK3 file
	   tried to load WAD file for Quake2 and this function founds another PK3 file in same folder as my WAD and started loading that!!
	else
	{
		FILE* file = fopen ( filename, "rb" );
		if ( file == NULL ) 
		{
			for ( int x = 0; x < Q3A_Resources.num_files; x++ )
			{
				LPSTR n = Q3A_Resources.files [ x ].fullname;

				if ( strnicmp ( n + strlen ( n ) - 4, ".pk3", 4 ) == 0 )
				{
					if ( file_loader::Load::From_PK3 ( n, filename, &data, &length ) )
						break;
				}
			}

			if ( x == Q3A_Resources.num_files )
				return FALSE;
		}
		else
		{
			fclose ( file );

			if ( !file_loader::Load::Direct ( filename, &data, &length ) )
				return FALSE;
		}

		files_cache.AddFile ( filename );
	}
	*/
	
	LPSTR path;
	path = NULL;
	SolveFullName ( filename, NULL, &path );

	files_found ff;

	file_loader::Find::Files ( "*.pk3", path, &ff );

	for ( int x = 0; x < ff.num_files; x++ )
		file_loader::Q3A::Add_PK3 ( ff.files [ x ].fullname );
	
	for ( int x = 0; x < ff.num_files; x++ )
	{
		AddShaders ( ff.files [ x ].fullname );
	}

	ff.Release ( );

	BOOL ok = InitMapQ3A ( data );
	
    if (data)
    {
        free(data);
        data = 0;
    }

	if ( ok )
	{
		externfiles.extmap |= EXT_QUAKE3;

		strcpy ( externfiles.extmap_name, "ikzdm1.bsp" );
	}
	else
	{
		externfiles.extmap ^= EXT_QUAKE3;
	}

	return TRUE;
}


#define HEADER_LUMPS    17
#define ENTITIES_LUMP   0
#define SHADERS_LUMP    1
#define PLANES_LUMP     2
#define NODES_LUMP      3
#define LEAVES_LUMP     4
#define LFACES_LUMP     5
#define LBRUSHES_LUMP   6
#define MODELS_LUMP     7
#define BRUSH_LUMP      8
#define BRUSHSIDES_LUMP 9
#define VERTICES_LUMP   10
#define ELEMENTS_LUMP   11
#define FOG_LUMP        12
#define FACES_LUMP      13
#define LIGHTMAPS_LUMP  14
#define LIGHTGRID_LUMP  15
#define VISIBILITY_LUMP 16

#define LIGHTMAP_BANKSIZE ( 128 * 128 * 3 )

struct lump3
{
	int fileofs,
		filelen;
};

struct header_t3
{
	char	id [ 4 ];
    int		version;
	lump3	lumps [ HEADER_LUMPS ];
} header3;


byte*      entities3;  int entity_size3;
plane_t3*  planes3;    int plane_count3;
model_t3*  models3;    int model_count3;
node_t3*   nodes3;     int node_count3;
leaf_t3*   leaves3;    int leaf_count3;
int*       lfaces3;    int lface_count3;
face_t3*   faces3;     int face_count3;
int*       elems3;     int elem_count3;
shader_t3* shaders3;   int shader_count3;
vertex_t3* vertices3;  int vertex_count3;
byte*      lightmaps3; int lightmap_count3;
vis_t3*    vis3;

// mike - 260604 //////////////////////////////////////
int					m_numOfLeafBrushes;
int					m_numOfBrushes;
int					m_numOfBrushSides;
int*				m_pLeafBrushes;
tBSPBrush*			m_pBrushes;
tBSPBrushSide*		m_pBrushSides;
///////////////////////////////////////////////////////

//struct _Q3MAP Q3MAP;
_Q3MAP Q3MAP;

int ReadLump2 ( byte* data, int lump, void** dest, int size )
{
	int length,
		ofs;

	length = header3.lumps [ lump ].filelen;
	ofs    = header3.lumps [ lump ].fileofs;

	*dest = new char [ length ];

	memcpy ( *dest, data + ofs, length );

	return length / size;
}

void DeleteMap ( void )
{
	SAFE_DELETE_ARRAY ( entities3 );
	SAFE_DELETE_ARRAY ( planes3 );
	SAFE_DELETE_ARRAY ( models3 );
	SAFE_DELETE_ARRAY ( nodes3 );
	SAFE_DELETE_ARRAY ( leaves3 );
	SAFE_DELETE_ARRAY ( lfaces3 );
	SAFE_DELETE_ARRAY ( faces3 );
	SAFE_DELETE_ARRAY ( elems3 );
	SAFE_DELETE_ARRAY ( shaders3 );
	SAFE_DELETE_ARRAY ( vertices3 );
	SAFE_DELETE_ARRAY ( lightmaps3 );
	SAFE_DELETE_ARRAY ( vis3 );
}

BOOL InitMapQ3A ( byte* data )
{
	if ( data [ 0 ] == 'I' && data [ 1 ] =='B' && data [ 2 ] == 'S' && data [ 3 ] == 'P' )
	{
		memcpy ( &header3, data, sizeof ( header3 ) );

		if ( header3.version == 46 || header3.version == 47 )
		{
		//	force:
				entity_size3    = ReadLump2 ( data, ENTITIES_LUMP,  ( void** ) &entities3, 1 );
				plane_count3    = ReadLump2 ( data, PLANES_LUMP,    ( void** ) &planes3,   sizeof ( plane_t3  ) );
				model_count3    = ReadLump2 ( data, MODELS_LUMP,    ( void** ) &models3,   sizeof ( model_t3  ) );
				node_count3     = ReadLump2 ( data, NODES_LUMP,     ( void** ) &nodes3,    sizeof ( node_t3   ) );
				leaf_count3     = ReadLump2 ( data, LEAVES_LUMP,    ( void** ) &leaves3,   sizeof ( leaf_t3   ) );
				lface_count3    = ReadLump2 ( data, LFACES_LUMP,    ( void** ) &lfaces3,   sizeof ( int       ) );
				face_count3     = ReadLump2 ( data, FACES_LUMP,     ( void** ) &faces3,    sizeof ( face_t3   ) );
				elem_count3     = ReadLump2 ( data, ELEMENTS_LUMP,  ( void** ) &elems3,    sizeof ( int       ) );
				shader_count3   = ReadLump2 ( data, SHADERS_LUMP,   ( void** ) &shaders3,  sizeof ( shader_t3 ) );
				vertex_count3   = ReadLump2 ( data, VERTICES_LUMP,  ( void** ) &vertices3, sizeof ( vertex_t3 ) );
				lightmap_count3 = ReadLump2 ( data, LIGHTMAPS_LUMP, ( void** ) &lightmaps3,LIGHTMAP_BANKSIZE    );
				int vis_count3  = ReadLump2 ( data, VISIBILITY_LUMP,( void** ) &vis3,      sizeof ( vis_t3    ) );

				
				// mike - 260604 //////////////////////////////////////
				m_numOfLeafBrushes  = ReadLump2 ( data, LBRUSHES_LUMP,( void** ) &m_pLeafBrushes,      sizeof ( int  ) );
				m_numOfBrushes      = ReadLump2 ( data, BRUSH_LUMP,   ( void** ) &m_pBrushes,          sizeof ( tBSPBrush  ) );
				m_numOfBrushSides   = ReadLump2 ( data, BRUSHSIDES_LUMP,   ( void** ) &m_pBrushSides,          sizeof ( tBSPBrushSide  ) );
				///////////////////////////////////////////////////////

				if ( vis_count3 > 0 )
				{
					Q3MAP.vis = TRUE;
				}
				else
				{
					Q3MAP.vis = FALSE;
				}
		}
		else
		{
			return FALSE;
		}

	}
	else
	{
		return FALSE;
	}

	// this call is very important - mark all shaders as unloaded
	shaders.ReInit ( );

	int lms [ 1000 ];

	for ( int i = 0; i < lightmap_count3; i++ )
	{
        byte* bits = lightmaps3 + ( LIGHTMAP_BANKSIZE * i );

		//gamma_rgb ( bits, LIGHTMAP_BANKSIZE, 3.5f );
		gamma_rgb ( bits, LIGHTMAP_BANKSIZE, 2.5f );

		// mike - 240604
		//gamma_rgb ( bits, LIGHTMAP_BANKSIZE, 5.5f );

		// mike - 230604
		//gamma_rgb ( bits, LIGHTMAP_BANKSIZE, 7.5f );
		

		lms [ i ] = Textures.Load_LightMap_By_Data_Q2 ( bits, 128, 128, 24 );
    }

	// fill face list and shader refs
	Q3MAP.faces = new _Q3MAP::face [ face_count3 ];

	for ( int i = 0; i < face_count3; i++ )
	{
		face_t3& f = faces3 [ i ];

	    Q3MAP.faces [ i ].type = f.type;

	    if ( f.lm_texture >= 0 && f.lm_texture < lightmap_count3 )
		{
			Q3MAP.faces [ i ].lmap = lms [ f.lm_texture ];
		}
		else
		{
			Q3MAP.faces [ i ].lmap = -1;
		}
	
		if ( f.type == FACETYPE_PATCH )
		{
			Q3MAP.faces [ i ].mesh = create_patch ( i );
		}
		else if ( f.type == FACETYPE_NORMAL )
		{
			Q3MAP.faces [ i ].vert_start = f.vert_start;
			Q3MAP.faces [ i ].vert_count = f.vert_count;
			Q3MAP.faces [ i ].org        = f.org;

			// precompute stuff for culling
			D3DXVECTOR3 o = f.org;
			D3DXVECTOR3 n = f.normal;
			Q3MAP.faces [ i ].dist   = D3DXVec3Dot ( &o, &n );
			Q3MAP.faces [ i ].normal = n;
		}
		else if ( f.type == FACETYPE_MESH )
		{
			Q3MAP.faces [ i ].vert_start = f.vert_start;
			Q3MAP.faces [ i ].vert_count = f.vert_count;
			Q3MAP.faces [ i ].elem_start = f.elem_start;
			Q3MAP.faces [ i ].elem_count = f.elem_count;
			Q3MAP.faces [ i ].org        = f.org;

			// precompute stuff for culling
			D3DXVECTOR3 o = f.org;
			D3DXVECTOR3 n = f.normal;
			Q3MAP.faces [ i ].dist   = D3DXVec3Dot ( &o, &n );
			Q3MAP.faces [ i ].normal = n;
		}
		else if ( f.type == FACETYPE_FLARE )
		{
			Q3MAP.faces [ i ].org = f.org;

			// precompute stuff for culling
			D3DXVECTOR3 o = f.org;
			D3DXVECTOR3 n = f.normal;
			Q3MAP.faces [ i ].dist   = D3DXVec3Dot ( &o, &n );
			Q3MAP.faces [ i ].normal = n;
		}
		
		// find shader & textures
		Q3MAP.faces [ i ].shader  = -1;
		Q3MAP.faces [ i ].tex_ref = -1;

		for ( int x = 0; x < shaders.entry_count; x++ )
		{
			shader* s = &shaders.entry [ x ];

			if ( stricmp ( s->name, shaders3 [ f.shader ].name ) == 0 )
			{
				Q3MAP.faces [ i ].shader  = x;
				Q3MAP.faces [ i ].tex_ref = -x - 2;  // faces are sorted by their tex_ref, so we give shaders a tex_ref (<-1)

				if ( !s->loaded )
				{
					files_cache.AddFile ( s->filename, s->filename, s->pk3name );

					// load shader stages
					for ( int st = 0; st < s->stage_count; st++ )
					{
						// load stage texture ( s )
						for ( int a = 0; a < s->stage [ st ].tex_count; a++ )
						{
							if ( stricmp ( s->stage [ st ].tex_name [ a ], "$lightmap" ) == 0 )
								continue;

							if ( stricmp ( s->stage [ st ].tex_name [ a ], "$whiteimage" ) == 0 ) 
								continue;
							
							// check if the texture is already loaded
							if ( s->stage [ st ].tex_ref [ a ] == -1 )
							{
								s->stage [ st ].tex_ref [ a ] = Textures.Load_By_FileName_Q3A ( s->stage [ st ].tex_name [ a ] );
							}
						}
					}
				
					s->loaded = TRUE;
				}
				break;
			}
		}

		// mot found in shader... maybe it's just a file
		if ( Q3MAP.faces [ i ].shader == -1 )
		{
			Q3MAP.faces [ i ].tex_ref = Textures.Load_By_FileName_Q3A ( shaders3 [ f.shader ].name );
		}

		if ( Q3MAP.faces [ i ].tex_ref == -1 )
		{
			int a = 0;
		}
	}

	// convert vertices
	Q3MAP.vert_count = vertex_count3;
	Q3MAP.verts      = new CUSTOMVERTEX [ Q3MAP.vert_count ];

	for ( int i = 0; i < vertex_count3; i++ )
	{
		Q3MAP.verts [ i ].v  = ConvertVectorFromQ3 ( vertices3 [ i ].point );

		// 250604
		//Q3MAP.verts [ i ].v  = vertices3 [ i ].point;
		Q3MAP.verts [ i ].n  = D3DXVECTOR3 ( vertices3  [ i ].normal [ 0 ], vertices3 [ i ].normal [ 2 ], vertices3 [ i ].normal [ 1 ] );
		Q3MAP.verts [ i ].tu = vertices3 [ i ].texture  [ 0 ];
		Q3MAP.verts [ i ].tv = vertices3 [ i ].texture  [ 1 ];
		Q3MAP.verts [ i ].lu = vertices3 [ i ].lightmap [ 0 ];
		Q3MAP.verts [ i ].lv = vertices3 [ i ].lightmap [ 1 ];
		
		D3DCOLOR col = vertices3 [ i ].color;

		byte r = RGBA_GETRED   ( col );
		byte g = RGBA_GETGREEN ( col );
		byte b = RGBA_GETBLUE  ( col );
		byte a = RGBA_GETALPHA ( col );

		Q3MAP.verts [ i ].color = D3DCOLOR_RGBA ( b, g, r, a );
	}

	// convert elems
	Q3MAP.elem_count = elem_count3;
	Q3MAP.elems      = new UINT16 [ Q3MAP.elem_count ];

	for ( int i = 0; i < elem_count3; i++ )
	{
		Q3MAP.elems [ i ] = ( UINT ) elems3 [ i ];
	}

	// sort faces by their texture / shader
	int* sort_face = new int [ face_count3 ];
	int* sort_tex  = new int [ face_count3 ];

    {
	    for ( int x = 0; x < face_count3; x++ )
	    {
		    sort_tex  [ x ] = Q3MAP.faces [ x ].tex_ref;
		    sort_face [ x ] = x;
	    }

	    // sort
	    for ( int x = 0; x < face_count3 - 1; x++ )
	    {
		    for ( int y = x + 1; y < face_count3; y++ )
		    {
			    if ( sort_tex [ x ] > sort_tex [ y ] )
			    {
				    swap ( &sort_tex  [ x ], &sort_tex  [ y ] );
				    swap ( &sort_face [ x ], &sort_face [ y ] );
			    }
		    }
	    }
    }

	// copy to Q3MAP data
	Q3MAP.sort.tex = NULL;

	int x = 0;
	int y = 0;

	while ( x < face_count3 )
	{
		Q3MAP.sort.tex = ( _Q3MAP::_sort::_tex* ) realloc ( Q3MAP.sort.tex, sizeof ( _Q3MAP::_sort::_tex ) * ( y + 1 ) );

		Q3MAP.sort.tex [ y ].face       = NULL;
		Q3MAP.sort.tex [ y ].face_count = 0;

		int tex = sort_tex [ x ];
		
		Q3MAP.sort.tex [ y ].tex_ref = tex;

		while ( sort_tex [ x ] == tex )
		{
			Q3MAP.sort.tex [ y ].face = ( int* ) realloc ( Q3MAP.sort.tex [ y ].face, sizeof ( int ) * ( Q3MAP.sort.tex [ y ].face_count + 1 ) );
			Q3MAP.sort.tex [ y ].face [ Q3MAP.sort.tex [ y ].face_count] = sort_face [ x ];
			Q3MAP.sort.tex [ y ].face_count++;
			x++;
		}

		y++;
	}
	
	Q3MAP.sort.tex_count = y;
	
	SAFE_DELETE ( sort_face );
	SAFE_DELETE ( sort_tex  );

	return TRUE;
}

D3DXPLANE frustum_planes3 [ 5 ];
int clip_count3;

bool frustum_cullf3 ( const float* bbox )
{
	D3DXVECTOR3 min = ConvertVectorFromQ3 ( bbox     );
	D3DXVECTOR3 max = ConvertVectorFromQ3 ( bbox + 3 );

    for ( int i = 4; i < 5; i++ )
	{
        D3DXPLANE p = frustum_planes3 [ i ];

        if ( p.a * min.x + p.b * min.y + p.c * min.z + p.d > 0 ) continue;
        if ( p.a * max.x + p.b * min.y + p.c * min.z + p.d > 0 ) continue;
        if ( p.a * min.x + p.b * max.y + p.c * min.z + p.d > 0 ) continue;
        if ( p.a * max.x + p.b * max.y + p.c * min.z + p.d > 0 ) continue;
        if ( p.a * min.x + p.b * min.y + p.c * max.z + p.d > 0 ) continue;
        if ( p.a * max.x + p.b * min.y + p.c * max.z + p.d > 0 ) continue;
        if ( p.a * min.x + p.b * max.y + p.c * max.z + p.d > 0 ) continue;
        if ( p.a * max.x + p.b * max.y + p.c * max.z + p.d > 0 ) continue;

        clip_count3++;
        return true;
	}

	return false;
}

bool frustum_cull3 ( const int* bbox )
{
	float bboxf [ 6 ];

	bboxf [ 0 ] = ( float ) bbox [ 0 ];
	bboxf [ 1 ] = ( float ) bbox [ 1 ];
	bboxf [ 2 ] = ( float ) bbox [ 2 ];
	bboxf [ 3 ] = ( float ) bbox [ 3 ];
	bboxf [ 4 ] = ( float ) bbox [ 4 ];
	bboxf [ 5 ] = ( float ) bbox [ 5 ];

	return frustum_cullf3 ( bboxf );
}

void frustum_setup3 ( )
{
	D3DMATRIX m;
	m_pD3D->GetTransform ( D3DTS_VIEW, &m );
	
	//  0  1  2  3 | _11 _12 _13 _14
	//  4  5  6  7 | _21 _22 _23 _24
	//  8  9 10 11 | _31 _32 _33 _34
	// 12 13 14 15 | _41 _42 _43 _44


    // right plane
	frustum_planes3 [ 0 ].a = m._14 - m._11;
    frustum_planes3 [ 0 ].b = m._24 - m._21;
    frustum_planes3 [ 0 ].c = m._34 - m._31;
    frustum_planes3 [ 0 ].d = m._44 - m._41;

    // left plane
	frustum_planes3 [ 1 ].a = m._14 + m._11;
    frustum_planes3 [ 1 ].b = m._24 + m._21;
    frustum_planes3 [ 1 ].c = m._34 + m._31;
    frustum_planes3 [ 1 ].d = m._44 + m._41;

    // bottom plane
    frustum_planes3 [ 2 ].a = m._14 + m._12;
    frustum_planes3 [ 2 ].b = m._24 + m._22;
    frustum_planes3 [ 2 ].c = m._34 + m._32;
    frustum_planes3 [ 2 ].d = m._44 + m._42;

    // top plane
    frustum_planes3 [ 3 ].a = m._14 - m._12;
    frustum_planes3 [ 3 ].b = m._24 - m._22;
    frustum_planes3 [ 3 ].c = m._34 - m._32;
    frustum_planes3 [ 3 ].d = m._44 - m._42;
    
	// near plane
	frustum_planes3 [ 4 ].a = m._14 + m._13;
    frustum_planes3 [ 4 ].b = m._24 + m._23;
    frustum_planes3 [ 4 ].c = m._34 + m._33;
    frustum_planes3 [ 4 ].d = m._44 + m._43;
}

int find_leaf3 ( D3DXVECTOR3 pos )
{
	int i = 0;

    while ( i >= 0 )
	{
        const node_t3&  n = nodes3  [ i ];
        const plane_t3& p = planes3 [ n.plane ];

        float d;
        
        d = p.normal [ 0 ] * pos.x + p.normal [ 1 ] * pos.y + p.normal [ 2 ] * pos.z - p.dist;

        if ( d >= 0 )
		{
			// in front or on the plane
            i = n.front;
        }
        else 
		{
			// behind the plane
            i = n.back;
        }
    }

    return ~i;
}

int find_node3 ( D3DXVECTOR3 o_pos, D3DXVECTOR3 n_pos, float BoundingSphere )
{
	int i = 0;

    while ( i >= 0 ) 
	{
		const node_t3&  n = nodes3  [ i ];
        const plane_t3& p = planes3 [ n.plane ];

        float	d1,
				d2;

        d1 = p.normal [ 0 ] * o_pos.x + p.normal [ 1 ] * o_pos.y + p.normal [ 2 ] * o_pos.z - p.dist;
		d2 = p.normal [ 0 ] * n_pos.x + p.normal [ 1 ] * n_pos.y + p.normal [ 2 ] * n_pos.z - p.dist;

        if ( d1 > BoundingSphere && d2 > BoundingSphere )
		{
			// in front or on the plane
			i = n.front;
        }
        else if ( d1 < -BoundingSphere && d2 < -BoundingSphere )
		{
			// behind the plane
            i = n.back;
        }
		else
		{
			break;
		}
    }

    return i;
}

void SetQ3ATextureStageStates ( int mode )
{

	switch ( mode )
	{
		case Q3A_MODE_LIGHTMAP:
		{
			// stage 1
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE     );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE     );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			
			// set texture mode
			m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

			// filter
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			
			// stage 2
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE     );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE   );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT     );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT     );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

			// set texture mode
			m_pD3D->SetSamplerState ( 1, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
			m_pD3D->SetSamplerState ( 1, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

			// filter
			m_pD3D->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		}	
		break;

		case Q3A_MODE_VERTEXLIGHTING:
		{
			// stage 1
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE     );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE   );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE     );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE     );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

			// set texture mode
			m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

			// filter
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

			// stage 2
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			// stage 3
			m_pD3D->SetTextureStageState ( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_pD3D->SetTextureStageState ( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		}
		break;
	}

	// common options
		
	// alphablending and lighting
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );

}

#define TESTVIS(from,to) (*(vis3->data + (from)*vis3->row_size + ((to)>>3)) & (1 << ((to) & 7)))

void Q3ARender ( )
{
	if ( !( externfiles.extmap & EXT_QUAKE3 ) )
		return;

	// MIKE FIX 190303 - make sure we have some valid data
	if ( Q3MAP.vert_count <= 0 )
		return;

	frustum_setup3 ( );

	// Render direct view or reflected stencil view
	g_dwCameraCullMode = D3DCULL_CCW;
	D3DXVECTOR3 viewpoint = D3DXVECTOR3 ( g_Camera_GetXPosition ( gCurrentCamera ), g_Camera_GetYPosition ( gCurrentCamera ), g_Camera_GetZPosition ( gCurrentCamera ) );
	if(g_pGlob)
	{
		if(g_pGlob->dwStencilMode==2)
		{
			if(g_pGlob->dwRedrawPhase==1)
			{
				// View is a reflection
				viewpoint.x = g_pGlob->fReflectionPlaneX;
				viewpoint.y = g_pGlob->fReflectionPlaneY;
				viewpoint.z = g_pGlob->fReflectionPlaneZ;				
				g_dwCameraCullMode = D3DCULL_CW;
			}
		}
	}

	// Convert from D3D to Q3V
	D3DXVECTOR3 pos  = ConvertVectorToQ3 ( viewpoint );
	// 250604
	//D3DXVECTOR3 pos  = viewpoint ;
	
	int idx         = find_leaf3 ( pos );
	int eye_cluster = leaves3 [ idx ].cluster;
	
	ZeroMemory ( &Q3MAP.mark_faces, sizeof ( BOOL ) * face_count3 );
	int rendered = 0;

	// find visible faces
	if ( eye_cluster >= 0 )
	{
		const leaf_t3*	leaves = leaves3;
		int				i	   = leaf_count3;

		while ( i-- )
		{
			const leaf_t3& leaf = *leaves++;

			if ( !Q3MAP.vis || TESTVIS ( eye_cluster,leaf.cluster ) )
			{
				if ( frustum_cull3 ( leaf.bbox ) )
					continue;
	
				const int*	p = lfaces3 + leaf.face_start;
				int			c = leaf.face_count;
				
				while ( c-- )
				{
	                int idx = *p++;
					
					// only if it's not marked yet
					if ( !Q3MAP.mark_faces [ idx ] )
					{
						// check flags
						const face_t3*	f = &faces3 [ idx ];
						int				s = Q3MAP.faces [ idx ].shader;
						
						if ( s >= 0 )
						{
							// Skip sky
							// mj comment sky
							// mike - 240604 - skip skies

							if ( shaders.entry [ s ].params.sky )
								continue;
						}

						if ( f->type == FACETYPE_PATCH )
						{
	                        if ( frustum_cullf3 ( f->bbox ) )
								continue;
						}
						else if ( f->type == FACETYPE_NORMAL )
						{
							// get face cull mode
							D3DCULL cull;

							if ( s >= 0 )
								cull = shaders.entry [ s ].params.cull;
							else 
								cull = D3DCULL_CCW;

							if ( cull != D3DCULL_NONE )
							{
								float d = D3DXVec3Dot ( &( pos - ( D3DXVECTOR3 ) vertices3 [ f->vert_start ].point ), &( D3DXVECTOR3 ) f->normal );

								// front culling
								if ( cull == D3DCULL_CW && d > 0 ) 
									continue;

								// back culling
								if ( cull == D3DCULL_CCW && d < 0 )
									continue;
							}
						}

						rendered++;
						Q3MAP.mark_faces [ idx ] = TRUE;
					}
				}
			}
		}

	}
	else
		memset ( &Q3MAP.mark_faces, 1, sizeof ( BOOL ) * face_count3 );

	m_pD3D->SetVertexShader ( NULL );
	m_pD3D->SetFVF ( D3DFVF_CUSTOMVERTEX );

	SetQ3ATextureStageStates ( Q3A_MODE_LIGHTMAP );
	//SetQ3ATextureStageStates ( Q3A_MODE_VERTEXLIGHTING );

	int x = Q3MAP.sort.tex_count;
	
	while ( x-- )
		Q3ARenderTextureFaces ( x );

	// release stages
	m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pD3D->SetTextureStageState ( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );

}

void SetupCameraCollision ( int iID, int iEntityID, float fRadius, int iResponse )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = CAMERA_COLLISION;
	gCollision [ iID ].fRadius = fRadius;
	gCollision [ iID ].iID     = iEntityID;
	gCollision [ iID ].iResponse = iResponse;
	gCollision [ iID ].bRespImpact = false;

	// Start at current position
	gCollisionSwitch [ iID ] = true;
	StartSingleCollisionIndex ( iID );
}

void SetCameraCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = CAMERA_COLLISION;
	gCollision [ iID ].fRadius = -1.0f;
	gCollision [ iID ].iID     = iEntity;
	gCollision [ iID ].vecRadius = D3DXVECTOR3 ( fX, fY, fZ );

	gCollisionSwitch [ iID ] = true;
	StartSingleCollisionIndex ( iID );
}

void SetObjectCollisionRadius ( int iID, int iEntity, float fX, float fY, float fZ )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = OBJECT_COLLISION;
	gCollision [ iID ].fRadius = -1.0f;
	gCollision [ iID ].iID     = iEntity;
	gCollision [ iID ].vecRadius = D3DXVECTOR3 ( fX, fY, fZ );

	gCollisionSwitch [ iID ] = true;
	StartSingleCollisionIndex ( iID );
}

void SetupObjectCollision ( int iID, int iEntityID, float fRadius, int iResponse )
{
	if ( bActivate )
	{
		memset ( gCollisionSwitch, 0, sizeof ( gCollisionSwitch ) );
		bActivate = false;
	}

	gCollision [ iID ].eType   = OBJECT_COLLISION;
	gCollision [ iID ].fRadius = fRadius;
	gCollision [ iID ].iID     = iEntityID;
	gCollision [ iID ].iResponse = iResponse;
	gCollision [ iID ].bRespImpact = false;

	// Start at current position
	gCollisionSwitch [ iID ] = true;
	StartSingleCollisionIndex ( iID );
}

void SetupCullingCamera ( int iID )
{
	gCurrentCamera=iID;
}

void SetupCollisionOff ( int iID )
{
	gCollisionSwitch [ iID ] = false;
}

extern float g_fCollisionThreshold;

void SetCollisionThreshhold ( int iID, float fSensitivity )
{
	// stop that slow sliding gravity decrement - if Ymod less than this senitivity value!
	//if ( fSensitivity < 0.003f )
	//	fSensitivity = 0.003f;

	//g_fCollisionThreshold = fSensitivity;

	// mike - 240604 - save threshold value
	gCollision [ iID ].fThreshold = fSensitivity;
}

void SetCollisionHeightAdjustment ( int iID, float fHeight )
{
	gCollision [ iID ].fAdjustY = fHeight;
	StartSingleCollisionIndex ( iID );
}

int GetCollisionResponse ( int iID )
{
	if ( gCollision [ iID ].bRespImpact )
	{
		gCollision [ iID ].bRespImpact=false;
		return 1;
	}
	else
		return 0;
}

float GetCollisionX ( int iID )
{
	return gCollision [ iID ].vecResp.x;
}

float GetCollisionY ( int iID )
{
	return gCollision [ iID ].vecResp.y;
}

float GetCollisionZ ( int iID )
{
	return gCollision [ iID ].vecResp.z;
}


//
// Commands
//

void LoadEx ( SDK_LPSTR szFilename, SDK_LPSTR szMap )
{
	// Load BSP and determine BSPType
	bool bLoadOk=false;
	if( Load ( (char*)szFilename, (char*)szMap ))
	{
		// Load okay
		bLoadOk=true;

		// Ensure interfaces present for BSP usage
		switch(g_dwBSPType)
		{
			case 1 : if(g_OwnBSP_Update==NULL)	bLoadOk=false;
			case 2 : if(g_Q2BSP_Update==NULL)	bLoadOk=false;
		}
	}
	if(bLoadOk==false)
	{
		RunTimeError(RUNTIMEERROR_BSPLOADFAILED);
	}
	else
	{
		// To ensure good lighting by default - autoset ambient to 100
		// leefix-040803-undocumented assumptions like this are difficult for users
		// so fixed as a bug and DARK BSPs can be resolved by asetting ambience manually
//		m_pD3D->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_ARGB(255,255,255,255) );
	}
}

void DeleteEx ( void )
{
	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_Delete(); return;
		case 2 : g_Q2BSP_Delete(); return;
		case 3 : Delete(); return;
	}
}

struct VERTEX2D
{
	float	x;
	float	y;
	float	z;

	float	rhw;

	DWORD	color;

	float	tu;
	float	tv;
};

#define D3DFVF_VERTEX2D ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

void UpdateEx ( void )
{
	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_Update(); return;
		case 2 : g_Q2BSP_Update(); return;
		case 3 : Update(); return;
	}
}

void StartEx ( void )
{
	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_Start(); return;
		case 2 : g_Q2BSP_Start(); return;
		case 3 : Start(); return;
	}
}

void EndEx ( void )
{
	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_End(); return;
		case 2 : g_Q2BSP_End(); return;
		case 3 : End(); return;
	}
}

void SetCameraCollisionRadiusEx ( int iID, int iEntityID, float fX, float fY, float fZ )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return;
	}

	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_SetCameraCollisionRadius(iID, iEntityID, fX, fY, fZ); return;
		case 2 : g_Q2BSP_SetCameraCollisionRadius(iID, iEntityID, fX, fY, fZ); return;
		case 3 : SetCameraCollisionRadius(iID, iEntityID, fX, fY, fZ); return;
	}
}

void SetObjectCollisionRadiusEx ( int iID, int iEntityID, float fX, float fY, float fZ )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return;
	}

	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_SetObjectCollisionRadius(iID, iEntityID, fX, fY, fZ); return;
		case 2 : g_Q2BSP_SetObjectCollisionRadius(iID, iEntityID, fX, fY, fZ); return;
		case 3 : SetObjectCollisionRadius(iID, iEntityID, fX, fY, fZ); return;
	}
}

void SetHardwareMultiTexturingOnEx ( void )
{
	switch(g_dwBSPType)
	{
		//case 1 : g_OwnBSP_SetupCameraCollision(iID, iEntityID, fRadius, iResponse); return;
		case 2 : g_Q2BSP_SetHardwareMultiTexturingOn( ); return;
		//case 3 : SetupCameraCollision(iID, iEntityID, fRadius, iResponse); return;
	}
}

void SetHardwareMultiTexturingOffEx ( void )
{
	switch(g_dwBSPType)
	{
		//case 1 : g_OwnBSP_SetupCameraCollision(iID, iEntityID, fRadius, iResponse); return;
		case 2 : g_Q2BSP_SetHardwareMultiTexturingOff( ); return;
		//case 3 : SetupCameraCollision(iID, iEntityID, fRadius, iResponse); return;
	}
}

void SetupCameraCollisionEx ( int iID, int iEntityID, float fRadius, int iResponse )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return;
	}

	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_SetupCameraCollision(iID, iEntityID, fRadius, iResponse); return;
		case 2 : g_Q2BSP_SetupCameraCollision(iID, iEntityID, fRadius, iResponse); return;
		case 3 : SetupCameraCollision(iID, iEntityID, fRadius, iResponse); return;
	}
}

void SetupObjectCollisionEx ( int iID, int iEntityID, float fRadius, int iResponse )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return;
	}

	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_SetupObjectCollision(iID, iEntityID, fRadius, iResponse); return;
		case 2 : g_Q2BSP_SetupObjectCollision(iID, iEntityID, fRadius, iResponse); return;
		case 3 : SetupObjectCollision(iID, iEntityID, fRadius, iResponse); return;
	}
}

void SetupCullingCameraEx ( int iID )
{
	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_SetupCullingCamera(iID); return;
		case 2 : g_Q2BSP_SetupCullingCamera(iID); return;
		case 3 : SetupCullingCamera(iID); return;
	}
}

void SetupCollisionOffEx ( int iID )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return;
	}

	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_SetupCollisionOff(iID); return;
		case 2 : g_Q2BSP_SetupCollisionOff(iID); return;
		case 3 : SetupCollisionOff(iID); return;
	}
}

void SetCollisionThreshholdEx ( int iID, float fSensitivity )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return;
	}

	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_SetCollisionThreshhold(iID,fSensitivity); return;
		case 2 : g_Q2BSP_SetCollisionThreshhold(iID,fSensitivity); return;
		case 3 : SetCollisionThreshhold(iID,fSensitivity); return;
	}
}

void SetCollisionHeightAdjustmentEx ( int iID, float fHeight )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return;
	}

	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_SetCollisionHeightAdjustment(iID,fHeight); return;
		case 2 : g_Q2BSP_SetCollisionHeightAdjustment(iID,fHeight); return;
		case 3 : SetCollisionHeightAdjustment(iID,fHeight); return;
	}
}

void ProcessCollisionEx ( int iID )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return;
	}

	switch(g_dwBSPType)
	{
		case 1 : g_OwnBSP_ProcessCollision(iID); return;
//		case 2 : g_OwnBSP_ProcessCollision(iID); return;
		case 3 : ProcessCollision(iID); return;
	}
}

/*
void CompileBSPEx ( SDK_LPSTR szInput, SDK_LPSTR szOutput, int iFastVis, SDK_FLOAT fEpsilon, int iSplits )
{
	if(g_BSPCompiler_CompileBSP)
	{
		g_BSPCompiler_CompileBSP ( (char*)szInput, (char*)szOutput, iFastVis, fEpsilon, iSplits );
	}
}
*/

//
// Expression Functions
//

int GetCollisionResponseEx ( int iID )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return 0;
	}
	switch(g_dwBSPType)
	{
		case 1 : return g_OwnBSP_GetCollisionResponse(iID);


//		case 2 : return GetCollisionResponse(iID);

		case 2 : return g_Q2BSP_GetCollisionResponse(iID);


		case 3 : return GetCollisionResponse(iID);
	}
	return 0;
}

DWORD GetCollisionXEx ( int iID )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return 0;
	}
	float fResult=0.0f;
	switch(g_dwBSPType)
	{
		case 1 : fResult=g_OwnBSP_GetCollisionX(iID); break;


//		case 2 : fResult=GetCollisionX(iID); break;
		case 3 : fResult=GetCollisionX(iID); break;
	}
	return *(DWORD*)&fResult;
}

DWORD GetCollisionYEx ( int iID )
{

	// ADD IN RETURN VALUES FOR HL MAPS

	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return 0;
	}
	float fResult=0.0f;
	switch(g_dwBSPType)
	{
		case 1 : fResult=g_OwnBSP_GetCollisionY(iID); break;
//		case 2 : fResult=GetCollisionY(iID); break;
		case 3 : fResult=GetCollisionY(iID); break;
	}
	return *(DWORD*)&fResult;
}

DWORD GetCollisionZEx ( int iID )
{
	if ( iID < 1 || iID > 24 )
	{
		RunTimeError(RUNTIMEERROR_BSPCOLLISIONNUMBERILLEGAL);
		return 0;
	}
	float fResult=0.0f;
	switch(g_dwBSPType)
	{
		case 1 : fResult=g_OwnBSP_GetCollisionZ(iID); break;
//		case 2 : fResult=GetCollisionZ(iID); break;
		case 3 : fResult=GetCollisionZ(iID); break;
	}
	return *(DWORD*)&fResult;
}

//////////////////////////////////////////////////////////////////////////////////
// DARK SDK SECTION //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#ifdef DARKSDK_COMPILE

void ConstructorWorld ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hCamera, HINSTANCE hObject )
{
	Constructor ( hSetup, hImage, hCamera, hObject );
}

void DestructorWorld ( void )
{
	Destructor ( );
}

void SetErrorHandlerWorld ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataWorld( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DWorld ( int iMode )
{
	RefreshD3D ( iMode );
}

void dbUpdateBSP ( void )
{
	Update ( );
}

void dbDeleteBSP ( void )
{
	Delete ( );
}

void dbStartBSP ( void )
{
	Start ( );
}

void dbEndBSP ( void )
{
	End ( );
}


void dbLoadBSP ( char* szFilename, char* szMap )
{
	LoadEx ( ( DWORD ) szFilename, ( DWORD ) szMap );
}

void dbSetBSPCamera ( int iID )
{
	SetupCullingCameraEx ( iID );
}

void dbSetBSPCameraCollision ( int iID, int iEntityID, float fRadius, int iResponse )
{
	SetupCameraCollision (  iID,  iEntityID,  fRadius,  iResponse );
}

void dbSetBSPObjectCollision ( int iID, int iEntityID, float fRadius, int iResponse )
{
	SetupObjectCollision (  iID, iEntityID,  fRadius,  iResponse );
}

void dbSetBSPCollisionThreshhold ( int iID, float fSensitivity )
{
	SetCollisionThreshholdEx ( iID, fSensitivity );
}

void dbSetBSPCollisionOff ( int iID )
{
	SetupCollisionOffEx ( iID );
}

void dbSetBSPCameraCollisionRadius ( int iID, int iEntityID, float fX, float fY, float fZ )
{
	SetCameraCollisionRadiusEx ( iID, iEntityID,  fX,  fY,  fZ );
}

void dbSetBSPObjectCollisionRadius ( int iID, int iEntityID, float fX, float fY, float fZ )
{
	SetObjectCollisionRadiusEx (  iID,  iEntityID, fX, fY, fZ );
}

void dbSetBSPCollisionHeightAdjustment ( int iID, float fHeight )
{
	SetCollisionHeightAdjustmentEx	( iID, fHeight );
}

void dbSetBSPMultiTexturingOn ( void )
{
	SetHardwareMultiTexturingOnEx ( );
}

void dbSetBSPMultiTexturingOff ( void )
{
	SetHardwareMultiTexturingOffEx ( );
}

void dbProcessBSPCollision ( int iID )
{
	ProcessCollisionEx ( iID );
}

int dbBSPCollisionHit ( int iID )
{
	return GetCollisionResponseEx ( iID );
}

float dbBSPCollisionX ( int iID )
{
	DWORD dwReturn = GetCollisionXEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbBSPCollisionY ( int iID )
{
	DWORD dwReturn = GetCollisionYEx ( iID );
	
	return *( float* ) &dwReturn;
}

float dbBSPCollisionZ ( int iID )
{
	DWORD dwReturn = GetCollisionZEx ( iID );
	
	return *( float* ) &dwReturn;
}

// lee - 300706 - GDK fixes
void dbSetBSPCollisionThreshold ( int iID, float fSensitivity ) { dbSetBSPCollisionThreshhold ( iID, fSensitivity ); }

#endif

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
