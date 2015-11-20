
//
// CObjectManager Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////
// INCLUDE COMMON ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#include "CommonC.h"

// hacked in for now to get to see shadow mapping
#include "ShadowMapping\cShadowMaps.h"
extern CascadedShadowsManager g_CascadedShadow;
float g_fShrinkObjectsTo = 0.0f;

// Globals for new main camea depth texture
bool g_bMainCameraDepthCaptureActive = true;
bool g_bFirstRenderClearsRenderTarget = false;
IDirect3DTexture9* g_pMainCameraDepthTexture = NULL;
IDirect3DSurface9* g_pMainCameraDepthTextureSurfaceRef = NULL;
IDirect3DSurface9* g_pMainCameraDepthStencilTexture = NULL;
D3DXHANDLE g_pMainCameraDepthHandle = NULL;
LPD3DXEFFECT g_pMainCameraDepthEffect = NULL;

// Occlusion object external
#include "Occlusion\cOcclusion.h"
extern COcclusion g_Occlusion;

// extern from DBOFormat
extern D3DXMATRIX			g_matThisViewProj;
extern D3DXMATRIX			g_matThisViewProjInverse;
extern D3DXMATRIX			g_matPreviousViewProj;

// Globals for DBO/Manager relationship
std::vector< sMesh* >		g_vRefreshMeshList;
std::vector< sObject* >     g_vAnimatableObjectList;
int							g_iSortedObjectCount;
sObject**					g_ppSortedObjectList;

//////////////////////////////////////////////////////////////////////////////////
// GLOBAL STENCIL BUFFER /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Shadow Globals
#define D3DFVF_SHADOWVERTEX ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )
struct SHADOWVERTEX
{
	D3DXVECTOR4 p;
	D3DCOLOR    color;
};

// Reflection Plane Globals
DBPRO_GLOBAL bool							g_bNormalizeToggle				= false;
DBPRO_GLOBAL D3DXMATRIX						g_matReflectionViewSaved;
DBPRO_GLOBAL D3DXPLANE						g_plnReflectionPlane;
DBPRO_GLOBAL D3DXVECTOR3					g_vecReflectionPoint;
DBPRO_GLOBAL bool							g_bReflectiveClipping			= false;

// 301008 - Saver - New Shadow Clipping feature
int											g_iShadowClipping				= 0;
float										g_fShadowClippingMin			= 0;
float										g_fShadowClippingMax			= 0;

// U71 - 061208 - can prepare scene in two places now, so need flag to keeps tabs on it
bool										g_bScenePrepared				= false;
bool										g_bRenderVeryEarlyObjects		= false;

// U74 - 120409 - during LOD QUAD transition, use ZBIAS to move quad out of way when 3D fades in/out by THIS amount in total
float										g_fZBiasEpsilon					= 0.0005f;

// U77 - 110211 - detect AnisotropyLevel
int											g_iAnisotropyLevel				= -1;

// 170215 - Retain the last objects distance and use to decide if shader should toggle to LOD variant
float										g_fObjectCamDistance			= 0.0f;

//////////////////////////////////////////////////////////////////////////////////
// MANAGER FUNCTIONS /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// IMPORTANT - COMPILER SWITCH "/EHa" must be in for the "try", "catch" code

namespace
{
	// u74b7
    // Structures for sorting using the STL sort
    // Used for sorting the m_ppSortedObjectList, m_pDepthSortedList
    struct OrderByReverseCameraDistance
    {
        bool operator()(sObject* pObjectA, sObject* pObjectB)
        {
            if (pObjectA->position.fCamDistance > pObjectB->position.fCamDistance)
                return true;
            if (pObjectA->position.fCamDistance == pObjectB->position.fCamDistance)
                return (pObjectA->dwObjectNumber < pObjectB->dwObjectNumber);
            return false;
        }
    };
    // u74b8
    struct OrderByTexture
    {
        bool operator()(sObject* pObjectA, sObject* pObjectB)
        {
            int iImageA = 0;
            int iImageB = 0;

            // Calculate order by object number now,
            // just in case it's needed and to avoid getting incorrect results
            // if instances are involved.
            bool bObjectOrder = pObjectA->dwObjectNumber < pObjectB->dwObjectNumber;

            // Get the image id for object a
		    if ( pObjectA->pInstanceOfObject )
			    pObjectA = pObjectA->pInstanceOfObject;
		    if ( pObjectA->ppMeshList && pObjectA->ppMeshList [ 0 ]->pTextures)
                iImageA = pObjectA->ppMeshList [ 0 ]->pTextures [ 0 ].iImageID;

            // Get the image id for object b
		    if ( pObjectB->pInstanceOfObject )
			    pObjectB = pObjectB->pInstanceOfObject;
		    if ( pObjectB->ppMeshList && pObjectB->ppMeshList [ 0 ]->pTextures)
                iImageB = pObjectB->ppMeshList [ 0 ]->pTextures [ 0 ].iImageID;

            if (iImageA < iImageB)
                return true;
            if (iImageA > iImageB)
                return false;

            // Same images, so order by object id
            return bObjectOrder;
        }
    };
    struct OrderByObject
    {
        bool operator()(sObject* pObjectA, sObject* pObjectB)
        {
            return (pObjectA->dwObjectNumber < pObjectB->dwObjectNumber);
        }
    };
}

// mike - 281106 - move these into cpp file for debugging
CObjectManager::sVertexData::sVertexData ( )
{
	memset ( this, 0, sizeof(sVertexData) );
}

CObjectManager::sVertexData::~sVertexData ( )
{
	SAFE_RELEASE ( pVB );
	//SAFE_DELETE  ( pNext ); // 151214 - avoid stack overflow
	sVertexData* pThis = pNext;
	while ( pThis )
	{
		sVertexData* pNextOne = pThis->pNext;
		pThis->pNext = NULL;
		SAFE_RELEASE ( pThis->pVB );
		delete pThis;
		pThis = pNextOne;
	}
	pNext = NULL;
}

CObjectManager::sIndexData::sIndexData ( )
{
	memset ( this, 0, sizeof(sIndexData) );
}

CObjectManager::sIndexData::~sIndexData ( )
{
	SAFE_RELEASE ( pIB );
	//SAFE_DELETE  ( pNext ); // 151214 - avoid stack overflow
	sIndexData* pThis = pNext;
	while ( pThis )
	{
		sIndexData* pNextOne = pThis->pNext;
		pThis->pNext = NULL;
		SAFE_RELEASE ( pThis->pIB );
		delete pThis;
		pThis = pNextOne;
	}
	pNext = NULL;
}

bool CObjectManager::UpdateObjectListSize ( int iSize )
{
	// if list count is larger than size passed in, we can ignore a resize
	if ( iSize < m_iListCount )
		return true;

	// allocate memory
	sObject**	ppSortedObjectVisibleList = new sObject* [ iSize ];
	sObject**	ppSortedObjectList        = new sObject* [ iSize ];
	bool*		pbMarkedList              = new bool     [ iSize ];

	// safety checks on new memory
	SAFE_MEMORY ( ppSortedObjectVisibleList );
	SAFE_MEMORY ( ppSortedObjectList );
	SAFE_MEMORY ( pbMarkedList );

	// set all pointers to null
	for ( int iArrayIndex = 0; iArrayIndex < iSize; iArrayIndex++ )
	{
		ppSortedObjectVisibleList [ iArrayIndex ] = NULL;
		ppSortedObjectList        [ iArrayIndex ] = NULL;
		pbMarkedList              [ iArrayIndex ] = false;
	}

	// copy old data to new arrays
	int iSizeToCopyNow = m_iListCount;
	if ( iSizeToCopyNow > 0 )
	{
		if ( m_ppSortedObjectVisibleList ) memcpy ( ppSortedObjectVisibleList, m_ppSortedObjectVisibleList, sizeof(sObject*) * iSizeToCopyNow );
		if ( g_ppSortedObjectList ) memcpy ( ppSortedObjectList, g_ppSortedObjectList, sizeof(sObject*) * iSizeToCopyNow );
		if ( m_pbMarkedList ) memcpy ( pbMarkedList, m_pbMarkedList, sizeof(bool) * iSizeToCopyNow );
	}

	// safely delete any of the arrays
	SAFE_DELETE_ARRAY ( m_ppSortedObjectVisibleList );
	SAFE_DELETE_ARRAY ( g_ppSortedObjectList );
	SAFE_DELETE_ARRAY ( m_pbMarkedList );

	// allocate memory
	m_ppSortedObjectVisibleList = ppSortedObjectVisibleList;
	g_ppSortedObjectList        = ppSortedObjectList;
	m_pbMarkedList              = pbMarkedList;

	// store the size of the list
	m_iListCount = iSize;

	// return back
	return true;
}

bool CObjectManager::Setup ( void )
{
	// clear manager members
    // u74b8 - do this the correct & safe way so that embedded classes will work.
    //memset ( this, 0, sizeof ( CObjectManager ) );

	// Render State Global Defaults
    memset( &m_RenderStates, 0, sizeof( m_RenderStates ) );
	m_RenderStates.dwGlobalCullDirection = D3DCULL_CCW;

	// set all pointers to null
	m_ppCurrentVBRef	= NULL;
	m_ppLastVBRef		= NULL;
	m_ppCurrentIBRef	= NULL;
	m_ppLastIBRef		= NULL;

	m_dwCurrentShader = 0;
	m_dwCurrentFVF = 0;
	m_dwLastShader = 0;
	m_dwLastFVF = 0;

	m_iCurrentTexture = 0;
	m_iLastTexture = 0;
	m_dwLastTextureCount = 0;
	m_bUpdateTextureList = false;
	m_bUpdateVertexDecs = 0;
	m_bUpdateStreams = 0;
	g_iSortedObjectCount = 0;
	m_iLastCount = 0;
	m_iListCount = 0;
	m_iVisibleObjectCount = 0;
	m_pbMarkedList = 0;
	g_ppSortedObjectList = 0;
	m_ppSortedObjectVisibleList = 0;
	m_pVertexDataList = 0;
	m_pIndexDataList = 0;

	// STENCIL State Defaults
    memset( &m_StencilStates, 0, sizeof( m_StencilStates ) );
	m_StencilStates.dwShadowShades = 1;
	m_StencilStates.dwShadowStrength = 64;
	m_StencilStates.dwShadowColor = D3DCOLOR_ARGB ( 0, 0, 0, 0 );
	m_StencilStates.dwReflectionColor = D3DCOLOR_ARGB ( 64, 0, 0, 0 );

    // clear shadow VB ptr
	m_pSquareVB			= NULL;

	// post process shading using QUAD
	m_pQUAD					= NULL;
	m_pQUADDefaultEffect	= NULL;

	// Reset global stadows
	m_bGlobalShadows	= true;

    g_bObjectReplacedUpdateBuffers = false;

    m_pCamera = 0;

	// all okay
	return true;
}

void DeleteEx ( int iID );

bool CObjectManager::Free ( void )
{
	// delete all objects manually
	if ( g_ObjectList )
	{
		// ensure 'g_ObjectList' as Free is also called by Objectmanager destructor
		for ( int i = 0; i < g_iObjectListCount; i++ )
		{
			DeleteEx ( i );
		}
	}	

	// ensure this is deleted before leave
	SAFE_RELEASE ( g_pMainCameraDepthStencilTexture );
	SAFE_RELEASE ( g_pMainCameraDepthTextureSurfaceRef );
	SAFE_RELEASE ( g_pMainCameraDepthTexture );

	// safely delete any arrays and objects
	SAFE_DELETE ( m_pVertexDataList );
	SAFE_DELETE ( m_pIndexDataList );
	SAFE_DELETE_ARRAY ( m_pbMarkedList );
	SAFE_DELETE_ARRAY ( g_ppSortedObjectList );
	SAFE_DELETE_ARRAY ( m_ppSortedObjectVisibleList );

	// free shadow VB and QUAD mesh
	SAFE_RELEASE ( m_pSquareVB );
	SAFE_DELETE ( m_pQUAD );
	SAFE_DELETE ( m_pQUADDefaultEffect );

	// all okay
	return true;
}

//
// VERTEX AND INDEX BUFFERS
//

CObjectManager::sIndexData* CObjectManager::FindIndexBuffer ( DWORD dwIndexCount, bool bUsesItsOwnBuffers )
{
	// find an index buffer which we wan use

	// check D3D device is valid
	if ( !m_pD3D )
		return NULL;

	// make sure the parameter is valid
	if ( dwIndexCount < 1 )
		return NULL;

	// local variables
	bool		 bMatch = false;
	sIndexData*  pIndexData  = m_pIndexDataList;

	// get device capabilities
	D3DCAPS9 caps;
	m_pD3D->GetDeviceCaps ( &caps );

	// no search if need to use its own buffer
	if ( bUsesItsOwnBuffers )
		pIndexData=NULL;

	// run through all nodes in list
	while ( pIndexData )
	{
		// see if we can fit the data into the buffer
		if ( pIndexData->dwCurrentIndexCount + dwIndexCount < pIndexData->dwMaxIndexCount )
		{
			bMatch = true;
			break;
		}

		// move to next node
		pIndexData = pIndexData->pNext;
	}

	// if we don't have a match then create a new item
	if ( !bMatch )
	{
		if ( !m_pIndexDataList )
		{
			// create new list
			m_pIndexDataList	= new sIndexData;
			pIndexData			= m_pIndexDataList;

			// ensure creation okay
			SAFE_MEMORY ( m_pIndexDataList );
		}
		else
		{
			// find end of list
			pIndexData = m_pIndexDataList;
			while ( pIndexData )
			{
				if ( pIndexData->pNext )
				{
					pIndexData = pIndexData->pNext;
					continue;
				}
				else
					break;
			}

			// add new item to list
			pIndexData->pNext = new sIndexData ( );
			pIndexData        = pIndexData->pNext;
		}

		// create a decent start size of the IB
		DWORD dwIndexBufferSize = caps.MaxVertexIndex;

		// own buffer needs only to be the size of the data
		if ( bUsesItsOwnBuffers )
			dwIndexBufferSize = dwIndexCount;

		// loop until succeed in creating a IB
		bool  bCreate  = true;
		while ( bCreate )
		{
			// attempt to create a IB
			if ( FAILED ( m_pD3D->CreateIndexBuffer ( 
														sizeof ( WORD ) * dwIndexBufferSize,
														D3DUSAGE_WRITEONLY,
														D3DFMT_INDEX16,
														//D3DPOOL_DEFAULT, 250105 - FPSC-EA - large use of objects causes crash!
														//D3DPOOL_MANAGED, 270105 - perhaps managed takes too long to swapin/out
														D3DPOOL_DEFAULT,
														&pIndexData->pIB,
														NULL
													 ) ) )
			{
				// failed, try half the size
				dwIndexBufferSize /= 2;
			}
			else
			{
				// only if IB can hold required vertex data
				if ( dwIndexBufferSize >= dwIndexCount )
				{
					// success, we can use this size
					bCreate = false;
				}
				else
				{
					// IB created, but just too small!
					SAFE_RELEASE(pIndexData->pIB);
					return NULL;
				}
			}

			// if we continue until a ridiculously low value, we must fail
			if ( dwIndexBufferSize <= 0 )
				return NULL;
		}

		// ensure can fit inside the max size of a buffer
		if ( dwIndexCount > dwIndexBufferSize )
		{
			// if not, index data cannot fit inside single IB!
			SAFE_RELEASE ( pIndexData->pIB );
			return NULL;
		}

		// save the format of the buffer and the number allowed
		pIndexData->dwMaxIndexCount = dwIndexBufferSize;
		pIndexData->dwCurrentIndexCount = 0;
	}

	// return the final buffer
	return pIndexData;
}

CObjectManager::sVertexData* CObjectManager::FindVertexBuffer ( DWORD dwFVF, LPDIRECT3DVERTEXDECLARATION9 pVertexDec, DWORD dwSize, DWORD dwVertexCount, DWORD dwIndexCount, bool bUsesItsOwnBuffers, int iType )
{
	// we need to find a buffer which the objects data can be added into
	// this function will go through the list of all buffers and find a
	// match for the FVF

	// check D3D device is valid
	if ( !m_pD3D ) return NULL;

	// make sure we have a FVF mode
	if ( dwFVF==0 && pVertexDec==NULL ) return NULL;

	// make sure the parameters are valid
	if ( dwSize < 1 || dwVertexCount < 1 ) return NULL;

	// local variables
	bool			bMatch			= false;
	sVertexData*	pVertexData		= m_pVertexDataList;

	// get device capabilities
	D3DCAPS9 caps;
	m_pD3D->GetDeviceCaps ( &caps );

	// make sure primitive count can be achieved
	DWORD dwPrimCountMax = caps.MaxPrimitiveCount;
	if ( dwIndexCount>0 )
	{
		if ( dwIndexCount/3 > dwPrimCountMax )
			return NULL;
	}
	else
	{
		if ( dwVertexCount/3 > dwPrimCountMax )
			return NULL;
	}

	// no search if need to use its own buffer
	if ( bUsesItsOwnBuffers )
		pVertexData=NULL;

	// run through all nodes in list
	while ( pVertexData )
	{
		// check if vertex declarations match
		bool bVertDecMatch = false;
		if ( pVertexData->dwFormat==0 )
		{
			UINT numElementsThis;
			D3DVERTEXELEMENT9 VertexDecFromThisBuffer[256];
			if ( pVertexData->pVertexDec )
			{
				HRESULT hr = pVertexData->pVertexDec->GetDeclaration( VertexDecFromThisBuffer, &numElementsThis);
				UINT numElementsRequest;
				D3DVERTEXELEMENT9 VertexDecFromRequest[256];
				if ( pVertexDec )
				{
					hr = pVertexDec->GetDeclaration( VertexDecFromRequest, &numElementsRequest);
					DWORD dwCMPSize = sizeof(D3DVERTEXELEMENT9)*numElementsRequest;
					if ( numElementsRequest == numElementsThis )
						if ( memcmp ( VertexDecFromRequest, VertexDecFromThisBuffer, dwCMPSize )==0 )
							bVertDecMatch = true;
				}
			}
		}
		else
			bVertDecMatch = true;

		// see if we find a match to the FVF
		if ( pVertexData->dwFormat == dwFVF && bVertDecMatch==true )
		{
			// see if we can fit the data into the buffer
			if ( pVertexData->dwCurrentVertexCount + dwVertexCount < pVertexData->dwMaxVertexCount )
			{
				bMatch = true;
				break;
			}
		}

		// move to next node
		pVertexData = pVertexData->pNext;
	}

	// if we don't have a match then create a new VB
	if ( !bMatch )
	{
		if ( !m_pVertexDataList )
		{
			// create new list
			m_pVertexDataList		= new sVertexData;
			pVertexData				= m_pVertexDataList;
			SAFE_MEMORY ( m_pVertexDataList );
		}
		else
		{
			// find end of list
			pVertexData = m_pVertexDataList;
			while ( pVertexData )
			{
				if ( pVertexData->pNext )
				{
					pVertexData = pVertexData->pNext;
					continue;
				}
				else
					break;
			}

			// add new item to list
			pVertexData->pNext = new sVertexData ( );
			pVertexData        = pVertexData->pNext;
		}

		// create a decent start size of the VB
		bool bCreate = true;
		DWORD dwVBSize = caps.MaxVertexIndex;

		// if size exceeds 16bit, make max size 16bit (32bit index supported maybe in future though it shows no speed increase!!)
		if ( dwIndexCount > 0 )
		{
			// mesh uses index buffer so can only have a 16bit vertex buffer
			if ( dwVBSize > 0x0000FFFF ) dwVBSize = 0x0000FFFF;
		}

		// own buffer needs only to be the size of the data
		if ( bUsesItsOwnBuffers )
			dwVBSize = dwVertexCount;

		// loop until succeed in creating a VB
		while ( bCreate )
		{
			DWORD dwUsage = D3DUSAGE_WRITEONLY;

			if ( iType == D3DPT_POINTLIST )
				dwUsage = D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS;

			// attempt to create a VB
			if ( FAILED ( m_pD3D->CreateVertexBuffer ( 
														dwSize * dwVBSize,
														dwUsage,
														dwFVF,
														//D3DPOOL_DEFAULT, 250105 - FPSC-EA - large use of objects causes crash!
														//D3DPOOL_MANAGED, 270105 - perhaps managed takes too long to swapin/out
														D3DPOOL_DEFAULT,
														&pVertexData->pVB,
														NULL
												     ) ) )
			{
				// failed, try half the size
				dwVBSize /= 2;
			}
			else
			{
				// only if VB can hold required vertex data
				if ( dwVBSize >= dwVertexCount )
				{
					// success, we can use this size
					bCreate = false;
				}
				else
				{
					// VB created, but just too small!
					SAFE_RELEASE(pVertexData->pVB);
					return NULL;
				}
			}

			// if we continue until a ridiculously low value, we must fail
			if ( dwVBSize <= 0 )
				return NULL;
		}

		// save the format of the VB and the number of vertices allowed
		pVertexData->dwFormat					= dwFVF;
		pVertexData->pVertexDec					= pVertexDec;
		pVertexData->dwMaxVertexCount			= dwVBSize;
		pVertexData->dwCurrentVertexCount		= 0;
	}

	// return the final buffer
	return pVertexData;
}

bool CObjectManager::AddObjectMeshToBuffers ( sMesh* pMesh, bool bUsesItsOwnBuffers )
{
	// vertex and index buffer set up
	WORD*		 pIndices    = NULL;
	sVertexData* pVertexData = NULL;
	sIndexData*	 pIndexData  = NULL;

	// find a vertex buffer we can use which matches the FVF component
	pVertexData = this->FindVertexBuffer (	pMesh->dwFVF,
											pMesh->pVertexDec,
											pMesh->dwFVFSize,
											pMesh->dwVertexCount,						
											pMesh->dwIndexCount,
											bUsesItsOwnBuffers,
											pMesh->iPrimitiveType
										); // if no indexbuffer, can make larger vertex buffer (16bit index only)

	// check the vertex buffer is valid
	if ( pVertexData==NULL )
	{
		pMesh->pDrawBuffer = NULL;
		return false;
	}

	// find an index buffer (if one is required)
	if ( pMesh->dwIndexCount> 0 )
	{
		// find and check the index buffer is valid
		pIndexData = this->FindIndexBuffer ( pMesh->dwIndexCount, bUsesItsOwnBuffers );
		SAFE_MEMORY ( pIndexData );
	}

	// create a new vertex buffer reference array
	SAFE_DELETE ( pMesh->pDrawBuffer );
	pMesh->pDrawBuffer = new sDrawBuffer;

	// check the reference array is okay
	SAFE_MEMORY ( pMesh->pDrawBuffer );

	// draw primitive type
	pMesh->pDrawBuffer->dwPrimType			= ( D3DPRIMITIVETYPE ) pMesh->iPrimitiveType;

	// store a reference to the vertex buffer
	pMesh->pDrawBuffer->pVertexBufferRef	= pVertexData->pVB;
	pMesh->pDrawBuffer->dwVertexStart		= ( pVertexData->dwPosition * sizeof(float) ) / pMesh->dwFVFSize;
	pMesh->pDrawBuffer->dwVertexCount		= pMesh->iDrawVertexCount;

	// primitive count for drawing
	pMesh->pDrawBuffer->dwPrimitiveCount	= pMesh->iDrawPrimitives;

	// store a reference to the indice buffer
	if( pIndexData )
	{
		pMesh->pDrawBuffer->pIndexBufferRef		= pIndexData->pIB;
		pMesh->pDrawBuffer->dwIndexStart		= pIndexData->dwCurrentIndexCount;
	}
	else
	{
		pMesh->pDrawBuffer->pIndexBufferRef		= NULL;
		pMesh->pDrawBuffer->dwIndexStart		= 0;
	}

	// store a reference to the FVF size
	pMesh->pDrawBuffer->dwFVFSize			= pMesh->dwFVFSize;

	// store references to the VB and IB list items
	pMesh->pDrawBuffer->pVBListEntryRef		= (LPVOID)pVertexData;
	pMesh->pDrawBuffer->pIBListEntryRef		= (LPVOID)pIndexData;

	// copy the indices (if applicable)
	if ( pIndexData )
	{
		// update base vertex index for indices (and adjust vertex start accordingly)
		pMesh->pDrawBuffer->dwBaseVertexIndex	 = ( WORD ) pVertexData->dwCurrentVertexCount;
		pMesh->pDrawBuffer->dwVertexStart		-= ( WORD ) pMesh->pDrawBuffer->dwBaseVertexIndex;

		// copy index data to index buffer (WORD based)
		CopyMeshDataToIndexBuffer ( pMesh, pIndexData->pIB, pIndexData->dwCurrentIndexCount );

		// increment the index count
		pIndexData->dwCurrentIndexCount += pMesh->dwIndexCount;
	}

	// copy vertex data to vertex buffer
	CopyMeshDataToVertexBufferSameFVF ( pMesh, pVertexData->pVB, pVertexData->dwPosition );

	// save the current position
	pVertexData->dwPosition += pMesh->dwVertexCount * ( pMesh->dwFVFSize/sizeof(float) );

	// increment the vertex count in the buffer
	pVertexData->dwCurrentVertexCount += pMesh->dwVertexCount;

	// everything went okay
	return true;
}

bool CObjectManager::AddObjectToBuffers ( sObject* pObject )
{
	// vertex and index buffer set up
	bool			bAllOkay		= true;
	WORD*			pIndices		= NULL;
	sVertexData*	pVertexData		= NULL;
	sIndexData*		pIndexData		= NULL;

	// run through each frame within an object
	for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
	{
		// get frame ptr
		sFrame* pFrame = pObject->ppFrameList [ iFrame ];

		// add each mesh to the buffers
		if ( pFrame->pMesh )		if ( !AddObjectMeshToBuffers ( pFrame->pMesh, pObject->bUsesItsOwnBuffers ) )				bAllOkay=false;
		if ( pFrame->pShadowMesh )	if ( !AddObjectMeshToBuffers ( pFrame->pShadowMesh, pObject->bUsesItsOwnBuffers ) )			bAllOkay=false;
		if ( pFrame->pBoundBox )	if ( !AddObjectMeshToBuffers ( pFrame->pBoundBox, pObject->bUsesItsOwnBuffers ) )			bAllOkay=false;
		if ( pFrame->pBoundSphere )	if ( !AddObjectMeshToBuffers ( pFrame->pBoundSphere, pObject->bUsesItsOwnBuffers ) )		bAllOkay=false;
		if ( pFrame->pLOD[0] )		if ( !AddObjectMeshToBuffers ( pFrame->pLOD[0], pObject->bUsesItsOwnBuffers ) )				bAllOkay=false;
		if ( pFrame->pLOD[1] )		if ( !AddObjectMeshToBuffers ( pFrame->pLOD[1], pObject->bUsesItsOwnBuffers ) )				bAllOkay=false;
		if ( pFrame->pLODForQUAD )	if ( !AddObjectMeshToBuffers ( pFrame->pLODForQUAD, pObject->bUsesItsOwnBuffers ) )				bAllOkay=false;
	}

	// update texture list when introduce new object
	m_bUpdateTextureList=true;

	// everything went..
	return bAllOkay;
}


bool CObjectManager::FlagAllObjectMeshesUsingBuffer ( sVertexData* pVertexData, sIndexData* pIndexData )
{
	// flag any object mesh that uses either of these buffers
	for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
	{
		// get index from shortlist
		int iObjectID = g_ObjectListRef [ iShortList ];

		// see if we have a valid list
		sObject* pObject = g_ObjectList [ iObjectID ];
		if ( !pObject )
			continue;

		// run through each frame within an object
		for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
		{
			// get frame ptr
			sFrame* pFrame = pObject->ppFrameList [ iFrame ];

			// add this object:mesh back in
			if ( pFrame->pMesh )
			{
				if ( pFrame->pMesh->pDrawBuffer )
				{
					if(pFrame->pMesh->pDrawBuffer->pVBListEntryRef==(LPVOID)pVertexData
					|| pFrame->pMesh->pDrawBuffer->pIBListEntryRef==(LPVOID)pIndexData )
					{
						// leefix - 070403 - ensure associated meshes are also removed from buffers to prevent duplication!
						if ( pFrame->pMesh->bAddObjectToBuffers==false )
						{
							pFrame->pMesh->bAddObjectToBuffers=true;
							// mike - 301106 - add flag to stop recursion
							RemoveBuffersUsedByObjectMesh ( pFrame->pMesh, false );
						}
					}
				}
			}
			if ( pFrame->pShadowMesh )
			{
				if ( pFrame->pShadowMesh->pDrawBuffer )
				{
					if(pFrame->pShadowMesh->pDrawBuffer->pVBListEntryRef==(LPVOID)pVertexData
					|| pFrame->pShadowMesh->pDrawBuffer->pIBListEntryRef==(LPVOID)pIndexData )
					{
						// leefix - 070403 - ensure associated meshes are also removed from buffers to prevent duplication!
						if ( pFrame->pShadowMesh->bAddObjectToBuffers==false )
						{
							pFrame->pShadowMesh->bAddObjectToBuffers=true;
							// mike - 301106 - add flag to stop recursion
							RemoveBuffersUsedByObjectMesh ( pFrame->pShadowMesh, false );
						}
					}
				}
			}
			if ( pFrame->pBoundBox )
			{
				if ( pFrame->pBoundBox->pDrawBuffer )
				{
					if(pFrame->pBoundBox->pDrawBuffer->pVBListEntryRef==(LPVOID)pVertexData
					|| pFrame->pBoundBox->pDrawBuffer->pIBListEntryRef==(LPVOID)pIndexData )
					{
						// leefix - 070403 - ensure associated meshes are also removed from buffers to prevent duplication!
						if ( pFrame->pBoundBox->bAddObjectToBuffers==false )
						{
							pFrame->pBoundBox->bAddObjectToBuffers=true;
							// mike - 301106 - add flag to stop recursion
							RemoveBuffersUsedByObjectMesh ( pFrame->pBoundBox, false );
						}
					}
				}
			}	
			if ( pFrame->pBoundSphere )
			{
				if ( pFrame->pBoundSphere->pDrawBuffer )
				{
					if(pFrame->pBoundSphere->pDrawBuffer->pVBListEntryRef==(LPVOID)pVertexData
					|| pFrame->pBoundSphere->pDrawBuffer->pIBListEntryRef==(LPVOID)pIndexData )
					{
						// leefix - 070403 - ensure associated meshes are also removed from buffers to prevent duplication!
						if ( pFrame->pBoundSphere->bAddObjectToBuffers==false )
						{
							pFrame->pBoundSphere->bAddObjectToBuffers=true;
							// mike - 301106 - add flag to stop recursion
							RemoveBuffersUsedByObjectMesh ( pFrame->pBoundSphere, false );
						}
					}
				}
			}	
			for ( int l=0; l<3; l++ )
			{
				sMesh* pLODMesh = NULL;
				if ( l<2 )
					pLODMesh = pFrame->pLOD[l];
				else
					pLODMesh = pFrame->pLODForQUAD;

				if ( pLODMesh )
				{
					if ( pLODMesh->pDrawBuffer )
					{
						if(pLODMesh->pDrawBuffer->pVBListEntryRef==(LPVOID)pVertexData
						|| pLODMesh->pDrawBuffer->pIBListEntryRef==(LPVOID)pIndexData )
						{
							if ( pLODMesh->bAddObjectToBuffers==false )
							{
								pLODMesh->bAddObjectToBuffers=true;
								RemoveBuffersUsedByObjectMesh ( pLODMesh, false );
							}
						}
					}
				}	
			}
		}
	}

	// everything went okay
	return true;
}

// mike - 301106 - add flag to stop recursion
bool CObjectManager::RemoveBuffersUsedByObjectMesh ( sMesh* pMesh, bool bRecurse )
{
	// get reference to drawbuffer
	sDrawBuffer* pDrawBuffer = pMesh->pDrawBuffer;
	if(pDrawBuffer)
	{
		DWORD* pdwAdd = (DWORD*)&pMesh->pDrawBuffer;

		// get reference to VB and IB ptrs
		sVertexData* pVertexData = (sVertexData*)pDrawBuffer->pVBListEntryRef;
		sIndexData*	 pIndexData  = (sIndexData* )pDrawBuffer->pIBListEntryRef;

		// scan for and delete vertex item
		sVertexData* pLastVertexData = NULL;
		sVertexData* pFindVertexData = m_pVertexDataList;
		while ( pFindVertexData )
		{
			// check this item
			sVertexData* pNextVertexData = pFindVertexData->pNext;
			if ( pFindVertexData==pVertexData )
			{
				// sever and delete vertex item
				pFindVertexData->pNext=NULL;

				// free VB
				SAFE_RELEASE( pFindVertexData->pVB );

				// delete vertexdata
				SAFE_DELETE( pFindVertexData );

				// adjust next value to leap deleted item
				if ( pLastVertexData ) pLastVertexData->pNext = pNextVertexData;

				// new start item to replace deleted one
				if ( m_pVertexDataList==pVertexData )
					m_pVertexDataList = pNextVertexData;

				// mike - 281106 - pVertexData points to pFindVertexData, we have just
				//				 - removed pFindVertexData but pVertexData still points
				//				 - to it, later on we call FlagAllObjectMeshesUsingBuffer passing
				//				 - in pVertexData but it's a junk value
				
				
				// done here
				break;
			}

			// next item
			pLastVertexData = pFindVertexData;
			pFindVertexData = pNextVertexData;
		}

		// scan for and delete index item
		sIndexData* pLastIndexData = NULL;
		sIndexData* pFindIndexData = m_pIndexDataList;
		while ( pFindIndexData )
		{
			// check this item
			sIndexData* pNextIndexData = pFindIndexData->pNext;
			if ( pFindIndexData==pIndexData )
			{
				// sever and delete Index item
				pFindIndexData->pNext=NULL;

				// release IB
				SAFE_RELEASE ( pFindIndexData->pIB );

				// delete indexdata
				SAFE_DELETE( pFindIndexData );

				// adjust next value to leap deleted item
				if ( pLastIndexData ) pLastIndexData->pNext = pNextIndexData;

				// new start item to replace deleted one
				if ( m_pIndexDataList==pIndexData )
					m_pIndexDataList = pNextIndexData;

				// done here
				break;
			}

			// next item
			pLastIndexData = pFindIndexData;
			pFindIndexData = pNextIndexData;
		}

		// flag any objects that used either of these buffers
		// mike - 301106 - add flag to stop recursion
		if ( bRecurse == true )
		{
			// useful when we KNOW that the VB IB buffers are not shared
			FlagAllObjectMeshesUsingBuffer ( pVertexData, pIndexData );
		}
	}

	// everything went okay
	return true;
}

// lee - 140307 - added to delete buffers quickly if we know VB/IBs are not shared
bool CObjectManager::RemoveBuffersUsedByObjectMeshDirectly ( sMesh* pMesh )
{
	// delete the buffers from the lit, and release the ptrs, but do NOT assume buffers are shared!
	return RemoveBuffersUsedByObjectMesh ( pMesh, false );
}

bool CObjectManager::RemoveBuffersUsedByObject ( sObject* pObject )
{
	// run through each frame within an object
	for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
	{
		// get frame ptr
		sFrame* pFrame = pObject->ppFrameList [ iFrame ];

		// lee - 140307 - if object 'uses its own buffers' there is no need to use
		// a recursive check for whether the VB/IB buffers are shared, as they cannot be
		// so we can simply delete the buffers with out recursive (buffer remove/flag back in)
		if ( pObject->bUsesItsOwnBuffers==true )
		{
			// delete all vertex/index entries 'directly' used by this mesh (no recursive)
			if(pFrame->pMesh) RemoveBuffersUsedByObjectMeshDirectly ( pFrame->pMesh );
			if(pFrame->pShadowMesh) RemoveBuffersUsedByObjectMeshDirectly ( pFrame->pShadowMesh );
			if(pFrame->pBoundBox) RemoveBuffersUsedByObjectMeshDirectly ( pFrame->pBoundBox );
			if(pFrame->pBoundSphere) RemoveBuffersUsedByObjectMeshDirectly ( pFrame->pBoundSphere );
			if(pFrame->pLOD[0]) RemoveBuffersUsedByObjectMeshDirectly ( pFrame->pLOD[0] );
			if(pFrame->pLOD[1]) RemoveBuffersUsedByObjectMeshDirectly ( pFrame->pLOD[1] );
			if(pFrame->pLODForQUAD) RemoveBuffersUsedByObjectMeshDirectly ( pFrame->pLODForQUAD );
		}
		else
		{
			// delete all vertex/index entries used by this mesh
			if(pFrame->pMesh) RemoveBuffersUsedByObjectMesh ( pFrame->pMesh );
			if(pFrame->pShadowMesh) RemoveBuffersUsedByObjectMesh ( pFrame->pShadowMesh );
			if(pFrame->pBoundBox) RemoveBuffersUsedByObjectMesh ( pFrame->pBoundBox );
			if(pFrame->pBoundSphere) RemoveBuffersUsedByObjectMesh ( pFrame->pBoundSphere );
			if(pFrame->pLOD[0]) RemoveBuffersUsedByObjectMesh ( pFrame->pLOD[0] );
			if(pFrame->pLOD[1]) RemoveBuffersUsedByObjectMesh ( pFrame->pLOD[1] );
			if(pFrame->pLODForQUAD) RemoveBuffersUsedByObjectMesh ( pFrame->pLODForQUAD );
		}
	}

	// everything went okay
	return true;
}

bool CObjectManager::AddFlaggedObjectsBackToBuffers ( void )
{
	// upon buffer removal, some object where flagged for re-creation
	for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
	{
		// get index from shortlist
		int iObjectID = g_ObjectListRef [ iShortList ];

		// see if we have a valid list
		sObject* pObject = g_ObjectList [ iObjectID ];
		if ( !pObject )
			continue;

		// 210214 - completely ignore excluded objects
		if ( pObject->bExcluded ) continue;

		// also ensure we skip the removed object (not to be re-added)
		if ( pObject->bReplaceObjectFromBuffers==true )
			continue;

		// run through each frame within an object
		for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
		{
			// refresh the VB data for this mesh (it will auto-lock the VB)
			sFrame* pFrame = pObject->ppFrameList [ iFrame ];

			// add this object:mesh back in
			if ( pFrame->pMesh )
			{
				if ( pFrame->pMesh->bAddObjectToBuffers==true )
				{
					AddObjectMeshToBuffers ( pFrame->pMesh, pObject->bUsesItsOwnBuffers );
					pFrame->pMesh->bAddObjectToBuffers=false;
				}
			}

			// add this object:shadowmesh back in
			if ( pFrame->pShadowMesh )
			{
				if ( pFrame->pShadowMesh->bAddObjectToBuffers==true )
				{
					AddObjectMeshToBuffers ( pFrame->pShadowMesh, pObject->bUsesItsOwnBuffers );
					pFrame->pShadowMesh->bAddObjectToBuffers=false;
				}
			}

			// add this object:boundbox back in
			if ( pFrame->pBoundBox )
			{
				if ( pFrame->pBoundBox->bAddObjectToBuffers==true )
				{
					AddObjectMeshToBuffers ( pFrame->pBoundBox, pObject->bUsesItsOwnBuffers );
					pFrame->pBoundBox->bAddObjectToBuffers=false;
				}
			}	

			// add this object:boundsphere back in
			if ( pFrame->pBoundSphere )
			{
				if ( pFrame->pBoundSphere->bAddObjectToBuffers==true )
				{
					AddObjectMeshToBuffers ( pFrame->pBoundSphere, pObject->bUsesItsOwnBuffers );
					pFrame->pBoundSphere->bAddObjectToBuffers=false;
				}
			}	

			// add this object:mesh back in
			for ( int l=0; l<3; l++ )
			{
				sMesh* pLODMesh = NULL;
				if ( l<2 )
					pLODMesh = pFrame->pLOD[l];
				else
					pLODMesh = pFrame->pLODForQUAD;

				if ( pLODMesh )
				{
					if ( pLODMesh->bAddObjectToBuffers==true )
					{
						AddObjectMeshToBuffers ( pLODMesh, pObject->bUsesItsOwnBuffers );
						pLODMesh->bAddObjectToBuffers=false;
					}
				}
			}
		}
	}

	// everything went okay
	return true;
}

bool CObjectManager::RemoveObjectFromBuffers ( sObject* pRemovedObject )
{
	// delete all buffers that this object resided in
	RemoveBuffersUsedByObject ( pRemovedObject );

	// upon buffer removal, some object where flagged for re-creation
	AddFlaggedObjectsBackToBuffers ();

	// update texture list when introduce new object(s)
	UpdateTextures();

	// everything went okay
	return true;
}

bool CObjectManager::ReplaceAllFlaggedObjectsInBuffers ( void )
{
	// only if global flag switched
	if ( g_bObjectReplacedUpdateBuffers )
	{
		// delete all buffers that these object resided in
		for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
		{
			// get index from shortlist
			int iObjectID = g_ObjectListRef [ iShortList ];
			sObject* pRemovedObject = g_ObjectList [ iObjectID ];
			if ( pRemovedObject )
			{
				// 210214 - completely ignore excluded objects
				if ( pRemovedObject->bExcluded ) continue;

				if ( pRemovedObject->bReplaceObjectFromBuffers )
					RemoveBuffersUsedByObject ( pRemovedObject );
			}
		}

		// upon buffer removal, some object where flagged for re-creation
		AddFlaggedObjectsBackToBuffers ();

		// when all buffers clear of removed objects, can add new instances of them back in..
		for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
		{
			// get index from shortlist
			int iObjectID = g_ObjectListRef [ iShortList ];
			sObject* pObject = g_ObjectList [ iObjectID ];
			if ( pObject )
			{
				// 210214 - completely ignore excluded objects
				if ( pObject->bExcluded ) continue;

				if ( pObject->bReplaceObjectFromBuffers )
				{
					// add object back in
					AddObjectToBuffers ( pObject );

					// and clear flag 
					pObject->bReplaceObjectFromBuffers = false;		
				}
			}
		}

		// update texture list when introduce new object(s)
		UpdateTextures();

		// reset global flag
		g_bObjectReplacedUpdateBuffers = false;
	}

	// everything went okay
	return true;
}

bool CObjectManager::UpdateObjectMeshInBuffer ( sMesh* pMesh )
{
	// only if have a drawbuffer
	if ( !pMesh->pDrawBuffer )
		return false;

	// if drawbuffer is insufficient, i.e. not big enough
	if ( pMesh->pDrawBuffer->dwVertexCount < pMesh->dwVertexCount 
	||	 pMesh->pDrawBuffer->dwFVFSize != pMesh->dwFVFSize )	
	{
		// recreate drawbuffer
		RemoveBuffersUsedByObjectMesh ( pMesh, true );
		AddObjectMeshToBuffers ( pMesh, true );
		return true;
	}

	// get the offset map
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// get vertex list item pointer
	sVertexData* pVertexData = (sVertexData*)pMesh->pDrawBuffer->pVBListEntryRef;
	IDirect3DVertexBuffer9* pVertexBuffer = pVertexData->pVB;

	// lock the vertex buffer (if not already locked)
	if ( pVertexData->bBufferLocked==false )
	{
		pVertexData->pfLockedData = NULL;
		if ( FAILED ( pVertexBuffer->Lock ( 0, 0, ( VOID** ) &pVertexData->pfLockedData, 0 ) ) )
			return false;

		// set the VB flag when locked
		pVertexData->bBufferLocked = true;
	}

	// copy vertex-data-block from object to VB
	DWORD dwPosWithinVB = (pMesh->pDrawBuffer->dwBaseVertexIndex + pMesh->pDrawBuffer->dwVertexStart) * pMesh->dwFVFSize;
	LPVOID pDestPtr = pVertexData->pfLockedData + dwPosWithinVB;
	LPVOID pSourceData = pMesh->pVertexData;
	DWORD dwSizeToCopy = pMesh->dwVertexCount * pMesh->dwFVFSize;
	memcpy ( pDestPtr, pSourceData, dwSizeToCopy );

	// draw quantity can change without having to recreate (like for shadows)
	if ( pMesh->pDrawBuffer )
	{
		pMesh->pDrawBuffer->dwVertexCount		= pMesh->iDrawVertexCount;
		pMesh->pDrawBuffer->dwPrimitiveCount	= pMesh->iDrawPrimitives;
	}

	// leeadd - 230304 - physics changes INDEX DATA TOO,
	// which a VB update above does not do..so add this
	if ( pMesh->pDrawBuffer )
	{
		// only if index buffer exists
		if ( pMesh->pDrawBuffer->pIndexBufferRef )
			CopyMeshDataToIndexBuffer ( pMesh, pMesh->pDrawBuffer->pIndexBufferRef, pMesh->pDrawBuffer->dwIndexStart );
	}

	// everything went okay
	return true;
}

bool CObjectManager::UpdateAllObjectsInBuffers ( void )
{
	// objects that have changed are flagged, and passed to VB updater..

	// lee - 300914 - now uses a short list added to when meshes require refreshing (much faster)
	if ( !g_vRefreshMeshList.empty() )
    {
        for ( DWORD iIndex = 0; iIndex < g_vRefreshMeshList.size(); ++iIndex )
        {
			// get mesh to refresh
            sMesh* pMesh = g_vRefreshMeshList [ iIndex ];
			if ( !pMesh ) continue;

			// only refresh if not already done so (can have multiple entries in this list)
			if ( pMesh->bVBRefreshRequired==true )
			{
				UpdateObjectMeshInBuffer ( pMesh );
				pMesh->bVBRefreshRequired=false;
			}
		}

		// go through all vertex buffer items (unlock any that have been locked)
		CompleteUpdateInBuffers();

		// clear refresh list for next cycle (must quicker than going through ALL objects each cycle)
		g_vRefreshMeshList.clear();
	}

	/* old method of scanning all objects each cycle
	// run through all objects
	for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
	{
		// get index from shortlist
		int iObjectID = g_ObjectListRef [ iShortList ];

		// see if we have a valid list
		sObject* pObject = g_ObjectList [ iObjectID ];
		if ( !pObject )
			continue;

		//Dave - skip non visible
		if ( !pObject->bVisible )
			continue;

		// run through each frame within an object
		for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
		{
			// refresh the VB data for this mesh (it will auto-lock the VB)
			sFrame* pFrame = pObject->ppFrameList [ iFrame ];

			// 301007 - new limb excluder
			if ( pFrame==NULL ) continue;
			if ( pFrame->bExcluded==true ) continue;

			// Update regular mesh (if required)
			sMesh* pMesh = pFrame->pMesh;
			if ( pMesh )
			{
				if ( pMesh->bVBRefreshRequired==true )
				{
					UpdateObjectMeshInBuffer ( pMesh );
					pMesh->bVBRefreshRequired=false;
				}
			}

			// Update shadow mesh (if required)
			sMesh* pShadowMesh = pFrame->pShadowMesh;
			if ( pShadowMesh )
			{
				if ( pShadowMesh->bVBRefreshRequired==true )
				{
					UpdateObjectMeshInBuffer ( pShadowMesh );
					pShadowMesh->bVBRefreshRequired=false;
				}
			}

			// Update bound box meshes (if required)
			sMesh* pBoundBox = pFrame->pBoundBox;
			if ( pBoundBox )
			{
				if ( pBoundBox->bVBRefreshRequired==true )
				{
					UpdateObjectMeshInBuffer ( pBoundBox );
					pBoundBox->bVBRefreshRequired=false;
				}
			}				

			// Update bound sphere meshes (if required)
			sMesh* pBoundSphere = pFrame->pBoundSphere;
			if ( pBoundSphere )
			{
				if ( pBoundSphere->bVBRefreshRequired==true )
				{
					UpdateObjectMeshInBuffer ( pBoundSphere );
					pBoundSphere->bVBRefreshRequired=false;
				}
			}

			// go through built-in object LOD and update VB if triggered
			for ( int l=0; l<3; l++ )
			{
				sMesh* pLOD = NULL;
				if ( l<2 )
					pLOD = pFrame->pLOD[l];
				else
					pLOD = pFrame->pLODForQUAD;

				if ( pLOD )
				{
					if ( pLOD->bVBRefreshRequired==true )
					{
						UpdateObjectMeshInBuffer ( pLOD );
						pLOD->bVBRefreshRequired=false;
					}
				}
			}

		}
	}

	// go through all vertex buffer items (unlock any that have been locked)
	CompleteUpdateInBuffers();
	*/

	// okay
	return true;
}

bool CObjectManager::CompleteUpdateInBuffers ( void )
{
	// go through all vertex buffer items (unlock any that have been locked)
	//sVertexData* pLastOnePeekRef = NULL;
	sVertexData* pVertexData = m_pVertexDataList;
	while ( pVertexData )
	{
		// if buffer has been locked
		if ( pVertexData->bBufferLocked==true )
		{
			// unlock and restore flag
			pVertexData->pVB->Unlock ( );
			pVertexData->bBufferLocked=false;
		}

		// move to next node
		//pLastOnePeekRef = pVertexData;
		pVertexData = pVertexData->pNext;
	}

	// okay
	return true;
}

bool CObjectManager::QuicklyUpdateObjectMeshInBuffer ( sMesh* pMesh, DWORD dwVertexFrom, DWORD dwVertexTo )
{
	// only if have a drawbuffer
	if ( !pMesh->pDrawBuffer )
		return false;

	// get the offset map
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// get vertex list item pointer
	sVertexData* pVertexData = (sVertexData*)pMesh->pDrawBuffer->pVBListEntryRef;
	IDirect3DVertexBuffer9* pVertexBuffer = pVertexData->pVB;

	// lock the vertex buffer (if not already locked)
	if ( pVertexData->bBufferLocked==false )
	{
		pVertexData->pfLockedData = NULL;
		if ( FAILED ( pVertexBuffer->Lock ( 0, 0, ( VOID** ) &pVertexData->pfLockedData, 0 ) ) )
			return false;

		// set the VB flag when locked
		pVertexData->bBufferLocked = true;
	}

	// copy only vertex data changed as described in params passed in
	DWORD dwPosWithinVB = (pMesh->pDrawBuffer->dwBaseVertexIndex + pMesh->pDrawBuffer->dwVertexStart + dwVertexFrom) * pMesh->dwFVFSize;
	LPVOID pDestPtr = pVertexData->pfLockedData + dwPosWithinVB;
	DWORD dwPosWithinMesh = dwVertexFrom * pMesh->dwFVFSize;
	LPVOID pSourceData = pMesh->pVertexData + dwPosWithinMesh;
	DWORD dwSizeToCopy = (dwVertexTo-dwVertexFrom) * pMesh->dwFVFSize;
	memcpy ( pDestPtr, pSourceData, dwSizeToCopy );

	// draw quantity can change without having to recreate (like for shadows)
	pMesh->pDrawBuffer->dwVertexCount		= pMesh->iDrawVertexCount;
	pMesh->pDrawBuffer->dwPrimitiveCount	= pMesh->iDrawPrimitives;

	// everything went okay
	return true;
}

bool CObjectManager::RenewReplacedMeshes ( sObject* pObject )
{
	// run through each mesh within an object
	bool bReplaceObjectOwningMesh=false;
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		// replace any meshes in VB/IB that have been changed
		sMesh* pMesh = pObject->ppMeshList [ iMesh ];
		if ( pMesh->bMeshHasBeenReplaced==true )
		{
			pMesh->bMeshHasBeenReplaced=false;
			bReplaceObjectOwningMesh=true;
			break;
		}
	}

	// replace object
	if ( bReplaceObjectOwningMesh )
	{
		pObject->bReplaceObjectFromBuffers = true;
		g_bObjectReplacedUpdateBuffers = true;
		bReplaceObjectOwningMesh = false;
	}

	// okay
	return true;
}

bool CObjectManager::RefreshObjectInBuffer ( sObject* pObject )
{
	pObject->bReplaceObjectFromBuffers = true;
	g_bObjectReplacedUpdateBuffers = true;
	return true;
}

void CObjectManager::RemoveTextureRefFromAllObjects ( LPDIRECT3DTEXTURE9 pTextureRef )
{
	// run through all objects
	for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
	{
		// get index from shortlist
		int iObjectID = g_ObjectListRef [ iShortList ];

		// see if we have a valid list
		sObject* pObject = g_ObjectList [ iObjectID ];
		if ( !pObject )
			continue;

		// run through each frame within an object
		for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
		{
			// get frame
			sFrame* pFrame = pObject->ppFrameList [ iFrame ];
			sMesh* pMesh = pFrame->pMesh;
			if ( pMesh )
			{
				// go through all textures in mesh
				RemoveTextureRefFromMesh ( pMesh, pTextureRef );
			}
		}
	}
}

//
// SORTING
//

bool CObjectManager::SortTextureList ( void )
{
    // If the object list size hasn't changed and no textures have changed, then nothing to do here.
    if ( m_iLastCount == g_iObjectListRefCount && m_bUpdateTextureList==false )
        return true;

    // Reset ready for next time
    m_iLastCount         = g_iObjectListRefCount;
    m_bUpdateTextureList = false;

    // make sure the lists we're using are valid
    SAFE_MEMORY ( g_ppSortedObjectList );
    SAFE_MEMORY ( m_ppSortedObjectVisibleList );

    // reset all data so we can build the list from scratch
    g_iSortedObjectCount = 0;
    g_bRenderVeryEarlyObjects = false;

	// at same time, collect object lists that DO NOT change from cycle to cycle
	//m_vXXXXXObjectList.clear();
	g_vAnimatableObjectList.clear();

    // run through all known items and put them into render list ready for sorting
    for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
    {
	    // Get an object id from shortlist
	    int iObjectID = g_ObjectListRef [ iShortList ];

	    // Actual object or instance of object
	    sObject* pOriginalObject = g_ObjectList [ iObjectID ];
        if ( ! pOriginalObject )
            continue;

		// 210214 - quick reject objects which have a sync mask of ZERO
		if ( pOriginalObject->dwCameraMaskBits==0 )
			continue;

		// 210214 - quick reject objects which are excluded
		if ( pOriginalObject->bExcluded )
			continue;

		// lee - 300914 - if this object holds some animation data, add to animatable list (done once per texture sort for speed!)
		if ( pOriginalObject->pAnimationSet ) 
		{
			g_vAnimatableObjectList.push_back ( pOriginalObject );
		}

		// get the object we would render
        sObject* pRenderObject = pOriginalObject;
        if ( pRenderObject->pInstanceOfObject )
		    pRenderObject = pRenderObject->pInstanceOfObject;

	    // See if we have enough information to render this object
        // A (possibly instanced) object with a mesh list and with mesh 0 having a texture.
        if ( pRenderObject && pRenderObject->ppMeshList && pRenderObject->ppMeshList[0]->pTextures)
	    {
            // Add the original object into the render list
            g_ppSortedObjectList [ g_iSortedObjectCount++ ] = pOriginalObject;

            // If we are sorting by distance, calculate the distance ready for sorting
            if ( g_eGlobalSortOrder == E_SORT_BY_DEPTH )
		    {
			    if ( pOriginalObject->bVeryEarlyObject == true )
			    {
                    // very early objects are placed at extreme distance
				    pOriginalObject->position.fCamDistance = 9999999.9f;
			    }
			    else
			    {
				    pOriginalObject->position.fCamDistance = CalculateObjectDistanceFromCamera ( pOriginalObject );
			    }
            }
            
            // Check to see if there is an early draw object
            if ( pOriginalObject->bVeryEarlyObject == true )
		    {
			    // If this object is an early draw, set global flag to show we have one in the scene
                g_bRenderVeryEarlyObjects = true;
            }
	    }
    }

    // Now sort the list appropriately
    switch(g_eGlobalSortOrder)
    {
    case E_SORT_BY_TEXTURE:
        std::sort(g_ppSortedObjectList, g_ppSortedObjectList + g_iSortedObjectCount, OrderByTexture() );
        break;
    case E_SORT_BY_OBJECT:
        std::sort(g_ppSortedObjectList, g_ppSortedObjectList + g_iSortedObjectCount, OrderByObject() );
        break;
    case E_SORT_BY_DEPTH:
        // Delay sort until the visibility list is generated
        // This increases the accuracy of the sorting for depth.
        break;
    case E_SORT_BY_NONE:
        // No sort needed
        break;
    default:
        break;
	}

	// return back to caller
	return true;
}

bool CObjectManager::SortVisibilityList ( void )
{
	// run through the sorted texture list and find out which objects
	// are visible, then create a new list which stores the visible objects

	// can quickly skip this step if the camera does not need to use latest visibilities (cheap shadow quad camera 5)
	if ( ((1<<g_pGlob->dwRenderCameraID) & m_dwSkipVisibilityListMask)!=0 )
		return true;

	// make sure we have a valid object
	if ( g_ppSortedObjectList==NULL )
		return true;

	// reset the number of visible objects to 0
	m_iVisibleObjectCount = 0;
    m_vVisibleObjectList.clear();
    m_vVisibleObjectEarly.clear();
    m_vVisibleObjectTransparent.clear();
    m_vVisibleObjectNoZDepth.clear();
    m_vVisibleObjectStandard.clear();

    // when the textures have been sorted we have a counter which stores
	// the number of sorted objects, this is m_iSortedObjectCount, we now
	// run through the sorted texture list and find which objects are visible
	for ( int iSort = 0; iSort < g_iSortedObjectCount; iSort++ )
	{
		// get a pointer to the object from the sorted draw list
		sObject* pObject = g_ppSortedObjectList [ iSort ];
		if ( !pObject ) continue;

		//Dave - if the object is not visible, and not a parent that is animating, continue
		if ( !pObject->bVisible && (!pObject->position.bParentOfInstance && !pObject->bAnimPlaying) )
		   continue;

		// VISIBILITY CULLING CHECK PROCESS
		bool bIsVisible=false;

		// 20120307 IRM
		// If the object is a parent to an instance and is animating, then always
		// count it as visible to ensure that any instance animations continue.
		// This is true even if the object is off-screen, hidden or even excluded.
		if (pObject->position.bParentOfInstance && pObject->bAnimPlaying)
		{
			bIsVisible = true;
		}
		else
		{
			// actual object or instance of object
			sObject* pActualObject = pObject;
			if ( pActualObject->pInstanceOfObject )
				pActualObject=pActualObject->pInstanceOfObject;

			// locked objects are always visible
			// glued objects are always visible (deferred to parent visibility)
			int iGluedToObj = pObject->position.iGluedToObj;
			if ( pObject->bLockedObject || iGluedToObj!=0 )
			{
				// leefix -040803- maintenance check, if glued to object that has been deleted, deal with it
				if ( iGluedToObj!=0 )
				{
					sObject* pParentObject = g_ObjectList [ iGluedToObj ];
					if ( pParentObject==NULL )
					{
						// wipe out glue assignment
						pObject->position.bGlued		= false;
						pObject->position.iGluedToObj	= 0;
						pObject->position.iGluedToMesh	= 0;
					}
				}

				// locked objects and glued are visible
				bIsVisible=true;
			}
			else
			{
				// send the position of the object and it's radius to the "CheckSphere" function, if this returns true the object will be visible
				//float fScaledRadius = pActualObject->collision.fScaledLargestRadius; // 211114 - instances scaled need to be considered
				float fScaledRadius = pObject->collision.fScaledLargestRadius;
				if ( fScaledRadius<=0.0f )
				{
					// objects with no mesh scope are visible
					bIsVisible=true;
				}
				else
				{
					// ensure have latest object center
					UpdateColCenter ( pObject );

					// get center of the object
					D3DXVECTOR3 vecRealCenter = pObject->position.vecPosition + pObject->collision.vecColCenter;

					// leeadd - 100805 - add in offset from first frame (limb zero), as this moves whole object render)
					if ( pActualObject->ppFrameList )
					{
						sFrame* pRootFrame = pActualObject->ppFrameList [ 0 ];
						if ( pRootFrame )
						{
							// leeadd - 211008 - u71 - added flag to NOT shift object bounds by frame zero matrix
							if ( (pRootFrame->dwStatusBits && 1)==0 ) 
							{
								// offset center to account for movement of the object by limb zero (root frame)
								vecRealCenter.x += pRootFrame->matUserMatrix._41;
								vecRealCenter.y += pRootFrame->matUserMatrix._42;
								vecRealCenter.z += pRootFrame->matUserMatrix._43;
							}
						}
					}

					// to avoid ugly clipping issues, double radius for objects that are anim-shifted
					float fFinalRadiusForVisCull = fScaledRadius * 2.0f;

					// objects within frustrum are visible
					if ( CheckSphere ( vecRealCenter.x, vecRealCenter.y, vecRealCenter.z, fFinalRadiusForVisCull ) )
						bIsVisible=true;

					// lerfix - 221008 - u71 - if reflection shading in effect, frustrum cull is not required
					if ( g_pGlob->dwStencilMode==2 )
						bIsVisible=true;
				}
			}
		}

		// determine visiblity

		// MIKE - 021203 - added in second part of if statement for external objects, physics DLL
		if ( bIsVisible || pObject->bDisableTransform == true )
		{
			// save a pointer to the object and place it in the new drawlist
			m_ppSortedObjectVisibleList [ m_iVisibleObjectCount++ ] = pObject;

            // Build individual draw lists for each layer
            if (pObject->bVeryEarlyObject == true)
            {
                m_vVisibleObjectEarly.push_back( pObject );
            }
            else if ( pObject->bNewZLayerObject || pObject->bLockedObject )
            {
				m_vVisibleObjectNoZDepth.push_back( pObject );
            }
            else if ( pObject->bGhostedObject || pObject->bTransparentObject )
            {
                m_vVisibleObjectTransparent.push_back( pObject );
            }
            else
            {
                m_vVisibleObjectStandard.push_back( pObject );
            }

            // u74b8 - If sort order is by distance, update the object distance
            if (g_eGlobalSortOrder == E_SORT_BY_DEPTH)
            {
			    if ( pObject->bVeryEarlyObject == true )
			    {
                    // very early objects are placed at extreme distance
				    pObject->position.fCamDistance = 9999999.9f;
			    }
			    else
			    {
				    pObject->position.fCamDistance = CalculateObjectDistanceFromCamera ( pObject );
			    }
            }
		}
	}

    // u74b8 - If sort order is by distance, sort the list into the correct order
    //         as it varies by camera.
    if (g_eGlobalSortOrder == E_SORT_BY_DEPTH)
    {
        // No ghost/transparent sort just yet - still need to take into account water -
        //but do need to sort everything else.
        std::sort( m_vVisibleObjectEarly.begin(), m_vVisibleObjectEarly.end(), OrderByReverseCameraDistance() );
        std::sort( m_vVisibleObjectNoZDepth.begin(), m_vVisibleObjectNoZDepth.end(), OrderByReverseCameraDistance() );
        std::sort( m_vVisibleObjectStandard.begin(), m_vVisibleObjectStandard.end(), OrderByReverseCameraDistance() );
    }

	// all went okay
	return true;
}

bool CObjectManager::UpdateTextures ( void )
{
	// clear tep list immediately as now invalid
	g_vAnimatableObjectList.clear();

	// triggers texture list update
	m_bUpdateTextureList=true;
	return true;
}

void CObjectManager::UpdateAnimationCyclePerObject ( sObject* pObject )
{
	// moved to calling functions (calls are CPU expensive)
	// if object is excluded, skip rest - no engine interaction!
	//if ( pObject->bExcluded==true )
	//	return;

	// simply control animation frame
	if ( pObject->bAnimPlaying )
	{
		// advance frame
		pObject->fAnimFrame += pObject->fAnimSpeed;

		// if reach end
		if ( pObject->fAnimFrame >= pObject->fAnimFrameEnd )
		{
			// if animation loops
			if ( pObject->bAnimLooping==false )
			{
				// U76 - 300710 - ensure we clip any over-run so we're dead on the final frame
				pObject->fAnimFrame = pObject->fAnimFrameEnd;

				// stop playing if reach end frame
				pObject->bAnimPlaying = false;
			}
			else
			{
				// leefix - 190303 - beta 4.7 - so play anim stays on last frame (is this DBV1 friendly?)
				pObject->fAnimFrame = pObject->fAnimLoopStart;
			}
		}

		// leeadd - 300605 - support looping frames backwards - speed can be minus!
		if ( pObject->fAnimSpeed<0 && pObject->fAnimFrame < pObject->fAnimLoopStart )
		{
			if ( pObject->bAnimLooping==false )
				pObject->bAnimPlaying = false;
			else
				pObject->fAnimFrame = pObject->fAnimFrameEnd;
		}
	}
	else
	{
		// control manual slerp
		if ( pObject->bAnimManualSlerp )
		{
			pObject->fAnimSlerpTime += pObject->fAnimInterp;
			if ( pObject->fAnimSlerpTime >= 1.0f )
			{
				pObject->bAnimManualSlerp = false;
				pObject->fAnimFrame = pObject->fAnimSlerpEndFrame;
			}
		}
	}

	// leeadd - 080305 - copy animation bound boxes to collision boundbox
	// leefix - 310305 - ONLY if not using the fixed box check from (make object collision box)
	// lee - 140306 - u60b3 - added bUseBoxCollision as this was not accounted with first flag
	// lee - 160415 - need this for intersectall bounds of animating objects if ( pObject->collision.bFixedBoxCheck==false && pObject->collision.bUseBoxCollision==false )
	if ( pObject->collision.bUseBoxCollision==false )
	{
		if ( pObject->pAnimationSet )
		{
			if ( pObject->pAnimationSet->pvecBoundMin )
			{
				int iThisKeyFrame = (int)pObject->fAnimFrame;
				if ( iThisKeyFrame > (int)pObject->pAnimationSet->ulLength ) iThisKeyFrame = pObject->pAnimationSet->ulLength-1;
				pObject->collision.vecMin = pObject->pAnimationSet->pvecBoundMin [ iThisKeyFrame ];
				pObject->collision.vecMax = pObject->pAnimationSet->pvecBoundMax [ iThisKeyFrame ];
				pObject->collision.vecCentre = pObject->pAnimationSet->pvecBoundCenter [ iThisKeyFrame ];
				pObject->collision.fRadius = pObject->pAnimationSet->pfBoundRadius [ iThisKeyFrame ];
			}
		}
		pObject->ppMeshList [ 0 ]->Collision.vecMin = pObject->collision.vecMin;
		pObject->ppMeshList [ 0 ]->Collision.vecMax = pObject->collision.vecMax;
		pObject->ppMeshList [ 0 ]->Collision.vecCentre = pObject->collision.vecCentre;
		pObject->ppMeshList [ 0 ]->Collision.fRadius = pObject->collision.fRadius;
	}
}

bool CObjectManager::UpdateAnimationCycle ( void )
{
	// lee - 300914 - new way only runs through object list of known objects with animations
	if ( !g_vAnimatableObjectList.empty() )
    {
        for ( DWORD iIndex = 0; iIndex < g_vAnimatableObjectList.size(); ++iIndex )
        {
			// get mesh to refresh
            sObject* pObject = g_vAnimatableObjectList [ iIndex ];
			if ( !pObject ) continue;

			// if not visible and not an animating parent, skip
			if ( !pObject->bVisible && (!pObject->position.bParentOfInstance && !pObject->bAnimPlaying) )
				continue;

			// call per object update function
			UpdateAnimationCyclePerObject ( pObject );
		}
	}

	/* old way runs through ALL objects
	// run through all objects (animate code)
	for ( int iShortList = 0; iShortList < g_iObjectListRefCount; iShortList++ )
	{
		// get index from shortlist
		int iObjectID = g_ObjectListRef [ iShortList ];

		// see if we have a valid list
		sObject* pObject = g_ObjectList [ iObjectID ];
		if ( !pObject )
			continue;

		//Dave
		if ( !pObject->bVisible && (!pObject->position.bParentOfInstance && !pObject->bAnimPlaying) )
			continue;

		// more early outs
		if ( pObject->bExcluded==true ) continue;
		if ( pObject->pAnimationSet==NULL ) continue;

		// call per object update function
		UpdateAnimationCyclePerObject ( pObject );
	}
	*/

	// okay
	return true;
}

bool CObjectManager::UpdateOnlyVisible ( void )
{
	// run through all objects
	// lee - 0300914 - took 34% with nothing much to animate, reduceloop size!
	//for ( int iObjectID = 0; iObjectID < m_iVisibleObjectCount; iObjectID++ )
	//{
	//	// see if we have a valid list
	//	sObject* pObject = m_ppSortedObjectVisibleList [ iObjectID ];
	//	if ( !pObject ) continue;
	//	//Dave
	//	if ( !pObject->bAnimPlaying && !pObject->bAnimUpdateOnce )
	//		continue;

	// lee - 300914 - this may MISS some objects such as manually limb adjusted objects down the road!
	if ( !g_vAnimatableObjectList.empty() )
    {
        for ( DWORD iIndex = 0; iIndex < g_vAnimatableObjectList.size(); ++iIndex )
        {
			// get mesh to refresh
            sObject* pObject = g_vAnimatableObjectList [ iIndex ];
			if ( !pObject ) continue;

			// allow parents of instances
			if ( pObject->position.bParentOfInstance==false )
			{
				// only need to calc matrix data and update anim data if VISIBLE!
				if ( pObject->bVisible==false || pObject->bUniverseVisible==false )
					continue;
			}

			// moved from DBOFormat (needs to be done before a VB update)
			// calculate all frame/slerp/animation data
			UpdateAllFrameData ( pObject, pObject->fAnimFrame );

			// reset hierarchy and calculate combined frame matrix data
			D3DXMATRIX matrix;
			D3DXMatrixIdentity ( &matrix );

			// special 'bone matrix' override (ragdoll systems, etc)
			if ( pObject->position.bCustomBoneMatrix==false ) UpdateFrame ( pObject->pFrame, &matrix );

			// moved this code to DBOFormat.cpp - handle vertex level animation (even if not animating)
			// instances that use animating objects must animate them indirectly
			sObject* pActualObject = pObject;
			if ( pActualObject->pInstanceOfObject )
			{
				// animate actual object of the instance indirectly
				UpdateObjectCamDistance ( pActualObject );
			}
			else
			{
				// animate object directly
				if ( pObject->bVisible && pObject->bUniverseVisible )
				{
					UpdateObjectCamDistance ( pObject );
					UpdateObjectAnimation ( pObject );
				}
				else
				{
					// moved instance animation here as we only want to call it once
					if ( pObject->position.bParentOfInstance )
						UpdateObjectAnimation ( pObject );
				}
			}
		}
	}

	// okay
	return true;
}

//
// RENDERING
//

bool CObjectManager::SetVertexShader ( sMesh* pMesh )
{
	// set the vertex shader for a mesh - only change if the FVF is different

	// if VertDec different in any way
	bool bRefresh = false;
	if ( m_bUpdateVertexDecs==true )
	{
		m_bUpdateVertexDecs = false;
		bRefresh = true;
	}

	// check the mesh is okay
	SAFE_MEMORY ( pMesh );

	// regular or custom shader
	if ( pMesh->bUseVertexShader )
		m_dwCurrentShader = (DWORD)pMesh->pVertexShader;
	else
		m_dwCurrentShader = 0;

	// store the current FVF as regular
	m_dwCurrentFVF = pMesh->dwFVF;

	// is the shader different to the previously set shader
	if ( m_dwCurrentShader != m_dwLastShader || bRefresh==true )
	{
		// custom shader or Fixed-Function\FX-Effect 
		if ( pMesh->pVertexShader )
		{
			// set the new vertex shader
			if ( FAILED ( m_pD3D->SetVertexShader ( pMesh->pVertexShader ) ) )
				return false;
		}
		else
		{
			// set no vertex shader
			if ( FAILED ( m_pD3D->SetVertexShader ( NULL ) ) )
				return false;
		}

		// store the current shader
		m_dwLastShader = m_dwCurrentShader;
	}

	// is the FVF different to the previously set FVF
	if ( (m_dwCurrentFVF != m_dwLastFVF) || m_dwCurrentFVF==0 || bRefresh==true )
	{
		// custom low level shader
		if ( pMesh->pVertexShader )
		{
			// vertex dec - usually from low level assembly shader
			if ( FAILED ( m_pD3D->SetVertexDeclaration ( pMesh->pVertexDec ) ) )
				return false;
		}
		else
		{
			// custom FVF or Regular
			if ( m_dwCurrentFVF==0 )
			{
				// custom vertex dec - usually from FX effect
				if ( FAILED ( m_pD3D->SetVertexDeclaration ( pMesh->pVertexDec ) ) )
					return false;

				// regular vertex FVF - standard usage
				if ( FAILED ( m_pD3D->SetFVF ( 0 ) ) )
					return false;
			}
			else
			{
				// regular vertex FVF - standard usage
				if ( FAILED ( m_pD3D->SetFVF ( m_dwCurrentFVF ) ) )
					return false;
			}
		}

		// store the current shader
		m_dwLastFVF = m_dwCurrentFVF;
	}

	// return okay
	return true;
}

bool CObjectManager::SetInputStreams ( sMesh* pMesh )
{
	// set the input streams for drawing - only change if different

	// make sure the mesh is valid
	SAFE_MEMORY ( pMesh );

	// store a pointer to the current VB and IB
	m_ppCurrentVBRef = pMesh->pDrawBuffer->pVertexBufferRef;
	m_ppCurrentIBRef = pMesh->pDrawBuffer->pIndexBufferRef;

	// see the difference flag to false
	bool bDifferent = false;
	if ( m_ppCurrentVBRef != m_ppLastVBRef )
		bDifferent = true;

	// when a new frame starts we need to reset the streams
	if ( m_bUpdateStreams ) bDifferent = true;

	// update VB only when necessary
	if ( bDifferent )
	{
		// store the current VB
		m_ppLastVBRef = m_ppCurrentVBRef;

		// set the stream source
		if ( FAILED ( m_pD3D->SetStreamSource ( 0,
												pMesh->pDrawBuffer->pVertexBufferRef,
												0, 
												pMesh->pDrawBuffer->dwFVFSize				 ) ) )
			return false;

	}

	// leefix - 230604 - u54 - removed IF which skipped NULL index buffers (messed with last-current state check)

	// see the difference flag to false
	bDifferent = false;
	if ( m_ppCurrentIBRef != m_ppLastIBRef )
		bDifferent = true;

	// when a new frame starts we need to reset the streams
	if ( m_bUpdateStreams ) bDifferent = true;

	// update VB only when necessary
	if ( bDifferent )
	{
		// store the current VB
		m_ppLastIBRef = m_ppCurrentIBRef;

		// set the indices (if any)
		if ( m_ppCurrentIBRef )
			if ( FAILED ( m_pD3D->SetIndices ( pMesh->pDrawBuffer->pIndexBufferRef ) ) )//, pMesh->pDrawBuffer->dwBaseVertexIndex ) ) )
				return false;
	}

	// update refresh used (resets at start of cycle)
	m_bUpdateStreams = false;

	return true;
}

bool CObjectManager::PreSceneSettings ( void )
{
	// cullmode
	m_RenderStates.dwCullDirection				= m_RenderStates.dwGlobalCullDirection;
	m_pD3D->SetRenderState ( D3DRS_CULLMODE,	m_RenderStates.dwCullDirection );
	m_RenderStates.bCull						= true;
	m_RenderStates.iCullMode					= 0;	

	// U77 - 110210 - allow anistropic filtering to look better when used
	if ( g_iAnisotropyLevel==-1 )
	{
		// best card can give
		D3DCAPS9 pCaps;
		m_pD3D->GetDeviceCaps(&pCaps);
		g_iAnisotropyLevel = pCaps.MaxAnisotropy;
	}
	for ( int texturestage=0; texturestage<8; texturestage++)
		m_pD3D->SetSamplerState ( texturestage, D3DSAMP_MAXANISOTROPY, g_iAnisotropyLevel );

	// okay
	return true;
}

bool CObjectManager::PreDrawSettings ( void )
{
	// obtain external render states not tracked by manager
	m_pD3D->GetRenderState ( D3DRS_AMBIENT, &m_RenderStates.dwAmbientColor );
	m_pD3D->GetRenderState ( D3DRS_FOGCOLOR, &m_RenderStates.dwFogColor );

	// 041013 - set multisampling on for everything (if activated in SET DISPLAY MODE)
	m_pD3D->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, TRUE );

	// wireframe
	m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_RenderStates.bWireframe							= false;

	// lighting
	m_pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );
	m_RenderStates.bLight								= true;

	// fog override starts off disabled
	m_RenderStates.bFogOverride=false;

	// fogenable
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	m_RenderStates.bFog									= false;

	// ambient
	m_pD3D->SetRenderState ( D3DRS_AMBIENT, m_RenderStates.dwAmbientColor );
	m_RenderStates.iAmbient								= 1;

	// transparency
	m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE,		false );
	m_pD3D->SetRenderState ( D3DRS_ALPHAFUNC,			D3DCMP_ALWAYS );
	m_pD3D->SetRenderState ( D3DRS_DEPTHBIAS,			0 );
	m_RenderStates.bTransparency						= false;
	m_RenderStates.dwAlphaTestValue						= 0;

	// ghost
	m_pD3D->SetRenderState ( D3DRS_ZENABLE,				TRUE );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );
	m_RenderStates.bZRead								= true;
	m_RenderStates.bZWrite								= true;
	m_RenderStates.bGhost								= false;
	
	// zbias handling
	m_pD3D->SetRenderState ( D3DRS_DEPTHBIAS,			0 );
	m_pD3D->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS,	0 );
	m_RenderStates.bZBiasActive							= false;
	m_RenderStates.fZBiasSlopeScale						= 0.0f;
	m_RenderStates.fZBiasDepth							= 0.0f;

	// default material render states
	m_pD3D->SetRenderState ( D3DRS_COLORVERTEX,					TRUE );
	m_pD3D->SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE,		D3DMCS_COLOR1 );
	m_pD3D->SetRenderState ( D3DRS_SPECULARMATERIALSOURCE,		D3DMCS_MATERIAL );
	m_pD3D->SetRenderState ( D3DRS_AMBIENTMATERIALSOURCE,		D3DMCS_MATERIAL );
	m_pD3D->SetRenderState ( D3DRS_EMISSIVEMATERIALSOURCE,		D3DMCS_MATERIAL );
	m_pD3D->SetRenderState ( D3DRS_SPECULARENABLE,				TRUE );

	// white default material 'set during init'
	m_RenderStates.gWhiteDefaultMaterial.Diffuse.r		= 1.0f;
	m_RenderStates.gWhiteDefaultMaterial.Diffuse.g		= 1.0f;
	m_RenderStates.gWhiteDefaultMaterial.Diffuse.b		= 1.0f;
	m_RenderStates.gWhiteDefaultMaterial.Diffuse.a		= 1.0f;
	m_RenderStates.gWhiteDefaultMaterial.Ambient.r		= 1.0f;
	m_RenderStates.gWhiteDefaultMaterial.Ambient.g		= 1.0f;
	m_RenderStates.gWhiteDefaultMaterial.Ambient.b		= 1.0f;
	m_RenderStates.gWhiteDefaultMaterial.Ambient.a		= 1.0f;
	m_RenderStates.gWhiteDefaultMaterial.Specular.r		= 0.0f;
	m_RenderStates.gWhiteDefaultMaterial.Specular.g		= 0.0f;
	m_RenderStates.gWhiteDefaultMaterial.Specular.b		= 0.0f;
	m_RenderStates.gWhiteDefaultMaterial.Specular.a		= 0.0f;
	m_RenderStates.gWhiteDefaultMaterial.Emissive.r		= 0.0f;
	m_RenderStates.gWhiteDefaultMaterial.Emissive.g		= 0.0f;
	m_RenderStates.gWhiteDefaultMaterial.Emissive.b		= 0.0f;
	m_RenderStates.gWhiteDefaultMaterial.Emissive.a		= 0.0f;
	m_RenderStates.gWhiteDefaultMaterial.Power			= 10.0f;

	// set default white material (for diffuse, ambience, etc)
	if ( FAILED ( m_pD3D->SetMaterial ( &m_RenderStates.gWhiteDefaultMaterial ) ) )
		return false;

	// fixed function blending stage defaults
	DWORD dwMaxTextureStage = MAXTEXTURECOUNT;
	for ( DWORD dwTextureStage = 0; dwTextureStage < dwMaxTextureStage; dwTextureStage++ )
	{
		// leefix - 180204 - set defaults at start of render phase
		m_RenderStates.dwAddressU[dwTextureStage] = D3DTADDRESS_WRAP;
		m_RenderStates.dwAddressV[dwTextureStage] = D3DTADDRESS_WRAP;
		m_RenderStates.dwMagState[dwTextureStage] = D3DTEXF_LINEAR;
		m_RenderStates.dwMinState[dwTextureStage] = D3DTEXF_LINEAR;
		m_RenderStates.dwMipState[dwTextureStage] = D3DTEXF_LINEAR;

		// texture filter modes
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_ADDRESSU, m_RenderStates.dwAddressU[dwTextureStage]==0 ? D3DTADDRESS_WRAP : m_RenderStates.dwAddressU[dwTextureStage] );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_ADDRESSV, m_RenderStates.dwAddressV[dwTextureStage]==0 ? D3DTADDRESS_WRAP : m_RenderStates.dwAddressV[dwTextureStage] );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MAGFILTER, m_RenderStates.dwMagState[dwTextureStage]==0 ? D3DTEXF_LINEAR : m_RenderStates.dwMagState[dwTextureStage] );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MINFILTER, m_RenderStates.dwMinState[dwTextureStage]==0 ? D3DTEXF_LINEAR : m_RenderStates.dwMinState[dwTextureStage] );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MIPFILTER, m_RenderStates.dwMipState[dwTextureStage]==0 ? D3DTEXF_LINEAR : m_RenderStates.dwMipState[dwTextureStage] );

		// texture blending modes
		if(dwTextureStage==0)
		{
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLOROP, D3DTOP_MODULATE );
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		}
		else
		{
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLOROP, D3DTOP_DISABLE );
		}

		// texture coordinate data
		m_pD3D->SetTextureStageState( dwTextureStage, D3DTSS_TEXCOORDINDEX, dwTextureStage );
		m_pD3D->SetTextureStageState( dwTextureStage, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
	}

	// leeadd - 140304 - Set default FOV from camera (zero does not change camera FOV!)
	tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData( 0 );
	m_RenderStates.fStoreCameraFOV = m_Camera_Ptr->fFOV;
	m_RenderStates.fObjectFOV = 0.0f;

	// success
	return true;
}

bool CObjectManager::SetMeshMaterial ( sMesh* pMesh, D3DMATERIAL9* pMaterial )
{
	if ( pMesh->bUsesMaterial )
	{
		// use diffuse from material (if present)
		m_pD3D->SetRenderState ( D3DRS_COLORVERTEX,					FALSE );
		m_pD3D->SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE,		D3DMCS_MATERIAL );

		// set the material from the mesh
		if ( FAILED ( m_pD3D->SetMaterial ( pMaterial ) ) )
			return false;
	}
	else
	{
		// use diffuse from mesh vertex (if any)
		m_pD3D->SetRenderState ( D3DRS_COLORVERTEX,					TRUE );
		m_pD3D->SetRenderState ( D3DRS_DIFFUSEMATERIALSOURCE,		D3DMCS_COLOR1 );

		// set no material
		if ( FAILED ( m_pD3D->SetMaterial ( &m_RenderStates.gWhiteDefaultMaterial ) ) )
			return false;
	}

	// success
	return true;
}

bool CObjectManager::SetMeshRenderStates( sMesh* pMesh )
{
	// wireframe
	if ( pMesh->bWireframe != m_RenderStates.bWireframe )
	{
		if ( pMesh->bWireframe )
			m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		else
			m_pD3D->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );

		m_RenderStates.bWireframe = pMesh->bWireframe;
	}

	// lighting
	if ( pMesh->bLight != m_RenderStates.bLight )
	{
		if ( pMesh->bLight )
			m_pD3D->SetRenderState ( D3DRS_LIGHTING, TRUE );
		else
			m_pD3D->SetRenderState ( D3DRS_LIGHTING, FALSE );

		m_RenderStates.bLight = pMesh->bLight;
	}

	// cullmode
	if ( pMesh->bCull != m_RenderStates.bCull || pMesh->iCullMode != m_RenderStates.iCullMode )
	{
		// lee - 040306 -u6rc5 - cull mode (direction override)
		if ( pMesh->iCullMode==2 )
		{
			m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
		}
		else
		{
			// lee - 121006 - u63 - cull mode CW (for manual reflection cull toggle)
			if ( pMesh->iCullMode==3 )
			{
				m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
			}
			else
			{
				// on/off
				if ( pMesh->bCull )
					m_pD3D->SetRenderState ( D3DRS_CULLMODE, m_RenderStates.dwCullDirection );
				else	
					m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
			}
		}
		m_RenderStates.bCull = pMesh->bCull;
		m_RenderStates.iCullMode = pMesh->iCullMode;
	}

	// fog system (from light DLL)
	if(g_pGlob)
	{
		if(g_pGlob->iFogState==1)
		{
			// fogenable
			if ( pMesh->bFog != m_RenderStates.bFog )
			{
				if ( pMesh->bFog )
					m_pD3D->SetRenderState ( D3DRS_FOGENABLE, TRUE );
				else	
					m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	
				m_RenderStates.bFog = pMesh->bFog;
			}

			// ghosts in fog must override fog color part (fog override)
			if ( pMesh->bFog && pMesh->bGhost )
			{
				if ( m_RenderStates.bFogOverride==false )
				{
					m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, D3DCOLOR_RGBA ( 0, 0, 0, 0 ) );
					m_RenderStates.bFogOverride=true;
				}
			}
			else
			{
//				leefix - 030205 - seems universe getting no fog color because this condition fails!?!
//				if ( m_RenderStates.bFogOverride==true )
//				{
				m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, m_RenderStates.dwFogColor );
				m_RenderStates.bFogOverride=false;
//				}
			}
		}
	}

	// ambient - leefix - 230604 - u54 - no ambience can now be 255 or 0 (so need to do code in those cases)
	if ( pMesh->bAmbient==false && m_RenderStates.iAmbient==1
	||	 m_RenderStates.iAmbient==0 || m_RenderStates.iAmbient==2 )
	{
		if ( pMesh->bAmbient )
		{
			m_pD3D->SetRenderState ( D3DRS_AMBIENT, m_RenderStates.dwAmbientColor );
			m_RenderStates.iAmbient = 1;
		}
		else
		{
			// leefix - 210303 - diffuse colour must be maintained over any ambience
			bool bWhite=false;
			if ( pMesh->pTextures )
				if ( pMesh->pTextures[0].iImageID!=0 )
					bWhite=true;

			// leefix - 210303 - white used for no ambient on a texture
			if ( bWhite )
			{
				m_pD3D->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_ARGB(255,255,255,255) );
				m_RenderStates.iAmbient = 2;
			}
			else
			{
				m_pD3D->SetRenderState ( D3DRS_AMBIENT, D3DCOLOR_ARGB(0,0,0,0) );
				m_RenderStates.iAmbient = 0;
			}
		}
	}

	// leefix - 070204 - introduced for better Zwrite control
	bool bCorrectZWriteState = pMesh->bZWrite;

	// transparency (leefix - 190303 - added second condition where transparency is reimposed after a ghosted object)
	bool bDoGhostAgain = false;
	if ( pMesh->bTransparency != m_RenderStates.bTransparency
	||	 pMesh->dwAlphaTestValue != m_RenderStates.dwAlphaTestValue
	|| ( pMesh->bTransparency==true && m_RenderStates.bGhost==true	) )
	{
		if ( pMesh->bTransparency )
		{
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,	true );
			m_pD3D->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
			m_pD3D->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
			m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE,		true );
			
			// mike - 020904 - use this for alpha testing - do not get edges anymore
			// lee - 240903 - need full range of alpha rendered, not just the upper band
			DWORD dwuseAlphaTestValue = pMesh->dwAlphaTestValue;

			if ( dwuseAlphaTestValue==0 )
			{
				m_pD3D->SetRenderState ( D3DRS_ALPHAFUNC,	D3DCMP_GREATER );
				m_pD3D->SetRenderState ( D3DRS_ALPHAREF,	(DWORD)0x00000000 );
			}
			else
			{
				// leeadd - 131205 - let SetAlphaMappingOn command scale the alpha-test to let semi-transparent pixel through
				if ( pMesh->bAlphaOverride==true )
				{
					// alpha mapping percentage vased alpha test
					DWORD dwPercAlpha = ( (pMesh->dwAlphaOverride & 0xFF000000) >> 24 ) ;
					float perc = (float)dwPercAlpha / 255.0f;
					// leefix - 071208 - alpha test transition not perfect as go from override to 0xCF based alpha, so cap it
					// DWORD dwAlphaLevelToDraw = (DWORD)(128.0f * perc);
					DWORD dwAlphaLevelToDraw = (DWORD)(255 * perc);
					dwuseAlphaTestValue = dwAlphaLevelToDraw;
					if ( dwuseAlphaTestValue > (DWORD)0x000000CF ) dwuseAlphaTestValue=(DWORD)0x000000CF;
					m_pD3D->SetRenderState ( D3DRS_ALPHAFUNC,	D3DCMP_GREATEREQUAL );
				}
				else
				{
					// regular alpha test
					dwuseAlphaTestValue=(DWORD)0x000000CF;
					m_pD3D->SetRenderState ( D3DRS_ALPHAFUNC,	D3DCMP_GREATEREQUAL );
				}
				m_pD3D->SetRenderState ( D3DRS_ALPHAREF,	dwuseAlphaTestValue );
			}
			m_RenderStates.dwAlphaTestValue = dwuseAlphaTestValue;
		}
		else
		{
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,	false );
			m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE,		false );
			m_pD3D->SetRenderState ( D3DRS_ALPHAFUNC,			D3DCMP_ALWAYS );
		}
		m_RenderStates.bTransparency = pMesh->bTransparency;

		// now must do ghost again - to combine with blend settings
		bDoGhostAgain = true;
	}

	// ghost
	if ( bDoGhostAgain==true
	||	pMesh->bGhost != m_RenderStates.bGhost
	||  pMesh->iGhostMode != m_RenderStates.iGhostMode )
	{
		if ( pMesh->bGhost )
		{
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, true );
			switch ( pMesh->iGhostMode )
			{
				case 0:
				{
					m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
					m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				}
				break;

				case 1:
				{
					// lee - 220306 - u6b4 - direct from best of DBC (darkghostmode7)
					DWORD dwDarkAlphaSourceBlend = D3DBLEND_ZERO;
					DWORD dwDarkAlphaDestinationBlend = D3DBLEND_SRCCOLOR;
					m_pD3D->SetRenderState( D3DRS_SRCBLEND,  dwDarkAlphaSourceBlend );
					m_pD3D->SetRenderState( D3DRS_DESTBLEND, dwDarkAlphaDestinationBlend );
				}
				break;

				case 2:
				{
					m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
					m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				}
				break;

				case 3:
				{
					m_pD3D->SetRenderState ( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
					m_pD3D->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
				}
				break;

				case 4:
				{
					m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCCOLOR );
					m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTCOLOR );
				}
				break;

				case 5:
				{
					// leeadd - 210806 - replace OLD-MODE-1 (used in FPSC) for Scorch Texture Multiply
					m_pD3D->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR );
					m_pD3D->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
				}
				break;
			}
			bCorrectZWriteState = false;
		}
		else
		{
			// no ghost and no transparency, end alpha blend effect
			if ( pMesh->bTransparency==false)
			{
				m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
			}
		}

		m_RenderStates.bGhost = pMesh->bGhost;
		m_RenderStates.iGhostMode = pMesh->iGhostMode;
	}

	// leefix - 070204 - simplified - set zwrite state
	if ( pMesh->bZWrite != m_RenderStates.bZWrite )
	{
		if ( pMesh->bZWrite )
			m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		else
			m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );

		m_RenderStates.bZWrite = pMesh->bZWrite;
	}

	// leeadd - 080604 - ZBIAS handling - always set unless not active, then unset once
	if ( pMesh->bZBiasActive )
	{
		m_pD3D->SetRenderState ( D3DRS_DEPTHBIAS,			*(DWORD*)&pMesh->fZBiasDepth );
		m_pD3D->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS,	*(DWORD*)&pMesh->fZBiasSlopeScale );
		m_RenderStates.bZBiasActive = true;
	}
	else
	{
		if ( m_RenderStates.bZBiasActive )
		{
			m_pD3D->SetRenderState ( D3DRS_DEPTHBIAS,			0 );
			m_pD3D->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS,	0 );
			m_RenderStates.bZBiasActive = false;
		}
	}

	// set zread state
	if ( pMesh->bZRead != m_RenderStates.bZRead )
	{
		if ( pMesh->bZRead )
			m_pD3D->SetRenderState ( D3DRS_ZENABLE,		TRUE );
		else
			m_pD3D->SetRenderState ( D3DRS_ZENABLE,		FALSE );

		m_RenderStates.bZRead = pMesh->bZRead;
	}

	// set the new material and render state
	SetMeshMaterial ( pMesh, &pMesh->mMaterial );

	// mike - 230505 - need to be able to set mip map LOD bias on a per mesh basis
	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPMAPLODBIAS, *( ( LPDWORD ) ( &pMesh->fMipMapLODBias ) ) );

	// success
	return true;
}

bool CObjectManager::SetMeshTextureStates ( sMesh* pMesh )
{
	// close off any stages from previous runs
	if ( m_dwLastTextureCount > pMesh->dwTextureCount )
	{
		DWORD dwTexCountMax = m_dwLastTextureCount;
		if ( dwTexCountMax > 7 ) dwTexCountMax = 7;
		for ( DWORD dwTextureStage = pMesh->dwTextureCount; dwTextureStage < dwTexCountMax; dwTextureStage++ )
		{
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLOROP, D3DTOP_DISABLE );
		}
	}

	// texture filtering and blending
	DWORD dwTextureCountMax = pMesh->dwTextureCount;
	if ( dwTextureCountMax>=MAXTEXTURECOUNT ) dwTextureCountMax=MAXTEXTURECOUNT;
	for ( DWORD dwTextureIndex = 0; dwTextureIndex < pMesh->dwTextureCount; dwTextureIndex++ )
	{
		// Determine texture stage to write to
		DWORD dwTextureStage = pMesh->pTextures [ dwTextureIndex ].dwStage;

		// Determine texture data ptr
		sTexture* pTexture = &pMesh->pTextures [ dwTextureIndex ];

		// texture wrap and filter modes
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_ADDRESSU, pTexture->dwAddressU==0 ? D3DTADDRESS_WRAP : pTexture->dwAddressU );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_ADDRESSV, pTexture->dwAddressV==0 ? D3DTADDRESS_WRAP : pTexture->dwAddressV );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MAGFILTER, pTexture->dwMagState==0 ? D3DTEXF_LINEAR : pTexture->dwMagState );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MINFILTER, pTexture->dwMinState==0 ? D3DTEXF_LINEAR : pTexture->dwMinState );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MIPFILTER, pTexture->dwMipState==0 ? D3DTEXF_LINEAR : pTexture->dwMipState );

		// texture blending modes
		if ( pMesh->bOverridePixelShader )
		{
			// use custom pixel shader to replace blending stages
			m_pD3D->SetPixelShader ( pMesh->pPixelShader );
		}
		else
		{
			// fixed function does not use pixel shaders
			m_pD3D->SetPixelShader ( 0 );

			// fixed function blending (leefix-210703-fixed now at source)
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLOROP, pTexture->dwBlendMode );
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLORARG1, pTexture->dwBlendArg1 );

			// lee - 240206 - u60 - will use TFACTOR (diffuse replacement) instead of regular DIFFUSE
			if ( dwTextureStage==0 && m_RenderStates.bIgnoreDiffuse==true )
			{
				// TFACTOR set in previous call from the INSTANCE drawmesh call (instance diffuse changes)
				m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			}
			else
				m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLORARG2, pTexture->dwBlendArg2 );

			// U73 - 210309 - apply extra ARG values
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLORARG0, pTexture->dwBlendArg0 );
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_ALPHAARG0, pTexture->dwBlendArg0 );
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_RESULTARG, pTexture->dwBlendArgR );

			// last texture stage can override alpha with tfactor
			if ( m_RenderStates.bNoMeshAlphaFactor==false )
			{
				if ( pMesh->bAlphaOverride==true )
				{
					if ( dwTextureStage==pMesh->dwTextureCount-1 )
					{
						// instance overrides alpha value using TFACTOR
						m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_ALPHAOP, pTexture->dwBlendMode );
						m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_ALPHAARG2, pTexture->dwBlendArg1 );
						m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
						m_pD3D->SetRenderState( D3DRS_TEXTUREFACTOR, pMesh->dwAlphaOverride );
					}
				}
				else
				{
					// regular alpha operations
					m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_ALPHAOP, pTexture->dwBlendMode );
					m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_ALPHAARG1, pTexture->dwBlendArg1 );
					m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_ALPHAARG2, pTexture->dwBlendArg2 );
				}
			}

			// texture coordinate data
			switch ( pTexture->dwTexCoordMode )
			{
				case 0 :{	// Regular UV Stage Match
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXCOORDINDEX, dwTextureStage );
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
						}
						break;

				case 1 :{	// Sphere Mapping
							D3DXMATRIX mat;
							mat._11 = 0.5f; mat._12 = 0.0f; mat._13 = 0.0f; mat._14 = 0.0f; 
							mat._21 = 0.0f; mat._22 =-0.5f; mat._23 = 0.0f; mat._24 = 0.0f; 
							mat._31 = 0.0f; mat._32 = 0.0f; mat._33 = 1.0f; mat._34 = 0.0f; 
							mat._41 = 0.5f; mat._42 = 0.5f; mat._43 = 0.0f; mat._44 = 1.0f; 
							D3DTRANSFORMSTATETYPE dwTexTS = D3DTS_TEXTURE1;
							if( dwTextureStage==2 ) dwTexTS=D3DTS_TEXTURE2;
							m_pD3D->SetTransform ( dwTexTS, &mat );
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
						}
						break;

				case 2 :{	// Cube Mapping (leefix - 190303 - works now as camera moves around)
							D3DXMATRIX mat;
							D3DXMATRIX matview;
							m_pD3D->GetTransform ( D3DTS_VIEW, &matview );
							D3DXMatrixInverse ( &mat, NULL, &matview );
							mat._41 = 0.0f; mat._42 = 0.0f; mat._43 = 0.0f; mat._44 = 1.0f; 
							D3DTRANSFORMSTATETYPE dwTexTS = D3DTS_TEXTURE0;
							if( dwTextureStage==1 ) dwTexTS=D3DTS_TEXTURE1;
							if( dwTextureStage==2 ) dwTexTS=D3DTS_TEXTURE2;
							m_pD3D->SetTransform ( dwTexTS, &mat );
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
						}
						break;

				case 3 :{	// Steal UV Stage From Zero
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXCOORDINDEX, 0 );
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
						}
						break;

				case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17:
						{	// Set alternate texture bank for UV data
							int iGetFrom = pTexture->dwTexCoordMode-10;
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXCOORDINDEX, iGetFrom );
							m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
						}
						break;
			}
		}
	}

	// store number of textures handled for this mesh
	m_dwLastTextureCount = pMesh->dwTextureCount;

	// okay
	return true;
}

bool CObjectManager::ShaderStart ( sMesh* pMesh, IDirect3DSurface9** ppCurrentRenderTarget, IDirect3DSurface9** ppCurrentDepthTarget, UINT* puPasses, bool* pbEffectRendering, bool* pbLocalOverrideAllTexturesAndEffects )
{
	// set the vertex shader
	if ( !SetVertexShader ( pMesh ) )
		return false;

	// U75 - 070410 - can switch off fore color wipe if object flags it
	*pbLocalOverrideAllTexturesAndEffects = m_RenderStates.bOverrideAllTexturesAndEffects;
	if ( pMesh->pDrawBuffer )
		if ( pMesh->pDrawBuffer->dwImmuneToForeColorWipe==1 )
			*pbLocalOverrideAllTexturesAndEffects = false;

	// set effect shader (if any)
	if ( pMesh->pVertexShaderEffect && *pbLocalOverrideAllTexturesAndEffects==false )
	{
		// use an effect
		D3DXMATRIX matWorld;
		m_pD3D->GetTransform ( D3DTS_WORLD, &matWorld );
		*puPasses = pMesh->pVertexShaderEffect->Start ( pMesh, matWorld );

		// if FX effect, flag effect code
		if ( pMesh->pVertexShaderEffect->m_pEffect )
		{
			// 170215 - technique override allows same effect to use a LOD technique based on distance PER OBJECT!
			//D3DXHANDLE pLODTech = pMesh->pVertexShaderEffect->m_hLODTechnique;
			//D3DXHANDLE pOriginalTech = pMesh->pVertexShaderEffect->m_hLODTechnique;
			//if ( pLODTech && pOriginalTech )
			//{
			//	if ( g_fObjectCamDistance > 600.0f )
			//		pMesh->pVertexShaderEffect->m_pEffect->SetTechnique ( pLODTech );
			//	else
			//		pMesh->pVertexShaderEffect->m_pEffect->SetTechnique ( pOriginalTech );
			//}

			// 070314 - also check if if the mesh has only one polygon, and reject it from any shader usage
			if ( 1 ) // pMesh->dwVertexCount > 3 ) // turns out this allows the object to get the shadow texture for the shader..who knew!
			{
				// effect shall be used
				*pbEffectRendering=true;

				// U74 - BETA9 - 100709 - set states prior to shader begin pass
				if ( pMesh->pTextures )
				{
					for ( DWORD dwTextureIndex = 0; dwTextureIndex < pMesh->dwTextureCount; dwTextureIndex++ )
					{
						DWORD dwTextureStage = pMesh->pTextures [ dwTextureIndex ].dwStage;
						if ( dwTextureStage < 16 )
						{
							// so the object states to do overwrite any shader states that begin pass will set
							sTexture* pTexture = &pMesh->pTextures [ dwTextureIndex ];
							if ( pTexture )
							{
								m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_ADDRESSU, pTexture->dwAddressU==0 ? D3DTADDRESS_WRAP : pTexture->dwAddressU );
								m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_ADDRESSV, pTexture->dwAddressV==0 ? D3DTADDRESS_WRAP : pTexture->dwAddressV );
								m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MAGFILTER, pTexture->dwMagState==0 ? D3DTEXF_LINEAR : pTexture->dwMagState );
								m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MINFILTER, pTexture->dwMinState==0 ? D3DTEXF_LINEAR : pTexture->dwMinState );
								m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MIPFILTER, pTexture->dwMipState==0 ? D3DTEXF_LINEAR : pTexture->dwMipState );
							}
						}
					}
				}
			}
		}
	}

	// U75 - 200310 - if using RT, store current render target
	if ( pMesh->pVertexShaderEffect )
	{
		if ( pMesh->pVertexShaderEffect->m_bUsesAtLeastOneRT==true )
		{
			m_pD3D->GetRenderTarget( 0, ppCurrentRenderTarget );
			m_pD3D->GetDepthStencilSurface( ppCurrentDepthTarget );
		}
	}

	// if rendering with an effect
	if ( *pbEffectRendering )
	{
		// FF affects HLSL pipeline (and vice versa), so switch off
		// the automated clipping plane (FF will stop clipping for HLSLs)
		if ( m_RenderStates.bOverriddenClipPlaneforHLSL==false )
		{
			m_pD3D->SetRenderState ( D3DRS_CLIPPLANEENABLE, 0x00 );
			m_RenderStates.bOverriddenClipPlaneforHLSL = true;
		}
	}

	// continue
	return true;
}

DWORD g_InstanceAlphaControlValue = 0;

bool CObjectManager::ShaderPass ( sMesh* pMesh, UINT uPass, UINT uPasses, bool bEffectRendering, bool bLocalOverrideAllTexturesAndEffects, IDirect3DSurface9* pCurrentRenderTarget, IDirect3DSurface9* pCurrentDepthTarget, sObject* pObject )
{
	// return true else if something like [depth render is skipped]
	bool bResult = true;

	// U75 - 070410 - override every texture and effect with single color
	// useful for advanced post-processing effects like depthoffield/heathaze
	if ( bLocalOverrideAllTexturesAndEffects==true )
	{
		// no texture, no effect, just plane color
		m_pD3D->SetTexture ( 0, NULL );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
		m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		for ( int t=1; t<=7; t++ )
		{
			m_pD3D->SetTexture ( t, NULL );
			m_pD3D->SetTextureStageState ( t, D3DTSS_COLOROP, D3DTOP_DISABLE );
			m_pD3D->SetTextureStageState ( t, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		}
		m_pD3D->SetRenderState ( D3DRS_TEXTUREFACTOR,	m_RenderStates.dwOverrideAllWithColor );
		m_pD3D->SetRenderState ( D3DRS_FOGCOLOR,		m_RenderStates.dwFogColor );
	}
	else
	{
		// U75 - 200310 - if using RT, determine if should switch to RT or final render target (current)
		if ( bEffectRendering )
		{
			// commit var
			LPD3DXEFFECT pEffect = pMesh->pVertexShaderEffect->m_pEffect;
			bool bMustCommit = false;

			// only if RT flagged (saves performance)
			if ( pMesh->pVertexShaderEffect->m_bUsesAtLeastOneRT==true )
			{
				// ensure technique exists
				D3DXHANDLE hTech = pEffect->GetCurrentTechnique();

				// get rendercolortarget string from this pass
				D3DXHANDLE hPass = pEffect->GetPass( hTech, uPass );
				D3DXHANDLE hRT = pEffect->GetAnnotationByName( hPass, "RenderColorTarget" );
				const char* szRT = 0;
				if ( hRT ) pEffect->GetString( hRT, &szRT );

				// if detect hack, use special depth texture render target
				if ( szRT && strnicmp( szRT, "[depthtexture]", strlen("[depthtexture]") )==NULL ) 
				{
					// NOTE: Optimize here, should not be using strings/compares
					if ( g_pGlob->dwRenderCameraID==0 )
					{
						// only render depths from camera zero
						int iSuccess = SwitchRenderTargetToDepth(0);
					}
					else
					{
						// actually SKIP this pass if any other camera
						return false;
					}
				}
				else
				{
					// check if RT string has contents
					if ( hRT && strcmp( szRT, "" ) != 0 )
					{
						// yes, now get handle to texture param we want to re-direct our render to
						D3DXHANDLE hTexParamWant = pEffect->GetParameterByName( NULL, szRT );

						// go through all textures recorded during the shader parse
						for ( DWORD t = 0; t < pMesh->pVertexShaderEffect->m_dwTextureCount; t++ )
						{
							// only consider RT textures that are flagged in bitmask
							DWORD dwThisTextureBit = 1 << t;
							if ( pMesh->pVertexShaderEffect->m_dwCreatedRTTextureMask & dwThisTextureBit )
							{
								// get the param handle of each texture in shader
								int iParam = 0;
								D3DXHANDLE hTexParam = NULL;
								if ( t<=31 ) 
								{
									iParam = pMesh->pVertexShaderEffect->m_iParamOfTexture [ t ];
									hTexParam = pEffect->GetParameter( NULL, iParam );
								}

								// if it matches the one we want
								if ( hTexParam == hTexParamWant )
								{
									// get texture ptr from effect
									IDirect3DBaseTexture9* pRTTex = NULL;
									pEffect->GetTexture( hTexParam, &pRTTex );

									// switch render target to internal shader RT
									IDirect3DSurface9 *pSurface;
									((IDirect3DTexture9*)pRTTex)->GetSurfaceLevel( 0, &pSurface );
									m_pD3D->SetRenderTarget( 0, pSurface );
									if ( pSurface ) pSurface->Release( );

									// put RT textures width and height into ViewSize
									D3DXHANDLE hViewSize = pEffect->GetParameterBySemantic( NULL, "ViewSize" );
									if ( hViewSize )
									{
										D3DSURFACE_DESC desc;
										pCurrentRenderTarget->GetDesc( &desc );
										int width = desc.Width, height = desc.Height;
										D3DXHANDLE hWidth = pEffect->GetAnnotationByName( hTexParam, "width" );
										D3DXHANDLE hHeight = pEffect->GetAnnotationByName( hTexParam, "height" );
										if ( hWidth ) pEffect->GetInt( hWidth, &width );
										if ( hHeight ) pEffect->GetInt( hWidth, &height );
										D3DXVECTOR4 vec( (float) width, (float) height, 0, 0 );
										pEffect->SetVector( hViewSize, &vec );
									}
								}
							}
						}
					}
					else
					{
						// no, we render to current as normal
						m_pD3D->SetRenderTarget( 0, pCurrentRenderTarget );
						m_pD3D->SetDepthStencilSurface( pCurrentDepthTarget );

						// set the Viewsize to match the width and height of the RT passed in
						D3DXHANDLE hViewSize = pEffect->GetParameterBySemantic( NULL, "ViewSize" );
						if ( hViewSize )
						{
							D3DSURFACE_DESC desc;
							pCurrentRenderTarget->GetDesc( &desc );
							D3DXVECTOR4 vec( (float) desc.Width, (float) desc.Height, 0, 0 );
							pEffect->SetVector( hViewSize, &vec );
						}
					}
				}

				// once textures established, commit effect state changes and begin this pass
				bMustCommit = true;
			}

			// U77 - 270111 - pass clipping data to shader (automatic)
			if ( pMesh->pVertexShaderEffect->m_VecClipPlaneEffectHandle )
			{
				tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwRenderCameraID );
				if ( m_Camera_Ptr )
				{
					if ( m_Camera_Ptr->iClipPlaneOn==1 )
					{
						// special mode which creates plane but does not use RenderState to set clip
						// as you cannot mix FF clip and HLSL clip in same scene (artefacts)
						D3DXVECTOR4 vec = (D3DXVECTOR4)m_Camera_Ptr->planeClip;
						pEffect->SetVector( pMesh->pVertexShaderEffect->m_VecClipPlaneEffectHandle, &vec );
					}
					else
					{
						// ensure shader stops using clip plane when not being clipped!
						D3DXVECTOR4 vec = D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 99999.0f );
						pEffect->SetVector( pMesh->pVertexShaderEffect->m_VecClipPlaneEffectHandle, &vec );
					}
				}
				else
				{
					// ensure shader stops using clip plane when not being clipped!
					D3DXVECTOR4 vec = D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 99999.0f );
					pEffect->SetVector( pMesh->pVertexShaderEffect->m_VecClipPlaneEffectHandle, &vec );
				}
				bMustCommit = true;
			}

			// special effect which can override the texture stage of an instanced object (Guru)
			if ( g_InstanceAlphaControlValue > 0 )
			{
				// eventually we can use the other RGB components to communicate this highlight info!
				DWORD dwRedPart = (g_InstanceAlphaControlValue >> 16) & 0xFF;
				DWORD dwGreenPart = (g_InstanceAlphaControlValue >> 8) & 0xFF;
				DWORD dwBluePart = (g_InstanceAlphaControlValue) & 0xFF;
				D3DXHANDLE gGlowIntensity = pMesh->pVertexShaderEffect->m_pEffect->GetParameterByName ( NULL, "GlowIntensity" );
				if ( gGlowIntensity )
				{
					pMesh->pVertexShaderEffect->m_GlowIntensityHandle = gGlowIntensity;
					D3DXVECTOR4 vecHighlight = D3DXVECTOR4((float)dwRedPart/255.0f,(float)dwGreenPart/255.0f,(float)dwBluePart/255.0f,0);
					pMesh->pVertexShaderEffect->m_pEffect->SetVector( gGlowIntensity, &vecHighlight );
					bMustCommit = true;
				}
			}
			else
			{
				D3DXHANDLE gGlowIntensity = pMesh->pVertexShaderEffect->m_GlowIntensityHandle;
				if ( gGlowIntensity )
				{
					pMesh->pVertexShaderEffect->m_GlowIntensityHandle = NULL;
					D3DXVECTOR4 vecHighlight = D3DXVECTOR4(0,0,0,0);
					pMesh->pVertexShaderEffect->m_pEffect->SetVector ( gGlowIntensity, &vecHighlight );
					bMustCommit = true;
				}
			}
			// Character Creator Tone Control - Using the object pointer to work out
			if ( pObject && pObject->pCharacterCreator )
			{
				
				for ( int i = 0 ; i < 4 ; i++ )
				{
					if ( pMesh->pVertexShaderEffect->m_ColorTone[i] == NULL )
					{
						char s[256];
						sprintf ( s , "ColorTone[%i]" , i );
						pMesh->pVertexShaderEffect->m_ColorTone[i] = pMesh->pVertexShaderEffect->m_pEffect->GetParameterByName ( NULL, s );
					}
					if ( pMesh->pVertexShaderEffect->m_ToneMix[i] == NULL )
					{
						char s[256];
						sprintf ( s , "ToneMix[%i]" , i );
						pMesh->pVertexShaderEffect->m_ToneMix[i] = pMesh->pVertexShaderEffect->m_pEffect->GetParameterByName ( NULL, s );
					}

					if ( pMesh->pVertexShaderEffect->m_ColorTone[i] )
					{
						//pMesh->pVertexShaderEffect->m_GlowIntensityHandle = gGlowIntensity;
						D3DXVECTOR4 vecColorTone = D3DXVECTOR4(pObject->pCharacterCreator->ColorTone[i][0],pObject->pCharacterCreator->ColorTone[i][1],pObject->pCharacterCreator->ColorTone[i][2],1.0f);
						pMesh->pVertexShaderEffect->m_pEffect->SetVector( pMesh->pVertexShaderEffect->m_ColorTone[i], &vecColorTone );
					}
					if ( pMesh->pVertexShaderEffect->m_ToneMix[i] )
					{
						pMesh->pVertexShaderEffect->m_pEffect->SetFloat( pMesh->pVertexShaderEffect->m_ToneMix[i], pObject->pCharacterCreator->ToneMix[i] );
					}
				}
				bMustCommit = true;
				pMesh->pVertexShaderEffect->m_bCharacterCreatorTonesOn = true;
			}
			else
			{		
				if ( pMesh->pVertexShaderEffect->m_bCharacterCreatorTonesOn == true )
				{
					for ( int i = 0 ; i < 4 ; i++ )
					{
						if ( pMesh->pVertexShaderEffect->m_ColorTone[i] == NULL )
						{
							char s[256];
							sprintf ( s , "ColorTone[%i]" , i );
							pMesh->pVertexShaderEffect->m_ColorTone[i] = pMesh->pVertexShaderEffect->m_pEffect->GetParameterByName ( NULL, s );
						}

						if ( pMesh->pVertexShaderEffect->m_ColorTone[i] )
						{
							//pMesh->pVertexShaderEffect->m_GlowIntensityHandle = gGlowIntensity;
							D3DXVECTOR4 vecColorTone = D3DXVECTOR4(-1,0,0,1.0f);
							pMesh->pVertexShaderEffect->m_pEffect->SetVector( pMesh->pVertexShaderEffect->m_ColorTone[i], &vecColorTone );
						}
					}
					bMustCommit = true;
					pMesh->pVertexShaderEffect->m_bCharacterCreatorTonesOn = false;
				}
			}

			// when flagged, we must update effect with changes we made
			if ( bMustCommit==true )
			{
				// commit effect state changes to begin this pass
				pEffect->CommitChanges( );
			}
		}

		// FX Effect or Regular
		if ( bEffectRendering )
		{
			// mike - 300905 - disable fog for shaders
			m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
			// m_RenderStates.bFog						= false;

			// begin effect rendernig
			pMesh->pVertexShaderEffect->m_pEffect->BeginPass ( uPass );
		}

		// old FF texturing code (some effects do not do any texturing stuff)
		// leefixback - 010405 - this allowed non PS shader to use DBP textures
		//                       but it killed shader ability to use DBP textures that HAD PS code!
		// if ( bEffectWillUseFFTexturing==true && uPass==0 )
		if ( bEffectRendering )
		{
			// SHADER EFFECT

			// u64 - 180107 - effects CAN use 'texture object' textures if the effect 
			// did not assign a specfic texture to them (paul request for DarkSHADER)
			if ( pMesh->pTextures )
			{
				for ( DWORD dwTextureIndex = 0; dwTextureIndex < pMesh->dwTextureCount; dwTextureIndex++ )
				{
					DWORD dwTextureStage = pMesh->pTextures [ dwTextureIndex ].dwStage;
					if ( dwTextureStage < 16 )
					{
						// get texture ptr
						sTexture* pTexture = &pMesh->pTextures [ dwTextureIndex ];

						// m_dwUseDynamicTextureMask holds a mask of 32 bits, 1=use dynamic texture form texture object command
						int iUseDyntex = ( ( pMesh->pVertexShaderEffect->m_dwUseDynamicTextureMask >> dwTextureStage ) & 1 );
						if ( iUseDyntex==1 )
						{
							// when in effect, only if texture in effect is NULL should this be allowed
							if ( pTexture->pTexturesRef )
							{
								m_pD3D->SetTexture ( dwTextureStage, pTexture->pTexturesRef );
							}
							else
							{
								if ( pMesh->pTextures [ dwTextureIndex ].pCubeTexture )
									m_pD3D->SetTexture ( dwTextureStage, pTexture->pCubeTexture );
								else
									m_pD3D->SetTexture ( dwTextureStage, NULL);
							}
						}
					}
				}

				// set dynamic depth mapping texture for this effect
				if ( pMesh->pVertexShaderEffect )
				{
					if ( pMesh->pVertexShaderEffect->m_pEffect==g_pMainCameraDepthEffect )
					{
						g_pMainCameraDepthEffect->SetTexture ( g_pMainCameraDepthHandle, g_pMainCameraDepthTexture );
					}
				}

				// set dynamic shadow mapping texture for this effect
				if ( g_CascadedShadow.m_depthTexture[0] )
				{
					if ( pMesh->pVertexShaderEffect->m_bPrimaryEffectForCascadeShadowMapping==true )
					{
						// depth handles only relate to ONE shader!
						pMesh->pVertexShaderEffect->m_pEffect->SetTexture ( g_CascadedShadow.m_depthHandle[0], g_CascadedShadow.m_depthTexture[0]->getTexture() );
						pMesh->pVertexShaderEffect->m_pEffect->SetTexture ( g_CascadedShadow.m_depthHandle[1], g_CascadedShadow.m_depthTexture[1]->getTexture() );
						pMesh->pVertexShaderEffect->m_pEffect->SetTexture ( g_CascadedShadow.m_depthHandle[2], g_CascadedShadow.m_depthTexture[2]->getTexture() );
						pMesh->pVertexShaderEffect->m_pEffect->SetTexture ( g_CascadedShadow.m_depthHandle[3], g_CascadedShadow.m_depthTexture[3]->getTexture() );
					}
					else
					{
						// any effect that has DepthMapTX4 will be filled with most distant shadow cascade render
						DWORD dwEffectIndex = pMesh->pVertexShaderEffect->m_dwEffectIndex;
						// Dave, ensure effect index is within range
						if ( dwEffectIndex < EFFECT_INDEX_SIZE )
						{
							if ( g_CascadedShadow.m_pEffectParam[dwEffectIndex] )
							{
								D3DXHANDLE hdepthHandle0 = g_CascadedShadow.m_pEffectParam[dwEffectIndex]->DepthMapTX1;//pMesh->pVertexShaderEffect->m_pEffect->GetParameterByName( NULL, "DepthMapTX1" );
								D3DXHANDLE hdepthHandle1 = g_CascadedShadow.m_pEffectParam[dwEffectIndex]->DepthMapTX2;//pMesh->pVertexShaderEffect->m_pEffect->GetParameterByName( NULL, "DepthMapTX2" );
								D3DXHANDLE hdepthHandle2 = g_CascadedShadow.m_pEffectParam[dwEffectIndex]->DepthMapTX3;//pMesh->pVertexShaderEffect->m_pEffect->GetParameterByName( NULL, "DepthMapTX3" );
								D3DXHANDLE hdepthHandle3 = g_CascadedShadow.m_pEffectParam[dwEffectIndex]->DepthMapTX4;//pMesh->pVertexShaderEffect->m_pEffect->GetParameterByName( NULL, "DepthMapTX4" );
								if ( hdepthHandle0 ) pMesh->pVertexShaderEffect->m_pEffect->SetTexture ( hdepthHandle0, g_CascadedShadow.m_depthTexture[0]->getTexture() );
								if ( hdepthHandle1 ) pMesh->pVertexShaderEffect->m_pEffect->SetTexture ( hdepthHandle1, g_CascadedShadow.m_depthTexture[1]->getTexture() );
								if ( hdepthHandle2 ) pMesh->pVertexShaderEffect->m_pEffect->SetTexture ( hdepthHandle2, g_CascadedShadow.m_depthTexture[2]->getTexture() );
								if ( hdepthHandle3 ) pMesh->pVertexShaderEffect->m_pEffect->SetTexture ( hdepthHandle3, g_CascadedShadow.m_depthTexture[3]->getTexture() );
							}
						}
					}
				}
			}
		}
		else
		{
			// FIXED FUNCTION TEXTURING

			// FF affects HLSL pipeline (and vice versa), so switch on
			// the automated clipping plane if end of override
			if ( m_RenderStates.bOverriddenClipPlaneforHLSL==true )
			{
				tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwRenderCameraID );
				if ( m_Camera_Ptr )
				{
					if ( m_Camera_Ptr->iClipPlaneOn!=0 )
						m_pD3D->SetRenderState ( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
					else
						m_pD3D->SetRenderState ( D3DRS_CLIPPLANEENABLE, 0x00 );
				}
				m_RenderStates.bOverriddenClipPlaneforHLSL = false;
			}

			// call the texturestate function
			if ( !SetMeshTextureStates ( pMesh ) )
				return bResult;

			// is there a texture
			if ( pMesh->pTextures )
			{
				// store the current texture
				m_iCurrentTexture = pMesh->pTextures [ 0 ].iImageID;

				// is it different to the last texture we set (leefix-040803-and only if single texture otherwise lightmaps might be used)
				if ( m_iCurrentTexture != m_iLastTexture || pMesh->dwTextureCount>1 )
				{
					// set the new texture - along with related stage textures
					for ( DWORD dwTextureIndex = 0; dwTextureIndex < pMesh->dwTextureCount; dwTextureIndex++ )
					{
						// Determine texture stage to write to
						DWORD dwTextureStage = pMesh->pTextures [ dwTextureIndex ].dwStage;

						// Determine texture data ptr
						sTexture* pTexture = &pMesh->pTextures [ dwTextureIndex ];

						if ( pTexture->pTexturesRef )
						{
							// set regular texture
							if ( FAILED ( m_pD3D->SetTexture ( dwTextureStage, pTexture->pTexturesRef ) ) )
								break;
						}
						else
						{
							if ( pMesh->pTextures [ dwTextureIndex ].pCubeTexture )
							{
								// set cube texture
								if ( FAILED ( m_pD3D->SetTexture ( dwTextureStage, pTexture->pCubeTexture ) ) )
									break;
							}
							else
							{
								// set no texture
								if ( FAILED ( m_pD3D->SetTexture ( dwTextureStage, NULL) ) )
									break;
							}
						}
					}

					// now store the current texture
					m_iLastTexture = m_iCurrentTexture;
				}
			}
			else
			{
				// default zero texture
				m_pD3D->SetTexture ( 0, NULL );
				m_iLastTexture = 0;
			}
		}
	}

	// always success (unless exited early with false from depth render skip)
	return bResult;
}

bool CObjectManager::ShaderPassEnd ( sMesh* pMesh, bool bEffectRendering )
{
	// End FX Effect
	if ( bEffectRendering )	pMesh->pVertexShaderEffect->m_pEffect->EndPass();

	// continue
	return true;
}

bool CObjectManager::ShaderFinish ( sMesh* pMesh, IDirect3DSurface9* pCurrentRenderTarget, IDirect3DSurface9* pCurrentDepthTarget )
{
	// U75 - 200310 - if using RT, restore current render target
	if ( pCurrentRenderTarget )
	{
		if ( pMesh->pVertexShaderEffect )
		{
			if ( pMesh->pVertexShaderEffect->m_bUsesAtLeastOneRT==true )
			{
				m_pD3D->SetRenderTarget( 0, pCurrentRenderTarget );
				m_pD3D->SetDepthStencilSurface( pCurrentDepthTarget );
			}
		}
	}

	// Run any end code for any effect used
	if ( pMesh->pVertexShaderEffect )
	    pMesh->pVertexShaderEffect->End();

	// continue
	return true;
}

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

bool CObjectManager::DrawMesh ( sMesh* pMesh, bool bIgnoreOwnMeshVisibility, sObject* pObject )
{
	// get pointer to drawbuffers
	sDrawBuffer* pDrawBuffer = pMesh->pDrawBuffer;
	if ( pDrawBuffer==NULL )
		return true;

	// skip if mesh is invisible
	if ( bIgnoreOwnMeshVisibility==false )
	{
		if ( pMesh->iMeshType==0 )
		{
			// regular mesh mode
			if ( pMesh->bVisible==false )
				return true;
		}
		if ( pMesh->iMeshType==1 )
		{
			// terrain mesh mode (scene culling system)
			if ( g_pGlob->dwRenderCameraID==0 )
				if ( pMesh->bVisible==false )
					return true;
		}
	}

	// call the renderstate function
	if ( !SetMeshRenderStates ( pMesh ) )
		return false;

	// set the input streams
	if ( !SetInputStreams ( pMesh ) )
		return false;

	// This POINTLIST code is not used by CORE PARTICLES (uses own renderer)
	// nor is it used by CLOTH&PARTICLES (uses quad based rendered meshes)
	// this is actuall used by the PHYSX PLUGIN FOR FLUID PARTICLES
	if ( pMesh->iPrimitiveType == D3DPT_POINTLIST )
	{
		// set a default - mike needs to do this in ANYTHING that creates a pointlist object
		if ( pMesh->Collision.fRadius==0.0f ) pMesh->Collision.fRadius = 50.0f;

		// handle point sprite for distance scaling and default mesh point sprite size
		m_pD3D->SetRenderState( D3DRS_POINTSCALEENABLE, TRUE );
		m_pD3D->SetRenderState( D3DRS_POINTSIZE,		FtoDW(pMesh->Collision.fRadius/100.0f) );
		m_pD3D->SetRenderState( D3DRS_POINTSIZE_MIN,	FtoDW(0.0f) );
		m_pD3D->SetRenderState( D3DRS_POINTSIZE_MAX,	FtoDW(pMesh->Collision.fRadius) );
		m_pD3D->SetRenderState( D3DRS_POINTSCALE_A,		FtoDW(0.0f) );
		m_pD3D->SetRenderState( D3DRS_POINTSCALE_B,		FtoDW(0.0f) );
		m_pD3D->SetRenderState( D3DRS_POINTSCALE_C,		FtoDW(2.0f) );

		// force a basic texture render state
		m_pD3D->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	}

	// start shader
	UINT uPasses = 1;
	bool bEffectRendering = false;
	IDirect3DSurface9 *pCurrentRenderTarget = NULL;
	IDirect3DSurface9 *pCurrentDepthTarget = NULL;
	bool bLocalOverrideAllTexturesAndEffects = false;
	ShaderStart ( pMesh, &pCurrentRenderTarget, &pCurrentDepthTarget, &uPasses, &bEffectRendering, &bLocalOverrideAllTexturesAndEffects );

	// leefix - 270314 - do not render meshes with an effect and a single poly
	bool bSkipDrawNow = false;
	if ( pMesh->pVertexShaderEffect && pMesh->dwVertexCount<=3 )
		bSkipDrawNow = true;

	// loop through all shader passes
	// each mesh can have several render passes
    for(UINT uPass = 0; uPass < uPasses; uPass++)
    {
		// start shader pass
		if ( ShaderPass ( pMesh, uPass, uPasses, bEffectRendering, bLocalOverrideAllTexturesAndEffects, pCurrentRenderTarget, pCurrentDepthTarget, pObject )==true )
		{
			// see if we have an index buffer
			if ( bSkipDrawNow==false )
			{
				if ( pMesh->pIndices )
				{
					// if multimaterial mesh
					if ( pMesh->bUseMultiMaterial && bLocalOverrideAllTexturesAndEffects==false )
					{
						// draw several indexed primitives (one for each material)
						sMultiMaterial* pMultiMaterial = pMesh->pMultiMaterial;
						for ( DWORD dwMaterialIndex=0; dwMaterialIndex<pMesh->dwMultiMaterialCount; dwMaterialIndex++ )
						{
							// set mesh-part texture
							m_pD3D->SetTexture ( 0, pMultiMaterial [ dwMaterialIndex ].pTexturesRef );

							// set mesh-part material and render state
							SetMeshMaterial ( pMesh, &pMultiMaterial [ dwMaterialIndex ].mMaterial );

							// draw mesh-part
							if ( pMultiMaterial [ dwMaterialIndex ].dwPolyCount > 0 )
							{
								if ( FAILED ( m_pD3D->DrawIndexedPrimitive (	pDrawBuffer->dwPrimType,
																				pDrawBuffer->dwBaseVertexIndex,
																				pDrawBuffer->dwVertexStart,
																				pDrawBuffer->dwVertexCount,
																				pDrawBuffer->dwIndexStart + pMultiMaterial [ dwMaterialIndex ].dwIndexStart,
																				pMultiMaterial [ dwMaterialIndex ].dwPolyCount	) ) )
								{
									uPass=uPasses;
									break;
								}
							}

							// add to polycount
							if ( g_pGlob ) g_pGlob->dwNumberOfPolygonsDrawn += pMultiMaterial [ dwMaterialIndex ].dwPolyCount;
							if ( g_pGlob ) g_pGlob->dwNumberOfPrimCalls++;
						}

						// restore texture settings next cycle
						m_iLastTexture=-9999999;
					}
					else
					{
						// draw an indexed primitive
						if ( FAILED ( m_pD3D->DrawIndexedPrimitive (	pDrawBuffer->dwPrimType,
																		pDrawBuffer->dwBaseVertexIndex,
																		pDrawBuffer->dwVertexStart,
																		pDrawBuffer->dwVertexCount,
																		pDrawBuffer->dwIndexStart,
																		pDrawBuffer->dwPrimitiveCount		) ) )
						{
							break;
						}

						// add to polycount
						if ( g_pGlob ) g_pGlob->dwNumberOfPolygonsDrawn += pDrawBuffer->dwPrimitiveCount;
						if ( g_pGlob ) g_pGlob->dwNumberOfPrimCalls++;
					}
				}
				else
				{
					// draw a standard primitive
					if ( FAILED ( m_pD3D->DrawPrimitive (	pDrawBuffer->dwPrimType,
															pDrawBuffer->dwVertexStart,
															pDrawBuffer->dwPrimitiveCount				) ) )
					{
						// leehack-040803-if fail to render, try smaller batches of primitives until we figure it out!
						DWORD dwHowManyLeft = pDrawBuffer->dwPrimitiveCount;
						DWORD dwVertexBeginData = pDrawBuffer->dwVertexStart;
						DWORD dwPrimCountBatch=65535/3;//leehack-040803-seems some cards report 65535 but can only handle a third of that?!?
						for ( DWORD dwI=0; dwI<=(pDrawBuffer->dwPrimitiveCount/dwPrimCountBatch); dwI++ )
						{
							DWORD dwHowManyToRender = dwPrimCountBatch;
							if ( dwHowManyLeft < dwPrimCountBatch )
								dwHowManyToRender = dwHowManyLeft;

							if ( FAILED ( m_pD3D->DrawPrimitive (	pDrawBuffer->dwPrimType,
																	dwVertexBeginData,
																	dwHowManyToRender		) ) )
								break;

							// next batch of vertex data
							dwVertexBeginData+=dwHowManyToRender*3;
							dwHowManyLeft -= dwPrimCountBatch;
						}
					}

					// add to polycount
					if ( g_pGlob ) g_pGlob->dwNumberOfPolygonsDrawn += pDrawBuffer->dwPrimitiveCount;
					if ( g_pGlob ) g_pGlob->dwNumberOfPrimCalls++;
				}
			}

			// end shader pass
			ShaderPassEnd ( pMesh, bEffectRendering );
		}
	}

	// finish shader
	ShaderFinish ( pMesh, pCurrentRenderTarget, pCurrentDepthTarget );

	// leeadd - 310506 - u62 - end pointlist session (used by PhysX plugin HW fluids)
	if ( pMesh->iPrimitiveType == D3DPT_POINTLIST )
	{
		// end pointlist states
		m_pD3D->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
	}
		
	// okay
	return true;
}

bool CObjectManager::DrawMesh ( sMesh* pMesh )
{
	return DrawMesh ( pMesh, false );
}

bool CObjectManager::AddInstanceStampObjectToBuffer ( sObject* pObject, sInstanceStampTileData* pAddingStampMapPtr, int lod, DWORD dwX, DWORD dwY, DWORD dwZ )
{
	// add this one instance stamp ref object to a convenient buffer
	sInstanceStamp* pStampPtr = (sInstanceStamp*)pObject->pCustomData;
	sInstanceStampTileData** pStampMapPtrBase = pStampPtr->map;
	sInstanceStampBuffer** pStampBufferPtrBase = pStampPtr->buffer[lod];
	DWORD dwXX = 1, dwYY = pStampPtr->dwXSize, dwZZ = pStampPtr->dwXSize * pStampPtr->dwYSize;

	// find convenient buffer
	DWORD dwBuffRefX = dwX;
	DWORD dwBuffRefY = 0;//dwY;
	DWORD dwBuffRefZ = dwZ;
	DWORD dwBuffLODRange = 0;
	if ( lod==0 ) 
	{
		// base HIGH LOD resolution coordinate
	}
	if ( lod==1 ) 
	{
		// reduce resolution to get common coordinate for LOW LOD
		DWORD iDivX = dwBuffRefX/2;
		DWORD iDivY = dwBuffRefY/2;
		DWORD iDivZ = dwBuffRefZ/2;
		dwBuffRefX = iDivX*2;
		dwBuffRefY = iDivY*2;
		dwBuffRefZ = iDivZ*2;
		dwBuffLODRange = 1;
	}
	if ( lod==2 ) 
	{
		// reduce resolution to get common coordinate for HIGH QUAD
		DWORD iDivX = dwBuffRefX/4;
		DWORD iDivY = dwBuffRefY/4;
		DWORD iDivZ = dwBuffRefZ/4;
		dwBuffRefX = iDivX*4;
		dwBuffRefY = iDivY*4;
		dwBuffRefZ = iDivZ*4;
		dwBuffLODRange = 3;
	}
	sInstanceStampBuffer** pStampBufferPtrPtr = pStampBufferPtrBase + (dwBuffRefX*dwXX)+(dwBuffRefY*dwYY)+(dwBuffRefZ*dwZZ);
	
	// go through all frames in object (different textures go into different buffers)
	sObject* pParentObject = g_ObjectList [ pAddingStampMapPtr->dwParentObjNumber ];
	if ( pParentObject->dwObjectNumber==30005 )
	{
		int stophere=42;
	}

	// count how many items in refgrid, to get better size of buffer allocation
	// lod=1 means we need to count refgrid tile AND the ones x+1 and z+1
	DWORD dwItemCount = 0;
	for ( DWORD dwCountX=dwBuffRefX; dwCountX<=dwBuffRefX+dwBuffLODRange; dwCountX++ )
	{
		for ( DWORD dwCountZ=dwBuffRefZ; dwCountZ<=dwBuffRefZ+dwBuffLODRange; dwCountZ++ )
		{
			sInstanceStampTileData** pTempStampMapPtr = pStampMapPtrBase + (dwCountX*dwXX)+(dwY*dwYY)+(dwCountZ*dwZZ);
			if ( pTempStampMapPtr )
			{
				DWORD dwItemsInThisGridRef = 0;
				sInstanceStampTileData* pThisStampMapTileData = *pTempStampMapPtr;
				while ( pThisStampMapTileData )
				{
					dwItemsInThisGridRef++;
					pThisStampMapTileData = pThisStampMapTileData->pNext;
				}
				dwItemCount+=dwItemsInThisGridRef;
			}
		}
	}

	// special objects have built-in LOD handling
	bool bUseSpecialBuiltInLOD = false;
	int iLODHIGH = -1, iLODMEDIUM = -1;
	for ( DWORD iFrameScan = 0; iFrameScan < (DWORD)pParentObject->iFrameCount; iFrameScan++ )
	{
		LPSTR pFrameName = pParentObject->ppFrameList[iFrameScan]->szName;
		if ( stricmp ( pFrameName, "lod_0" )==NULL ) iLODHIGH = iFrameScan;
		if ( stricmp ( pFrameName, "lod_1" )==NULL ) iLODMEDIUM = iFrameScan;
	}
	for ( DWORD iFrameScan = 0; iFrameScan < (DWORD)pParentObject->iFrameCount; iFrameScan++ )
	{
		LPSTR pFrameName = pParentObject->ppFrameList[iFrameScan]->szName;
		if ( stricmp ( pFrameName, "lod_2" )==NULL ) iLODMEDIUM = iFrameScan;
	}
	if ( iLODHIGH!=-1 && iLODMEDIUM!=-1 ) bUseSpecialBuiltInLOD = true;

	// as parent is a hidden source object, set limb visibility so only LOWEST LOD is visible
	// for later when rendering the QUAD IMPOSTER texture
	if ( iLODMEDIUM!=-1 )
	{
		for ( DWORD dwFrameScan=0; dwFrameScan<(DWORD)pParentObject->iFrameCount; dwFrameScan++ )
			if ( pParentObject->ppFrameList[dwFrameScan]->pMesh )
				pParentObject->ppFrameList[dwFrameScan]->pMesh->bVisible = false;
		if ( pParentObject->ppFrameList[iLODMEDIUM]->pMesh )
			pParentObject->ppFrameList[iLODMEDIUM]->pMesh->bVisible = true;
	}

	// go through all frames of this parent object
	for ( DWORD iFrame = 0; iFrame < (DWORD)pParentObject->iFrameCount; iFrame++ )
	{
		// HIGH LOD and LOW LOD use actual object frame meshes
		DWORD dwMaxVBSize = 12 * 3 * 32;
		DWORD dwMaxIBSize = 36 * sizeof(DWORD);
		sInstanceStampBuffer* pStampBufferPtr = NULL;
		if ( lod==0 || lod==1 )
		{
			// if in special LOD mode, reject all but the LOD meshes specified
			if ( bUseSpecialBuiltInLOD==true )
			{
				if ( lod==0 )
				{
					if ( iFrame!=iLODHIGH ) continue;
				}
				if ( lod==1 )
				{
					if ( iFrame!=iLODMEDIUM ) continue;
				}
			}

			// starting buffer size 
			sFrame* pFrame = pParentObject->ppFrameList [ iFrame ];
			if ( pFrame ) 
			{
				if ( pFrame->pMesh ) 
				{
					dwMaxVBSize = 256 + (dwItemCount * pFrame->pMesh->dwVertexCount * pFrame->pMesh->dwFVFSize);
					dwMaxIBSize = 256 + (dwItemCount * pFrame->pMesh->dwIndexCount * sizeof(DWORD) );
				}
			}

			// if non LOD frame hierarchy, ensure we ADD other meshes in frame sequence that match texture (as we need MAX buffer size for all polygons destined for same buffer)
			if ( bUseSpecialBuiltInLOD==false )
			{
				for ( int iFrameScan=0; iFrameScan<pParentObject->iFrameCount; iFrameScan++ )
				{
					if ( iFrame!=iFrameScan )
					{
						sFrame* pScanFrame = NULL;
						if ( pParentObject->ppFrameList[iFrameScan]->pMesh ) 
							if ( pParentObject->ppFrameList[iFrameScan]->pMesh->pTextures ) 
								pScanFrame=pParentObject->ppFrameList[iFrameScan];

						if ( pScanFrame && pFrame->pMesh && pFrame->pMesh->pTextures )
						{
							if ( pFrame->pMesh->pTextures[0].pTexturesRef==pScanFrame->pMesh->pTextures[0].pTexturesRef )
							{
								dwMaxVBSize += (dwItemCount * pScanFrame->pMesh->dwVertexCount * pScanFrame->pMesh->dwFVFSize);
								dwMaxIBSize += (dwItemCount * pScanFrame->pMesh->dwIndexCount * sizeof(DWORD) );
							}
						}
					}
				}
			}

			// find any suitable buffer at this location
			if ( pStampBufferPtrPtr!=NULL )
			{
				pStampBufferPtr = *pStampBufferPtrPtr;
				while ( pStampBufferPtr )
				{
					DWORD dwThisMeshFVFSize = 0;
					LPDIRECT3DBASETEXTURE9 pThisMeshTexture = NULL;
					if ( pParentObject->ppFrameList[iFrame]->pMesh )
					{ 
						dwThisMeshFVFSize = pParentObject->ppFrameList[iFrame]->pMesh->dwFVFSize;
						pThisMeshTexture = pParentObject->ppFrameList[iFrame]->pMesh->pTextures[0].pTexturesRef;
					}
					if ( pStampBufferPtr->pMeshFromParent->pTextures[0].pTexturesRef==pThisMeshTexture
					&&   pStampBufferPtr->pMeshFromParent->dwFVFSize==dwThisMeshFVFSize )
					{
						// this buffer matches texture of specified frame, re-use this one
						break;
					}
					pStampBufferPtr = pStampBufferPtr->pNext;
				}
			}
		}
		else
		{
			// HIGH QUAD and LOW QUAD uses quad-mesh (only need one quad per item)
			if ( iFrame!=0 ) continue;

			// starting buffer size 
			DWORD dwThisFVF = g_pQUADMesh->dwFVFSize;
			dwMaxVBSize = 256 + (dwItemCount * g_pQUADMesh->dwVertexCount * dwThisFVF);
			dwMaxIBSize = 0;

			// find any suitable buffer at this location
			if ( pStampBufferPtrPtr!=NULL )
			{
				pStampBufferPtr = *pStampBufferPtrPtr;
				while ( pStampBufferPtr )
				{
					//eventually use communual texture
					//LPDIRECT3DBASETEXTURE9 pThisMeshTexture = NULL;
					//if ( pParentObject->ppFrameList[iFrame]->pMesh ) pThisMeshTexture = pParentObject->ppFrameList[iFrame]->pMesh->pTextures[0].pTexturesRef;
					if ( true ) //pStampBufferPtr->pMeshFromParent->pTextures[0].pTexturesRef==pThisMeshTexture )
					{
						// this buffer matches texture of specified frame, re-use this one
						break;
					}
					pStampBufferPtr = pStampBufferPtr->pNext;
				}
			}
		}

		// get new or existing buffer ptr
		if ( pStampBufferPtr==NULL )
		{
			// need buffer at this ref location
			sInstanceStampBuffer** pNewStampBufferPtrPtr = pStampBufferPtrBase + (dwBuffRefX*dwXX)+(dwBuffRefY*dwYY)+(dwBuffRefZ*dwZZ);
			pStampBufferPtrPtr = pNewStampBufferPtrPtr;

			// determine parent mesh for the buffer
			sMesh* pFindParentMeshToMakeBuffer = NULL;

			// create buffers for render and choose correct mesh (LOD)
			sMesh* pMesh = NULL;
			if ( lod==0 || lod==1 )
			{
				// real object meshes
				sFrame* pFrame = pParentObject->ppFrameList [ iFrame ];
				pMesh = pFrame->pMesh;
				if ( bUseSpecialBuiltInLOD==true )
				{
					// lod is a neighboring frame (specified by name LOD_X) - filtered above
				}
				else
				{
					// traditional LOD mesh
					if ( lod==1 ) pMesh = pFrame->pLOD[1];
				}

				// if no LOD, use main mesh data (legacy content)
				if ( pMesh==NULL ) pMesh = pFrame->pMesh;
			}
			else
			{
				// basic global quad mesh
				pMesh = g_pQUADMesh;
			}

			// mesh parent
			pFindParentMeshToMakeBuffer = pMesh;
			if ( pFindParentMeshToMakeBuffer==NULL )
				continue;

			// create new or reuse insert buffer
			if ( pStampPtr->pInertBufferList[lod]==NULL )
			{
				// brand new buffer
				pStampBufferPtr = new sInstanceStampBuffer;
				if ( pStampBufferPtr==NULL ) return false;
				memset ( pStampBufferPtr, 0, sizeof(sInstanceStampBuffer) );

				// init pStampBufferPtr
				pStampBufferPtr->bVisible = true;
				pStampBufferPtr->bDirty = true;
				pStampBufferPtr->dwLODLevel = lod;
				pStampBufferPtr->pMeshFromParent = pFindParentMeshToMakeBuffer;
				pStampBufferPtr->dwPrimType = D3DPT_TRIANGLELIST;
				pStampBufferPtr->dwBaseVertexIndex = 0;
				pStampBufferPtr->dwVertexStart = 0;
				pStampBufferPtr->dwIndexStart = 0;
				pStampBufferPtr->dwVBOffset = 0;
				pStampBufferPtr->dwIBOffset = 0;
				pStampBufferPtr->dwMinX = 9999999;
				pStampBufferPtr->dwMinY = 9999999;
				pStampBufferPtr->dwMinZ = 9999999;
				pStampBufferPtr->dwMaxX = 0;
				pStampBufferPtr->dwMaxY = 0;
				pStampBufferPtr->dwMaxZ = 0;
				pStampBufferPtr->vecMin = D3DXVECTOR3(9999999,9999999,9999999);
				pStampBufferPtr->vecMax = D3DXVECTOR3(0,0,0);
				pStampBufferPtr->vecCentre = D3DXVECTOR3(0,0,0);
				pStampBufferPtr->vecSize = D3DXVECTOR3(0,0,0);
				for ( int bb=0; bb<32; bb++ )
				{
					pStampBufferPtr->bOccluded[bb] = false;
					pStampBufferPtr->iQueryBusyStage[bb] = 0;
					pStampBufferPtr->dwQueryValue[bb] = 0;
					if ( pStampBufferPtr->d3dQuery[bb]==NULL )
						m_pD3D->CreateQuery( D3DQUERYTYPE_OCCLUSION, &pStampBufferPtr->d3dQuery[bb] );
					pStampBufferPtr->d3dTex[bb] = NULL;
				}
				pStampBufferPtr->iQuadTexIndexCount = 0;
				for ( int cc=0; cc<35; cc++ )
				{
					pStampBufferPtr->d3dTex[cc] = NULL;
					pStampBufferPtr->fDistanceFromCamera[cc] = 0;
					pStampBufferPtr->fAngleFromCamera[cc] = 0;
					pStampBufferPtr->bQuadTextureGenerated[cc] = false;
					pStampBufferPtr->fQuadTextureLastAngleFromCamera[cc] = 0;
					pStampBufferPtr->fQuadTextureLastHeightFromCamera[cc] = 0;
				}

				// store item ptr for later traversal
				if ( lod==2 )
				{
					pStampBufferPtr->pFirstLOD2Item = pAddingStampMapPtr;
					pAddingStampMapPtr->pLOD2BufferCollectionNext = NULL;
				}
				
				// get more buffer details
				DWORD dwFVFSize = pStampBufferPtr->pMeshFromParent->dwFVFSize;
				DWORD dwUsage = D3DUSAGE_WRITEONLY;
				DWORD dwFVF = pStampBufferPtr->pMeshFromParent->dwFVF;

				// set initial size of buffers
				pStampBufferPtr->dwCurrentMaxVBSize = dwMaxVBSize;
				pStampBufferPtr->dwCurrentMaxIBSize = dwMaxIBSize;

				// create vertex buffer
				pStampBufferPtr->pVBRef = NULL;
				if ( FAILED ( m_pD3D->CreateVertexBuffer ( 
															dwMaxVBSize,
															dwUsage,
															dwFVF,
															D3DPOOL_DEFAULT,
															&pStampBufferPtr->pVBRef,
															NULL
															) ) )
				{
					SAFE_RELEASE(pStampBufferPtr->pVBRef);
					SAFE_RELEASE(pStampBufferPtr->pIBRef);
					return false;
				}

				// create index buffer
				pStampBufferPtr->pIBRef = NULL;
				if ( dwMaxIBSize > 0 )
				{
					if ( FAILED ( m_pD3D->CreateIndexBuffer ( 
																dwMaxIBSize,
																dwUsage,
																D3DFMT_INDEX32,
																D3DPOOL_DEFAULT,
																&pStampBufferPtr->pIBRef,
																NULL
																) ) )
					{
						SAFE_RELEASE(pStampBufferPtr->pVBRef);
						SAFE_RELEASE(pStampBufferPtr->pIBRef);
						return false;
					}
				}
			}
			else
			{
				// take top inert item from inert list
				pStampBufferPtr = pStampPtr->pInertBufferList[lod];
				pStampPtr->pInertBufferList[lod] = pStampPtr->pInertBufferList[lod]->pInertListNext;
				pStampBufferPtr->pInertListNext = NULL;

				// lee, optimisation idea, search inert buffers for one large enough, and use that
				// before we take the performance hit of deleting and recreating as is below..

				// assign new mesh parent
				pStampBufferPtr->pMeshFromParent = pFindParentMeshToMakeBuffer;

				// reset buffer for new usage
				pStampBufferPtr->dwLODLevel = lod;
				pStampBufferPtr->dwBaseVertexIndex = 0;
				pStampBufferPtr->dwVertexStart = 0;
				pStampBufferPtr->dwIndexStart = 0;
				pStampBufferPtr->dwVBOffset = 0;
				pStampBufferPtr->dwIBOffset = 0;
				pStampBufferPtr->dwMinX = 9999999;
				pStampBufferPtr->dwMinY = 9999999;
				pStampBufferPtr->dwMinZ = 9999999;
				pStampBufferPtr->dwMaxX = 0;
				pStampBufferPtr->dwMaxY = 0;
				pStampBufferPtr->dwMaxZ = 0;
				pStampBufferPtr->vecMin = D3DXVECTOR3(9999999,9999999,9999999);
				pStampBufferPtr->vecMax = D3DXVECTOR3(0,0,0);
				pStampBufferPtr->vecCentre = D3DXVECTOR3(0,0,0);
				pStampBufferPtr->vecSize = D3DXVECTOR3(0,0,0);
				for ( int bb=0; bb<32; bb++ )
				{
					pStampBufferPtr->bOccluded[bb] = false;
					pStampBufferPtr->iQueryBusyStage[bb] = 0;
					pStampBufferPtr->dwQueryValue[bb] = 0;
					if ( pStampBufferPtr->d3dQuery[bb]==NULL )
						m_pD3D->CreateQuery( D3DQUERYTYPE_OCCLUSION, &pStampBufferPtr->d3dQuery[bb] );
					pStampBufferPtr->d3dTex[bb] = NULL;
				}
				pStampBufferPtr->iQuadTexIndexCount = 0;
				for ( int cc=0; cc<35; cc++ )
				{
					pStampBufferPtr->d3dTex[cc] = NULL;
					pStampBufferPtr->fDistanceFromCamera[cc] = 0;
					pStampBufferPtr->fAngleFromCamera[cc] = 0;
					pStampBufferPtr->bQuadTextureGenerated[cc] = false;
					pStampBufferPtr->fQuadTextureLastAngleFromCamera[cc] = 0;
					pStampBufferPtr->fQuadTextureLastHeightFromCamera[cc] = 0;
				}

				// store item ptr for later traversal
				if ( lod==2 )
				{
					pStampBufferPtr->pFirstLOD2Item = pAddingStampMapPtr;
					pAddingStampMapPtr->pLOD2BufferCollectionNext = NULL;
				}
				
				// reduce size back down to basic if it is enlarged
				bool bChangeBufferSizeNow = false;
				if ( pStampBufferPtr->dwCurrentMaxVBSize > dwMaxVBSize*2 
				|| 	 pStampBufferPtr->dwCurrentMaxIBSize > dwMaxIBSize*2 )
					bChangeBufferSizeNow = true;

				// or enlarge if not big enough for new site
				if ( pStampBufferPtr->dwCurrentMaxVBSize < dwMaxVBSize 
				|| 	 pStampBufferPtr->dwCurrentMaxIBSize < dwMaxIBSize )
					bChangeBufferSizeNow = true;

				// change buffer size if necessary
				if ( bChangeBufferSizeNow==true )
				{
					// delete dx buffer and restart them at normal size
					SAFE_RELEASE(pStampBufferPtr->pVBRef);
					SAFE_RELEASE(pStampBufferPtr->pIBRef);

					// set initial size of buffers
					pStampBufferPtr->dwCurrentMaxVBSize = dwMaxVBSize;
					pStampBufferPtr->dwCurrentMaxIBSize = dwMaxIBSize;

					// create refresh dx buffers
					DWORD dwFVFSize = pStampBufferPtr->pMeshFromParent->dwFVFSize;
					DWORD dwUsage = D3DUSAGE_WRITEONLY;
					DWORD dwFVF = pStampBufferPtr->pMeshFromParent->dwFVF;
					if ( FAILED ( m_pD3D->CreateVertexBuffer ( 
																dwMaxVBSize,
																dwUsage,
																dwFVF,
																D3DPOOL_DEFAULT,
																&pStampBufferPtr->pVBRef,
																NULL
																) ) )
					{
						return false;
					}
					if ( dwMaxIBSize > 0 )
					{
						if ( FAILED ( m_pD3D->CreateIndexBuffer ( 
																	dwMaxIBSize,
																	dwUsage,
																	D3DFMT_INDEX32,
																	D3DPOOL_DEFAULT,
																	&pStampBufferPtr->pIBRef,
																	NULL
																	) ) )
						{
							SAFE_RELEASE(pStampBufferPtr->pVBRef);
							return false;
						}
					}
					else
					{
						pStampBufferPtr->pIBRef = NULL;
					}
				}
			}

			// add buffer to active list (monitor and control later)
			pStampBufferPtr->pActiveListNext = pStampPtr->pActiveBufferList[lod];
			pStampPtr->pActiveBufferList[lod] = pStampBufferPtr;

			// add ref location to buffer
			pStampBufferPtr->dwRefPosX = dwBuffRefX;
			pStampBufferPtr->dwRefPosY = dwBuffRefY;
			pStampBufferPtr->dwRefPosZ = dwBuffRefZ;

			// add to buffer reference grid
			if ( *pNewStampBufferPtrPtr==NULL )
			{
				// first buffer at this slot
				*pNewStampBufferPtrPtr = pStampBufferPtr;
			}
			else
			{
				// add newly created buffer to chain
				pStampBufferPtr->pNext = *pNewStampBufferPtrPtr;
				*pNewStampBufferPtrPtr = pStampBufferPtr;
			}
		}
		else
		{
			// uses existing buffer
			if ( lod==2 )
			{
				// for LOD2 buffer additions (only done once), create a list of all items creating it (for quad rendering later)
				pAddingStampMapPtr->pLOD2BufferCollectionNext = pStampBufferPtr->pFirstLOD2Item;
				pStampBufferPtr->pFirstLOD2Item = pAddingStampMapPtr;
			}
		}

		// record how much of the buffer we use
		DWORD dwBeforeStartV = pStampBufferPtr->dwVBOffset;
		DWORD dwBeforeStartI = pStampBufferPtr->dwIBOffset;

		// with buffer we can proceed to populate it
		if ( pStampBufferPtr )
		{
			// fill vertex buffer with mesh data
			if ( pStampBufferPtr->pVBRef )
			{
				// lock vertex buffer
				// LEE tip, SHOULD LOCK JUST TO SEGMENT TO WRITE TO EVENTUALLY!
				LPSTR pfLockedData = NULL;
				if ( FAILED ( pStampBufferPtr->pVBRef->Lock ( 0, 0, ( VOID** ) &pfLockedData, 0 ) ) )
					return false;

				// lock index buffer
				LPSTR pfLockedIndexData = NULL;
				if ( pStampBufferPtr->pIBRef )
				{
					if ( FAILED ( pStampBufferPtr->pIBRef->Lock ( 0, 0, ( VOID** ) &pfLockedIndexData, 0 ) ) )
					{
						pStampBufferPtr->pVBRef->Unlock();
						return false;
					}
				}

				// go through all reference data
				LPSTR pDestPtr = pfLockedData;
				pDestPtr += pStampBufferPtr->dwVBOffset * sizeof(float);

				LPSTR pDestIndexPtr = pfLockedIndexData;
				if ( pDestIndexPtr )
					pDestIndexPtr += pStampBufferPtr->dwIBOffset * sizeof(DWORD);

				DWORD dwIndexOffsetSoFar = 0;
				dwIndexOffsetSoFar += pStampBufferPtr->dwIBOffset;

				// expand grid ref bound box each time add to buffer
				if ( dwX < pStampBufferPtr->dwMinX ) pStampBufferPtr->dwMinX = dwX;
				if ( dwY < pStampBufferPtr->dwMinY ) pStampBufferPtr->dwMinY = dwY;
				if ( dwZ < pStampBufferPtr->dwMinZ ) pStampBufferPtr->dwMinZ = dwZ;
				if ( dwX > pStampBufferPtr->dwMaxX ) pStampBufferPtr->dwMaxX = dwX;
				if ( dwY > pStampBufferPtr->dwMaxY ) pStampBufferPtr->dwMaxY = dwY;
				if ( dwZ > pStampBufferPtr->dwMaxZ ) pStampBufferPtr->dwMaxZ = dwZ;

				// expand vector boundbox for culling and occlusion
				float fItemPosX = pAddingStampMapPtr->fPosX;
				float fItemPosY = pAddingStampMapPtr->fPosY;
				float fItemPosZ = pAddingStampMapPtr->fPosZ;
				if ( lod==0 || lod==1 )
				{
					D3DXMATRIX matRot;
					D3DXMatrixRotationYawPitchRoll ( &matRot, pAddingStampMapPtr->fRotY, pAddingStampMapPtr->fRotX, pAddingStampMapPtr->fRotZ );
					D3DXMATRIX matRotAndScale;
					D3DXMatrixScaling ( &matRotAndScale, pAddingStampMapPtr->fScaleX, pAddingStampMapPtr->fScaleY, pAddingStampMapPtr->fScaleZ );
					D3DXMatrixMultiply ( &matRotAndScale, &matRotAndScale, &matRot );
					for ( int iBoxCorners=0; iBoxCorners<8; iBoxCorners++ )
					{
						D3DXVECTOR3 vecItemCorner;
						if ( iBoxCorners==0 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMin.x,pParentObject->collision.vecMin.y,pParentObject->collision.vecMin.z);
						if ( iBoxCorners==1 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMax.x,pParentObject->collision.vecMin.y,pParentObject->collision.vecMin.z);
						if ( iBoxCorners==2 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMin.x,pParentObject->collision.vecMax.y,pParentObject->collision.vecMin.z);
						if ( iBoxCorners==3 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMax.x,pParentObject->collision.vecMax.y,pParentObject->collision.vecMin.z);
						if ( iBoxCorners==4 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMin.x,pParentObject->collision.vecMin.y,pParentObject->collision.vecMax.z);
						if ( iBoxCorners==5 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMax.x,pParentObject->collision.vecMin.y,pParentObject->collision.vecMax.z);
						if ( iBoxCorners==6 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMin.x,pParentObject->collision.vecMax.y,pParentObject->collision.vecMax.z);
						if ( iBoxCorners==7 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMax.x,pParentObject->collision.vecMax.y,pParentObject->collision.vecMax.z);
						D3DXVec3TransformCoord ( &vecItemCorner, &vecItemCorner, &matRotAndScale );
						if ( pStampBufferPtr->vecMin.x > fItemPosX+vecItemCorner.x ) pStampBufferPtr->vecMin.x = fItemPosX+vecItemCorner.x;
						if ( pStampBufferPtr->vecMin.y > fItemPosY+vecItemCorner.y ) pStampBufferPtr->vecMin.y = fItemPosY+vecItemCorner.y;
						if ( pStampBufferPtr->vecMin.z > fItemPosZ+vecItemCorner.z ) pStampBufferPtr->vecMin.z = fItemPosZ+vecItemCorner.z;
						if ( pStampBufferPtr->vecMax.x < fItemPosX+vecItemCorner.x ) pStampBufferPtr->vecMax.x = fItemPosX+vecItemCorner.x;
						if ( pStampBufferPtr->vecMax.y < fItemPosY+vecItemCorner.y ) pStampBufferPtr->vecMax.y = fItemPosY+vecItemCorner.y;
						if ( pStampBufferPtr->vecMax.z < fItemPosZ+vecItemCorner.z ) pStampBufferPtr->vecMax.z = fItemPosZ+vecItemCorner.z;
					}
				}
				else
				{
					for ( int iMinAndMax=0; iMinAndMax<2; iMinAndMax++ )
					{
						D3DXVECTOR3 vecItemCorner;
						if ( iMinAndMax==0 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMin.x,pParentObject->collision.vecMin.y,pParentObject->collision.vecMin.z);
						if ( iMinAndMax==1 ) vecItemCorner = D3DXVECTOR3(pParentObject->collision.vecMax.x,pParentObject->collision.vecMax.y,pParentObject->collision.vecMax.z);
						if ( pStampBufferPtr->vecMin.x > fItemPosX+vecItemCorner.x ) pStampBufferPtr->vecMin.x = fItemPosX+vecItemCorner.x;
						if ( pStampBufferPtr->vecMin.y > fItemPosY+vecItemCorner.y ) pStampBufferPtr->vecMin.y = fItemPosY+vecItemCorner.y;
						if ( pStampBufferPtr->vecMin.z > fItemPosZ+vecItemCorner.z ) pStampBufferPtr->vecMin.z = fItemPosZ+vecItemCorner.z;
						if ( pStampBufferPtr->vecMax.x < fItemPosX+vecItemCorner.x ) pStampBufferPtr->vecMax.x = fItemPosX+vecItemCorner.x;
						if ( pStampBufferPtr->vecMax.y < fItemPosY+vecItemCorner.y ) pStampBufferPtr->vecMax.y = fItemPosY+vecItemCorner.y;
						if ( pStampBufferPtr->vecMax.z < fItemPosZ+vecItemCorner.z ) pStampBufferPtr->vecMax.z = fItemPosZ+vecItemCorner.z;
					}
				}
				pStampBufferPtr->vecSize = (pStampBufferPtr->vecMax - pStampBufferPtr->vecMin)/2;
				pStampBufferPtr->vecCentre = pStampBufferPtr->vecMin + pStampBufferPtr->vecSize;

				// keep for loop so CONTINUE can skip rest of add code
				for ( int iOneFrameOnly=0; iOneFrameOnly<1; iOneFrameOnly++ )
				{
					// can skip immediately if meshlimbvisibility false (when it was instance added to instance stamp)
					if ( lod==0 || lod==1 )
						if ( bUseSpecialBuiltInLOD==false )
							if ( (pAddingStampMapPtr->dwMeshVisibility & (1<<iFrame))==0 )
								continue;

					// get the mesh to fill buffer with 
					sFrame* pFrame = NULL;
					sMesh* pMesh = NULL;
					if ( lod==0 || lod==1 )
					{
						// get a pointer to the frame
						pFrame = pParentObject->ppFrameList [ iFrame ];
						if ( pFrame==NULL ) continue;
						if ( pFrame->bExcluded==true ) continue;

						// get mesh from frame (if one exists)
						pMesh = pFrame->pMesh;
						if ( bUseSpecialBuiltInLOD==true )
						{
							// lod is a neighboring frame (specified by name LOD_X) - filtered above
						}
						else
						{
							if ( lod==1 ) pMesh = pFrame->pLOD[1];
						}

						// if no LOD, use main mesh data (legacy content)
						if ( pMesh==NULL ) pMesh = pFrame->pMesh;
					}
					else
					{
						// mesh is from common quad mesh
						pMesh = g_pQUADMesh;
					}

					// cannot have zero mesh at this point
					if ( pMesh==NULL ) continue;

					// copy this vertex data into buffer now
					LPVOID pSourceData = pMesh->pVertexData;
					DWORD dwFVFSize = pMesh->dwFVFSize;

					// determine if this will overflow vertex buffer
					int iWillOverflowPresentBuffer = 0;
					DWORD dwSizeToCopy = pMesh->dwVertexCount * dwFVFSize;
					if ( pDestPtr + dwSizeToCopy >= pfLockedData + pStampBufferPtr->dwCurrentMaxVBSize )
						iWillOverflowPresentBuffer+=1;

					// determine if this will overflow index buffer
					DWORD dwIndexSizeToCopy = pMesh->dwIndexCount * sizeof(DWORD);
					if ( pDestIndexPtr )
						if ( pDestIndexPtr + dwIndexSizeToCopy >= pfLockedIndexData + pStampBufferPtr->dwCurrentMaxIBSize )
							iWillOverflowPresentBuffer+=2;

					// if trigger overflow detection, create larger buffer
					if ( iWillOverflowPresentBuffer>0 )
					{
						// cannot fit any more in buffer, so increase size of buffer
						DWORD dwOldMaxVBSize = pStampBufferPtr->dwCurrentMaxVBSize;
						DWORD dwOldMaxIBSize = pStampBufferPtr->dwCurrentMaxIBSize;

						// copy current data
						LPSTR pStoreVData = new char[dwOldMaxVBSize];
						if ( pStoreVData==NULL ) 
						{
							if ( pStampBufferPtr->pIBRef ) pStampBufferPtr->pIBRef->Unlock();
							pStampBufferPtr->pVBRef->Unlock();
							return false;
						}
						LPSTR pStoreIData = NULL;
						if ( pStampBufferPtr->pIBRef )
						{
							pStoreIData = new char[dwOldMaxIBSize];
							if ( pStoreIData==NULL )
							{
								delete pStoreVData;
								if ( pStampBufferPtr->pIBRef ) pStampBufferPtr->pIBRef->Unlock();
								pStampBufferPtr->pVBRef->Unlock();
								return false;
							}
						}
						memcpy ( pStoreVData, pfLockedData, dwOldMaxVBSize );
						if ( pStoreIData ) memcpy ( pStoreIData, pfLockedIndexData, dwOldMaxIBSize );

						// release old buffer lock
						if ( pStampBufferPtr->pIBRef ) pStampBufferPtr->pIBRef->Unlock();
						pStampBufferPtr->pVBRef->Unlock();

						// delete old buffer
						SAFE_RELEASE(pStampBufferPtr->pVBRef);
						SAFE_RELEASE(pStampBufferPtr->pIBRef);

						// correct new size already calculated and stored in dwMaxVBSize/dwMaxIBSize
						D3DCAPS9 caps;
						m_pD3D->GetDeviceCaps ( &caps );
						if ( dwMaxVBSize > caps.MaxVertexIndex ) dwMaxVBSize = caps.MaxVertexIndex-1;
						if ( dwMaxIBSize > 0xFFFFFF ) dwMaxIBSize = 0xFFFFFF;

						// ensure new allocation no smaller than old one
						if ( dwMaxVBSize<dwOldMaxVBSize ) dwMaxVBSize=dwOldMaxVBSize;
						if ( dwMaxIBSize<dwOldMaxIBSize ) dwMaxIBSize=dwOldMaxIBSize;

						// new buffer size
						pStampBufferPtr->dwCurrentMaxVBSize = dwMaxVBSize;
						pStampBufferPtr->dwCurrentMaxIBSize = dwMaxIBSize;

						// create new buffers
						DWORD dwFVFSize = pStampBufferPtr->pMeshFromParent->dwFVFSize;
						DWORD dwUsage = D3DUSAGE_WRITEONLY;
						DWORD dwFVF = pStampBufferPtr->pMeshFromParent->dwFVF;
						if ( FAILED ( m_pD3D->CreateVertexBuffer ( 
																	dwMaxVBSize,
																	dwUsage,
																	dwFVF,
																	D3DPOOL_DEFAULT,
																	&pStampBufferPtr->pVBRef,
																	NULL
																	) ) )
						{
							SAFE_DELETE(pStoreVData);
							SAFE_DELETE(pStoreIData);
							SAFE_RELEASE(pStampBufferPtr->pVBRef);
							SAFE_RELEASE(pStampBufferPtr->pIBRef);
							return false;
						}
						if ( dwMaxIBSize > 0 )
						{
							if ( FAILED ( m_pD3D->CreateIndexBuffer ( 
																		dwMaxIBSize,
																		dwUsage,
																		D3DFMT_INDEX32,
																		D3DPOOL_DEFAULT,
																		&pStampBufferPtr->pIBRef,
																		NULL
																		) ) )
							{
								SAFE_DELETE(pStoreVData);
								SAFE_DELETE(pStoreIData);
								SAFE_RELEASE(pStampBufferPtr->pVBRef);
								SAFE_RELEASE(pStampBufferPtr->pIBRef);
								return false;
							}
						}

						// lock new buffers
						if ( FAILED ( pStampBufferPtr->pVBRef->Lock ( 0, 0, ( VOID** ) &pfLockedData, 0 ) ) )
						{
							SAFE_DELETE(pStoreVData);
							SAFE_DELETE(pStoreIData);
							SAFE_RELEASE(pStampBufferPtr->pVBRef);
							SAFE_RELEASE(pStampBufferPtr->pIBRef);
							return false;
						}
						if ( pStampBufferPtr->pIBRef )
						{
							if ( FAILED ( pStampBufferPtr->pIBRef->Lock ( 0, 0, ( VOID** ) &pfLockedIndexData, 0 ) ) )
							{
								SAFE_DELETE(pStoreVData);
								SAFE_DELETE(pStoreIData);
								SAFE_RELEASE(pStampBufferPtr->pVBRef);
								SAFE_RELEASE(pStampBufferPtr->pIBRef);
								return false;
							}
						}

						// copy existing data to new buffer
						memcpy ( pfLockedData, pStoreVData, dwOldMaxVBSize );
						if ( pStoreIData ) memcpy ( pfLockedIndexData, pStoreIData, dwOldMaxIBSize );

						// free usages
						SAFE_DELETE(pStoreVData);
						SAFE_DELETE(pStoreIData);

						// update ptrs as we have new buffers
						pDestPtr = pfLockedData;
						pDestIndexPtr = pfLockedIndexData;
						pDestPtr += pStampBufferPtr->dwVBOffset * sizeof(float);
						if ( pDestIndexPtr ) pDestIndexPtr += pStampBufferPtr->dwIBOffset * sizeof(DWORD);

						// if not enough space to continue, skip adding this mesh
						if ( pDestPtr + dwSizeToCopy >= pfLockedData + pStampBufferPtr->dwCurrentMaxVBSize )
							continue;
					}

					// transfer data
					if ( pFrame )
					{
						float* pSrcPtrAt = (float*)pSourceData;
						float* pDestPtrAt = (float*)pDestPtr;
						memcpy ( pDestPtrAt, pSrcPtrAt, dwSizeToCopy );
						DWORD dwSizeInFloats = dwFVFSize/4;
						D3DXMATRIX matFinal;
						if ( pMesh->pBones )
						{
							matFinal = pMesh->pBones[0].matTranslation;
							D3DXMatrixMultiply ( &matFinal, &matFinal, &pFrame->matCombined );
						}
						else
						{
							matFinal = pFrame->matCombined;
						}
						D3DXMATRIX matScale;
						D3DXMatrixScaling ( &matScale, pAddingStampMapPtr->fScaleX, pAddingStampMapPtr->fScaleY, pAddingStampMapPtr->fScaleZ );
						D3DXMatrixMultiply ( &matFinal, &matFinal, &matScale );
						D3DXMATRIX matRotateItem;
						D3DXMatrixRotationYawPitchRoll ( &matRotateItem, pAddingStampMapPtr->fRotY, pAddingStampMapPtr->fRotX, pAddingStampMapPtr->fRotZ );
						D3DXMatrixMultiply ( &matFinal, &matFinal, &matRotateItem );
						D3DXMATRIX matFinalNorm = pFrame->matCombined;
						matFinalNorm._41 = 0;
						matFinalNorm._42 = 0;
						matFinalNorm._43 = 0;
						D3DXMatrixMultiply ( &matFinalNorm, &matFinalNorm, &matRotateItem );
						for ( DWORD v=0; v<pMesh->dwVertexCount; v++ )
						{
							D3DXVECTOR3 vec = *(D3DXVECTOR3*)pSrcPtrAt;
							D3DXVECTOR3 norm = *(D3DXVECTOR3*)(pSrcPtrAt+3);
							D3DXVec3TransformCoord ( &vec, &vec, &matFinal );
							D3DXVec3TransformCoord ( &norm, &norm, &matFinalNorm );
							vec.x += pAddingStampMapPtr->fPosX;
							vec.y += pAddingStampMapPtr->fPosY;
							vec.z += pAddingStampMapPtr->fPosZ;
							*(D3DXVECTOR3*)pDestPtrAt = vec;
							*(D3DXVECTOR3*)(pDestPtrAt+3) = norm;
							pDestPtrAt+=dwSizeInFloats;
							pSrcPtrAt+=dwSizeInFloats;
						}
					}
					else
					{
						// scale quad by size of item
						D3DXVECTOR3 vecScale = D3DXVECTOR3(1,1,1);
						if ( bUseSpecialBuiltInLOD==true )
						{
							sFrame* pFrame = pParentObject->ppFrameList [ iLODHIGH ];
							vecScale.x = pFrame->pMesh->Collision.vecMax.x-pFrame->pMesh->Collision.vecMin.x;
							vecScale.y = pFrame->pMesh->Collision.vecMax.y-pFrame->pMesh->Collision.vecMin.y;
							vecScale.z = pFrame->pMesh->Collision.vecMax.z-pFrame->pMesh->Collision.vecMin.z;
							vecScale.x = 500.0f;
							vecScale.y = 500.0f;
							vecScale.z = 500.0f;
						}

						// assign this item a quad index
						pAddingStampMapPtr->iQuadTexIndex[0] = pStampBufferPtr->iQuadTexIndexCount;
						pStampBufferPtr->iQuadTexIndexCount++;

						// record byte offset into vertex buffer so can overwrite with true imposter coordinates later
						pAddingStampMapPtr->dwImposterOffsetIntoBuffer[0] = pStampBufferPtr->dwVBOffset * sizeof(float);

						// transfer simpler QUAD mesh to buffer
						float* pSrcPtrAt = (float*)pSourceData;
						float* pDestPtrAt = (float*)pDestPtr;
						memcpy ( pDestPtrAt, pSrcPtrAt, dwSizeToCopy );
						DWORD dwSizeInFloats = dwFVFSize/4;
						for ( DWORD v=0; v<pMesh->dwVertexCount; v++ )
						{
							D3DXVECTOR3 vec = *(D3DXVECTOR3*)pSrcPtrAt;
							D3DXVECTOR3 norm = *(D3DXVECTOR3*)(pSrcPtrAt+3);
							float fOrigU = *(float*)(pSrcPtrAt+6);
							float fOrigV = *(float*)(pSrcPtrAt+7);
							norm.x = vec.x * vecScale.x;
							norm.y = vec.y * vecScale.y;
							norm.z = vec.z * vecScale.z;
							vec.x = pAddingStampMapPtr->fPosX;
							vec.y = pAddingStampMapPtr->fPosY;
							vec.z = pAddingStampMapPtr->fPosZ;
							*(D3DXVECTOR3*)pDestPtrAt = vec;
							*(D3DXVECTOR3*)(pDestPtrAt+3) = norm;
							*(float*)(pDestPtrAt+6) = fOrigU;
							*(float*)(pDestPtrAt+7) = fOrigV;							
							pDestPtrAt+=dwSizeInFloats;
							pSrcPtrAt+=dwSizeInFloats;
						}
					}

					// copy index data over to buffer now
					if ( pStampBufferPtr->pIBRef )
					{
						LPVOID pIndexSourceData = pMesh->pIndices;
						for ( DWORD i=0; i<pMesh->dwIndexCount; i++ )
						{
							WORD wReadAs16BIT = *((WORD*)pIndexSourceData+i);
							*((DWORD*)pDestIndexPtr+i) = pStampBufferPtr->dwVertexCount + wReadAs16BIT;
						}
					}

					// advance ptrs
					pDestPtr+=dwSizeToCopy;
					if ( pDestIndexPtr ) 
					{
						pDestIndexPtr+=dwIndexSizeToCopy;
						dwIndexOffsetSoFar+=pMesh->dwIndexCount;
					}

					// update stats
					pStampBufferPtr->dwVertexCount = (pDestPtr-pfLockedData)/pStampBufferPtr->pMeshFromParent->dwFVFSize;
					if ( pDestIndexPtr ) 
						pStampBufferPtr->dwPrimitiveCount = dwIndexOffsetSoFar/3;
					else
						pStampBufferPtr->dwPrimitiveCount = pStampBufferPtr->dwVertexCount/3;

					// update VB/IB counts
					pStampBufferPtr->dwVBOffset = ((pStampBufferPtr->dwVertexCount * dwFVFSize) / sizeof(float));
					if ( pDestIndexPtr ) pStampBufferPtr->dwIBOffset = dwIndexOffsetSoFar;
			
				} //foreach

				// unlock buffers
				if ( pStampBufferPtr->pIBRef ) pStampBufferPtr->pIBRef->Unlock();
				pStampBufferPtr->pVBRef->Unlock();
			}
		}
	}

	// done
	return true;
}

bool CObjectManager::UpdateInstanceStampObject ( sObject* pObject )
{
	// disabled all batching for now, will resurrect it when performance requires this extra boost
	// and can solve the problem of adding live polygons to dynamic VBs and causing stutter/slow down
	return true;

	// work out what should be added/removed from buffers
	// based on paradroid system of entrance/exit references
	sInstanceStamp* pStampPtr = (sInstanceStamp*)pObject->pCustomData;

	// get camera location
	tagCameraData* pCameraData = ( tagCameraData* ) g_Camera3D_GetInternalData ( g_pGlob->dwCurrentSetCameraID );
	float fCX = pCameraData->vecPosition.x;
	float fCY = 0.0f;
	float fCZ = pCameraData->vecPosition.z;
	if ( fCY > 100.0f * (20.0f+pStampPtr->dwViewYSize-1) ) fCY = 100.0f * (20.0f+pStampPtr->dwViewYSize-1);

	// monitor if additions still taking place
	bool bAdditionsTakingPlace = false;

	// ref grid map pointer and size dimensions
	sInstanceStampTileData** pStampMapPtrBase = pStampPtr->map;
	DWORD dwXX = 1, dwYY = pStampPtr->dwXSize, dwZZ = pStampPtr->dwXSize * pStampPtr->dwYSize;

	// camera position
	float fStepX = 100.0f * pStampPtr->dwViewXSize;
	float fStepY = 100.0f * pStampPtr->dwViewYSize;
	float fStepZ = 100.0f * pStampPtr->dwViewZSize;
	int iCamRefX = (int)(fCX/fStepX);
	int iCamRefY = (int)(fCY/fStepY);
	int iCamRefZ = (int)(fCZ/fStepZ);
	int iCamRefX1 = iCamRefX-1;
	int iCamRefY1 = iCamRefY-1;
	int iCamRefZ1 = iCamRefZ-1;
	int iCamRefX2 = iCamRefX+1;
	int iCamRefY2 = iCamRefY+1;
	int iCamRefZ2 = iCamRefZ+1;
	if ( iCamRefX1<0 ) iCamRefX1 = 0;
	if ( iCamRefY1<0 ) iCamRefY1 = 0;
	if ( iCamRefZ1<0 ) iCamRefZ1 = 0;
	if ( iCamRefX2>=(int)pStampPtr->dwXSize ) iCamRefX2 = pStampPtr->dwXSize - 1;
	if ( iCamRefY2>=(int)pStampPtr->dwYSize ) iCamRefY2 = pStampPtr->dwYSize - 1;
	if ( iCamRefZ2>=(int)pStampPtr->dwZSize ) iCamRefZ2 = pStampPtr->dwZSize - 1;

	// go through each LOD level (not the QUAD levels though as they are already generated)
	for ( int lod=0; lod<2; lod++ )
	{
		// early exit count (will depend on workload per add) - controls smoothness of cycles
		int earlyexitcount = 5;
		if ( lod==1 ) earlyexitcount = 10;

		// loop to cover HIGH(-1 to +1) and LOW areas(-x to +x)
		int loddepth = 1;
		if ( lod==1 ) loddepth=4;
		int iCameraScanY = iCamRefY;
		for ( int iCameraScanX=iCamRefX-loddepth; iCameraScanX<=iCamRefX+loddepth; iCameraScanX++ )
		{
			for ( int iCameraScanZ=iCamRefZ-loddepth; iCameraScanZ<=iCamRefZ+loddepth; iCameraScanZ++ )
			{
				// Cap ref coordinate to size of given map
				int iCameraX = iCameraScanX;
				int iCameraY = iCameraScanY;
				int iCameraZ = iCameraScanZ;
				if ( iCameraX<0 ) iCameraX = 0;
				if ( iCameraY<0 ) iCameraY = 0;
				if ( iCameraZ<0 ) iCameraZ = 0;
				if ( iCameraX>=(int)pStampPtr->dwXSize ) iCameraX = pStampPtr->dwXSize - 1;
				if ( iCameraY>=(int)pStampPtr->dwYSize ) iCameraY = pStampPtr->dwYSize - 1;
				if ( iCameraZ>=(int)pStampPtr->dwZSize ) iCameraZ = pStampPtr->dwZSize - 1;

				// skip the middle built covered by LOD0
				if ( lod==1 )
					if ( iCameraX>=iCamRefX1 && iCameraX<=iCamRefX2 )
						if ( iCameraY>=iCamRefY1 && iCameraY<=iCamRefY2 )
							if ( iCameraZ>=iCamRefZ1 && iCameraZ<=iCamRefZ2 )
								continue;

				// ref that needs a buffer to live in
				DWORD dwX = iCameraX;
				DWORD dwY = iCameraY;
				DWORD dwZ = iCameraZ;
				sInstanceStampTileData** pStampMapPtr = pStampMapPtrBase + (dwX*dwXX)+(dwY*dwYY)+(dwZ*dwZZ);
				if ( (*pStampMapPtr)==NULL ) continue;

				// go through all items in this ref
				sInstanceStampTileData* pItem = (*pStampMapPtr);
				while ( pItem )
				{
					if ( pItem->bAddedToBuffer[lod]==false )
					{
						AddInstanceStampObjectToBuffer(pObject,pItem,lod,dwX,dwY,dwZ);
						pItem->bAddedToBuffer[lod] = true;
						bAdditionsTakingPlace = true;

						// early exit to smooth out update delay
						earlyexitcount--;
						if ( earlyexitcount<=0 )
						{
							// ensure 'later' LOW to HIGH phase does not start too soon!
							break;
						}
					}
					pItem = pItem->pNext;
				}
			}
		}

		// REMOVE BUFFERS
		// also scan areas that need to have buffers removed
		// code = detect when bounds of a buffer are entirely outside camera zone
		if ( lod==1 )
		{
			// area within stuff can remain visible
			DWORD dwCameraMinX = iCamRefX - 5;
			DWORD dwCameraMaxX = iCamRefX + 5;
			DWORD dwCameraMinZ = iCamRefZ - 5;
			DWORD dwCameraMaxZ = iCamRefZ + 5;

			// check against active buffers to see if any have left the above region
			sInstanceStampBuffer* pActiveBufferList = pStampPtr->pActiveBufferList[lod];
			while ( pActiveBufferList )
			{
				// get next active ptr ready (as we delete next ref in this cycle)
				sInstanceStampBuffer* pNextActiveBufferList = pActiveBufferList->pActiveListNext;

				// is buffer bound intersecting camera zone
				bool bBufferWithinCameraZone = false;
				if ( pActiveBufferList->dwMinX <= dwCameraMaxX && pActiveBufferList->dwMaxX >= dwCameraMinX )
					if ( pActiveBufferList->dwMinZ <= dwCameraMaxZ && pActiveBufferList->dwMaxZ >= dwCameraMinZ )
						bBufferWithinCameraZone = true;

				if ( bBufferWithinCameraZone==false )
				{
					// remove this buffer from ref map
					DWORD dwX = pActiveBufferList->dwRefPosX;
					DWORD dwY = pActiveBufferList->dwRefPosY;
					DWORD dwZ = pActiveBufferList->dwRefPosZ;
					sInstanceStampBuffer** pStampBufferPtrBase = pStampPtr->buffer[lod];
					sInstanceStampBuffer** pNewStampBufferPtrPtr = pStampBufferPtrBase + (dwX*dwXX)+(dwY*dwYY)+(dwZ*dwZZ);
					if ( *pNewStampBufferPtrPtr==pActiveBufferList )
					{
						*pNewStampBufferPtrPtr = (*pNewStampBufferPtrPtr)->pNext;
					}
					else
					{
						sInstanceStampBuffer* pFind = *pNewStampBufferPtrPtr;
						while ( pFind )
						{
							if ( pFind->pNext==pActiveBufferList )
							{
								pFind->pNext = pActiveBufferList->pNext;
								break;
							}
							pFind = pFind->pNext;
						}
					}

					// reset instance flags as they will no longer be in any buffers
					for ( DWORD dwRX=pActiveBufferList->dwMinX; dwRX<=pActiveBufferList->dwMaxX; dwRX++ )
					{
						for ( DWORD dwRY=pActiveBufferList->dwMinY; dwRY<=pActiveBufferList->dwMaxY; dwRY++ )
						{
							for ( DWORD dwRZ=pActiveBufferList->dwMinZ; dwRZ<=pActiveBufferList->dwMaxZ; dwRZ++ )
							{
								// possible ref that needs a buffer to live in
								sInstanceStampTileData** pStampMapPtr = pStampMapPtrBase + (dwRX*dwXX)+(dwRY*dwYY)+(dwRZ*dwZZ);
								if ( (*pStampMapPtr)==NULL ) continue;
								sInstanceStampTileData* pItem = (*pStampMapPtr);
								while ( pItem )
								{
									//pItem->dwBuffersUsed[lod] = 0;
									pItem->bAddedToBuffer[lod] = false;
									pItem = pItem->pNext;
								}
							}
						}
					}

					// reset counters
					pActiveBufferList->dwVBOffset = 0;
					pActiveBufferList->dwIBOffset = 0;
					pActiveBufferList->dwPrimitiveCount = 0;
					pActiveBufferList->dwVertexCount = 0;

					// add this buffer to the inert list
					if ( pStampPtr->pInertBufferList[lod]==NULL )
						pStampPtr->pInertBufferList[lod] = pActiveBufferList;
					else
					{
						pActiveBufferList->pInertListNext = pStampPtr->pInertBufferList[lod];
						pStampPtr->pInertBufferList[lod] = pActiveBufferList;
					}

					// remove buffer from active list
					if ( pActiveBufferList==pStampPtr->pActiveBufferList[lod] )
						pStampPtr->pActiveBufferList[lod] = pActiveBufferList->pActiveListNext;
					else
					{
						sInstanceStampBuffer* pFind = pStampPtr->pActiveBufferList[lod];
						while ( pFind )
						{
							if ( pFind->pActiveListNext==pActiveBufferList )
							{
								pFind->pActiveListNext = pActiveBufferList->pActiveListNext;
								break;
							}
							pFind = pFind->pActiveListNext;
						}
					}

					// cut off from active buffer list
					pActiveBufferList->pActiveListNext = NULL;
					pActiveBufferList->pNext = NULL;

					// start again as we have modified list to ensure we are not traversing deleted item
					pActiveBufferList = pStampPtr->pActiveBufferList[lod];
				}
				else
				{
					// go to next buffer in active list
					pActiveBufferList = pNextActiveBufferList;
				}
			}
		}
	}

	// If addition takes place, we moved, so dirty LOW LOD AREAS in case we need to add new HIGH items
	if ( bAdditionsTakingPlace==true )
	{
		sInstanceStampBuffer* pActiveBufferList = pStampPtr->pActiveBufferList[1];
		while ( pActiveBufferList )
		{
			pActiveBufferList->bDirty = true;
			pActiveBufferList = pActiveBufferList->pActiveListNext;
		}
	}

	// ACTIVATE HIGH ITEMS in LOW LOD AREAS where HIGH CENTER AREA overlaps them
	// (and then HIDE the LOW LOD AREA so that we do not have HIGH and LOW lods overlapping)
	if ( bAdditionsTakingPlace==false )
	{
		int lod = 1; //LLA
		sInstanceStampBuffer* pActiveBufferList = pStampPtr->pActiveBufferList[lod];
		while ( pActiveBufferList )
		{
			if ( pActiveBufferList->bDirty==true )
			{
				// does center area encroach on any LOW LOD AREA buffer?
				bool bHighCenterInterectsLowLodArea = false;
				if ( iCamRefX1<=(int)pActiveBufferList->dwMaxX && iCamRefX2>=(int)pActiveBufferList->dwMinX)
					if ( iCamRefZ1<=(int)pActiveBufferList->dwMaxZ && iCamRefZ2>=(int)pActiveBufferList->dwMinZ)
						bHighCenterInterectsLowLodArea = true;

				// if there is an intersect
				if ( bHighCenterInterectsLowLodArea==true )
				{
					// we need to completely add HIGH items to all spaces covered by LLA
					int earlyexitcount = 5;
					DWORD dwY=pActiveBufferList->dwRefPosY;
					for ( DWORD dwX=pActiveBufferList->dwRefPosX; dwX<=pActiveBufferList->dwRefPosX+1; dwX++ )
					{
						for ( DWORD dwZ=pActiveBufferList->dwRefPosZ; dwZ<=pActiveBufferList->dwRefPosZ+1; dwZ++ )
						{
							// possible ref that needs a buffer to live in
							sInstanceStampTileData** pStampMapPtr = pStampMapPtrBase + (dwX*dwXX)+(dwY*dwYY)+(dwZ*dwZZ);
							if ( (*pStampMapPtr)==NULL ) continue;

							// go through all items in this ref
							int lod = 0;
							sInstanceStampTileData* pItem = (*pStampMapPtr);
							while ( pItem )
							{
								if ( pItem->bAddedToBuffer[lod]==false )
								{
									AddInstanceStampObjectToBuffer(pObject,pItem,lod,dwX,dwY,dwZ);
									pItem->bAddedToBuffer[lod] = true;

									// early exit to smooth out update delay
									earlyexitcount--;
									if ( earlyexitcount<=0 )
										break;
								}
								pItem = pItem->pNext;
							}
						}
					}
					if ( earlyexitcount==5 )
					{
						// then when we can complete the full loop without an early exit, hide the LOW LOD AREA
						pActiveBufferList->bDirty = false;
						pActiveBufferList->bVisible = false;
					}
				}
				else
				{
					// LLA is NOT intersecting HIGH CENTER AREA (and LLA is invisible) (weird but works, do not reverse!)
					if ( pActiveBufferList->bVisible==false )
					{
						// delete any HCA buffers which are entirely within this LLA (copy of above - possible cleanup here!)
						sInstanceStampBuffer* pActiveHighBufferList = pStampPtr->pActiveBufferList[0];
						while ( pActiveHighBufferList )
						{
							// get next active ptr ready (as we delete next ref in this cycle)
							sInstanceStampBuffer* pNextActiveBufferList = pActiveHighBufferList->pActiveListNext;

							// is HIGH buffer bound intersecting LLA
							bool bBufferWithinLLA = false;
							if ( pActiveHighBufferList->dwRefPosX>=pActiveBufferList->dwRefPosX && pActiveHighBufferList->dwRefPosX<=pActiveBufferList->dwRefPosX+1 )
								if ( pActiveHighBufferList->dwRefPosZ>=pActiveBufferList->dwRefPosZ && pActiveHighBufferList->dwRefPosZ<=pActiveBufferList->dwRefPosZ+1 )
									bBufferWithinLLA = true;

							if ( bBufferWithinLLA==true )
							{
								// remove this buffer from ref map
								sInstanceStampBuffer** pStampBufferPtrBase = pStampPtr->buffer[0];
								DWORD dwY = pActiveHighBufferList->dwRefPosY;
								DWORD dwX = pActiveHighBufferList->dwRefPosX;
								DWORD dwZ = pActiveHighBufferList->dwRefPosZ;
								sInstanceStampBuffer** pNewStampBufferPtrPtr = pStampBufferPtrBase + (dwX*dwXX)+(dwY*dwYY)+(dwZ*dwZZ);
								if ( *pNewStampBufferPtrPtr==pActiveHighBufferList )
								{
									*pNewStampBufferPtrPtr = (*pNewStampBufferPtrPtr)->pNext;
								}
								else
								{
									sInstanceStampBuffer* pFind = *pNewStampBufferPtrPtr;
									while ( pFind )
									{
										if ( pFind->pNext==pActiveHighBufferList )
										{
											pFind->pNext = pActiveHighBufferList->pNext;
											break;
										}
										pFind = pFind->pNext;
									}
								}

								// reset instance flags as they will no longer be in any buffers
								DWORD dwRX=pActiveHighBufferList->dwRefPosX;
								DWORD dwRY=pActiveHighBufferList->dwRefPosY;
								DWORD dwRZ=pActiveHighBufferList->dwRefPosZ;

								// possible ref that needs a buffer to live in
								sInstanceStampTileData** pStampMapPtr = pStampMapPtrBase + (dwRX*dwXX)+(dwRY*dwYY)+(dwRZ*dwZZ);
								if ( (*pStampMapPtr)==NULL ) continue;
								sInstanceStampTileData* pItem = (*pStampMapPtr);
								while ( pItem )
								{
									//pItem->dwBuffersUsed[0] = 0;
									pItem->bAddedToBuffer[0] = false;
									pItem = pItem->pNext;
								}

								// reset counters
								pActiveHighBufferList->dwVBOffset = 0;
								pActiveHighBufferList->dwIBOffset = 0;
								pActiveHighBufferList->dwPrimitiveCount = 0;
								pActiveHighBufferList->dwVertexCount = 0;

								// add this buffer to the inert list
								if ( pStampPtr->pInertBufferList[0]==NULL )
									pStampPtr->pInertBufferList[0] = pActiveHighBufferList;
								else
								{
									pActiveHighBufferList->pInertListNext = pStampPtr->pInertBufferList[0];
									pStampPtr->pInertBufferList[0] = pActiveHighBufferList;
								}

								// remove buffer from active list
								if ( pActiveHighBufferList==pStampPtr->pActiveBufferList[0] )
									pStampPtr->pActiveBufferList[0] = pActiveHighBufferList->pActiveListNext;
								else
								{
									sInstanceStampBuffer* pFind = pStampPtr->pActiveBufferList[0];
									while ( pFind )
									{
										if ( pFind->pActiveListNext==pActiveHighBufferList )
										{
											pFind->pActiveListNext = pActiveHighBufferList->pActiveListNext;
											break;
										}
										pFind = pFind->pActiveListNext;
									}
								}

								// cut off from active buffer list
								pActiveHighBufferList->pActiveListNext = NULL;
								pActiveHighBufferList->pNext = NULL;

								// start again as we have modified list to ensure we are not traversing deleted item
								pActiveHighBufferList = pStampPtr->pActiveBufferList[0];
							}
							else
							{
								// go to next buffer in active list
								pActiveHighBufferList = pNextActiveBufferList;
							}
						}

						// show LLA
						pActiveBufferList->bVisible = true;
					}
				}
			}
			pActiveBufferList = pActiveBufferList->pActiveListNext;
		}
	}

	/* even some HIGH LOD polygons reach HIGH QUAD?!? - this can save me a draw call if I can solve it
	// decide which QUAD buffers are visible
	int lod=2;
	sInstanceStampBuffer* pQUADBufferList = pStampPtr->pActiveBufferList[lod];
	while ( pQUADBufferList )
	{
		// find Hgih Quad Area corner
		DWORD dwHQARefX = pQUADBufferList->dwRefPosX;
		DWORD dwHQARefY = pQUADBufferList->dwRefPosY;
		DWORD dwHQARefZ = pQUADBufferList->dwRefPosZ;

		// check all four LOW LOD buffer visibilities captured by HIGH QUAD area
		bool bHideTheQUAD = false;
		for ( int scanlod=0; scanlod<1; scanlod++ )
		{
			sInstanceStampBuffer** pStampBufferPtrBase = pStampPtr->buffer[scanlod];
			int iCountVisible = 0;
			int loddepth = 4;
			if ( scanlod==1 ) loddepth = 2;
			for ( int dwZ=0; dwZ<loddepth; dwZ++ )
			{
				for ( int dwX=0; dwX<loddepth; dwX++ )
				{
					sInstanceStampBuffer** pNewStampBufferPtrPtr = pStampBufferPtrBase + ((dwHQARefX+dwX)*dwXX)+(dwHQARefY*dwYY)+((dwHQARefZ+dwZ)*dwZZ);
					if ( pNewStampBufferPtrPtr )
					{
						sInstanceStampBuffer* pThisBuffer = *pNewStampBufferPtrPtr;
						while ( pThisBuffer )
						{
							if ( pThisBuffer->bVisible==true ) 
								iCountVisible++;
							pThisBuffer = pThisBuffer->pNext;
						}
					}
				}
			}
			if ( scanlod==0 && iCountVisible>0 ) bHideTheQUAD = true;
			//if ( scanlod==1 && iCountVisible>0 ) bHideTheQUAD = true; // wuads in closer so we can fade between LOW LOD and HIGH QUAD
		}

		// only completely visible LOW LOD set will hide HIGH QUAD
		if ( bHideTheQUAD==true )
			pQUADBufferList->bVisible = false;
		else
			pQUADBufferList->bVisible = true;

		pQUADBufferList = pQUADBufferList->pActiveListNext;
	}
	*/

	// complete
	return true;
}

 // Construct a world-space billboard that (in screen-space) 
 // fully covers the input bounding box.
void CObjectManager::createImposterBillboard(
			const D3DXVECTOR3& curCameraPos,
			const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projMatrix,
			const D3DXMATRIX& worldTransform, 
			D3DXVECTOR3 boundingBoxVerts[NUM_BOX_VERTS],
			float boundingRadius,
			sImposter* pImposter, float& imposterNearPlane, float& imposterFarPlane)
{
	int stride = sizeof(D3DXVECTOR3);
	D3DXVECTOR3 screenVerts[NUM_BOX_VERTS];
	int screenWidth = 1360;//dbScreenWidth();
	int screenHeight = 768;//dbScreenHeight();
	D3DVIEWPORT9 viewport = { 0, 0, screenWidth, screenHeight, 0, 1 };

	// Project bounding box into screen space.
	D3DXVec3ProjectArray(screenVerts, stride, boundingBoxVerts, stride, 
                            &viewport, &projMatrix, &viewMatrix, &worldTransform,
                            NUM_BOX_VERTS);

	// Determine the smallest screen-space quad that encompasses the bounding box.
	float minX = screenVerts[0].x;
	float maxX = screenVerts[0].x;
	float minY = screenVerts[0].y;
	float maxY = screenVerts[0].y;
	float minZ = screenVerts[0].z;
	for (int i = 1; i < NUM_BOX_VERTS; ++i)
	{
		minX = min(screenVerts[i].x, minX);
		maxX = max(screenVerts[i].x, maxX);
		minY = min(screenVerts[i].y, minY);
		maxY = max(screenVerts[i].y, maxY);
		minZ = min(screenVerts[i].z, minZ);
	}

	// Add extra vertices here to compute the near plane and far plane positions.
	const int NUM_VERTS = 4;
	D3DXVECTOR3 screenSpaceVerts[NUM_VERTS] =
	{
		D3DXVECTOR3(minX, minY, minZ),
		D3DXVECTOR3(maxX, minY, minZ),
		D3DXVECTOR3(maxX, maxY, minZ),
		D3DXVECTOR3(minX, maxY, minZ)
	};
		
	// Unproject the screen-space quad into world-space.
    D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);

	D3DXVECTOR3 worldSpaceVerts[NUM_VERTS];

	D3DXVec3UnprojectArray(worldSpaceVerts, stride, screenSpaceVerts, stride, 
                              &viewport, &projMatrix, &viewMatrix, &identity,
                              NUM_VERTS); 

	// Save the imposter vertices for later.
	pImposter->verts[0].pos = worldSpaceVerts[0];
	pImposter->verts[1].pos = worldSpaceVerts[1];
	pImposter->verts[2].pos = worldSpaceVerts[3];
	pImposter->verts[3].pos = worldSpaceVerts[1];
	pImposter->verts[4].pos = worldSpaceVerts[2];
	pImposter->verts[5].pos = worldSpaceVerts[3];
	pImposter->centre = (worldSpaceVerts[0] + worldSpaceVerts[1] + 
       worldSpaceVerts[2] + worldSpaceVerts[3]) * 0.25f;

	pImposter->cameraDir = curCameraPos - pImposter->centre;
	D3DXVec3Normalize(&pImposter->cameraDir, &pImposter->cameraDir);

	// Calculate near and far planes.
	D3DXVECTOR3 nearPlaneVec = pImposter->centre - curCameraPos;
	imposterNearPlane = D3DXVec3Length(&nearPlaneVec);
	imposterFarPlane = imposterNearPlane + (boundingRadius * 2.0f);
}

// Create view and projection matrices, based on the current camera and 
// using the imposter quad as the viewing plane.
void CObjectManager::createMatrices(
			const D3DXVECTOR3& curCameraPos,
			const D3DXVECTOR3& curCameraUp,
			D3DXMATRIX& imposterViewMatrix, D3DXMATRIX& imposterProjMatrix, sImposter* pImposter, float imposterNearPlane, float imposterFarPlane)
{
	D3DXMatrixLookAtLH(&imposterViewMatrix, &curCameraPos, 
		               &pImposter->centre, &curCameraUp);
	D3DXVECTOR3 widthVec = pImposter->verts[1].pos - pImposter->verts[0].pos;
	D3DXVECTOR3 heightVec = pImposter->verts[5].pos - pImposter->verts[0].pos;
	float viewWidth = D3DXVec3Length(&widthVec);
	float viewHeight = D3DXVec3Length(&heightVec);
	D3DXMatrixPerspectiveLH(&imposterProjMatrix, viewWidth, viewHeight, 
				    imposterNearPlane, imposterFarPlane);
}

// Initialise a D3D viewport for rendering a poster into a render texture sub-region.
void CObjectManager::initImposterViewport(int textureWidth, int textureHeight,
                                          const D3DXVECTOR2& imposterUVOffset, 
                                          const D3DXVECTOR2& imposterUVScale)
{
	D3DVIEWPORT9 viewport;
	viewport.X = (DWORD) (textureWidth * imposterUVOffset.x);
	viewport.Y = (DWORD) (textureHeight * imposterUVOffset.y);
	viewport.Width = (DWORD) (textureWidth * imposterUVScale.x);
	viewport.Height = (DWORD) (textureHeight * imposterUVScale.y);
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	m_pD3D->SetViewport(&viewport);
}

D3DFORMAT CObjectManager::GetValidStencilBufferFormat ( D3DFORMAT BackBufferFormat )
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

	LPDIRECT3D9 m_pDX;
	m_pD3D->GetDirect3D(&m_pDX);

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

int CObjectManager::SwitchRenderTargetToDepth ( int iFlag )
{
	// create render target if not exists
	if ( g_pMainCameraDepthTexture==NULL )
	{
		D3DSURFACE_DESC desc;
		IDirect3DSurface9* pCurrentRenderTarget;
		m_pD3D->GetRenderTarget ( 0, &pCurrentRenderTarget );
		pCurrentRenderTarget->GetDesc( &desc );
		D3DXCreateTexture( m_pD3D, desc.Width, desc.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pMainCameraDepthTexture );
		g_pMainCameraDepthTexture->GetSurfaceLevel ( 0, &g_pMainCameraDepthTextureSurfaceRef );
		m_pD3D->CreateDepthStencilSurface( desc.Width, desc.Height, GetValidStencilBufferFormat(desc.Format), D3DMULTISAMPLE_NONE, 0, TRUE, &g_pMainCameraDepthStencilTexture, NULL );
	}

	// copy to debug image from last usage
	if ( g_pMainCameraDepthTexture && g_pMainCameraDepthTextureSurfaceRef && rand()%100==1 )
	{
		LPDIRECT3DTEXTURE9 pDebugSee = g_Image_GetPointer ( 59949 );
		if ( pDebugSee )
		{
			if ( g_pMainCameraDepthTextureSurfaceRef )
			{
				LPDIRECT3DSURFACE9 pShadowDebugImage;
				pDebugSee->GetSurfaceLevel ( 0, &pShadowDebugImage );
				if ( pShadowDebugImage )
				{
					HRESULT hRes = D3DXLoadSurfaceFromSurface ( pShadowDebugImage, NULL, NULL, g_pMainCameraDepthTextureSurfaceRef, NULL, NULL, D3DX_DEFAULT, 0 );
					pShadowDebugImage->Release();
				}
			}
		}
	}

	// we channel all renders to a special depth texture render target
	m_pD3D->SetRenderTarget( 0, g_pMainCameraDepthTextureSurfaceRef );
	m_pD3D->SetDepthStencilSurface( g_pMainCameraDepthStencilTexture );

	// clear if first render of this cycle
	if ( g_bFirstRenderClearsRenderTarget==false )
	{
		// R=PROG-Z / G=UNPROG-Z MOTION / B=UNPROG-Z DOF / A=SPARE
		D3DCOLOR color = D3DCOLOR_RGBA(255, 255, 0, 0);
		m_pD3D->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
		g_bFirstRenderClearsRenderTarget = true;
	}

	// always success for now
	return 1;
}

bool CObjectManager::DrawInstanceStampObject ( sObject* pObject )
{
	// first, update instance stamp object based on ref map and camera position
	UpdateInstanceStampObject(pObject);

	// set camera frustum to current view and projection matrices
	DWORD dwCameraIndex = g_pGlob->dwRenderCameraID; // 31,32,33,34 are shadow camera cascade 0,1,2,3
	if ( dwCameraIndex > 34 ) dwCameraIndex = 34;
	SetupFrustum ( 0.0f );

	// world matrix position for this object render
	pObject->collision.fScaledLargestRadius = -1;

	// access reference map
	sInstanceStamp* pStampPtr = (sInstanceStamp*)pObject->pCustomData;
	if ( pStampPtr==NULL ) return true;

	// get camera for distance calculation and calculate distances to all buffers
	tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( dwCameraIndex );
	D3DXVECTOR3 vecCurrentCameraPosition;
	D3DXMATRIX matCurrentCameraProjection;
	if ( dwCameraIndex>=31 )
	{
		// shadow camera position
		vecCurrentCameraPosition.x = g_CascadedShadow.m_pLightCamera->GetWorldMatrix()->_41;
		vecCurrentCameraPosition.x = g_CascadedShadow.m_pLightCamera->GetWorldMatrix()->_42;
		vecCurrentCameraPosition.x = g_CascadedShadow.m_pLightCamera->GetWorldMatrix()->_43;
		memcpy ( &matCurrentCameraProjection, &g_CascadedShadow.m_matShadowView[dwCameraIndex-31], sizeof(D3DXMATRIX) );
	}
	else
	{
		// regular camera position and projection matrix
		vecCurrentCameraPosition = m_Camera_Ptr->vecPosition;
		matCurrentCameraProjection = m_Camera_Ptr->matProjection;
	}

	// calculate distances and angles
	//for ( int lod=0; lod<INSTANCESTAMPLODMAX; lod++ ) // only using QUAD level for now
	for ( int lod=2; lod<INSTANCESTAMPLODMAX; lod++ )
	{
		// the list of buffers to draw in this level of detail
		sInstanceStampBuffer* pActiveBufferList = pStampPtr->pActiveBufferList[lod];
		if ( dwCameraIndex==0 )
		{
			while ( pActiveBufferList )
			{
				// calculate distance every now and again (can skip this calc until we move an appreciable distance globally)
				float fDX = pActiveBufferList->vecCentre.x - vecCurrentCameraPosition.x;
				float fDY = pActiveBufferList->vecCentre.y - vecCurrentCameraPosition.y;
				float fDZ = pActiveBufferList->vecCentre.z - vecCurrentCameraPosition.z;
				float fDD = sqrt ( fabs(fDX*fDX)+fabs(fDY*fDY)+fabs(fDZ*fDZ) );
				pActiveBufferList->fDistanceFromCamera[dwCameraIndex] = fDD;
				pActiveBufferList->fAngleFromCamera[dwCameraIndex] = D3DXToDegree(fabs(atan2(fDZ,fDX)));
				pActiveBufferList = pActiveBufferList->pActiveListNext;
			}
		}
	}

	// render any quad textures if required
	if ( dwCameraIndex==0 ) // only render quad textures and assign quad verts for camera zero for now
	{
		// NOTE: We are going to need vertex buffers for EACH CAMERA as quad verts change from view to view!
		int lod = 2;
		int iEarlyQuadRenderExit = 1;
		sInstanceStampBuffer* pActiveBufferList = pStampPtr->pActiveBufferList[lod];
		while ( pActiveBufferList )
		{
			// trigger quad texture to regenerate
			bool bRegenerate = false;
			float fAngleDifference = fabs(pActiveBufferList->fAngleFromCamera[dwCameraIndex] - pActiveBufferList->fQuadTextureLastAngleFromCamera[dwCameraIndex]);
			if ( fAngleDifference > 180 ) fAngleDifference = 360.0f - fAngleDifference;
			if ( fabs(pActiveBufferList->fQuadTextureLastHeightFromCamera[dwCameraIndex]-vecCurrentCameraPosition.y)>100.0f ) fAngleDifference=360;
			//if ( pActiveBufferList->fDistanceFromCamera[dwCameraIndex]>=2300.0f )
			if ( 1 )
			{
				if ( (fAngleDifference>1.0f && pActiveBufferList->bQuadTextureGenerated[dwCameraIndex]==true) || (pActiveBufferList->bQuadTextureGenerated[dwCameraIndex]==false) )
				{
					// record state when regeneration happened, then trigger a quad texture render
					pActiveBufferList->bQuadTextureGenerated[dwCameraIndex] = true;
					pActiveBufferList->fQuadTextureLastAngleFromCamera[dwCameraIndex] = pActiveBufferList->fAngleFromCamera[dwCameraIndex];
					pActiveBufferList->fQuadTextureLastHeightFromCamera[dwCameraIndex] = vecCurrentCameraPosition.y;
					bRegenerate = true;
				}
			}

			// keep locked vertex buffer ptr global so we can keep it open for more writes, and close at end (faster than multi-locks)
			LPSTR pfLockedData = NULL;

			// select texture size based on number of quad textures we will be rendering
			int RenderTextureWidth = 1024;
			int RenderTextureHeight = 1024;
			if ( pActiveBufferList->iQuadTexIndexCount==1 )
			{
				RenderTextureWidth = 256;
				RenderTextureHeight = 256;
			}
			else
			{
				if ( pActiveBufferList->iQuadTexIndexCount<=4 )
				{
					RenderTextureWidth = 512;
					RenderTextureHeight = 512;
				}
			}

			// always process quad textures, but only regenerate texture using above flag
			// without 'bRegenerate==true' here, the vertex locks REALLY SLOW DOWN lots of buffers!
			// but is OKAY when there are just a few objects, so maybe there is something in-between we can use?
			if ( bRegenerate==true )
			{
				// create communal texture
				if ( pActiveBufferList->d3dTex[dwCameraIndex]==NULL )
				{
					// create render target
					D3DXCreateTexture( m_pD3D, RenderTextureWidth, RenderTextureHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pActiveBufferList->d3dTex[dwCameraIndex] );
				}
				if ( pActiveBufferList->d3dTex[dwCameraIndex]!=NULL )
				{
					// create common depth surface for these render operations
					if ( m_pImposterRendererDSV==NULL )
					{
						D3DSURFACE_DESC backbufferdesc;
						g_pGlob->pHoldBackBufferPtr->GetDesc(&backbufferdesc);
						D3DFORMAT CommonFormat = backbufferdesc.Format;

						// does depth surface have to be same size as render target?
						D3DFORMAT DepthFormat = GetValidStencilBufferFormat(CommonFormat);
						HRESULT hRes = m_pD3D->CreateDepthStencilSurface(	2048, 2048,
																			DepthFormat, D3DMULTISAMPLE_NONE, 0, TRUE,
																			&m_pImposterRendererDSV, NULL );
					}

					// store 
					D3DXMATRIX matSaveW;
					D3DXMATRIX matSaveV;
					D3DXMATRIX matSaveP;
					D3DVIEWPORT9 vpSaved;
					if ( bRegenerate==true )
					{
						m_pD3D->GetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &matSaveW );
						m_pD3D->GetTransform ( D3DTS_VIEW, ( D3DXMATRIX* ) &matSaveV );
						m_pD3D->GetTransform ( D3DTS_PROJECTION, ( D3DXMATRIX* ) &matSaveP );
						m_pD3D->GetViewport ( &vpSaved );
					}

					// record render targets before we steal if for a bit
					IDirect3DSurface9* pCurrentRenderTarget = NULL;
					IDirect3DSurface9* pCurrentDepthTarget = NULL;
					if ( bRegenerate==true )
					{
						m_pD3D->GetRenderTarget( 0, &pCurrentRenderTarget );
						m_pD3D->GetDepthStencilSurface( &pCurrentDepthTarget );
					}

					// set render target into communual texture
					IDirect3DSurface9* pSurface = NULL;
					pActiveBufferList->d3dTex[dwCameraIndex]->GetSurfaceLevel ( 0, &pSurface );
					if ( pSurface )
					{
						if ( bRegenerate==true )
						{
							// render to this
							m_pD3D->SetRenderTarget ( 0, pSurface );
							m_pD3D->SetDepthStencilSurface ( m_pImposterRendererDSV );

							// Clear the texture.
							D3DCOLOR color = D3DCOLOR_RGBA(32, 32, 32, 0);
							m_pD3D->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0);
						}
						pSurface->Release();

						// all objects in this batch
						sInstanceStampTileData* pItem = pActiveBufferList->pFirstLOD2Item;
						while ( pItem )
						{
							// reposition object for this render
							sObject* pParentObject = g_ObjectList [ pItem->dwParentObjNumber ];
							if ( pParentObject )
							{
								// create imposter for this item (if not exist)
								if ( pItem->pImposter[dwCameraIndex]==NULL )
								{
									pItem->pImposter[dwCameraIndex] = new sImposter;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[0].x = pParentObject->collision.vecMin.x;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[0].y = pParentObject->collision.vecMin.y;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[0].z = pParentObject->collision.vecMin.z;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[1].x = pParentObject->collision.vecMax.x;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[1].y = pParentObject->collision.vecMin.y;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[1].z = pParentObject->collision.vecMin.z;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[2].x = pParentObject->collision.vecMin.x;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[2].y = pParentObject->collision.vecMax.y;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[2].z = pParentObject->collision.vecMin.z;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[3].x = pParentObject->collision.vecMax.x;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[3].y = pParentObject->collision.vecMax.y;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[3].z = pParentObject->collision.vecMin.z;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[4].x = pParentObject->collision.vecMin.x;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[4].y = pParentObject->collision.vecMin.y;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[4].z = pParentObject->collision.vecMax.z;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[5].x = pParentObject->collision.vecMax.x;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[5].y = pParentObject->collision.vecMin.y;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[5].z = pParentObject->collision.vecMax.z;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[6].x = pParentObject->collision.vecMin.x;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[6].y = pParentObject->collision.vecMax.y;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[6].z = pParentObject->collision.vecMax.z;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[7].x = pParentObject->collision.vecMax.x;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[7].y = pParentObject->collision.vecMax.y;
									pItem->pImposter[dwCameraIndex]->boundingBoxVerts[7].z = pParentObject->collision.vecMax.z;
									pItem->pImposter[dwCameraIndex]->radius = pParentObject->collision.fRadius;
									pItem->pImposter[dwCameraIndex]->uvOffset = D3DXVECTOR2(0,0);
									pItem->pImposter[dwCameraIndex]->pAdjustUV = false;
								}

								//-- Camera point and place(each imposter has a radius value so we can be sure we move back far enough
								//dbPointCamera(this->cameraID_, obj->position.vecPosition.x, obj->position.vecPosition.y, obj->position.vecPosition.z);
								float fX = pItem->fPosX;
								float fY = pItem->fPosY;
								float fZ = pItem->fPosZ;
								float fXRotate, fYRotate, fZRotate;
								D3DXVECTOR3 vecCamPos = vecCurrentCameraPosition;
								GetAngleFromPoint ( vecCamPos.x, vecCamPos.y, vecCamPos.z, fX, fY, fZ, &fXRotate, &fYRotate, &fZRotate);
								D3DXMATRIX matRot, matTempRot, matCamView;
								D3DXMatrixRotationX ( &matRot, D3DXToRadian ( fXRotate ) );
								D3DXMatrixRotationY ( &matTempRot, D3DXToRadian ( fYRotate ) );
								D3DXMatrixMultiply ( &matRot, &matRot, &matTempRot );
								D3DXMatrixRotationZ ( &matTempRot, D3DXToRadian ( fZRotate ) );
								D3DXMatrixMultiply ( &matRot, &matRot, &matTempRot );
								D3DXVECTOR3 vecRight, vecUp, vecLook;
								vecRight.x=1;
								vecRight.y=0;
								vecRight.z=0;
								vecUp.x=0;
								vecUp.y=1;
								vecUp.z=0;
								vecLook.x=0;
								vecLook.y=0;
								vecLook.z=1;
								D3DXVec3TransformCoord ( &vecRight,	&vecRight,	&matRot );
								D3DXVec3TransformCoord ( &vecUp,	&vecUp,		&matRot );
								D3DXVec3TransformCoord ( &vecLook,	&vecLook,	&matRot );
								D3DXMatrixIdentity ( &matCamView );
								matCamView._11 = vecRight.x; 
								matCamView._12 = vecUp.x; 
								matCamView._13 = vecLook.x;
								matCamView._21 = vecRight.y; 
								matCamView._22 = vecUp.y; 
								matCamView._23 = vecLook.y;
								matCamView._31 = vecRight.z;
								matCamView._32 = vecUp.z; 
								matCamView._33 = vecLook.z;
								matCamView._41 =- D3DXVec3Dot ( &vecCamPos, &vecRight );
								matCamView._42 =- D3DXVec3Dot ( &vecCamPos, &vecUp    );
								matCamView._43 =- D3DXVec3Dot ( &vecCamPos, &vecLook  );
								//dbPositionCamera(this->cameraID_, obj->position.vecPosition.x, obj->position.vecPosition.y, obj->position.vecPosition.z);
								vecCamPos.x = pItem->fPosX;
								vecCamPos.y = pItem->fPosY;
								vecCamPos.z = pItem->fPosZ;
    							//dbMoveCamera(this->cameraID_, -pImposter->radius * 10);
								vecCamPos += ( -pItem->pImposter[dwCameraIndex]->radius * 10 * vecLook );

								// get matrices from above repositioning
								D3DXMATRIX projMatrix(matCurrentCameraProjection);
								D3DXMATRIX viewMatrix(matCamView);
								D3DXVECTOR3 vecLastObjPosition = pParentObject->position.vecPosition;
								D3DXVECTOR3 vecLastObjRotation = pParentObject->position.vecRotate;
								D3DXVECTOR3 vecLastObjScale = pParentObject->position.vecScale;
								pParentObject->position.vecPosition = D3DXVECTOR3 ( pItem->fPosX, pItem->fPosY, pItem->fPosZ );
								pParentObject->position.vecRotate = D3DXVECTOR3 ( D3DXToDegree(pItem->fRotX), D3DXToDegree(pItem->fRotY), D3DXToDegree(pItem->fRotZ) );
								//pParentObject->position.dwRotationOrder = ROTORDER_ZYX;
								pParentObject->position.vecScale = D3DXVECTOR3 ( pItem->fScaleX, pItem->fScaleY, pItem->fScaleZ );
								CalcObjectWorld ( pParentObject );
								D3DXMATRIX wMat = pParentObject->position.matWorld;

								// Create the imposter geometry, view and projection matrices.
								float nearPlane, farPlane;
    							createImposterBillboard(vecCamPos, viewMatrix, projMatrix, wMat, pItem->pImposter[dwCameraIndex]->boundingBoxVerts, pItem->pImposter[dwCameraIndex]->radius, pItem->pImposter[dwCameraIndex], nearPlane, farPlane);
								D3DXMATRIX imposterViewMatrix, imposterProjMatrix;
								createMatrices(vecCamPos, vecUp, imposterViewMatrix, imposterProjMatrix, pItem->pImposter[dwCameraIndex], nearPlane, farPlane);

								// create quad UV coordinate from iQuadTexIndex
								float fDiv = 1.0f+((int)sqrt((double)pActiveBufferList->iQuadTexIndexCount));
								int iQuadTexI = pItem->iQuadTexIndex[dwCameraIndex];
								int iQuadTexYY = iQuadTexI / fDiv;
								if ( iQuadTexYY > (fDiv-1) ) iQuadTexYY=(fDiv-1);
								int iQuadTexXX = iQuadTexI-(iQuadTexYY*fDiv);
								float fU = (1.0f / fDiv) * iQuadTexXX;
								float fV = (1.0f / fDiv) * iQuadTexYY;
								pItem->pImposter[dwCameraIndex]->uvOffset = D3DXVECTOR2(fU,fV);

								// write imposter geometry into dynamic vertex buffer
								if ( pfLockedData==NULL ) pActiveBufferList->pVBRef->Lock ( 0, 0, ( VOID** ) &pfLockedData, 0 );
								if ( pfLockedData )
								{
									// transfer imposter vert data to buffer
									float* pDestPtrAt = (float*)(pfLockedData+pItem->dwImposterOffsetIntoBuffer[dwCameraIndex]);
									DWORD dwSizeInFloats = pActiveBufferList->pMeshFromParent->dwFVFSize/4;
									for ( DWORD v=0; v<pActiveBufferList->pMeshFromParent->dwVertexCount; v++ )
									{
										int iv = 0;
										if ( v==0 ) iv = 0;
										if ( v==1 ) iv = 1;
										if ( v==2 ) iv = 4;
										if ( v==3 ) iv = 4;
										if ( v==4 ) iv = 5;
										if ( v==5 ) iv = 0;
										D3DXVECTOR3 vec = pItem->pImposter[dwCameraIndex]->centre;
										D3DXVECTOR3 norm = pItem->pImposter[dwCameraIndex]->verts[iv].pos - pItem->pImposter[dwCameraIndex]->centre;
										*(D3DXVECTOR3*)pDestPtrAt = vec;
										*(D3DXVECTOR3*)(pDestPtrAt+3) = norm;
										if ( pItem->pImposter[dwCameraIndex]->pAdjustUV==false )
										{
											// only need to update UV coordinates once when texture divided up
											float fOrigU = *(float*)(pDestPtrAt+6);
											float fOrigV = *(float*)(pDestPtrAt+7);
											*(float*)(pDestPtrAt+6) = fU + (fOrigU/fDiv);
											*(float*)(pDestPtrAt+7) = fV + (fOrigV/fDiv);							
										}
										pDestPtrAt+=dwSizeInFloats;
									}
									pItem->pImposter[dwCameraIndex]->pAdjustUV = true;
								}

								if ( bRegenerate==true )
								{
									// Set viewport for rendering to the sub-region of the render texture.
									D3DXVECTOR2 imposterUVScale(1.0f/fDiv,1.0f/fDiv);
									initImposterViewport ( RenderTextureWidth, RenderTextureHeight, pItem->pImposter[dwCameraIndex]->uvOffset, imposterUVScale );

									// Initialise DirectX transforms.
									m_pD3D->SetTransform(D3DTS_VIEW, &imposterViewMatrix);
									m_pD3D->SetTransform(D3DTS_PROJECTION, &imposterProjMatrix);

									// Set the world transform for the Mesh.
									m_pD3D->SetTransform(D3DTS_WORLD, &wMat);

									// Render the mesh to the imposter
									if ( pParentObject->ppMeshList[0]->pVertexShaderEffect )
									{
										cSpecialEffect* pEff = pParentObject->ppMeshList[0]->pVertexShaderEffect;
										//D3DXHANDLE hClippingOnState = pEff->m_pEffect->GetParameterByName ( NULL, "m_fClippingOnState" );
										D3DXHANDLE hShadowStrength = pEff->m_pEffect->GetParameterByName ( NULL, "ShadowStrength" );
										float fOriginalShadowStrength;
										pEff->m_pEffect->GetFloat ( hShadowStrength, &fOriginalShadowStrength );
										//pEff->m_pEffect->SetFloat ( hClippingOnState, 1 );
										pEff->m_pEffect->SetFloat ( hShadowStrength, 0 );
										DrawObjectEx ( pParentObject );
										//pEff->m_pEffect->SetFloat ( hClippingOnState, -1 );
										pEff->m_pEffect->SetFloat ( hShadowStrength, fOriginalShadowStrength );
									}
								}

								// restore
								pParentObject->position.vecPosition = vecLastObjPosition;
								pParentObject->position.vecRotate = vecLastObjRotation;
								pParentObject->position.vecScale = vecLastObjScale;
								//pParentObject->position.dwRotationOrder = ROTORDER_XYZ;
								CalcObjectWorld ( pParentObject );
							}
							pItem = pItem->pLOD2BufferCollectionNext;
						}

						// restore render targets
						if ( bRegenerate==true )
						{
							m_pD3D->SetRenderTarget( 0, pCurrentRenderTarget );
							m_pD3D->SetDepthStencilSurface ( pCurrentDepthTarget );
						}
					}

					// restore
					if ( bRegenerate==true )
					{
						m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &matSaveW );
						m_pD3D->SetTransform ( D3DTS_VIEW, ( D3DXMATRIX* ) &matSaveV );
						m_pD3D->SetTransform ( D3DTS_PROJECTION, ( D3DXMATRIX* ) &matSaveP );
						m_pD3D->SetViewport ( &vpSaved );
					}
				}

				// unlock if previously locked for writing
				if ( pfLockedData )
				{
					pActiveBufferList->pVBRef->Unlock();
					pfLockedData = NULL;
				}
			}

			// next buffer
			pActiveBufferList = pActiveBufferList->pActiveListNext;

			// early exit code
			if ( bRegenerate==true )
			{
				iEarlyQuadRenderExit--;
				if ( iEarlyQuadRenderExit<=0 )
					pActiveBufferList = NULL;
			}
		}
	}

	// render all my buffers
	D3DXMATRIX matOrig;
	m_pD3D->GetTransform ( D3DTS_WORLD, &matOrig );
	//for ( int lod=0; lod<INSTANCESTAMPLODMAX; lod++ ) // only render QUAD layer for now
	for ( int lod=2; lod<INSTANCESTAMPLODMAX; lod++ )
	{
		// HACK - mutate LOD0 so I can see them!
		if ( 0 )
		{
			if ( lod==0 )
			{
				D3DXMATRIX matObject;
				matObject = matOrig;
				matObject._41 += -20 + rand()%40;
				matObject._43 += -20 + rand()%40;
				m_pD3D->SetTransform ( D3DTS_WORLD, &matObject );
			}
			else
			{
				m_pD3D->SetTransform ( D3DTS_WORLD, &matOrig );
			}
		}

		// only allow LOD2 if camera zero
		if ( lod==2 && dwCameraIndex!=0 )
			continue;

		// sort the buffer list into front to back order (reduces overdraw)
		sInstanceStampBuffer* pActiveBufferLastInList = NULL;
		sInstanceStampBuffer* pActiveBufferList = pStampPtr->pActiveBufferList[lod];
		while ( pActiveBufferList )
		{
			// next in list
			sInstanceStampBuffer* pActiveBufferNextInList = pActiveBufferList->pActiveListNext;

			// is THIS buffer FARTHER than NEXT in list?
			if ( pActiveBufferNextInList && pActiveBufferList->fDistanceFromCamera[0] > pActiveBufferNextInList->fDistanceFromCamera[0] )
			{
				// yes, so switch them around so nearer buffers are drawn first
				if ( pActiveBufferLastInList==NULL )
				{
					// adjust main list ptr if first item in list
					pStampPtr->pActiveBufferList[lod] = pActiveBufferNextInList;
					sInstanceStampBuffer* pThirdOne = pActiveBufferNextInList->pActiveListNext;
					pActiveBufferNextInList->pActiveListNext = pActiveBufferList;
					pActiveBufferList->pActiveListNext = pThirdOne;
					pActiveBufferNextInList = pActiveBufferList;
					pActiveBufferList = pStampPtr->pActiveBufferList[lod];
				}
				else
				{
					// can simply swap list ptrs
					pActiveBufferLastInList->pActiveListNext = pActiveBufferNextInList;
					sInstanceStampBuffer* pThirdOne = pActiveBufferNextInList->pActiveListNext;
					pActiveBufferNextInList->pActiveListNext = pActiveBufferList;
					pActiveBufferList->pActiveListNext = pThirdOne;
					pActiveBufferNextInList = pActiveBufferList;
					pActiveBufferList = pActiveBufferLastInList->pActiveListNext;
				}
			}

			// move to next buffer in active list
			pActiveBufferLastInList = pActiveBufferList;
			pActiveBufferList = pActiveBufferNextInList;
		}

		// go through all buffers and render them (respecting occlusion and frustum)
		pActiveBufferList = pStampPtr->pActiveBufferList[lod];
		while ( pActiveBufferList )
		{
			// get this buffer ptr
			sInstanceStampBuffer* pStampBufferPtr = pActiveBufferList;

			// if low LOD entirely beyond QUAD imposters, can hide it right away
			bool bHideIfBeyondMaxDistance = false;
			if ( lod==0 )
				if ( pActiveBufferList->fDistanceFromCamera[0] > 4000.0f )
					bHideIfBeyondMaxDistance = true;
			if ( lod==1 )
				if ( pActiveBufferList->fDistanceFromCamera[0] > 5000.0f )
					bHideIfBeyondMaxDistance = true;

			// is buffer active
			if ( pStampBufferPtr && bHideIfBeyondMaxDistance==false )
			{
				// if polygons to render
				if ( pStampBufferPtr->dwPrimitiveCount > 0 && pStampBufferPtr->bVisible==true )
				{
					// is this buffer within the view frustum
					if ( CheckRectangleEx ( 0, pStampBufferPtr->vecCentre.x, pStampBufferPtr->vecCentre.y, pStampBufferPtr->vecCentre.z, pStampBufferPtr->vecSize.x, pStampBufferPtr->vecSize.y, pStampBufferPtr->vecSize.z )==false )
					{
						// outside of frustrum, but should NOT be occluded in this state (or they flicker-disappear when swing back into view)
						//pStampBufferPtr->bOccluded[dwCameraIndex] = false;
					}
					else
					{
						// do not use instance stamp occlusion - it had the 'one frame out' flaw
						//pStampBufferPtr->bOccluded[dwCameraIndex] = true;

						/*
						// start query
						bool bRealRenderHappened = false;
						// and only beyond specific range (fZeroOcclusionRange)
						float fZeroOcclusionRange = 200.0f;
						if ( dwCameraIndex==0 && pActiveBufferList->fDistanceFromCamera[0]>fZeroOcclusionRange )
						{
							if ( pStampBufferPtr->d3dQuery[dwCameraIndex] && pStampBufferPtr->iQueryBusyStage[dwCameraIndex]==0 )
							{
								pStampBufferPtr->d3dQuery[dwCameraIndex]->Issue( D3DISSUE_BEGIN );
								pStampBufferPtr->iQueryBusyStage[dwCameraIndex] = 1;
								if ( pStampBufferPtr->bOccluded[dwCameraIndex]==true )
								{
									// only render to depth for test (performance friendly render)
									m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, FALSE );
									m_pD3D->SetRenderState ( D3DRS_COLORWRITEENABLE, 0 );
									m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );

									// NOTE: Could render a camera facing quad stretched wide?

									// render boundbox
									m_pD3D->SetStreamSource ( 0, g_pStampBufferVBRef, 0, g_dwStampBufferFVFSize ); 
									m_pD3D->SetIndices ( NULL );
									m_pD3D->SetVertexShader ( NULL );
									m_pD3D->SetTexture ( 0, NULL );
									D3DXMATRIX matBoundBox, matBoundBoxSize;
									D3DXMatrixIdentity(&matBoundBoxSize);
									float fExtraMargin = 15.0f;
									D3DXMatrixScaling(&matBoundBoxSize,pStampBufferPtr->vecSize.x*fExtraMargin,pStampBufferPtr->vecSize.y*fExtraMargin,pStampBufferPtr->vecSize.z*fExtraMargin);
									D3DXMatrixTranslation(&matBoundBox,pStampBufferPtr->vecCentre.x,pStampBufferPtr->vecCentre.y,pStampBufferPtr->vecCentre.z);
									D3DXMatrixMultiply(&matBoundBox,&matBoundBoxSize,&matBoundBox);
									m_pD3D->SetTransform ( D3DTS_WORLD, &matBoundBox );
									m_pD3D->DrawPrimitive (	D3DPT_TRIANGLELIST, 0, 12 );
									m_pD3D->SetTransform ( D3DTS_WORLD, &matOrig );

									// counter the count as it's not a real rendered obe (just a depth one)
									//if ( g_pGlob ) g_pGlob->dwNumberOfPolygonsDrawn -= pMesh->pDrawBuffer->dwPrimitiveCount;
									//if ( g_pGlob ) g_pGlob->dwNumberOfPrimCalls--;

									// only render to depth for test (performance friendly render) - restore 
									m_pD3D->SetRenderState ( D3DRS_COLORWRITEENABLE, 0x0000000F );
									m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, TRUE );
									m_pD3D->SetRenderState ( D3DRS_CULLMODE, m_RenderStates.dwCullDirection );

									// flag as having rendered something for query check
									bRealRenderHappened = true;
								}
							}
						}
						else
						{
							// only occlusion for main camera, leave shadows, relfection, e.t.c cameras as is
							pStampBufferPtr->bOccluded[dwCameraIndex]=false;
						}
						*/

						// if previously occluded, keep invisible
						if ( 1 )  //pStampBufferPtr->bOccluded[dwCameraIndex]==false || pStampBufferPtr->iOccludedCounter[dwCameraIndex]>0 )
						{
							// draw this buffer now!

							// set render states
							if ( !SetMeshRenderStates ( pStampBufferPtr->pMeshFromParent ) )
								return false;

							// set VB and IB stream sources
							if ( pStampBufferPtr->pVBRef )
							{
								if ( FAILED ( m_pD3D->SetStreamSource ( 0,
																		pStampBufferPtr->pVBRef,
																		0, 
																		pStampBufferPtr->pMeshFromParent->dwFVFSize				 ) ) ) 
									return false;
							}

							if ( pStampBufferPtr->pIBRef )
							{
								if ( FAILED ( m_pD3D->SetIndices ( pStampBufferPtr->pIBRef ) ) ) 
									return false;
							}

							// parent mesh to get texture and shader from
							sMesh* pMesh = pStampBufferPtr->pMeshFromParent;

							// ensure depth clipping is used for all static buffers
							D3DXHANDLE hClippingOnState = NULL;
							cSpecialEffect* pEff = pMesh->pVertexShaderEffect;
							if ( pEff )
							{
								hClippingOnState = pEff->m_pEffect->GetParameterByName ( NULL, "m_fClippingOnState" );
								pEff->m_pEffect->SetFloat ( hClippingOnState, -1 );
							}

							// custom shader or Fixed-Function\FX-Effect 
							if ( !SetVertexShader ( pStampBufferPtr->pMeshFromParent ) ) return false;

							// start shader
							UINT uPasses = 1;
							bool bEffectRendering = false;
							IDirect3DSurface9 *pCurrentRenderTarget = NULL;
							IDirect3DSurface9 *pCurrentDepthTarget = NULL;
							bool bLocalOverrideAllTexturesAndEffects = false;
							ShaderStart ( pMesh, &pCurrentRenderTarget, &pCurrentDepthTarget, &uPasses, &bEffectRendering, &bLocalOverrideAllTexturesAndEffects );

							// loop through all shader passes
							// each mesh can have several render passes
							for(UINT uPass = 0; uPass < uPasses; uPass++)
							{
								// start shader pass
								if ( ShaderPass ( pMesh, uPass, uPasses, bEffectRendering, bLocalOverrideAllTexturesAndEffects, pCurrentRenderTarget, pCurrentDepthTarget )==true )
								{
									// override quad texture if using dynamic render target from true-imposter system
									IDirect3DTexture9* pQuadTex = pStampBufferPtr->d3dTex[dwCameraIndex];
									if ( pQuadTex )
									{
										m_pD3D->SetTexture ( 0, pQuadTex );
									}

									// draw polygons
									if ( pStampBufferPtr->pMeshFromParent->pIndices )
									{
										// draw an indexed primitive
										HRESULT hDrawSuccess = m_pD3D->DrawIndexedPrimitive (	pStampBufferPtr->dwPrimType,
																								pStampBufferPtr->dwBaseVertexIndex,
																								pStampBufferPtr->dwVertexStart,
																								pStampBufferPtr->dwVertexCount,
																								pStampBufferPtr->dwIndexStart,
																								pStampBufferPtr->dwPrimitiveCount		);

										if ( g_pGlob ) g_pGlob->dwNumberOfPolygonsDrawn += pStampBufferPtr->dwPrimitiveCount;
										if ( g_pGlob ) g_pGlob->dwNumberOfPrimCalls++;
										//bRealRenderHappened = true;
									}
									else
									{
										HRESULT hDrawSuccess = m_pD3D->DrawPrimitive (	pStampBufferPtr->dwPrimType, 0, pStampBufferPtr->dwPrimitiveCount );
										if ( g_pGlob ) g_pGlob->dwNumberOfPolygonsDrawn += pStampBufferPtr->dwPrimitiveCount;
										if ( g_pGlob ) g_pGlob->dwNumberOfPrimCalls++;
										//bRealRenderHappened = true;
									}

									// end shader pass
									ShaderPassEnd ( pMesh, bEffectRendering );
								}
							}

							// finish shader
							ShaderFinish ( pMesh, pCurrentRenderTarget, pCurrentDepthTarget );

							// restore shader to normal operation
							if ( pEff ) pEff->m_pEffect->SetFloat ( hClippingOnState, 1 );
						}

						/* old occlusion system not used any more
						// end query
						if ( dwCameraIndex==0 )
						{
							if ( pStampBufferPtr->d3dQuery[dwCameraIndex] && pStampBufferPtr->iQueryBusyStage[dwCameraIndex]==1 )
							{
								pStampBufferPtr->d3dQuery[dwCameraIndex]->Issue( D3DISSUE_END );
								if ( bRealRenderHappened==false )
								{
									// cancel query now, no render at all between issue phase means corrupt result!
									pStampBufferPtr->iQueryBusyStage[dwCameraIndex] = 0;
								}
								else
								{
									pStampBufferPtr->iQueryBusyStage[dwCameraIndex] = 2;
								}
							}
							if ( pStampBufferPtr->d3dQuery[dwCameraIndex] && pStampBufferPtr->iQueryBusyStage[dwCameraIndex]==2 )
							{
								DWORD pixelsVisible = 0;
								HRESULT dwResult = pStampBufferPtr->d3dQuery[dwCameraIndex]->GetData((void *)&pixelsVisible, sizeof(DWORD), 0);
								if ( dwResult==S_OK )
								{
									// get pixels result
									pStampBufferPtr->dwQueryValue[dwCameraIndex] = pixelsVisible;
									pStampBufferPtr->iQueryBusyStage[dwCameraIndex] = 0;

									// pixels produced, so refresh our counter
									if ( pixelsVisible>0 ) 
										pStampBufferPtr->iOccludedCounter[dwCameraIndex] = 5;
									else
										if ( pStampBufferPtr->iOccludedCounter[dwCameraIndex] > 0 )
											pStampBufferPtr->iOccludedCounter[dwCameraIndex]--;

									// if producing pixels, must be visible
									if ( pStampBufferPtr->dwQueryValue[dwCameraIndex]>0 )
										pStampBufferPtr->bOccluded[dwCameraIndex] = false;
									else
										pStampBufferPtr->bOccluded[dwCameraIndex] = true;
								}
								else
								{
									if ( dwResult!=S_FALSE )
									{
										// cancel whole thing if error returned
										pStampBufferPtr->iQueryBusyStage[dwCameraIndex] = 0;
									}
								}
							}
						}
						*/

						//
						// render a bound box to see the encompassing buffer
						//
						if ( g_pStampBufferVBRef && 0 )
						{
							m_pD3D->SetStreamSource ( 0, g_pStampBufferVBRef, 0, g_dwStampBufferFVFSize ); 
							m_pD3D->SetIndices ( NULL );
							m_pD3D->SetVertexShader ( NULL );
							m_pD3D->SetTexture ( 0, NULL );

							// set boundbox world matrix
							D3DXMATRIX matBoundBox, matBoundBoxSize;
							D3DXMatrixIdentity(&matBoundBoxSize);
							D3DXMatrixScaling(&matBoundBoxSize,pStampBufferPtr->vecSize.x,pStampBufferPtr->vecSize.y,pStampBufferPtr->vecSize.z);
							D3DXMatrixTranslation(&matBoundBox,pStampBufferPtr->vecCentre.x,pStampBufferPtr->vecCentre.y,pStampBufferPtr->vecCentre.z);
							D3DXMatrixMultiply(&matBoundBox,&matBoundBoxSize,&matBoundBox);
							m_pD3D->SetTransform ( D3DTS_WORLD, &matBoundBox );

							// draw bound box
							m_pD3D->DrawPrimitive (	D3DPT_TRIANGLELIST, 0, 12 );

							// restore world matrix
							m_pD3D->SetTransform ( D3DTS_WORLD, &matOrig );
						}
					}
				}
			}

			// move to next bufer in active list
			pActiveBufferList = pActiveBufferList->pActiveListNext;
		}
	}

	// complete
	return true;
}

bool CObjectManager::DrawObjectEx ( sObject* pObject )
{
	return DrawObject ( pObject, false );
}

bool CObjectManager::DrawObject ( sObject* pObject, bool bFrustrumCullMeshes )
{
	// each object resets this (instance object can set it)
	g_InstanceAlphaControlValue = 0;

	// check the object and array index value
	SAFE_MEMORY ( pObject );
	
	// if resource destroyed, quit now
	if ( pObject->dwObjectNumber > 0 )
		if ( g_ObjectList [ pObject->dwObjectNumber ]==NULL )
			return true;

	// skip if object is designated as invisible
	if ( pObject->bVisible==false
	||   pObject->bNoMeshesInObject==true )
		return true;

	// setup the world matrix for the object
	CalcObjectWorld ( pObject );

	// Simple hardware occlusion to determine on-screen visibility (in rendered pixels)
	bool bOcclusionRenderHappened = false;
	if ( g_Occlusion.d3dQuery[pObject->dwObjectNumber]!=NULL )
	{
		if ( g_Occlusion.GetOcclusionMode()==1 )
		{
			if ( g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber]==99 )
			{
				// 99 = must be triggered by GET OBJECT OCCLUSION (not every cycle = too expensive)
				g_Occlusion.d3dQuery[pObject->dwObjectNumber]->Issue( D3DISSUE_BEGIN );
				g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber] = 1;
			}
		}
	}

	// only render if universe believes it visible (or rendering for shadow camera)
	if ( pObject->bUniverseVisible==true || g_pGlob->dwRenderCameraID>=31 )
	{
		// Occlusion Render Happened
		bOcclusionRenderHappened = true;

		// for linked objects
		D3DXMATRIX matSavedWorld;
		D3DXMATRIX matNewWorld;

		// external DLLs can disable transform it they want
		if ( !pObject->bDisableTransform )
		{
			m_pD3D->SetTransform ( D3DTS_WORLD, &pObject->position.matWorld );
		}
		else
		{
			// World Transform
			D3DXMATRIX	matTranslation, matScale, matObject;
			D3DXMatrixTranslation ( &matTranslation, 0.0f, 0.0f, 0.0f );
			D3DXMatrixScaling     ( &matScale,       1.0f, 1.0f, 1.0f );
			matObject = matScale * matTranslation;
			m_pD3D->SetTransform ( D3DTS_WORLD, &matObject );
		}

		// an object can control its own FOV for rendering
		if ( pObject->bLockedObject==false )
		{
			if ( pObject->fFOV != m_RenderStates.fObjectFOV )
			{
				if ( pObject->fFOV == 0.0f )
					g_Camera3D_SetFOV ( m_RenderStates.fStoreCameraFOV );
				else
					g_Camera3D_SetFOV ( pObject->fFOV );

				m_RenderStates.fObjectFOV = pObject->fFOV;
			}
		}

		// if instance stamp object, use new rendering sequence
		if ( pObject->dwCustomSize==4000000001 )
		{
			DrawInstanceStampObject(pObject);
			return true;
		}

		// get LOD flag from core object
		int iUsingWhichLOD = pObject->iUsingWhichLOD;

		// actual object or instance of object
		sObject* pActualObject = pObject;
		// if object uses bInstanceAlphaOverride, we might be doing a per-object shader operation
		if ( pObject->bInstanceAlphaOverride )
		{
			// allows drawmesh to know if this instance is a per-instance changer
			g_InstanceAlphaControlValue = pObject->dwInstanceAlphaOverride;
		}
		if ( pObject->pInstanceOfObject )
		{
			// get actual object via instance ptr
			pActualObject=pActualObject->pInstanceOfObject;

			// if instance uses alpha factor, apply to object
			if ( pObject->bInstanceAlphaOverride )
			{
				// if mesh exists with blending and argument mode, set the individual instance alpha value
				if ( pActualObject->iMeshCount > 0 )
				{
					sMesh* pMesh = pActualObject->ppMeshList [ 0 ];
					if ( pMesh->dwTextureCount > 0 )
					{
						m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP, pMesh->pTextures [ 0 ].dwBlendMode );
						m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, pMesh->pTextures [ 0 ].dwBlendArg1 );
						m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
						m_pD3D->SetRenderState( D3DRS_TEXTUREFACTOR, pObject->dwInstanceAlphaOverride );
					}
				}
 				m_RenderStates.bNoMeshAlphaFactor = true;
			}
		}

		// store current object distance globally so DrawMesh can set Shader technique based on distance (SET OBJECT LOD TECHNIQUE)
		g_fObjectCamDistance = pObject->position.fCamDistance;

		// first identify if 'pInstanceMeshVisible' limbs should be visible/invisible
		if ( pObject->iUsingWhichLOD==-1000 )
		{
			// LIMB VISIBILITY LOD STYLE
			if ( pObject->pInstanceOfObject )
			{
				// INSTANCE OBJECT
				pObject->pInstanceMeshVisible [ pObject->iLOD0LimbIndex ] = false;
				pObject->pInstanceMeshVisible [ pObject->iLOD1LimbIndex ] = false;
				pObject->pInstanceMeshVisible [ pObject->iLOD2LimbIndex ] = false;
				if ( pObject->position.fCamDistance > pObject->fLODDistance[1] )
				{
					// furthest
					pObject->pInstanceMeshVisible [ pObject->iLOD2LimbIndex ] = true;
				}
				else
				{
					if ( pObject->position.fCamDistance > pObject->fLODDistance[0] )
					{
						// mid-way
						pObject->pInstanceMeshVisible [ pObject->iLOD1LimbIndex ] = true;
					}
					else
					{
						// closest
						pObject->pInstanceMeshVisible [ pObject->iLOD0LimbIndex ] = true;
					}
				}
			}
		}

		// run through all of the frames within the object
		if ( pActualObject->ppFrameList )
		{
			for ( int iFrame = 0; iFrame < pActualObject->iFrameCount; iFrame++ )
			{
				// if instance limb visibility hidden, skip now
				if ( pObject->pInstanceMeshVisible )
				{
					// if limb in instance hidden, skip
					if ( pObject->pInstanceMeshVisible [ iFrame ]==false )
						continue;
				}

				// get a pointer to the frame
				sFrame* pFrame = pActualObject->ppFrameList [ iFrame ];

				// 301007 - new limb excluder
				if ( pFrame==NULL ) continue;
				if ( pFrame->bExcluded==true ) continue;

				// get mesh from frame
				sMesh* pMesh = pFrame->pMesh;

				// calculate correct absolute world matrix
				CalculateAbsoluteWorldMatrix ( pObject, pFrame, pMesh );

				// if flagged, reject meshes outside of camera frustrum (bVeryEarlyObject prevents sky meshes from disappearing)
				if ( bFrustrumCullMeshes==true && pObject->bVeryEarlyObject==false )
				{
					if ( pMesh )
					{
						D3DXVECTOR3 vecCentre = pMesh->Collision.vecCentre;
						D3DXVec3TransformCoord ( &vecCentre, &vecCentre, &pFrame->matAbsoluteWorld );
						D3DXVECTOR3 vecDirection = vecCentre - pObject->position.vecPosition;
						D3DXVECTOR3 vecCamDirection = m_pCamera->vecLook;
						D3DXVec3Normalize ( &vecDirection, &vecDirection );
						D3DXVec3Normalize ( &vecCamDirection, &vecCamDirection );
						float fDot = D3DXVec3Dot ( &vecDirection, &vecCamDirection );
						if ( fDot<=-0.6f ) // at 200 HFOV can see these!
							continue;
					}
				}

				// draw mesh
				if ( pMesh )
				{
					// apply the final transform
					if ( !pMesh->bLinked )
					{
						// mike 170505 - new matrix for completely custom, physics needs this for implementing it's own matrix
						if ( pFrame->bOverride )
						{
							if ( FAILED ( m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &pFrame->matOverride ) ) )
								return false;
						}
						else if ( !pObject->bDisableTransform )
						{
							// mike - 140803
							if ( FAILED ( m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &pFrame->matAbsoluteWorld ) ) )
								return false;
						}
					}

					// LOD System
					sMesh* pCurrentLOD = pMesh;
					if ( pObject->iUsingWhichLOD==-1000 )
					{
						// LIMB VISIBILITY LOD STYLE
						if ( pObject->pInstanceOfObject )
						{
							// done above, before we enter frame loop
						}
						else
						{
							// ACTUAL OBJECT
							sMesh* pMeshLOD0 = pObject->ppFrameList[pObject->iLOD0LimbIndex]->pMesh;
							sMesh* pMeshLOD1 = pObject->ppFrameList[pObject->iLOD1LimbIndex]->pMesh;
							sMesh* pMeshLOD2 = pObject->ppFrameList[pObject->iLOD2LimbIndex]->pMesh;
							if ( pMeshLOD0 ) pMeshLOD0->bVisible = false;
							if ( pMeshLOD1 ) pMeshLOD1->bVisible = false;
							if ( pMeshLOD2 ) pMeshLOD2->bVisible = false;
							if ( pObject->position.fCamDistance > pObject->fLODDistance[1] )
							{
								// furthest
								if ( pMeshLOD2) pMeshLOD2->bVisible = true;
							}
							else
							{
								if ( pObject->position.fCamDistance > pObject->fLODDistance[0] )
								{
									// mid-way
									if ( pMeshLOD1 ) pMeshLOD1->bVisible = true;
								}
								else
								{
									// closest
									if ( pMeshLOD0 ) pMeshLOD0->bVisible = true;
								}
							}
							if ( pMeshLOD1==NULL && pMeshLOD2==NULL && pMeshLOD0 ) pMeshLOD0->bVisible = true;
						}
					}
					else
					{
						// MESH SELECT LOD STYLE

						// update world transform for LOD quad (always faces camera)
						D3DXMATRIX matQuadRotation;

						// u74b7 - moved to UpdateLayer
						if ( (pObject->iUsingOldLOD==3 || pObject->iUsingWhichLOD==3) && m_pCamera )
						{
							float dx = pObject->position.vecPosition.x - m_pCamera->vecPosition.x;
							float dz = pObject->position.vecPosition.z - m_pCamera->vecPosition.z;
							float theangle = atan2 ( dx, dz );
							D3DXMatrixRotationY(&matQuadRotation, theangle );
							matQuadRotation._41 = pFrame->matAbsoluteWorld._41;
							matQuadRotation._42 = pFrame->matAbsoluteWorld._42;
							matQuadRotation._43 = pFrame->matAbsoluteWorld._43;
						}

						// leeadd - U71 - determine LOD meshes (current and old (transition) if applicable)
						if ( iUsingWhichLOD==1 && pFrame->pLOD[0] ) pCurrentLOD = pFrame->pLOD[0];
						if ( iUsingWhichLOD==2 && pFrame->pLOD[1] ) pCurrentLOD = pFrame->pLOD[1];
						if ( iUsingWhichLOD==3 && pFrame->pLODForQUAD ) pCurrentLOD = pFrame->pLODForQUAD;
						sMesh* pOldLOD = NULL;
						if ( pObject->iUsingOldLOD != -1 )
						{
							// the old lod mesh
							if ( pObject->iUsingOldLOD==0 ) pOldLOD = pMesh;
							if ( pObject->iUsingOldLOD==1 && pFrame->pLOD[0] ) pOldLOD = pFrame->pLOD[0];
							if ( pObject->iUsingOldLOD==2 && pFrame->pLOD[1] ) pOldLOD = pFrame->pLOD[1];
							if ( pObject->iUsingOldLOD==3 && pFrame->pLODForQUAD ) pOldLOD = pFrame->pLODForQUAD;

							// transition in progress from OLD to CURRENT
							pObject->fLODTransition += 0.03f;
							if ( pObject->fLODTransition >= 2.0f )
							{
								// end transition and restore alpha states
								pObject->fLODTransition = 0.0f;
								pObject->iUsingOldLOD = -1;
								D3DCOLOR dwAlphaValueOnly = D3DCOLOR_ARGB ( 255, 0, 0, 0 );
								pCurrentLOD->dwAlphaOverride = dwAlphaValueOnly;
								pCurrentLOD->bAlphaOverride = false;
								pCurrentLOD->bZWrite = true;
								pCurrentLOD->bZBiasActive = false; // U74 - 120409 - refresh each cycle for each instance
								pCurrentLOD->fZBiasDepth = 0.0f; // U74 - 120409 - refresh each cycle for each instance
								pOldLOD->dwAlphaOverride = dwAlphaValueOnly;
								pOldLOD->bAlphaOverride = false;
								pOldLOD->bZWrite = true;
								pOldLOD->bZBiasActive = false;
								pOldLOD->fZBiasDepth = 0.0f;
								pOldLOD = NULL;

								// U72 - 100109 - record alpha state of this mesh (for when instance is not being calculated, i.e. updated in actual mesh for render state change)
								if ( pObject->pInstanceOfObject ) pObject->dwInstanceAlphaOverride = dwAlphaValueOnly;
							}
							else
							{
								// change alpha level of meshes involved in transition
								if ( pObject->fLODTransition<=1.0f )
								{
									// FIRST fade in current LOD mesh
									DWORD dwAlpha = (DWORD)(pObject->fLODTransition*255);
									D3DCOLOR dwAlphaValueOnly = D3DCOLOR_ARGB ( dwAlpha, 0, 0, 0 );
									pCurrentLOD->dwAlphaOverride = dwAlphaValueOnly;
									pCurrentLOD->bAlphaOverride = true;
									pCurrentLOD->bTransparency = true;

									// AND first bit of fade in switch off Zwrite so the 'appearing' image goes not clip the
									// new current mesh and other objects in the area (causing the background to come through)
									if ( pObject->iUsingWhichLOD!=3 )
									{
										if ( pObject->fLODTransition < 0.5f )
											pCurrentLOD->bZWrite = false;
										else
											pCurrentLOD->bZWrite = true;

										pCurrentLOD->bZBiasActive = false;
										pCurrentLOD->fZBiasDepth = 0.0f;
									}
									else
									{
										// last LODQUAD mesh is a plane, so can adjust bias ahead to ensure it is rendered ABOVE everything
										pCurrentLOD->bZWrite = false;
										pCurrentLOD->fZBiasDepth = -g_fZBiasEpsilon;
										pCurrentLOD->bZBiasActive = true;
									}

									// and OLD LOD must stay as reset
									if ( pOldLOD )
									{
										pOldLOD->dwAlphaOverride = D3DCOLOR_ARGB ( 255, 0, 0, 0 );
										pOldLOD->bAlphaOverride = false;
										pOldLOD->bZWrite = true;
										pOldLOD->bZBiasActive = false;
										pOldLOD->fZBiasDepth = 0.0f;
									}
								}
								else
								{
									// Ensure current LOD mesh is default (writing Z and no bias)
									pCurrentLOD->dwAlphaOverride = D3DCOLOR_ARGB ( 255, 0, 0, 0 );
									pCurrentLOD->bAlphaOverride = false;
									pCurrentLOD->bZWrite = true;
									pCurrentLOD->bZBiasActive = false;
									pCurrentLOD->fZBiasDepth = 0.0f;

									// AND now as OLD one fades away, push zbias so NEW/CURRENT mesh has all of Z buffer opportunity
									if ( pObject->iUsingOldLOD!=3 )
									{
										// except the last LODQUAD, which needs zbias as is to do proper fade out
										pOldLOD->fZBiasDepth = g_fZBiasEpsilon * (pObject->fLODTransition-1.0f);
										pOldLOD->bZBiasActive = true;

										// AND last bit of fade out switch off Zwrite so the 'almost gone' image goes not clip the
										// new current mesh and other objects in the area (causing the background to come through)
										if ( pObject->fLODTransition > 1.5f )
											pOldLOD->bZWrite = false;
										else
											pOldLOD->bZWrite = true; // U74 - 120409 - refresh each cycle for each instance
									}
									else
									{
										// For the last LOD QUAD, make the decal fade out slower (to avoid the flick against the sky)
										pObject->fLODTransition -= 0.01f;

										// U74 - 120409 - no zbias effect
										pOldLOD->bZBiasActive = false;
										pOldLOD->fZBiasDepth = 0.0f;

										// also disable ALL zwrites from LODQUAD to avoid artefacts
										pOldLOD->bZWrite = false;
									}

									// THEN fade out old LOD mesh
									DWORD dwAlpha = (DWORD)((2.0f-pObject->fLODTransition)*255);
									D3DCOLOR dwAlphaValueOnly = D3DCOLOR_ARGB ( dwAlpha, 0, 0, 0 );
									pOldLOD->dwAlphaOverride = dwAlphaValueOnly;
									pOldLOD->bAlphaOverride = true;
									pOldLOD->bTransparency = true;
								}
							}
						}
						else
						{
							// U72 - 100109 - mesh not in transition, but still need the alpha state if this is an instanced object
							if ( pObject->pInstanceOfObject && pCurrentLOD )
							{
								pCurrentLOD->dwAlphaOverride = pObject->dwInstanceAlphaOverride;
								pCurrentLOD->bAlphaOverride = false;
								pCurrentLOD->bZWrite = true;
								pCurrentLOD->bZBiasActive = false;
								pCurrentLOD->fZBiasDepth = 0.0f;
							}
						}

						// leeadd - U71 - if in transition, draw OLD first
						if ( pOldLOD )
						{
							if ( pObject->iUsingOldLOD==3 )
								m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &matQuadRotation );
							else
								m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &pFrame->matAbsoluteWorld );

							// draw old LOD mesh
							if ( !DrawMesh ( pOldLOD ) )
								return false;

							// restore projection matrix
							if ( pObject->iUsingWhichLOD!=3 )
								m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &pFrame->matAbsoluteWorld );
						}

						// update world transform for LOD quad (always faces camera)
						if ( pObject->iUsingWhichLOD==3 )
							m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &matQuadRotation );
					}

					// draw the current mesh
					if ( !DrawMesh ( pCurrentLOD, (pObject->pInstanceMeshVisible!=NULL) , pObject ) )
						return false;

					// render any bound meshes
					if ( pObject->bDrawBounds || pMesh->bDrawBounds )
					{
						if ( pFrame->pBoundBox ) DrawMesh ( pFrame->pBoundBox );
						if ( pFrame->pBoundSphere ) DrawMesh ( pFrame->pBoundSphere );
					}

					// for linked objects
					if ( pMesh->bLinked )
					{
						if ( !pObject->bDisableTransform )
						{
							m_pD3D->GetTransform ( D3DTS_WORLD, &matSavedWorld );
							matNewWorld = pFrame->matOriginal * matSavedWorld;
							m_pD3D->SetTransform ( D3DTS_WORLD, &matNewWorld );
						}
					}
				}
			}
		}

		// if instance uses alpha factor, apply to object
		m_RenderStates.bNoMeshAlphaFactor = false;
		m_RenderStates.bIgnoreDiffuse = false;
	}

	// calculate object visibility based on hardware occlusion
	if ( g_Occlusion.d3dQuery[pObject->dwObjectNumber]!=NULL )
	{
		if ( g_Occlusion.GetOcclusionMode()==1 )
		{
			if ( g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber]==1 )
			{
				g_Occlusion.d3dQuery[pObject->dwObjectNumber]->Issue( D3DISSUE_END );
				if ( bOcclusionRenderHappened==false )
				{
					// cancel query now, no render at all between issue phase means corrupt result!
					g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber] = 0;
				}
				else
				{
					g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber] = 2;
				}
			}
			if ( g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber]==2 )
			{
				DWORD pixelsVisible = 0;
				HRESULT dwResult = g_Occlusion.d3dQuery[pObject->dwObjectNumber]->GetData((void *)&pixelsVisible, sizeof(DWORD), 0);
				if ( dwResult==S_OK )
				{
					// get pixels result
					g_Occlusion.dwQueryValue[pObject->dwObjectNumber] = pixelsVisible;
					g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber] = 0;
				}
				else
				{
					if ( dwResult!=S_FALSE )
					{
						// cancel whole thing if error returned
						g_Occlusion.iQueryBusyStage[pObject->dwObjectNumber] = 0;
					}
				}
			}
		}
	}

	// sorted, return back
	return true;
}

bool CObjectManager::PostDrawRestores ( void )
{
	// cleanup render states before leave draw process
	m_pD3D->SetPixelShader ( 0 );

	// fixed function blending restores
	DWORD dwMaxTextureStage = 7;
	for ( DWORD dwTextureStage = 0; dwTextureStage < dwMaxTextureStage; dwTextureStage++ )
	{
		// texture filter modes
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pD3D->SetSamplerState ( dwTextureStage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		// texture coordinate data
		m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_TEXCOORDINDEX, dwTextureStage );

		// texture blending modes
		if ( dwTextureStage==0 )
		{
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLOROP, D3DTOP_MODULATE );
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		}
		else
		{
			m_pD3D->SetTextureStageState ( dwTextureStage, D3DTSS_COLOROP, D3DTOP_DISABLE );
		}
	}

    // must always restore ambient level (for layers)
	m_pD3D->SetRenderState ( D3DRS_AMBIENT, m_RenderStates.dwAmbientColor );

	// leeadd - 140304 - ensure FOV is restored
	if ( m_RenderStates.fObjectFOV != 0.0f )
	{
		g_Camera3D_SetFOV ( m_RenderStates.fStoreCameraFOV );
		m_RenderStates.fObjectFOV = 0.0f;
	}

    if ( g_pGlob && g_pGlob->iFogState == 1 )
    {
	    m_pD3D->SetRenderState ( D3DRS_FOGENABLE, TRUE );
    	m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, m_RenderStates.dwFogColor );
    }

    // okay
	return true;
}

bool CObjectManager::Reset ( void )
{
	// set values to default
	m_iCurrentTexture = -22000000;		// current texture being used
	m_iLastTexture    = -22000000;		// last texture being used
	m_dwLastTextureCount = 0;
	m_dwCurrentShader    = 0;
	m_dwCurrentFVF       = 0;				// current FVF
	m_dwLastShader       = 0;
	m_dwLastFVF          = 0;				// previous FVF
	m_bUpdateVertexDecs  = true;			// reset update vertex settings 
	m_bUpdateStreams     = true;			// reset update stream sources every frame

	// leefix - 200303 - reset cullmode from various stencil effects
	m_RenderStates.dwCullDirection				= m_RenderStates.dwGlobalCullDirection;
	m_pD3D->SetRenderState ( D3DRS_CULLMODE,	m_RenderStates.dwCullDirection );
	m_RenderStates.bCull						= true;
	m_RenderStates.iCullMode					= 0;	

	// U75 - 070410 - added new render state to control whether entire render is blanked to a color
	tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwRenderCameraID );
	if ( m_Camera_Ptr ) m_RenderStates.dwOverrideAllWithColor = m_Camera_Ptr->dwForegroundColor;
	m_RenderStates.bOverrideAllTexturesAndEffects = false;
	if ( m_RenderStates.dwOverrideAllWithColor != 0 ) m_RenderStates.bOverrideAllTexturesAndEffects = true;
	m_RenderStates.bOverriddenClipPlaneforHLSL = false;

	return true;
}

void CObjectManager::UpdateViewProjForMotionBlur(void)
{
	// 270515 - record current viewproj (for working out previous viewproj for motion blur)
	tagCameraData* m_pCamera = (tagCameraData*)g_Camera3D_GetInternalData ( 0 );
	if ( m_pCamera )
	{
		g_matPreviousViewProj = g_matThisViewProj;
		D3DXMATRIX matView, matProj;
	    g_matThisViewProj = m_pCamera->matView * m_pCamera->matProjection;
		D3DXMatrixInverse ( &g_matThisViewProjInverse, NULL, &g_matThisViewProj );
	}
}

void CObjectManager::UpdateInitOnce ( void )
{
	// ensure that the D3D device is valid
	if ( !m_pD3D )
		return;

	// replace any buffers if object modified (from mesh size change or limb mods)
	if ( !m_ObjectManager.ReplaceAllFlaggedObjectsInBuffers() )
		return;

	// sort texture list if needed
	// lee - 290914 - only update texture sorted list when overall object count changes (g_iObjectListRefLastCount) MASSIVE SPEED-UP!
	// lee - 300914 - broke whole renderer (blue screen issue) and this code already inside SortTextureList
	//if ( g_iObjectListRefCount!=g_iObjectListRefLastCount )
	//	if ( !SortTextureList ( ) )
	//		return;
	//g_iObjectListRefLastCount=g_iObjectListRefCount;
	SortTextureList();

    // get camera data into member variable
	m_pCamera = (tagCameraData*)g_Camera3D_GetInternalData ( g_pGlob->dwRenderCameraID );
	if ( m_pCamera )
	{
		// projection matrix
		D3DXMATRIX matProj = m_pCamera->matProjection;
		m_pD3D->SetTransform ( D3DTS_PROJECTION, &matProj );

		// regular or reflected (clip mode has reflection modes at 2 and 4)
		if ( m_pCamera->iClipPlaneOn==2 || m_pCamera->iClipPlaneOn==4 )
		{
			// Reflect camera view in clip plane (mirror)
			D3DXMATRIX matView, matReflect;
			D3DXMatrixReflect ( &matReflect, &m_pCamera->planeClip );
			D3DXMatrixMultiply ( &matView, &matReflect, &m_pCamera->matView );
			m_pD3D->SetTransform ( D3DTS_VIEW, &matView );
		}
		else
		{
			// Regular camera view
			D3DXMATRIX matView = m_pCamera->matView;
			m_pD3D->SetTransform ( D3DTS_VIEW, &matView );
		}
	}

	// setup the viewing frustum data
	if ( !SetupFrustum ( 0.0f ) )
		return;

	// setup the visibility list
	if ( !SortVisibilityList ( ) )
		return;

	// update only those that are visible
	if ( !m_ObjectManager.UpdateOnlyVisible() )
		return;

	// refresh all data in VB (from any vertex changes in objects)
	if ( !m_ObjectManager.UpdateAllObjectsInBuffers() )
		return;

	// lee - 250813 - occlusion renders scene to populate depth buffer in anticipation of occlusion system
	g_Occlusion.StartOcclusionStep();

	// can render even earlier in pipeline, so this can be flagged to happen earlier in UpdateOnce
	g_bScenePrepared = false;

	// 270515 - helps clear depth texture render target each cycle
	g_bFirstRenderClearsRenderTarget = false;

	// some objects need to be rendered before ANYTHING (camera, light, matrix, terrain even stencilstart)
	// ideal for things like sky boxes that do not interfere with the Z buffer for scene fidelity
	if ( g_bRenderVeryEarlyObjects==true )
	{
		// Prepare main render
		UpdateInit();

		// prepare initial scene states
		if ( !PreSceneSettings ( ) )
			return;

		// render VERY EARLY objects (such as sky)
		UpdateLayer ( -1 );

		// scene prepared
		g_bScenePrepared = true;
	}

	// okay
	return;
}

bool CObjectManager::UpdateInit ( void )
{
	// ensure that the D3D device is valid
	if ( !m_pD3D )
		return false;

	// reset values
	if ( !Reset ( ) )
		return false;
	
	// okay
	return true;
}

// calculate distance from object to camera
float CObjectManager::CalculateObjectDistanceFromCamera ( sObject* pObject )
{
    // u74b8 - If the camera isn't selected, just use main camera
    //if (!m_pCamera) m_pCamera = 
	// lee - 280714 - always use camera zero for distance check (as can only store one in object structure)
	tagCameraData* pCamera = (tagCameraData*) g_Camera3D_GetInternalData(0);

    // u74b8 - Use current camera position as the start point, or 0,0,0 for a locked object
    D3DXVECTOR3 vecCamPos = (pObject->bLockedObject) ? D3DXVECTOR3(0,0,0) : pCamera->vecPosition;

    // u74b8 - Follow glued objects until you reach the end of the glue-chain, or a
    // glued object that no longer exists.
    while ( pObject->position.iGluedToObj != 0 )
    {
        if (g_ObjectList [ pObject->position.iGluedToObj ] == NULL)
        {
            // Glued to an object that does not exist, so break the chain
            pObject->position.iGluedToObj = 0;
            break;
        }

        pObject = g_ObjectList [ pObject->position.iGluedToObj ];
    }

    float fdx = pObject->position.vecPosition.x - vecCamPos.x;
    float fdy = pObject->position.vecPosition.y - vecCamPos.y;
    float fdz = pObject->position.vecPosition.z - vecCamPos.z;

	return sqrt ( (fdx * fdx) + (fdy * fdy) + (fdz * fdz) );
}

bool CObjectManager::UpdateLayer ( int iLayer )
{
	// if resources destroyed, quit now
	if ( GetSortedObjectVisibleList()==NULL )
		return true;

	// prepare render states to draw
	if ( !PreDrawSettings ( ) )
		return false;

    bool Status = UpdateLayerInner(iLayer);

    // restore render states after draw
	if ( !PostDrawRestores ( ) )
		return false;

    return Status;
}

bool CObjectManager::UpdateLayerInner ( int iLayer )
{
	// work vars
	int iObject = 0;
	bool bUseStencilWrite=false;
	D3DLIGHT9 lightzero;
	D3DXVECTOR3 vecShadowPos;

    // Get camera information for LOD and distance calculation
	// lee - 011014 - ensure rendercamera of 31-34 selects mask for camera 31 (shadow camera)
    DWORD dwCurrentCameraBit;
	if ( g_pGlob->dwRenderCameraID<31 )
		dwCurrentCameraBit = 1 << g_pGlob->dwRenderCameraID;
	else
		dwCurrentCameraBit = 1 << 31;

	// run through all visible objects and draw them (unrolled for performance)
	switch ( iLayer )
	{
	case -1 : // Very Early Objects (rendered even before StencilStart)

        if ( ! m_vVisibleObjectEarly.empty() )
        {
            for ( DWORD iIndex = 0; iIndex < m_vVisibleObjectEarly.size(); ++iIndex )
            {
                sObject* pObject = m_vVisibleObjectEarly [ iIndex ];
                if ( !pObject ) continue;

			    // leeadd - 211006 - u63 - ignore objects whose masks reject the current camera
			    if ( (pObject->dwCameraMaskBits & dwCurrentCameraBit)==0 )
				    continue;

			    // leeadd - 240106 - if any LOD activity
                // u74b8 - avoid recalculation of distance if already sorted by distance
                if ( pObject->bHadLODNeedCamDistance && g_eGlobalSortOrder != E_SORT_BY_DEPTH)
				    pObject->position.fCamDistance = CalculateObjectDistanceFromCamera ( pObject );

			    // call the draw function
			    if ( !DrawObject ( pObject, true ) )
				    return false;

			    // update any shadow meshes
			    if ( m_bGlobalShadows )
				    if ( pObject->bCastsAShadow && pObject->bVisible )
					    if ( g_pGlob->dwStencilMode==1 )
						    for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
							    if ( pObject->ppFrameList [ iFrame ]->bExcluded==false ) // 301007 - new limb excluder
								    UpdateShadowFrame ( pObject->ppFrameList [ iFrame ], &pObject->position.matWorld, g_iShadowPositionFixed, g_vecShadowPosition );
		    }
        }
		break;

	case 0 : // Main Layer

        if ( ! m_vVisibleObjectStandard.empty() )
        {
            for ( DWORD iIndex = 0; iIndex < m_vVisibleObjectStandard.size(); ++iIndex )
            {
                sObject* pObject = m_vVisibleObjectStandard [ iIndex ];
				if ( !pObject ) continue;

				// leeadd - 211006 - u63 - ignore objects whose masks reject the current camera
				if ( (pObject->dwCameraMaskBits & dwCurrentCameraBit)==0 )
					continue;

				/* Replace with a nice debug visual of shadow volume soon
				// DEBUG SHOW SHADOW
				if ( 1 )
				{
					// set world matrix to base object (should be limb world)
					int tokay=0;
					D3DXMATRIX matDef;
					D3DXMatrixIdentity ( &matDef );
					if ( FAILED ( m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &pObject->position.matWorld ) ) ) 	return false;
					for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
					{
						if ( pObject->ppFrameList [ iFrame ]->pShadowMesh )
						{
							tokay=1;
							DrawMesh ( pObject->ppFrameList [ iFrame ]->pShadowMesh );
						}
					}
				}
				*/

				// do not render static objects
				if ( pObject->bStatic )
					continue;

				// or stencil objects
				if ( pObject->bReflectiveObject )
					continue;

				// leeadd - 240106 - if any LOD activity
                // u74b8 - avoid recalculation of distance if already sorted by distance
                if ( pObject->bHadLODNeedCamDistance && g_eGlobalSortOrder != E_SORT_BY_DEPTH)
					pObject->position.fCamDistance = CalculateObjectDistanceFromCamera ( pObject );

				// call the draw function
				if ( !DrawObject ( pObject, false ) )
					return false;

				// update any shadow meshes
				if ( m_bGlobalShadows )
					if ( pObject->bCastsAShadow && pObject->bVisible )
						if ( g_pGlob->dwStencilMode==1 )
							for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
								if ( pObject->ppFrameList [ iFrame ]->bExcluded==false ) // 301007 - new limb excluder
									UpdateShadowFrame ( pObject->ppFrameList [ iFrame ], &pObject->position.matWorld, g_iShadowPositionFixed, g_vecShadowPosition );
			}
        }
        break;

	case 1 : // Reflection Stencil Layer

        if ( ! m_vVisibleObjectStandard.empty() )
        {
			// STENCIL DRAW STAGE
			bUseStencilWrite=false;

            for ( DWORD iIndex = 0; iIndex < m_vVisibleObjectStandard.size(); ++iIndex )
            {
                sObject* pObject = m_vVisibleObjectStandard [ iIndex ];
				if ( !pObject ) continue;

				// leeadd - 211006 - u63 - ignore objects whose masks reject the current camera
				if ( (pObject->dwCameraMaskBits & dwCurrentCameraBit)==0 )
					continue;

				// only render Reflective objects
				if ( pObject->bReflectiveObject )
				{
					// prepare for stencil write
					if ( bUseStencilWrite==false )
					{
						// set position and direction of reflection plane
						D3DXPlaneFromPointNormal ( &g_plnReflectionPlane, &pObject->position.vecPosition, &pObject->position.vecLook );
						g_vecReflectionPoint = pObject->position.vecPosition;

						// set the user clipping flag for reflection
						g_bReflectiveClipping = pObject->bReflectiveClipping;

						// turn depth buffer off, and stencil buffer on
						m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,    TRUE );
						m_pD3D->SetRenderState ( D3DRS_STENCILFUNC,      D3DCMP_ALWAYS );
						m_pD3D->SetRenderState ( D3DRS_STENCILREF,       0x1 );
						m_pD3D->SetRenderState ( D3DRS_STENCILMASK,      0xffffffff );
						m_pD3D->SetRenderState ( D3DRS_STENCILWRITEMASK, 0xffffffff );
// leefix - 050303 - fixes garbage in stencil buffer found in GF3 cards
//							m_pD3D->SetRenderState ( D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP );
//							m_pD3D->SetRenderState ( D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP );
// leeifx - 150503 - several objects in overlap would cause stencil to fill with zero when it should stay one
//							m_pD3D->SetRenderState ( D3DRS_STENCILZFAIL,     D3DSTENCILOP_ZERO );
						m_pD3D->SetRenderState ( D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP );
						m_pD3D->SetRenderState ( D3DRS_STENCILFAIL,      D3DSTENCILOP_ZERO );
						m_pD3D->SetRenderState ( D3DRS_STENCILPASS,      D3DSTENCILOP_REPLACE );

						// make sure no pixels are written to the z buffer or frame buffer
						m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,     FALSE );
						m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
						m_pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_ZERO );
						m_pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_ONE );

						// clear the stencil buffer
						m_pD3D->Clear ( 0L, NULL, D3DCLEAR_STENCIL, 0L, 1.0f, 0L );

						// flag that the stencil is engaged
						bUseStencilWrite=true;
					}

					// poke holes in stencil
					if ( !DrawObject ( pObject, false ) )
						return false;
				}
			}
			if ( bUseStencilWrite )
			{
				// Complete stencil write by restoring render mode
				m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,	FALSE );
				m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,	 TRUE );
				m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
				bUseStencilWrite=false;
			}
        }
		break;

	case 2 : // Shadow Stencil Layer

        if ( ! m_vVisibleObjectStandard.empty() )
        {
			// STENCIL DRAW STAGE
			bUseStencilWrite=false;

            for ( DWORD iIndex = 0; iIndex < m_vVisibleObjectStandard.size(); ++iIndex )
            {
                sObject* pObject = m_vVisibleObjectStandard [ iIndex ];
                if ( !pObject ) continue;

				// leeadd - 211006 - u63 - ignore objects whose masks reject the current camera
				if ( (pObject->dwCameraMaskBits & dwCurrentCameraBit)==0 )
					continue;

				sObject* pActualObject = pObject;
				if ( pActualObject->pInstanceOfObject )
					pActualObject=pActualObject->pInstanceOfObject;

				// also draw any shadows cast by this object
				if ( pActualObject->bCastsAShadow==true && pObject->bVisible==true && pObject->bHideShadow==false )
				{
					// prepare for stencil write
					if ( bUseStencilWrite==false )
					{
						// render the shadow
						StartStencilMeshWrite();
						bUseStencilWrite=true;

						// shadow shader takes light zero, so hack it in
						m_pD3D->GetLight( 0, &lightzero );
						if ( g_iShadowPositionFixed==-1 )
							vecShadowPos = g_vecShadowPosition;
						else
						{
							D3DLIGHT9 lightfrom;
							m_pD3D->GetLight( g_iShadowPositionFixed, &lightfrom );
							vecShadowPos = lightfrom.Position;
						}
						lightzero.Position = vecShadowPos;
						lightzero.Type=D3DLIGHT_POINT;
						m_pD3D->SetLight( 0, &lightzero );
					}

					// all frames of this object
					bool buseCPUNoShaderShadow=false;
					for ( int iFrame = 0; iFrame < pActualObject->iFrameCount; iFrame++ )
					{
						// get a pointer to the frame
						sFrame* pActualFrame = pActualObject->ppFrameList [ iFrame ];
						if ( pActualFrame )
						{
							if ( pActualFrame->bExcluded==true ) // 301007 - new limb excluder
								continue;

							// get mesh from frame
							sMesh* pShadowMesh = pActualFrame->pShadowMesh;
							CalculateAbsoluteWorldMatrix ( pObject, pActualFrame, pShadowMesh );
							if ( pShadowMesh )
							{
								if ( pShadowMesh->Collision.fLargestRadius != -1 )
								{
									// leeadd - shadows can be clipped if outside range
									if ( g_iShadowClipping > 0 )
									{
										if ( g_iShadowClipping==1 )
										{
											if ( pActualFrame->matAbsoluteWorld._43 < g_fShadowClippingMin ) continue;
											if ( pActualFrame->matAbsoluteWorld._43 > g_fShadowClippingMax ) continue;
										}
									}

									// draw shadow at correct world position
									m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &pActualFrame->matAbsoluteWorld );
									DrawMesh ( pShadowMesh );
								}
								else
								{
									// use CPU shadow technique
									buseCPUNoShaderShadow=true;
									continue;
								}
							}
						}
					}
					if ( buseCPUNoShaderShadow==true )
					{
						// set world matrix to base object (should be limb world)
						D3DXMATRIX matDef;
						D3DXMatrixIdentity ( &matDef );
						if ( FAILED ( m_pD3D->SetTransform ( D3DTS_WORLD, ( D3DXMATRIX* ) &pObject->position.matWorld ) ) )
							return false;

						// prepare for stencil write
						if ( bUseStencilWrite==false )
						{
							// render the shadow
							StartStencilMeshWrite();
							bUseStencilWrite=true;
						}

						// old way of drawing shadow
						DrawStencilMesh ( pObject->iFrameCount, pObject->ppFrameList, NULL );
					}
				}
			}
			if ( bUseStencilWrite )
			{
				// Complete stencil write by restoring render mode
				FinishStencilMeshWrite();
				bUseStencilWrite=false;

				// restore light zero now shader finished with light zero position
				m_pD3D->SetLight( 0, &lightzero );
			}
        }
		break;
			
	case 3 : // Overlay Ghost Layer (in stages)

        if ( ! m_vVisibleObjectTransparent.empty() )
        {
			// leeadd - 021205 - new feature which can divide transparent depth-sorted objects by a water
			// line so everything below is rendered, then the water, then everything at normal surface order
			bool bWaterPlaneDivision = false;
			float fWaterPlaneDivisionY = 99999.99f;

			// get list of ghosted objects for depth sort
	        for ( DWORD iIndex = 0; iIndex < m_vVisibleObjectTransparent.size(); ++iIndex )
            {
                sObject* pObject = m_vVisibleObjectTransparent [ iIndex ];
				if ( !pObject ) continue;

				// leeadd - 211006 - u63 - ignore objects whose masks reject the current camera
				if ( (pObject->dwCameraMaskBits & dwCurrentCameraBit)==0 )
					continue;

				// calculate distance from object to camera (fills fCamDistance)
				if ( pObject->bTransparencyWaterLine==true )
				{
					// leeadd - 021205 - transparent object water line, using HEIGHY (Y) as ordering (great for water planes)
					if ( pObject->position.vecPosition.y < fWaterPlaneDivisionY )
						fWaterPlaneDivisionY = pObject->position.vecPosition.y;

					// set as furthest surface distance object (and first to be drawn after underwater objs)
					// u74b8 - use the current camera
                    if (g_eGlobalSortOrder != E_SORT_BY_DEPTH)
						pObject->position.fCamDistance = CalculateObjectDistanceFromCamera ( pObject );
					else
						pObject->position.fCamDistance = 0.0f;
					pObject->position.fCamDistance += m_pCamera->fZFar;
					bWaterPlaneDivision = true;
				}
				else
				{
					// regular object vs camera distance
                    // u74b8 - If already sorted by distance, then we've also already
                    //         calculated the camera distance and there's no need to do it again.
                    if (g_eGlobalSortOrder != E_SORT_BY_DEPTH)
                    {
						pObject->position.fCamDistance = CalculateObjectDistanceFromCamera ( pObject );
                    }
				}
			}

			// if some objs underwater division, increase their cam distances so they ALL are drawn first (in same order)
			if ( bWaterPlaneDivision==true )
			{
                for ( DWORD iIndex = 0; iIndex < m_vVisibleObjectTransparent.size(); ++iIndex )
                {
                    sObject* pObject = m_vVisibleObjectTransparent [ iIndex ];
					if ( pObject->bTransparencyWaterLine==false )
					{
						float fBaseOfObj = (pObject->collision.vecMax.y - pObject->collision.vecMin.y);
						fBaseOfObj *= pObject->position.vecScale.y;
						fBaseOfObj = pObject->position.vecPosition.y - fBaseOfObj;
						if ( fBaseOfObj < fWaterPlaneDivisionY )
                        {
                            // u74b8 - use the current camera
							// pObjectA->position.fCamDistance += m_Camera_Ptr->fZFar;
							pObject->position.fCamDistance += m_pCamera->fZFar;
                        }
					}
				}
			}

			// u74b7 - sort objects by distance, replaced bubblesort with STL sort
            std::sort(m_vVisibleObjectTransparent.begin(), m_vVisibleObjectTransparent.end(), OrderByReverseCameraDistance() );

            // draw in correct back to front order
            for ( DWORD iIndex = 0; iIndex < m_vVisibleObjectTransparent.size(); ++iIndex )
            {
                sObject* pObject = m_vVisibleObjectTransparent [ iIndex ];
				if ( !pObject ) 
					continue;

				// u75b9 - fixes Transparency and Camera Mask problem
                if (( pObject->dwCameraMaskBits & dwCurrentCameraBit ) == 0)
                    continue;

                if ( !DrawObject ( pObject, false ) )
                    return false;
            }

	    }
		// end ghost layer
		break;

	case 4 : // Overlay Locked/NoZ Layer (added 050406 - u6rc6)
	    
        if ( ! m_vVisibleObjectNoZDepth.empty() )
        {
			// NOZDEPTH LOOP (locked and nozdepth)
			// ( Pass A-ZDepth : Pass B-NoZDepth )
			bool bClearZBuffer = false;
			for ( int iPass=0; iPass<2; iPass++ )
			{
				// LOCKED STAGE
				float fCurrentFOV = 0.0f;
				bool bResetCamera = false;
				D3DXMATRIX matCurrentCameraView;

                for ( DWORD iIndex = 0; iIndex < m_vVisibleObjectNoZDepth.size(); ++iIndex )
                {
                    sObject* pObject = m_vVisibleObjectNoZDepth [ iIndex ];
					if ( !pObject ) continue;

					// leeadd - 211006 - u63 - ignore objects whose masks reject the current camera
					if ( (pObject->dwCameraMaskBits & dwCurrentCameraBit)==0 )
						continue;

					// only render nozdepth objects on second pass
					bool bRenderObject = false;
					if ( iPass==0 && pObject->bNewZLayerObject==false )
					{
						// object has zdepth pass 1
						bRenderObject=true;
					}
					if ( iPass==1 && pObject->bNewZLayerObject==true )
					{
						// object has no zdepth pass 2
						bRenderObject=true;
						if ( bClearZBuffer==false )
						{
							// clear zbuffer
							m_pD3D->Clear ( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
							bClearZBuffer=true;
						}
					}

					// only if object should be rendered
					if ( !bRenderObject )
						continue;

					// locked objects
					if ( pObject->bLockedObject )
					{
						// reset camera
						if ( bResetCamera==false )
						{
							// Store current camera
							m_pD3D->GetTransform ( D3DTS_VIEW, &matCurrentCameraView );

							// LEEFIX - 191102 - record current FOV, and set default FOV
							tagCameraData* m_Camera_Ptr = (tagCameraData*)g_Camera3D_GetInternalData( 0 );
							fCurrentFOV = m_Camera_Ptr->fFOV;
							// Dave check for switch bank flag being set
							if ( (m_Camera_Ptr->dwCameraSwitchBank & 1) == 0 )
								g_Camera3D_SetFOV ( D3DXToDegree(3.14159265358979323846f/2.905f) );

							// Use Default unmodified camera
							D3DXMATRIX matDefaultCameraView;
							D3DXMatrixIdentity ( &matDefaultCameraView );
							matDefaultCameraView._11 = 1; 
							matDefaultCameraView._12 = 0; 
							matDefaultCameraView._13 = 0;
							matDefaultCameraView._21 = 0; 
							matDefaultCameraView._22 = 1; 
							matDefaultCameraView._23 = 0;
							matDefaultCameraView._31 = 0;
							matDefaultCameraView._32 = 0; 
							matDefaultCameraView._33 = 1;

							// Assign new default camera
							m_pD3D->SetTransform ( D3DTS_VIEW, &matDefaultCameraView );

							// clear zbuffer
							m_pD3D->Clear ( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );

							// New camera established
							bResetCamera=true;
						}
					}
					else
					{
						if ( bResetCamera==true )
						{
							// Restore camera view
							m_pD3D->SetTransform ( D3DTS_VIEW, &matCurrentCameraView );
							g_Camera3D_SetFOV ( fCurrentFOV );
							bResetCamera=false;
						}
					}

					// draw
					if ( !DrawObject ( pObject, false ) )
						return false;
				}
				if ( bResetCamera )
				{
					// Restore camera view
					m_pD3D->SetTransform ( D3DTS_VIEW, &matCurrentCameraView );
					bResetCamera=false;

					// LEEFIX - 191102 - restore FOV if Locked Object set it (replaced in DBO system wioth better method)
					g_Camera3D_SetFOV ( fCurrentFOV );
				}
			}
		}
		break;
	}

	// okay
	return true;
}

bool CObjectManager::Update ( void )
{
	// leeadd - U71 - can render even earlier in pipeline, so this can be flagged to happen earlier in UpdateOnce
	if ( g_bScenePrepared==false )
	{
		// Prepare main render
		UpdateInit();

		// prepare initial scene states
		if ( !PreSceneSettings ( ) )
			return false;

		// scene prepared
		g_bScenePrepared = true;
	}
	
	// Render Scene from Universe
	if ( g_pUniverse )
	{
		// U77 - 090411 - soetimes some universe renders can WIPE OUT fog color (needed for subsequent 3D passes)
		DWORD dwRememberFogColor;
		m_pD3D->GetRenderState ( D3DRS_FOGCOLOR, &dwRememberFogColor );
		g_pUniverse->Render ( );
		m_pD3D->SetRenderState ( D3DRS_FOGCOLOR, dwRememberFogColor );
	}

	// Main layer render
	UpdateLayer ( 0 );

	// Etch any reflection stencils
	if(g_pGlob->dwStencilMode==2)
		if(g_pGlob->dwRedrawPhase==0)
			UpdateLayer ( 1 );

	// Shadow rendering
	if ( m_bGlobalShadows )
	{
		// Render Shadows from Casting Objects
		if(g_pGlob->dwStencilMode==1)
			if(g_pGlob->dwRedrawPhase==0)
				UpdateLayer ( 2 );

		// Render Shadows from Universe
		if(g_pGlob->dwStencilMode==1)
			if(g_pGlob->dwRedrawPhase==0)
				if ( g_pUniverse )
					g_pUniverse->RenderShadows ( );
	}

	// okay
	return true;
}

bool CObjectManager::UpdateGhostLayer ( void )
{
	// lee - 050406 - u6rc6 - overlay render layer (ghost used to be part of UpdateNoZLayer)
	Reset();
	UpdateLayer ( 3 );
	return true;
}

bool CObjectManager::UpdateNoZLayer ( void )
{
	// Must reset when return to manager
	Reset();

	// Overlay render layer (lock, nozdepth)
	UpdateLayer ( 4 );

	// okay
	return true;
}

//
// Stencil Buffer Management
//

void CObjectManager::SetGlobalShadowsOn ( void )
{
	m_bGlobalShadows = true;
}

void CObjectManager::SetGlobalShadowsOff ( void )
{
	m_bGlobalShadows = false;
}

void CObjectManager::DeleteStencilEffect ( sObject* pObject )
{
	// Stencil effect mode
	DWORD dwMode=g_pGlob->dwStencilMode;

	// decrement shadow count
	if( pObject->bCastsAShadow )
	{
		// remove shadow meshes from all frames
		for ( int iFrame = 0; iFrame < pObject->iFrameCount; iFrame++ )
		{
			// get fframe ptr
			sFrame* pFrame = pObject->ppFrameList [ iFrame ];

			// if frame has shadow
			if ( pFrame->pShadowMesh )
			{
				// remove shadow mesh from drawrender list
				m_ObjectManager.RemoveBuffersUsedByObjectMesh ( pFrame->pShadowMesh );

				// and ensure any objects removed are replaced before the render
				g_bObjectReplacedUpdateBuffers = true;

				// delete shadow mesh
				SAFE_DELETE( pFrame->pShadowMesh );
			}
		}

		pObject->bCastsAShadow = false;
		g_pGlob->dwStencilShadowCount--;
		if(g_pGlob->dwStencilShadowCount==0) dwMode=0;
	}

	// decrement reflection count
	if( pObject->bReflectiveObject )
	{
		pObject->bReflectiveObject = false;
		g_pGlob->dwStencilReflectionCount--;
		if(g_pGlob->dwStencilReflectionCount==0) dwMode=0;
	}

	// defer stencil effect mode
	if(dwMode==0)
	{
		// u74b7 - reorder stencilmode checking to allow the removal of last reflections to reduce the draw count
		if(g_pGlob->dwStencilReflectionCount>0)
		{
			g_pGlob->dwStencilMode=2;
			g_pGlob->dwRedrawCount=2;
		}
		else
		{
			if(g_pGlob->dwStencilShadowCount>0)
				g_pGlob->dwStencilMode=1;
			else
			    g_pGlob->dwStencilMode=0;
			g_pGlob->dwRedrawCount=1;
		}
	}
}

void CObjectManager::DoPrepareFillRender ( DWORD dwFillColor )
{
	// create filler initailly
	if(m_pSquareVB==NULL)
	{
		m_pD3D->CreateVertexBuffer ( 4 * sizeof ( SHADOWVERTEX ), D3DUSAGE_WRITEONLY, D3DFVF_SHADOWVERTEX, D3DPOOL_MANAGED, &m_pSquareVB, NULL );
   	}
	if(m_pSquareVB)
	{
		SHADOWVERTEX*	v  = NULL;
		float			sx = 5000.0f;
		float			sy = 5000.0f;

		m_pSquareVB->Lock ( 0, 0, ( VOID** ) &v, 0 );
		v [ 0 ].p = D3DXVECTOR4 (  0, sy, 0.0f, 1.0f );
		v [ 1 ].p = D3DXVECTOR4 (  0,  0, 0.0f, 1.0f );
		v [ 2 ].p = D3DXVECTOR4 ( sx, sy, 0.0f, 1.0f );
		v [ 3 ].p = D3DXVECTOR4 ( sx,  0, 0.0f, 1.0f );
		v [ 0 ].color = dwFillColor;
		v [ 1 ].color = dwFillColor;
		v [ 2 ].color = dwFillColor;
		v [ 3 ].color = dwFillColor;			
		m_pSquareVB->Unlock ( );
	}
}

void CObjectManager::DoStencilRenderStart ( void )
{
	if(g_pGlob->dwStencilMode==1) //shadow
	{
		if ( m_bGlobalShadows )
		{
			// clear stencil buffer before cast shadow data
			m_pD3D->Clear ( 0L, NULL, D3DCLEAR_STENCIL, 0L, 1.0f, 0L );
		}
	}
	if(g_pGlob->dwStencilMode==2) // reflection
	{
		if(g_pGlob->dwRedrawPhase==1)
		{
			// clear the zbuffer ( leave frame and stencil buffer intact )
			m_pD3D->Clear ( 0L, NULL, D3DCLEAR_ZBUFFER, 0L, 1.0f, 0L );

			// Save the view matrix
			m_pD3D->GetTransform ( D3DTS_VIEW, &g_matReflectionViewSaved );

			// Reflect camera in X-Z plane mirror
			D3DXMATRIX matView, matReflect;
			D3DXMatrixReflect ( &matReflect, &g_plnReflectionPlane );
			D3DXMatrixMultiply ( &matView, &matReflect, &g_matReflectionViewSaved );
			m_pD3D->SetTransform ( D3DTS_VIEW, &matView );

			// For BSP culling camera location within a reflection, record plane position
			g_pGlob->fReflectionPlaneX = g_vecReflectionPoint.x;
			g_pGlob->fReflectionPlaneY = g_vecReflectionPoint.y;
			g_pGlob->fReflectionPlaneZ = g_vecReflectionPoint.z;

			// Set clipping plane to stop objs behind mirror being drawn
			if ( g_bReflectiveClipping )
			{
				m_pD3D->SetClipPlane ( 0, (float*)g_plnReflectionPlane );
				m_pD3D->SetRenderState ( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
			}

			// Setup render states to a blended render scene against mask in stencil
			// buffer. An important step here is to reverse the cull-order of the
			// polygons, since the view matrix is being relected.
			m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,	TRUE );
			m_pD3D->SetRenderState ( D3DRS_STENCILREF,		0x1 );
			m_pD3D->SetRenderState ( D3DRS_STENCILFUNC,		D3DCMP_EQUAL );
			m_pD3D->SetRenderState ( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP );

			// Before render stencil view
			DoPrepareFillRender ( 0xFF000088 );

			// do general fill to show back and reverse of reflection area
			m_pD3D->SetRenderState  ( D3DRS_ZWRITEENABLE,		FALSE );
			m_pD3D->SetRenderState  ( D3DRS_FOGENABLE,			FALSE );
			m_pD3D->SetRenderState  ( D3DRS_CULLMODE,			D3DCULL_NONE );
			m_pD3D->SetRenderState  ( D3DRS_SRCBLEND,			D3DBLEND_ONE );
			m_pD3D->SetRenderState  ( D3DRS_DESTBLEND,			D3DBLEND_ZERO );
			m_pD3D->SetVertexShader ( NULL );
			m_pD3D->SetFVF ( D3DFVF_SHADOWVERTEX );
			m_pD3D->SetStreamSource ( 0, m_pSquareVB, 0, sizeof ( SHADOWVERTEX ) );
			m_pD3D->SetRenderState  ( D3DRS_CULLMODE,			D3DCULL_NONE );
			m_pD3D->DrawPrimitive   ( D3DPT_TRIANGLESTRIP,		0, 2 );
			m_pD3D->SetRenderState  ( D3DRS_FOGENABLE,			TRUE );

			// Settings for regular drawing
			m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,	TRUE );
			m_pD3D->SetRenderState ( D3DRS_SRCBLEND,		D3DBLEND_DESTCOLOR );
			m_pD3D->SetRenderState ( D3DRS_DESTBLEND,		D3DBLEND_ZERO );

			// Need to render everything in reverse
			m_pD3D->SetRenderState ( D3DRS_CULLMODE,		D3DCULL_CW );
			m_RenderStates.dwGlobalCullDirection		  = D3DCULL_CW;
		}
	}
}

void CObjectManager::DoStencilRenderEnd ( void )
{
	if(g_pGlob->dwStencilMode==1) //shadow
	{
		if ( m_bGlobalShadows )
		{
			// Apply all Shadows determined by stencil buffer
			m_pD3D->SetRenderState ( D3DRS_ZENABLE,          FALSE );
			m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,    TRUE );
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
			m_pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
			m_pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

			// U77 - 110112 - fog CANNOT be part of shadow!
			m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
			m_RenderStates.bFog = false;

			// render shades for shadow overlap
			m_pD3D->SetVertexShader	( NULL );
			m_pD3D->SetFVF			( D3DFVF_SHADOWVERTEX );
			m_pD3D->SetStreamSource	( 0, m_pSquareVB, 0,	sizeof ( SHADOWVERTEX ) );

			// full shade range supported
			int iNumberOfShades = m_StencilStates.dwShadowShades;
			DWORD dwBaseColor = (m_StencilStates.dwShadowStrength)/iNumberOfShades;
			DWORD dwColor = dwBaseColor;
			if ( iNumberOfShades > 1 ) 
			{
				m_pD3D->SetRenderState	( D3DRS_STENCILPASS,	D3DSTENCILOP_ZERO );
				m_pD3D->SetRenderState	( D3DRS_STENCILFUNC,	D3DCMP_EQUAL );
				for ( int iShades=iNumberOfShades*-1; iShades<0; iShades++ )
				{
					dwColor = dwBaseColor * (iShades*-1);
					DoPrepareFillRender( m_StencilStates.dwShadowColor + (dwColor<<24) );
					m_pD3D->SetRenderState ( D3DRS_STENCILREF, *(DWORD*)&iShades );
					m_pD3D->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, 2 );
				}
				dwColor = dwBaseColor * iNumberOfShades;
			}
			DoPrepareFillRender( m_StencilStates.dwShadowColor + (dwColor<<24) );
			m_pD3D->SetRenderState	( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
			m_pD3D->SetRenderState	( D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL );
			m_pD3D->SetRenderState ( D3DRS_STENCILREF, 0x00 );
			m_pD3D->DrawPrimitive ( D3DPT_TRIANGLESTRIP, 0, 2 );

			// restore render states
			m_pD3D->SetRenderState ( D3DRS_ZENABLE,          TRUE );
			m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,    FALSE );
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
		}
	}
	if(g_pGlob->dwStencilMode==2) //reflection
	{
		// Reflection render complete
		if(g_pGlob->dwRedrawPhase==1)
		{
			// fill render for distortion effect on reflection
			DoPrepareFillRender( m_StencilStates.dwReflectionColor );
			m_pD3D->SetRenderState ( D3DRS_ZENABLE,          FALSE );
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
			m_pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
			m_pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			m_pD3D->SetRenderState ( D3DRS_STENCILREF,  0x1 );
			m_pD3D->SetRenderState ( D3DRS_STENCILFUNC, D3DCMP_EQUAL );
			m_pD3D->SetRenderState ( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
			m_pD3D->SetVertexShader ( NULL );
			m_pD3D->SetFVF ( D3DFVF_SHADOWVERTEX );
			m_pD3D->SetStreamSource ( 0, m_pSquareVB, 0, sizeof ( SHADOWVERTEX ) );
			m_pD3D->SetRenderState ( D3DRS_CULLMODE, D3DCULL_NONE );
			m_pD3D->DrawPrimitive   ( D3DPT_TRIANGLESTRIP, 0, 2 );
			m_pD3D->SetRenderState ( D3DRS_FOGENABLE,        TRUE );

			// clipping plane off
			if ( g_bReflectiveClipping )
				m_pD3D->SetRenderState ( D3DRS_CLIPPLANEENABLE,		0x00 );

			// restore for continued normal rendering
			m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,		FALSE );
			m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
			m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

			// cull back to normal LEEFIX - 171002 - CW to CCW
			m_pD3D->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_CCW );
			m_RenderStates.dwGlobalCullDirection			  = D3DCULL_CCW;

			// restore view
			m_pD3D->SetTransform ( D3DTS_VIEW, &g_matReflectionViewSaved );
		}
	}
}

void CObjectManager::StartStencilMeshWrite ( void )
{
	// render the shadow (leefix - 260604-u54-reduces selfshadow artifacts)
	float fZBiasSlopeScale = 0.1f;
	m_pD3D->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS,	*(DWORD*)&fZBiasSlopeScale );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
	m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,		TRUE );
	m_pD3D->SetRenderState ( D3DRS_STENCILFUNC,			D3DCMP_ALWAYS );
	m_pD3D->SetRenderState ( D3DRS_STENCILREF,			0x1 );
	m_pD3D->SetRenderState ( D3DRS_STENCILMASK,			0xffffffff );
	m_pD3D->SetRenderState ( D3DRS_STENCILWRITEMASK,	0xffffffff );
	m_pD3D->SetRenderState ( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP );
	m_pD3D->SetRenderState ( D3DRS_STENCILFAIL,			D3DSTENCILOP_KEEP );
	m_pD3D->SetRenderState ( D3DRS_STENCILPASS,			D3DSTENCILOP_INCR );
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
	m_pD3D->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_ZERO );
	m_pD3D->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

	// lee - 220306 - u6b4 - shadow cannot use the fog effect
	m_pD3D->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	m_RenderStates.bFog	= false;
}

void CObjectManager::DrawFrameOrMesh ( int iFrameCount, sFrame** ppFrameList, sMesh* pOrShadowMesh )
{
	if ( ppFrameList )
	{
		// frame hierarchy
		for ( int iFrame = 0; iFrame < iFrameCount; iFrame++ )
			if ( ppFrameList [ iFrame ]->pShadowMesh )
				DrawMesh ( ppFrameList [ iFrame ]->pShadowMesh );
	}
	else
	{
		// single mesh
		DrawMesh ( pOrShadowMesh );
	}
}

void CObjectManager::DrawStencilMesh ( int iFrameCount, sFrame** ppFrameList, sMesh* pOrShadowMesh )
{
	// DEPTH-FAIL Approach

	// set stencil write operation and CULL
	m_pD3D->SetRenderState ( D3DRS_STENCILZFAIL,		D3DSTENCILOP_INCR );
	m_pD3D->SetRenderState ( D3DRS_STENCILPASS,			D3DSTENCILOP_KEEP );
	m_RenderStates.dwCullDirection = D3DCULL_CCW;
	m_RenderStates.bCull = false;
	m_RenderStates.iCullMode = 0;	

	// BACK of shadow
	DrawFrameOrMesh ( iFrameCount, ppFrameList, pOrShadowMesh );

	// reverse stencil write operation and reverse CULL
	m_pD3D->SetRenderState ( D3DRS_STENCILZFAIL,		D3DSTENCILOP_DECR );
	m_pD3D->SetRenderState ( D3DRS_STENCILPASS,			D3DSTENCILOP_KEEP );
	m_RenderStates.dwCullDirection = D3DCULL_CW;
	m_RenderStates.bCull = false;
	m_RenderStates.iCullMode = 0;	

	// FRONT of shadow
	DrawFrameOrMesh ( iFrameCount, ppFrameList, pOrShadowMesh );

	// CULL direction restore
	m_RenderStates.bCull = false;
	m_RenderStates.dwCullDirection = D3DCULL_CCW;
	m_RenderStates.iCullMode = 0;	
}

void CObjectManager::FinishStencilMeshWrite ( void )
{
	// Complete stencil write by restoring render mode
	m_pD3D->SetRenderState ( D3DRS_SHADEMODE,			D3DSHADE_GOURAUD );
	m_pD3D->SetRenderState ( D3DRS_CULLMODE,			D3DCULL_CCW );
	m_pD3D->SetRenderState ( D3DRS_STENCILENABLE,		FALSE );
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );
	m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
	m_pD3D->SetRenderState ( D3DRS_DEPTHBIAS,			0 );
	m_pD3D->SetRenderState ( D3DRS_SLOPESCALEDEPTHBIAS,	0 );
}

// Post process shaders using QUAD

void CObjectManager::CreateScreenQUAD ( int iInitialCameraZeroImage )
{
	// create a plane quad
	if ( m_pQUAD==NULL )
	{
		// create mesh for quad
		m_pQUAD = new sMesh;
		DWORD dwColor = D3DXCOLOR(1,1,1,1);
		MakeMeshPlain ( true, m_pQUAD, 1, 1, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwColor );
		SetCullCWCCW ( m_pQUAD, 0 );

		// also apply default shader
		LPSTR pQuadFX = "quad.fx";
		m_pQUADDefaultEffect = new cExternalEffect;
		m_pQUADDefaultEffect->Load ( pQuadFX, false, false );
		if ( SetSpecialEffect ( m_pQUAD, m_pQUADDefaultEffect ) )
		{
			AddObjectMeshToBuffers ( m_pQUAD, true );
			m_pQUAD->bVertexShaderEffectRefOnly = true;
			m_pQUAD->bVBRefreshRequired = true;
			g_vRefreshMeshList.push_back ( m_pQUAD );
		}
		else
			SAFE_DELETE(m_pQUADDefaultEffect);

		// also apply default texture (camera zero image)
		SetBaseTextureStage ( m_pQUAD, 0, iInitialCameraZeroImage );
	}
}

void CObjectManager::SetScreenTextureQUAD ( int iStage, int iImage )
{
	// only if got QUAD
	if ( !m_pQUAD )
		return;

	// texture mesh with image
	SetBaseTextureStage ( m_pQUAD, iStage, iImage );
}

void CObjectManager::SetScreenEffectQUAD ( int iEffectID )
{
	// only if got QUAD
	if ( !m_pQUAD )
		return;

	// check if effect numbe valid
	if ( iEffectID<=0 )
		return;

	// check the effect exists
	if ( !ConfirmEffect ( iEffectID ) )
		return;

	// get effect ptr
	sEffectItem* pEffectItem = m_EffectList [ iEffectID ];

	// assign vertex shader from effect to QUAD
	SetSpecialEffect ( m_pQUAD, NULL );
	ResetVertexDataInMeshPerMesh ( m_pQUAD );
	SetSpecialEffect ( m_pQUAD, pEffectItem->pEffectObj );
	AddObjectMeshToBuffers ( m_pQUAD, true );
	m_pQUAD->bVertexShaderEffectRefOnly = true;
	m_pQUAD->bVBRefreshRequired = true;
	g_vRefreshMeshList.push_back ( m_pQUAD );
}

void CObjectManager::RenderScreenQUAD ( void )
{
	if ( m_pQUAD )
	{
		DrawMesh ( m_pQUAD );
	}
}

int CObjectManager::GetVisibleObjectCount ( void )
{
	// 301007 - new function
	return m_iVisibleObjectCount;
}

sObject** CObjectManager::GetSortedObjectVisibleList ( void )
{
	// 301007 - new function
	return m_ppSortedObjectVisibleList;
}

