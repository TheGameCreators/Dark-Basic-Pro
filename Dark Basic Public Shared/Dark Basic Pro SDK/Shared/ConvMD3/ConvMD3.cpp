
//////////////////////////////////////////////////////////////////////////////////
// INCLUDES / LIBS ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

#include "ConvMD3.h"			// MD3 include
#include < mmsystem.h >			// multimedia functions
#include < vector >				// using STL vector
using namespace std;			// namespace info

#ifdef DARKSDK_COMPILE
	#include ".\..\..\..\DarkGDK\Code\Include\DarkSDKDisplay.h"
#endif

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

DBPRO_GLOBAL GFX_GetDirect3DDevicePFN		g_GFX_GetDirect3DDevice;		
DBPRO_GLOBAL HINSTANCE						g_hSetup						= NULL;
DBPRO_GLOBAL LPDIRECT3DDEVICE9				m_pD3D							= NULL;
DBPRO_GLOBAL GlobStruct*					g_pGlob							= NULL;
DBPRO_GLOBAL sObject**						g_ObjectList;
DBPRO_GLOBAL sObject*						g_pObject = NULL;				// DBO object
DBPRO_GLOBAL MD3							g_MD3           [   3 ];		// MD3 object, one for each body part
DBPRO_GLOBAL char							g_szPath        [ 256 ];		// model path
DBPRO_GLOBAL char							g_szLower       [ 256 ];		// lower body part
DBPRO_GLOBAL char							g_szUpper       [ 256 ];		// upper body part
DBPRO_GLOBAL char							g_szHead        [ 256 ];		// head
DBPRO_GLOBAL char							g_szLowerSkin   [ 256 ];		// lower skin
DBPRO_GLOBAL char							g_szUpperSkin   [ 256 ];		// upper skin
DBPRO_GLOBAL char							g_szHeadSkin    [ 256 ];		// head skin
DBPRO_GLOBAL char							g_szAnimation   [ 256 ];		// animation config
DBPRO_GLOBAL char							g_szLastTexture [ 256 ];		// last texture
DBPRO_GLOBAL UCHAR*							g_pBuffer     = NULL;			// file buffer
DBPRO_GLOBAL UCHAR*							g_pTempBuffer = NULL;			// temp file buffer
DBPRO_GLOBAL vector < sFrame* >				g_pFrameList;					// frame list
DBPRO_GLOBAL float							g_fShrinkObjectsTo				= 0.0f;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

DARKSDK bool			GetModelPartNames         ( char* szFilename );
DARKSDK bool			GetModelData              ( int iID, char* szFilename, MD3* pMD3 );
DARKSDK bool			GetHeaderData             ( int iID, MD3* pMD3 );
DARKSDK bool			GetTagData                ( int iID, MD3* pMD3 );
DARKSDK bool			GetMeshData               ( int iID, MD3* pMD3 );
DARKSDK D3DXQUATERNION	QuaternionFrom3x3Matrix   ( float matrix [ 9 ] );
DARKSDK D3DXMATRIX		MatrixFromQuaternion      ( D3DXQUATERNION quat );
DARKSDK bool			AddMeshToList             ( MD3* pMD3, MD3Tag* pTag );
DARKSDK bool            CopyDataAcrossToMesh      ( sFrame* pFrame, MD3Mesh* pMD3Mesh, int iFrame, MD3Tag* pTag );
DARKSDK bool			RemoveRedundantTransforms ( void );

DARKSDK bool Delete ( void )
{
	for ( int iObject = 0; iObject < 3; iObject++ )
	{
		SAFE_DELETE_ARRAY ( g_MD3 [ iObject ].pInterpTags );
		SAFE_DELETE_ARRAY ( g_MD3 [ iObject ].pTags       );
		SAFE_DELETE_ARRAY ( g_MD3 [ iObject ].pMeshes     );
	}

	return true;
};

DARKSDK bool Load ( char* szFilename )
{
	// load an MD3 file and convert into a DBO object

	// check the filename is valid
	SAFE_MEMORY ( szFilename );

	// get the names for the model
	GetModelPartNames ( szFilename );

	// load each part of the model
	GetModelData ( 0, g_szLower, &g_MD3 [ 0 ] );
	GetModelData ( 0, g_szUpper, &g_MD3 [ 1 ] );
	GetModelData ( 0, g_szHead,  &g_MD3 [ 2 ] );

	// add the meshes into the list
	AddMeshToList ( &g_MD3 [ 0 ], NULL );
	AddMeshToList ( &g_MD3 [ 1 ], &g_MD3 [ 0 ].pTags [ 0 ] );
	AddMeshToList ( &g_MD3 [ 2 ], &g_MD3 [ 1 ].pTags [ 0 ] );

	// create a new object
	g_pObject         = new sObject;
	g_pObject->pFrame = new sFrame;
	sFrame* pFrame    = g_pObject->pFrame;
	
	// remove transforms which we don't need
	RemoveRedundantTransforms ( );

	// add all of the frames into the object
    for ( std::vector< sFrame* >::size_type iFrame = 0; iFrame < g_pFrameList.size ( ); iFrame++ )
	{
		pFrame->pMesh       = g_pFrameList [ iFrame ]->pMesh;
		pFrame->matOriginal = g_pFrameList [ iFrame ]->matOriginal;
		
		strcpy ( pFrame->szName, g_pFrameList [ iFrame ]->szName );

		pFrame->pMesh->bLinked        = true;
		pFrame->pMesh->dwTextureCount = 1;
		pFrame->pMesh->bUsesMaterial  = true;
		
		pFrame->pChild = new sFrame;
		pFrame         = pFrame->pChild;
	}

	pFrame = g_pObject->pFrame;
	
	return true;
}

DARKSDK bool RemoveRedundantTransforms ( void )
{
	// take out transforms which won't be applied to the drawing process, we only apply
	// the last transform of a body part to the model so we need to remove some transforms
	// here so that they don't get used in the main drawing process

	// go from the last frame to the end
	for ( int iFrame = g_pFrameList.size ( ) - 1; iFrame > 0; iFrame-- )
	{
		// get the current frame
		sFrame* pFrame = g_pFrameList [ iFrame ];
		char    szMain [ 4 ];

		// clear out the string
		memset ( szMain, 0, sizeof ( szMain ) );

		// skip if there isn't enough info
		if ( strlen ( pFrame->szName ) < 2 )
			continue;
	
		// make sure info is valid
		if ( strlen ( pFrame->szName ) > 2 )
			memcpy ( &szMain, &pFrame->szName, sizeof ( char ) * 2 );

		// now check frames below for any other instances of this
		for ( int iSubFrame = iFrame - 1; iSubFrame >= 0; iSubFrame-- )
		{
			// get the sub frame
			sFrame* pSubFrame = g_pFrameList [ iSubFrame ];
			char    szSub [ 4 ];

			// clear out the sub string
			memset ( szSub, 0, sizeof ( szSub ) );

			// skip if there isn't enough info
			if ( strlen ( pSubFrame->szName ) < 2 )
				continue;
	
			// make sure info is valid
			if ( strlen ( pSubFrame->szName ) > 2 )
				memcpy ( szSub, pSubFrame->szName, sizeof ( char ) * 2 );

			// see if we find a match
			if ( strcmp ( szMain, szSub ) == 0 )
			{
				/*
				// set to an identity matrix
				D3DXMatrixIdentity ( &pSubFrame->matOriginal );

				sFrame* pSubFrameLink = pSubFrame->pMesh->pSubFrameList;

				// clear out sub frames
				for ( int iSubFrameList = 0; iSubFrameList < pSubFrame->pMesh->dwSubMeshListCount; iSubFrameList++ )
				{
					D3DXMatrixIdentity ( &pSubFrameLink->matOriginal );
					pSubFrameLink = pSubFrameLink->pChild;
				}
				*/				
	
				D3DXMatrixIdentity ( &pSubFrame->matOriginal );

				for ( DWORD iSubFrameList = 0; iSubFrameList < pSubFrame->pMesh->dwSubMeshListCount; iSubFrameList++ )
				{
					D3DXMatrixIdentity ( &pSubFrame->pMesh->pSubFrameList [ iSubFrameList ].matOriginal );
				}
			}
		}
	}

	return true;
}

DARKSDK bool AddMeshToList ( MD3* pMD3, MD3Tag* pTag )
{
	// adds a mesh to the list

	// go through each mesh within the MD3
	for ( int iMesh = 0; iMesh < pMD3->Header.iNumMeshes; iMesh++ )
	{
		// MD3 mesh, frame and DBO mesh
		MD3Mesh* pMD3Mesh = NULL;
		sFrame*  pFrame   = NULL;
		sMesh*   pMesh    = NULL;
		sMesh*   pSubMesh = NULL;

		// set up pointers
		pMD3Mesh             = &pMD3->pMeshes [ iMesh ];										// MD3 mesh
		pFrame               = new sFrame;														// new frame
		pFrame->pMesh        = new sMesh;														// new mesh for the frame
		pMesh                = pFrame->pMesh;													// set pointer to mesh
		pMesh->pTextures     = new sTexture [ 1 ];													// allocate new texture
		pMesh->pSubFrameList = new sFrame [ pMD3->pMeshes [ iMesh ].Header.iNumMeshFrames ];	// animation frames
		//pMesh->pSubFrameList = new sFrame;														// animation frames
		
		pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;

		// set up strings
		strcpy ( pFrame->szName,          pMD3Mesh->Header.szName );	// copy MD3 mesh name over to frame
		strcpy ( pMesh->pTextures->pName, g_szPath );					// copy the MD3 path to the texture name
		
		if ( strlen ( pMD3Mesh->pSkins->szName ) > 0 )
		{
			strcpy ( g_szLastTexture, pMD3Mesh->pSkins->szName );
			strcat ( pMesh->pTextures->pName, pMD3Mesh->pSkins->szName );
		}
		else
			strcat ( pMesh->pTextures->pName, g_szLastTexture );

		// set frame matrix
		D3DXMatrixIdentity ( &pFrame->matOriginal );

		// number of sub meshes within the main mesh
		pFrame->pMesh->dwSubMeshListCount = pMD3->pMeshes [ iMesh ].Header.iNumMeshFrames;

		// initial frame 0
		CopyDataAcrossToMesh ( pFrame, pMD3Mesh, 0, &pMD3->pTags [ 0 ] );

		MD3Tag* pLastTag = NULL;

		sFrame* pCurrent = pMesh->pSubFrameList;

		// we need to take all of the animation frames and add into the sub mesh list
		for ( int iSubMesh = 0; iSubMesh < pMD3->pMeshes [ iMesh ].Header.iNumMeshFrames; iSubMesh++ )
		{
			MD3Tag* pSubTag = pTag;

			int iFrame = iSubMesh;

			/////////
			
			pSubTag = &pMD3->pTags [ iSubMesh * pMD3->Header.iNumTags ];
			pMesh->pSubFrameList [ iSubMesh ].pMesh = new sMesh;

			pMesh->pSubFrameList [ iSubMesh ].pMesh->iPrimitiveType = D3DPT_TRIANGLELIST;
			CopyDataAcrossToMesh ( &pMesh->pSubFrameList [ iSubMesh ], pMD3Mesh, iSubMesh, pSubTag );
			
			/////////

			/*
			pSubTag = &pMD3->pTags [ iSubMesh * pMD3->Header.iNumTags ];
			pCurrent->pMesh = new sMesh;
			CopyDataAcrossToMesh ( pCurrent, pMD3Mesh, iSubMesh, pSubTag );
			
			pCurrent->pChild = new sFrame;
			pCurrent = pCurrent->pChild;
			*/
		}

		// send the mesh to the back of the mesh list
		g_pFrameList.push_back ( pFrame );
	}

	return true;
}

DARKSDK bool CopyDataAcrossToMesh ( sFrame* pFrame, MD3Mesh* pMD3Mesh, int iFrame, MD3Tag* pTag )
{
	// copy data from an MD3 frame into a DBO mesh

	sMesh* pMesh = pFrame->pMesh;

	// check the parameters are valid
	SAFE_MEMORY ( pMesh    );
	SAFE_MEMORY ( pMD3Mesh );

	if ( iFrame < 0 )
		return false;

	// sort out mesh properties
	pMesh->dwVertexCount    = pMD3Mesh->Header.iNumVerts;									// number of vertices
	pMesh->dwIndexCount     = pMD3Mesh->Header.iNumTriangles * 3;							// number of indices
	pMesh->iDrawVertexCount = pMD3Mesh->Header.iNumVerts;									// vertices to draw
	pMesh->iDrawPrimitives  = pMD3Mesh->Header.iNumTriangles;								// primitives to draw
	pMesh->dwFVFOriginal    = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;	// original fvf
	pMesh->dwFVF		    = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;	// current fvf
	pMesh->dwFVFSize        = sizeof ( float ) * 8 + sizeof ( DWORD );						// fvf size
	pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;

	// create FVF space
	SetupMeshFVFData ( pMesh, pMesh->dwFVF, pMesh->dwVertexCount, pMesh->dwIndexCount );

	// get the offset map for the vertex data
	sOffsetMap offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	if ( pTag )
	{
		D3DXMATRIX	matTemp;
		D3DXMatrixIdentity ( &matTemp );
		matTemp        = MatrixFromQuaternion ( pTag->qRot );
		matTemp [ 12 ] = pTag->vecPos.x;
		matTemp [ 13 ] = -pTag->vecPos.z;
		matTemp [ 14 ] = pTag->vecPos.y;
		pFrame->matOriginal = matTemp;
	}

	// copy the vertices across
	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	{
		if ( pMesh->dwFVF & D3DFVF_XYZ )
		{
			*( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) =  pMD3Mesh->pKeyFrames [ iFrame ].pVertices [ iVertex ].x;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = -pMD3Mesh->pKeyFrames [ iFrame ].pVertices [ iVertex ].z;
			*( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) =  pMD3Mesh->pKeyFrames [ iFrame ].pVertices [ iVertex ].y;
		}

		if ( pMesh->dwFVF & D3DFVF_TEX1 )
		{
			*( ( float* ) pMesh->pVertexData + offsetMap.dwTU [ 0 ] + ( offsetMap.dwSize * iVertex ) ) = pMD3Mesh->pTexCoords [ iVertex ].fTexCoord [ 0 ];
			*( ( float* ) pMesh->pVertexData + offsetMap.dwTV [ 0 ] + ( offsetMap.dwSize * iVertex ) ) = pMD3Mesh->pTexCoords [ iVertex ].fTexCoord [ 1 ];
		}
	}

	// copy the indices
	int iIndexPos = 0;
	for ( int iIndex = 0; iIndex < ( int ) pMesh->dwIndexCount / 3; iIndex++ )
	{
		pMesh->pIndices [ iIndexPos++ ] = pMD3Mesh->pFaces [ iIndex ].uiIndices [ 0 ];
		pMesh->pIndices [ iIndexPos++ ] = pMD3Mesh->pFaces [ iIndex ].uiIndices [ 2 ];
		pMesh->pIndices [ iIndexPos++ ] = pMD3Mesh->pFaces [ iIndex ].uiIndices [ 1 ];
	}

	// leefix - 150503 - generate normals for mesh
	GenerateNormals ( pMesh );

	return true;
}

DARKSDK bool Draw ( void )
{
	D3DXMATRIX matScale, matTranslation, matRotation, matWorld, matRotateX, matRotateY, matRotateZ;

	// create a scaling and position matrix
	D3DXMatrixScaling ( &matScale, 1.0f, 1.0f, 1.0f );
	D3DXMatrixTranslation ( &matTranslation, 0.0f, -2.0f, 120.0f );

	// handle rotations
	D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( 90.0f ) );
	D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( 90.0f ) );
	D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( 180.0f ) );

	// build up final rotation and world matrix
	matRotation = matRotateX  * matRotateY * matRotateZ;
	matWorld    = matRotation * matScale   * matTranslation;

	m_pD3D->SetTransform ( D3DTS_WORLD, &matWorld );

	// dx8->dx9
	m_pD3D->SetVertexShader ( NULL );
	m_pD3D->SetFVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

	m_pD3D->SetRenderState ( D3DRS_LIGHTING, FALSE );
	m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );

	m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	sFrame* pFrame = g_pObject->pFrame;	

	while ( pFrame )
	{
		if ( pFrame->pMesh )
		{
			sMesh* pMesh = pFrame->pMesh;

			pFrame->pMesh->dwThisTime = timeGetTime ( );

			if ( pFrame->pMesh->dwThisTime > pFrame->pMesh->dwLastTime + 65 )
			{
				if ( pFrame->pMesh->iCurrentFrame < 200 )
					pFrame->pMesh->iCurrentFrame++;
				else
					pFrame->pMesh->iCurrentFrame = 0;

				pFrame->pMesh->dwLastTime = pFrame->pMesh->dwThisTime;
			}

			pFrame->pMesh->iLastFrame = pFrame->pMesh->iCurrentFrame - 1;
			pFrame->pMesh->iNextFrame = pFrame->pMesh->iCurrentFrame + 1;

			if ( pFrame->pMesh->iLastFrame < 0 )
				pFrame->pMesh->iLastFrame = 0;

			if ( pFrame->pMesh->iNextFrame > 200 )
				pFrame->pMesh->iNextFrame = 0;

			if ( (DWORD)pFrame->pMesh->iNextFrame >= pFrame->pMesh->dwSubMeshListCount )
				pFrame->pMesh->iNextFrame = 0;


			// get the offset map for the vertex data
			sOffsetMap offsetMap;
			GetFVFOffsetMap ( pMesh, &offsetMap );

			// copy the vertices across
			if ( (DWORD)pFrame->pMesh->iCurrentFrame < pFrame->pMesh->dwSubMeshListCount )
			{
				for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
				{
					if ( pMesh->dwFVF & D3DFVF_XYZ )
					{
						pFrame->matOriginal = pMesh->pSubFrameList [ pFrame->pMesh->iCurrentFrame ].matOriginal;

						D3DXVECTOR3 vecLastPos = D3DXVECTOR3 (
																*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iLastFrame ].pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
																*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iLastFrame ].pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
																*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iLastFrame ].pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
															 );

						D3DXVECTOR3 vecNextPos = D3DXVECTOR3 (
																*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame ].pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
																*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame ].pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
																*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iNextFrame ].pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
															 );

						D3DXVECTOR3 vecCurrPos = D3DXVECTOR3 (
																*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iCurrentFrame ].pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
																*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iCurrentFrame ].pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
																*( ( float* ) pMesh->pSubFrameList [ pFrame->pMesh->iCurrentFrame ].pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
															 );

						D3DXVECTOR3 vecFinal = vecLastPos + ( vecNextPos - vecLastPos ) * 0.0001f;
						
						*( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecFinal.x;
						*( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecFinal.y;
						*( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecFinal.z;
					}
				}
			}
		}

		pFrame = pFrame->pChild;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	pFrame = g_pObject->pFrame;

	D3DXMATRIX	matSavedWorld,
				matNewWorld;

	while ( pFrame )
	{
		if ( pFrame->pMesh )
		{
			if ( pFrame->pMesh->pTextures->pTexturesRef )
				m_pD3D->SetTexture ( 0, pFrame->pMesh->pTextures->pTexturesRef );

			m_pD3D->DrawIndexedPrimitiveUP (
												D3DPT_TRIANGLELIST,
												0,
												pFrame->pMesh->dwVertexCount,
												pFrame->pMesh->iDrawPrimitives,
												pFrame->pMesh->pIndices,
												D3DFMT_INDEX16,
												pFrame->pMesh->pVertexData,
												pFrame->pMesh->dwFVFSize
										);

			m_pD3D->GetTransform ( D3DTS_WORLD, &matSavedWorld );
			D3DXMatrixMultiply   ( &matNewWorld, ( D3DXMATRIX* ) &pFrame->matOriginal, &matSavedWorld );
			m_pD3D->SetTransform ( D3DTS_WORLD, &matNewWorld );
		}

		pFrame = pFrame->pChild;
	}
	
	return true;
}

DARKSDK bool GetModelData ( int iID, char* szFilename, MD3* pMD3 )
{
	// extract data from the model

	// attempt to open the file
	FILE* fp = NULL;
	
	if ( !( fp = fopen ( szFilename, "rb" ) ) )
		return false;

	// local variables
	int		iFileSize = 0;
	int     iEnd      = 0;
	int		iStart    = 0;

	// get the start, end and file size
	fseek ( fp, 0, SEEK_END );
	iEnd = ftell ( fp );
	fseek ( fp, 0, SEEK_SET );
	iStart = ftell ( fp );
	iFileSize = iEnd - iStart;

	// allocate a buffer to store the data
	g_pBuffer     = new unsigned char [ iFileSize ];
	g_pTempBuffer = g_pBuffer;

	// check the newly allocated memory
	SAFE_MEMORY ( g_pBuffer );

	// get all of the data from the file into the buffer
	fread ( g_pBuffer, 1, iFileSize, fp );

	// finally close the file
	fclose ( fp );

	GetHeaderData ( iID, pMD3 );
	GetTagData    ( iID, pMD3 );
	GetMeshData   ( iID, pMD3 );

	SAFE_DELETE_ARRAY ( g_pBuffer );

	fclose ( fp );

	return true;
}

DARKSDK bool GetHeaderData ( int iID, MD3* pMD3 )
{
	// get the header
	pMD3->Header = *( MD3Header* ) g_pTempBuffer;

	// check the header is valid
	if ( pMD3->Header.iID != 860898377 || pMD3->Header.iVersion != 15 )
		return false;
	
	// move the pointer forward past the header and the bone frames
	g_pTempBuffer += sizeof ( MD3Header );
	g_pTempBuffer += pMD3->Header.iNumFrames * 56;

	return true;
}

DARKSDK bool GetTagData ( int iID, MD3* pMD3 )
{
	// allocate tag memory
	pMD3->pTags       = new MD3Tag [ pMD3->Header.iNumTags * pMD3->Header.iNumFrames ];
	pMD3->pInterpTags = new MD3Tag [ pMD3->Header.iNumTags ];

	// get all of the tag data
	for ( int iTag = 0; iTag < pMD3->Header.iNumTags * pMD3->Header.iNumFrames; iTag++ )
	{
		// get the name
		memcpy ( pMD3->pTags [ iTag ].szName, g_pTempBuffer, 64 );
		g_pTempBuffer += 64;

		// get the position
		memcpy ( pMD3->pTags [ iTag ].vecPos, g_pTempBuffer, 12 );
		g_pTempBuffer += 12;

		// get the matrix ( 3 * 3 ) and convert to a quaternion
		float matrix [ 9 ];
		memcpy ( matrix, g_pTempBuffer, 36 );
		pMD3->pTags [ iTag ].qRot = QuaternionFrom3x3Matrix ( matrix );
		g_pTempBuffer += 36;
	}

	return true;
}

DARKSDK bool GetMeshData ( int iID, MD3* pMD3 )
{
	pMD3->pMeshes = new MD3Mesh [ pMD3->Header.iNumMeshes ];

	UCHAR* pTemp = g_pTempBuffer;

	for ( int iMesh = 0; iMesh < pMD3->Header.iNumMeshes; iMesh++ )
	{
		memcpy ( &pMD3->pMeshes [ iMesh ].Header, pTemp, sizeof ( MD3MeshHeader ) );
		
		pTemp += pMD3->pMeshes [ iMesh ].Header.iHeaderSize;

		pMD3->pMeshes [ iMesh ].pSkins = new MD3Skin [ pMD3->pMeshes [ iMesh ].Header.iNumSkins ];

		for ( int iSkin = 0; iSkin < pMD3->pMeshes [ iMesh ].Header.iNumSkins; iSkin++ )
		{
			char szName  [ 68 ];
			char szNameA [ 68 ];

			memset ( szNameA, 0, sizeof ( szNameA ) );
			
			memcpy ( szName, pTemp, 68 );
			pTemp += 68;

			int iPos = 0;

			for ( int iTemp = 0; iTemp < 68; iTemp++ )
			{
				if ( szName [ iTemp ] != 0 )
				{
					szNameA [ iPos++ ] = szName [ iTemp ];
				}
			}

			// string to store pathname without MD3 extension
			char szFinal [ 68 ];
//			char szTemp  [ 68 ]; -redundant!

			// clear the string
			memset ( szFinal, 0, sizeof ( szFinal ) );

			int iLen = strlen ( szNameA );
			int iCount = 0;

			// go through each character in the string, from the back until
			// we find a "." for the extension
			for ( int iChar = strlen ( szNameA ); iChar > 0; iChar--, iCount++ )
			{
				if ( szNameA [ iChar ] == '/' || szNameA [ iChar ] == '\\' )
				{
					// copy the path to the string
					memcpy ( szFinal, &szNameA [ iChar + 1 ], sizeof ( char ) * iCount );
					break;
				}
			}

			strcpy ( pMD3->pMeshes [ iMesh ].pSkins [ iSkin ].szName, szFinal );
		}

		pMD3->pMeshes [ iMesh ].pKeyFrames = new MD3KeyFrame [ pMD3->pMeshes [ iMesh ].Header.iNumMeshFrames ];
		pTemp = g_pTempBuffer + pMD3->pMeshes [ iMesh ].Header.iVertexOffset;
		
		for ( int iFrame = 0; iFrame < pMD3->pMeshes [ iMesh ].Header.iNumMeshFrames; iFrame++ )
		{
			pMD3->pMeshes [ iMesh ].pKeyFrames [ iFrame ].pVertices = new MD3Vertex [ pMD3->pMeshes [ iMesh ].Header.iNumVerts ];

			for ( int iVertex = 0; iVertex < pMD3->pMeshes [ iMesh ].Header.iNumVerts; iVertex++ )
			{
				short sTemp = *( short* ) pTemp;
				pMD3->pMeshes [ iMesh ].pKeyFrames [ iFrame ].pVertices [ iVertex ].x = ( float ) sTemp / 64.0f;

				sTemp = *( short* ) &pTemp [ 2 ];
				pMD3->pMeshes [ iMesh ].pKeyFrames [ iFrame ].pVertices [ iVertex ].y = ( float ) sTemp / 64.0f;

				sTemp = *( short* ) &pTemp [ 4 ];
				pMD3->pMeshes [ iMesh ].pKeyFrames [ iFrame ].pVertices [ iVertex ].z = ( float ) sTemp / 64.0f;
			
				pTemp += 8;
			}
		}

		pMD3->pMeshes [ iMesh ].pFaces = new MD3Face [ pMD3->pMeshes [ iMesh ].Header.iNumTriangles ];

		pTemp = g_pTempBuffer + pMD3->pMeshes [ iMesh ].Header.iTriOffset;

		for ( int iIndex = 0; iIndex < pMD3->pMeshes [ iMesh ].Header.iNumTriangles; iIndex++ )
		{
			memcpy ( pMD3->pMeshes [ iMesh ].pFaces [ iIndex ].uiIndices, pTemp, 12 );
			pTemp += 12;
		}

		pMD3->pMeshes [ iMesh ].pTexCoords = new MD3TexCoord [ pMD3->pMeshes [ iMesh ].Header.iNumVerts ];

		pTemp = g_pTempBuffer + pMD3->pMeshes [ iMesh ].Header.iUVOffset;

		for ( int iTex = 0; iTex < pMD3->pMeshes [ iMesh ].Header.iNumVerts; iTex++ )
		{
			memcpy ( pMD3->pMeshes [ iMesh ].pTexCoords [ iTex ].fTexCoord, pTemp, 8  );
			pTemp += 8;
		}

		pMD3->pMeshes [ iMesh ].pInterp = 0;

		g_pTempBuffer += pMD3->pMeshes [ iMesh ].Header.iMeshSize;
		pTemp = g_pTempBuffer;
	}

	return true;
}

DARKSDK bool GetModelPartNames ( char* szFilename )
{
	// get the filenames for the model parts

	// string to store pathname without MD3 extension
	char szFinal [ 256 ];

	// clear the string
	memset ( szFinal, 0, sizeof ( szFinal ) );

	// go through each character in the string, from the back until
	// we find a "." for the extension
	for ( int iChar = strlen ( szFilename ); iChar > 0; iChar-- )
	{
		if ( szFilename [ iChar ] == '.' )
		{
			// copy the path to the string
			memcpy ( szFinal, szFilename, sizeof ( char ) * iChar );
			break;
		}
	}

	// add the lower, upper, head and anim names to the strings
	sprintf ( g_szLower,     "%s/lower.md3",     szFinal );
	sprintf ( g_szUpper,     "%s/upper.md3",     szFinal );
	sprintf ( g_szHead,      "%s/head.md3",      szFinal );
	sprintf ( g_szAnimation, "%s/animation.cfg", szFinal );

	sprintf ( g_szPath,      "%s/", szFinal );

	sprintf ( g_szLowerSkin, "%s/lower_default.skin", szFinal );
	sprintf ( g_szUpperSkin, "%s/upper_default.skin", szFinal );
	sprintf ( g_szHeadSkin,  "%s/head_default.skin",  szFinal );
	
	return true;
}

DARKSDK D3DXMATRIX MatrixFromQuaternion ( D3DXQUATERNION quat )
{
	D3DXMATRIX	matrix;

	D3DXMatrixIdentity ( &matrix );

	matrix [  0 ] = 1.0f - 2.0f * ( quat.y * quat.y + quat.z * quat.z ); 
	matrix [  1 ] = 2.0f * ( quat.x * quat.y - quat.z * quat.w );
	matrix [  2 ] = 2.0f * ( quat.x * quat.z + quat.y * quat.w );

  	matrix [  4 ] = 2.0f * ( quat.x * quat.y + quat.z * quat.w );
	matrix [  5 ] = 1.0f - 2.0f * ( quat.x * quat.x + quat.z * quat.z );
	matrix [  6 ] = 2.0f * ( quat.y * quat.z - quat.x * quat.w );
	
	matrix [  8 ] = 2.0f * ( quat.x * quat.z - quat.y * quat.w );
	matrix [  9 ] = 2.0f * ( quat.y * quat.z + quat.x * quat.w );
	matrix [ 10 ] = 1.0f - 2.0f * ( quat.x * quat.x + quat.y * quat.y );

	matrix [ 15 ] = 1.0f;
	
	return matrix;
}

DARKSDK D3DXQUATERNION QuaternionFrom3x3Matrix ( float matrix [ 9 ] )
{
	D3DXQUATERNION	quat;

	float fTr = matrix [ 0 ] + matrix [ 4 ] + matrix [ 8 ] + 1.0f;

	if ( fTr > 0 )
	{
		// if the trace of the matrix is greater than 0, perform an instant calculation

		float fS = 0.5f / sqrtf ( fTr );

		quat.w = 0.25f / fS;
		quat.x = ( matrix [ 7 ] - matrix [ 5 ] ) * fS;
		quat.y = ( matrix [ 2 ] - matrix [ 6 ] ) * fS;
		quat.z = ( matrix [ 3 ] - matrix [ 1 ] ) * fS;
	}
	else if ( matrix [ 0 ] > matrix [ 4 ] && matrix [ 0 ] > matrix [ 8 ] )
	{
		// if the first diagonal element is the largest

		float fS = 1 / ( sqrt ( matrix [ 0 ] - matrix [ 4 ] - matrix [ 8 ] + 1.0f ) * 2 );

		quat.x = 0.5f * fS;
		quat.y = ( matrix [ 1 ] + matrix [ 3 ] ) * fS;
		quat.z = ( matrix [ 2 ] + matrix [ 6 ] ) * fS;
		quat.w = ( matrix [ 5 ] + matrix [ 7 ] ) * fS;
	}
	else if ( matrix [ 4 ] > matrix [ 8 ] )
	{
		// if the second

		float fS = 1 / ( sqrt ( matrix [ 4 ] - matrix [ 0 ] - matrix [ 8 ] + 1.0f ) * 2 );

		quat.x = ( matrix [ 1 ] + matrix [ 3 ] ) * fS;
		quat.y = 0.5f * fS;
		quat.z = ( matrix [ 5 ] + matrix [ 7 ] ) * fS;
		quat.w = ( matrix [ 2 ] + matrix [ 6 ] ) * fS;
	}
	else
	{
		// if the last element is the largest

		float fS = 1 / ( sqrt ( matrix [ 8 ] - matrix [ 0 ] - matrix [ 4 ] + 1.0f ) * 2 );

		quat.x = ( matrix [ 2 ] + matrix [ 6 ] ) * fS;
		quat.y = ( matrix [ 5 ] + matrix [ 7 ] ) * fS;
		quat.z = 0.5f * fS;
		quat.w = ( matrix [ 1 ] + matrix [ 3 ] ) * fS;
	}

	return quat;
}

// put these in to stop linker errors

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


DARKSDK bool GetD3DFromModule ( HINSTANCE hSetup )
{
	// get module
	#ifndef DARKSDK_COMPILE
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

#ifdef DARKSDK_COMPILE

bool	Convert		( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize );
void	Free		( LPSTR );

bool ConvertMD3 ( LPSTR pFilename, DWORD *pBlock, DWORD* pdwSize )
{
	return Convert ( pFilename, pBlock, pdwSize );
}

void FreeMD3 ( LPSTR pBlock )
{
	Free ( pBlock );
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
