#include "cenhancedxc.h"
#include ".\cerror.h"
#include ".\cdatac.h"

#define DEBUG_MODE	0




#define ANIM_SIMPLE 0
#define ANIM_LOOP   1
#define ANIM_CYCLE  2

#define ANIM_DIR_FORWARD 0
#define ANIM_DIR_BACK	 1
#define ANIM_DIR_END	 2

//////////////////////////////////////////////////////////////////////////////
// variable declarations /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
tagModelData*		m_pModelData;

CData				m_List;
CData				m_MeshList;
float				m_fTimer;
LPDIRECT3DDEVICE8	m_pD3D;
D3DXMATRIX*			m_matObject;
char				m_szPath [ 256 ];

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// function pointer types ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef void	          ( *RetVoidFunctionPointerPFN ) ( ... );
typedef bool	          ( *RetBoolFunctionPointerPFN ) ( ... );
typedef int 	          ( *RetIntFunctionPointerPFN  ) ( ... );
typedef char		      ( *RetCharFunctionPointerPFN ) ( ... );
typedef IDirect3DDevice8* ( *RetD3DFunctionPointerPFN  ) ( ... );
typedef LPDIRECT3DTEXTURE8	( *IMAGE_RetLPD3DTEX8ParamInt3PFN )  ( int, int, int );
typedef LPDIRECT3DTEXTURE8	( *IMAGE_RetLPD3DTEX8ParamIntPFN )  ( int );
typedef bool				( *IMAGE_LoadDirectPFN )  ( char* szFilename, LPDIRECT3DTEXTURE8* pImage );

///////////////////////////////////////////////////////////////////////////////////////////////
/// image /////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
/*
HINSTANCE						g_Image;

RetVoidFunctionPointerPFN		g_Image_Constructor;
RetVoidFunctionPointerPFN		g_Image_Destructor;

RetBoolFunctionPointerPFN		g_Image_Load;
IMAGE_LoadDirectPFN				g_Image_LoadDirect;

RetBoolFunctionPointerPFN		g_Image_Save;
RetVoidFunctionPointerPFN		g_Image_Delete;
IMAGE_RetLPD3DTEX8ParamInt3PFN	g_Image_Make;

RetIntFunctionPointerPFN		g_Image_GetWidth;
RetIntFunctionPointerPFN		g_Image_GetHeight;
RetBoolFunctionPointerPFN		g_Image_GetExist;

RetVoidFunctionPointerPFN		g_Image_SetSharing;
RetVoidFunctionPointerPFN		g_Image_SetMemory;

RetVoidFunctionPointerPFN		g_Image_Lock;
RetVoidFunctionPointerPFN		g_Image_Unlock;
RetVoidFunctionPointerPFN		g_Image_Write;
RetVoidFunctionPointerPFN		g_Image_Get;

RetVoidFunctionPointerPFN		g_Image_SetMipmapMode;
RetVoidFunctionPointerPFN		g_Image_SetMipmapType;
RetVoidFunctionPointerPFN		g_Image_SetMipmapBias;
RetVoidFunctionPointerPFN		g_Image_SetMipmapNum;

RetVoidFunctionPointerPFN		g_Image_SetColorKey;
RetVoidFunctionPointerPFN		g_Image_SetTranslucency;

IMAGE_RetLPD3DTEX8ParamIntPFN	g_Image_GetPointer;					// get pointer to image data, useful to external apps
*/
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/*
typedef IDirect3DDevice8* ( *GFX_GetDirect3DDevicePFN ) ( void );

HINSTANCE					g_GFX;							// for dll loading

GFX_GetDirect3DDevicePFN	g_GFX_GetDirect3DDevice;	// get pointer to D3D device
*/
//////////////////////////////////////////////////////////////////////////

int							m_iCurrentID;
D3DMATERIAL8				gWhiteMaterial;

void Update ( int iID, D3DXMATRIX* matrix )
{
	m_pModelData              = ( tagModelData* ) m_List.Get ( iID );
	if(m_pModelData) m_pModelData->m_matObject = matrix;
}

void Constructor ( LPDIRECT3DDEVICE8 lpDevice )
{
	m_pD3D = lpDevice;

	/*
	g_GFX = hSetup;
	g_Image = hImage;
	if ( !hSetup || !hImage )
	{
		hSetup = LoadLibrary ( "DBProSetupDebug.dll" );		// load the setup library
		hImage = LoadLibrary ( "DBProImageDebug.dll" );		// load the image library
	}

	g_Image_Constructor          = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Constructor@@YAXPAUHINSTANCE__@@@Z" );
	g_Image_Destructor           = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Destructor@@YAXXZ" );

	g_Image_Load                 = ( RetBoolFunctionPointerPFN )		GetProcAddress ( hImage, "?Load@@YA_NPADH@Z" );
	g_Image_LoadDirect           = ( IMAGE_LoadDirectPFN )				GetProcAddress ( hImage, "?LoadDirect@@YA_NPADPAPAUIDirect3DTexture8@@@Z" );

	g_Image_Save                 = ( RetBoolFunctionPointerPFN )		GetProcAddress ( hImage, "?Save@@YA_NHPAD@Z" );
	g_Image_Delete               = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Delete@@YAXH@Z" );
	g_Image_Make                 = ( IMAGE_RetLPD3DTEX8ParamInt3PFN )	GetProcAddress ( hImage, "?Make@@YAPAUIDirect3DTexture8@@HHH@Z" );
	
	g_Image_GetWidth             = ( RetIntFunctionPointerPFN )			GetProcAddress ( hImage, "?GetWidth@@YAHH@Z" );
	g_Image_GetHeight            = ( RetIntFunctionPointerPFN )			GetProcAddress ( hImage, "?GetHeight@@YAHH@Z" );
	g_Image_GetExist             = ( RetBoolFunctionPointerPFN )		GetProcAddress ( hImage, "?GetExist@@YA_NH@Z" );

	g_Image_GetPointer           = ( IMAGE_RetLPD3DTEX8ParamIntPFN )	GetProcAddress ( hImage, "?GetPointer@@YAPAUIDirect3DTexture8@@H@Z" );

	g_Image_SetSharing           = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetSharing@@YAX_N@Z" );
	g_Image_SetMemory            = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMemory@@YAXH@Z" );

	g_Image_Lock                 = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Lock@@YAXH@Z" );
	g_Image_Unlock               = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Unlock@@YAXH@Z" );
	g_Image_Write                = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Write@@YAXHHHHHHH@Z" );
	g_Image_Get                  = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?Get@@YAXHHHPAH00@Z" );

	g_Image_SetMipmapMode        = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMipmapMode@@YAX_N@Z" );
	g_Image_SetMipmapType        = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMipmapType@@YAXH@Z" );
	g_Image_SetMipmapBias        = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMipmapBias@@YAXM@Z" );
	g_Image_SetMipmapNum         = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetMipmapNum@@YAXH@Z" );
	
	g_Image_SetColorKey          = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetColorKey@@YAXHHH@Z" );
	g_Image_SetTranslucency      = ( RetVoidFunctionPointerPFN )		GetProcAddress ( hImage, "?SetTranslucency@@YAXHH@Z" );

	g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice8@@XZ" );
	m_pD3D                  = g_GFX_GetDirect3DDevice ( );
	*/

	memset ( m_szPath, 0, sizeof ( m_szPath ) );
	m_fTimer = 0.0f;

	// Prepare a pure white material for texture usage
	memset ( &gWhiteMaterial, 0, sizeof ( gWhiteMaterial ) );
	gWhiteMaterial.Diffuse.r = 1.0f;
	gWhiteMaterial.Diffuse.g = 1.0f;
	gWhiteMaterial.Diffuse.b = 1.0f;
	gWhiteMaterial.Diffuse.a = 1.0f;	
	gWhiteMaterial.Ambient.r = 1.0f;
	gWhiteMaterial.Ambient.g = 1.0f;
	gWhiteMaterial.Ambient.b = 1.0f;
	gWhiteMaterial.Ambient.a = 1.0f;
}

void Destructor ( void )
{
	// get rid of any meshes
	link* pCheck = m_MeshList.m_start;
	while(pCheck)
	{
		tagModelData* ptr = NULL;
		ptr = ( tagModelData* ) m_MeshList.Get ( pCheck->id );
		if ( ptr == NULL ) continue;

		SAFE_DELETE ( ptr->m_Object.m_Frames );
		SAFE_DELETE ( ptr->m_Object.m_Meshes );
		SAFE_DELETE ( ptr );

		pCheck = pCheck->next;
	}
	m_MeshList.DeleteAll ( );

	// get rid of any models
	pCheck = m_List.m_start;
	while(pCheck)
	{
		tagModelData* ptr = NULL;
		ptr = ( tagModelData* ) m_List.Get ( pCheck->id );
		if ( ptr == NULL ) continue;

		SAFE_DELETE ( ptr->m_Object.m_Frames );
		SAFE_DELETE ( ptr->m_Object.m_Meshes );
		SAFE_DELETE ( ptr );

		pCheck = pCheck->next;
	}
	m_List.DeleteAll ( );
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
//		Constructor ( g_GFX, g_Image );
	}
}

void LoadMesh ( int iMeshID, char* szFilename )
{
	// Model Number Zero used as temp loading area (for mesh conversion)
	int iTempModelNumber=0;

	// variable definitions
	int iCharCount = 0;
	tagModelData modelData;
	memset ( &modelData, 0, sizeof ( modelData ) );
	for ( int iChar = strlen ( szFilename ); iChar > 0; iChar--, iCharCount++ )
	{
		if ( szFilename [ iChar ] == '\\' || szFilename [ iChar ] == '/' )
		{
			memcpy ( modelData.m_szPath, szFilename, sizeof ( char ) * ( strlen ( szFilename ) - iCharCount + 1 ) );
			break;
		}
	}

	m_pModelData = &modelData;

	///////////////////////////////////////
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	tagModelData* ptr = NULL;
	ptr = ( tagModelData* ) m_List.Get ( iTempModelNumber );
	if ( ptr != NULL )
	{
		m_List.Delete ( iTempModelNumber );
	}

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	tagModelData* test;
	test = new tagModelData;
	memset ( test,          0, sizeof ( tagModelData ) );
	memcpy ( test, &modelData, sizeof ( tagModelData ) );
	m_List.Add ( iTempModelNumber, ( VOID* ) test, 0, 1 );
	///////////////////////////////////////

	// update internal model data
	m_pModelData = ( tagModelData* ) m_List.Get ( iTempModelNumber );
	if ( LoadModelData ( &m_pModelData->m_Object, szFilename, modelData.m_szPath, false ) )
	{
		// setup animation properties
		m_pModelData->m_bAnimPlaying = false;															// anim flag, off by default
		m_pModelData->m_fAnimSpeed   = 1.0f;																// current speed
		m_pModelData->m_dwLastTime   = timeGetTime ( );													// initial time
		m_pModelData->m_dwThisTime   = timeGetTime ( );													// initial time
		m_pModelData->m_fFrame       = 0.0f;																// current frame
		m_pModelData->m_iFrameJump   = 1;
		m_pModelData->m_iAnimDirection = ANIM_DIR_FORWARD;
		m_pModelData->m_iAnimMode = ANIM_SIMPLE;

		// when model loaded, convert to single mesh
		MakeMeshFromObject ( iMeshID, iTempModelNumber );

		// delete model data
		SAFE_DELETE ( test->m_Object.m_Frames );
		SAFE_DELETE ( test->m_Object.m_Meshes );
		SAFE_DELETE ( test );
		m_List.Delete ( iTempModelNumber );

		return;
	}
	else
	{
		if ( test != NULL )
		{
			SAFE_DELETE ( test->m_Object.m_Frames );
			SAFE_DELETE ( test->m_Object.m_Meshes );
			SAFE_DELETE ( test );
			m_List.Delete ( iTempModelNumber );
		}

		return;
	}
}

void DeleteMesh ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_MeshList.Get ( iID ) ) )
		return;

	SAFE_DELETE ( m_pModelData->m_Object.m_Frames );
	SAFE_DELETE ( m_pModelData->m_Object.m_Meshes );
	SAFE_DELETE ( m_pModelData );
	m_MeshList.Delete ( iID );
}

void SaveMesh ( int iMeshID, char* szFilename )
{
	// get mesh object
	if ( ! ( m_pModelData = ( tagModelData* ) m_MeshList.Get ( iMeshID ) ) )
		return;

	// get mesh info
	sMesh* pMesh = m_pModelData->m_Object.m_Meshes;

	// create D£DXMATERIAL array
	D3DXMATERIAL* pMatList = new D3DXMATERIAL[pMesh->m_NumMaterials];
	for(DWORD m=0; m<pMesh->m_NumMaterials; m++)
	{
		pMatList[m].MatD3D=pMesh->m_Materials[m];
		pMatList[m].pTextureFilename=NULL;
	}

	// save mesh to an X file
	HRESULT hRes = D3DXSaveMeshToX( szFilename, pMesh->m_Mesh, NULL, pMatList, pMesh->m_NumMaterials, DXFILEFORMAT_TEXT );

	// free usages
	SAFE_DELETE(pMatList);
}

void ChangeMesh	( int iObjectID, int iLimbID, int iMeshID )
{
	// variable declarations
	tagModelData* pObject		= NULL;	// source object
	tagModelData* pMeshObj		= NULL;	// mesh object
	sMesh* pActualMesh			= NULL;	// mesh data

	// get source
	if ( ! ( pObject = ( tagModelData* ) m_List.Get ( iObjectID ) ) )
		return;

	// get mesh object
	if ( ! ( pMeshObj = ( tagModelData* ) m_MeshList.Get ( iMeshID ) ) )
		return;

	// get first mesh from mesh object
	if ( ! ( pActualMesh = pMeshObj->m_Object.m_Meshes ) )
		return;

	// use objects FVF to control mesh creation
	DWORD dwInFVF = pObject->m_Object.m_dwFVF;
	DWORD dwInFVFSize = pObject->m_Object.m_dwFVFSize;

	// determine info on sourcemesh
	DWORD dwInNumPoly=pActualMesh->m_pAttributeTable [ 0 ].FaceCount;
	DWORD dwInNumVert=pActualMesh->m_pAttributeTable [ 0 ].VertexCount;

	// clone sourcemesh
	ID3DXMesh* pNewMesh;
	pActualMesh->m_Mesh->CloneMeshFVF ( 0, dwInFVF, m_pD3D, &pNewMesh );

	// create area for tempmeshdata
	float* pDataFromNewMesh;
	float* pTempMeshDataRaw = (float*)new char[dwInNumVert*dwInFVFSize];
	if ( SUCCEEDED ( pNewMesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &pDataFromNewMesh ) ) )
	{
		memcpy(pTempMeshDataRaw, pDataFromNewMesh, dwInFVFSize*dwInNumVert);
		pNewMesh->UnlockVertexBuffer ( );
	}
	WORD* pIndiceFromNewMesh;
	WORD* pTempIndiceData = new WORD[dwInNumPoly*3];
	if ( SUCCEEDED ( pNewMesh->LockIndexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &pIndiceFromNewMesh ) ) )
	{
		memcpy(pTempIndiceData, pIndiceFromNewMesh, dwInNumPoly*3*sizeof(WORD));
		pNewMesh->UnlockIndexBuffer ( );
	}

	// Make mesh data not depend on indices
	DWORD dwNewVertCount=0;
	float* pTempMeshData = CreatePureTriangleMeshData( pTempMeshDataRaw, &dwNewVertCount, dwInNumVert, dwInFVFSize, pTempIndiceData, dwInNumPoly );

	// free temps so far
	SAFE_DELETE(pTempMeshDataRaw);
	SAFE_DELETE(pTempIndiceData);

	// find frame(limb) holdnig mesh(es)
	sFrame* pLimbFrame = pObject->m_Object.m_Frames->FindFrame ( iLimbID );

	// delete the mesh in the object
	sMesh* pStoreNeighborMesh=NULL;
	sFrameMeshList* List = pLimbFrame->m_MeshList;
	while ( List != NULL )
	{
		// Next List Item
		sFrameMeshList* NextList = List->m_Next;

		// Delete mesh(es)
		if ( List->m_Mesh )
		{
			pStoreNeighborMesh = List->m_Mesh->m_Next;
			List->m_Mesh->m_Next=NULL;
			delete List->m_Mesh;
		}

		// Delete framelist item
		if ( List->m_Next )
		{
			List->m_Next=NULL;
			delete List;
		}

		// next mesh in list
		List = NextList;
	}

	// create a new mesh
	sMesh* Mesh = MakeMeshFromData ( dwInFVF, dwInFVFSize, pTempMeshData, dwNewVertCount, D3DPT_TRIANGLELIST );

	// restore neighbor mesh
	Mesh->m_Next=pStoreNeighborMesh;

	// copy mesh data from mesh item to new object mesh
	sMesh* pReplacingMesh = pLimbFrame->m_MeshList->m_Mesh;
	pLimbFrame->m_MeshList->m_Mesh = Mesh;
	pLimbFrame->m_MeshList->m_Next = NULL;

	// replace reference to old mesh if found in meshlist
	if(pObject->m_Object.m_Meshes==pReplacingMesh)
		pObject->m_Object.m_Meshes=Mesh;
	else
	{
		sMesh* pCurrent = pObject->m_Object.m_Meshes;
		while(pCurrent)
		{
			sMesh* pNextMesh = pCurrent->m_Next;
			if(pNextMesh==pReplacingMesh)
			{
				pCurrent->m_Next = Mesh;
				break;
			}
			pCurrent=pNextMesh;
		}
	}

	// Free usages
	SAFE_DELETE(pTempMeshData);
	SAFE_RELEASE(pNewMesh);
}

bool MakeFromMesh (		int iID, DWORD dwInFVF, DWORD dwInFVFSize,
						float* pInMesh, DWORD dwInNumPoly, DWORD dwInNumVert, DWORD dwInPrimType )
{
	///////////////////////////////////////
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	tagModelData* ptr = NULL;
	ptr = ( tagModelData* ) m_List.Get ( iID );
	if ( ptr != NULL )
	{
		SAFE_DELETE ( ptr->m_Object.m_Frames );
		SAFE_DELETE ( ptr->m_Object.m_Meshes );
		SAFE_DELETE ( ptr );
		m_List.Delete ( iID );
	}
	///////////////////////////////////////

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	tagModelData* test;
	test = new tagModelData;
	memset ( test,          0, sizeof ( tagModelData ) );
	m_List.Add ( iID, ( VOID* ) test, 0, 1 );
	///////////////////////////////////////

	// update internal model data
	m_pModelData = ( tagModelData* ) m_List.Get ( iID );
	if ( MakeModelData ( &m_pModelData->m_Object, dwInFVF, dwInFVFSize, pInMesh, dwInNumPoly, dwInNumVert, dwInPrimType ) )
	{
		// setup animation properties
		m_pModelData->m_bAnimPlaying = false;															// anim flag, off by default
		m_pModelData->m_fAnimSpeed   = 0.0f;																// current speed
		m_pModelData->m_dwLastTime   = timeGetTime ( );													// initial time
		m_pModelData->m_dwThisTime   = timeGetTime ( );													// initial time
		m_pModelData->m_fFrame       = 0.0f;																// current frame
		m_pModelData->m_iFrameJump   = 0;
		m_pModelData->m_iAnimDirection = ANIM_DIR_FORWARD;
		m_pModelData->m_iAnimMode = ANIM_SIMPLE;
		
		// Complete
		return true;
	}
	else
	{
		// Failed to load, remove from list
		if ( test != NULL )
		{
			SAFE_DELETE ( test->m_Object.m_Frames );
			SAFE_DELETE ( test->m_Object.m_Meshes );
			SAFE_DELETE ( test );
			m_List.Delete ( iID );
		}

		// Failed
		return false;
	}
}

void NewObjectFromMesh ( int iObjectID, int iMeshID )
{
	// variable declarations
	tagModelData* pMeshObj		= NULL;	// mesh object
	sMesh* pActualMesh			= NULL;	// mesh data

	// get mesh object
	if ( ! ( pMeshObj = ( tagModelData* ) m_MeshList.Get ( iMeshID ) ) )
		return;

	// get first mesh from mesh object
	if ( ! ( pActualMesh = pMeshObj->m_Object.m_Meshes ) )
		return;
	
	// use mesh FVF to control objectmesh creation
	DWORD dwInFVF = pMeshObj->m_Object.m_dwFVF;
	DWORD dwInFVFSize = pMeshObj->m_Object.m_dwFVFSize;
	DWORD dwInPrimType = pActualMesh->m_iPrimType;

	// determine info on sourcemesh
	DWORD dwInNumPoly=pActualMesh->m_pAttributeTable [ 0 ].FaceCount;
	DWORD dwInNumVert=pActualMesh->m_pAttributeTable [ 0 ].VertexCount;

	// create copy of meshdata
	float* pDataFromNewMesh;
	float* pInMesh = (float*)new char[dwInNumVert*dwInFVFSize];
	if ( SUCCEEDED ( pActualMesh->m_Mesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &pDataFromNewMesh ) ) )
	{
		memcpy(pInMesh, pDataFromNewMesh, dwInNumVert*dwInFVFSize);
		pActualMesh->m_Mesh->UnlockVertexBuffer ( );
	}

	// make object from meshdata
	MakeFromMesh ( iObjectID, dwInFVF, dwInFVFSize, pInMesh, dwInNumPoly, dwInNumVert, dwInPrimType );

	// free usage
	SAFE_DELETE(pInMesh);
}

bool MakeMeshFromMesh (	int iID, DWORD dwInFVF, DWORD dwInFVFSize,
						float* pInMesh, DWORD dwInNumPoly, DWORD dwInNumVert, DWORD dwInPrimType )
{
	///////////////////////////////////////
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	tagModelData* ptr = NULL;
	ptr = ( tagModelData* ) m_MeshList.Get ( iID );
	if ( ptr != NULL )
	{
		SAFE_DELETE ( ptr->m_Object.m_Frames );
		SAFE_DELETE ( ptr->m_Object.m_Meshes );
		SAFE_DELETE ( ptr );
		m_MeshList.Delete ( iID );
	}
	///////////////////////////////////////

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	tagModelData* test;
	test = new tagModelData;
	memset ( test, 0, sizeof ( tagModelData ) );
	m_MeshList.Add ( iID, ( VOID* ) test, 0, 1 );
	///////////////////////////////////////

	// update internal model data
	m_pModelData = ( tagModelData* ) m_MeshList.Get ( iID );
	if ( MakeModelData ( &m_pModelData->m_Object, dwInFVF, dwInFVFSize, pInMesh, dwInNumPoly, dwInNumVert, dwInPrimType ) )
	{
		// setup animation properties
		m_pModelData->m_bAnimPlaying = false;															// anim flag, off by default
		m_pModelData->m_fAnimSpeed   = 1.0f;																// current speed
		m_pModelData->m_dwLastTime   = timeGetTime ( );													// initial time
		m_pModelData->m_dwThisTime   = timeGetTime ( );													// initial time
		m_pModelData->m_fFrame       = 0.0f;																// current frame
		m_pModelData->m_iFrameJump   = 1;
		m_pModelData->m_iAnimDirection = ANIM_DIR_FORWARD;
		m_pModelData->m_iAnimMode = ANIM_SIMPLE;
		
		// Complete
		return true;
	}
	else
	{
		// Failed to load, remove from list
		if ( test != NULL )
		{
			SAFE_DELETE ( test->m_Object.m_Frames );
			SAFE_DELETE ( test->m_Object.m_Meshes );
			SAFE_DELETE ( test );
			m_MeshList.Delete ( iID );
		}

		// Failed
		return false;
	}
}

void MakeMeshFromObjectPart ( int iMeshID, int iObjectID, int iLimbPart )
{
	// makes a mesh from an object
	tagModelData* pObject = NULL;
	if ( ! ( pObject = ( tagModelData* ) m_List.Get ( iObjectID ) ) )
		return;

	// use objects FVF to control mesh creation
	DWORD dwFVF = pObject->m_Object.m_dwFVF;
	DWORD dwFVFSize = pObject->m_Object.m_dwFVFSize;

	// prepare temp mesh data array
	DWORD dwVertMax = 512;
	DWORD dwVertsSoFar = 0;
	float* pTempMeshData = (float*)new char[dwVertMax*dwFVFSize];
	float* pMeshDataPtr = pTempMeshData;

	// prepare temp indice data array
	DWORD dwPolyMax = 512;
	DWORD dwPolysSoFar = 0;
	WORD* pTempIndiceData = new WORD[dwPolyMax*3];
	WORD* pIndiceDataPtr = pTempIndiceData;

	// retain scale and rotation, but not position of main object
	bool bLocalMatrix=false;
	D3DXMATRIX RecordMatrix;
	if(*pObject->m_matObject)
	{
		RecordMatrix = *pObject->m_matObject;
		(*pObject->m_matObject)._41=0.0f;
		(*pObject->m_matObject)._42=0.0f;
		(*pObject->m_matObject)._43=0.0f;
	}
	else
	{
		D3DXMatrixIdentity(&RecordMatrix);
		pObject->m_matObject=&RecordMatrix;
		bLocalMatrix=true;
	}

	// convert and add all meshes to single mesh (as though drawing it)
	D3DXMATRIX Matrix;
	D3DXMatrixIdentity ( &Matrix );
	UpdateAnimation ( &pObject->m_Object, pObject->m_fFrame, TRUE );
	UpdateFrame ( &pObject->m_Object, pObject->m_Object.m_Frames, &Matrix );
	pObject->m_Object.m_Meshes->CopyFrameToBoneMatrices ( );
	
	// build mesh - all frame meshes
	pObject->m_Object.m_matObject = pObject->m_matObject;
	BuildMesh ( &pObject->m_Object, pObject->m_Object.m_Frames, dwFVF, dwFVFSize,
				&dwVertMax, &pTempMeshData, &pMeshDataPtr, &dwVertsSoFar,
				&dwPolyMax, &pTempIndiceData, &pIndiceDataPtr, &dwPolysSoFar, iLimbPart );

	// restore object world position
	if(bLocalMatrix)
		pObject->m_matObject = NULL;
	else
		*pObject->m_matObject = RecordMatrix;

	// make mesh data not depend on indices
	DWORD dwNewVertCount=0;
	float* pNoIndexRequiredMesh = CreatePureTriangleMeshData( pTempMeshData, &dwNewVertCount, dwVertsSoFar, dwFVFSize, pTempIndiceData, dwPolysSoFar );
	SAFE_DELETE(pTempMeshData);

	// create new mesh-object from single mesh
	MakeMeshFromMesh ( iMeshID, dwFVF, dwFVFSize, pNoIndexRequiredMesh, dwNewVertCount/3, dwNewVertCount, D3DPT_TRIANGLELIST );

	// free usages
	SAFE_DELETE(pTempMeshData);
	SAFE_DELETE(pTempIndiceData);
	SAFE_DELETE(pNoIndexRequiredMesh);
}

void NewObjectFromLimb ( int iNewObjectID, int iSrcObjectID, int iLimbID )
{
	// create temp mesh from limb
	int iTempMesh=0;
	MakeMeshFromObjectPart ( iTempMesh, iSrcObjectID, iLimbID );

	// create object from mesh
	NewObjectFromMesh ( iNewObjectID, iTempMesh );

	// free mesh
	tagModelData* ptr = NULL;
	ptr = ( tagModelData* ) m_MeshList.Get ( iTempMesh );
	if ( ptr != NULL )
	{
		SAFE_DELETE ( ptr->m_Object.m_Frames );
		SAFE_DELETE ( ptr->m_Object.m_Meshes );
		SAFE_DELETE ( ptr );
	}
	m_MeshList.Delete ( iTempMesh );
}

void MakeMeshFromObject ( int iMeshID, int iObjectID )
{
	int iNoPart=-1;
	MakeMeshFromObjectPart( iMeshID, iObjectID, iNoPart );
}

int GetMeshExist ( int iID )
{	
	if ( ( m_pModelData = ( tagModelData* ) m_MeshList.Get ( iID ) ) )
		return 1;
	else
		return 0;
}

tagModelData* Load ( int iID, char* szFilename )
{
	// variable definitions
	tagModelData			modelData;
	memset ( &modelData, 0, sizeof ( modelData ) );

	int iCharCount = 0;
	for ( int iChar = strlen ( szFilename ); iChar > 0; iChar--, iCharCount++ )
	{
		// run through the characters in the filename and find
		// the first occurrence of a slash, then we know we have
		// hit the part of the filename we're looking for - the path
		if ( szFilename [ iChar ] == '\\' || szFilename [ iChar ] == '/' )
		{
			memcpy ( modelData.m_szPath, szFilename, sizeof ( char ) * ( strlen ( szFilename ) - iCharCount + 1 ) );
			break;
		}
	}

	modelData.lpTexture = NULL;
	m_pModelData = &modelData;

	///////////////////////////////////////
	// check if an object already exists //
	// with the same id, if it does then //
	// delete it                         //
	///////////////////////////////////////
	tagModelData* ptr = NULL;
	ptr = ( tagModelData* ) m_List.Get ( iID );
	if ( ptr != NULL )
	{
		SAFE_DELETE ( ptr->m_Object.m_Frames );
		SAFE_DELETE ( ptr->m_Object.m_Meshes );
		SAFE_DELETE ( ptr );
		m_List.Delete ( iID );
	}

	///////////////////////////////////////

	///////////////////////////////////////
	// create a new object and insert in //
	// the list                          //
	///////////////////////////////////////
	tagModelData* test;
	test = new tagModelData;
	memset ( test,          0, sizeof ( tagModelData ) );
	memcpy ( test, &modelData, sizeof ( tagModelData ) );
	m_List.Add ( iID, ( VOID* ) test, 0, 1 );
	///////////////////////////////////////

	// update internal model data
	m_pModelData = ( tagModelData* ) m_List.Get ( iID );
	if ( LoadModelData ( &m_pModelData->m_Object, szFilename, modelData.m_szPath, true ) )
	{
		// setup animation properties
		m_pModelData->m_bAnimPlaying = false;															// anim flag, off by default
		m_pModelData->m_fAnimSpeed   = 1.0f;																// current speed
		m_pModelData->m_dwLastTime   = timeGetTime ( );													// initial time
		m_pModelData->m_dwThisTime   = timeGetTime ( );													// initial time
		m_pModelData->m_fFrame       = 0.0f;																// current frame
		m_pModelData->m_iFrameJump   = 1;
		m_pModelData->m_iAnimDirection = ANIM_DIR_FORWARD;

		m_pModelData->m_Object.m_bOverrideVertexShader = false;
		m_pModelData->m_Object.m_dwVertexShader		   = 0;

		if ( m_pModelData->m_Object.m_Animations.m_AnimationSet )
		{
			m_pModelData->m_iFrameCount = m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Length;	// number of frames
			m_pModelData->m_iEndFrame   = m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Length;
			m_pModelData->m_iStartFrame = 0;
		}

		m_pModelData->m_iAnimMode = ANIM_SIMPLE;

		/////////////////////////////////////////////////
		/////////////////////////////////////////////////

		// we now need to work out bounding boxes for all of the limbs
		sMesh* pMesh = m_pModelData->m_Object.m_Meshes;

		for ( int iTemp = 0; iTemp < m_pModelData->m_Object.m_NumMeshes; iTemp++ )
		{
			BYTE* pVertices;

			pMesh->m_Mesh->LockVertexBuffer ( D3DLOCK_READONLY, &pVertices );

			D3DXComputeBoundingBox ( 
										pVertices,
										pMesh->m_Mesh->GetNumVertices ( ),
										pMesh->m_Mesh->GetFVF ( ),
										&pMesh->m_vecMin,
										&pMesh->m_vecMax
								   );

			pMesh->m_Mesh->UnlockVertexBuffer ( );

			#if DEBUG_MODE_MESH
			if ( pMesh->m_pMeshBox )
			{
				// only create a box model if debug mode is on
				D3DXMATRIX matTrans;
				D3DXMATRIX matScale;

				D3DXMatrixTranslation (
											&matTrans,
											( pMesh->m_vecMin.x + pMesh->m_vecMax.x ) / 2,
											( pMesh->m_vecMin.y + pMesh->m_vecMax.y ) / 2,
											( pMesh->m_vecMin.z + pMesh->m_vecMax.z ) / 2
									  );

				D3DXMatrixScaling (
										&matScale,
										pMesh->m_vecMax.x - pMesh->m_vecMin.x,
										pMesh->m_vecMax.y - pMesh->m_vecMin.y,
										pMesh->m_vecMax.z - pMesh->m_vecMin.z
								  );

				D3DXCreateBox ( m_pD3D, 1.0f, 1.0f, 1.0f, &pMesh->m_pMeshBox, NULL );
				D3DXComputeNormals ( pMesh->m_pMeshBox, NULL );

				pMesh->m_matBox = matScale * matTrans;
			}
			#endif

			// next mesh
			pMesh = pMesh->m_Next;
		}
		
		// this is used to create the main bounding box, it's a bad way of doing it,
		// as it loads in the model again, every time I try and work it out manually the
		// bounding box isn't in the right position, or completely screws up, it's really
		// annoying me now............ it should be really easy to do aswell.....
		LPD3DXBUFFER	pD3DXMtrlBuffer;
		DWORD			dwNumMaterials;
		LPD3DXMESH		pTempMesh;
		BYTE*			pVertices;

		D3DXLoadMeshFromX ( szFilename, D3DXMESH_SYSTEMMEM, m_pD3D, NULL, &pD3DXMtrlBuffer, &dwNumMaterials, &pTempMesh );

		pTempMesh->LockVertexBuffer ( D3DLOCK_READONLY, &pVertices );
        
		D3DXComputeBoundingBox ( 
									pVertices,
									pTempMesh->GetNumVertices ( ),
									pTempMesh->GetFVF ( ),
									&m_pModelData->m_Object.m_Min,
									&m_pModelData->m_Object.m_Max
							   );
    
		pTempMesh->UnlockVertexBuffer ( );

		SAFE_RELEASE ( pTempMesh );
		SAFE_RELEASE ( pD3DXMtrlBuffer );
		
		/////////////////////////////////////////////////
	

		
		#if DEBUG_MODE
		if ( m_pModelData->m_pMeshBox )
		{
			D3DXMATRIX matTrans;
			D3DXMATRIX matScale;

			D3DXMatrixTranslation (
										&matTrans,
										( m_pModelData->m_Object.m_Min.x + m_pModelData->m_Object.m_Max.x ) / 2, 
										( m_pModelData->m_Object.m_Min.y + m_pModelData->m_Object.m_Max.y ) / 2, 
										( m_pModelData->m_Object.m_Min.z + m_pModelData->m_Object.m_Max.z ) / 2 
								);

			D3DXMatrixScaling (
								&matScale,
								m_pModelData->m_Object.m_Max.x - m_pModelData->m_Object.m_Min.x,
								m_pModelData->m_Object.m_Max.y - m_pModelData->m_Object.m_Min.y,
								m_pModelData->m_Object.m_Max.z - m_pModelData->m_Object.m_Min.z
							  );

			m_pModelData->m_matBox = matScale * matTrans;

			D3DXCreateBox ( m_pD3D, 1.0f, 1.0f, 1.0f, &m_pModelData->m_pMeshBox, NULL );
			D3DXComputeNormals ( m_pModelData->m_pMeshBox, NULL );
		}
		#endif
		

		// Complete
		return m_pModelData;
	}
	else
	{
		// Failed to load, remove from list
		if ( test != NULL )
		{
			m_List.Delete ( iID );
			SAFE_DELETE ( test->m_Object.m_Frames );
			SAFE_DELETE ( test->m_Object.m_Meshes );
			SAFE_DELETE ( test );
		}

		// Failed
		return NULL;
	}
}

bool Delete ( int iID )
{
	// update internal model data
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	SAFE_DELETE ( m_pModelData->m_Object.m_Frames );
	SAFE_DELETE ( m_pModelData->m_Object.m_Meshes );
	SAFE_DELETE ( m_pModelData );
	m_List.Delete ( iID );
	
	return true;
}

void SetVertexShaderOn ( int iID, DWORD dwShader )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_Object.m_bOverrideVertexShader = true;
	m_pModelData->m_Object.m_dwVertexShader		   = dwShader;
}

void SetVertexShaderOff ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_Object.m_bOverrideVertexShader = false;
	m_pModelData->m_Object.m_dwVertexShader		   = 0;
}

bool AppendAnimationFromFile ( int iID, char* szFilename, int iFrame )
{
	// Get Object
	tagModelData* ptr = NULL;
	ptr = ( tagModelData* ) m_List.Get ( iID );
	if ( ptr==NULL)
		return false;

	// Add animation only from file
	if ( AppendAnimationData ( &m_pModelData->m_Object, szFilename, iFrame ) )
	{
		m_pModelData->m_iFrameCount = m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Length;
		m_pModelData->m_iEndFrame   = m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Length;
	}
	else
		return false;

	// Complete
	return true;
}

void SetSphereRadius ( int iID, float fRadius )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_Object.m_Radius = fRadius;
}

float GetRadius ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Radius;
}

void UpdateTimer ( void )
{

}

void Play ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_bAnimPlaying = true;
}

void Play ( int iID, int iStart )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_bAnimPlaying = true;
	m_pModelData->m_fFrame       = (float)iStart;
}

void Play ( int iID, int iStart, int iEnd )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_bAnimPlaying = true;
	m_pModelData->m_fFrame       = (float)iStart;

	m_pModelData->m_iStartFrame  = iStart;
	m_pModelData->m_iEndFrame    = iEnd;
}

void Loop ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_bAnimPlaying = true;
	//m_pModelData->m_bAnimLoop    = true;
	m_pModelData->m_iAnimMode = ANIM_LOOP;
}

void Loop ( int iID, int iStart )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_iStartFrame  = iStart;
	m_pModelData->m_bAnimPlaying = true;
	m_pModelData->m_fFrame       = (float)iStart;
	m_pModelData->m_iAnimMode = ANIM_LOOP;
}

void Loop ( int iID, int iStart, int iEnd )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_bAnimPlaying = true;
	m_pModelData->m_fFrame       = (float)iStart;

	m_pModelData->m_iStartFrame  = iStart;
	m_pModelData->m_iEndFrame    = iEnd;

	m_pModelData->m_iAnimMode = ANIM_LOOP;
}

float GetMinX ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Min.x;
}

float GetMinY ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Min.y;
}

float GetMinZ ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Min.z;
}

float GetMaxX ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Max.x;
}

float GetMaxY ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Max.y;
}

float GetMaxZ ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Max.z;
}

void Stop ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_bAnimPlaying = false;
}

void SetAnimationMode ( int iID, int iMode )
{
	m_pModelData->m_iAnimMode = iMode;
}

void SetSpeed ( int iID, int iSpeed )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_iFrameJump = iSpeed;
}

void SetFrame ( int iID, int iFrame )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_fFrame = (float)iFrame;
}

void SetFrame ( int iID, float fFrame )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_fFrame = fFrame;
}

void SetInterpolation ( int iID, int iJump )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	m_pModelData->m_fAnimSpeed = (float)iJump;
}

int GetAnimationCount ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	return m_pModelData->m_iFrameCount;
}

bool GetPlaying ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	return m_pModelData->m_bAnimPlaying;
}

bool GetLooping ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( m_pModelData->m_iAnimMode == ANIM_LOOP )
		return true;
	else
		return false;
}

int GetFrame ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return -1;

	return (int)m_pModelData->m_fFrame;
}

int GetSpeed ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return -1;

	return m_pModelData->m_iFrameJump;
}

int GetInterpolation ( int iID )
{
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return -1;

	return (int)m_pModelData->m_fAnimSpeed;
}

void ClearAllKeyFrames ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	SAFE_DELETE ( m_pModelData->m_Object.m_Animations.m_AnimationSet );
}

void ClearKeyFrame ( int iID, int iFrame )
{
	// clear a key frame of animation

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// variable declarations
	sAnimation** pAnimation = NULL;		// a list of pointers to pointers
	sAnimation* pFrame      = NULL;		// pointer to initial frame data
	int			iTemp       = 0;		// used for loops

	// allocate a list of pointers to pointers for the keyframes
	if ( ! ( pAnimation = new sAnimation* [ m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Length ] ) )
		Error ( "Failed to allocate key frame pointer list" );

	// get the start frame
	pFrame = m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Animation;

	// run through all frames and store pointers to data
	for ( iTemp = 0; iTemp < (int)m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Length; iTemp++ )
	{
		// store the current frame
		pAnimation [ iTemp ] = pFrame;

		// jump to the next frame
		if ( pFrame->m_Next )
			pFrame = pFrame->m_Next;
	}

	// see which frame we're at
	if ( iFrame == 0 )
	{
		// update first frame link
		m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Animation = pAnimation [ 1 ];
	}
	else if ( iFrame > 0 && iFrame < (int)m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Length )
	{
		// update in between 2 frames
		pAnimation [ iFrame - 1 ]->m_Next = pAnimation [ iFrame + 1 ];
	}
	else if ( iFrame == (int)m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Length )
	{
		// update last frame
		pAnimation [ iFrame - 1 ]->m_Next = NULL;
	}
	
	// minus the length
	m_pModelData->m_Object.m_Animations.m_AnimationSet->m_Length--;
	
	// delete the temp array
	SAFE_DELETE_ARRAY ( pAnimation );
}

void SetKeyFrame ( int iID, int iFrame )
{
	SetFrame ( iID, iFrame );
}

bool GetLimbExist ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return true;
	else
		return false;
}

bool GetLimbLink ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
	{
		return pFrame->m_bFree;
	}
	else
		return false;
}

float GetLimbOffsetX ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_Limb.vecOffset.x;
}

float GetLimbOffsetY ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_Limb.vecOffset.y;
}

float GetLimbOffsetZ ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_Limb.vecOffset.z;
}

float GetLimbAngleX ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_Limb.vecRotate.x;
}

float GetLimbAngleY ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_Limb.vecRotate.y;
}

float GetLimbAngleZ ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_Limb.vecRotate.z;
}

float GetLimbXPosition ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_matCombined._41;
}

float GetLimbYPosition ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_matCombined._42;
}

float GetLimbZPosition ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_matCombined._43;
}

float GetLimbXDirection ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_Limb.vecRotate.x;
}

float GetLimbYDirection ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_Limb.vecRotate.y;
}

float GetLimbZDirection ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;
	
	return pFrame->m_Limb.vecRotate.z;
}

IDirect3DTexture8* GetLimbTexturePtr ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return NULL;

	// get limb texture id
	return pFrame->m_MeshList->m_Mesh->GetTexture(0);
}

int GetLimbTexture ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return NULL;

	// no limb teture id info - need an imagedata scan!
	return 0;
}

bool GetLimbVisible ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return false;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return false;
	
	return pFrame->m_Limb.bVisible;
}

char* GetLimbTextureName ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return NULL;

	return NULL;
}

char* GetLimbName ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return NULL;

	return pFrame->m_Name;
}

int GetLimbCount ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0;

	return m_pModelData->m_Object.m_NumFrames;
}

float GetLimbScaleX ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;

	return pFrame->m_Limb.vecScale.x;
}

float GetLimbScaleY ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;

	return pFrame->m_Limb.vecScale.y;
}

float GetLimbScaleZ ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return 0.0f;

	return pFrame->m_Limb.vecScale.z;
}

void Draw ( int iID, int iBaseTextureStage )
{
	D3DXMATRIX	Matrix;

	m_pModelData = ( tagModelData* ) m_List.Get ( iID );
	if(m_pModelData==NULL) return;
	m_pModelData->m_Object.m_matObject = m_pModelData->m_matObject;

	// animation speed is set here
	if ( m_pModelData->m_bAnimPlaying )
	{
		m_pModelData->m_dwThisTime = timeGetTime ( );

		if ( m_pModelData->m_dwThisTime > m_pModelData->m_dwLastTime + m_pModelData->m_fAnimSpeed )
		{
			if ( m_pModelData->m_iAnimDirection == ANIM_DIR_FORWARD )
			{
				if ( m_pModelData->m_fFrame < m_pModelData->m_iEndFrame )
				{
					m_pModelData->m_fFrame += (m_pModelData->m_iFrameJump/100.0f);
				}
				else
					m_pModelData->m_iAnimDirection = ANIM_DIR_END;
			}
			else if ( m_pModelData->m_iAnimDirection == ANIM_DIR_BACK )
			{
				if ( m_pModelData->m_fFrame > m_pModelData->m_iStartFrame )
				{
					m_pModelData->m_fFrame -= (m_pModelData->m_iFrameJump/100.0f);
				}
				else
					m_pModelData->m_iAnimDirection = ANIM_DIR_END;
			}

			if ( m_pModelData->m_iAnimDirection == ANIM_DIR_END )
			{
				if ( m_pModelData->m_iAnimMode == ANIM_LOOP )
				{
					m_pModelData->m_fFrame         = (float)m_pModelData->m_iStartFrame;
					m_pModelData->m_bAnimPlaying   = true;
					m_pModelData->m_iAnimDirection = ANIM_DIR_FORWARD;
				}
				else if ( m_pModelData->m_iAnimMode == ANIM_SIMPLE )
				{
					m_pModelData->m_fFrame         = (float)m_pModelData->m_iStartFrame;
					m_pModelData->m_bAnimPlaying   = false;
					m_pModelData->m_iAnimDirection = ANIM_DIR_FORWARD;
				}
				else if ( m_pModelData->m_iAnimMode == ANIM_CYCLE )
				{
					if ( m_pModelData->m_fFrame < (float)( m_pModelData->m_iEndFrame + (m_pModelData->m_iFrameJump/100.0f) ) && m_pModelData->m_fFrame > ( m_pModelData->m_iEndFrame - (m_pModelData->m_iFrameJump/100.0f)  ) )
					{
						m_pModelData->m_iAnimDirection = ANIM_DIR_BACK;
					}
					else if ( m_pModelData->m_fFrame == (float)m_pModelData->m_iStartFrame )
					{
						m_pModelData->m_iAnimDirection = ANIM_DIR_FORWARD;
					}
				}
			}

			m_pModelData->m_dwLastTime = timeGetTime ( );
		}
	}
	UpdateAnimation ( &m_pModelData->m_Object, m_pModelData->m_fFrame, TRUE );

	// update the frame matrices
	D3DXMatrixIdentity ( &Matrix );
	UpdateFrame ( &m_pModelData->m_Object, m_pModelData->m_Object.m_Frames, &Matrix );

	if(m_pModelData->m_Object.m_Meshes)
		m_pModelData->m_Object.m_Meshes->CopyFrameToBoneMatrices ( );
	
	// draw all meshes
	D3DXMATRIX matHierarchy;
	D3DXMatrixIdentity ( &matHierarchy );
	DrawFrame ( &m_pModelData->m_Object, m_pModelData->m_Object.m_Frames, iBaseTextureStage, &matHierarchy );
	
	#if DEBUG_MODE
	if ( m_pModelData->m_pMeshBox )
	{
		D3DXMATRIX mat = m_pModelData->m_matBox * *m_pModelData->m_matObject;
		m_pD3D->SetTransform( D3DTS_WORLD, &mat );
		/*
		m_pD3D->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
		m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
		m_pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pModelData->m_pMeshBox->DrawSubset( 0 );
		m_pD3D->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_pD3D->SetRenderState( D3DRS_LIGHTING, TRUE );
		*/

		m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		m_pD3D->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		m_pModelData->m_pMeshBox->DrawSubset( 0 );
		m_pD3D->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		m_pD3D->SetRenderState( D3DRS_LIGHTING, TRUE );
		m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	}
	#endif
}

ID3DXMesh* GetMeshData ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	if ( m_pModelData->m_Object.m_Meshes->m_Mesh )
		return m_pModelData->m_Object.m_Meshes->m_Mesh;
	else
		return NULL;
}

LPDIRECT3DTEXTURE8* GetTextureData ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	if ( !m_pModelData->m_Object.m_Frames )
		return NULL;

	if ( m_pModelData->m_Object.m_Frames->m_MeshList )
	{
		if ( m_pModelData->m_Object.m_Frames->m_MeshList->m_Mesh )
		{
			if ( m_pModelData->m_Object.m_Frames->m_MeshList->m_Mesh->m_Textures )
			{
				return m_pModelData->m_Object.m_Frames->m_MeshList->m_Mesh->m_Textures;
			}
			else
				return NULL;
		}
		else
			return NULL;
	}
	else
		return NULL;
}

DWORD GetNumMaterials ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	if ( !m_pModelData->m_Object.m_Frames )
		return NULL;

	if ( m_pModelData->m_Object.m_Frames->m_MeshList )
	{
		if ( m_pModelData->m_Object.m_Frames->m_MeshList->m_Mesh )
		{
			if ( m_pModelData->m_Object.m_Frames->m_MeshList->m_Mesh->m_Textures )
			{
				return m_pModelData->m_Object.m_Frames->m_MeshList->m_Mesh->m_NumMaterials;
			}
			else
				return NULL;
		}
		else
			return NULL;
	}
	else
		return NULL;
}

D3DMATERIAL8* GetMaterialData ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	if ( !m_pModelData->m_Object.m_Frames )
		return NULL;

	if ( m_pModelData->m_Object.m_Frames->m_MeshList )
	{
		if ( m_pModelData->m_Object.m_Frames->m_MeshList->m_Mesh )
		{
			if ( m_pModelData->m_Object.m_Frames->m_MeshList->m_Mesh->m_Textures )
			{
				return m_pModelData->m_Object.m_Frames->m_MeshList->m_Mesh->m_Materials;
			}
			else
				return NULL;
		}
		else
			return NULL;
	}
	else
		return NULL;
}

D3DXMATRIX GetCombined ( int iID, int iFrame )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	D3DXMATRIX matZero;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return matZero;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iFrame ) ) )
		return matZero;

	return pFrame->m_matCombined;
}

D3DXMATRIX GetTransformed ( int iID, int iFrame )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iFrame ) ) )
		return NULL;

	return pFrame->m_matTransformed;
}

D3DXMATRIX GetOriginal ( int iID, int iFrame )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return NULL;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iFrame ) ) )
		return NULL;

	return pFrame->m_matOriginal;
}

void GetFVF ( int iID, DWORD** pdwFVF )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	*pdwFVF = &m_pModelData->m_Object.m_dwFVF;
}

void GetFVFSize ( int iID, DWORD** pdwFVFSize )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	*pdwFVFSize = &m_pModelData->m_Object.m_dwFVFSize;
}

DWORD GetNumberOfMeshes ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0;

	// return number of meshes in meshlist (for any mesh modifier)
	return m_pModelData->m_Object.m_NumMeshes;
}

void GetMeshDataFromMeshList ( int iID, int iIndex, LPD3DXMESH** pData )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// get a pointer to the start mesh
	sMesh* pMesh = m_pModelData->m_Object.m_Meshes;
	for ( int iTemp = 0; iTemp < m_pModelData->m_Object.m_NumMeshes; iTemp++ )
	{
		// Data from mesh
		if ( iTemp==iIndex )
		{
			*pData = &pMesh->m_Mesh;
			return;
		}

		// next mesh
		pMesh = pMesh->m_Next;
	}

	// not found
	*pData = NULL;
}

void GetMeshAttribFromMeshList ( int iID, int iIndex, D3DXATTRIBUTERANGE*** pData )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// get a pointer to the start mesh
	sMesh* pMesh = m_pModelData->m_Object.m_Meshes;
	for ( int iTemp = 0; iTemp < m_pModelData->m_Object.m_NumMeshes; iTemp++ )
	{
		// Data from mesh
		if ( iTemp==iIndex )
		{
			*pData = &pMesh->m_pAttributeTable;
			return;
		}

		// next mesh
		pMesh = pMesh->m_Next;
	}

	// not found
	*pData = NULL;
}

void ChangeToNewDeclaration ( int iID, DWORD* pdwDeclaration, DWORD dwOrFVF )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// get a pointer to the start mesh
	sMesh* pMesh = m_pModelData->m_Object.m_Meshes;
	for ( int iTemp = 0; iTemp < m_pModelData->m_Object.m_NumMeshes; iTemp++ )
	{
		// modify the mesh data
		LPD3DXMESH   pOriginalMesh = pMesh->m_Mesh;		// pointer to first mesh
		LPD3DXMESH   pNewMesh      = NULL;				// new mesh

		// clone the mesh
		if(dwOrFVF>0)
			pOriginalMesh->CloneMeshFVF ( 0, dwOrFVF, m_pD3D, &pNewMesh );
		else
			pOriginalMesh->CloneMesh ( 0, pdwDeclaration, m_pD3D, &pNewMesh );

		// only if successful
		if ( pNewMesh )
		{
			// free up the original mesh
			SAFE_RELEASE ( pOriginalMesh );

			// get rid of the original attribute table
			SAFE_DELETE_ARRAY ( pMesh->m_pAttributeTable );

			// store pointer to new mesh
			pMesh->m_Mesh = pNewMesh;

			// get new attribute table
			pMesh->m_Mesh->Optimize ( D3DXMESHOPT_ATTRSORT, NULL, NULL, NULL, NULL, &pNewMesh );
			SAFE_RELEASE ( pMesh->m_Mesh );
			pMesh->m_Mesh = pNewMesh;
			
			// get the size
			pMesh->m_Mesh->GetAttributeTable ( NULL, &pMesh->m_dwAttributeTableSize );

			// create new table
			pMesh->m_pAttributeTable = new D3DXATTRIBUTERANGE [ pMesh->m_dwAttributeTableSize ];

			// get table data
			pMesh->m_Mesh->GetAttributeTable ( pMesh->m_pAttributeTable, NULL );

			// update fvf information
			m_pModelData->m_Object.m_dwFVF = pMesh->m_Mesh->GetFVF ( );
			m_pModelData->m_Object.m_dwFVFSize = D3DXGetFVFVertexSize( m_pModelData->m_Object.m_dwFVF );
		}

		// next mesh
		pMesh = pMesh->m_Next;
	}
}

void HideLimb ( int iID, int iLimbID )
{
	// hides a limb

	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// we can now set the property of a limb, in this case we're
	// setting the visible property to false
	pFrame->m_Limb.bVisible = false;
}

void ShowLimb ( int iID, int iLimbID )
{
	// show a limb

	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// make the limb visible
	pFrame->m_Limb.bVisible = true;
}

void OffsetLimb ( int iID, int iLimbID, float fX, float fY, float fZ )
{
	// offset a limbs position

	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// setup the offset position
	pFrame->m_Limb.vecOffset = D3DXVECTOR3 ( fX, fY, fZ );
}

void RotateLimb ( int iID, int iLimbID, float fX, float fY, float fZ )
{
	// rotates a limb around it's 3 axes

	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// setup the rotation angles
	pFrame->m_Limb.vecRotate = D3DXVECTOR3 ( fX, fY, fZ );
}

void ScaleLimb ( int iID, int iLimbID, float fX, float fY, float fZ )
{
	// scale a limb

	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// setup the scale values
	pFrame->m_Limb.vecScale = D3DXVECTOR3 ( fX/100.0f, fY/100.0f, fZ/100.0f );
}

void AddLimb ( int iID, int iLimbID, int iMeshID )
{
	// variable declarations
	tagModelData* pMeshObj		= NULL;	// mesh object
	sMesh* pActualMesh			= NULL;	// mesh data

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// fail if already exists
	if ( ( m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// get mesh object
	if ( ! ( pMeshObj = ( tagModelData* ) m_MeshList.Get ( iMeshID ) ) )
		return;

	// get first mesh from mesh object
	if ( ! ( pActualMesh = pMeshObj->m_Object.m_Meshes ) )
		return;
	
	// use mesh FVF to control objectmesh creation
	DWORD dwInFVF = m_pModelData->m_Object.m_dwFVF;
	DWORD dwInFVFSize = m_pModelData->m_Object.m_dwFVFSize;

	// determine info on sourcemesh
	DWORD dwInNumPoly=pActualMesh->m_pAttributeTable [ 0 ].FaceCount;
	DWORD dwInNumVert=pActualMesh->m_pAttributeTable [ 0 ].VertexCount;

	// convert mesh first
	ID3DXMesh* pNewMesh;
	pActualMesh->m_Mesh->CloneMeshFVF ( 0, dwInFVF, m_pD3D, &pNewMesh );

	// create copy of meshdata
	float* pDataFromNewMesh;
	float* pInMesh = (float*)new char[dwInNumVert*dwInFVFSize];
	if ( SUCCEEDED ( pNewMesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &pDataFromNewMesh ) ) )
	{
		memcpy(pInMesh, pDataFromNewMesh, dwInNumVert*dwInFVFSize);
		pNewMesh->UnlockVertexBuffer ( );
	}

	// create new frame from mesh
	sMesh* Mesh = MakeMeshFromData ( dwInFVF, dwInFVFSize, pInMesh, dwInNumVert, D3DPT_TRIANGLELIST );

	// free meshdata
	SAFE_RELEASE(pNewMesh);
	SAFE_DELETE(pInMesh);

	// link in mesh
	Mesh->m_Next    = m_pModelData->m_Object.m_Meshes;
	m_pModelData->m_Object.m_Meshes = Mesh;
	m_pModelData->m_Object.m_NumMeshes++;

	// create a new frame
	sFrame* pFrame = new sFrame ( );

	// frame details
	pFrame->m_iID = iLimbID;
	pFrame->m_Name = new char [ 7 ];
	strcpy ( pFrame->m_Name, "%NEW%" );
	pFrame->AddMesh ( Mesh );
	pFrame->m_bFree=true;

	// Add to end of framelist
	sFrame* pCurrent = m_pModelData->m_Object.m_Frames;
	while(pCurrent)
	{
		if(pCurrent->m_Sibling==NULL)
		{
			pCurrent->m_Sibling = pFrame;
			break;
		}
		else
			pCurrent=pCurrent->m_Sibling;
	}

	// increase frame count if applicable
	if ( iLimbID+1 > m_pModelData->m_Object.m_NumFrames )
		m_pModelData->m_Object.m_NumFrames = iLimbID+1;
}

void RemoveFrameFromHierarchy ( tagModelData* m_pModelData, sFrame* pFrame )
{
	// get parent of limb
	sFrame* pParent = pFrame->m_Parent;
	sFrame* pStartOfSybChain = NULL;
	if(pParent==NULL)
		pStartOfSybChain = m_pModelData->m_Object.m_Frames;
	else
		pStartOfSybChain = pParent->m_Child;

	// remove limb from symbling chain
	sFrame* pLastSyb = NULL;
	sFrame* pSybChain = pStartOfSybChain;
	while(pSybChain)
	{
		sFrame* pNextSyb = pSybChain->m_Sibling;
		if(pSybChain==pFrame)
		{
			if(pSybChain==pStartOfSybChain)
			{
				if(pParent==NULL)
					m_pModelData->m_Object.m_Frames = pNextSyb;
				else
					pParent->m_Child = pNextSyb;
			}
			else
				pLastSyb->m_Sibling = pFrame->m_Sibling;

			break;
		}
		pLastSyb = pSybChain;
		pSybChain = pNextSyb;
	}
	
	// remove all family links to model
	pFrame->m_Parent=NULL;
	pFrame->m_Sibling=NULL;
}

void RemoveLimb ( int iID, int iLimbID )
{
	// pointer to frame data
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// remove limb from symbling chain
	RemoveFrameFromHierarchy ( m_pModelData, pFrame );

	// delete frame allocations (this and children of limb)
	SAFE_DELETE(pFrame);
}

void LinkLimb ( int iID, int iLimbID, int iParentID )
{
	// Rule : can only add new limb (so cannot create a recursive-infinite-nest)

	// find obj responsible
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// find target limb
	sFrame* pTargetFrame = NULL;
	if ( ! ( pTargetFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iParentID ) ) )
		return;

	// find free limb
	sFrame* pFreeFrame = NULL;
	if ( ! ( pFreeFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// check free limb state
	if ( pFreeFrame->m_bFree==false )
		return;

	// free limb cannot be root limb
	if(m_pModelData->m_Object.m_Frames==pFreeFrame)
		return;

	// remove limb from symbling chain
	RemoveFrameFromHierarchy ( m_pModelData, pFreeFrame );

	// attach limb to parent limb
	pFreeFrame->m_Parent = pTargetFrame;
	pFreeFrame->m_Sibling = NULL;

	// make parent aware of new child
	if(pTargetFrame->m_Child)
	{
		// child part of sybling chain
		sFrame* pCurrent = pTargetFrame->m_Child;
		while(pCurrent)
		{
			if(pCurrent->m_Sibling==NULL)
			{
				pCurrent->m_Sibling = pFreeFrame;
			}
			pCurrent = pCurrent->m_Sibling;
		}
	}
	else
	{
		// first child of parent
		pTargetFrame->m_Child = pFreeFrame;
	}

	// limb no longer free
	pFreeFrame->m_bFree=false;
}

void SetTexture ( int iID, LPDIRECT3DTEXTURE8 pTexture )
{
	// set the texture of an object, this is applied to all
	// limbs contained within the model

	// variable declarations
	sFrame* pFrame = NULL;		// pointer to frame data
	int		iTemp  = 0;			// used for loops
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// run through all frames
	for ( iTemp = 0; iTemp < m_pModelData->m_Object.m_NumFrames; iTemp++ )
	{
		// see if the frame exists
		if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iTemp ) ) )
			return;

		// check we have a valid mesh list
		if ( pFrame->m_MeshList )
		{
			pFrame->m_MeshList->m_Mesh->m_Textures [ 0 ] = pTexture;
		}
	}
	
	// sometimes a model may have no frames of animation and just be 
	// 1 segment, we deal with this case here
	if ( m_pModelData->m_Object.m_NumFrames == 0 )
	{
		// delete the original texture array, it may not exist so we do this just in case,
		// then we recreate the texture list
		SAFE_DELETE_ARRAY ( m_pModelData->m_Object.m_Meshes->m_Textures );

		// allocate memory for the textures
		m_pModelData->m_Object.m_Meshes->m_Textures = new LPDIRECT3DTEXTURE8 [ m_pModelData->m_Object.m_Meshes->m_NumMaterials ];

		// run through the materials and setup the texture
		for ( int iTemp = 0; iTemp <  (int)m_pModelData->m_Object.m_Meshes->m_NumMaterials; iTemp++ )
			m_pModelData->m_Object.m_Meshes->m_Textures [ iTemp ] = pTexture;
	}
}

int GetFrames ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return -1;

	return m_pModelData->m_Object.m_NumFrames;
}

float GetXSize ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Max.x;
}

float GetYSize ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Max.y;
}

float GetZSize ( int iID )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return 0.0f;

	return m_pModelData->m_Object.m_Max.z;
}

void ChangeFrameMeshData ( sFrame* pFrame, int iTask, float fA, float fB, float fC )
{
	if ( !pFrame->m_MeshList )
		return;
	if ( !pFrame->m_MeshList->m_Mesh )
		return;
	if ( !pFrame->m_MeshList->m_Mesh->m_Mesh )
		return;

	// Get Vertex Info
	DWORD dwNumVertices = pFrame->m_MeshList->m_Mesh->m_Mesh->GetNumVertices ( );
	DWORD dwFVF         = pFrame->m_MeshList->m_Mesh->m_Mesh->GetFVF ( );

	// need diffuse for colour
	if ( iTask==1 && !(dwFVF & D3DFVF_DIFFUSE) )
		return;

	// need tex for UVscroll
	if ( iTask==2 && !(dwFVF & D3DFVF_TEX1) )
		return;

	// need tex for UVscale
	if ( iTask==3 )
	{
		if ( !(dwFVF & D3DFVF_TEX1) )
			return;

		// scale 100 -> 1.0f
		fA/=100.0f;
		fB/=100.0f;
	}

	// need normals for Fade
	if ( iTask==4 )
	{
		if( !(dwFVF & D3DFVF_NORMAL) )
			return;

		// fade 100 -> 1.0f
		fA/=100.0f;
		fB/=100.0f;
		fC/=100.0f;
	}

	// need diffuse for fade
	if ( iTask==5 )
	{
		if( !(dwFVF & D3DFVF_DIFFUSE) )
			return;

		// fade 100 -> 1.0f
		fA/=100.0f;
		fB/=100.0f;
		fC/=100.0f;
	}

	// Calculate Vertex Offsets
	DWORD dwFVFSize     = 0;
	DWORD dwXYZOffset = dwFVFSize;
	if ( dwFVF & D3DFVF_XYZ ) dwFVFSize+=12;
	DWORD dwNORMALOffset = dwFVFSize;
	if ( dwFVF & D3DFVF_NORMAL ) dwFVFSize+=12;
	DWORD dwDIFFUSEOffset = dwFVFSize;
	if ( dwFVF & D3DFVF_DIFFUSE ) dwFVFSize+=4;
	DWORD dwTEXOffset = dwFVFSize;
	if ( dwFVF & D3DFVF_TEX1 ) dwFVFSize+=8;

	// Change Diffuse Data
	LPSTR pVertices = NULL;
	if ( SUCCEEDED ( pFrame->m_MeshList->m_Mesh->m_Mesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK , ( BYTE** ) &pVertices ) ) )
	{
		// go through each triangle and set the diffuse component
		for ( int iTemp = 0; iTemp < (int)dwNumVertices; iTemp++ )
		{
			// do task to vertex
			switch(iTask)
			{
				case 1 : {	// new diffuse colour
							DWORD* pDiffuse = (DWORD*)(pVertices+dwDIFFUSEOffset);
							*pDiffuse = D3DCOLOR_ARGB ( 255, (DWORD)fA, (DWORD)fB, (DWORD)fC );
							break;
						 }

				case 2 : {	// scroll UV
							float* pU = (float*)(pVertices+dwTEXOffset);
							float* pV = (float*)(pVertices+dwTEXOffset+4);
							if ( *pU > 1.0e15f ) *pU = 0.0f; else *pU = *pU + fA;
							if ( *pV > 1.0e15f ) *pV = 0.0f; else *pV = *pV + fB;
							break;
						 }

				case 3 : {	// scale UV
							float* pU = (float*)(pVertices+dwTEXOffset);
							float* pV = (float*)(pVertices+dwTEXOffset+4);
							*pU = *pU * fA;
							*pV = *pV * fB;
							break;
						 }

				case 4 : {	// Fade using normals
							float* pX = (float*)(pVertices+dwNORMALOffset);
							float* pY = (float*)(pVertices+dwNORMALOffset+4);
							float* pZ = (float*)(pVertices+dwNORMALOffset+8);
							*pX = *pX * fA;
							*pY = *pY * fB;
							*pZ = *pZ * fC;
							break;
						 }

				case 5 : {	// Fade using diffuse
							DWORD* pDiffuse = (DWORD*)(pVertices+dwDIFFUSEOffset);
							*pDiffuse = D3DCOLOR_ARGB ( 255, (DWORD)(fA*255.0f), (DWORD)(fB*255.0f), (DWORD)(fC*255.0f) );
							break;
						 }
			}

			// next vertex
			pVertices+=dwFVFSize;
		}
		pFrame->m_MeshList->m_Mesh->m_Mesh->UnlockVertexBuffer ( );
	}
}

void Color ( int iID, int iR, int iG, int iB )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// run through all frames
	sFrame* pFrame        = NULL;
	for ( int iFrame = 0; iFrame < m_pModelData->m_Object.m_NumFrames; iFrame++ )
	{
		// see if the frame exists
		if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iFrame ) ) )
			return;

		// modify mesh within frame
		ChangeFrameMeshData ( pFrame, 1, (float)iR, (float)iG, (float)iB );
	}
}

void Fade ( int iID, float fPercentage )
{
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// run through all frames
	sFrame* pFrame        = NULL;
	for ( int iFrame = 0; iFrame < m_pModelData->m_Object.m_NumFrames; iFrame++ )
	{
		// see if the frame exists
		if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iFrame ) ) )
			return;

		// modify mesh within frame
		ChangeFrameMeshData ( pFrame, 5, fPercentage, fPercentage, fPercentage );
	}
}

void ScrollTexture ( int iID, float fU, float fV )
{
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	if ( m_pModelData->m_Object.m_Meshes==NULL )
		return;

	// disable scrolling of non limb objects
	if ( m_pModelData->m_Object.m_Meshes->m_NumBones > 0 )
		return;

	// run through all frames
	for ( int iFrame = 0; iFrame < m_pModelData->m_Object.m_NumFrames; iFrame++ )
	{
		// see if the frame exists
		if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iFrame ) ) )
			return;

		// modify mesh within frame
		ChangeFrameMeshData ( pFrame, 2, fU, fV, 0.0f );
	}
}

void ScaleTexture ( int iID, float fU, float fV )
{
	sFrame* pFrame = NULL;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	if ( m_pModelData->m_Object.m_Meshes==NULL )
		return;

	// disable scrolling of non limb objects
	if ( m_pModelData->m_Object.m_Meshes->m_NumBones > 0 )
		return;

	// run through all frames
	for ( int iFrame = 0; iFrame < m_pModelData->m_Object.m_NumFrames; iFrame++ )
	{
		// see if the frame exists
		if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iFrame ) ) )
			return;

		// modify mesh within frame
		ChangeFrameMeshData ( pFrame, 3, fU, fV, 0.0f );
	}
}

void TextureLimb ( int iID, int iLimbID, int iImageID )
{
	// set the texture of a limb

	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// disable scrolling of non limb objects
	if ( m_pModelData->m_Object.m_Meshes->m_NumBones > 0 )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// set the texture pointer
	//pFrame->m_MeshList->m_Mesh->m_Textures [ 0 ] = g_Image_GetPointer ( iImageID );
	pFrame->m_MeshList->m_Mesh->m_Textures [ 0 ] = NULL;
}

void ColorLimb ( int iID, int iLimbID, DWORD dwColor )
{
	// set the color of a limb

	// pointer to frame data
	sFrame* pFrame = NULL;
	
	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// disable scrolling of non limb objects
	if ( m_pModelData->m_Object.m_Meshes->m_NumBones > 0 )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// break down the dwColor variable into individual components
	DWORD dwRed   = ( ( ( dwColor ) >> 16)       & 0xff );
	DWORD dwGreen = ( ( ( dwColor ) >>  8 ) & 0xff );
	DWORD dwBlue  = ( ( ( dwColor )  ) & 0xff );

	// modify mesh within frame
	ChangeFrameMeshData ( pFrame, 1, (float)dwRed, (float)dwGreen, (float)dwBlue );
}

void ScrollLimbTexture ( int iID, int iLimbID, float fU, float fV )
{
	// scrolls texture of a limb

	// pointer to frame data
	sFrame* pFrame        = NULL;
	BYTE**	Ptr           = NULL;
	DWORD	dwNumVertices = 0;
	DWORD   dwFVF         = 0;
	int     iCount        = 0;
	int		iTemp	      = 0;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;
	
	// disable scrolling of non limb objects
	if ( m_pModelData->m_Object.m_Meshes->m_NumBones > 0 )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// modify mesh within frame
	ChangeFrameMeshData ( pFrame, 2, fU, fV, 0.0f );
}

void ScaleLimbTexture ( int iID, int iLimbID, float fU, float fV )
{
	// scrolls texture of a limb

	// pointer to frame data
	sFrame* pFrame        = NULL;
	BYTE**	Ptr           = NULL;
	DWORD	dwNumVertices = 0;
	DWORD   dwFVF         = 0;
	int     iCount        = 0;
	int		iTemp	      = 0;

	// see if the model exists
	if ( ! ( m_pModelData = ( tagModelData* ) m_List.Get ( iID ) ) )
		return;

	// disable scrolling of non limb objects
	if ( m_pModelData->m_Object.m_Meshes->m_NumBones > 0 )
		return;

	// now see if the limb exists
	if ( ! ( pFrame = m_pModelData->m_Object.m_Frames->FindFrame ( iLimbID ) ) )
		return;

	// modify mesh within frame
	ChangeFrameMeshData ( pFrame, 3, fU, fV, 0.0f );
}

// Data Access Functions

void GetMeshData( int iMeshID, DWORD* pdwFVF, DWORD* pdwFVFSize, DWORD* pdwVertMax, LPSTR* pData, DWORD* dwDataSize, bool bLockData )
{
	// Read Data
	if(bLockData==true)
	{
		// makes a mesh from a meshobject
		tagModelData* pMeshObj = NULL;
		if ( ! ( pMeshObj = ( tagModelData* ) m_MeshList.Get ( iMeshID ) ) )
			return;

		// get first mesh from mesh object
		sMesh* pActualMesh			= NULL;	// mesh data
		if ( ! ( pActualMesh = pMeshObj->m_Object.m_Meshes ) )
			return;

		// use mesh FVF to control objectmesh creation
		DWORD dwInFVF = pMeshObj->m_Object.m_dwFVF;
		DWORD dwInFVFSize = pMeshObj->m_Object.m_dwFVFSize;
		DWORD dwInNumPoly=pActualMesh->m_pAttributeTable [ 0 ].FaceCount;
		DWORD dwInNumVert=pActualMesh->m_pAttributeTable [ 0 ].VertexCount;

		// convert mesh first
		ID3DXMesh* pNewMesh;
		pActualMesh->m_Mesh->CloneMeshFVF ( 0, dwInFVF, m_pD3D, &pNewMesh );
		ID3DXMesh* pWorkMesh = pNewMesh;
	
		// create copy of meshdata
		float* pDataFromNewMesh;
		float* pInMesh = (float*)new char[dwInNumVert*dwInFVFSize];
		if ( SUCCEEDED ( pWorkMesh->LockVertexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &pDataFromNewMesh ) ) )
		{
			memcpy(pInMesh, pDataFromNewMesh, dwInNumVert*dwInFVFSize);
			pWorkMesh->UnlockVertexBuffer ( );
		}

		// create copy of meshdata
		WORD* pIndicesFromNewMesh;
		WORD* pInIndices = new WORD[dwInNumPoly*3];
		if ( SUCCEEDED ( pWorkMesh->LockIndexBuffer ( D3DLOCK_NOSYSLOCK, ( BYTE** ) &pIndicesFromNewMesh ) ) )
		{
			memcpy(pInIndices, pIndicesFromNewMesh, dwInNumPoly*3*sizeof(WORD));
			pWorkMesh->UnlockIndexBuffer ( );
		}

		// make mesh data not depend on indices
		DWORD dwNewVertCount=0;
		float* pNoIndexRequiredMesh = CreatePureTriangleMeshData( pInMesh, &dwNewVertCount, dwInNumVert, dwInFVFSize, pInIndices, dwInNumPoly );
		SAFE_DELETE(pInIndices);
		SAFE_DELETE(pInMesh);

		// mesh data
		*pdwFVF = dwInFVF;
		*pdwFVFSize = dwInFVFSize;
		*pdwVertMax = dwNewVertCount;

		// create memory
		DWORD dwSizeOfData = dwNewVertCount*dwInFVFSize;
		*pData = new char[dwSizeOfData];
		*dwDataSize = dwSizeOfData;

		// copy mesh to new memory
		memcpy( *pData, pNoIndexRequiredMesh, dwSizeOfData );

		// free usages
		SAFE_DELETE(pNoIndexRequiredMesh);

		// free work mesh
		SAFE_RELEASE(pWorkMesh);
	}
	else
	{
		// free memory
		delete *pData;
	}
}

void SetMeshData( int iMeshID, DWORD dwFVF, DWORD dwFVFSize, LPSTR pMeshData, DWORD dwVertMax )
{
	// zero mesh not perimitted
	if ( iMeshID<=0 )
		return;

	// create new mesh-object from single mesh
	MakeMeshFromMesh ( iMeshID, dwFVF, dwFVFSize, (float*)pMeshData, dwVertMax/3, dwVertMax, D3DPT_TRIANGLELIST );
}
