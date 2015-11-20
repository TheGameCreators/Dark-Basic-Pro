
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// mem leak dump check
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
_CrtMemState g_crt_mem_state;

#include "ConvX.h"

#ifndef DARKSDK_COMPILE
	#include <initguid.h>
	#include "dxfile.h"
#endif

#define IMPL_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)\
	EXTERN_C const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

IMPL_GUID(DBIID_IDirectXFileData,           0x3d82ab44, 0x62da, 0x11cf, 0xab, 0x39, 0x0, 0x20, 0xaf, 0x71, 0xe4, 0x33);
IMPL_GUID(DBIID_IDirectXFileDataReference,  0x3d82ab45, 0x62da, 0x11cf, 0xab, 0x39, 0x0, 0x20, 0xaf, 0x71, 0xe4, 0x33);

// lee - 060206 - added during upgrade to DEC2005 version (not sure why d3dx9mesh.h not used)
// it could be initguid and dxfile.h is doing something..
// perhaps initguid must preceed DEFINE_GUID calls?!?
DEFINE_GUID(DXFILEOBJ_XSkinMeshHeader,
0x3cf169ce, 0xff7c, 0x44ab, 0x93, 0xc0, 0xf7, 0x8f, 0x62, 0xd1, 0x72, 0xe2);
DEFINE_GUID(DXFILEOBJ_SkinWeights, 
0x6f0d123b, 0xbad2, 0x4167, 0xa0, 0xd0, 0x80, 0x22, 0x4f, 0x25, 0xfa, 0xbb);
DEFINE_GUID(DXFILEOBJ_FVFData, 
0xb6e70a0e, 0x8ef9, 0x4e83, 0x94, 0xad, 0xec, 0xc8, 0xb0, 0xc0, 0x48, 0x97);
// 040908 - added for U70 to support TS76 new X file export (and others)
DEFINE_GUID(DXFILEOBJ_DeclData, 
0xbf22e553, 0x292c, 0x4781, 0x9f, 0xea, 0x62, 0xbd, 0x55, 0x4b, 0xdd, 0x93);
DEFINE_GUID(DXFILEOBJ_VertexElement, 
0xf752461c, 0x1e23, 0x48f6, 0xb9, 0xf8, 0x83, 0x50, 0x85, 0x0f, 0x33, 0x6f);

#include "rmxftmpl.h"
#include "rmxfguid.h"	

#include < vector >
using namespace std;

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
#endif
#pragma comment ( lib, "d3dxof.lib" )

//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Globals for DBO/Manager relationship
std::vector< sMesh* >		g_vRefreshMeshList;
std::vector< sObject* >     g_vAnimatableObjectList;
int							g_iSortedObjectCount;
sObject**					g_ppSortedObjectList;

// DBP Format uses ObjectManager
#include ".\..\Objects\CObjectManagerC.h"
CObjectManager m_ObjectManager;
bool CObjectManager::Setup ( void ) { return false; }
bool CObjectManager::Free ( void ) { return false; }

struct sNewVertex
{
	// to store vertex information

	D3DXVECTOR3 vecPosition;
	D3DXVECTOR3 vecNormal;
	float		tu, tv;
	float		tu2, tv2; //leeadded - 261105 - for loading second set of UV data
	float		tu3, tv3; //leeadded - 120106 - for loading third set of UV data
};

int g_iLegacyModeIgnoresFVFDATA = 0; // U75 - default is no legacy mode (set by DLL function call)
int g_iNumberOfExtraUVDataStagesAvailable = 0;

struct sNewBones
{
	// bone data

    char*		szName;
    DWORD		dwNumWeights;
    DWORD*		pIndices;
    float*		pfWeights;
    D3DXMATRIX  matrix;
};

struct sNewMaterial
{
	// material information

	D3DXVECTOR4 vecColour;
	float		fPower;
	D3DXVECTOR3 vecSpecular;
	D3DXVECTOR3 vecEmissive;
	char*		szFilename;
};

typedef IDirect3DDevice9*		( *GFX_GetDirect3DDevicePFN )	( void );

DBPRO_GLOBAL GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;		
DBPRO_GLOBAL HINSTANCE						g_hSetup					= NULL;
DBPRO_GLOBAL LPDIRECT3DDEVICE9				m_pD3D						= NULL;
DBPRO_GLOBAL sObject*						g_pObjectX					= NULL;
DBPRO_GLOBAL sObject*						g_pObjectXFinal   			= NULL;
DBPRO_GLOBAL sMesh*							g_pNewMeshList;
DBPRO_GLOBAL GlobStruct*					g_pGlob						= NULL;
DBPRO_GLOBAL sObject**						g_ObjectList;
DBPRO_GLOBAL sObject**						g_ObjectListX;
DBPRO_GLOBAL sNewVertex*					g_pVertexList				= NULL;			// vertices
DBPRO_GLOBAL DWORD*							g_pIndexList				= NULL;			// indices
DBPRO_GLOBAL D3DXVECTOR3*					g_pNormalList				= NULL;			// indices
DBPRO_GLOBAL DWORD*							g_pNormalIndexList			= NULL;			// indices
DBPRO_GLOBAL DWORD*							g_pFaceIndexOldRef			= NULL;			// store original indexes (if quads or greater processed)
DBPRO_GLOBAL sVertexColor*					g_pVertexColorList			= NULL;			// vertice colors
DBPRO_GLOBAL DWORD							g_dwVertexCount				= 0;			// number of vertices
DBPRO_GLOBAL DWORD							g_dwFaceCount				= 0;			// number of indices
DBPRO_GLOBAL DWORD							g_dwNormalFaceCount			= 0;
DBPRO_GLOBAL DWORD							g_dwNormalCount				= 0;			// number of normals ( can be greater than vertices)
DBPRO_GLOBAL DWORD							g_dwVertexColorCount		= 0;
DBPRO_GLOBAL DWORD*							g_pMaterialList				= NULL;			// material list
DBPRO_GLOBAL DWORD							g_dwMaterialListIndexCount	= 0;			// number of indices in material list
DBPRO_GLOBAL DWORD							g_dwMaterialCount			= 0;			// number of materials
DBPRO_GLOBAL WORD							g_dMaxSkinWeightsPerVertex	= 0;			// vertex skin weights
DBPRO_GLOBAL WORD							g_dMaxSkinWeightsPerFace	= 0;			// face skin weights
DBPRO_GLOBAL DWORD							g_dwBones					= 0;			// bone count
DBPRO_GLOBAL bool							g_bDisableBones				= false;		// disable bones if needed
DBPRO_GLOBAL vector < sNewBones    >		g_BoneList;									// list of bones
DBPRO_GLOBAL vector < sNewMaterial >		g_NewMaterialList;							// list of materials
DBPRO_GLOBAL float							g_fShrinkObjectsTo			= 0.0f;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//#ifndef DARKSDK_COMPILE
// internal functions
DARKSDK bool	Load ( char* szFilename );
DARKSDK bool	LoadModelData ( char* szFilename, sFrame* pFrame, bool bAnim );
DARKSDK bool	Delete ( void );

DARKSDK bool    ParseXFileData ( IDirectXFileData* pDataObj, sFrame* pParentFrame, char* szTexturePath, sAnimationSet *pParentAnim, sAnimation *pCurrentAnim, bool bAnim );
DARKSDK bool    MapFramesToBones ( sMesh** pMesh, sFrame *Frame, int iCount );
DARKSDK bool    MapFramesToAnimations ( sObject* pObject );
DARKSDK void    SetupTextures ( sMesh* pMesh, D3DXMATERIAL* pMaterials, DWORD dwCount, DWORD* pAttribute, DWORD dwIndexCount );
DARKSDK sFrame* FindFrame ( char* szName, sFrame* pFrame );
DARKSDK bool    UpdateAnimation ( sObject* pObject, float fTime );
//DARKSDK void    ResetFrameMatrices ( sFrame* pFrame );
DARKSDK bool    UpdateAnimationData ( sAnimation* pAnim, float fTime );

DARKSDK bool	ProduceMaterialIndiceTables ( void );
DARKSDK bool    SplitAnySolidMultiMeshes ( void );
//#endif


DARKSDK bool GetD3DFromModule ( HINSTANCE hSetup )
{
	#ifndef DARKSDK_COMPILE
	// get module
	if ( !hSetup )
	{
		g_hSetup = LoadLibrary ( "DBProSetupDebug.dll" );
		if ( !g_hSetup ) return false;
		hSetup = g_hSetup;
	}
	#endif

	// get d3d ptr
	#ifndef DARKSDK_COMPILE
		g_GFX_GetDirect3DDevice = ( GFX_GetDirect3DDevicePFN ) GetProcAddress ( hSetup, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );
	#else
		g_GFX_GetDirect3DDevice = dbGetDirect3DDevice;
	#endif

	m_pD3D                  = g_GFX_GetDirect3DDevice ( );

	// okay
	return true;
}

DARKSDK bool Load ( char* szFilename )
{
	// load an X model which will get converted to the DBO format

	// free existing object core
	Delete();

	// create new object core
	g_pObjectX = new sObject;

	// check the filename and D3D device are valid
	SAFE_MEMORY ( g_pObjectX );
	SAFE_MEMORY ( szFilename );
	SAFE_MEMORY ( m_pD3D );

	// allocate a new frame for the object
	g_pObjectX->pFrame = new sFrame;

	// ensure creation successful
	SAFE_MEMORY ( g_pObjectX->pFrame );

	// because of the way in which an X file is parsed we have to run
	// 2 passes, the first will build the frame list with mesh information
	// and the second pass will retrieve the animation information
	if ( !LoadModelData ( szFilename, g_pObjectX->pFrame, false ) )
		return false;

	if ( !LoadModelData ( szFilename, g_pObjectX->pFrame, true ) )
		return false;

	// setup generic properties
	SetupObjectsGenericProperties ( g_pObjectX );

	// produce sets of indice-table by material
	if ( !ProduceMaterialIndiceTables ( ) )
		return false;

	// mike - 070804 - need to reset these values
	g_dwNormalFaceCount = 0;
	g_dwNormalCount = 0;

	// object must have mesh and frame list
	if ( g_pObjectX->ppFrameList && g_pObjectX->ppMeshList )
	{
		// everything went okay
		return true;
	}
	else
	{
		// failed to create the object
		return false;
	}
}

DARKSDK bool CreateMeshInFrame ( sFrame* pFrame, DWORD dwNumVertices, DWORD dwNumFaces )
{
	// check we have a valid frame
	SAFE_MEMORY ( pFrame );

	// ceate a new mesh to hold the data
	pFrame->pMesh = new sMesh;

	// ensure the mesh was created
	SAFE_MEMORY ( pFrame->pMesh );

	// setup mesh properties and allocate memory
	pFrame->pMesh->dwVertexCount	= dwNumVertices;
	pFrame->pMesh->dwIndexCount		= dwNumFaces * 3;
	pFrame->pMesh->pVertexData		= (BYTE*)new sVertex [ pFrame->pMesh->dwVertexCount ];	// allocate vertex memory
	pFrame->pMesh->pIndices			= new WORD    [ pFrame->pMesh->dwIndexCount  ];			// allocate index memory
	pFrame->pMesh->dwFVF			= D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	pFrame->pMesh->dwFVFSize		= 32;

	// setup mesh drawing properties
	pFrame->pMesh->iPrimitiveType	= D3DPT_TRIANGLELIST;
	pFrame->pMesh->iDrawVertexCount	= dwNumVertices;
	pFrame->pMesh->iDrawPrimitives	= dwNumFaces;

	// check the newly created memory is okay
	SAFE_MEMORY ( pFrame->pMesh->pVertexData );
	SAFE_MEMORY ( pFrame->pMesh->pIndices  );

	// clear out newly created memory
	memset ( pFrame->pMesh->pVertexData, 0, sizeof ( sVertex ) * pFrame->pMesh->dwVertexCount );
	memset ( pFrame->pMesh->pIndices,    0, sizeof ( WORD    ) * pFrame->pMesh->dwIndexCount  );

	return true;
}

DARKSDK bool CheckMeshIfItNeedsCuttingUp ( sMesh* pMesh )
{
	// no need to dice material if only 0 or 1 texture
	if ( pMesh->dwTextureCount <= 1 )
	{
		// success - dont need material bank for splitting no more
		SAFE_DELETE(pMesh->pMaterialBank);
		return false;
	}

	// make sure the textures within the mesh are actually different, in some cases
	// we can simply discard all but one of the textures and return
	bool bSame = true;
	bool bTextureFilesPresent = false;
	for ( int iTextureA = 0; iTextureA < (int)pMesh->dwTextureCount; iTextureA++ )
	{
		char* szCurrent = pMesh->pTextures [ iTextureA ].pName;
		if ( strlen ( szCurrent ) > 0 )
		{
			bTextureFilesPresent = true;
		}
		for ( int iTextureB = 0; iTextureB < (int)pMesh->dwTextureCount; iTextureB++ )
		{
			if ( strcmp ( szCurrent, pMesh->pTextures [ iTextureB ].pName ) != 0 )
			{
				bSame = false;
				break;
			}
		}
		if ( !bSame )
			break;
	}

	// if all are the same, make as one texture
	if ( bSame && bTextureFilesPresent )
	{
		// new single texture in place of many textures

		// MIKE - 011103 - must allocate array instead of single item!
		sTexture* tex = new sTexture [ 1 ];
		memcpy ( tex, &pMesh->pTextures [ 0 ], sizeof ( sTexture ) );
		SAFE_DELETE_ARRAY ( pMesh->pTextures );
		pMesh->pTextures = tex;
		pMesh->dwTextureCount = 1;

		// success - dont need material bank for splutting no more
		SAFE_DELETE(pMesh->pMaterialBank);

		// only texture quantity changed
		return false;
	}

	// yes it needs cutting up
	return true;
}

DARKSDK bool ProduceMaterialIndiceOfFrame ( sObject* pObject, sFrame* pOriginalFrame )
{
	// pointer to mesh to replace (maybe)
	sMesh* pMesh = pOriginalFrame->pMesh;

	// check if needs cutting up
	if ( CheckMeshIfItNeedsCuttingUp ( pMesh )==false )
	{
		// does not need cutting up - return success
		return true;
	}

	// mike - 111005 - no need to do anything if we have no indices
	if ( pMesh->dwIndexCount == 0 )
		return true;

	// new index data array
	DWORD dwIndexCount = pMesh->dwIndexCount;
	WORD* pNewIndiceData = new WORD[dwIndexCount];
	WORD* pIndiceDataPtr = pNewIndiceData;

	// prepare multi-material settings
	DWORD dwPolyCounter=0;
	DWORD dwIndexCounter=0;
	pMesh->bUseMultiMaterial = true;
	pMesh->dwMultiMaterialCount = pMesh->dwTextureCount;
	pMesh->pMultiMaterial = new sMultiMaterial [ pMesh->dwMultiMaterialCount ];
	memset ( pMesh->pMultiMaterial, 0, sizeof(sMultiMaterial) * pMesh->dwMultiMaterialCount );

	// go through all textures and extract data
	for ( int iTextureIndex = 0; iTextureIndex < (int)pMesh->dwTextureCount; iTextureIndex++ )
	{
		// reset polycount
		dwPolyCounter=0;

		// index start position
		pMesh->pMultiMaterial [ iTextureIndex ].dwIndexStart = dwIndexCounter;

		// start adding to new index data in order of texture use in material sets
		for ( int iIndex = 0; iIndex < (int)pMesh->dwIndexCount; iIndex+=3 )
		{
			// work out which texture this mesh should use
			int iFaceIndex = iIndex / 3;
			int iTextureUsedHere = pMesh->pAttributeWorkData [ iFaceIndex ];

			// only process vertices that belong in this mesh
			if ( iTextureIndex == iTextureUsedHere )
			{
				*(pIndiceDataPtr++) = pMesh->pIndices [ iIndex+0 ];
				*(pIndiceDataPtr++) = pMesh->pIndices [ iIndex+1 ];
				*(pIndiceDataPtr++) = pMesh->pIndices [ iIndex+2 ];
				dwIndexCounter+=3;
				dwPolyCounter++;
			}
		}

		// fill multi-material structure
		strcpy ( pMesh->pMultiMaterial [ iTextureIndex ].pName, pMesh->pTextures [ iTextureIndex ].pName );
		memcpy ( &pMesh->pMultiMaterial [ iTextureIndex ].mMaterial, &pMesh->pMaterialBank [ iTextureIndex ].MatD3D, sizeof ( D3DMATERIAL9 ) );

		// index end and polycount
		pMesh->pMultiMaterial [ iTextureIndex ].dwIndexCount = dwPolyCounter*3;
		pMesh->pMultiMaterial [ iTextureIndex ].dwPolyCount = dwPolyCounter;
	}

	// delete old index data
	SAFE_DELETE(pMesh->pIndices);

	// set index array
	pMesh->pIndices = pNewIndiceData;

	// success
	return true;
}

DARKSDK bool ProduceMaterialIndiceTables ( void )
{
	// LEELEE : takes a solid mesh which has multiple textures and
	// produces a list of material sets, each describing what the texture and
	// material are, and the index data to draw them. This index data is stored
	// in a sorted single list, which the material sets reference (so it can be
	// placed in the index buffer as a single block replacing the old index block

	// check the object
	SAFE_MEMORY ( g_pObjectX );

	// go through all frames of object
	for ( int iFrame = 0; iFrame < g_pObjectX->iFrameCount; iFrame++ )
	{
		if ( g_pObjectX->ppFrameList [ iFrame ]->pMesh )
			ProduceMaterialIndiceOfFrame ( g_pObjectX, g_pObjectX->ppFrameList [ iFrame ] );
	}

	// success
	return true;
}

DARKSDK bool LoadModelData ( char* szFilename, sFrame* pFrame, bool bAnim )
{
	// loads the model data

	// check the parameters are valid
	SAFE_MEMORY ( szFilename );
	SAFE_MEMORY ( pFrame );

	// setup variables
	IDirectXFile*			pDXFile       = NULL;	// file interface
	IDirectXFileEnumObject* pDXEnum       = NULL;	// object interface
	IDirectXFileData*		pDXData       = NULL;	// data interface
	char*					szTexturePath = "";		// default texture path

	// create the file object
	if ( FAILED ( DirectXFileCreate ( &pDXFile ) ) )
		return false;

	// register the templates from RM
	if ( FAILED ( pDXFile->RegisterTemplates ( ( LPVOID ) D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES ) ) )
		return false;

//	// leeadd - 171003 - this registers manual templates stored in the xfile itself
//	HANDLE hfile = CreateFile ( szFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
//	if ( hfile != INVALID_HANDLE_VALUE )
//	{
//		DWORD dwBufferSize = GetFileSize ( hfile, 0 );
//		LPSTR pBuffer = new char[dwBufferSize];
//		DWORD bytesread=0;
//		ReadFile(hfile, pBuffer, dwBufferSize, &bytesread, NULL );
//		pDXFile->RegisterTemplates ( ( LPVOID ) pBuffer, dwBufferSize );
//		SAFE_DELETE(pBuffer);
//		CloseHandle ( hfile );
//	}
//	else
//		return false;

	// mem leak dump check
//	_CrtMemDumpAllObjectsSince( &g_crt_mem_state );

	// snapshot
//	_CrtMemCheckpoint ( &g_crt_mem_state );

	// create an enumeration object
	if ( FAILED ( pDXFile->CreateEnumObject ( ( LPVOID ) szFilename, DXFILELOAD_FROMFILE, &pDXEnum ) ) )
		return false;

	// loop through all objects looking for the frames and meshes
	while ( SUCCEEDED ( pDXEnum->GetNextDataObject ( &pDXData ) ) )
	{
		// check the current block of data
		if ( !ParseXFileData ( pDXData, pFrame, szTexturePath, NULL, NULL, bAnim ) )
			return false;
		
		// release it
		SAFE_RELEASE ( pDXData );
	}

	// release dx handles
	SAFE_RELEASE ( pDXEnum );
	SAFE_RELEASE ( pDXFile );

	// return to caller
	return true;
}

DARKSDK bool Delete ( void )
{
	SAFE_DELETE ( g_pObjectX );

	SAFE_DELETE_ARRAY ( g_pVertexList   );
	SAFE_DELETE_ARRAY ( g_pIndexList    );
	SAFE_DELETE_ARRAY ( g_pVertexColorList );
	SAFE_DELETE_ARRAY ( g_pMaterialList );
		
	g_dwVertexCount            = 0;
	g_dwFaceCount              = 0;
	g_dwVertexColorCount       = 0;
	g_dwMaterialListIndexCount = 0;
	g_dwMaterialCount          = 0;
	g_dMaxSkinWeightsPerVertex = 0;
	g_dMaxSkinWeightsPerFace   = 0;
	g_dwBones                  = 0;

	// free resources in bone list
	for ( int i = 0; i < (int)g_BoneList.size(); i++ )
	{
		SAFE_DELETE ( g_BoneList [ i ].pfWeights );
		SAFE_DELETE ( g_BoneList [ i ].pIndices );
		SAFE_DELETE ( g_BoneList [ i ].szName );
	}
	// free resources in material list
	for ( int i = 0; i < (int)g_NewMaterialList.size(); i++ )
		SAFE_DELETE ( g_NewMaterialList [ i ].szFilename );

	// clear lists
	g_BoneList.clear        ( );
	g_NewMaterialList.clear ( );

	return true;
}

#ifdef DARKSDK_COMPILE

bool	Convert		( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
void	Free		( LPSTR );

bool ConvertX ( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize )
{
	return Convert ( pFilename, pBlock, pdwSize );
}

void FreeX ( LPSTR pBlock )
{
	Free ( pBlock );
}

#endif

DARKSDK void SetLegacyModeOn ( void )
{
	g_iLegacyModeIgnoresFVFDATA = 1;
}

DARKSDK void SetLegacyModeOff ( void )
{
	g_iLegacyModeIgnoresFVFDATA = 0;
}

DARKSDK bool Convert ( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize )
{
	// obtain D3D
	GetD3DFromModule ( NULL );

	// load object
	if ( !Load ( pFilename ) )
	{
		// Failed to load
		Free ( NULL );
		return false;
	}

	// create block
	if ( !DBOConvertObjectToBlock ( g_pObjectX, pBlock, pdwSize ) )
	{
		// Failed to create
		Free ( NULL );
		return false;
	}

	// okay
	return true;
}

DARKSDK void Free ( LPSTR pBlock )
{
	// delete block
	SAFE_DELETE(pBlock);

	// free object
	Delete();

	// free local DLL hook
	if ( g_hSetup )
	{
		FreeLibrary ( g_hSetup );
		g_hSetup = NULL;
	}
}

DARKSDK bool XFILE_GetTemplateInfo ( IDirectXFileData* pDataObj, const GUID** guidType, char** szName )
{
	// gets the template info - identifier and name

	// make sure we have a valid DX file interface
	SAFE_MEMORY ( pDataObj );

	// set size to 0
	DWORD dwSize = 0;

	// get the template type
	if ( FAILED ( pDataObj->GetType ( guidType ) ) )
		return false;

	// get the template name ( if any )
	if ( FAILED ( pDataObj->GetName ( NULL, &dwSize ) ) )
		return false;

	if ( dwSize )
	{
		if ( szName )
		{
			// allocate memory and copy the name across
			*szName = new char [ dwSize ];

			// check the memory was created
			SAFE_MEMORY ( *szName );

			// finally get the name
			pDataObj->GetName ( *szName, &dwSize );
		}
	}
	else
	{
		if ( szName )
		{
			// if there isn't a name then give it a default
			*szName = new char [ 9 ];

			// check the memory
			SAFE_MEMORY ( *szName );

			// copy the default name across
			strcpy ( *szName, "$NoName$" );
		}
	}
	
	// all okay
	return true;
}

DARKSDK bool XFILE_CreateFrame ( sFrame** ppFrame, sFrame* pParentFrame, sFrame** ppSubFrame, char* szName )
{
	// create a new frame structure
	*ppFrame = new sFrame;

	SAFE_MEMORY ( *ppFrame );
	SAFE_MEMORY ( pParentFrame );
	SAFE_MEMORY ( *ppSubFrame );

	// store the name
	strcpy ( ( *ppFrame )->szName, szName );
	szName = NULL;

	// add to parent frame
	( *ppFrame )->pParent  = pParentFrame;
	( *ppFrame )->pSibling = pParentFrame->pChild;
	pParentFrame->pChild   = *ppFrame;

	// set sub frame parent
	*ppSubFrame = *ppFrame;
	
	return true;
}

DARKSDK bool XFILE_CreateTransformationMatrix ( IDirectXFileData* pDataObj, sFrame* pParentFrame )
{
	// get the frame transformation matrix

	// check parameters are valid
	SAFE_MEMORY ( pDataObj );
	SAFE_MEMORY ( pParentFrame );

	// setup a matrix and size variable
	D3DXMATRIX*	pFrameMatrix = NULL;
	DWORD		dwSize       = 0;

	// get the data from the X file interface
	if ( FAILED ( pDataObj->GetData ( NULL, &dwSize, ( PVOID* ) &pFrameMatrix ) ) )
		return false;

	// store the matrix in the parent frame
	pParentFrame->matOriginal = *pFrameMatrix;

	return true;
}

DARKSDK void XFILE_NEW_MakeTriangleIndicesFromFaceData ( DWORD* pdwFaceCount, DWORD** ppTriIndices, DWORD** ppOldRef, DWORD* pOriginalIndices )
{
	// if face data available
	if ( *pdwFaceCount )
	{
		// read index for original buffer
		DWORD readindex = 0;
		vector < DWORD > indices;
		DWORD dwNewFaceCount = 0;

		// go through original index data
		for ( DWORD faceindex = 0; faceindex < *pdwFaceCount; faceindex++ )
		{
			// this is number of indices per face
			int iIndicePerFace = pOriginalIndices [ readindex++ ];

			// only handle triangles and above
			if ( iIndicePerFace>=3 )
			{
				// store first two indexes of first basic triangle
				int readindexA = pOriginalIndices [ readindex+0 ];
				int readindexB = pOriginalIndices [ readindex+2 ];

				// first three indices are basic triangle
				for ( int i = 0; i < 3; i++ )
					indices.push_back ( pOriginalIndices [ readindex++ ] );

				// additional triangles created (A+B+NEW)
				for ( int i=3; i<iIndicePerFace; i++ )
				{
					indices.push_back ( readindexA );
					indices.push_back ( readindexB );
					readindexB = pOriginalIndices [ readindex ];
					indices.push_back ( pOriginalIndices [ readindex++ ] );
				}

				// Count new faces
				dwNewFaceCount += (iIndicePerFace-2);
			}
		}

		// create old ref array for material assignments later
		if ( ppOldRef )
		{
			g_pFaceIndexOldRef = new DWORD [ dwNewFaceCount ];
			DWORD dwFaceIndex=0, dwNewFaceIndex=0;

			// mike - 020206 - addition for vs8
			DWORD faceindex = 0;

			// go through original index data again to record face indexes (for oldref used in material assigns)
			for ( readindex=0, faceindex=0; faceindex < *pdwFaceCount; faceindex++ )
			{
				// this is number of indices per face
				int iIndicePerFace = pOriginalIndices [ readindex++ ];

				// only handle triangles and above
				if ( iIndicePerFace>=3 )
				{
					// advance read index
					readindex+=iIndicePerFace;

					// Place face index in all triangles made from original face
					for ( int fill=0; fill<(iIndicePerFace-2); fill++ )
						g_pFaceIndexOldRef [ dwNewFaceIndex++ ] = dwFaceIndex;
					
					// next face
					dwFaceIndex++;
				}
			}
		}

		// use list of new indices to make new index buffer
		SAFE_DELETE(*ppTriIndices);
		DWORD dwIndexBufferSize = indices.size();
		*ppTriIndices = new DWORD [ dwIndexBufferSize ];
		*pdwFaceCount = dwNewFaceCount;
		for ( DWORD index = 0; index < dwIndexBufferSize; index++ )
			*((*ppTriIndices)+index) = indices [ index ];
	}
}

DARKSDK bool XFILE_NEW_GetVerticesAndIndices ( IDirectXFileData* pDataObject, sFrame* pFrame )
{
	// extracts vertices and indices from the data object

	// check the parameters are valid
	if ( !pDataObject || !pFrame )
		return false;

	// local variables
	DWORD     dwSize    = 0;		// size of data
	DWORD*    pData     = NULL;		// initial pointer to data
	float*    pVertices = NULL;		// used for vertices
	DWORD*	  pIndices  = NULL;		// for indices
	int		  i         = 0;		// i, j and k are for loops
	int		  j         = 0;
	int		  k         = 0;

	// get the next data block
	pDataObject->GetData ( NULL, &dwSize, ( void** ) &pData );

	// ensure the data is valid
	if ( !pData )
		return false;
	
	// get vertex count and pointer to vertices
	g_dwVertexCount  = *( pData + 0 );				// count is at start of block
	pVertices        = ( float* ) ( pData + 1 );	// starting vertex is + 1

	// allocate vertex array
	g_pVertexList = new sNewVertex [ g_dwVertexCount ];
	
	// check memory
	if ( !g_pVertexList )
		return false;

	// clear out vertex list
	memset ( g_pVertexList, 0, sizeof ( sNewVertex ) * g_dwVertexCount );
	
	// get each vertex from the pointer
	for ( i = 0; i < ( int ) g_dwVertexCount; i++ )
	{
		// store x, y and z, increment pointer each time to move to next piece of data
		g_pVertexList [ i ].vecPosition.x = *pVertices;		pVertices++;
		g_pVertexList [ i ].vecPosition.y = *pVertices;		pVertices++;
		g_pVertexList [ i ].vecPosition.z = *pVertices;		pVertices++;
	}

	// get index count
	g_dwFaceCount  = *( pData + ( g_dwVertexCount * 3 ) + 1 );				// count is + 1 from last vertex
	pIndices       = ( DWORD* ) ( pData + ( g_dwVertexCount * 3 ) + 2 );	// starting index is + 2 from last vertex

	// allocate indices (as triangles)
	SAFE_DELETE ( g_pIndexList );
	XFILE_NEW_MakeTriangleIndicesFromFaceData ( &g_dwFaceCount, &g_pIndexList, &g_pFaceIndexOldRef, pIndices );

	// finished, return back
	return true;
}

DARKSDK bool XFILE_NEW_GetNormals ( IDirectXFileData* pSubData, sFrame* pFrame )
{
	// get normals from data if they exist

	// check the parameters are valid
	if ( !pSubData || !pFrame )
		return false;

	// local variables
	DWORD*				pData         = NULL;	// pointer to data
	DWORD				dwSize        = 0;		// size of data
	DWORD*				pIndices      = NULL;
	float*				pNormals      = NULL;
	int					i             = 0;		// i, j and k are for loops
	int					j             = 0;
	int					k             = 0;

	// Rest globals
	g_dwNormalCount		= 0;
	g_dwNormalFaceCount	= 0;
	g_pNormalIndexList	= NULL;
	g_pNormalList		= NULL;

	// now extract data pointer
	pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );

	// check it's okay
	if ( !pData )
		return false;

	// we may well have normals which have an index list, we need to
	// get the list of normals and then add this in based on the
	// indices into the main vertex list

	// set up initial data
	g_dwNormalCount = *( pData + 0 );				// number of normals
	pNormals      = ( float* ) ( pData + 1 );	// pointer to normal data

	// allocate array to store normals
	g_pNormalList = new D3DXVECTOR3 [ g_dwNormalCount ];

	// store normals in list
	for ( i = 0; i < ( int ) g_dwNormalCount; i++ )
	{
		g_pNormalList [ i ].x = *pNormals;	pNormals++;
		g_pNormalList [ i ].y = *pNormals;	pNormals++;
		g_pNormalList [ i ].z = *pNormals;	pNormals++;
	}

	// now we get the number of indices for the normal list
	g_dwNormalFaceCount = *( pData + ( g_dwNormalCount * 3 ) + 1 );
	pIndices    = ( DWORD* ) ( pData + ( g_dwNormalCount * 3 ) + 2 );

	// allocate list (as triangles)
	SAFE_DELETE ( g_pNormalIndexList );
	XFILE_NEW_MakeTriangleIndicesFromFaceData ( &g_dwNormalFaceCount, &g_pNormalIndexList, NULL, pIndices );

	return true;
}

DARKSDK void XFILE_NEW_CopyNormalsToVertices ( void )
{
	// if normal face count and vertex face count differ, something odd..
	if ( g_dwNormalFaceCount != g_dwFaceCount )
		return;

	// vertex data may be expanded if more normals
	if ( g_dwNormalCount > g_dwVertexCount )
	{
		// leefix - 200104 - vertex data is master, and normals copy over
		sNewVertex* pNewVertexData = new sNewVertex [ g_dwVertexCount ];
		DWORD* pNewIndexData = new DWORD [ g_dwFaceCount * 3 ];
		memcpy ( pNewVertexData, g_pVertexList, sizeof(sNewVertex)*g_dwVertexCount );
		memcpy ( pNewIndexData, g_pIndexList, sizeof(DWORD)*g_dwFaceCount*3 );

		// go through each face in face list
		for ( DWORD dwFaceBase = 0; dwFaceBase < g_dwFaceCount*3; dwFaceBase++ )
		{
			// vertices of face
			DWORD dwVIndex = g_pIndexList [ dwFaceBase ];

			// normals of face
			DWORD dwNIndex = g_pNormalIndexList [ dwFaceBase ];

			// copy the normal over
			pNewVertexData [ dwVIndex ].vecNormal.x = g_pNormalList [ dwNIndex ].x;
			pNewVertexData [ dwVIndex ].vecNormal.y = g_pNormalList [ dwNIndex ].y;
			pNewVertexData [ dwVIndex ].vecNormal.z = g_pNormalList [ dwNIndex ].z;
		}

		/* leefix - 300109 - hud.x model had missing polys as last seven normals had same number in faceindex data
		// create new vertex data allocation (based on normals data)
		sNewVertex* pNewVertexData = new sNewVertex [ g_dwNormalCount ];
		DWORD* pNewIndexData = new DWORD [ g_dwFaceCount * 3 ];

		// can simply copy over the index table from the normals data (the new template for the index)
		memcpy ( pNewIndexData, g_pNormalIndexList, sizeof(DWORD)*g_dwFaceCount*3 );

		// copy existing vertex data to new data
		for ( int i = 0; i < ( int ) g_dwFaceCount * 3; i++ )
		{
			// NEW vertex index based on normal data
			DWORD dwNewExpandedIndex = g_pNormalIndexList [ i ];

			// copy the vertex data from original position
			DWORD dwOldIndex = g_pIndexList [ i ];
			pNewVertexData [ dwNewExpandedIndex ] = g_pVertexList [ dwOldIndex ];

			// copy the normal vector (using the normal face data for both indexes)
			pNewVertexData [ dwNewExpandedIndex ].vecNormal.x = g_pNormalList [ dwNewExpandedIndex ].x;
			pNewVertexData [ dwNewExpandedIndex ].vecNormal.y = g_pNormalList [ dwNewExpandedIndex ].y;
			pNewVertexData [ dwNewExpandedIndex ].vecNormal.z = g_pNormalList [ dwNewExpandedIndex ].z;
		}
		g_dwVertexCount = g_dwNormalCount;
		*/

		// delete old and replace with new data
		SAFE_DELETE ( g_pVertexList );
		SAFE_DELETE ( g_pIndexList );
		g_pVertexList = pNewVertexData;
		g_pIndexList = pNewIndexData;
	}
	else
	{
		// copy all normals direct to vertice data
		for ( int i = 0; i < ( int ) g_dwNormalFaceCount * 3; i++ )
		{
			g_pVertexList [ g_pIndexList [ i ] ].vecNormal.x = g_pNormalList [ g_pNormalIndexList [ i ] ].x;
			g_pVertexList [ g_pIndexList [ i ] ].vecNormal.y = g_pNormalList [ g_pNormalIndexList [ i ] ].y;
			g_pVertexList [ g_pIndexList [ i ] ].vecNormal.z = g_pNormalList [ g_pNormalIndexList [ i ] ].z;
		}
	}

	// release any previously allocated memory
	SAFE_DELETE_ARRAY ( g_pNormalList );
	SAFE_DELETE_ARRAY ( g_pNormalIndexList  );
}

DARKSDK bool XFILE_NEW_GetTextureCoordinates ( IDirectXFileData* pSubData, sFrame* pFrame )
{
	// extract texture coordinates from model

	// check the parameters are valid
	if ( !pSubData || !pFrame )
		return false;

	// local variables
	DWORD*	pData          = NULL;	// pointer to data
	DWORD	dwSize         = 0;		// size of data
	int		i              = 0;		// i is for loops
	DWORD	dwTextureCount = 0;		// number of texture coordinates
	float*	pCoordinates   = NULL;	// pointer to coordinates

	// now extract data pointer
	pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );

	// check it's okay
	if ( !pData )
		return false;

	// get access to data from pointers
	dwTextureCount = *( pData + 0 );
	pCoordinates   = ( float* ) ( pData + 1 );

	// now we can copy the texture coordinates across to the vertex list
	for ( i = 0; i < ( int ) dwTextureCount; i++ )
	{
		g_pVertexList [ i ].tu = *pCoordinates;		pCoordinates++;
		g_pVertexList [ i ].tv = *pCoordinates;		pCoordinates++;
	}

	// finished
	return true;
}

DARKSDK bool XFILE_NEW_GetFVFData ( IDirectXFileData* pSubData, sFrame* pFrame )
{
	// extract FVFDATA from model (can be second stage texture UV DATA)
	// FOR NOW, WE EXTRACT AND TREAT AS SECOND STAGE UV DATA
	// OR, WE EXTRACT AND TREAT AS SECOND+THIRD STAGE UV DATA

	// check the parameters are valid
	if ( !pSubData || !pFrame )
		return false;

	// now extract data pointer
	DWORD*	pData          = NULL;	// pointer to data
	DWORD	dwSize         = 0;		// size of data
	pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );
	if ( !pData ) return false;

	// get access to data from pointers
	DWORD dwFVF = *( pData + 0 );
	DWORD dwDWORDS = *( pData + 1 );
	DWORD* pCoordinates   = ( DWORD* ) ( pData + 2 );

	// cordinates per vertex
	int iCoordsPerVertex = 2;
	g_iNumberOfExtraUVDataStagesAvailable = 1;
	if ( dwFVF==770 )
	{
		g_iNumberOfExtraUVDataStagesAvailable = 2;
		iCoordsPerVertex = 4;
	}

	// now we can copy across to the temp vertex list
	DWORD dwIndex = 0;
	for ( int i = 0; i < ( int ) dwDWORDS; i+=iCoordsPerVertex )
	{
		if ( dwFVF==258 ) 
		{
			// second stage
			DWORD dwU = *(pCoordinates++);
			DWORD dwV = *(pCoordinates++);
			float fU = *(float*)&dwU;
			float fV = *(float*)&dwV;
			g_pVertexList [ dwIndex ].tu2 = fU;
			g_pVertexList [ dwIndex ].tv2 = fV;
		}
		if ( dwFVF==770 ) 
		{
			// second+third stage
			DWORD dwU1 = *(pCoordinates++);
			DWORD dwV1 = *(pCoordinates++);
			DWORD dwU2 = *(pCoordinates++);
			DWORD dwV2 = *(pCoordinates++);
			float fU1 = *(float*)&dwU1;
			float fV1 = *(float*)&dwV1;
			float fU2 = *(float*)&dwU2;
			float fV2 = *(float*)&dwV2;
			g_pVertexList [ dwIndex ].tu2 = fU1;
			g_pVertexList [ dwIndex ].tv2 = fV1;
			g_pVertexList [ dwIndex ].tu3 = fU2;
			g_pVertexList [ dwIndex ].tv3 = fV2;
		}
		dwIndex++;
	}

	// U75 - 080809 - legacy loader ignores FVFDATA
	if ( g_iLegacyModeIgnoresFVFDATA==1 )
	{
		// solves FPSC source crashing when load an FVF=530 X as DBO
		g_iNumberOfExtraUVDataStagesAvailable = 0;
	}

	// finished
	return true;
}

DARKSDK bool XFILE_NEW_GetVertexColors ( IDirectXFileData* pSubData, sFrame* pFrame )
{
	// extract vertex colour from model

	// check the parameters are valid
	if ( !pSubData || !pFrame )
		return false;

	// get size and data ptr
	DWORD dwSize = 0;
	DWORD* pData = NULL;
	pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );

	// check it's okay
	if ( !pData )
		return false;

	// get access to data from pointers
	g_dwVertexColorCount = *( pData + 0 );
	sVertexColor* pVColor = ( sVertexColor* ) ( pData + 1 );

	// copy vertex data into global store for combining later
	SAFE_DELETE(g_pVertexColorList);
	g_pVertexColorList = new sVertexColor [ g_dwVertexColorCount ];
	memcpy ( g_pVertexColorList, pVColor, g_dwVertexColorCount*sizeof(sVertexColor) );

	// finished
	return true;
}

DARKSDK bool XFILE_NEW_GetMaterialName ( IDirectXFileData* pSubData, sNewMaterial* pMaterial )
{
	// see if we have a name for the material

	// check parameters
	if ( !pSubData || !pMaterial )
		return false;

	// local variables
	IDirectXFileObject*	pSubObjectTexture = NULL;	// object interface
	IDirectXFileData*	pSubDataTexture   = NULL;	// object data
	const GUID*			guidTypeTexture   = NULL;	// identifier
	char*				szNameTexture     = NULL;	// name

	// get sub object
	if ( SUCCEEDED ( pSubData->GetNextObject ( &pSubObjectTexture ) ) )
	{
		// determine object type
		if ( SUCCEEDED ( pSubObjectTexture->QueryInterface ( DBIID_IDirectXFileData, ( void** ) &pSubDataTexture ) ) )
		{
			// get guid and name
			if ( !XFILE_GetTemplateInfo ( pSubDataTexture, &guidTypeTexture, &szNameTexture ) )
				return false;

			// see if we have a texture filename
			if ( *guidTypeTexture == TID_D3DRMTextureFilename )
			{
				// locals
				DWORD*	pData  = NULL;	// data
				DWORD   dwSize = 0;		// size
				char*   szName = NULL;	// name

				// get the data
				pSubDataTexture->GetData ( NULL, &dwSize, ( void** ) &pData );

				// store pointer to name
				szName = ( char* ) *( pData + 0 );

				// allocate memory to hold name
				// LEEFIX : 301003 - need extra byte for null termionator :)
				pMaterial->szFilename = new char [ (strlen ( szName ) * sizeof ( char ))+1 ];

				if ( !pMaterial->szFilename )
					return false;

				// finally copy data
				strcpy ( pMaterial->szFilename, szName );
			}

			// MIKE - 311003
			SAFE_DELETE_ARRAY ( szNameTexture );
			SAFE_RELEASE ( pSubDataTexture );
		}

		SAFE_RELEASE ( pSubObjectTexture );
	}

	return true;
}

DARKSDK bool XFILE_NEW_GetMaterialData ( IDirectXFileData* pSubDataMaterial )
{
	// locals
	DWORD*			pData    = NULL;	// pointer to data
	DWORD			dwSize   = 0;		// size of data
	sNewMaterial	material;			// to store material information

	memset ( &material, 0, sizeof ( material ) );

	// extract data pointer
	pSubDataMaterial->GetData ( NULL, &dwSize, ( void** ) &pData );

	// get pointer to data
	sNewMaterial* pPointer = ( sNewMaterial* ) ( pData + 0 );

	// copy material data
	memcpy ( &material, pPointer, sizeof ( sNewMaterial ) );

	material.szFilename = NULL;

	// look for a name
	XFILE_NEW_GetMaterialName ( pSubDataMaterial, &material );

	// send data to back of list
	g_NewMaterialList.push_back ( material );

	// all done
	return true;
}

DARKSDK bool XFILE_NEW_GetOptionalMaterialData ( IDirectXFileData* pSubData )
{
	// search for material data

	if ( !pSubData )
		return false;

	// local variables
	IDirectXFileObject*	pSubObjectMaterial = NULL;		// object interface
	IDirectXFileData*	pSubDataMaterial   = NULL;		// data interface
	const GUID*			guidTypeMaterial   = NULL;		// identifier
	char*				szNameMaterial     = NULL;		// name
	bool				bSearch            = true;		// search flag

	// loop round looking for material data
	while ( bSearch )
	{
		// get the sub object
		if ( SUCCEEDED ( pSubData->GetNextObject ( &pSubObjectMaterial ) ) )
		{
			// determine if data obj or reference
			pSubDataMaterial = NULL;
			if ( FAILED ( pSubObjectMaterial->QueryInterface ( DBIID_IDirectXFileData, ( void** ) &pSubDataMaterial ) ) )
			{
				IDirectXFileDataReference* pSubDataRef = NULL;
				if ( SUCCEEDED ( pSubObjectMaterial->QueryInterface ( DBIID_IDirectXFileDataReference, ( void** ) &pSubDataRef ) ) )
				{
					pSubDataRef->Resolve ( &pSubDataMaterial ); 
					SAFE_RELEASE ( pSubDataRef ); 
				}
			}

			// if valid data
			if ( pSubDataMaterial )
			{
				// get information about the type
				if ( XFILE_GetTemplateInfo ( pSubDataMaterial, &guidTypeMaterial, &szNameMaterial ) )
				{
					// do we have a material
					if ( *guidTypeMaterial == TID_D3DRMMaterial )
					{
						XFILE_NEW_GetMaterialData ( pSubDataMaterial );
					}

					// MIKE - 311003
					SAFE_DELETE_ARRAY ( szNameMaterial );
				}

				SAFE_RELEASE ( pSubDataMaterial );
			}
			else
				bSearch = false;

			SAFE_RELEASE ( pSubObjectMaterial );
		}
		else
			bSearch = false;
	}

	// all done
	return true;
}

DARKSDK bool XFILE_NEW_GetMeshMaterialList ( IDirectXFileData* pSubData, sFrame* pFrame )
{
	// 111005

	// get the material list indices from the data

	if ( !pSubData || !pFrame )
		return false;

	// local variables
	DWORD*	pData    = NULL;	// pointer to data
	DWORD	dwSize   = 0;		// size of data
	DWORD*  pIndices = NULL;	// pointer to index data
	int		i        = 0;		// used for loops

	// now extract data pointer
	pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );

	// store properties
	g_dwMaterialCount          = *( pData + 0 );							// number of materials
	g_dwMaterialListIndexCount = *( pData + 1 );							// number of indices
	g_pMaterialList            = new DWORD [ g_dwMaterialListIndexCount ];	// allocate list
	pIndices                   = ( DWORD* ) ( pData + 2 );					// pointer to indices

	// check newly allocated memory
	if ( !g_pMaterialList )
		return false;

	// copy material list indices
	for ( i = 0; i < ( int ) g_dwMaterialListIndexCount; i++ )
	{
		g_pMaterialList [ i ] = *pIndices;
		pIndices++;
	}

	// lee - 171203 - update material list if face data expanded (due to quads and other non-tri faces)
	#ifndef DARKSDK_COMPILE
	if ( g_pFaceIndexOldRef )
	{
		// create new material array and fill with correct data
		DWORD dwNewMaterialListIndexCount = g_dwFaceCount;
		DWORD* pNewMaterialList = new DWORD [ dwNewMaterialListIndexCount ];
		for ( DWORD newindex=0; newindex<dwNewMaterialListIndexCount; newindex++ )
		{
			int oldindex = g_pFaceIndexOldRef [ newindex ];

			// mike - 310305 - stop overflow
			if ( oldindex >= (int)g_dwMaterialListIndexCount )
				oldindex = g_dwMaterialListIndexCount - 1;

			pNewMaterialList [ newindex ] = g_pMaterialList [ oldindex ];
		}

		// remove old material array and replace with new
		SAFE_DELETE(g_pMaterialList);
		g_pMaterialList = pNewMaterialList;
		g_dwMaterialListIndexCount = dwNewMaterialListIndexCount;

		// free usages
		SAFE_DELETE_ARRAY ( g_pFaceIndexOldRef );
	}
	#endif

	// leefix - 041103 - consider materials reference by face and if we have 
	// processed quads or greater, those face references have changed!
	/*
	if ( g_pFaceIndexOldRef )
	{
		// only works for QUADS (2poly) - later must cater for 5,6,7 poly-etc
		DWORD newindex=0;
		DWORD dwNewMaterialListIndexCount = 0;
		dwNewMaterialListIndexCount = g_dwFaceCount;
		DWORD* pNewMaterialList = new DWORD [ dwNewMaterialListIndexCount ];
		for ( DWORD j = 0; j < ( int ) g_dwMaterialListIndexCount; j++ )
		{
			pNewMaterialList [ newindex+0 ] = g_pMaterialList [ j ];
			pNewMaterialList [ newindex+1 ] = g_pMaterialList [ j ];
			newindex+=2;
		}
		g_dwMaterialListIndexCount = dwNewMaterialListIndexCount;
		SAFE_DELETE(g_pMaterialList);
		g_pMaterialList = pNewMaterialList;

		// rather redundant as really a quad equates to two identical index references
		// might need this data though when we go for 5,6,7,8 sided polygons..
		SAFE_DELETE_ARRAY ( g_pFaceIndexOldRef );
	}
	*/

	// look for optional material and texture data
	XFILE_NEW_GetOptionalMaterialData ( pSubData );

	// all done
	return true;
}

DARKSDK bool XFILE_NEW_GetSkinMeshHeader ( IDirectXFileData* pSubData, sFrame* pFrame )
{
	// get the skin mesh header data

	if ( !pSubData || !pFrame )
		return false;

	// local variables
	WORD*	pData    = NULL;	// pointer to data
	DWORD	dwSize   = 0;		// size of data

	// get pointer to data and size
	pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );

	// MIKE - 141003 - this is where we can get a problem, need to cast to
	//               - WORD or DWORD dependant on data size
	if ( dwSize == 12 )
	{
		DWORD*	pData  = NULL;
		DWORD	dwSize = 0;

		pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );

		g_dMaxSkinWeightsPerVertex = (WORD) *( pData + 0 );
		g_dMaxSkinWeightsPerFace   = (WORD) *( pData + 1 );
		g_dwBones                  = *( pData + 2 );
	}
	else
	{
		WORD* pData  = NULL;
		DWORD dwSize = 0;

		pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );

		g_dMaxSkinWeightsPerVertex = *( pData + 0 );
		g_dMaxSkinWeightsPerFace   = *( pData + 1 );
		g_dwBones                  = *( pData + 2 );
	}

	// finished
	return true;
}

DARKSDK bool XFILE_NEW_GetSkinWeights ( IDirectXFileData* pSubData, sFrame* pFrame )
{
	// get information on the bone, the structure contains
	//		
	//		name
	//		number of weights
	//		an array of indices [ number of weights ]
	//		an array of weights [ number of weights ]
	//		transform matrix

	// check parameters
	if ( !pSubData || !pFrame )
		return false;

	// local variables
	DWORD*		pData        = NULL;	// pointer to data
	DWORD		dwSize       = 0;		// size of data
	int			i			 = 0;		// used for temp loops
	char*       szName       = NULL;	// name of the bone
	DWORD*		pIndices     = NULL;	// indices into vertices
	float*		pWeights     = NULL;	// weights
	sNewBones	bone;					// bone structure to be filled in

	// now extract data pointer
	pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );

	// check data is valid
	if ( !pData )
		return false;

	// clear out bone structure
	memset ( &bone, 0, sizeof ( sNewBones ) );

	// get pointer to name
	szName = ( char* ) *( pData + 0 );

	// allocate new memory to hold name
	bone.szName = new char [ strlen ( szName ) * sizeof ( char ) + 1 ];

	// check memory
	if ( !bone.szName )
		return false;

	// copy name across to bone structure
	strcpy ( bone.szName, szName );

	// store number of weights
	bone.dwNumWeights = *( pData + 1 );
	
	// see if we have any weights
	if ( bone.dwNumWeights )
	{
		// allocate index and weight arrays
		bone.pIndices  = new DWORD [ bone.dwNumWeights ];
		bone.pfWeights = new float [ bone.dwNumWeights ];

		// ensure memory is valid
		if ( !bone.pIndices || !bone.pfWeights )
			return false;

		// store pointer to indices and weights
		pIndices = ( pData + 2 );
		pWeights = ( float* ) ( pData + 2 + bone.dwNumWeights );

		// copy bone indices across
		for ( i = 0; i < (int)bone.dwNumWeights; i++ )
		{
			bone.pIndices [ i ] = *pIndices;
			pIndices++;
		}

		// and now copy weights across
		for ( i = 0; i < (int)bone.dwNumWeights; i++ )
		{
			bone.pfWeights [ i ] = *pWeights;
			pWeights++;
		}

		// copy matrix
		memcpy ( &bone.matrix, ( D3DXMATRIX* ) ( pData + 2 + ( bone.dwNumWeights * 2 ) ), sizeof ( D3DXMATRIX ) );
	}
	else
	{
		// if we have no weights matrix is directly after number of weights so + 2
		memcpy ( &bone.matrix, ( D3DXMATRIX* ) ( pData + 2 ), sizeof ( D3DXMATRIX ) );
	}
	
	// send bone to back of list
	g_BoneList.push_back ( bone );
	
	// all done
	return true;
}

DARKSDK bool XFILE_NEW_GetDeclData ( IDirectXFileData* pSubData, sFrame* pFrame )
{
	// DWORD nElements;
	// array VertexElement Elements[nElements];
	// DWORD nDWords;
	// array DWORD data[nDWords];
	//
	// VertexElement
	// {
	//  DWORD Type;
	//  DWORD Method;
	//  DWORD Usage;
	//  DWORD UsageIndex;
	// }
	#define MAX_FVF_DECL_SIZE 65
	D3DVERTEXELEMENT9 pVertexDecl [ MAX_FVF_DECL_SIZE ];

	// check parameters
	if ( !pSubData || !pFrame )
		return false;

	// now extract data pointer
	DWORD*		pData        = NULL;	// pointer to data
	DWORD		dwSize       = 0;		// size of data
	pSubData->GetData ( NULL, &dwSize, ( void** ) &pData );

	// check data is valid
	if ( !pData ) return false;

	// element count
	DWORD dwVertexDeclCount			= *( pData++ );

	// element data
	for ( DWORD n=0; n<dwVertexDeclCount; n++ )
	{
		DWORD dwElementType			= *( pData+0 );
		DWORD dwElementMethod		= *( pData+1 );
		DWORD dwElementUsage		= *( pData+2 );
		DWORD dwElementUsageIndex	= *( pData+3 );
		pVertexDecl[n].Type			= (BYTE)dwElementType;
		pVertexDecl[n].Method		= (BYTE)dwElementMethod;
		pVertexDecl[n].Usage		= (BYTE)dwElementUsage;
		pVertexDecl[n].UsageIndex	= (BYTE)dwElementUsageIndex;
		pData+=4;
	}

	// DWORDs count
	DWORD dwVertexDeclDataCount =	*( pData++ );

	// DWORDs data
	DWORD* pVertexDeclData = new DWORD [ dwVertexDeclDataCount ];
	for ( DWORD n=0; n<dwVertexDeclDataCount; n++ )
		pVertexDeclData[n]			= *( pData++ );

	// now all data extracted, we can extract it into meaningful values
	// and place them in the arrays used to collect normals, texture coords, diffuse, etc
	// go through declaration data in order specified
	DWORD* pDWORDPtr = pVertexDeclData;
	for ( DWORD vi=0; vi<g_dwVertexCount; vi++ )
	{
		for ( DWORD n=0; n<dwVertexDeclCount; n++ )
		{
			switch ( pVertexDecl[n].Usage )
			{
				case D3DDECLUSAGE_POSITION:
					g_pVertexList [ vi ].vecPosition.x = *(float*)pDWORDPtr; pDWORDPtr++;
					g_pVertexList [ vi ].vecPosition.y = *(float*)pDWORDPtr; pDWORDPtr++;
					g_pVertexList [ vi ].vecPosition.z = *(float*)pDWORDPtr; pDWORDPtr++;
				break;
				case D3DDECLUSAGE_POSITIONT :	pDWORDPtr+=4; break;
				case D3DDECLUSAGE_PSIZE :		pDWORDPtr+=1; break;
				case D3DDECLUSAGE_NORMAL:
					g_pVertexList [ vi ].vecNormal.x = *(float*)pDWORDPtr; pDWORDPtr++;
					g_pVertexList [ vi ].vecNormal.y = *(float*)pDWORDPtr; pDWORDPtr++;
					g_pVertexList [ vi ].vecNormal.z = *(float*)pDWORDPtr; pDWORDPtr++;
				break;
				case D3DDECLUSAGE_COLOR:
					if ( g_pVertexColorList==NULL ) g_pVertexColorList = new sVertexColor [ g_dwVertexColorCount ];
					g_pVertexColorList [ vi ].index = vi;
					g_pVertexColorList [ vi ].color = *(sColorRGBA*)pDWORDPtr; pDWORDPtr++;
				break;
				case D3DDECLUSAGE_TEXCOORD:
				{
					switch ( pVertexDecl[n].Type )
					{
						case D3DDECLTYPE_FLOAT2:
							g_pVertexList [ vi ].tu = *(float*)pDWORDPtr; pDWORDPtr++;
							g_pVertexList [ vi ].tv = *(float*)pDWORDPtr; pDWORDPtr++;
						break;
						case D3DDECLTYPE_FLOAT3:
							g_pVertexList [ vi ].tu = *(float*)pDWORDPtr; pDWORDPtr++;
							g_pVertexList [ vi ].tv = *(float*)pDWORDPtr; pDWORDPtr++;
							pDWORDPtr++;
						break;
						case D3DDECLTYPE_FLOAT4:
							g_pVertexList [ vi ].tu = *(float*)pDWORDPtr; pDWORDPtr++;
							g_pVertexList [ vi ].tv = *(float*)pDWORDPtr; pDWORDPtr++;
							pDWORDPtr++;
							pDWORDPtr++;
						break;
					}
				}
				break;
				case D3DDECLUSAGE_TANGENT :		pDWORDPtr+=4; break;
				case D3DDECLUSAGE_BINORMAL :	pDWORDPtr+=4; break;
			}
		}
	}

	// free usages
	SAFE_DELETE ( pVertexDeclData );

	// all done
	return true;
}

DARKSDK bool XFILE_NEW_CreateMeshInFrame ( sFrame* pFrame, DWORD dwNumVertices, DWORD dwNumFaces )
{
	// LEELEE - Ensure when change IndiceCount, that this function still gets FACE COUNT!
	// check we have a valid frame
	SAFE_MEMORY ( pFrame );

	// ceate a new mesh to hold the data
	pFrame->pMesh = new sMesh;

	// ensure the mesh was created
	SAFE_MEMORY ( pFrame->pMesh );

	// mesh 'can' store 32bit index data temporarily for later conversion to vertex only.
	// and should only use 32bit if going to do a vertex expanding (due to >16bit verts)
	bool b32BITIndexData=false;
	if ( dwNumVertices > 0xFFFF )
		b32BITIndexData=true;

	// set standard FVF format
	pFrame->pMesh->dwFVF			= D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	pFrame->pMesh->dwFVFSize		= 32;

	// an extra set of UV Data is available
	if ( g_iNumberOfExtraUVDataStagesAvailable==1 )
	{
		pFrame->pMesh->dwFVF		= D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;
		pFrame->pMesh->dwFVFSize	+= 8;
	}
	if ( g_iNumberOfExtraUVDataStagesAvailable==2 )
	{
		pFrame->pMesh->dwFVF		= D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3;
		pFrame->pMesh->dwFVFSize	+= 16;
	}

	// add additional FVF flags as required
	if ( g_pVertexColorList )
	{
		pFrame->pMesh->dwFVF		|= D3DFVF_DIFFUSE;
		pFrame->pMesh->dwFVFSize	+= 4;
	}

	// setup mesh properties and allocate memory - index buffer
	pFrame->pMesh->dwVertexCount	= dwNumVertices;
	pFrame->pMesh->dwIndexCount		= dwNumFaces * 3;
	DWORD dwVertexDataSize = pFrame->pMesh->dwFVFSize * dwNumVertices;
	pFrame->pMesh->pVertexData		= ( BYTE* ) new char [ dwVertexDataSize ];
	if ( b32BITIndexData )
		pFrame->pMesh->pIndices			= (WORD*)new DWORD [ pFrame->pMesh->dwIndexCount ];
	else
		pFrame->pMesh->pIndices			= new WORD [ pFrame->pMesh->dwIndexCount ];

	// setup mesh drawing properties
	pFrame->pMesh->iPrimitiveType	= D3DPT_TRIANGLELIST;
	pFrame->pMesh->iDrawVertexCount	= dwNumVertices;
	pFrame->pMesh->iDrawPrimitives	= dwNumFaces;

	// check the newly created memory is okay
	SAFE_MEMORY ( pFrame->pMesh->pVertexData );
	SAFE_MEMORY ( pFrame->pMesh->pIndices  );

	// clear out newly created memory
	memset ( pFrame->pMesh->pVertexData, 0, dwVertexDataSize );
	if ( b32BITIndexData )
	 	memset ( pFrame->pMesh->pIndices, 0, sizeof ( DWORD ) * pFrame->pMesh->dwIndexCount  );
	else
	 	memset ( pFrame->pMesh->pIndices, 0, sizeof ( WORD ) * pFrame->pMesh->dwIndexCount  );

	return true;
}

DARKSDK bool XFILE_NEW_SetupVertexData ( sFrame* pFrame )
{
	// get the vertices and indices into the object
	if ( !pFrame )
		return false;

	// get mesh ptr
	sMesh* pMesh = pFrame->pMesh;

	// get offset data
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// get a pointer to the vertex data
	BYTE* pVertex = pMesh->pVertexData;

	// copy vertex data
	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	{
		// position
		if ( offsetMap.dwZ != 0 )
		{
			*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].vecPosition.x;
			*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].vecPosition.y;
			*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].vecPosition.z;
		}

		// normal (dwNX changed to dwNZ 040908 - new use of function with DECLDATA read)
		if ( offsetMap.dwNZ != 0 )
		{
			*( ( float* ) pVertex + offsetMap.dwNX + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].vecNormal.x;
			*( ( float* ) pVertex + offsetMap.dwNY + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].vecNormal.y;
			*( ( float* ) pVertex + offsetMap.dwNZ + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].vecNormal.z;
		}

		// base level UV data
		if ( offsetMap.dwTU [ 0 ] != 0 )
		{
			*( ( float* ) pVertex + offsetMap.dwTU [ 0 ] + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].tu;
			*( ( float* ) pVertex + offsetMap.dwTV [ 0 ] + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].tv;
		}

		// if diffuse present, fill it
		if ( offsetMap.dwDiffuse != 0 )
		{
			DWORD dwColor = D3DXCOLOR ( 1, 1, 1, 1 );
			*( ( DWORD* ) pVertex + offsetMap.dwDiffuse + ( offsetMap.dwSize * iVertex ) ) = dwColor;
		}

		// if extra set of UV Data
		if ( g_iNumberOfExtraUVDataStagesAvailable==1 )
		{
			*( ( float* ) pVertex + offsetMap.dwTU [ 1 ] + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].tu2;
			*( ( float* ) pVertex + offsetMap.dwTV [ 1 ] + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].tv2;
		}
		if ( g_iNumberOfExtraUVDataStagesAvailable==2 )
		{
			*( ( float* ) pVertex + offsetMap.dwTU [ 1 ] + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].tu2;
			*( ( float* ) pVertex + offsetMap.dwTV [ 1 ] + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].tv2;
			*( ( float* ) pVertex + offsetMap.dwTU [ 2 ] + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].tu3;
			*( ( float* ) pVertex + offsetMap.dwTV [ 2 ] + ( offsetMap.dwSize * iVertex ) ) = g_pVertexList [ iVertex ].tv3;
		}
	}

	// diffuse color component
	if ( g_pVertexColorList && offsetMap.dwDiffuse != 0 )
	{
		// copy vertex color data (index provided by vertex color data itself)
		for ( DWORD dwVertColArrayIndex = 0; dwVertColArrayIndex < g_dwVertexColorCount; dwVertColArrayIndex++ )
		{
			// copy vertex color to correct place in vertex data
			sVertexColor VertexColor = g_pVertexColorList [ dwVertColArrayIndex ];
			int iVertex = VertexColor.index;
			DWORD dwColor = D3DXCOLOR ( VertexColor.color.red, VertexColor.color.green, VertexColor.color.blue, VertexColor.color.alpha );
			*( ( DWORD* ) pVertex + offsetMap.dwDiffuse + ( offsetMap.dwSize * iVertex ) ) = dwColor;
		}

		// switch mesh from material usage to vertex diffuse usage (this code comes after texture setup)
		pMesh->bUsesMaterial = false;

		// free usages
		SAFE_DELETE ( g_pVertexColorList );
	}

	// now copy indices (16 or 32 bit)
	if ( pMesh->dwVertexCount>0xFFFF )
	{
		DWORD* pDWORDIndexData = (DWORD*)pMesh->pIndices;
		for ( int iIndex = 0; iIndex < ( int ) pMesh->dwIndexCount; iIndex++ )
			pDWORDIndexData [ iIndex ] = g_pIndexList [ iIndex ];
	}
	else
	{
		for ( int iIndex = 0; iIndex < ( int ) pMesh->dwIndexCount; iIndex++ )
			pMesh->pIndices [ iIndex ] = (WORD) g_pIndexList [ iIndex ];
	}
	
	// we're finished now
	return true;
}

DARKSDK bool XFILE_NEW_SetupBones ( sFrame* pFrame )
{
	// sort out bones for the model

	if ( !pFrame )
		return false;

	pFrame->pMesh->dwBoneCount = g_dwBones;

	if ( pFrame->pMesh->dwBoneCount == 0 )
		return true;

	bool bDisable = false;
	
	// allocate bone array
	pFrame->pMesh->pBones = new sBone [ pFrame->pMesh->dwBoneCount ];
	
	// run through all bones and store information
	for ( int iTemp = 0; iTemp < ( int ) pFrame->pMesh->dwBoneCount; iTemp++ )
	{
		// get number of bone influences
		pFrame->pMesh->pBones [ iTemp ].dwNumInfluences = g_BoneList [ iTemp ].dwNumWeights;

		// LEEFIX - 161003 - It is perfectly okay for some bones to have no weights
//		g_bDisableBones = true;
		if ( g_BoneList [ iTemp ].dwNumWeights == 0 )
		{
			// this bone does not affect our model at this time
			pFrame->pMesh->pBones [ iTemp ].pVertices = NULL;
			pFrame->pMesh->pBones [ iTemp ].pWeights  = NULL;
		}
		else
		{
			// allocate vertex and weight arrays
			pFrame->pMesh->pBones [ iTemp ].pVertices = new DWORD [ pFrame->pMesh->pBones [ iTemp ].dwNumInfluences ];
			pFrame->pMesh->pBones [ iTemp ].pWeights  = new float [ pFrame->pMesh->pBones [ iTemp ].dwNumInfluences ];
			for ( int i = 0; i < (int)g_BoneList [ iTemp ].dwNumWeights; i++ )
			{
				pFrame->pMesh->pBones [ iTemp ].pVertices [ i ] = g_BoneList [ iTemp ].pIndices [ i ];
				pFrame->pMesh->pBones [ iTemp ].pWeights  [ i ] = g_BoneList [ iTemp ].pfWeights [ i ];
			}
		}

		// we can now get the translation matrix
		memcpy ( pFrame->pMesh->pBones [ iTemp ].matTranslation, &g_BoneList [ iTemp ].matrix, sizeof ( D3DXMATRIX ) );

		// finally store the name of the bone
		strcpy ( pFrame->pMesh->pBones [ iTemp ].szName, g_BoneList [ iTemp ].szName );
	}

	// return back to caller
	return true;
}

DARKSDK bool XFILE_NEW_SetupTextures ( sMesh* pMesh )
{
	// mesh texture data
	if ( !pMesh )
		return false;

	// LEELEE - I can see g_MaterialList and g_pMaterialList - very confusing to know which is which!

	// number of loaddata materials
	DWORD dwCount = g_NewMaterialList.size ( ); 

	// at least one texture must be created
	if ( dwCount < 1 ) dwCount=1;

	// individual materials to each face (optional-see origin)
	if ( g_pMaterialList )
	{
		// create array to store material indexes within mesh
		DWORD dwNumberOfFaces = pMesh->dwIndexCount / 3;
		pMesh->pAttributeWorkData = new DWORD [ dwNumberOfFaces ];

		// copy actual index data from filedata
		memcpy ( pMesh->pAttributeWorkData, g_pMaterialList, sizeof ( DWORD ) * g_dwMaterialListIndexCount );

		// LEELEE - notice the g_dwMaterialListIndexCount is misleading, as it should be in FaceCount (ie DWORD nFaceIndexes)
		// LEEFIX - 171003 - Material List must be padded with last material index (for cases of 1,1,0)
		if ( g_dwMaterialListIndexCount < dwNumberOfFaces )
		{
			DWORD dwPadIndex = g_pMaterialList [ g_dwMaterialListIndexCount-1 ];
			for ( DWORD dwIndex=g_dwMaterialListIndexCount; dwIndex<dwNumberOfFaces; dwIndex++ )
				pMesh->pAttributeWorkData [ dwIndex ] = dwPadIndex;
		}
	}

	// create texture array for mesh
	pMesh->dwTextureCount = dwCount;
	pMesh->pTextures      = new sTexture [ dwCount ];

	// if loaddata has material properties, store them in mesh
	if ( g_NewMaterialList.size ( ) > 0 )
	{
		// material bank holds all materials
		pMesh->pMaterialBank = new D3DXMATERIAL [ dwCount ];
		memset ( pMesh->pMaterialBank, 0, sizeof(D3DXMATERIAL) * dwCount );

		// go through each material in bank
		for ( int iTexture = 0; iTexture < ( int ) dwCount; iTexture++ )
		{
			// copy material data from loaddata
			pMesh->pMaterialBank [ iTexture ].MatD3D.Diffuse.r  = g_NewMaterialList [ iTexture ].vecColour.x;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Diffuse.g  = g_NewMaterialList [ iTexture ].vecColour.y;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Diffuse.b  = g_NewMaterialList [ iTexture ].vecColour.z;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Diffuse.a  = g_NewMaterialList [ iTexture ].vecColour.w;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Ambient.r  = g_NewMaterialList [ iTexture ].vecColour.x;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Ambient.g  = g_NewMaterialList [ iTexture ].vecColour.y;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Ambient.b  = g_NewMaterialList [ iTexture ].vecColour.z;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Ambient.a  = g_NewMaterialList [ iTexture ].vecColour.w;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Specular.r = g_NewMaterialList [ iTexture ].vecSpecular.x;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Specular.g = g_NewMaterialList [ iTexture ].vecSpecular.y;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Specular.b = g_NewMaterialList [ iTexture ].vecSpecular.z;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Specular.a = 1.0f;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Emissive.r = g_NewMaterialList [ iTexture ].vecEmissive.x;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Emissive.g = g_NewMaterialList [ iTexture ].vecEmissive.y;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Emissive.b = g_NewMaterialList [ iTexture ].vecEmissive.z;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Emissive.a = 1.0f;
			pMesh->pMaterialBank [ iTexture ].MatD3D.Power = g_NewMaterialList [ iTexture ].fPower;

			// copy material name from loaddata
			if ( g_NewMaterialList [ iTexture ].szFilename )
				strcpy ( pMesh->pTextures [ iTexture ].pName, g_NewMaterialList [ iTexture ].szFilename  );
			else
				strcpy ( pMesh->pTextures [ iTexture ].pName, "" );
		}

		// default material assignments
		memset ( &pMesh->mMaterial, 0, sizeof ( pMesh->mMaterial ) );
		pMesh->mMaterial.Diffuse  = pMesh->pMaterialBank [ 0 ].MatD3D.Diffuse;
		pMesh->mMaterial.Ambient  = pMesh->pMaterialBank [ 0 ].MatD3D.Ambient;
		pMesh->mMaterial.Specular = pMesh->pMaterialBank [ 0 ].MatD3D.Specular;
		pMesh->mMaterial.Emissive = pMesh->pMaterialBank [ 0 ].MatD3D.Emissive;
		pMesh->mMaterial.Power = pMesh->pMaterialBank [ 0 ].MatD3D.Power;
		pMesh->bUsesMaterial = true;
	}
	else
	{
		D3DMATERIAL9 material;
		material.Diffuse.r = 0.0f;
		material.Diffuse.g = 0.0f;
		material.Diffuse.b = 0.0f;
		material.Diffuse.a = 0.0f;
		material.Ambient	= material.Diffuse;
		pMesh->mMaterial = material;
		pMesh->pMaterialBank = new D3DXMATERIAL [ 1 ];
		pMesh->pMaterialBank [ 0 ].MatD3D           = material;
		pMesh->pMaterialBank [ 0 ].pTextureFilename = NULL;
		pMesh->bUsesMaterial = true;
	}

	return true;
}

DARKSDK bool XFILE_GetMeshData ( IDirectXFileData* pDataObj, sFrame* pParentFrame )
{
	// gets the mesh data from the X file and places it into the frame structure

	// first off check the parameters are valid
	SAFE_MEMORY ( pDataObj     );		// X file interface
	SAFE_MEMORY ( pParentFrame );		// frame to place data into
	SAFE_MEMORY ( m_pD3D       );		// D3D device

	// delete global variables
	SAFE_DELETE_ARRAY ( g_pVertexList   );
	SAFE_DELETE_ARRAY ( g_pIndexList    );
	SAFE_DELETE_ARRAY ( g_pFaceIndexOldRef );
	SAFE_DELETE_ARRAY ( g_pMaterialList );

	// reset values
	g_dwVertexCount            = 0;
	g_dwFaceCount              = 0;
	g_dwMaterialListIndexCount = 0;
	g_dwMaterialCount          = 0;
	g_dMaxSkinWeightsPerVertex = 0;
	g_dMaxSkinWeightsPerFace   = 0;
	g_dwBones                  = 0;
	g_iNumberOfExtraUVDataStagesAvailable = 0;

	// free resources in bone list
	for ( int i = 0; i < (int)g_BoneList.size(); i++ )
	{
		SAFE_DELETE ( g_BoneList [ i ].pfWeights );
		SAFE_DELETE ( g_BoneList [ i ].pIndices );
		SAFE_DELETE ( g_BoneList [ i ].szName );
	}
	// free resources in material list
	for ( int i = 0; i < (int)g_NewMaterialList.size(); i++ )
		SAFE_DELETE ( g_NewMaterialList [ i ].szFilename );

	// clear lists
	g_BoneList.clear        ( );
	g_NewMaterialList.clear ( );

	XFILE_NEW_GetVerticesAndIndices ( pDataObj, pParentFrame );

	bool				bSearch     = true;			// search flag
	IDirectXFileData*	pDataObject = pDataObj;		// data
	IDirectXFileObject*	pSubObject  = NULL;			// interface to object
	IDirectXFileData*	pSubData    = NULL;			// data pointer
	const GUID*			guidType    = NULL;			// type of data
	char*				szName	    = NULL;			// name

	while ( bSearch )
	{
		// see if a sub object exists
		if ( SUCCEEDED ( pDataObject->GetNextObject ( &pSubObject ) ) )
		{
			// attempt to access the data
			if ( SUCCEEDED ( pSubObject->QueryInterface ( DBIID_IDirectXFileData, ( void** ) &pSubData ) ) )
			{
				// find out the template we're dealing with
				if ( !XFILE_GetTemplateInfo ( pSubData, &guidType, &szName ) )
					return false;

				// normals
				if ( *guidType == TID_D3DRMMeshNormals )
					XFILE_NEW_GetNormals ( pSubData, pParentFrame );
				
				// texture coordinates
				if ( *guidType == TID_D3DRMMeshTextureCoords )
					XFILE_NEW_GetTextureCoordinates ( pSubData, pParentFrame );

				// texture coordinates SECOND UV LAYER (FVFDATA)
				if ( *guidType == DXFILEOBJ_FVFData )
					XFILE_NEW_GetFVFData ( pSubData, pParentFrame );
				
				// vertex colours
				if ( *guidType == TID_D3DRMMeshVertexColors )
					XFILE_NEW_GetVertexColors ( pSubData, pParentFrame );

				// material list
				if ( *guidType == TID_D3DRMMeshMaterialList )
					XFILE_NEW_GetMeshMaterialList ( pSubData, pParentFrame );

				// skin mesh header
				if ( *guidType == DXFILEOBJ_XSkinMeshHeader )
					XFILE_NEW_GetSkinMeshHeader ( pSubData, pParentFrame );

				// bones and weights
				if ( *guidType == DXFILEOBJ_SkinWeights )
					XFILE_NEW_GetSkinWeights ( pSubData, pParentFrame );

				// new U70 - 040908
				// DeclData - vertex declaration and data (replaces FVF mode)
				// can contain any of the above vertex information 
				if ( *guidType == DXFILEOBJ_DeclData  )
					XFILE_NEW_GetDeclData ( pSubData, pParentFrame );

				// MIKE - 311003
				SAFE_DELETE_ARRAY ( szName );
				SAFE_RELEASE ( pSubData );
			}
			else
				bSearch = false;

			SAFE_RELEASE ( pSubObject );
		}
		else
			bSearch = false;
	}

	// when all data gathered, copy normals to vertices ( can be more normals! )
	XFILE_NEW_CopyNormalsToVertices ();
	
	// call set up functions
	XFILE_NEW_CreateMeshInFrame ( pParentFrame, g_dwVertexCount, g_dwFaceCount );
	XFILE_NEW_SetupTextures     ( pParentFrame->pMesh );
	XFILE_NEW_SetupVertexData	( pParentFrame );

	// setup bones for mesh
	XFILE_NEW_SetupBones ( pParentFrame );

	// LEEFIX - 161003 - Simply cannot have more than 16bit's worth of vertices (max 0xFFFF)
	// and use index at same time! So if too many verts, convert to no index buffer version
	if ( g_dwVertexCount > 0xFFFF )
	{
		// unwraps index data and adds it to make larger vertex data
		ConvertLocalMeshToVertsOnly ( pParentFrame->pMesh );

		// must also unwrap the bone data which now points to incorrect vertex data
		// and can use the g_pConversionMap generated from the above conversion function
		// handle any mesh associated with this frame
		sMesh* pMesh = pParentFrame->pMesh;
		if ( pMesh )
		{
			for ( DWORD dwBone=0; dwBone<pMesh->dwBoneCount; dwBone++ )
			{
				for ( DWORD dwI=0; dwI<pMesh->pBones[dwBone].dwNumInfluences; dwI++ )
				{
					DWORD dwVIndex = pMesh->pBones[dwBone].pVertices[dwI];
					DWORD dwRelocatedVertexIndex = g_pConversionMap[dwVIndex];
					pMesh->pBones[dwBone].pVertices[dwI] = dwRelocatedVertexIndex;
				}
			}
		}

		// best to free conversion array now
		SAFE_DELETE(g_pConversionMap);
	}
	
	// clean up material list strings
	for ( int i = 0; i < (int)g_NewMaterialList.size ( ); i++ )
		SAFE_DELETE_ARRAY ( g_NewMaterialList [ i ].szFilename );
	g_NewMaterialList.clear();

	return true;
}

DARKSDK bool XFILE_CreateAnimationSet ( sAnimationSet** ppAnimSet, sAnimationSet** ppSubAnimSet, char* szName )
{
	// create an animation set for an object

	// create a new animation set
	*ppAnimSet = new sAnimationSet;

	// check to see the memory was allocated and make sure the name is valid
	SAFE_MEMORY ( *ppAnimSet );
	SAFE_MEMORY ( szName     );

	// store the animation name
	strcpy ( ( *ppAnimSet )->szName, szName );

	// link into the animation set list
	( *ppAnimSet )->pNext  = g_pObjectX->pAnimationSet;
	g_pObjectX->pAnimationSet = *ppAnimSet;
	
	// store a pointer to the anim set in sub anim set
	*ppSubAnimSet = *ppAnimSet;

	return true;
}

DARKSDK bool XFILE_CreateAnimation ( sAnimation** ppAnim, sAnimation** ppSubAnim, sAnimationSet* pParentAnim, char* szName )
{
	// create an animation for an object

	// create a new animation
	*ppAnim = new sAnimation;

	// make sure memory is okay
	SAFE_MEMORY ( *ppAnim     );
	SAFE_MEMORY ( szName      );
	SAFE_MEMORY ( pParentAnim );

	// store animation name
	strcpy ( ( *ppAnim )->szName, szName );

	// link into the animation list
	( *ppAnim )->pNext      = pParentAnim->pAnimation;
	pParentAnim->pAnimation = *ppAnim;

	// store the sub animation
	*ppSubAnim = *ppAnim;

	return true;
}

DARKSDK bool XFILE_SetupRotationAnimation ( PBYTE* pDataPtr, sAnimation* pAnim, sAnimationSet* pParentAnim, DWORD dwNumKeys )
{
	// setup animation for the rotation

	// check the parameters
	SAFE_MEMORY ( pDataPtr    );
	SAFE_MEMORY ( pAnim       );
	SAFE_MEMORY ( pParentAnim );

	// rotation key
	sXFileRotateKey* pRotKey;

	// delete any current rotation keys
	SAFE_DELETE_ARRAY ( pAnim->pRotateKeys );

	// create a new set of rotation keys
	pAnim->pRotateKeys = new sRotateKey [ dwNumKeys ];

	// check the memory
	SAFE_MEMORY ( pAnim->pRotateKeys );

	// store the number of rotation keys
	pAnim->dwNumRotateKeys = dwNumKeys;
	pRotKey                = ( sXFileRotateKey* ) ( ( char* ) pDataPtr + ( sizeof ( DWORD ) * 2 ) );
	
	// run through all of the keys (this data is time sorted later in object DLL)
	for ( int iKey = 0; iKey < ( int ) dwNumKeys; iKey++ )
	{
		pAnim->pRotateKeys [ iKey ].dwTime       = pRotKey->dwTime;	// get the time
		pAnim->pRotateKeys [ iKey ].Quaternion.x = -pRotKey->x;		// x rotation
		pAnim->pRotateKeys [ iKey ].Quaternion.y = -pRotKey->y;		// y rotation
		pAnim->pRotateKeys [ iKey ].Quaternion.z = -pRotKey->z;		// z rotation
		pAnim->pRotateKeys [ iKey ].Quaternion.w =  pRotKey->w;		// w rotation

		// check the length
		if ( pRotKey->dwTime > pParentAnim->ulLength )
			pParentAnim->ulLength = pRotKey->dwTime;

		// move to the next rotation key
		pRotKey++;
	}

	// all went okay
	return true;
}

DARKSDK bool XFILE_SetupScalingAnimation ( PBYTE* pDataPtr, sAnimation* pAnim, sAnimationSet* pParentAnim, DWORD dwNumKeys )
{
	// sort out scaling keys

	// check the parameters
	SAFE_MEMORY ( pDataPtr    );
	SAFE_MEMORY ( pAnim       );
	SAFE_MEMORY ( pParentAnim );

	// scale key
	sXFileScaleKey* pScaleKey = NULL;

	// delete any current scale keys
	SAFE_DELETE_ARRAY ( pAnim->pScaleKeys );

	// create a new set of scale keys
	pAnim->pScaleKeys = new sScaleKey [ dwNumKeys ];

	// check the newly allocated memory
	SAFE_MEMORY ( pAnim->pScaleKeys );

	// store the number of scale keys
	pAnim->dwNumScaleKeys = dwNumKeys;
	pScaleKey             = ( sXFileScaleKey* ) ( ( char* ) pDataPtr + ( sizeof ( DWORD ) * 2 ) );
	
	// go through all keys (this data is time sorted later in object DLL)
	for ( int iKey = 0; iKey < ( int ) dwNumKeys; iKey++ )
	{
		pAnim->pScaleKeys [ iKey ].dwTime   = pScaleKey->dwTime;
		pAnim->pScaleKeys [ iKey ].vecScale = pScaleKey->vecScale;

		if ( pScaleKey->dwTime > pParentAnim->ulLength )
			pParentAnim->ulLength = pScaleKey->dwTime;

		pScaleKey++;
	}

	return true;
}

DARKSDK bool XFILE_SetupPositionAnimation ( PBYTE* pDataPtr, sAnimation* pAnim, sAnimationSet* pParentAnim, DWORD dwNumKeys )
{
	// set up animation with position keys

	// first off check the memory we will access
	SAFE_MEMORY ( pDataPtr    );
	SAFE_MEMORY ( pAnim       );
	SAFE_MEMORY ( pParentAnim );

	// scale key
	sXFilePositionKey* pPosKey = NULL;

	// clear out any current position keys
	SAFE_DELETE_ARRAY ( pAnim->pPositionKeys );

	// allocate a new array of position keys
	pAnim->pPositionKeys = new sPositionKey [ dwNumKeys ];

	// check they were allocated correctly
	SAFE_MEMORY ( pAnim->pPositionKeys );

	// store the number of keys and get a pointer to the data
	pAnim->dwNumPositionKeys = dwNumKeys;
	pPosKey                  = ( sXFilePositionKey* ) ( ( char* ) pDataPtr + ( sizeof ( DWORD ) * 2 ) );
	
	//  all keyframes (this data is time sorted later in object DLL)
	for ( int iKey = 0; iKey < ( int ) dwNumKeys; iKey++ )
	{
		pAnim->pPositionKeys [ iKey ].dwTime = pPosKey->dwTime;
		pAnim->pPositionKeys [ iKey ].vecPos = pPosKey->vecPos;

		if ( pPosKey->dwTime > pParentAnim->ulLength )
			pParentAnim->ulLength = pPosKey->dwTime;

		pPosKey++;
	}

	return true;
}

DARKSDK bool XFILE_SetupMatrixAnimation ( PBYTE* pDataPtr, sAnimation* pAnim, sAnimationSet* pParentAnim, DWORD dwNumKeys )
{
	// set up animation that uses matrix keys

	// check the pointers
	SAFE_MEMORY ( pDataPtr    );
	SAFE_MEMORY ( pAnim       );
	SAFE_MEMORY ( pParentAnim );

	// delete any current matrix keys
	SAFE_DELETE_ARRAY ( pAnim->pMatrixKeys );
	
	// matrix key
	sXFileMatrixKey* pMatKey = NULL;

	// allocate a new set of matrix keys
	pAnim->pMatrixKeys = new sMatrixKey [ dwNumKeys ];

	// check the memory was allocated
	SAFE_MEMORY ( pAnim->pMatrixKeys );

	// store number of keys and get a pointer to the matrix data
	pAnim->dwNumMatrixKeys = dwNumKeys;
	pMatKey                = ( sXFileMatrixKey* ) ( ( char* ) pDataPtr + ( sizeof ( DWORD ) * 2 ) );

	for ( int iKey = 0; iKey < ( int ) dwNumKeys; iKey++ )
	{
		pAnim->pMatrixKeys [ iKey ].dwTime    = pMatKey->dwTime;
		pAnim->pMatrixKeys [ iKey ].matMatrix = pMatKey->matMatrix;

		if ( pMatKey->dwTime > pParentAnim->ulLength )
			pParentAnim->ulLength = pMatKey->dwTime;

		pMatKey++;
	}

	// calculate the interpolation matrices
	if ( dwNumKeys > 1 )
	{
		for ( int iKey = 0; iKey < ( int ) dwNumKeys - 1; iKey++ )
		{
			DWORD dwTime;

			pAnim->pMatrixKeys [ iKey ].matInterpolation = pAnim->pMatrixKeys [ iKey + 1 ].matMatrix - pAnim->pMatrixKeys [ iKey ].matMatrix;
			dwTime                                       = pAnim->pMatrixKeys [ iKey + 1 ].dwTime    - pAnim->pMatrixKeys [ iKey ].dwTime;

			if ( !dwTime )
				dwTime = 1;

			pAnim->pMatrixKeys [ iKey ].matInterpolation /= ( float ) dwTime;
		}
	}

	return true;
}

DARKSDK bool XFILE_GetAnimationData ( IDirectXFileData* pDataObj, sAnimation* pAnim, sAnimationSet* pParentAnim )
{
	// get the animation data from the file and move it into the DBO format
	
	// check the parameters
	SAFE_MEMORY ( pDataObj    );
	SAFE_MEMORY ( pAnim       );
	SAFE_MEMORY ( pParentAnim );

	// local variables
	PBYTE* pDataPtr  = NULL;	// data pointer from file
	DWORD  dwSize    = 0;		// size of data
	DWORD  dwKeyType = 0;		// type of animation
	DWORD  dwNumKeys = 0;		// number of animation keys

	// load in this animation's key data
	if ( FAILED ( pDataObj->GetData ( NULL, &dwSize, ( PVOID* ) &pDataPtr ) ) )
		return false;

	// get the key type and number of keys in the animation
	dwKeyType = ( ( DWORD* ) pDataPtr ) [ 0 ];
	dwNumKeys = ( ( DWORD* ) pDataPtr ) [ 1 ];

	// check the type of animation and call the appropriate function
	switch ( dwKeyType )
	{
		case 0:
		{
			// rotation
			if ( !XFILE_SetupRotationAnimation ( pDataPtr, pAnim, pParentAnim, dwNumKeys ) )
				return false;
		}
		break;

		case 1:
		{
			// scaling
			if ( !XFILE_SetupScalingAnimation ( pDataPtr, pAnim, pParentAnim, dwNumKeys ) )
				return false;
		}
		break;

		case 2:
		{
			// position
			if ( !XFILE_SetupPositionAnimation ( pDataPtr, pAnim, pParentAnim, dwNumKeys ) )
				return false;
		}
		break;

		case 4:
		{
			// matrix keys
			if ( !XFILE_SetupMatrixAnimation ( pDataPtr, pAnim, pParentAnim, dwNumKeys ) )
				return false;
		}
		break;

		default:
			break;
	}

	return true;
}

DARKSDK bool ParseXFileData ( IDirectXFileData* pDataObj, sFrame* pParentFrame, char* szTexturePath, sAnimationSet* pParentAnim, sAnimation* pCurrentAnim, bool bAnim )
{
	// parse the X file data, this is a fairly complex function that runs through all
	// of the data in an X file

	// check some of the parameters, we don't need to check the anim ones as they
	// don't need to be valid at the moment
	SAFE_MEMORY ( pDataObj );
	SAFE_MEMORY ( pParentFrame );
	SAFE_MEMORY ( szTexturePath );
	
	IDirectXFileObject*			pSubObj     = NULL;
	IDirectXFileData*			pSubData    = NULL;
	IDirectXFileDataReference*	pDataRef    = NULL;
	const GUID*					guidType    = NULL;
	char*						szName      = NULL;
	DWORD						dwSize      = 0;
	sFrame*						pSubFrame   = NULL;
	sFrame*						pFrame      = NULL;
	sAnimationSet*				pSubAnimSet = NULL;
	sAnimation*					pSubAnim    = NULL;
	sAnimation*					pAnim       = NULL;
	sAnimationSet*				pAnimSet    = NULL;

	// get the template info for the X file object
	if ( !XFILE_GetTemplateInfo ( pDataObj, &guidType, &szName ) )
		return false;

	// set sub frame
	pSubFrame = pParentFrame;

	// set sub frame parent
	pSubAnimSet = pParentAnim;
	pSubAnim    = pCurrentAnim;

	// process the templates

	// section for non animation
	if ( !bAnim )
	{
		if ( *guidType == TID_D3DRMFrame )
		{
			// create a frame
			if ( !XFILE_CreateFrame ( &pFrame, pParentFrame, &pSubFrame, szName ) )
				return false;
		}
		
		if ( *guidType == TID_D3DRMFrameTransformMatrix )
		{
			// get the frame transformation matrix
			XFILE_CreateTransformationMatrix ( pDataObj, pParentFrame );
		}

		// mesh
		if ( *guidType == TID_D3DRMMesh )
		{
//			if getting to the data is needed, this may help...
//			DWORD dwSize = 0;
//			LPSTR pDataPtr = NULL;
//			pDataObj->GetData ( NULL, &dwSize, (void**)&pDataPtr );
//			DWORD* pRawData = (DWORD*)pDataPtr;
//			DWORD dwRawVerts = *(pRawData+0);

			// leefix - 100303 - if mesh exists in current frame, must create new sibling frame to hold new mesh
			if ( pParentFrame->pMesh )
			{
				// create a new frame for extra meshes
				if ( !XFILE_CreateFrame ( &pFrame, pParentFrame, &pSubFrame, szName ) )
					return false;

				// create new mesh in new sibling frame
				if ( !XFILE_GetMeshData ( pDataObj, pFrame ) )
					return false;
			}
			else
			{
                // apply the mesh name to the frame if the frame hasn't yet got a name
                if (pParentFrame->szName[0] == 0 && szName[0] != 0)
                    strcpy ( pParentFrame->szName, szName );

                // create new mesh in frame
				if ( !XFILE_GetMeshData ( pDataObj, pParentFrame ) )
					return false;
			}
		}

		// skip animation sets and animations
		if ( *guidType == TID_D3DRMAnimationSet || *guidType == TID_D3DRMAnimation || *guidType == TID_D3DRMAnimationKey )
		{
			// take no further action if animation token
			SAFE_DELETE_ARRAY ( szName );
			return true;
		}
	}

	if ( bAnim )
	{
		// process an animation set
		if ( *guidType == TID_D3DRMAnimationSet )
		{
			if ( !XFILE_CreateAnimationSet ( &pAnimSet, &pSubAnimSet, szName ) )
				return false;
		}

		// create an animation
		if ( *guidType == TID_D3DRMAnimation && pParentAnim != NULL ) 
		{
			if ( !XFILE_CreateAnimation ( &pAnim, &pSubAnim, pParentAnim, szName ) )
				return false;
		}

		// process an animation key
		if ( *guidType == TID_D3DRMAnimationKey && pCurrentAnim != NULL )
		{
			// get the animation data from the key
			if ( !XFILE_GetAnimationData ( pDataObj, pCurrentAnim, pParentAnim ) )
				return false;
		}

		// process an animation options
		if ( *guidType == TID_D3DRMAnimationOptions && pCurrentAnim != NULL )
		{
			// get the animation options - open/closed and spline/linear
			DWORD dwSize = 0;
			PBYTE* pDataPtr  = NULL;	// data pointer from file
			if ( FAILED ( pDataObj->GetData ( NULL, &dwSize, ( PVOID* ) &pDataPtr ) ) )
				return false;

			// get animation options data
			DWORD dwOpenClosed			= ( ( DWORD* ) pDataPtr ) [ 0 ];
			DWORD dwPositionQuality		= ( ( DWORD* ) pDataPtr ) [ 1 ];

			// switch animation flags based on options
			if ( dwPositionQuality==1 )
				pCurrentAnim->bLinear = TRUE;
			else
				pCurrentAnim->bLinear = FALSE;
		}

		// process a frame reference
		if ( *guidType == TID_D3DRMFrame && pCurrentAnim != NULL )
		{
			strcpy ( pCurrentAnim->szName, szName );
			
			// take no further action if frame (ref) token
			SAFE_DELETE_ARRAY ( szName );
			return true;
		}
	}

	// MIKE - 311003
	SAFE_DELETE_ARRAY ( szName );

	// scan for embedded templates
	while ( SUCCEEDED ( pDataObj->GetNextObject ( &pSubObj ) ) )
	{
		// process embedded references
		if ( SUCCEEDED ( pSubObj->QueryInterface ( DBIID_IDirectXFileDataReference, ( void** ) &pDataRef ) ) )
		{
			if ( SUCCEEDED ( pDataRef->Resolve ( &pSubData ) ) )
			{
				if ( !ParseXFileData ( pSubData, pSubFrame, szTexturePath, pSubAnimSet, pSubAnim, bAnim ) )
					return false;

				SAFE_RELEASE ( pSubData );
			}

			SAFE_RELEASE ( pDataRef );
		}

		// process non - referenced embedded templates
		if ( SUCCEEDED ( pSubObj->QueryInterface ( DBIID_IDirectXFileData, ( void** ) &pSubData ) ) )
		{
			if ( !ParseXFileData ( pSubData, pSubFrame, szTexturePath, pSubAnimSet, pSubAnim, bAnim ) )
				return false;

			SAFE_RELEASE ( pSubData );
		}

		SAFE_RELEASE ( pSubObj );
	}

	return true;
}


/* This seems redundant, if so, delete from exisrtance

  void SetupTextures ( sMesh* pMesh, D3DXMATERIAL* pMaterials, DWORD dwCount, DWORD* pAttribute, DWORD dwIndexCount )
{
	// at least one texture must be created
	if ( dwCount < 1 ) dwCount=1;

	// Store the attribute data for later processing (if any)
	if ( pAttribute )
	{
		DWORD dwNumberOfFaces = dwIndexCount / 3;
		pMesh->pAttributeWorkData = new DWORD [ dwNumberOfFaces ];
		memcpy ( pMesh->pAttributeWorkData, pAttribute, sizeof(DWORD) * dwNumberOfFaces );
	}

	// get properties
	pMesh->dwTextureCount = dwCount;							// save the number of textures
	pMesh->pTextures      = new sTexture [ dwCount ];			// allocate texture memory

	// if have a material 
	if ( pMaterials )
	{
		// copy entire material buffer to temporary bank (in case mesh needs splitting)
		pMesh->pMaterialBank  = new D3DXMATERIAL [ dwCount ];
		memcpy ( pMesh->pMaterialBank, pMaterials, sizeof ( D3DXMATERIAL ) * dwCount );

		// copy single material (normally the default if no splitting)
		pMesh->bUsesMaterial = true;
		memcpy ( &pMesh->mMaterial, &pMaterials [ 0 ].MatD3D, sizeof ( D3DMATERIAL9 ) );

		// run through all textures, get materials and store
		for ( int iTexture = 0; iTexture < ( int ) dwCount; iTexture++ )
		{
			if ( pMaterials [ iTexture ].pTextureFilename )
			{
				DWORD dwSize = strlen ( pMaterials [ iTexture ].pTextureFilename );
				strcpy ( pMesh->pTextures [ iTexture ].pName, pMaterials [ iTexture ].pTextureFilename );
			}
			else
				strcpy ( pMesh->pTextures [ iTexture ].pName, "" );
		}
	}
	else
	{
		// default material
		D3DMATERIAL9 material;
		material.Diffuse.r = 0.0f;
		material.Diffuse.g = 0.0f;
		material.Diffuse.b = 0.0f;
		material.Diffuse.a = 0.0f;
		material.Ambient	= material.Diffuse;

		// Store as base material
		pMesh->mMaterial = material;

		// create a single material bank item
		pMesh->bUsesMaterial = true;
		pMesh->pMaterialBank = new D3DXMATERIAL [ 1 ];
		pMesh->pMaterialBank[0].MatD3D = material;
		pMesh->pMaterialBank[0].pTextureFilename = NULL;
	}
}
*/





















































































//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

// MIKE - 081003 - we need to keep this code because some of the DBO files refer to it without
//               - keeping the code in the DBO files

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

DARKSDK void AnglesFromMatrix ( D3DXMATRIX* pmatMatrix, D3DXVECTOR3* pVecAngles )
{
	// Calculate angle vector based on matrix
	D3DXVECTOR3 pDirVec = D3DXVECTOR3 ( 0.0f, 0.0f, 1.0f );
	D3DXVec3TransformCoord ( &pDirVec, &pDirVec, pmatMatrix );
	GetAngleFromPoint ( 0.0f, 0.0f, 0.0f, pDirVec.x, pDirVec.y, pDirVec.z, &(pVecAngles->x), &(pVecAngles->y), &(pVecAngles->z));
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
