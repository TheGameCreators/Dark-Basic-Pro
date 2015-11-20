
//
// Common/Private Functions Implementation
//

//#define _CRT_SECURE_NO_DEPRECATE

#include "CommonC.h"
#include ".\..\Core\SteamCheckForWorkshop.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKCamera.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKImage.h"
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDK3DMaths.h"
#endif

//////////////////////////////////////////////////////////////////////////////////
// GLOBAL DATA TO COMMON FUNCTIONS ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// MM Globals
#include ".\..\MemoryManager\DarkMemoryManager.h"
char g_MM_DLLName [ 256 ] = { "Basic3D" };
char g_MM_FunctionName [ 256 ] = { "<none>" };

// Object Manager and Renderer
DBPRO_GLOBAL CObjectManager					m_ObjectManager;
DBPRO_GLOBAL int*							g_ObjectListRef					= NULL;				// short list of valid object indexes
DBPRO_GLOBAL int							g_iObjectListRefCount			= 0;
//DBPRO_GLOBAL int							g_iObjectListRefLastCount		= 0;
DBPRO_GLOBAL sObject**						g_ObjectList					= NULL;				// a list of pointers to pointers - we're going to create a dynamic array
DBPRO_GLOBAL int							g_iObjectListCount				= 0;				// number of objects currently in list
DBPRO_GLOBAL bool							g_bGlobalVBIBUsageFlag			= true;				// true means objects created use their own VB buffers
DBPRO_GLOBAL bool							g_bGlobalSortByTextureUsageFlag	= true;				// true means will sort by texture (preU71 behaviour)
DBPRO_GLOBAL bool							g_bGlobalSortByObjectNumberFlag = false;			// true means will sort by objectnumber if not sorted by texture
DBPRO_GLOBAL bool							g_bGlobalSortByObjectDepthFlag  = false;			// true means sort by depth DURING RESORT, NOT PER CYCLE (as done for transparent objects automatically)!!
DBPRO_GLOBAL eSortOrder                     g_eGlobalSortOrder              = E_SORT_BY_TEXTURE; // E_SORT_BY_TEXTURE or E_SORT_BY_NONE

// Mesh Manager Structures
DBPRO_GLOBAL sMesh**						g_RawMeshList					= NULL;				// a list of pointers to pointers
DBPRO_GLOBAL int							g_iRawMeshListCount				= 0;				// number of meshes currently in list
DBPRO_GLOBAL PTR_FuncCreateStr				g_pCreateDeleteStringFunction	= NULL;

// Globstruct
#ifdef DARKSDK_COMPILE
	static GlobStruct*						g_pGlob							= NULL;				//defined elsewher
#else
	DBPRO_GLOBAL GlobStruct*				g_pGlob							= NULL;				//defined elsewher
#endif

DBPRO_GLOBAL char							m_pWorkString[_MAX_PATH];
DBPRO_GLOBAL bool							g_bCreateChecklistNow;
DBPRO_GLOBAL DWORD							g_dwMaxStringSizeInEnum;

// Vertex and Pixel Shader Globals
DBPRO_GLOBAL sVertexShader					m_VertexShaders [ MAX_VERTEX_SHADERS ];
DBPRO_GLOBAL sPixelShader					m_PixelShaders  [ MAX_VERTEX_SHADERS ];
DBPRO_GLOBAL sEffectItem*					m_EffectList    [ MAX_EFFECTS ];

// Standard datatype to hold full collision details ( collision, etc)
DBPRO_GLOBAL sFullCollisionResult			g_DBPROCollisionResult;
DBPRO_GLOBAL sFullPickResult				g_DBPROPickResult;

// Shadow System globals
DBPRO_GLOBAL D3DXVECTOR3					g_vecShadowPosition;
DBPRO_GLOBAL int							g_iShadowPositionFixed = 0;

// Universe and Scene Globals
DBPRO_GLOBAL cLightMaps						g_LightMaps;
DBPRO_GLOBAL cUniverse*						g_pUniverse						= NULL;
DBPRO_GLOBAL int							g_iAreaBox						= 0;
DBPRO_GLOBAL int							g_iAreaBoxCount					= 0;

DBPRO_GLOBAL HINSTANCE						g_XObject    = NULL;
DBPRO_GLOBAL HINSTANCE						g_3DSObject  = NULL;
DBPRO_GLOBAL HINSTANCE						g_MDLObject  = NULL;
DBPRO_GLOBAL HINSTANCE						g_MD2Object  = NULL;
DBPRO_GLOBAL HINSTANCE						g_MD3Object  = NULL;
DBPRO_GLOBAL HINSTANCE						g_PrimObject = NULL;
static HINSTANCE							g_Image      = NULL;
DBPRO_GLOBAL HINSTANCE						g_Setup      = NULL;

// 181113 - Instance Stamp Bound Box Renderer
IDirect3DVertexBuffer9*						g_pStampBufferVBRef				= NULL;
DWORD										g_dwStampBufferFVFSize			= 0;
#define D3DFVF_BOUNDBOXVERTEX ( D3DFVF_XYZ )
struct BOUNDBOXVERTEX
{
	D3DXVECTOR3 p;
};
sMesh*										g_pQUADMesh						= NULL;


/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// OTHER 'THINGS' ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

DBPRO_GLOBAL RetGetMeshDataPFN				g_GetMeshData;
DBPRO_GLOBAL RetGetTextureDataPFN			g_GetTextureData;
DBPRO_GLOBAL RetGetNumMaterialsPFN			g_GetNumMaterials;
DBPRO_GLOBAL RetGetMaterialDataPFN			g_GetMaterialData;
DBPRO_GLOBAL GetMatrixDataPFN				g_GetCombined;
DBPRO_GLOBAL GetMatrixDataPFN				g_GetTransformed;
DBPRO_GLOBAL GetMatrixDataPFN				g_GetOriginal;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		UpdateTimer;
DBPRO_GLOBAL GetMatrixPFN					g_Types_GetMatrix;
DBPRO_GLOBAL GetVectorPFN					g_Types_GetVector;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_LoadMesh;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_DeleteMesh;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_SaveMesh;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_ChangeMesh;
DBPRO_GLOBAL RetVoidFunctionPointerPFN		g_MakeMeshFromObject;
DBPRO_GLOBAL RetIntFunctionPointerPFN		g_GetMeshExist;
DBPRO_GLOBAL HINSTANCE						g_GFX;
DBPRO_GLOBAL GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;
DBPRO_GLOBAL HINSTANCE						m_hData = NULL;
DBPRO_GLOBAL LPDIRECT3DDEVICE9				m_pD3D = NULL;
DBPRO_GLOBAL D3DCAPS9						m_Caps;
DBPRO_GLOBAL CAMERA3D_SetFOVPFN				g_Camera3D_SetFOV;

DBPRO_GLOBAL MEMBLOCKS_GetMemblockExist		g_Memblock_GetMemblockExist;
DBPRO_GLOBAL MEMBLOCKS_GetMemblockPtr		g_Memblock_GetMemblockPtr;
DBPRO_GLOBAL MEMBLOCKS_GetMemblockSize		g_Memblock_GetMemblockSize;
DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia	g_Memblock_MemblockFromImage;
DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia	g_Memblock_MemblockFromBitmap;
DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia	g_Memblock_MemblockFromSound;
DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia	g_Memblock_MemblockFromMesh;
DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock	g_Memblock_ImageFromMemblock;
DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock	g_Memblock_BitmapFromMemblock;
DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock	g_Memblock_SoundFromMemblock;
DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock	g_Memblock_MeshFromMemblock;
DBPRO_GLOBAL MEMBLOCKS_MakeMemblock			g_Memblock_MakeMemblock;
DBPRO_GLOBAL MEMBLOCKS_DeleteMemblock		g_Memblock_DeleteMemblock;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/* now using MM
DWORD g_dwMemoryConsumed = 0;
void* operator new (size_t size)
{
 void *p=malloc(size);
 if (p==0) // did malloc succeed?
  throw std::bad_alloc(); // ANSI/ISO compliant behavior
 g_dwMemoryConsumed+=size;
 return p;
}
void operator delete (void *p)
{
 if(p)
 {
  g_dwMemoryConsumed-=_msize(p);
  free(p); 
 }
}
*/


//////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR FUNCTIONS /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL void ConstructorD3D ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hTypes, HINSTANCE hCamera )
{
	#ifndef DARKSDK_COMPILE
	if ( !hSetup || !hImage )
	{
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );		// load the setup library
		hImage = LoadLibrary ( "DBProImageDebug.dll" );		// load the image library
	}
	#endif

	g_Image  = hImage;
	g_Setup  = hSetup;
	m_hData  = hTypes;
	g_Camera = hCamera;

	#ifndef DARKSDK_COMPILE
		g_Camera_Position              = ( CAMERA_Int1Float3PFN )				GetProcAddress ( hCamera, "?Position@@YAXHMMM@Z" );
		g_Camera_GetXPosition		= ( CAMERA_GetFloatPFN )					GetProcAddress ( hCamera, "?GetXPosition@@YAMH@Z" );
		g_Camera_GetYPosition		= ( CAMERA_GetFloatPFN )					GetProcAddress ( hCamera, "?GetYPosition@@YAMH@Z" );
		g_Camera_GetZPosition		= ( CAMERA_GetFloatPFN )					GetProcAddress ( hCamera, "?GetZPosition@@YAMH@Z" );

		g_Image_Constructor         = ( IMAGE_RetVoidParamHINSTANCEPFN )		GetProcAddress ( hImage, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
		g_Image_Destructor          = ( IMAGE_RetVoidParamVoidPFN )				GetProcAddress ( hImage, "?Destructor@@YAXXZ" );

		g_Image_InternalLoad        = ( IMAGE_LoadInternalTexturePFN )			GetProcAddress ( hImage, "?LoadInternal@@YAHPAD@Z" );
		g_Image_InternalLoadEx      = ( IMAGE_LoadInternalTextureDividePFN )	GetProcAddress ( hImage, "?LoadInternal@@YAHPADH@Z" );

		g_Image_Load	            = ( IMAGE_RetBoolParamIntPCharPFN )			GetProcAddress ( hImage, "?Load@@YA_NHPAD@Z" );
		g_Image_Save	            = ( IMAGE_RetBoolParamIntPCharPFN )			GetProcAddress ( hImage, "?Save@@YA_NHPAD@Z" );
		g_Image_Delete	            = ( IMAGE_RetVoidParamIntPFN )				GetProcAddress ( hImage, "?Delete@@YAXH@Z" );
		g_Image_Make                = ( IMAGE_RetLPD3DTEX9ParamInt3PFN )		GetProcAddress ( hImage, "?Make@@YAPAUIDirect3DTexture9@@HHH@Z" );
		g_Image_MakeFormat          = ( IMAGE_RetLPD3DTEX9ParamInt4PFN )		GetProcAddress ( hImage, "?MakeJustFormat@@YAPAUIDirect3DTexture9@@HHHW4_D3DFORMAT@@@Z" );

		g_Image_GetPointer          = ( IMAGE_RetLPD3DTEX9ParamIntPFN )			GetProcAddress ( hImage, "?GetPointer@@YAPAUIDirect3DTexture9@@H@Z" );
		g_Image_GetWidth            = ( IMAGE_RetIntParamIntPFN )				GetProcAddress ( hImage, "?GetWidth@@YAHH@Z" );
		g_Image_GetHeight           = ( IMAGE_RetIntParamIntPFN )				GetProcAddress ( hImage, "?GetHeight@@YAHH@Z" );
		g_Image_GetExist            = ( IMAGE_RetBoolParamIntPFN )				GetProcAddress ( hImage, "?GetExist@@YA_NH@Z" );
		g_Image_GetName             = ( IMAGE_RetLPSTRParamIntPFN )				GetProcAddress ( hImage, "?GetName@@YAPADH@Z" );

		g_Image_SetSharing          = ( IMAGE_RetVoidParamBoolPFN )				GetProcAddress ( hImage, "?SetSharing@@YAX_N@Z" );
		g_Image_SetMemory           = ( IMAGE_RetVoidParamIntPFN )				GetProcAddress ( hImage, "?SetMemory@@YAXH@Z" );

		g_Image_Lock	            = ( IMAGE_RetVoidParamIntPFN )				GetProcAddress ( hImage, "?Lock@@YAXH@Z" );
		g_Image_Unlock	            = ( IMAGE_RetVoidParamIntPFN )				GetProcAddress ( hImage, "?Unlock@@YAXH@Z" );
		g_Image_Write	            = ( IMAGE_RetVoidParamInt7PFN )				GetProcAddress ( hImage, "?Write@@YAXHHHHHHH@Z" );

		g_Image_SetMipmapMode       = ( IMAGE_RetVoidParamBoolPFN )				GetProcAddress ( hImage, "?SetMipmapMode@@YAX_N@Z" );
		g_Image_SetMipmapType       = ( IMAGE_RetVoidParamIntPFN )				GetProcAddress ( hImage, "?SetMipmapType@@YAXH@Z" );
		g_Image_SetMipmapBias       = ( IMAGE_RetVoidParamFloatPFN )			GetProcAddress ( hImage, "?SetMipmapBias@@YAXM@Z" );
		g_Image_SetMipmapNum        = ( IMAGE_RetVoidParamIntPFN )				GetProcAddress ( hImage, "?SetMipmapNum@@YAXH@Z" );

		g_Image_SetColorKey         = ( IMAGE_RetVoidParamInt4PFN )				GetProcAddress ( hImage, "?SetColorKey@@YAXHHH@Z" );
		g_Image_SetTranslucency     = ( IMAGE_RetVoidParamInt2PFN )				GetProcAddress ( hImage, "?SetTranslucency@@YAXHH@Z" );

		g_Image_SetCubeFace			= ( IMAGE_RetVoidParamICUBEIPFN )			GetProcAddress ( hImage, "?SetCubeFace@@YAXHPAUIDirect3DCubeTexture9@@H@Z" );

		g_GFX_GetDirect3DDevice		= ( GFX_GetDirect3DDevicePFN )				GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
		
		g_Types_GetMatrix			= ( GetMatrixPFN )							GetProcAddress ( m_hData, "?GetMatrix@@YA?AUD3DXMATRIX@@H@Z" );
		g_Types_GetVector			= ( GetVectorPFN )							GetProcAddress ( m_hData, "?GetVector4@@YA?AUD3DXVECTOR4@@H@Z" );

	#else
		
		g_Camera_Position           = dbPositionCamera;
		g_Camera_GetXPosition		= dbCameraPositionX;
		g_Camera_GetYPosition		= dbCameraPositionY;
		g_Camera_GetZPosition		= dbCameraPositionZ;

		g_Image_Constructor         = ConstructorImage;
		g_Image_Destructor          = DestructorImage;

		g_Image_InternalLoad        = dbLoadImageInternal;
		g_Image_InternalLoadEx      = dbLoadImageInternal;

		//g_Image_Load	            = dbLoadImageEx;
		//g_Image_Save	            = dbSaveImage;
		g_Image_Delete	            = dbDeleteImage;
		g_Image_Make                = dbMakeImage;
		//g_Image_MakeFormat          = dbMakeImageJustFormat;
		
		g_Image_GetPointer          = dbGetImagePointer;
		g_Image_GetWidth            = dbGetImageWidth;
		g_Image_GetHeight           = dbGetImageHeight;
		//g_Image_GetExist            = dbImageExist;
		g_Image_GetName             = dbGetImageName;

		g_Image_SetSharing          = dbSetImageSharing;
		g_Image_SetMemory           = dbSetImageMemory;

		g_Image_Lock	            = dbLockImage;
		g_Image_Unlock	            = dbUnlockImage;
		g_Image_Write	            = dbWriteImage;

		g_Image_SetMipmapMode       = dbSetImageMipmapMode;
		g_Image_SetMipmapType       = dbSetImageMipmapType;
		g_Image_SetMipmapBias       = dbSetImageMipmapBias;
		g_Image_SetMipmapNum        = dbSetImageMipmapNum;

		//g_Image_SetColorKey         = dbSetImageColorKey;
		//g_Image_SetTranslucency     = dbSetTranslucency;

		g_Image_SetCubeFace         = dbSetImageCubeFace;

		g_GFX_GetDirect3DDevice		= dbGetDirect3DDevice;
		
		g_Types_GetMatrix			= dbGetMatrix;
		g_Types_GetVector			= dbGetVector4;
	#endif

	m_pD3D                  = g_GFX_GetDirect3DDevice ( );

	m_pD3D->GetDeviceCaps ( &m_Caps );

	// FROM MATRIX3D DLL (maybe this should be in CAMERA ONLY)
	// NEED a common D3D Scene Default Setup Function (that all can call)
	//maybe each dll has its own setup function call...
	// FOG Default
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, 0 );
	m_pD3D->SetRenderState ( D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR );
	m_pD3D->SetRenderState ( D3DRS_FOGSTART, 0 );
	float fFullOne=50000.0f; m_pD3D->SetRenderState ( D3DRS_FOGEND, *(DWORD*)&fFullOne );

	// LIGHT Default 
	// lee - 230306 - u6b4 - these settings would be overritten with LIGHT-DLL direction mode setting default (now copied these settings to LightDLL=no specular by default)
	D3DLIGHT9 DefaultLight;
	memset ( &DefaultLight, 0, sizeof ( D3DLIGHT9 ) );
	DefaultLight.Type         = D3DLIGHT_DIRECTIONAL;						// set to point light
	DefaultLight.Diffuse.r    = 1.0f;								// diffuse colour to reflect all
	DefaultLight.Diffuse.g    = 1.0f;								// diffuse colour to reflect all
	DefaultLight.Diffuse.b    = 1.0f;								// diffuse colour to reflect all
	DefaultLight.Direction    = D3DXVECTOR3 ( 0.1f, -0.7f, 0.2f );	// original light default
	DefaultLight.Range        = 5000.0f;							// light range
	DefaultLight.Attenuation0 = 1.0f;								// light intensity over distance
	m_pD3D->SetLight    ( 0, &DefaultLight );
	m_pD3D->LightEnable ( 0, true );

	// NODETREE Universe Default
	g_pUniverse = new cUniverse;
	g_pUniverse->Make ( 3000, 3000, 3000 );

	// Clear Full Collision Data
	memset ( &g_DBPROCollisionResult, 0, sizeof(g_DBPROCollisionResult) );

	// Delete any data created from an effect error
	SAFE_DELETE(g_pEffectErrorMsg);

	// Create a vertex buffer so I can render arbitary boundboxes (instance stamp system)
	g_dwStampBufferFVFSize = sizeof ( BOUNDBOXVERTEX ); 
	if(g_pStampBufferVBRef==NULL)
	{
		m_pD3D->CreateVertexBuffer ( 12 * 3 * g_dwStampBufferFVFSize, D3DUSAGE_WRITEONLY, D3DFVF_BOUNDBOXVERTEX, D3DPOOL_MANAGED, &g_pStampBufferVBRef, NULL );
   	}
	if(g_pStampBufferVBRef)
	{
		// fill vertex buffer with box
		BOUNDBOXVERTEX*	v  = NULL;
		g_pStampBufferVBRef->Lock ( 0, 0, ( VOID** ) &v, 0 );
		// front
		int pi = 0;
		v [ pi+0 ].p = D3DXVECTOR3 ( -1.0f,  1.0f, -1.0f );
		v [ pi+1 ].p = D3DXVECTOR3 (  1.0f,  1.0f, -1.0f );
		v [ pi+2 ].p = D3DXVECTOR3 (  1.0f, -1.0f, -1.0f );
		v [ pi+3 ].p = D3DXVECTOR3 ( -1.0f,  1.0f, -1.0f );
		v [ pi+4 ].p = D3DXVECTOR3 (  1.0f, -1.0f, -1.0f );
		v [ pi+5 ].p = D3DXVECTOR3 ( -1.0f, -1.0f, -1.0f );
		// right
		pi = 6;
		v [ pi+0 ].p = D3DXVECTOR3 (  1.0f,  1.0f, -1.0f );
		v [ pi+1 ].p = D3DXVECTOR3 (  1.0f,  1.0f,  1.0f );
		v [ pi+2 ].p = D3DXVECTOR3 (  1.0f, -1.0f,  1.0f );
		v [ pi+3 ].p = D3DXVECTOR3 (  1.0f,  1.0f, -1.0f );
		v [ pi+4 ].p = D3DXVECTOR3 (  1.0f, -1.0f,  1.0f );
		v [ pi+5 ].p = D3DXVECTOR3 (  1.0f, -1.0f, -1.0f );
		// back
		pi = 12;
		v [ pi+0 ].p = D3DXVECTOR3 (  1.0f,  1.0f,  1.0f );
		v [ pi+1 ].p = D3DXVECTOR3 ( -1.0f,  1.0f,  1.0f );
		v [ pi+2 ].p = D3DXVECTOR3 ( -1.0f, -1.0f,  1.0f );
		v [ pi+3 ].p = D3DXVECTOR3 (  1.0f,  1.0f,  1.0f );
		v [ pi+4 ].p = D3DXVECTOR3 ( -1.0f, -1.0f,  1.0f );
		v [ pi+5 ].p = D3DXVECTOR3 (  1.0f, -1.0f,  1.0f );
		// left
		pi = 18;
		v [ pi+0 ].p = D3DXVECTOR3 ( -1.0f,  1.0f,  1.0f );
		v [ pi+1 ].p = D3DXVECTOR3 ( -1.0f,  1.0f, -1.0f );
		v [ pi+2 ].p = D3DXVECTOR3 ( -1.0f, -1.0f, -1.0f );
		v [ pi+3 ].p = D3DXVECTOR3 ( -1.0f,  1.0f,  1.0f );
		v [ pi+4 ].p = D3DXVECTOR3 ( -1.0f, -1.0f, -1.0f );
		v [ pi+5 ].p = D3DXVECTOR3 ( -1.0f, -1.0f,  1.0f );
		// top
		pi = 24;
		v [ pi+0 ].p = D3DXVECTOR3 ( -1.0f,  1.0f,  1.0f );
		v [ pi+1 ].p = D3DXVECTOR3 (  1.0f,  1.0f,  1.0f );
		v [ pi+2 ].p = D3DXVECTOR3 (  1.0f,  1.0f, -1.0f );
		v [ pi+3 ].p = D3DXVECTOR3 ( -1.0f,  1.0f,  1.0f );
		v [ pi+4 ].p = D3DXVECTOR3 (  1.0f,  1.0f, -1.0f );
		v [ pi+5 ].p = D3DXVECTOR3 ( -1.0f,  1.0f, -1.0f );
		// bottom
		pi = 30;
		v [ pi+0 ].p = D3DXVECTOR3 ( -1.0f, -1.0f, -1.0f );
		v [ pi+1 ].p = D3DXVECTOR3 (  1.0f, -1.0f, -1.0f );
		v [ pi+2 ].p = D3DXVECTOR3 (  1.0f, -1.0f,  1.0f );
		v [ pi+3 ].p = D3DXVECTOR3 ( -1.0f, -1.0f, -1.0f );
		v [ pi+4 ].p = D3DXVECTOR3 (  1.0f, -1.0f,  1.0f );
		v [ pi+5 ].p = D3DXVECTOR3 ( -1.0f, -1.0f,  1.0f );
		// finish vertex buffer data
		g_pStampBufferVBRef->Unlock ( );
	}

	// Create Global QUAD Mesh 
	g_pQUADMesh = new sMesh();
	DWORD dwVertexCount = 6;
	SetupMeshFVFData ( g_pQUADMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, 0 );
	float pos_pos_x =  0.6f;
	float neg_pos_x = -0.6f;
	float pos_pos_y =  0.6f;
	float neg_pos_y = -0.6f;
	SetupShortVertex ( g_pQUADMesh->dwFVF, g_pQUADMesh->pVertexData,  0,  neg_pos_x, pos_pos_y, 0.0f,  0.0f, 0.0f );
	SetupShortVertex ( g_pQUADMesh->dwFVF, g_pQUADMesh->pVertexData,  1,  pos_pos_x, pos_pos_y, 0.0f,  1.0f, 0.0f );
	SetupShortVertex ( g_pQUADMesh->dwFVF, g_pQUADMesh->pVertexData,  2,  pos_pos_x, neg_pos_y, 0.0f,  1.0f, 1.0f );
	SetupShortVertex ( g_pQUADMesh->dwFVF, g_pQUADMesh->pVertexData,  3,  pos_pos_x, neg_pos_y, 0.0f,  1.0f, 1.0f );
	SetupShortVertex ( g_pQUADMesh->dwFVF, g_pQUADMesh->pVertexData,  4,  neg_pos_x, neg_pos_y, 0.0f,  0.0f, 1.0f );
	SetupShortVertex ( g_pQUADMesh->dwFVF, g_pQUADMesh->pVertexData,  5,  neg_pos_x, pos_pos_y, 0.0f,  0.0f, 0.0f );

	// setup mesh drawing properties
	g_pQUADMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	g_pQUADMesh->iDrawVertexCount = dwVertexCount;
	g_pQUADMesh->iDrawPrimitives  = 2;
}

DARKSDK_DLL void Constructor ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hTypes, HINSTANCE hCamera )
{
	// on DLL entry, set identity
	strcpy ( g_MM_FunctionName, "Constructor" );
	#ifdef  __USE_MEMORY_MANAGER__
	mm_SnapShot();
	#endif

	// setup the object library, we load up any DLLs
	ConstructorD3D ( hSetup, hImage, hTypes, hCamera );

}

DARKSDK_DLL void DestructorD3D ( void )
{
	// free instance stamp boundbox vertex buffer
	SAFE_DELETE ( g_pQUADMesh );
	SAFE_RELEASE ( g_pStampBufferVBRef );

	// free manager resources
	m_ObjectManager.Free();

	// delete all texture lists, meshes and nodes of the universe
	if ( g_pUniverse )
	{
		delete g_pUniverse;
		g_pUniverse=NULL;
	}

	// delete each object
	for ( int iObj = 0; iObj < g_iObjectListCount; iObj++ )
		SAFE_DELETE ( g_ObjectList [ iObj ] );

	// free object array
	SAFE_DELETE_ARRAY ( g_ObjectListRef );
	SAFE_DELETE_ARRAY ( g_ObjectList );

	// delete each raw-mesh

	// mike - 061005 - leave meshes alone if we're only doing an alt tab
	// lee - 060206 - error trap
	if ( g_pGlob )
	{
		if ( g_pGlob->bInvalidFlag == false )
		{
			if ( g_RawMeshList )
				for ( int iMesh = 0; iMesh < g_iRawMeshListCount; iMesh++ )
					SAFE_DELETE ( g_RawMeshList [ iMesh ] );

			// free raw-mesh array
			SAFE_DELETE_ARRAY ( g_RawMeshList );
		}
	}
}

DARKSDK_DLL void Destructor ( void )
{
	DestructorD3D();
}

DARKSDK_DLL void SetErrorHandler ( LPVOID pErrorHandlerPtr )
{
	// Update error handler pointer
	g_pErrorHandler = (CRuntimeErrorHandler*)pErrorHandlerPtr;
}

DARKSDK_DLL void PassCoreData ( LPVOID pGlobPtr )
{
	// Held in Core, used here..
	g_pGlob = (GlobStruct*)pGlobPtr;
	g_pCreateDeleteStringFunction = g_pGlob->CreateDeleteString;

	#ifndef DARKSDK_COMPILE
		// setup function pointers
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( g_pGlob->g_GFX, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
		
		// camera functions from glob
		g_Camera3D_SetAutoCam = ( CAMERA3D_RetVoidParamFloat4PFN )	GetProcAddress ( g_pGlob->g_Camera3D, "?SetAutoCam@@YAXMMMM@Z" );
		g_Camera3D_GetInternalData = ( CAMERA3D_GetInternalDataPFN )	GetProcAddress ( g_pGlob->g_Camera3D, "?GetInternalData@@YAPAXH@Z" );
		g_Camera3D_GetInternalUpdate = ( CAMERA3D_GetInternalUpdatePFN )	GetProcAddress ( g_pGlob->g_Camera3D, "?InternalUpdate@@YAXH@Z" );
		g_Camera3D_SetFOV				= ( CAMERA3D_SetFOVPFN )		GetProcAddress ( g_pGlob->g_Camera3D, "?SetFOV@@YAXM@Z" );

		// light functions from glob
		g_Light3D_GetInternalData = ( LIGHT3D_GetInternalDataPFN )	GetProcAddress ( g_pGlob->g_Light3D, "?GetLightData@@YAPAXH@Z" );

		// memblock DLL ptrs
		if ( g_pGlob->g_Memblocks )
		{
			g_Memblock_GetMemblockExist = ( MEMBLOCKS_GetMemblockExist ) GetProcAddress ( g_pGlob->g_Memblocks, "?MemblockExist@@YAHH@Z" );
			g_Memblock_GetMemblockPtr = ( MEMBLOCKS_GetMemblockPtr ) GetProcAddress ( g_pGlob->g_Memblocks, "?GetMemblockPtr@@YAKH@Z" );
			g_Memblock_GetMemblockSize = ( MEMBLOCKS_GetMemblockSize ) GetProcAddress ( g_pGlob->g_Memblocks, "?GetMemblockSize@@YAHH@Z" );
			g_Memblock_MemblockFromImage = ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromImage@@YAXHH@Z" );
			g_Memblock_MemblockFromBitmap = ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromBitmap@@YAXHH@Z" );
			g_Memblock_MemblockFromSound = ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromSound@@YAXHH@Z" );
			g_Memblock_MemblockFromMesh = ( MEMBLOCKS_MemblockFromMedia ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMemblockFromMesh@@YAXHH@Z" );
			g_Memblock_ImageFromMemblock = ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateImageFromMemblock@@YAXHH@Z" );
			g_Memblock_BitmapFromMemblock = ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateBitmapFromMemblock@@YAXHH@Z" );
			g_Memblock_SoundFromMemblock = ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateSoundFromMemblock@@YAXHH@Z" );
			g_Memblock_MeshFromMemblock = ( MEMBLOCKS_MediaFromMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?CreateMeshFromMemblock@@YAXHH@Z" );
			g_Memblock_MakeMemblock = ( MEMBLOCKS_MakeMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?MakeMemblock@@YAXHH@Z" );
			g_Memblock_DeleteMemblock = ( MEMBLOCKS_DeleteMemblock ) GetProcAddress ( g_pGlob->g_Memblocks, "?DeleteMemblock@@YAXH@Z" );
		}

	#else
		
		g_GFX_GetDirect3DDevice		= dbGetDirect3DDevice;
		
		
		g_Camera3D_SetAutoCam = dbSetAutoCam;
		g_Camera3D_GetInternalData = dbGetCameraInternalData;
		g_Camera3D_GetInternalUpdate = dbCameraInternalUpdate;
		g_Camera3D_SetFOV				= dbSetCameraFOV;
		
	#endif

	
	m_pD3D                  = g_GFX_GetDirect3DDevice ( );

	

	// default settings for reflective plane control (using setreflectionon command)
	g_pGlob->dwRedrawCount=1;
	g_pGlob->dwStencilMode=0;
	g_pGlob->dwStencilShadowCount=0;
	g_pGlob->dwStencilReflectionCount=0;

	// allocate the list of pointers
	g_iObjectListRefCount=0;
	g_ObjectListRef = new int [ g_iObjectListCount ];
	g_ObjectList = new sObject* [ g_iObjectListCount ];

	if ( !g_ObjectList )
		RunTimeError ( RUNTIMEERROR_OBJECTMANAGERFAILED );

	// set all pointers to null
	for ( int iTemp = 0; iTemp < g_iObjectListCount; iTemp++ )
	{
		g_ObjectListRef [ iTemp ] = 0;
		g_ObjectList [ iTemp ] = NULL;
	}

	if ( !m_ObjectManager.Setup ( ) )
		RunTimeError ( RUNTIMEERROR_OBJECTMANAGERFAILED );

	if ( !m_ObjectManager.UpdateObjectListSize ( g_iObjectListCount ) )
		RunTimeError ( RUNTIMEERROR_OBJECTMANAGERFAILED );
}

DARKSDK_DLL void MakeCube ( int iID, float fSize );

DARKSDK_DLL void RefreshD3D ( int iMode )
{
	if(iMode==0)
	{
		// Remove all traces of old D3D usage
		DestructorD3D();
	}
	if(iMode==1)
	{
		// Get new D3D and recreate everything D3D related
		ConstructorD3D ( g_pGlob->g_GFX, g_pGlob->g_Image, g_pGlob->g_Vectors, g_pGlob->g_Camera3D );
		PassCoreData ( g_pGlob );
	}
}

//////////////////////////////////////////////////////////////////////////////////
// EXPORTED INTERNAL FUNCTIONS ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK_DLL void UpdateViewProjForMotionBlur ( void )
{
	m_ObjectManager.UpdateViewProjForMotionBlur ( );
}

DARKSDK_DLL void UpdateAnimationCycle ( void )
{
	// U75 - 080410 - moved here (once per SYNC, not FASTSYNC calls! as it speeds up anim)
	m_ObjectManager.UpdateAnimationCycle ( );
}


DARKSDK_DLL void UpdateOnce ( void )
{
	m_ObjectManager.UpdateInitOnce ( );
}

DARKSDK_DLL void Update ( void )
{
	m_ObjectManager.Update ( );
}

DARKSDK_DLL void UpdateGhostLayer ( void )
{
	m_ObjectManager.UpdateGhostLayer ( );
}

DARKSDK_DLL void UpdateNoZDepth ( void )
{
	m_ObjectManager.UpdateNoZLayer ( );
}

DARKSDK_DLL void* GetInternalData ( int iID )
{
	// ensure the object is present
	if ( !ConfirmObject ( iID ) )
		return NULL;

	// return a pointer to the data
	return g_ObjectList [ iID ];
}

DARKSDK_DLL void StencilRenderStart ( void )
{
	m_ObjectManager.DoStencilRenderStart();
}

DARKSDK_DLL void StencilRenderEnd ( void )
{
	m_ObjectManager.DoStencilRenderEnd();
}

DARKSDK_DLL void AutomaticStart ( void )
{
	DoAutomaticStart();
}

DARKSDK_DLL void AutomaticEnd ( void )
{
	DoAutomaticEnd();
}

//
// Internal Collision-BSP Functions
//

DARKSDK_DLL void ColPosition ( int iID, float fX, float fY, float fZ )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return;

	// set the new position
	sObject* pObject = g_ObjectList [ iID ];
	pObject->collision.bHasBeenMovedForResponse=true;

	float fOffsetX = GetColCenterX(pObject);
	float fOffsetY = GetColCenterY(pObject);
	float fOffsetZ = GetColCenterZ(pObject);
	pObject->position.vecPosition.x = fX - fOffsetX;
	pObject->position.vecPosition.y = fY - fOffsetY;
	pObject->position.vecPosition.z = fZ - fOffsetZ;
}

DARKSDK_DLL float GetXColPosition ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0.0f;

	sObject* pObject = g_ObjectList [ iID ];
	float fPos = pObject->position.vecPosition.x;
	float fOffsetX = GetColCenterX(pObject);
	return fPos + fOffsetX;
}

DARKSDK_DLL float GetYColPosition ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0.0f;

	sObject* pObject = g_ObjectList [ iID ];
	float fPos = pObject->position.vecPosition.y;
	float fOffsetY = GetColCenterY(pObject);
	return fPos + fOffsetY;
}

DARKSDK_DLL float GetZColPosition ( int iID )
{
	// check the object exists
	if ( !ConfirmObject ( iID ) )
		return 0.0f;

	sObject* pObject = g_ObjectList [ iID ];
	float fPos = pObject->position.vecPosition.z;
	float fOffsetZ = GetColCenterZ(pObject);
	return fPos + fOffsetZ;
}

//////////////////////////////////////////////////////////////////////////////////
// DBPRO CORE MANAGEMENT FUNCTIONS ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DARKSDK_DLL LPSTR GetReturnStringFromWorkString(void)
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

//////////////////////////////////////////////////////////////////////////////////
// FILE EXISTENCE FUNCTION ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#ifdef DARKSDK_COMPILE
	static bool DoesFileExist ( LPSTR pFilename )
#else
	DARKSDK_DLL bool DoesFileExist ( LPSTR pFilename )
#endif
{
	// success or failure
	bool bSuccess = true;

	// leeadd - 180606 - u62 - uses actual or virtual file
	char VirtualFilename[_MAX_PATH];
	strcpy(VirtualFilename, pFilename);
	g_pGlob->UpdateFilenameFromVirtualTable( (DWORD)VirtualFilename);

	// open File To See If Exist
	HANDLE hfile = CreateFile(VirtualFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile==INVALID_HANDLE_VALUE)
		bSuccess=false;
	else
		CloseHandle(hfile);

	// return result
	return bSuccess;
}

//////////////////////////////////////////////////////////////////////////////////
// CHECK FUNCTIONS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


// mike - 061206 - for dtsp2
bool g_bInstanceReturnFlag = false;

void dbSetInstanceReturnFlag ( bool bStatus )
{
	g_bInstanceReturnFlag = bStatus;
}

DARKSDK_DLL bool ConfirmObjectInstanceEx ( int iID, bool bInstanceReturnFlag )
{
	// mike - 061206 - need this for dtsp2 but it is going to affect other things
	if ( g_bInstanceReturnFlag == true )
		bInstanceReturnFlag = true;

	if ( iID < 1 || iID > MAXIMUMVALUE )
	{ 
		RunTimeError ( RUNTIMEERROR_B3DMODELNUMBERILLEGAL );
		return false;
	}

	if ( iID < g_iObjectListCount )
	{ 
		if ( g_ObjectList [ iID ] )
		{
			// leeadd - 310305 - new idea to speed up operations
			// mike - 130405 - this screws up other functions
			// lee - 040406 - u6rc5 - users want to use excluded objects - mike, what functions screw up?
			//if ( g_ObjectList [ iID ]->bExcluded==true )
			//{
			//	// this means it leaves silently
			//	return false;
			//}
			if ( g_ObjectList [ iID ]->pFrame )
			{
				return true;
			}
			if ( g_ObjectList [ iID ]->pInstanceOfObject )
			{
				// lee - 310306 - u6rc4 - silent return as instance objects can only do certain things (new function added for confirmobjectInstanceAllowed
				// return true;
				return bInstanceReturnFlag;
			}
			
		}
	}

	// leeadd - 290308 - U68 - report which object does not exist (very seful)
	char pWhichObjectNumber[512];
	wsprintf ( pWhichObjectNumber, "Object Number Not Found: %d", iID );
	RunTimeError ( RUNTIMEERROR_B3DMODELNOTEXISTS, pWhichObjectNumber );

	// cone
	return false;	
}

DARKSDK_DLL bool ConfirmObjectInstance ( int iID )
{
	// see above
	return ConfirmObjectInstanceEx ( iID, true );
}

DARKSDK_DLL bool ConfirmObject ( int iID )
{
	// see above (bInstanceReturnFlag=false)
	return ConfirmObjectInstanceEx ( iID, false );
}


DARKSDK_DLL bool CheckObjectExist ( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE ) return false;
	if ( iID < g_iObjectListCount )
	{ 
		if ( g_ObjectList [ iID ] )
		{
			if ( g_ObjectList [ iID ]->pFrame ) return true;
			if ( g_ObjectList [ iID ]->pInstanceOfObject ) return true;
		}
	}
	return false;	
}

DARKSDK_DLL bool ConfirmNewObject( int iID )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{ 
		RunTimeError ( RUNTIMEERROR_B3DMODELNUMBERILLEGAL );
		return false;
	}

	if ( iID < g_iObjectListCount )
	{ 
		if ( g_ObjectList [ iID ] )
		{
			if ( g_ObjectList [ iID ]->pFrame )
			{
				RunTimeError ( RUNTIMEERROR_B3DMODELALREADYEXISTS );
				return false;
			}
		}
	}

	return true;
}

bool ConfirmObjectAndLimbInstanceEx ( int iID, int iLimbID, bool bInstanceReturnFlag )
{
	if ( iID < 1 || iID > MAXIMUMVALUE )
	{ 
		RunTimeError ( RUNTIMEERROR_B3DMODELNUMBERILLEGAL );
		return false;
	}

	if ( iID < g_iObjectListCount )
	{ 
		if ( g_ObjectList [ iID ] )
		{
			if ( g_ObjectList [ iID ]->pInstanceOfObject && bInstanceReturnFlag==false )
			{
				// lee - 310306 - u6rc4 - silent return if instance not allowed
				return false;
			}
			// leefix - 290506 - u62 - fpsccode - instanced object limbs can be SHOW/HIDE
			if ( g_ObjectList [ iID ]->pInstanceMeshVisible )
			{
				// limb part of vis-array - continue
			}
			else
			{
				if ( !g_ObjectList [ iID ]->pFrame )
				{
					RunTimeError ( RUNTIMEERROR_B3DMODELNOTEXISTS );
					return false;
				}
			}
		}
		else
		{
			RunTimeError ( RUNTIMEERROR_B3DMODELNOTEXISTS );
			return false;
		}
	}
	else
	{
		RunTimeError ( RUNTIMEERROR_B3DMODELNOTEXISTS );
		return false;
	}

	if ( iLimbID < 0 || iLimbID > MAXIMUMVALUE )
	{ 
		RunTimeError ( RUNTIMEERROR_LIMBNUMBERILLEGAL );
		return false;
	}

	if ( g_ObjectList [ iID ]->pInstanceMeshVisible )
	{
		// limb part of vis-array
	}
	else
	{
		if ( iLimbID >= g_ObjectList [ iID ]->iFrameCount )
		{
			RunTimeError ( RUNTIMEERROR_LIMBNOTEXIST );
			return false;
		}

		if ( !g_ObjectList [ iID ]->ppFrameList [ iLimbID ] )
		{
			RunTimeError ( RUNTIMEERROR_LIMBNOTEXIST );
			return false;
		}
	}

	return true;
}

DARKSDK_DLL bool ConfirmObjectAndLimbInstance ( int iID, int iLimbID )
{
	// see above
	return ConfirmObjectAndLimbInstanceEx ( iID, iLimbID, true );
}

DARKSDK_DLL bool ConfirmObjectAndLimb( int iID, int iLimbID )
{
	// see above
	return ConfirmObjectAndLimbInstanceEx ( iID, iLimbID, false );
}

DARKSDK_DLL bool ConfirmMesh ( int iMeshID )
{
	if ( iMeshID < 1 || iMeshID > MAXIMUMVALUE )
	{ 
		RunTimeError ( RUNTIMEERROR_B3DMESHNUMBERILLEGAL );
		return false;
	}

	if ( iMeshID < g_iRawMeshListCount )
	{ 
		if ( g_RawMeshList [ iMeshID ] )
		{
			return true;
		}
	}

	RunTimeError ( RUNTIMEERROR_B3DMESHNOTEXIST );
	return false;	
}

DARKSDK_DLL bool ConfirmNewMesh ( int iMeshID )
{
	if ( iMeshID < 1 || iMeshID > MAXIMUMVALUE )
	{ 
		RunTimeError ( RUNTIMEERROR_B3DMESHNUMBERILLEGAL );
		return false;
	}

	if ( iMeshID < g_iRawMeshListCount )
	{ 
		if ( g_RawMeshList [ iMeshID ] )
		{
			// delete old mesh
			DeleteRawMesh ( g_RawMeshList [ iMeshID ] );
			g_RawMeshList [ iMeshID ] = NULL;
		}
	}

	return true;
}

DARKSDK_DLL bool ConfirmEffect ( int iEffectID )
{
	if ( iEffectID < 1 || iEffectID > MAX_EFFECTS )
	{ 
		RunTimeError ( RUNTIMEERROR_B3DEFFECTNUMBERILLEGAL );
		return false;
	}
	if ( m_EffectList [ iEffectID ]==NULL )
	{
		RunTimeError ( RUNTIMEERROR_B3DEFFECTNOTEXISTS );
		return false;
	}

	// success
	return true;
}

DARKSDK_DLL bool ConfirmNewEffect ( int iEffectID )
{
	if ( iEffectID < 1 || iEffectID > MAX_EFFECTS )
	{ 
		RunTimeError ( RUNTIMEERROR_B3DEFFECTNUMBERILLEGAL );
		return false;
	}
	if ( m_EffectList [ iEffectID ] )
	{
		RunTimeError ( RUNTIMEERROR_B3DEFFECTALREADYEXISTS );
		return false;
	}

	// success
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// OBJECT MANAGEMENT FUNCTIONS ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DARKSDK_DLL void ResizeObjectList ( int iSize )
{
	// this function will get called if a user goes over the current limit of objects
	// when this happens we will need to resize the list so it can store the new data
	int*		pNewObjectListRef;
	sObject**	pNewObjectList;

	// size
	int iOriginalCount = g_iObjectListCount;
	g_iObjectListCount += 50;
	if ( g_iObjectListCount <= iSize )
		g_iObjectListCount = iSize + 5;

	// create
	pNewObjectListRef = new int [ g_iObjectListCount ];
	pNewObjectList = new sObject* [ g_iObjectListCount ];

	// lee - 200306 - u6b4 - if not created (super large size), do not crash!
	if ( pNewObjectListRef && pNewObjectList )
	{
		for ( int iTemp = 0; iTemp < g_iObjectListCount; iTemp++ )
		{
			pNewObjectListRef [ iTemp ] = 0;
			pNewObjectList [ iTemp ] = NULL;
		}
		for ( int iTemp = 0; iTemp < iOriginalCount; iTemp++ )
		{
			pNewObjectListRef [ iTemp ] = g_ObjectListRef [ iTemp ];
			pNewObjectList [ iTemp ] = g_ObjectList [ iTemp ];
		}
		SAFE_DELETE_ARRAY ( g_ObjectListRef );
		SAFE_DELETE_ARRAY ( g_ObjectList );
		g_ObjectListRef = pNewObjectListRef;
		g_ObjectList = pNewObjectList;
	}

	// leefix - 010306 - u60 - only need to be the size of the shortlist qty
	// so we have moved this to the shortlist creator! (shortlist controls every loop!)
//	m_ObjectManager.UpdateObjectListSize ( g_iObjectListCount );
}

DARKSDK_DLL void AddObjectToObjectListRef ( int iID )
{
	// check if already in list
	for ( int iIndex = 0; iIndex < g_iObjectListRefCount; iIndex++ )
		if ( g_ObjectListRef [ iIndex ]==iID )
			return;

	// add new entry
	g_ObjectListRef [ g_iObjectListRefCount ] = iID;
	g_iObjectListRefCount++;

	// update global arrays for shortlist entry expansion
	m_ObjectManager.UpdateObjectListSize ( g_iObjectListRefCount );
}

DARKSDK_DLL void  RemoveObjectFromObjectListRef ( int iID )
{
	// check if in list
	for ( int iIndex = 0; iIndex < g_iObjectListRefCount; iIndex++ )
	{
		if ( g_ObjectListRef [ iIndex ]==iID )
		{
			// shuffle to remove it
			DWORD dwSize = (g_iObjectListRefCount-iIndex)-1;
			if ( dwSize > 0 ) memcpy ( &g_ObjectListRef[iIndex], &g_ObjectListRef[iIndex+1], dwSize*sizeof(int) );
			g_iObjectListRefCount--;
			return;
		}
	}
}

DARKSDK_DLL void ResizeRawMeshList ( int iIndexNeeded )
{
	// temp count store and ptr
	int iOriginalCount = g_iRawMeshListCount;
	sMesh**	pNewRawMeshList = NULL;

// leefix - 080206 - only if needed!
//	g_iRawMeshListCount += 50;

	// increment only if needed
	if ( g_iRawMeshListCount <= iIndexNeeded )
		g_iRawMeshListCount = iIndexNeeded + 5;

	// create new list
	pNewRawMeshList = new sMesh* [ g_iRawMeshListCount ];
	
	// clear new list
	for ( int iTemp = 0; iTemp < g_iRawMeshListCount; iTemp++ )
		pNewRawMeshList [ iTemp ] = NULL;

	// copy old list to new list
	for ( int iTemp = 0; iTemp < iOriginalCount; iTemp++ )
		pNewRawMeshList [ iTemp ] = g_RawMeshList [ iTemp ];

	// remove old list from memory
	SAFE_DELETE_ARRAY ( g_RawMeshList );

	// update actual list ptr
	g_RawMeshList = pNewRawMeshList;
}

static float wrapangleoffset(float da)
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

DARKSDK_DLL void GetAngleFromPoint(float x1, float y1, float z1, float x2, float y2, float z2, float* ax, float* ay, float* az)
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

DARKSDK_DLL void RegenerateLookVectors ( sObject* pObject )
{
	// regenerate the look, up and right vectors
	pObject->position.vecLook  = D3DXVECTOR3 ( 0, 0, 1 );		// look vector
	pObject->position.vecUp    = D3DXVECTOR3 ( 0, 1, 0 );		// up vector
	pObject->position.vecRight = D3DXVECTOR3 ( 1, 0, 0 );		// right vector
	
	if ( pObject->position.bFreeFlightRotation )
	{
		// free flight modifies lookupright directly (uses current rotation matrix)
		D3DXVec3TransformCoord ( &pObject->position.vecLook,	&pObject->position.vecLook,		&pObject->position.matFreeFlightRotate );
		D3DXVec3TransformCoord ( &pObject->position.vecUp,		&pObject->position.vecUp,		&pObject->position.matFreeFlightRotate );
		D3DXVec3TransformCoord ( &pObject->position.vecRight,	&pObject->position.vecRight,	&pObject->position.matFreeFlightRotate );
	}
	else
	{
		// transform by euler rotation
		UpdateEulerRotation ( pObject );
		D3DXVec3TransformCoord ( &pObject->position.vecLook,	&pObject->position.vecLook,		&pObject->position.matRotation );
		D3DXVec3TransformCoord ( &pObject->position.vecUp,		&pObject->position.vecUp,		&pObject->position.matRotation );
		D3DXVec3TransformCoord ( &pObject->position.vecRight,	&pObject->position.vecRight,	&pObject->position.matRotation );
	}
}

DARKSDK_DLL void AnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles )
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

DARKSDK_DLL void CheckRotationConversion ( sObject* pObject, bool bUseFreeFlightMode )
{
	// has there been a change?
	if ( bUseFreeFlightMode != pObject->position.bFreeFlightRotation )
	{
		// Caluclates equivilant rotation data if switch rotation-modes
		if( bUseFreeFlightMode==true )
		{
			// Euler to Freeflight
			UpdateEulerRotation ( pObject );
			pObject->position.matFreeFlightRotate = pObject->position.matRotation;
		}
	}

	// always calculate freeflight to euler (for angle feedback)
	if( bUseFreeFlightMode==true )
	{
		// Freeflight to Euler
		AnglesFromMatrix ( &pObject->position.matFreeFlightRotate, &pObject->position.vecRotate );
	}

	// new rotation mode
	pObject->position.bFreeFlightRotation = bUseFreeFlightMode;
}

DARKSDK_DLL void UpdateOverlayFlag ( sObject* pObject )
{

	// leefix - 190303 - false unless an overlay flag is being used
	pObject->bOverlayObject = false;
	if ( pObject->bNewZLayerObject )	pObject->bOverlayObject = true;
	if ( pObject->bTransparentObject )	pObject->bOverlayObject = true;	
	if ( pObject->bLockedObject )		pObject->bOverlayObject = true;
	if ( pObject->bGhostedObject )		pObject->bOverlayObject = true;
}

DARKSDK_DLL void SetObjectTransparency ( sObject* pObject, int iTransparency )
{
	// promote to overlay layer (or not)
	if ( iTransparency==2 || iTransparency==3 || iTransparency==5 || iTransparency==6 )
		pObject->bTransparentObject = true;
	else
		pObject->bTransparentObject = false;

	// leeadd - 021205 - new feature - create a water line (for two layers of depth sorted objs above and below line)
	if ( iTransparency==5 )
		pObject->bTransparencyWaterLine = true;
	else
		pObject->bTransparencyWaterLine = false;

	// leeadd - 061208 - transparency mode 7 sets an object to very early draw phase
	if ( iTransparency==7 )
		pObject->bVeryEarlyObject = true;
	else
		pObject->bVeryEarlyObject = false;

	// leefix - 190303 - better overlay flag handling
	UpdateOverlayFlag ( pObject );
}

DARKSDK_DLL void NewObjectAutoCam( float fRadius )
{
	g_Camera3D_SetAutoCam ( 0.0f, 0.0f, 0.0f, fRadius );
}

DARKSDK_DLL void SetupModelFunctionPointers ( void* pModel, HINSTANCE hDLL )
{
	// MIKEMIKE : Propose we remove this!
}

DARKSDK_DLL void SetupPrimitiveFunctionPointers ( void* pData )
{
	// MIKEMIKE : Propose we remove this!
}

DARKSDK_DLL void SetupDefaultProperties ( void* pData )
{
	// MIKEMIKE : Propose we remove this!
}

DARKSDK_DLL void SetupDefaultPosition ( int iID )
{
	Position ( iID, 0.0f, 0.0f, 0.0f );
	Scale    ( iID, 100.0f, 100.0f, 100.0f );
	Rotate   ( iID, 0.0f, 0.0f, 0.0f );
}

DARKSDK_DLL bool PrepareCustomObject ( void* m_pData )
{
	// MIKEMIKE : Propose we remove this!
	// Complete
	return true;
}

DARKSDK_DLL void GetCullDataFromModel ( int iID )
{
	// MIKEMIKE : Propose we remove this!
}

DARKSDK_DLL bool CreateModelFromCustom( int iID, void* m_pData )
{
	// MIKEMIKE : Propose we remove this!
	return true;
}

DARKSDK_DLL bool SetNewObjectFinalProperties ( int iID, float fRadius )
{
	// setup new object and introduce to buffers
	sObject* pObject = g_ObjectList [ iID ];
	if ( pObject == NULL ) return false;

	// lee - 250307 - store object number for reference
	pObject->dwObjectNumber = iID;

	// calculate collision data
	SetupObjectsGenericProperties ( pObject );
	
	// verify object has at least one frame and mesh
	// mike - 071005 - objects without meshes are now acceptable e.g. dummy objects that store custom data
	//if ( pObject->ppFrameList==NULL || pObject->ppMeshList==NULL )
	if ( pObject->ppFrameList==NULL )
	{
		// free object if insufficient data
		RunTimeError ( RUNTIMEERROR_B3DOBJECTLOADFAILED );
		SAFE_DELETE ( g_ObjectList [ iID ] );
		return false;
	}

	// map bone and animation information to frames
	bool bUpdateBones = true;
	if ( g_ObjectList [ iID ]->pAnimationSet )
	{
		if ( g_ObjectList [ iID ]->pAnimationSet->pAnimation )
		{
			// specifically for MDL models
			if ( !g_ObjectList [ iID ]->pAnimationSet->pAnimation->bBoneType )
				bUpdateBones = false;
		}
	}
	if ( bUpdateBones )
	{
		if ( pObject->ppMeshList )
		{
			InitFramesToBones ( pObject->ppMeshList, pObject->iMeshCount );
			MapFramesToBones ( pObject->ppMeshList, pObject->pFrame, pObject->iMeshCount );
		}
	}

	// leefix - 010304 - no reason to have animset with no anims
	if ( g_ObjectList [ iID ]->pAnimationSet )
	{
		if ( g_ObjectList [ iID ]->pAnimationSet->pAnimation==NULL )
		{
			// delete anim set as no data to back it up
			SAFE_DELETE ( g_ObjectList [ iID ]->pAnimationSet );
		}
	}

	// map matrix frames to animation data
	MapFramesToAnimations ( pObject );

	// store original vertex data immediately
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];
		DWORD dwTotalVertSize = pMesh->dwVertexCount * pMesh->dwFVFSize;
		#ifndef NEVERSTOREORIGINALVERTICES
		if ( pMesh->pOriginalVertexData==NULL )
		{
			// create original vertex data memory
			pMesh->pOriginalVertexData = (BYTE*)new char [ dwTotalVertSize ];
			memcpy ( pMesh->pOriginalVertexData, pMesh->pVertexData, dwTotalVertSize );
		}
		#endif
	}

	// ensure vectors are initially updated
	for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
	{
		sFrame* pFrame = pObject->ppFrameList [ iFrame ];
		if ( pFrame ) pFrame->bVectorsCalculated = false;
	}

	// lee - 280306 - u6rc3 - trigger at least one anim-update if object contains bones
	pObject->bAnimUpdateOnce = true;
	
	// calculate bounding areas of object
	pObject->collision.fScaledLargestRadius = 0.001f; // 140207 - this triggers scalerd radius to be filled in CalculateAllBounds
	if ( !CalculateAllBounds ( pObject, false ) )
		return false;
	
	// now setup default position, scale etc.
	SetupDefaultPosition ( iID );

	// sphere uses fixed box check (when autocollision used)
	SetColToSpheres ( pObject );

	// set autocamera
	if ( fRadius==-1.0f ) fRadius = g_ObjectList [ iID ]->collision.fRadius;
	NewObjectAutoCam ( fRadius );
	
	// finally add the object's vertex and index data to the buffers
	g_ObjectList [ iID ]->bReplaceObjectFromBuffers = true;
	m_ObjectManager.g_bObjectReplacedUpdateBuffers = true;

	// Set the object to conform to VBIB usage
	g_ObjectList [ iID ]->bUsesItsOwnBuffers = g_bGlobalVBIBUsageFlag;

	// okay
	return true;
}

DARKSDK_DLL bool CreateNewObject ( int iID, LPSTR pName )
{
	// create a new object

	// ensure the object is okay to use
	if ( !ConfirmNewObject ( iID ) )
		return false;

	// check memory allocation
	ID_ALLOCATION ( iID );

	// create a new, empty mesh
	if ( !CreateMesh ( &g_ObjectList [ iID ], pName ) )
		return false;

	// add object id to shortlist
	AddObjectToObjectListRef ( iID );

	// object created successfully
	return true;
}

DARKSDK_DLL bool DeleteObject ( int iID )
{
	// lee - 030306 - u60 - excluded objects are not caught by old way
	if ( !CheckObjectExist ( iID ) )
		return false;

	// remove object from buffers
	sObject* pObject = g_ObjectList [ iID ];

    // If this object is dependent on another, decrease the other objects dependency count
    if (pObject->pObjectDependency)
    {
        pObject->pObjectDependency->dwDependencyCount--;
        pObject->pObjectDependency = NULL;
    }

    // leefix - 010306 - u60 - no need to remove resource or further instances of an instanced object
	sObject* pInstanceOf = pObject->pInstanceOfObject;
	if ( pInstanceOf==NULL )
	{
		// remove resource of this object
		m_ObjectManager.RemoveBuffersUsedByObject ( pObject );

        // Only need to search for dependents if there are some for this object
        if (pObject->dwDependencyCount)
        {
		    // ensure all instances associated with this object are removed also
		    for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
		    {
			    // get index from shortlist
			    int iScanObjectID = g_ObjectListRef [ iShortList ];

			    // see if we have a valid list
			    sObject* pScanObject = g_ObjectList [ iScanObjectID ];
			    if ( !pScanObject ) continue;

			    // if not this object and an instance of it, delete it also
			    if ( pScanObject->pObjectDependency==pObject )
			    {
				    // delete instance/shared-clone of the master object (being deleted)
				    DeleteObject ( iScanObjectID );

				    // as recursing through shortlist, we may have skipped one so backtrack
				    if ( iShortList >= 0 )
					    iShortList--;
			    }
            }
		}
	}
	else
	{
		// instance has created some memory
		SAFE_DELETE_ARRAY ( pObject->pInstanceMeshVisible );
	}

	// remove object from universe
	if ( g_pUniverse )
		g_pUniverse->Detach ( pObject );

	// Delete old stencil effect (if any)
	m_ObjectManager.DeleteStencilEffect ( pObject );

	// delete mesh
	if ( !DeleteMesh ( &g_ObjectList [ iID ] ) )
		return false;

	// clear item from list perminantly
	RemoveObjectFromObjectListRef ( iID );
	g_ObjectList [ iID ] = NULL;

	// ensure temp lists are cleared as this object no longer exists
	m_ObjectManager.UpdateTextures();

	// object deleted okay
	return true;
}

DARKSDK_DLL bool CreateNewObject ( int iID, LPSTR pName, int iFrame )
{
	// ensure the object is okay to use
	if ( !ConfirmNewObject ( iID ) )
		return false;

	// check memory allocation
	ID_ALLOCATION ( iID );

	// create a new, empty mesh
	if ( !CreateMesh ( &g_ObjectList [ iID ], pName ) )
		return false;

	sMesh*  pMesh  = g_ObjectList [ iID ]->pFrame->pMesh;
	sFrame* pFrame = g_ObjectList [ iID ]->pFrame;

	for ( int i = 0; i < iFrame - 1; i++ )
	{
		pFrame->pChild        = new sFrame;
		pFrame->pChild->pMesh = new sMesh;

		pMesh  = pFrame->pChild->pMesh;
		pFrame = pFrame->pChild;
	}

	// add object id to shortlist
	AddObjectToObjectListRef ( iID );

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// TEXTURE/STATE/SHADER FUNCTIONS ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DARKSDK_DLL DWORD VectortoRGBA( D3DXVECTOR3* v, FLOAT fHeight )
{
    DWORD r = (DWORD)( 127.0f * v->x + 128.0f );
    DWORD g = (DWORD)( 127.0f * v->y + 128.0f );
    DWORD b = (DWORD)( 127.0f * v->z + 128.0f );
    DWORD a = (DWORD)( 255.0f * fHeight );
    
    return( (a<<24L) + (r<<16L) + (g<<8L) + (b<<0L) );
}

DARKSDK_DLL bool DoesDepthBufferHaveStencil(D3DFORMAT d3dfmt)
{
	if(d3dfmt==D3DFMT_D24S8) return true;
	if(d3dfmt==D3DFMT_D15S1) return true;
	if(d3dfmt==D3DFMT_D24X4S4) return true;
	return false;
}

DARKSDK_DLL BOOL SupportsStencilBuffer()
{
	D3DSURFACE_DESC ddsd;
	LPDIRECT3DSURFACE9 pSurface;
	m_pD3D->GetDepthStencilSurface(&pSurface);
	if(pSurface)
	{
		pSurface->GetDesc(&ddsd);
		SAFE_RELEASE(pSurface);
	}

    D3DCAPS9 d3dCaps;
    m_pD3D->GetDeviceCaps( &d3dCaps );
    if ( d3dCaps.StencilCaps & ( D3DSTENCILCAPS_DECR | D3DSTENCILCAPS_INCR | D3DSTENCILCAPS_KEEP | D3DSTENCILCAPS_REPLACE | D3DSTENCILCAPS_ZERO ))
		if ( DoesDepthBufferHaveStencil ( ddsd.Format )==true )
			return TRUE;

    return FALSE;
}

DARKSDK_DLL BOOL SupportsUserClippingPlane()
{
    D3DCAPS9 d3dCaps;
    m_pD3D->GetDeviceCaps( &d3dCaps );
    if ( d3dCaps.MaxUserClipPlanes > 0 )
		return TRUE;

    return FALSE;
}

DARKSDK_DLL BOOL SupportsEnvironmentBumpMapping()
{
    D3DCAPS9 d3dCaps;
    m_pD3D->GetDeviceCaps( &d3dCaps );
    if ( d3dCaps.TextureOpCaps & ( D3DTEXOPCAPS_BUMPENVMAP | D3DTEXOPCAPS_BUMPENVMAPLUMINANCE ))
	    if( d3dCaps.MaxTextureBlendStages >= 3 )
		    return TRUE;

    return FALSE;
}

DARKSDK_DLL BOOL SupportsCubeMapping()
{
    D3DCAPS9 d3dCaps;
    m_pD3D->GetDeviceCaps( &d3dCaps );
    if ( d3dCaps.TextureOpCaps & ( D3DPTEXTURECAPS_CUBEMAP | D3DPTEXTURECAPS_CUBEMAP_POW2 | D3DPTEXTURECAPS_MIPCUBEMAP ))
	    return TRUE;

    return FALSE;
}

DARKSDK_DLL BOOL SupportsDotProduct3BumpMapping()
{
    D3DCAPS9 d3dCaps;
    m_pD3D->GetDeviceCaps( &d3dCaps );
    if ( d3dCaps.TextureOpCaps & ( D3DTEXOPCAPS_DOTPRODUCT3 ))
        if( d3dCaps.MaxTextureBlendStages >= 2 )
			return TRUE;

	return FALSE;
}

DARKSDK_DLL int GetBitDepthFromFormat(D3DFORMAT Format)
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

DARKSDK_DLL void CreateImageNormalMap( int iImageIndex )
{
}

DARKSDK_DLL LPDIRECT3DCUBETEXTURE9 CreateNewImageCubeMap ( int i1, int i2, int i3, int i4, int i5, int i6 )
{
	// Image determines cubetexture format
	DWORD dwWidth = g_Image_GetWidth ( i1 );
	D3DFORMAT Format = D3DFMT_A8R8G8B8;

	// Individual Images
	LPDIRECT3DCUBETEXTURE9 pCubeTexture = NULL;
	HRESULT hRes = D3DXCreateCubeTexture( m_pD3D, dwWidth, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &pCubeTexture );
	if ( pCubeTexture )
	{
		// Copy images to cubetexture surfaces
		for ( DWORD s=0; s<6; s++)
		{
			LPDIRECT3DSURFACE9 pCubeSurface=NULL;
			HRESULT hRes = pCubeTexture->GetCubeMapSurface( (D3DCUBEMAP_FACES)s, 0, &pCubeSurface );
			if ( pCubeSurface )
			{
				int iImage;
				if ( s==0 ) iImage = i1;
				if ( s==1 ) iImage = i2;
				if ( s==2 ) iImage = i3;
				if ( s==3 ) iImage = i4;
				if ( s==4 ) iImage = i5;
				if ( s==5 ) iImage = i6;
				LPDIRECT3DSURFACE9 pImageSurface=NULL;
				LPDIRECT3DTEXTURE9 pImageTexture = g_Image_GetPointer ( iImage );
				pImageTexture->GetSurfaceLevel ( 0, &pImageSurface );
				if ( pImageSurface )
				{
					D3DXLoadSurfaceFromSurface( pCubeSurface, NULL, 0, pImageSurface, NULL, 0, D3DX_DEFAULT, 0);
					pImageSurface->Release();
				}
				pCubeSurface->Release();
			}
		}
	}
	return pCubeTexture;
}

// Data Access Functions

DARKSDK_DLL void GetMeshData( int iMeshID, DWORD* pdwFVF, DWORD* pdwFVFSize, DWORD* pdwVertMax, LPSTR* pData, DWORD* dwDataSize, bool bLockData )
{
	// Read Data
	if(bLockData==true)
	{
		// ensure mesh is valid
		if ( !ConfirmMesh ( iMeshID ) )
			return;

		// get mesh pointer
		sMesh* pOriginalMesh = g_RawMeshList [ iMeshID ];	// mesh data
		if ( pOriginalMesh==NULL )
			return;

		// create new mesh from scratch
		sMesh* pActualMesh = new sMesh;

		// copy from existing mesh
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity ( &matWorld );
		MakeMeshFromOtherMesh ( true, pActualMesh, pOriginalMesh, &matWorld );

		// eliminate any index data from new mesh
		ConvertLocalMeshToVertsOnly ( pActualMesh );

		// mesh data
		*pdwFVF = pActualMesh->dwFVF;
		*pdwFVFSize = pActualMesh->dwFVFSize;
		*pdwVertMax = pActualMesh->dwVertexCount;

		// create memory
		DWORD dwSizeOfData = pActualMesh->dwVertexCount * pActualMesh->dwFVFSize;
		*pData = new char[dwSizeOfData];
		*dwDataSize = dwSizeOfData;

		// copy mesh to new memory
		memcpy( *pData, pActualMesh->pVertexData, dwSizeOfData );

		// free temp mesh
		SAFE_DELETE(pActualMesh);
	}
	else
	{
		// free memory
		delete *pData;
	}
}

DARKSDK_DLL void SetMeshData( int iMeshID, DWORD dwFVF, DWORD dwFVFSize, LPSTR pMeshData, DWORD dwVertMax )
{
	// ensure mesh is valid, else delete it
	if ( !ConfirmNewMesh ( iMeshID ) )
		return;

	// create new mesh from data
	sMesh* pMesh = new sMesh;
	if (!MakeLocalMeshFromPureMeshData ( pMesh, dwFVF, dwFVFSize, (float*)pMeshData, dwVertMax, D3DPT_TRIANGLELIST ) )
		return;

	// check memory allocation
	ID_MESH_ALLOCATION ( iMeshID );

	// mesh stored in list
	g_RawMeshList [ iMeshID ] = pMesh;

}

DARKSDK_DLL void CreateNewOrSharedEffect ( sMesh* pMesh, bool bChangeMesh )
{
	// Search if effect already loaded
	int iEffectIDFound = 0;
	for ( int iEffectID=0; iEffectID<MAX_EFFECTS; iEffectID++ )
	{
		if ( m_EffectList [ iEffectID ] )
		{
			if ( _stricmp ( m_EffectList [ iEffectID ]->pEffectObj->m_pEffectName, (LPSTR)pMesh->pEffectName )==NULL )
			{
				iEffectIDFound=iEffectID;
				break;
			}
		}
	}

	// Found matching effect name
	if ( iEffectIDFound>0 )
	{
		// apply to specific mesh
		sEffectItem* pEffectItem = m_EffectList [ iEffectIDFound ];
		SetSpecialEffect ( pMesh, pEffectItem->pEffectObj, bChangeMesh );
		pMesh->bVertexShaderEffectRefOnly = true;
	}
	else
	{
		// No, create external effect obj
		cSpecialEffect* pEffectObj = new cExternalEffect;
		pEffectObj->Load ( (LPSTR)pMesh->pEffectName, false, bChangeMesh );
		SetSpecialEffect ( pMesh, pEffectObj );
	}
}

DARKSDK sFrame*  CreateNewFrame ( sObject* pObject, LPSTR pName, bool bNewMesh )
{
	sMesh* pNewMesh = NULL;
    
    // If a mesh is required, create one
    if (bNewMesh)
    {
        pNewMesh = new sMesh;
	    if ( ! pNewMesh )
            return 0;
    }

    // Add a new frame using the (possibly existing) mesh
    AddNewFrame ( pObject, pNewMesh, pName );

    // Identify the new frame pointer
    sFrame* pLastRootFrame = pObject->pFrame;
	while ( pLastRootFrame->pSibling )
		pLastRootFrame = pLastRootFrame->pSibling;

    // recreate all meshand frame lists
    CreateFrameAndMeshList ( pObject );

    // ensure bounds are recalculated
    pObject->bUpdateOverallBounds=true;

    // update mesh(es) of object
    m_ObjectManager.RefreshObjectInBuffer ( pObject );

    return pLastRootFrame;
}




#ifdef DARKSDK_COMPILE

void ConstructorBasic3D ( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hTypes, HINSTANCE hCamera )
{
	Constructor ( hSetup, hImage, hTypes, hCamera );
}

void SetErrorHandlerBasic3D ( LPVOID pErrorHandlerPtr )
{
	SetErrorHandler ( pErrorHandlerPtr );
}

void PassCoreDataBasic3D ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

void RefreshD3DBasic3D ( int iMode )
{
	RefreshD3D ( iMode );
}

void DestructorBasic3D ( void )
{
	Destructor ( );
}

void dbUpdateBasic3D ( void )
{
	Update ( );
}

void dbUpdateOnce ( void )
{
	UpdateOnce ( );
}

void dbUpdateNoZDepth ( void )
{
	UpdateNoZDepth ( );
}

// mike - 281106 - need to add this in
void dbUpdateGhostLayer ( void )
{
	UpdateGhostLayer ( );
}

void dbStencilRenderStart ( void )
{
	StencilRenderStart ( );
}

void dbStencilRenderEnd	( void )
{
	StencilRenderEnd ( );
}

void dbAutomaticStart ( void )
{
	AutomaticStart ( );
}

void dbAutomaticEnd	( void )
{
	AutomaticEnd ( );
}

void dbGetMeshData ( int iMeshID, DWORD* pdwFVF, DWORD* pdwFVFSize, DWORD* pdwVertMax, LPSTR* pData, DWORD* dwDataSize, bool bLockData )
{
	GetMeshData ( iMeshID, pdwFVF, pdwFVFSize, pdwVertMax, pData, dwDataSize, bLockData );
}

void dbSetMeshData ( int iMeshID, DWORD dwFVF, DWORD dwFVFSize, LPSTR pMeshData, DWORD dwVertMax )
{
	SetMeshData ( iMeshID, dwFVF, dwFVFSize, pMeshData, dwVertMax );
}

void* dbObjectGetInternalData ( int iID )
{
	// mike - 210405 - need to expose this function

	return GetInternalData ( iID );
}

#endif