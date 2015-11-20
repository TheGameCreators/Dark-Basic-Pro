

//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include "ConvMDL.h"
#include "direct.h"

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
#endif

#include < vector >
using namespace std;

#define	ON_EPSILON		0.01
#define	EQUAL_EPSILON	0.001
#define	Q_PI			3.14159265358979323846

//////////////////////////////////////////////////////////////////////////////////
// GLOBALS ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// DBP Format uses ObjectManager
#include ".\..\Objects\CObjectManagerC.h"
CObjectManager m_ObjectManager;
bool CObjectManager::Setup ( void ) { return false; }
bool CObjectManager::Free ( void ) { return false; }


// Globals for DBO/Manager relationship
#include <vector>
std::vector< sMesh* >		g_vRefreshMeshList;
std::vector< sObject* >     g_vAnimatableObjectList;
int							g_iSortedObjectCount;
sObject**					g_ppSortedObjectList;

typedef IDirect3DDevice9*		( *GFX_GetDirect3DDevicePFN )	( void );
typedef float vector3 [ 3 ];
typedef float vector4 [ 4 ];

DBPRO_GLOBAL GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;		
DBPRO_GLOBAL HINSTANCE						g_hSetup						= NULL;
DBPRO_GLOBAL LPDIRECT3DDEVICE9				m_pD3D							= NULL;
DBPRO_GLOBAL sObject*						g_pObject						= NULL;
DBPRO_GLOBAL sObject*						g_pTempObject					= NULL;
DBPRO_GLOBAL GlobStruct*					g_pGlob							= NULL;
DBPRO_GLOBAL sObject**						g_ObjectList;
DBPRO_GLOBAL FILE*							g_FP							= NULL;
DBPRO_GLOBAL int							g_iFileSize     				= 0;
DBPRO_GLOBAL BYTE*							g_pbData        				= NULL;
DBPRO_GLOBAL sMDLHeader*					g_pHeader       				= NULL;
DBPRO_GLOBAL sMDLTexture*					g_pTextures     				= NULL;
DBPRO_GLOBAL LPDIRECT3DTEXTURE9*			g_ppTextures    				= NULL;
DBPRO_GLOBAL int							g_iTextureIndex 				= 0;
DBPRO_GLOBAL sMDLDraw*						g_pDrawList     				= NULL;
DBPRO_GLOBAL int							g_iDrawCount    				= 0;
DBPRO_GLOBAL float							g_BoneTransform  [ 128 ] [ 3 ] [ 4 ];
DBPRO_GLOBAL D3DXMATRIX						g_matBoneTransforms [ 2000 ] [ 1000 ];
DBPRO_GLOBAL D3DXMATRIX**					g_ppBoneFrames;
DBPRO_GLOBAL int							g_iBoneFrameA;
DBPRO_GLOBAL int							g_iBoneFrameB;
DBPRO_GLOBAL vector < int > 				g_iOffsetList;
DBPRO_GLOBAL int* 							g_piOffsetList 					= NULL;
DBPRO_GLOBAL float							g_fShrinkObjectsTo				= 0.0f;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK bool GetFileData				( char* szFilename );
DARKSDK bool GetHeaderData				( void );
DARKSDK bool GetTextureData				( void );
DARKSDK bool UploadTexture				( sMDLTexture* pTexture, BYTE* pbData, BYTE* pbPal );
DARKSDK bool StartConversionToDBO		( void );
DARKSDK bool BuildDrawList				( void );
DARKSDK bool SetupBones					( float fFrame );
DARKSDK bool BuildAnimationData			( void );

DARKSDK void QuaternionMatrix			( const vector4 quaternion, float ( *matrix ) [ 4 ] );
DARKSDK void AngleQuaternion			( const vector3 angles, vector4 quaternion );
DARKSDK void QuaternionSlerp			( const vector4 p, vector4 q, float t, vector4 qt );
DARKSDK void R_ConcatTransforms			( const float in1 [ 3 ] [ 4 ], const float in2 [ 3 ] [ 4 ], float out [ 3 ] [ 4 ] );
DARKSDK bool VectorCompare				( vector3 v1, vector3 v2 );
//DARKSDK void VectorTransform			( const D3DXVECTOR3 in1, const D3DXMATRIX matrix, D3DXVECTOR3 &out );

void VectorTransform			( const D3DXVECTOR3 in1, const D3DXMATRIX matrix, D3DXVECTOR3 &out );

DARKSDK void CalculateRotations			( vector3* pos, vector4 *q, sMDLSequence* pSequence, sMDLAnimation* pAnim, float fFrame );
DARKSDK void CalculateBoneQuaternion	( int iFrame, float fS, sMDLBone* pBone, sMDLAnimation* pAnim, float *fQ );
DARKSDK void CalculateBonePosition		( int frame, float s, sMDLBone* pBone, sMDLAnimation* pAnim, float* pos );

DARKSDK bool SetupFrame					( sFrame* pFrame, int iFrame );
DARKSDK bool BuildFrameList				( void );
DARKSDK void UpdateBones				( void );


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

DARKSDK void PassCoreData( LPVOID pGlobPtr )
{
	g_pGlob = (GlobStruct*)pGlobPtr;
}

DARKSDK bool Load ( char* szFilename )
{
	// load an MDL model and convert to DBO format

	// leefix - 110503 - textures go in temp folder
	DBOCalculateLoaderTempFolder();

	// get the file data
	if ( ! GetFileData ( szFilename ) )
		return false;

	// get the header data
	if ( ! GetHeaderData ( ) )
		return false;
	
	// load in texture data
	if ( ! GetTextureData ( ) )
		return false;

	// convert to DBO
	if ( ! StartConversionToDBO ( ) )
		return false;

	g_pObject = g_pTempObject;

	return true;
}

DARKSDK bool Delete ( void )
{
	SAFE_DELETE_ARRAY ( g_pTextures );
	SAFE_DELETE_ARRAY ( g_pDrawList );
	
	for ( int iFrame = 0; iFrame < g_iBoneFrameB; iFrame++ )
	{
		SAFE_DELETE_ARRAY ( g_ppBoneFrames [ iFrame ] );
	}

	SAFE_DELETE_ARRAY ( g_ppBoneFrames );

	SAFE_DELETE_ARRAY ( g_pbData );
	SAFE_DELETE_ARRAY ( g_piOffsetList );

	g_pTempObject->pAnimationSet->pAnimation->piBoneOffsetList = NULL;
	g_pTempObject->pAnimationSet->pAnimation->ppBoneFrames = NULL;

	SAFE_DELETE ( g_pTempObject );
	

	return true;
}

DARKSDK bool StartConversionToDBO ( void )
{
	g_pDrawList = new sMDLDraw [ 800 ] ( );

	if ( ! SetupBones ( 0.0f ) )
		return false;

	if ( ! BuildDrawList ( ) )
		return false;

	if ( ! BuildFrameList ( ) )
		return false;

	if ( ! BuildAnimationData ( ) )
		return false;

	return true;
}

DARKSDK bool BuildAnimationData ( void )
{
	float fFrame = 0.0f;

	// final = num frames / 0.1
	// e.g. 101 / 0.1 = 1010

	sMDLSequence* pSequence = ( sMDLSequence* ) ( ( BYTE* ) g_pHeader + g_pHeader->iSeqIndex ) + 0;
	
	for ( int iFrame = 0; iFrame < pSequence->iNumFrames / 0.1; iFrame++, fFrame += 0.1f )
	{
		SetupBones ( fFrame );

		for ( DWORD iBone = 0; iBone < g_pTempObject->pFrame->pMesh->dwBoneCount; iBone++ )
		{
			D3DXMATRIX matrix;
			
			memcpy ( &matrix, g_BoneTransform [ iBone ], sizeof ( g_BoneTransform [ iBone ] ) );

			memcpy ( 
						g_matBoneTransforms [ iFrame ] [ iBone ],
						&matrix,
						sizeof ( D3DXMATRIX )
				   );
		}
	}

	g_ppBoneFrames = new D3DXMATRIX* [ (DWORD) (pSequence->iNumFrames / 0.1) ];
	
	g_iBoneFrameA = (int) (pSequence->iNumFrames / 0.1 ) - 10;
	g_iBoneFrameB = (int) (pSequence->iNumFrames / 0.1 ) - 10;

	// mike - 020206 - addition for vs8
	//for ( iFrame = 0; iFrame < g_iBoneFrameA; iFrame++ )
	for ( int iFrame = 0; iFrame < g_iBoneFrameA; iFrame++ )
	{
		g_ppBoneFrames [ iFrame ] = new D3DXMATRIX [ g_iBoneFrameB ];
	}

	// mike - 020206 - addition for vs8
	//for ( iFrame = 0; iFrame < g_iBoneFrameA; iFrame++ )
	for ( int iFrame = 0; iFrame < g_iBoneFrameA; iFrame++ )
	{
		for ( int iSubFrame = 0; iSubFrame < g_iBoneFrameB; iSubFrame++ )
		{
			g_ppBoneFrames [ iFrame ] [ iSubFrame ] = g_matBoneTransforms [ iFrame ] [ iSubFrame ];
		}
	}

	g_pTempObject->pAnimationSet->pAnimation->ppBoneFrames = g_ppBoneFrames;
	g_pTempObject->pAnimationSet->pAnimation->iBoneFrameA  = g_iBoneFrameA;
	g_pTempObject->pAnimationSet->pAnimation->iBoneFrameB  = g_iBoneFrameB;

	return true;
}

DARKSDK bool BuildFrameList ( void )
{
	g_pTempObject         = new sObject;
	g_pTempObject->pFrame = new sFrame;

	g_pTempObject->pAnimationSet                                   = new sAnimationSet;
	g_pTempObject->pAnimationSet->pAnimation                       = new sAnimation;
	g_pTempObject->pAnimationSet->pAnimation->bBoneType            = 0;
	g_pTempObject->pAnimationSet->pAnimation->piBoneOffsetList     = g_piOffsetList;
	g_pTempObject->pAnimationSet->pAnimation->iBoneOffsetListCount = g_iOffsetList.size ( );

	sFrame* pFrame = g_pTempObject->pFrame;

	for ( int iFrame = 0; iFrame < g_iDrawCount; iFrame++ )
	{
		SetupFrame ( pFrame, iFrame );
	
		if ( iFrame < g_iDrawCount - 1 )
		{
			pFrame->pChild = new sFrame;
			pFrame         = pFrame->pChild;
		}
	}

	return true;
}

DARKSDK bool SetupFrame ( sFrame* pFrame, int iFrame )
{
	sMesh* pMesh = NULL;

	pFrame->pMesh = new sMesh;
	pMesh         = pFrame->pMesh;

	pMesh->dwVertexCount    = g_pDrawList [ iFrame ].iCount;				// number of vertices
	pMesh->dwIndexCount     = 0;											// number of indices
	pMesh->iDrawVertexCount = g_pDrawList [ iFrame ].iCount - 2;			// number of vertices to draw
	pMesh->iDrawPrimitives  = g_pDrawList [ iFrame ].iCount - 2;			// number of primitives to draw
	pMesh->dwFVFOriginal    = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;		// original fvf
	pMesh->dwFVF		    = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;		// current fvf
	pMesh->dwFVFSize        = sizeof ( float ) * 8;							// fvf size

	pMesh->iPrimitiveType = g_pDrawList [ iFrame ].iType;

	// create FVF space
	SetupMeshFVFData ( pMesh, pMesh->dwFVF, pMesh->dwVertexCount, pMesh->dwIndexCount );

	// get the offset map for the vertex data
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	int iVertexPosition = 0;

	sMDLBodyPart* pBodyPart     = ( sMDLBodyPart* ) ( ( BYTE* ) g_pHeader + g_pHeader->iBodyPartIndex ) + 0;
	sMDLModel*	  pModel        = ( sMDLModel*    ) ( ( BYTE* ) g_pHeader + pBodyPart->iModelIndex    ) + 0;
	BYTE*		  pBoneVertices = ( ( BYTE* )	g_pHeader + pModel->iVertInfoIndex );

	pMesh->pBones      = new sBone [ g_pHeader->iNumBones ];
	pMesh->dwBoneCount = g_pHeader->iNumBones;
	
	for ( DWORD iBone = 0; iBone < pMesh->dwBoneCount; iBone++ )
	{
		D3DXMATRIX matrix;
		
		memcpy ( &matrix, g_BoneTransform [ iBone ], sizeof ( g_BoneTransform [ iBone ] ) );
		memcpy ( pMesh->pBones [ iBone ].matTranslation, &matrix, sizeof ( D3DXMATRIX ) );
	}

	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	{
		// get position and texture coordinates
		float fX  = g_pDrawList [ iFrame ].pVertices [ iVertexPosition ].x;
		float fY  = g_pDrawList [ iFrame ].pVertices [ iVertexPosition ].y;
		float fZ  = g_pDrawList [ iFrame ].pVertices [ iVertexPosition ].z;

		float fTU = g_pDrawList [ iFrame ].pVertices [ iVertexPosition ].tu;
		float fTV = g_pDrawList [ iFrame ].pVertices [ iVertexPosition ].tv;

		// copy data across
		*( ( float* ) pMesh->pVertexData + offsetMap.dwX        + ( offsetMap.dwSize * iVertexPosition ) ) = fX;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwY        + ( offsetMap.dwSize * iVertexPosition ) ) = fY;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwZ        + ( offsetMap.dwSize * iVertexPosition ) ) = fZ;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwTU [ 0 ] + ( offsetMap.dwSize * iVertexPosition ) ) = fTU;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwTV [ 0 ] + ( offsetMap.dwSize * iVertexPosition ) ) = fTV;

		iVertexPosition++;
	}

	// leefix - 150503 - generate normals for mesh and remove redundant index data and restore prim type
	GenerateNormals ( pMesh );
	SAFE_DELETE ( pMesh->pIndices );
	pMesh->iPrimitiveType = g_pDrawList [ iFrame ].iType;
	pMesh->dwIndexCount = 0;

	// MDL culling is switched off (actually reversed in the data)
	D3DXVECTOR3 v;
	pMesh->bCull = false;
	// mike - 020206 - addition for vs8
	//for ( iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	{
		v.x = *( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iVertex ) );
		v.y = *( ( float* ) pMesh->pVertexData + offsetMap.dwNY + ( offsetMap.dwSize * iVertex ) );
		v.z = *( ( float* ) pMesh->pVertexData + offsetMap.dwNZ + ( offsetMap.dwSize * iVertex ) );
		*( ( float* ) pMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iVertex ) ) = 1.0f-v.x;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwNY + ( offsetMap.dwSize * iVertex ) ) = 1.0f-v.y;
		*( ( float* ) pMesh->pVertexData + offsetMap.dwNZ + ( offsetMap.dwSize * iVertex ) ) = 1.0f-v.z;
	}

	DWORD dwTotalVertSize = pMesh->dwVertexCount * pMesh->dwFVFSize;
	pMesh->pOriginalVertexData = ( BYTE* ) new char [ dwTotalVertSize ];
	memcpy ( pMesh->pOriginalVertexData, pMesh->pVertexData, dwTotalVertSize );

	pMesh->pTextures = new sTexture [ 1 ];
	
	pMesh->bUsesMaterial = false;
	strcpy ( pMesh->pTextures->pName, g_pDrawList [ iFrame ].szTexture );
	pMesh->dwTextureCount = 1;

	SetupObjectsGenericProperties ( g_pTempObject );

	return true;
}

DARKSDK bool BuildDrawList ( void )
{
	g_iDrawCount = 0;

	int           iPart      = 0;
	int			  iTriangle  = 0;
	sMDLBodyPart* pBodyPart  = ( sMDLBodyPart* ) ( ( BYTE* ) g_pHeader + g_pHeader->iBodyPartIndex ) + iPart;
	sMDLModel*	  pModel     = ( sMDLModel*    ) ( ( BYTE* ) g_pHeader + pBodyPart->iModelIndex    ) + iPart;
	D3DXVECTOR3*  pVertices  = ( D3DXVECTOR3*  ) ( ( BYTE* ) g_pHeader + pModel->iVertIndex );
	sMDLTexture*  pTexture   = ( sMDLTexture*  ) ( ( BYTE* ) g_pHeader + g_pHeader->iTextureIndex );
	short*        psSkin     = ( short*        ) ( ( BYTE* ) g_pHeader + g_pHeader->iSkinIndex );
	BYTE*		  pVertBone  = (                   ( BYTE* ) g_pHeader + pModel->iVertInfoIndex );
	sMDLMesh*	  pMesh      = ( sMDLMesh* )     ( ( BYTE* ) g_pHeader + pModel->iMeshIndex ) + 0;
	short*		  psIndex    = ( short*    )     ( ( BYTE* ) g_pHeader + pMesh->iTriIndex   );
	
	for ( int iMesh = 0; iMesh < pModel->iNumMesh; iMesh++ )
	{
		float				tu1;
		float				tv1;
		sMDLMesh*			pMesh   = ( sMDLMesh* ) ( ( BYTE* ) g_pHeader + pModel->iMeshIndex ) + iMesh;
		short*				psIndex = ( short*    ) ( ( BYTE* ) g_pHeader + pMesh->iTriIndex   );
		int					iNum    = 0;
		int					iCheck  = 0;
		D3DPRIMITIVETYPE	iType;

		tu1 = 1.0f / ( float ) pTexture [ psSkin [ pMesh->iSkinRef ] ].iWidth;
		tv1 = 1.0f / ( float ) pTexture [ psSkin [ pMesh->iSkinRef ] ].iHeight;

		while ( iTriangle = *( psIndex++ ) )
		{
			iNum   = 0;
			iCheck = 0;
			
			if ( iTriangle < 0 )
			{
				iType	  = D3DPT_TRIANGLEFAN;
				iTriangle = -iTriangle;
			}
			else
				iType = D3DPT_TRIANGLESTRIP;

			g_pDrawList [ g_iDrawCount ].iTexture  = pTexture [ psSkin [ pMesh->iSkinRef ] ].iIndex;
			g_pDrawList [ g_iDrawCount ].iType	   = iType;
			g_pDrawList [ g_iDrawCount ].iCount    = iTriangle;

			if ( !g_pDrawList [ g_iDrawCount ].pVertices )
				g_pDrawList [ g_iDrawCount ].pVertices = new sMDLVertex [ iTriangle ] ( );

			strcpy ( g_pDrawList [ g_iDrawCount ].szTexture, pTexture [ psSkin [ pMesh->iSkinRef ] ].sName );
	
			for ( ; iTriangle > 0; iTriangle--, psIndex += 4 )
			{
				g_pDrawList [ g_iDrawCount ].pVertices [ iNum ].tu = psIndex [ 2 ] * tu1;
				g_pDrawList [ g_iDrawCount ].pVertices [ iNum ].tv = psIndex [ 3 ] * tv1;

				memcpy ( &g_pDrawList [ g_iDrawCount ].pVertices [ iNum ], pVertices [ psIndex [ 0 ] ], sizeof ( float ) * 3 );

				g_iOffsetList.push_back ( pVertBone [ psIndex [ 0 ] ] );
				
				iNum++;
				iCheck++;
			}

			g_iDrawCount++;
		}
	}

	g_piOffsetList = new int [ g_iOffsetList.size ( ) ];

    for ( std::vector<int>::size_type iOffset = 0; iOffset < g_iOffsetList.size ( ); iOffset++ )
	{
		g_piOffsetList [ iOffset ] = g_iOffsetList [ iOffset ];
	}

	return true;
}

DARKSDK void UpdateBones ( void )
{
	static int iFrame = 0;

	if ( iFrame < 999 )
		iFrame++;
	else
		iFrame = 0;
	
	int iPos = 0;

	for ( int iMesh = 0; iMesh < g_pTempObject->iMeshCount; iMesh++ )
	{
		sMesh* pMesh = g_pTempObject->ppMeshList [ iMesh ];

		// get the offset map for the vertex data
		sOffsetMap offsetMap;
		GetFVFOffsetMap ( pMesh, &offsetMap );

		int iVertexPosition = 0;
		
		for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
		{
			D3DXVECTOR3 vecInput = D3DXVECTOR3 (
													*( ( float* ) pMesh->pOriginalVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pMesh->pOriginalVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pMesh->pOriginalVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
											   );

			D3DXVECTOR3 vecOutput;

			VectorTransform ( vecInput, g_ppBoneFrames [ iFrame ] [ g_piOffsetList [ iPos++ ] ], vecOutput );

			// copy data across
			*( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecOutput.x;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecOutput.y;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecOutput.z;
		}
	}
}

DARKSDK bool SetupBones ( float fFrame )
{
	sMDLBone*			pBones			= NULL;
	sMDLSequence*		pSequence		= NULL;
	sMDLAnimation*		pAnim			= NULL;
	sMDLSequenceGroup*	pSequenceGroup	= NULL;

	float				fBoneMatrix [ 3 ] [ 4 ];

	static vector3		pos  [ 128 ];
	static vector4		q    [ 128 ];
	
	static vector3		pos2 [ 128 ];
	static vector4		q2   [ 128 ];

	static vector3		pos3 [ 128 ];
	static vector4		q3   [ 128 ];

	static vector3		pos4 [ 128 ];
	static vector4		q4   [ 128 ];

	pSequence      = ( sMDLSequence*      ) ( ( BYTE* ) g_pHeader + g_pHeader->iSeqIndex )      + 0;
	pSequenceGroup = ( sMDLSequenceGroup* ) ( ( BYTE* ) g_pHeader + g_pHeader->iSeqGroupIndex ) + pSequence->iSeqGroup;
	pAnim          = ( sMDLAnimation*     ) ( ( BYTE* ) g_pHeader + pSequenceGroup->iData       + pSequence->iAnimIndex );

	CalculateRotations ( pos, q, pSequence, pAnim, fFrame );

	pBones = ( sMDLBone* ) ( ( BYTE* ) g_pHeader + g_pHeader->iBoneIndex );

	for ( int i = 0; i < g_pHeader->iNumBones; i++ )
	{
		QuaternionMatrix ( q [ i ], fBoneMatrix );

		fBoneMatrix [ 0 ] [ 3 ] = pos [ i ] [ 0 ];
		fBoneMatrix [ 1 ] [ 3 ] = pos [ i ] [ 1 ];
		fBoneMatrix [ 2 ] [ 3 ] = pos [ i ] [ 2 ];

		if ( pBones [ i ].iParent == -1 )
			memcpy ( g_BoneTransform [ i ], fBoneMatrix, sizeof ( float ) * 12 );
		else
			R_ConcatTransforms ( g_BoneTransform [ pBones [ i ].iParent ], fBoneMatrix, g_BoneTransform [ i ] );
	}

	return true;
}

DARKSDK void R_ConcatTransforms ( const float in1 [ 3 ] [ 4 ], const float in2 [ 3 ] [ 4 ], float out [ 3 ] [ 4 ] )
{
	out [ 0 ] [ 0 ] = in1 [ 0 ] [ 0 ] * in2 [ 0 ] [ 0 ] + in1 [ 0 ] [ 1 ] * in2 [ 1 ] [ 0 ] + in1 [ 0 ] [ 2 ] * in2 [ 2 ] [ 0 ];
	out [ 0 ] [ 1 ] = in1 [ 0 ] [ 0 ] * in2 [ 0 ] [ 1 ] + in1 [ 0 ] [ 1 ] * in2 [ 1 ] [ 1 ] + in1 [ 0 ] [ 2 ] * in2 [ 2 ] [ 1 ];
	out [ 0 ] [ 2 ] = in1 [ 0 ] [ 0 ] * in2 [ 0 ] [ 2 ] + in1 [ 0 ] [ 1 ] * in2 [ 1 ] [ 2 ] + in1 [ 0 ] [ 2 ] * in2 [ 2 ] [ 2 ];
	out [ 0 ] [ 3 ] = in1 [ 0 ] [ 0 ] * in2 [ 0 ] [ 3 ] + in1 [ 0 ] [ 1 ] * in2 [ 1 ] [ 3 ] + in1 [ 0 ] [ 2 ] * in2 [ 2 ] [ 3 ] + in1 [ 0 ] [ 3 ];
	out [ 1 ] [ 0 ] = in1 [ 1 ] [ 0 ] * in2 [ 0 ] [ 0 ] + in1 [ 1 ] [ 1 ] * in2 [ 1 ] [ 0 ] + in1 [ 1 ] [ 2 ] * in2 [ 2 ] [ 0 ];
	out [ 1 ] [ 1 ] = in1 [ 1 ] [ 0 ] * in2 [ 0 ] [ 1 ] + in1 [ 1 ] [ 1 ] * in2 [ 1 ] [ 1 ] + in1 [ 1 ] [ 2 ] * in2 [ 2 ] [ 1 ];
	out [ 1 ] [ 2 ] = in1 [ 1 ] [ 0 ] * in2 [ 0 ] [ 2 ] + in1 [ 1 ] [ 1 ] * in2 [ 1 ] [ 2 ] + in1 [ 1 ] [ 2 ] * in2 [ 2 ] [ 2 ];
	out [ 1 ] [ 3 ] = in1 [ 1 ] [ 0 ] * in2 [ 0 ] [ 3 ] + in1 [ 1 ] [ 1 ] * in2 [ 1 ] [ 3 ] + in1 [ 1 ] [ 2 ] * in2 [ 2 ] [ 3 ] + in1 [ 1 ] [ 3 ];
	out [ 2 ] [ 0 ] = in1 [ 2 ] [ 0 ] * in2 [ 0 ] [ 0 ] + in1 [ 2 ] [ 1 ] * in2 [ 1 ] [ 0 ] + in1 [ 2 ] [ 2 ] * in2 [ 2 ] [ 0 ];
	out [ 2 ] [ 1 ] = in1 [ 2 ] [ 0 ] * in2 [ 0 ] [ 1 ] + in1 [ 2 ] [ 1 ] * in2 [ 1 ] [ 1 ] + in1 [ 2 ] [ 2 ] * in2 [ 2 ] [ 1 ];
	out [ 2 ] [ 2 ] = in1 [ 2 ] [ 0 ] * in2 [ 0 ] [ 2 ] + in1 [ 2 ] [ 1 ] * in2 [ 1 ] [ 2 ] + in1 [ 2 ] [ 2 ] * in2 [ 2 ] [ 2 ];
	out [ 2 ] [ 3 ] = in1 [ 2 ] [ 0 ] * in2 [ 0 ] [ 3 ] + in1 [ 2 ] [ 1 ] * in2 [ 1 ] [ 3 ] + in1 [ 2 ] [ 2 ] * in2 [ 2 ] [ 3 ] + in1 [ 2 ] [ 3 ];
}

DARKSDK void QuaternionMatrix ( const vector4 quaternion, float ( *matrix ) [ 4 ] )
{
	matrix [ 0 ] [ 0 ] = (float) (1.0 - 2.0 * quaternion [ 1 ] * quaternion [ 1 ] - 2.0 * quaternion [ 2 ] * quaternion [ 2 ]);
	matrix [ 1 ] [ 0 ] = (float) (2.0 *       quaternion [ 0 ] * quaternion [ 1 ] + 2.0 * quaternion [ 3 ] * quaternion [ 2 ]);
	matrix [ 2 ] [ 0 ] = (float) (2.0 *       quaternion [ 0 ] * quaternion [ 2 ] - 2.0 * quaternion [ 3 ] * quaternion [ 1 ]);

	matrix [ 0 ] [ 1 ] = (float) (2.0 *       quaternion [ 0 ] * quaternion [ 1 ] - 2.0 * quaternion [ 3 ] * quaternion [ 2 ]);
	matrix [ 1 ] [ 1 ] = (float) (1.0 - 2.0 * quaternion [ 0 ] * quaternion [ 0 ] - 2.0 * quaternion [ 2 ] * quaternion [ 2 ]);
	matrix [ 2 ] [ 1 ] = (float) (2.0 *       quaternion [ 1 ] * quaternion [ 2 ] + 2.0 * quaternion [ 3 ] * quaternion [ 0 ]);

	matrix [ 0 ] [ 2 ] = (float) (2.0 *       quaternion [ 0 ] * quaternion [ 2 ] + 2.0 * quaternion [ 3 ] * quaternion [ 1 ]);
	matrix [ 1 ] [ 2 ] = (float) (2.0 *       quaternion [ 1 ] * quaternion [ 2 ] - 2.0 * quaternion [ 3 ] * quaternion [ 0 ]);
	matrix [ 2 ] [ 2 ] = (float) (1.0 - 2.0 * quaternion [ 0 ] * quaternion [ 0 ] - 2.0 * quaternion [ 1 ] * quaternion [ 1 ]);
}

DARKSDK void CalculateRotations ( vector3* pos, vector4 *q, sMDLSequence* pSequence, sMDLAnimation* pAnim, float fFrame )
{
	int			iFrame;
	sMDLBone*	pBone;
	float		fS;

	iFrame = ( int ) fFrame;
	fS     = ( fFrame - iFrame );
	pBone  = ( sMDLBone* ) ( ( BYTE* ) g_pHeader + g_pHeader->iBoneIndex );

	for ( int iBone = 0; iBone < g_pHeader->iNumBones; iBone++, pBone++, pAnim++ )
	{
		CalculateBoneQuaternion ( iFrame, fS, pBone, pAnim,   q [ iBone ] );
		CalculateBonePosition   ( iFrame, fS, pBone, pAnim, pos [ iBone ] );
	}
}

DARKSDK void CalculateBoneQuaternion ( int iFrame, float fS, sMDLBone* pBone, sMDLAnimation* pAnim, float *fQ )
{
	vector4			q1, q2;
	vector3			angle1, angle2;
	int				k;
	unAnimValue*	pAnimValue;

	for ( int j = 0; j < 3; j++ )
	{
		if ( pAnim->usOffset [ j + 3 ] == 0 )
		{
			angle2 [ j ] = angle1 [ j ] = pBone->fValue [ j + 3 ];
		}
		else
		{
			pAnimValue = ( unAnimValue* ) ( ( byte* ) pAnim + pAnim->usOffset [ j + 3 ] );
			k          = iFrame;

			while (	pAnimValue->num.byTotal <= k )
			{
				k		   -= pAnimValue->num.byTotal;
				pAnimValue += pAnimValue->num.byValid + 1;
			}

			if ( pAnimValue->num.byValid > k )
			{
				angle1 [ j ] = pAnimValue [ k + 1 ].value;

				if ( pAnimValue->num.byValid > k + 1 )
				{
					angle2 [ j ] = pAnimValue [ k + 2 ].value;
				}
				else
				{
					if ( pAnimValue->num.byTotal > k + 1 )
						angle2 [ j ] = angle1 [ j ];
					else
						angle2 [ j ] = pAnimValue [ pAnimValue->num.byValid + 2 ].value;
				}
			}
			else
			{
				angle1 [ j ] = pAnimValue [ pAnimValue->num.byValid ].value;

				if ( pAnimValue->num.byTotal > k + 1 )
					angle2 [ j ] = angle1 [ j ];
				else
					angle2 [ j ] = pAnimValue [ pAnimValue->num.byValid + 2 ].value;
			}

			angle1 [ j ] = pBone->fValue [ j + 3 ] + angle1 [ j ] * pBone->fScale [ j + 3 ];
			angle2 [ j ] = pBone->fValue [ j + 3 ] + angle2 [ j ] * pBone->fScale [ j + 3 ];
		}

		if ( pBone->iBoneControl [ j + 3 ] != -1 )
		{
			angle1 [ j ] += 0.0;
			angle2 [ j ] += 0.0;
		}
	}

	if ( !VectorCompare ( angle1, angle2 ) )
	{
		AngleQuaternion ( angle1, q1     );
		AngleQuaternion ( angle2, q2     );
		QuaternionSlerp ( q1, q2, fS, fQ );
	}
	else
	{
		AngleQuaternion ( angle1, fQ );
	}
}

DARKSDK bool VectorCompare ( vector3 v1, vector3 v2 )
{
	for ( int i = 0; i < 3; i++ )
	{
		if ( fabs ( v1 [ i ] - v2 [ i ] ) > EQUAL_EPSILON )
			return false;
	}
			
	return true;
}

DARKSDK void AngleQuaternion ( const vector3 angles, vector4 quaternion )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angle = angles [ 2 ] * 0.5f;
	sy    = sin ( angle );
	cy    = cos ( angle );

	angle = angles [ 1 ] * 0.5f;
	sp    = sin ( angle );
	cp    = cos ( angle );

	angle = angles [ 0 ] * 0.5f;
	sr    = sin ( angle );
	cr    = cos ( angle );

	quaternion [ 0 ] = sr * cp * cy - cr * sp * sy;
	quaternion [ 1 ] = cr * sp * cy + sr * cp * sy;
	quaternion [ 2 ] = cr * cp * sy - sr * sp * cy;
	quaternion [ 3 ] = cr * cp * cy + sr * sp * sy;
}

DARKSDK void QuaternionSlerp ( const vector4 p, vector4 q, float t, vector4 qt )
{
	int i;
	float omega, cosom, sinom, sclp, sclq;

	float a = 0;
	float b = 0;

	for ( i = 0; i < 4; i++ )
	{
		a += ( p [ i ] - q [ i ] ) * ( p [ i ] - q [ i ] );
		b += ( p [ i ] + q [ i ] ) * ( p [ i ] + q [ i ] );
	}
	
	if ( a > b )
	{
		for ( i = 0; i < 4; i++ )
			q [ i ] = -q [ i ];
	}

	cosom = p [ 0 ] * q [ 0 ] + p [ 1 ] * q [ 1 ] + p [ 2 ] * q [ 2 ] + p [ 3 ] * q [ 3 ];

	if ( ( 1.0 + cosom ) > 0.00000001 )
	{
		if ( ( 1.0 - cosom ) > 0.00000001 )
		{
			omega = acos ( cosom );
			sinom = sin  ( omega );
			sclp  = sin  ( ( 1.0f - t ) * omega ) / sinom;
			sclq  = sin  ( t * omega ) / sinom;
		}
		else
		{
			sclp = 1.0f - t;
			sclq = t;
		}
		
		for ( i = 0; i < 4; i++ )
			qt [ i ] = sclp * p [ i ] + sclq * q [ i ];
	}
	else
	{
		qt [ 0 ] = -p [ 1 ];
		qt [ 1 ] =  p [ 0 ];
		qt [ 2 ] = -p [ 3 ];
		qt [ 3 ] =  p [ 2 ];

		sclp = sin ( ( 1.0f - t ) * 0.5f * (float)Q_PI );
		sclq = sin ( t * 0.5f * (float)Q_PI );

		for ( i = 0; i < 3; i++ )
			qt [ i ] = sclp * p [ i ] + sclq * qt [ i ];
	}
}

DARKSDK void CalculateBonePosition ( int frame, float s, sMDLBone* pBone, sMDLAnimation* pAnim, float* pos )
{
	int	j, k;
	unAnimValue* pAnimValue = NULL;
	
	for ( j = 0; j < 3; j++ )
	{
		pos [ j ] = pBone->fValue [ j ];
	
		if ( pAnim->usOffset [ j ] != 0 )
		{
			pAnimValue = ( unAnimValue*) ( ( byte* ) pAnim + pAnim->usOffset [ j ] );
			k          = frame;

			while ( pAnimValue->num.byTotal <= k )
			{
				k          -= pAnimValue->num.byTotal;
				pAnimValue += pAnimValue->num.byValid + 1;
			}

			if ( pAnimValue->num.byValid > k )
			{
				if ( pAnimValue->num.byValid > k + 1)
					pos [ j ] += (float) (( pAnimValue [ k + 1 ].value * ( 1.0 - s ) + s * pAnimValue [ k + 2 ].value ) * pBone->fScale [ j ]);
				else
					pos [ j ] += pAnimValue [ k + 1 ].value * pBone->fScale [ j ];
			}
			else
			{
				if ( pAnimValue->num.byTotal <= k + 1 )
					pos [ j ] += (float) (( pAnimValue [ pAnimValue->num.byValid ].value * ( 1.0 - s ) + s * pAnimValue [ pAnimValue->num.byValid + 2 ].value ) * pBone->fScale [ j ]);
				else
					pos [ j ] += pAnimValue [ pAnimValue->num.byValid ].value * pBone->fScale [ j ];
			}
		}

		if ( pBone->iBoneControl [ j ] != -1 )
			pos [ j ] += 0.0;
	}
}

DARKSDK bool GetFileData ( char* szFilename )
{
	// open the file
	if ( ( g_FP = fopen ( szFilename, "rb" ) ) == NULL )
		return false;

	// get the file size
	fseek ( g_FP, 0, SEEK_END );
	g_iFileSize = ftell ( g_FP );
	fseek ( g_FP, 0, SEEK_SET );
	
	// create enough memory to hold file data
	g_pbData = new BYTE [ g_iFileSize ];

	// check new memory
	SAFE_MEMORY ( g_pbData );

	// read in data
	fread ( g_pbData, g_iFileSize, 1, g_FP );

	// close the file
	fclose ( g_FP );

	return true;
}

DARKSDK bool GetHeaderData ( void )
{
	g_pHeader = ( sMDLHeader* ) ( BYTE* ) g_pbData;

	return true;
}

DARKSDK bool GetTextureData ( void )
{
	// create a block of memory to store the texture data
	g_pTextures = new sMDLTexture [ g_pHeader->iNumSkinRef ];

	memset ( g_pTextures, 0, sizeof ( sMDLTexture ) * g_pHeader->iNumSkinRef );

	// check the new memory
	SAFE_MEMORY ( g_pTextures );

	// get the first texture data
	sMDLTexture* pTextureData = ( sMDLTexture* ) ( ( BYTE* ) g_pbData + g_pHeader->iTextureIndex );

	// run through each texture
	for ( int iTexture = 0; iTexture < g_pHeader->iNumTextures; iTexture++ )
	{
		
		// load the texture from the file so it can be extracted
		if ( !( UploadTexture 
								( 
									&pTextureData [ iTexture ],
									g_pbData + pTextureData [ iTexture ].iIndex, 
									g_pbData + pTextureData [ iTexture ].iWidth * pTextureData [ iTexture ].iHeight + pTextureData [ iTexture ].iIndex
								) ) )
			return false;
		
		// store data in texture structure
		memcpy ( g_pTextures + iTexture, pTextureData + iTexture, sizeof ( sMDLTexture ) );

		g_pTextures [ iTexture ].iIndex = iTexture;
		pTextureData [ iTexture ].iIndex = iTexture;
	}

	return true;
}

DARKSDK bool UploadTexture ( sMDLTexture* pTexture, BYTE* pbData, BYTE* pbPal )
{
	// get the texture from the file

	int		i,
			j,
			iRow1 [ 256 ],
		    iRow2 [ 256 ],
			iCol1 [ 256 ],
			iCol2 [ 256 ];
	BYTE	*bPix1,
		    *bPix2,
			*bPix3,
			*bPix4,
			*pTex,
			*pOut;

	// mike - 020206 - addition for vs8
	int iOutWidth = 0;
	int iOutHeight = 0;

	// convert texture to power of 2
	for ( iOutWidth = 1; iOutWidth < pTexture->iWidth; iOutWidth <<= 1 )
		;

	for ( iOutHeight = 1; iOutHeight < pTexture->iHeight; iOutHeight <<= 1 )
		;

	// make sure texture sizes don't go above 256
	if ( iOutWidth > 256 )
		iOutWidth = 256;

	if ( iOutHeight > 256 )
		iOutHeight = 256;

	pTex = new BYTE [ iOutWidth * iOutHeight * 4 ];
	pOut = pTex;

	if ( !pTex )
		return false;

	for ( i = 0; i < iOutWidth; i++ )
	{
		iCol1 [ i ] = (int) (( i + 0.25 ) * ( pTexture->iWidth / ( float ) iOutWidth ));
		iCol2 [ i ] = (int) (( i + 0.75 ) * ( pTexture->iWidth / ( float ) iOutWidth ));
	}

	for ( i = 0; i < iOutHeight; i++ )
	{
		iRow1 [ i ] = ( int ) ( ( i + 0.25 ) * ( pTexture->iHeight / ( float ) iOutHeight ) ) * pTexture->iWidth;
		iRow2 [ i ] = ( int ) ( ( i + 0.75 ) * ( pTexture->iHeight / ( float ) iOutHeight ) ) * pTexture->iWidth;
	}

	LPDIRECT3DTEXTURE9	lpTexture = NULL;

	if ( FAILED ( D3DXCreateTexture ( m_pD3D, iOutWidth, iOutHeight, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &lpTexture ) ) )
		return false;

	D3DLOCKED_RECT	d3dlr;
	int				iCount = 0;

	if ( FAILED ( lpTexture->LockRect ( 0, &d3dlr, NULL, 0 ) ) )
		return false;

	DWORD* pPix = ( DWORD* ) d3dlr.pBits;

	for ( i = 0; i < iOutHeight; i++ )
	{
		for ( j = 0; j < iOutWidth; j++, pOut += 4 )
		{
			bPix1 = &pbPal [ pbData [ iRow1 [ i ] + iCol1 [ j ] ] * 3 ];
			bPix2 = &pbPal [ pbData [ iRow1 [ i ] + iCol2 [ j ] ] * 3 ];
			bPix3 = &pbPal [ pbData [ iRow2 [ i ] + iCol1 [ j ] ] * 3 ];
			bPix4 = &pbPal [ pbData [ iRow2 [ i ] + iCol2 [ j ] ] * 3 ];

			pOut [ 0 ] = ( bPix1 [ 0 ] + bPix2 [ 0 ] + bPix3 [ 0 ] + bPix4 [ 0 ] ) >> 2;
			pOut [ 1 ] = ( bPix1 [ 1 ] + bPix2 [ 1 ] + bPix3 [ 1 ] + bPix4 [ 1 ] ) >> 2;
			pOut [ 2 ] = ( bPix1 [ 2 ] + bPix2 [ 2 ] + bPix3 [ 2 ] + bPix4 [ 2 ] ) >> 2;
			pOut [ 3 ] = 0xFF;

			*pPix++ = D3DCOLOR_ARGB ( 255, pOut [ 0 ], pOut [ 1 ], pOut [ 2 ] );
		}
	}

	lpTexture->UnlockRect ( NULL );

	// leefix - 110503 - saves into temp folder
	char pFinalFile[_MAX_PATH];
	strcpy ( pFinalFile, g_WindowsTempDirectory );
	strcat ( pFinalFile, pTexture->sName );
	D3DXSaveTextureToFile ( pFinalFile, D3DXIFF_BMP, lpTexture, NULL );

	lpTexture->Release ( );

	SAFE_DELETE_ARRAY ( pTex );

	return true;
}

#ifdef DARKSDK_COMPILE

void	PassCoreData	( LPVOID pGlobPtr );
bool	Convert			( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
void	Free			( LPSTR );

void	PassCoreDataMDL ( LPVOID pGlobPtr )
{
	PassCoreData ( pGlobPtr );
}

bool	ConvertMDL	( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize )
{
	return Convert	( pFilename, pBlock, pdwSize );
}

void	FreeMDL	( LPSTR pfile )
{
	Free ( pfile ); 
}

#endif


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
	if ( !DBOConvertObjectToBlock ( g_pObject, pBlock, pdwSize ) )
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

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////