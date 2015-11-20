
//
// Common/Private Functions Header
//

#ifndef _COMMONC_H_
#define _COMMONC_H_

// Defines
#pragma warning(disable : 4530)

//////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTX ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "windows.h"
#include <D3DX9.h>

//////////////////////////////////////////////////////////////////////////////////
// INCLUDE SUPPORT FILES /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "cpositionc.h"
#include ".\..\camera\ccameradatac.h"
#include ".\..\light\clightdatac.h"
#include ".\..\core\globstruct.h"
#include ".\..\error\cerror.h"
#include "BoxCollision\CCollision.h"
#include "CObjectManagerC.h"
#include "cObjectDataC.h"
#include "universe.h"
#include "CSG.h"
#include "cLightMaps.h"

//////////////////////////////////////////////////////////////////////////////////
// DEFINES ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#ifdef COMPILEFORDARKSDK
//#ifdef DARKSDK_COMPILE - lee - 300706 - this is the correct one but causes errors (esp. with strings)
 //#define DARKSDK			__declspec ( dllexport )
 #define SDK_BOOL			bool
 #define SDK_FLOAT			float
 #define SDK_LPSTR			char*
 #define SDK_RETFLOAT(f)	f
 #define SDK_RETSTR			
#else
//#define DARKSDK			__declspec ( dllexport )
 #define SDK_BOOL			int
 #define SDK_FLOAT			DWORD
 #define SDK_LPSTR			DWORD
 #define SDK_RETFLOAT(f)	*(DWORD*)&f 
 #define SDK_RETSTR			DWORD lpStr, 
#endif

//////////////////////////////////////////////////////////////////////////////////
// MACROS ////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//#define SAFE_DELETE( p )		{ if ( p ) { delete ( p );       ( p ) = NULL; } }
//#define SAFE_RELEASE( p )		{ if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
//#define SAFE_DELETE_ARRAY( p )	{ if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

// we use this define whenever we create an object, it checks if the specified ID
// is within the current range, if it is not we will allocate some new memory and
// resize the current list
#define ID_ALLOCATION( a )		{ if ( a >= g_iObjectListCount ) ResizeObjectList ( a ); }
#define ID_MESH_ALLOCATION( a )	{ if ( a >= g_iRawMeshListCount ) ResizeRawMeshList ( a ); }

//////////////////////////////////////////////////////////////////////////////////
// VERTEX AND PIXEL SHADER STRUCTURES ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#define	MAX_VERTEX_SHADERS		30
#define	MAX_PIXEL_TEXTURES		8
#define MAX_EFFECTS				65535

struct sVertexShader
{
	LPDIRECT3DVERTEXSHADER9			pVertexShader;		// vertex shader object
	LPDIRECT3DVERTEXDECLARATION9	pVertexDec;			// vertex shader declaration

	DWORD							dwDecArrayCount;	// vertex shader dec count
	D3DVERTEXELEMENT9*				pDecArray;			// vertex shader dec elements
	
	sVertexShader ( )
	{
		memset ( this, 0, sizeof ( sVertexShader ) );
	}
};

struct sPixelShader
{
	LPDIRECT3DPIXELSHADER9			pPixelShader;		// pixel shader object
	
	sPixelShader ( )
	{
		memset ( this, 0, sizeof ( sPixelShader ) );
	}
};

struct sEffectItem
{
	cSpecialEffect*					pEffectObj;		// effect ptr

	sEffectItem ( )
	{
		memset ( this, 0, sizeof ( sEffectItem ) );
		pEffectObj = new cExternalEffect;
	}
	~sEffectItem ( )
	{
		SAFE_DELETE ( pEffectObj );
	}
};

//////////////////////////////////////////////////////////////////////////////////
// COLLISION COMMON DATA /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
struct sFullCollisionResult
{
	bool bUsed;
	float fClosestDist;
	D3DXVECTOR3 vecPos;
	D3DXVECTOR3 vecNormal;
	D3DXVECTOR3 vecDifference;
	int iTextureIndex;
	float fTextureU, fTextureV;
	int iPolysChecked;
	DWORD dwArbitaryValue;
	bool bCollidedAgainstFloor;
};

//////////////////////////////////////////////////////////////////////////////////
// PICK COMMON DATA //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
struct sFullPickResult
{
	int iObjectID;
	float fPickDistance;
	D3DXVECTOR3 vecPickVector;
};
extern sFullPickResult g_DBPROPickResult;

//////////////////////////////////////////////////////////////////////////////////
// EXTERNAL COMMON DATA //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

enum eSortOrder
{
    E_SORT_BY_TEXTURE = 0,
    E_SORT_BY_NONE    = 1,
    E_SORT_BY_OBJECT  = 2,
    E_SORT_BY_DEPTH   = 3,
};

extern CObjectManager					m_ObjectManager;
extern int*								g_ObjectListRef;
extern int								g_iObjectListRefCount;
//extern int								g_iObjectListRefLastCount;
extern sObject**						g_ObjectList;
extern int								g_iObjectListCount;
extern bool								g_bGlobalVBIBUsageFlag;
//extern bool								g_bGlobalSortByTextureUsageFlag;
//extern bool								g_bGlobalSortByObjectNumberFlag;
//extern bool								g_bGlobalSortByObjectDepthFlag;
extern eSortOrder                       g_eGlobalSortOrder;
extern sMesh**							g_RawMeshList;
extern int								g_iRawMeshListCount;
extern sVertexShader					m_VertexShaders [ MAX_VERTEX_SHADERS ];
extern sPixelShader						m_PixelShaders  [ MAX_VERTEX_SHADERS ];
extern sEffectItem*						m_EffectList    [ MAX_EFFECTS ];
extern sFullCollisionResult				g_DBPROCollisionResult;

// Shadow System globals
extern D3DXVECTOR3						g_vecShadowPosition;
extern int								g_iShadowPositionFixed;

// Universe and Scene External Globals
extern cLightMaps						g_LightMaps;
extern cUniverse*						g_pUniverse;
extern int								g_iAreaBox;
extern int								g_iAreaBoxCount;

// MIKE 140503 - global timer for animation
extern float   g_fInvTicksPerSec;
extern float   g_fInvTicksPerMs;
extern __int64 g_i64StartTime;
extern __int64 g_i64LastTime;

//////////////////////////////////////////////////////////////////////////////////
// EXTERNAL COMMON DATA FUNCTION-PTRS ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// DLL HANDLES - Not needed largely!!
/*
extern HINSTANCE	g_XObject ;
extern HINSTANCE	g_3DSObject ;
extern HINSTANCE	g_MDLObject;
extern HINSTANCE	g_MD2Object ;
extern HINSTANCE	g_MD3Object ;
extern HINSTANCE	g_PrimObject;
extern HINSTANCE	g_Image     ;
extern HINSTANCE	g_Setup  ;
*/
// MISC TYPEDEFS
typedef void							( *RetVoidFunctionPointerPFN ) ( ... );
typedef int								( *RetIntFunctionPointerPFN ) ( ... );
typedef ID3DXMesh*						( *RetGetMeshDataPFN  )     ( ... );
typedef LPDIRECT3DTEXTURE9*				( *RetGetTextureDataPFN  )  ( ... );
typedef DWORD							( *RetGetNumMaterialsPFN  ) ( ... );
typedef D3DMATERIAL9*					( *RetGetMaterialDataPFN  ) ( ... );
typedef D3DXMATRIX						( *GetMatrixPFN )           ( int );
typedef D3DXVECTOR4						( *GetVectorPFN )           ( int );
typedef D3DXMATRIX						( *GetMatrixDataPFN  ) ( ... );
typedef IDirect3DDevice9*				( *GFX_GetDirect3DDevicePFN )	( void );

// Used to call memblockDLL for memblock return ptr function
typedef int						( *MEMBLOCKS_GetMemblockExist   ) ( int );
typedef DWORD					( *MEMBLOCKS_GetMemblockPtr   ) ( int );
typedef DWORD					( *MEMBLOCKS_GetMemblockSize  ) ( int );
typedef void					( *MEMBLOCKS_MemblockFromMedia ) ( int, int );
typedef void					( *MEMBLOCKS_MediaFromMemblock ) ( int, int );
typedef void					( *MEMBLOCKS_MakeMemblock  ) ( int, int );
typedef void					( *MEMBLOCKS_DeleteMemblock ) ( int );

// canot have more thanone storage class! 081108
extern MEMBLOCKS_GetMemblockExist		g_Memblock_GetMemblockExist;
extern MEMBLOCKS_GetMemblockPtr			g_Memblock_GetMemblockPtr;
extern MEMBLOCKS_GetMemblockSize		g_Memblock_GetMemblockSize;
extern MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromImage;
extern MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromBitmap;
extern MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromSound;
extern MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromMesh;
extern MEMBLOCKS_MediaFromMemblock		g_Memblock_ImageFromMemblock;
extern MEMBLOCKS_MediaFromMemblock		g_Memblock_BitmapFromMemblock;
extern MEMBLOCKS_MediaFromMemblock		g_Memblock_SoundFromMemblock;
extern MEMBLOCKS_MediaFromMemblock		g_Memblock_MeshFromMemblock;
extern MEMBLOCKS_MakeMemblock			g_Memblock_MakeMemblock;
extern MEMBLOCKS_DeleteMemblock			g_Memblock_DeleteMemblock;
//extern DBPRO_GLOBAL MEMBLOCKS_GetMemblockExist			g_Memblock_GetMemblockExist;
//extern DBPRO_GLOBAL MEMBLOCKS_GetMemblockPtr			g_Memblock_GetMemblockPtr;
//extern DBPRO_GLOBAL MEMBLOCKS_GetMemblockSize			g_Memblock_GetMemblockSize;
//extern DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromImage;
//extern DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromBitmap;
//extern DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromSound;
//extern DBPRO_GLOBAL MEMBLOCKS_MemblockFromMedia		g_Memblock_MemblockFromMesh;
//extern DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock		g_Memblock_ImageFromMemblock;
//extern DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock		g_Memblock_BitmapFromMemblock;
//extern DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock		g_Memblock_SoundFromMemblock;
//extern DBPRO_GLOBAL MEMBLOCKS_MediaFromMemblock		g_Memblock_MeshFromMemblock;
//extern DBPRO_GLOBAL MEMBLOCKS_MakeMemblock				g_Memblock_MakeMemblock;
//extern DBPRO_GLOBAL MEMBLOCKS_DeleteMemblock			g_Memblock_DeleteMemblock;

// MISC
extern RetGetMeshDataPFN				g_GetMeshData;
extern RetGetTextureDataPFN				g_GetTextureData;
extern RetGetNumMaterialsPFN			g_GetNumMaterials;
extern RetGetMaterialDataPFN			g_GetMaterialData;
extern GetMatrixDataPFN					g_GetCombined;
extern GetMatrixDataPFN					g_GetTransformed;
extern GetMatrixDataPFN					g_GetOriginal;
extern RetVoidFunctionPointerPFN		UpdateTimer;
extern GetMatrixPFN						g_Types_GetMatrix;
extern GetVectorPFN						g_Types_GetVector;
extern RetVoidFunctionPointerPFN		g_LoadMesh;
extern RetVoidFunctionPointerPFN		g_DeleteMesh;
extern RetVoidFunctionPointerPFN		g_SaveMesh;
extern RetVoidFunctionPointerPFN		g_ChangeMesh;
extern RetVoidFunctionPointerPFN		g_MakeMeshFromObject;
extern RetIntFunctionPointerPFN			g_GetMeshExist;

extern HINSTANCE						g_GFX;							// for dll loading
extern GFX_GetDirect3DDevicePFN			g_GFX_GetDirect3DDevice;	// get pointer to D3D device
extern HINSTANCE						m_hData;
extern LPDIRECT3DDEVICE9				m_pD3D;
extern D3DCAPS9							m_Caps;

typedef void	( *CAMERA3D_SetFOVPFN )				( float );
extern CAMERA3D_SetFOVPFN				g_Camera3D_SetFOV;

//////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR FUNCTIONS /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

/*
#ifndef DARKSDK_COMPILE
	#define DARKSDK __declspec ( dllexport )
	#define DBPRO_GLOBAL 
#else
	#define DARKSDK static
	#define DBPRO_GLOBAL static
#endif
*/

#ifdef DARKSDK_COMPILE
		void ConstructorBasic3D			( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hTypes, HINSTANCE hCamera );
		void SetErrorHandlerBasic3D		( LPVOID pErrorHandlerPtr );
		void PassCoreDataBasic3D		( LPVOID pGlobPtr );
		void RefreshD3DBasic3D			( int iMode );
		void DestructorBasic3D			( void );

		void  dbUpdateBasic3D			( void );

		void  dbUpdateNoZDepth			( void );
		void  dbStencilRenderStart		( void );
		void  dbStencilRenderEnd		( void );
		void  dbAutomaticStart			( void );
		void  dbAutomaticEnd			( void );

		void dbGetMeshData		( int iMeshID, DWORD* pdwFVF, DWORD* pdwFVFSize, DWORD* pdwVertMax, LPSTR* pData, DWORD* dwDataSize, bool bLockData );
		void dbSetMeshData		( int iMeshID, DWORD dwFVF, DWORD dwFVFSize, LPSTR pMeshData, DWORD dwVertMax );
#endif

DARKSDK void Constructor		( HINSTANCE hSetup, HINSTANCE hImage, HINSTANCE hTypes, HINSTANCE hCamera );
DARKSDK void SetErrorHandler	( LPVOID pErrorHandlerPtr );
DARKSDK void PassCoreData		( LPVOID pGlobPtr );
DARKSDK void RefreshD3D			( int iMode );
DARKSDK void Destructor			( void );

//////////////////////////////////////////////////////////////////////////////////
// EXPORTED INTERNAL FUNCTIONS ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void UpdateViewProjForMotionBlur ( void );
DARKSDK void UpdateAnimationCycle ( void );
DARKSDK void UpdateOnce			( void );
DARKSDK void Update				( void );
DARKSDK void UpdateGhostLayer	( void );
DARKSDK void UpdateNoZDepth		( void );
DARKSDK void* GetInternalData	( int iID );
DARKSDK void StencilRenderStart	( void );
DARKSDK void StencilRenderEnd	( void );
DARKSDK void AutomaticStart		( void );
DARKSDK void AutomaticEnd		( void );
DARKSDK void GetMeshData		( int iMeshID, DWORD* pdwFVF, DWORD* pdwFVFSize, DWORD* pdwVertMax, LPSTR* pData, DWORD* dwDataSize, bool bLockData );
DARKSDK void SetMeshData		( int iMeshID, DWORD dwFVF, DWORD dwFVFSize, LPSTR pMeshData, DWORD dwVertMax );


//////////////////////////////////////////////////////////////////////////////////
// Internal Collision-BSP Functions //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK void  ColPosition ( int iID, float fX, float fY, float fZ );
DARKSDK float GetXColPosition ( int iID );
DARKSDK float GetYColPosition ( int iID );
DARKSDK float GetZColPosition ( int iID );

//////////////////////////////////////////////////////////////////////////////////
// DBPRO CORE MANAGEMENT FUNCTIONS ///////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
LPSTR GetReturnStringFromWorkString		( void );
bool  DoesFileExist						( LPSTR pFilename );

//////////////////////////////////////////////////////////////////////////////////
// CHECK FUNCTIONS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
bool ConfirmObjectInstance				( int iID );
bool ConfirmObject						( int iID );
bool CheckObjectExist					( int iID );
bool ConfirmNewObject					( int iID );
bool ConfirmObjectAndImage				( int iID, int iImage );
bool ConfirmObjectAndLimbInstance		( int iID, int iLimbID );
bool ConfirmObjectAndLimb				( int iID, int iLimbID );
bool ConfirmMesh						( int iID );
bool ConfirmNewMesh						( int iID );
bool ConfirmEffect						( int iEffectID );
bool ConfirmNewEffect					( int iEffectID );

//////////////////////////////////////////////////////////////////////////////////
// OBJECT MANAGEMENT FUNCTIONS ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void  ResizeObjectList					( int iSize );
void  AddObjectToObjectListRef			( int iID );
void  RemoveObjectFromObjectListRef		( int iID );
void  ResizeRawMeshList					( int iSize );
float wrapangleoffset					( float da );
void  GetAngleFromPoint					( float x1, float y1, float z1, float x2, float y2, float z2, float* ax, float* ay, float* az );
void  RegenerateLookVectors				( sObject* pObject );
void  AnglesFromMatrix					( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles );
void  CheckRotationConversion			( sObject* pObject, bool bUseFreeFlightMode );
void  UpdateOverlayFlag					( sObject* pObject );
void  SetObjectTransparency				( sObject* pObject, int iTransparency );

void  NewObjectAutoCam					( float fRadius );
void  SetupModelFunctionPointers		( void* pModel, HINSTANCE hDLL );
void  SetupPrimitiveFunctionPointers	( void* pData );
void  SetupDefaultProperties			( void* pData );
void  SetupDefaultPosition				( int iID );
bool  PrepareCustomObject				( void* m_pData );
void  GetCullDataFromModel				( int iID );
bool  CreateModelFromCustom				( int iID, void* m_pData );

DARKSDK bool    SetNewObjectFinalProperties		( int iID, float fRadius );
DARKSDK bool    CreateNewObject					( int iID, LPSTR pName );
DARKSDK bool    DeleteObject					( int iID );
DARKSDK bool    CreateNewObject                 ( int iID, LPSTR pName, int iFrame );
DARKSDK sFrame* CreateNewFrame                  ( sObject* pObject, LPSTR pName, bool bNewMesh );

//////////////////////////////////////////////////////////////////////////////////
// TEXTURE/STATE/SHADER FUNCTIONS ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
DWORD VectortoRGBA								( D3DXVECTOR3* v, FLOAT fHeight );
bool DoesDepthBufferHaveStencil					( D3DFORMAT d3dfmt );
BOOL SupportsStencilBuffer						();
BOOL SupportsUserClippingPlane					();
BOOL SupportsEnvironmentBumpMapping				();
BOOL SupportsCubeMapping						();
BOOL SupportsDotProduct3BumpMapping				();
int GetBitDepthFromFormat						( D3DFORMAT Format );
void CreateImageNormalMap						( int iImageIndex );
LPDIRECT3DCUBETEXTURE9 CreateNewImageCubeMap	( int i1, int i2, int i3, int i4, int i5, int i6 );
void SetVertexShaderMesh						( int iID, DWORD dwFVFOverride );
void FreeVertexShaderMesh						( int iID ) ;
void CleatTextureStageFlags						( void );
void ClearTextureSettings						( int iID );
void CreateNewOrSharedEffect					( sMesh* pMesh, bool bChangeMesh );

#endif _COMMONC_H_
