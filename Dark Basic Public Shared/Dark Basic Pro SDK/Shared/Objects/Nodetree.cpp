
//
// DBONodetree Functions Implementation
//

//////////////////////////////////////////////////////////////////////////////////////
// INCLUDES //////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
#include "Nodetree.h"
#include "CommonC.h"

//////////////////////////////////////////////////////////////////////////////////////
// EXTERNALS /////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
extern		LPDIRECT3DDEVICE8					m_pD3D;

const int	CP_FRONT	= 1001;
const int	CP_BACK		= 1002;
const int	CP_ONPLANE	= 1003;
const int	CP_SPANNING	= 1004;

#define EPSILON 0.01f

struct D3DLVERTEX
{
    float		x; 
    float		y;    
    float		z;
	float		nx, ny, nz;
	D3DCOLOR	color;
	float		tu;
	float		tv;
};

struct PLANE
{
	D3DXVECTOR3 PointOnPlane;
	D3DXVECTOR3 Normal;
};

struct POLYGON
{
	D3DLVERTEX*	VertexList;
	D3DXVECTOR3 Normal;
	WORD		NumberOfVertices;
	WORD		NumberOfIndices;
	WORD*		Indices;
};


//////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS /////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

extern int	g_iNodeTreeSubDivisionCount;
extern int	g_iNodeTreeMaxTriangleCount;
extern int	g_iNodeTreeMaxSubDivisionCount;

cNodeTree::sObjectList::sObjectList ( )
{
	memset ( this, 0, sizeof ( sObjectList ) );
}

cNodeTree::sObjectList::~sObjectList ( )
{
	SAFE_DELETE ( pTransformedMesh );
	SAFE_DELETE ( pNext );
}

cNodeTree::cNodeTree ( )
{
	memset ( this, 0, sizeof ( this ) );
	m_bDebug = false;
}

cNodeTree::~cNodeTree ( )
{
	// destroy sub nodes
	SAFE_DELETE ( m_pNodes [ 0 ] );
	SAFE_DELETE ( m_pNodes [ 1 ] );
	SAFE_DELETE ( m_pNodes [ 2 ] );
	SAFE_DELETE ( m_pNodes [ 3 ] );
	SAFE_DELETE ( m_pNodes [ 4 ] );
	SAFE_DELETE ( m_pNodes [ 5 ] );
	SAFE_DELETE ( m_pNodes [ 6 ] );
	SAFE_DELETE ( m_pNodes [ 7 ] );
}

void cNodeTree::TurnDebugOn  ( void )
{
	m_bDebug = true;
}

void cNodeTree::TurnDebugOff ( void )
{
	m_bDebug = false;
}

void cNodeTree::DestroyNode ( cNodeTree* pNode )
{
	// ensure node exists
	if ( !pNode )
		return;

	// delete vertex data for node
	SAFE_DELETE_ARRAY ( pNode->m_pVertexList );
	SAFE_DELETE_ARRAY ( pNode->m_pVertices );

	// get object list ptr
	sObjectList* pList = &pNode->m_ObjectList;

	// run through all items in the list
	while ( pList )
	{
		// restore objects to normal mode
		if ( pList->pObjectRef )
		{
			pList->pObjectRef->bStatic = false;
		}

		// delete mesh used to store transformed data
		SAFE_DELETE ( pList->pTransformedMesh );

		// move to the next item in the list
		pList = pList->pNext;
	}

	// reset vertex count
	m_iVertexCount = 0;
	
	// delete child nodes to this one
	DestroyNode ( pNode->m_pNodes [ 0 ] );
	DestroyNode ( pNode->m_pNodes [ 1 ] );
	DestroyNode ( pNode->m_pNodes [ 2 ] );
	DestroyNode ( pNode->m_pNodes [ 3 ] );
	DestroyNode ( pNode->m_pNodes [ 4 ] );
	DestroyNode ( pNode->m_pNodes [ 5 ] );
	DestroyNode ( pNode->m_pNodes [ 6 ] );
	DestroyNode ( pNode->m_pNodes [ 7 ] );
}

bool cNodeTree::Destroy ( void )
{
// lee - 040303 - never responsible for new object creation (apart from one global)
//	if ( m_pGlobalObject )
//	{
//		if ( m_pGlobalObject->iFrameCount )
//		{
//			m_ObjectManager.RemoveObjectFromBuffers ( m_pGlobalObject );
//			DeleteMesh ( ( sObject** ) &m_pGlobalObject );
//		}
//
//		SAFE_DELETE ( m_pGlobalObject );
//	}
	SAFE_DELETE ( m_pGlobalObject );
	
	DestroyNode ( this );

	SAFE_DELETE ( m_pNodes [ 0 ] );
	SAFE_DELETE ( m_pNodes [ 1 ] );
	SAFE_DELETE ( m_pNodes [ 2 ] );
	SAFE_DELETE ( m_pNodes [ 3 ] );
	SAFE_DELETE ( m_pNodes [ 4 ] );
	SAFE_DELETE ( m_pNodes [ 5 ] );
	SAFE_DELETE ( m_pNodes [ 6 ] );
	SAFE_DELETE ( m_pNodes [ 7 ] );
	
	return true;
}

void cNodeTree::ClearForNewNodeUpdate ( void )
{
	// delete child nodes to this one
	for( int n=0; n<8; n++)
	{
		DestroyNode ( m_pNodes [ n ] );
		m_pNodes [ n ] = NULL;
	}

	// clear lists
	m_pMeshList.clear();
	m_pGlobalMeshList.clear();
}

bool cNodeTree::AddObject ( sObject* pObject )
{
	// add an object into the scene

	// check the pointer is valid
	SAFE_MEMORY ( pObject );

	// add whole object to node tree
	AddObjectToList ( pObject );

	// object is now part of node tree
	pObject->bStatic = true;

	// success
	return false;
}

bool cNodeTree::AddObjectToList ( sObject* pObject )
{
	// add an object into the object list, we will need to setup
	// the linked list and then store a pointer to this object,

	// check the pointer is valid
	SAFE_MEMORY ( pObject );

	// get a pointer to the main list
	sObjectList* pObjectList = &m_ObjectList;

	// leefix - 020303 - convert cloned object for node process
	// we then create a mesh to hold the vertex data for transforming
	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		// get ptr to mesh
		sMesh* pOriginalMesh = pObject->ppMeshList [ iMesh ];

		// move through the list until we find the end
		while ( pObjectList )
		{
			if ( pObjectList->pNext )
				pObjectList = pObjectList->pNext;
			else
				break;
		}

		// setup current node
		pObjectList->pObjectRef = pObject;
		pObjectList->pTransformedMesh = new sMesh;
		MakeMeshFromOtherMesh ( true, pObjectList->pTransformedMesh, pOriginalMesh, NULL );

		// create next pointer
		pObjectList->pNext = new sObjectList;		
		SAFE_MEMORY ( pObjectList->pNext );

		// add mesh to list
		AddMeshToList ( pObject, pObjectList->pTransformedMesh );
	}
	
	// success
	return true;
}

bool cNodeTree::AddMeshToList ( sObject* pObject, sMesh* pMesh )
{
	// ensure all meshes in cloned object are not index based (triangle=3verts)
	ConvertLocalMeshToFVF ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );

	// ensure all meshes in cloned object are not strips (spheres)
	ConvertLocalMeshToTriList ( pMesh );

	// deal with textures inside mesh also...
	// ...todo...

	// transform all vert data in cloned object
	TransformVertices ( pObject, pMesh );

	// success
	return true;
}

void cNodeTree::TransformVertices ( sObject* pObject, sMesh* pMesh )
{
	// get the offset map for the vertices
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh->dwFVF, &offsetMap );

	// now we need to transform the vertices for each object
	for ( int iVertex = 0; iVertex < pMesh->dwVertexCount; iVertex++ )
	{
		// get a pointer to the vertex data
		BYTE* pVertex = pMesh->pVertexData;

		// convert the position into a vector so we can perform the transform
		D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
												*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
												*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
												*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
											  );

		// transform the position by it's world matrix
		D3DXVec3TransformCoord ( &vecPosition, &vecPosition, &pObject->position.matWorld );

		*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecPosition.x;
		*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecPosition.y;
		*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecPosition.z;
	}
}

bool cNodeTree::GetCentrePoint ( D3DXVECTOR3* pVecCentre, int* piVertexCount )
{
	SAFE_MEMORY ( pVecCentre    );
	SAFE_MEMORY ( piVertexCount );

	int			 iCurrentMesh   = 0;								// current mesh being accessed
	int			 iCurrentVertex = 0;								// current vertex being accessed
	sObjectList* pList;
	sOffsetMap	 offsetMap;

	// get the address of the main object list
	pList = &m_ObjectList;

	// run through the object list
	while ( pList )
	{
		// get the copied pMesh
		sMesh* pMesh = pList->pTransformedMesh;
		
		// check the pointer is valid
		if ( !pMesh )
			break;

		// get the offset map for the FVF
		GetFVFOffsetMap ( pMesh->dwFVF, &offsetMap );

		// for each mesh go through all of the vertices
		for ( iCurrentVertex = 0; iCurrentVertex < ( int ) pMesh->dwVertexCount; iCurrentVertex++ )
		{
			// make sure we have position data in the vertices
			if ( pMesh->dwFVF & D3DFVF_XYZ )
			{
				// get the x, y and z components
				float fX = *( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );
				float fY = *( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iCurrentVertex ) );
				float fZ = *( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iCurrentVertex ) );

				// add the vertices to the centre vector
				pVecCentre->x += fX;
				pVecCentre->y += fY;
				pVecCentre->z += fZ;

				// increment the vertex count
				*piVertexCount += 1;
			}
		}

		// move to the next item in the list
		pList = pList->pNext;
	}

	// divide the centre vector by the number of vertices to find
	// the centre point of the scene
	pVecCentre->x /= *piVertexCount;
	pVecCentre->y /= *piVertexCount;	
	pVecCentre->z /= *piVertexCount;

	// success
	return true;
}

bool cNodeTree::GetBounds ( D3DXVECTOR3 vecCentre, float* pfWidth, float* pfHeight, float* pfDepth )
{
	SAFE_MEMORY ( pfWidth  );
	SAFE_MEMORY ( pfHeight );
	SAFE_MEMORY ( pfDepth  );

	int			 iCurrentMesh   = 0;
	int			 iCurrentVertex = 0;
	sObjectList* pList;
	sOffsetMap	 offsetMap;

	// get the address of the main object list
	pList = &m_ObjectList;

	// run through the object list
	while ( pList )
	{
		// get the copied pMesh
		sMesh* pMesh = pList->pTransformedMesh;

		// check the pointer is valid
		if ( !pMesh )
			break;

		// get the offset map for the FVF
		GetFVFOffsetMap ( pMesh->dwFVF, &offsetMap );

		// for each mesh go through all of the vertices
		for ( iCurrentVertex = 0; iCurrentVertex < ( int ) pMesh->dwVertexCount; iCurrentVertex++ )
		{
			// make sure we have position data in the vertices
			if ( pMesh->dwFVF & D3DFVF_XYZ )
			{
				// get the x, y and z components
				float fX = *( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );
				float fY = *( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iCurrentVertex ) );
				float fZ = *( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iCurrentVertex ) );

				int iCurrentWidth  = abs ( fX - vecCentre.x );
				int iCurrentHeight = abs ( fY - vecCentre.y );
				int iCurrentDepth  = abs ( fZ - vecCentre.z );

				if ( iCurrentWidth > *pfWidth )
					*pfWidth = iCurrentWidth;

				if ( iCurrentHeight > *pfHeight )
					*pfHeight = iCurrentHeight;

				if ( iCurrentDepth > *pfDepth )
					*pfDepth  = iCurrentDepth;
			}
		}

		// move to the next item in the list
		pList = pList->pNext;
	}

	// modify
	*pfWidth  *= 2.1f;
	*pfHeight *= 2.1f;
	*pfDepth  *= 2.1f;

	if ( *pfWidth > *pfHeight && *pfWidth > *pfDepth )
	{
		m_fWidth = *pfWidth;
	}
	else if ( *pfHeight > *pfWidth && *pfHeight > *pfDepth )
	{
		m_fWidth = *pfHeight;
	}
	else
		m_fWidth = *pfDepth;

	// success
	return true;
}

bool cNodeTree::GetBoundingBox ( void )
{
	// get the bounding box for the scene

	float		 fMaxWidth      = 0.0f;								// width
	float		 fMaxHeight     = 0.0f;								// height
	float		 fMaxDepth      = 0.0f;								// depth
	int			 iCurrentMesh   = 0;								// current mesh being accessed
	int			 iCurrentVertex = 0;								// current vertex being accessed
	int			 iVertexCount   = 0;
	D3DXVECTOR3  vecCentre      = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );	// centre point
	sObjectList* pList          = NULL;								// list for objects
	
	// get the centre point
	this->GetCentrePoint ( &m_vecCentre, &iVertexCount );

	// get the bounds of the scene
	this->GetBounds ( m_vecCentre, &m_vecDimension.x, &m_vecDimension.y, &m_vecDimension.z );
	
	return true;
}

int cNodeTree::GetVertexCount ( void )
{
	// get the number of vertices in the object list

	// variable declarations
	sObjectList* pList  = NULL;		// object list
	int          iCount = 0;		// vertex counter
	
	// get the address of the main object list
	pList = &m_ObjectList;

	// run through all items in the list
	while ( pList )
	{
		// get the copied pMesh
		sMesh* pMesh = pList->pTransformedMesh;

		// check the pointer is valid
		if ( !pMesh )
			break;

		// increment the vertex count
		iCount += ( int ) pMesh->dwVertexCount;

		// move to the next item in the list
		pList = pList->pNext;
	}

	// return the number of vertices counted
	return iCount;
}

bool cNodeTree::CreateVertexList ( void )
{
	// creates an array of vectors which will hold the vertices

	// set up variables
	m_iVertexCount   = 0;
	m_iTriangleCount = 0;

	sObjectList* pList;
	sOffsetMap	 offsetMap;
	int          iVertex = 0;

	// setup vertices
	m_iVertexCount   = this->GetVertexCount ( );	// get the number of vertices in the scene
	m_iTriangleCount = m_iVertexCount / 3;			// number of triangles

	m_pVertexList = new sDataList [ m_iVertexCount ];
	memset ( m_pVertexList, 0, sizeof ( sDataList ) * m_iVertexCount );

	// get the address of the main object list
	pList = &m_ObjectList;

	// run through the object list
	while ( pList )
	{
		// get the copied object
		sMesh* pMesh = pList->pTransformedMesh;

		// check the pointer is valid
		if ( !pMesh )
			break;

		// get the offset map for the FVF
		GetFVFOffsetMap ( pMesh->dwFVF, &offsetMap );

		// for each mesh go through all of the vertices
		for ( int iCurrentVertex = 0; iCurrentVertex < ( int ) pMesh->dwVertexCount; iCurrentVertex++ )
		{
			// make sure we have position data in the vertices
			if ( pMesh->dwFVF & D3DFVF_XYZ )
			{
				// get the x, y and z components
				float fX = *( ( float* ) pMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) );
				float fY = *( ( float* ) pMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iCurrentVertex ) );
				float fZ = *( ( float* ) pMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iCurrentVertex ) );

				m_pVertexList [ iVertex ].pMeshRef		= pMesh;
				m_pVertexList [ iVertex ].pObjectRef	= pList->pObjectRef;
				m_pVertexList [ iVertex ].vecVertex		= D3DXVECTOR3 ( fX, fY, fZ );
				iVertex++;
			}
		}

		// move to the next item in the list
		pList = pList->pNext;
	}

	// success
	return true;
}

bool cNodeTree::CreateNode ( sDataList* pData, int iVertexCount, D3DXVECTOR3 vecCentre, float fWidth )
{
	int iTriangleCount = iVertexCount / 3;

	m_vecCentre = vecCentre;
	m_fWidth    = fWidth;

	if ( ( iTriangleCount > g_iNodeTreeMaxTriangleCount ) && ( g_iNodeTreeSubDivisionCount < g_iNodeTreeMaxSubDivisionCount ) )
	{
		m_bSubDivided = true;

		vector < bool > pList1 ( iTriangleCount );	// top left front
		vector < bool > pList2 ( iTriangleCount );	// top left back
		vector < bool > pList3 ( iTriangleCount );	// top right back
		vector < bool > pList4 ( iTriangleCount );	// top right front
		vector < bool > pList5 ( iTriangleCount );	// bottom left front
		vector < bool > pList6 ( iTriangleCount );	// bottom left back
		vector < bool > pList7 ( iTriangleCount );	// bottom right back
		vector < bool > pList8 ( iTriangleCount );	// bottom right front

		for ( int iVertex = 0; iVertex < iVertexCount; iVertex++ )
		{
			D3DXVECTOR3 vec = pData [ iVertex ].vecVertex;

			if ( ( vec.x <= m_vecCentre.x ) && ( vec.y >= m_vecCentre.y ) && ( vec.z >= m_vecCentre.z ) ) 
			{
				// top left front
				pList1 [ iVertex / 3 ] = true;
			}
			
			if ( ( vec.x <= m_vecCentre.x ) && ( vec.y >= m_vecCentre.y ) && ( vec.z <= m_vecCentre.z ) ) 
			{
				// top left back
				pList2 [ iVertex / 3 ] = true;
			}

			if ( ( vec.x >= m_vecCentre.x ) && ( vec.y >= m_vecCentre.y ) && ( vec.z <= m_vecCentre.z ) ) 
			{
				// top right back
				pList3 [ iVertex / 3 ] = true;
			}

			if ( ( vec.x >= m_vecCentre.x ) && ( vec.y >= m_vecCentre.y ) && ( vec.z >= m_vecCentre.z ) ) 
			{
				// top right front
				pList4 [ iVertex / 3 ] = true;
			}

			if ( ( vec.x <= m_vecCentre.x ) && ( vec.y <= m_vecCentre.y ) && ( vec.z >= m_vecCentre.z ) ) 
			{
				// bottom left front
				pList5 [ iVertex / 3 ] = true;
			}

			if ( ( vec.x <= m_vecCentre.x ) && ( vec.y <= m_vecCentre.y ) && ( vec.z <= m_vecCentre.z ) ) 
			{
				// bottom left back
				pList6 [ iVertex / 3 ] = true;
			}

			if ( ( vec.x >= m_vecCentre.x ) && ( vec.y <= m_vecCentre.y ) && ( vec.z <= m_vecCentre.z ) ) 
			{
				// bottom right back
				pList7 [ iVertex / 3 ] = true;
			}

			if ( ( vec.x >= m_vecCentre.x ) && ( vec.y <= m_vecCentre.y ) && ( vec.z >= m_vecCentre.z ) ) 
			{
				// bottom right front
				pList8 [ iVertex / 3 ] = true;
			}
		}

		int triCount1 = 0;	int triCount2 = 0;	int triCount3 = 0;	int triCount4 = 0;
		int triCount5 = 0;	int triCount6 = 0;	int triCount7 = 0;	int triCount8 = 0;
		
		for ( int iTriangle = 0; iTriangle < iTriangleCount; iTriangle++ )
		{
			if ( pList1 [ iTriangle ] )
				triCount1++;
			
			if ( pList2 [ iTriangle ] )
				triCount2++;

			if ( pList3 [ iTriangle ] )
				triCount3++;
			
			if ( pList4 [ iTriangle ] )
				triCount4++;

			if ( pList5 [ iTriangle ] )
				triCount5++;
			
			if ( pList6 [ iTriangle ] )
				triCount6++;

			if ( pList7 [ iTriangle ] )
				triCount7++;
			
			if ( pList8 [ iTriangle ] )
				triCount8++;
		}

		CreateNewNode ( pData, pList1, iVertexCount, vecCentre, fWidth, triCount1, 0 );
		CreateNewNode ( pData, pList2, iVertexCount, vecCentre, fWidth, triCount2, 1 );
		CreateNewNode ( pData, pList3, iVertexCount, vecCentre, fWidth, triCount3, 2 );
		CreateNewNode ( pData, pList4, iVertexCount, vecCentre, fWidth, triCount4, 3 );
		CreateNewNode ( pData, pList5, iVertexCount, vecCentre, fWidth, triCount5, 4 );
		CreateNewNode ( pData, pList6, iVertexCount, vecCentre, fWidth, triCount6, 5 );
		CreateNewNode ( pData, pList7, iVertexCount, vecCentre, fWidth, triCount7, 6 );
		CreateNewNode ( pData, pList8, iVertexCount, vecCentre, fWidth, triCount8, 7 );
	}
	else
	{
		this->AssignVerticesToNode ( pData, iVertexCount );
	}

	return true;
}

D3DXVECTOR3 cNodeTree::GetNewNodeCentre ( D3DXVECTOR3 vecCentre, float fWidth, int iID )
{
	D3DXVECTOR3 vecNodeCentre = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	
	switch ( iID )
	{
		case 0:
			vecNodeCentre = D3DXVECTOR3 ( vecCentre.x - fWidth / 4, vecCentre.y + fWidth / 4, vecCentre.z + fWidth / 4 );
		break;

		case 1:
			vecNodeCentre = D3DXVECTOR3 ( vecCentre.x - fWidth / 4, vecCentre.y + fWidth / 4, vecCentre.z - fWidth / 4 );
		break;

		case 2:
			vecNodeCentre = D3DXVECTOR3 ( vecCentre.x + fWidth / 4, vecCentre.y + fWidth / 4, vecCentre.z - fWidth / 4);
		break;

		case 3:
			vecNodeCentre = D3DXVECTOR3 ( vecCentre.x + fWidth / 4, vecCentre.y + fWidth / 4, vecCentre.z + fWidth / 4 );
		break;

		case 4:
			vecNodeCentre = D3DXVECTOR3 ( vecCentre.x - fWidth / 4, vecCentre.y - fWidth / 4, vecCentre.z + fWidth / 4 );
		break;

		case 5:
			vecNodeCentre = D3DXVECTOR3 ( vecCentre.x - fWidth / 4, vecCentre.y - fWidth / 4, vecCentre.z - fWidth / 4 );
		break;

		case 6:
			vecNodeCentre = D3DXVECTOR3 ( vecCentre.x + fWidth / 4, vecCentre.y - fWidth / 4, vecCentre.z - fWidth / 4 );
		break;

		case 7:
			vecNodeCentre = D3DXVECTOR3 ( vecCentre.x + fWidth / 4, vecCentre.y - fWidth / 4, vecCentre.z + fWidth / 4 );
		break;
	}

	return vecNodeCentre;
}

void cNodeTree::CreateNewNode ( sDataList* pData, vector < bool > pList, int iVertexCount, D3DXVECTOR3 vecCentre, float fWidth, int iTriangleCount, int iID )
{
	if ( !iTriangleCount )
		return;

	sDataList* pNodeVertices = new sDataList [ iTriangleCount * 3 ];
	
	int iIndex = 0;

	for ( int iVertex = 0; iVertex < iVertexCount; iVertex++ )
	{
		if ( pList [ iVertex / 3 ] )
		{
			pNodeVertices [ iIndex ].pObjectRef		= pData [ iVertex ].pObjectRef;
			pNodeVertices [ iIndex ].pMeshRef		= pData [ iVertex ].pMeshRef;
			pNodeVertices [ iIndex ].vecVertex		= pData [ iVertex ].vecVertex;
			iIndex++;
		}
	}

	m_pNodes [ iID ] = new cNodeTree;

	memset ( m_pNodes [ iID ], 0, sizeof ( cNodeTree ) );

	m_pNodes [ iID ]->m_bSubDivided = false;
	m_pNodes [ iID ]->m_fWidth      = 0.0f;
	m_pNodes [ iID ]->m_vecCentre   = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );

	
	D3DXVECTOR3 vecNodeCentre = GetNewNodeCentre ( vecCentre, fWidth, iID );

	g_iNodeTreeSubDivisionCount++;

	m_pNodes [ iID ]->CreateNode ( pNodeVertices, iTriangleCount * 3, vecNodeCentre, fWidth / 2 );
	
	g_iNodeTreeSubDivisionCount--;

	SAFE_DELETE_ARRAY ( pNodeVertices );
}

void cNodeTree::AssignVerticesToNode ( sDataList* pData, int iVertexCount )
{
	m_bSubDivided    = false;
	m_iTriangleCount = iVertexCount / 3;
	m_pVertices      = new sDataList [ iVertexCount ];

	memset ( m_pVertices, 0,     sizeof ( sDataList ) * iVertexCount );
	memcpy ( m_pVertices, pData, sizeof ( sDataList ) * iVertexCount );
}

bool cNodeTree::Update ( void )
{
	// updates the nodetree with all objects

	// must clear node first or duplication errors occur
	ClearForNewNodeUpdate();

	// get a bounding box for all objects (produces m_fWidth)
	if ( !GetBoundingBox ( ) )
		return false;

	// create the vertex list
	if ( !CreateVertexList ( ) )
		return false;

	// create the nodes
	if ( !CreateNode ( m_pVertexList, m_iVertexCount, m_vecCentre, m_fWidth ) )
		return false;

	// rebuild meshes
	RebuildMesh         ( this );

	// rebuild mesh list
	BuildGlobalMeshList ( this );

	// rebuild final object
	BuildGlobalObject   ( );

	return true;
}

void cNodeTree::BuildGlobalObject ( void )
{
	// build an sObject which contains all of the meshes within the node tree

	// ensure old global is deleted
	SAFE_DELETE(m_pGlobalObject);

	// create new object
	m_pGlobalObject         = new sObject;
	sFrame* pCurrentFrame	= NULL;
	
	// go through global mesh lit and produce frames within global object
	for ( int iFrame = 0; iFrame < m_pGlobalMeshList.size ( ); iFrame++ )
	{
		// create frame
		sFrame* pNewFrame = new sFrame;

		// assign frame ptr
		pNewFrame->pMesh = m_pGlobalMeshList [ iFrame ];

		// assign vertex data to frame
		pNewFrame->pMesh->iDrawVertexCount = pNewFrame->pMesh->dwVertexCount;
		pNewFrame->pMesh->iDrawPrimitives  = pNewFrame->pMesh->dwIndexCount / 3;

// lee - 040303 - this method always creates a redundant frame at end!
//		// create child for next iteration
//		pFrame->pChild = new sFrame;

		// assign new frame
		if ( m_pGlobalObject->pFrame==NULL )
			m_pGlobalObject->pFrame = pNewFrame;
		else
			pCurrentFrame->pSibling = pNewFrame;

		// current frame
		pCurrentFrame = pNewFrame;
	}

	// when all frames filled, calcilate mesh/frame lists
	CreateFrameAndMeshList ( m_pGlobalObject );

	// calculate bounds to entire node tree object
	CalculateAllBounds ( m_pGlobalObject, false );

	// add all meshes of the global object to the VB/IB buffers
	m_ObjectManager.AddObjectToBuffers ( m_pGlobalObject );
}

void cNodeTree::BuildGlobalMeshList ( cNodeTree* pNode )
{
	// build a list of all meshes contained within the node tree, we do this
	// so we can create an sObject which will contain all of the data

	// return when the node is invalid
	if ( !pNode )
		return;

	// run through meshes within the current node
	for ( int iMesh = 0; iMesh < pNode->m_pMeshList.size ( ); iMesh++ )
	{
		// add the mesh if it's valid
		if ( pNode->m_pMeshList [ iMesh ]->dwVertexCount )
			m_pGlobalMeshList.push_back ( pNode->m_pMeshList [ iMesh ] );
	}
	
	// run through sub nodes
	BuildGlobalMeshList ( pNode->m_pNodes [ 0 ] );
	BuildGlobalMeshList ( pNode->m_pNodes [ 1 ] );
	BuildGlobalMeshList ( pNode->m_pNodes [ 2 ] );
	BuildGlobalMeshList ( pNode->m_pNodes [ 3 ] );
	BuildGlobalMeshList ( pNode->m_pNodes [ 4 ] );
	BuildGlobalMeshList ( pNode->m_pNodes [ 5 ] );
	BuildGlobalMeshList ( pNode->m_pNodes [ 6 ] );
	BuildGlobalMeshList ( pNode->m_pNodes [ 7 ] );
}

void cNodeTree::DrawDebug ( cNodeTree* pNode )
{
	if ( !pNode )
		return;

	DWORD dwFVF = 0;

	m_pD3D->GetVertexShader ( &dwFVF );
	m_pD3D->SetVertexShader ( D3DFVF_XYZ );

	D3DXVECTOR3 vecCentre = pNode->m_vecCentre;
	float       fSize     = pNode->m_fWidth;

	float fWidth  = fSize / 2.0f;
	float fHeight = fSize / 2.0f;
	float fDepth  = fSize / 2.0f;

	D3DXVECTOR3 vecTopLeftFront     = D3DXVECTOR3 ( vecCentre.x - fWidth, vecCentre.y + fHeight, vecCentre.z + fDepth );
	D3DXVECTOR3 vecTopLeftBack      = D3DXVECTOR3 ( vecCentre.x - fWidth, vecCentre.y + fHeight, vecCentre.z - fDepth );
	D3DXVECTOR3 vecTopRightBack     = D3DXVECTOR3 ( vecCentre.x + fWidth, vecCentre.y + fHeight, vecCentre.z - fDepth );
	D3DXVECTOR3 vecTopRightFront    = D3DXVECTOR3 ( vecCentre.x + fWidth, vecCentre.y + fHeight, vecCentre.z + fDepth );
	D3DXVECTOR3 vecBottomLeftFront  = D3DXVECTOR3 ( vecCentre.x - fWidth, vecCentre.y - fHeight, vecCentre.z + fDepth );
	D3DXVECTOR3 vecBottomLeftBack   = D3DXVECTOR3 ( vecCentre.x - fWidth, vecCentre.y - fHeight, vecCentre.z - fDepth );
	D3DXVECTOR3 vecBottomRightBack  = D3DXVECTOR3 ( vecCentre.x + fWidth, vecCentre.y - fHeight, vecCentre.z - fDepth );
	D3DXVECTOR3 vecBottomRightFront = D3DXVECTOR3 ( vecCentre.x + fWidth, vecCentre.y - fHeight, vecCentre.z + fDepth );

	D3DXVECTOR3 vecLines [ 24 ];

	vecLines [ 0 ] = vecTopLeftFront;		vecLines [ 1 ] = vecTopRightFront;
	vecLines [ 2 ] = vecTopLeftBack;  		vecLines [ 3 ] = vecTopRightBack;
	vecLines [ 4 ] = vecTopLeftFront;		vecLines [ 5 ] = vecTopLeftBack;
	vecLines [ 6 ] = vecTopRightFront;		vecLines [ 7 ] = vecTopRightBack;

	vecLines [  8 ] = vecBottomLeftFront;	vecLines [  9 ] = vecBottomRightFront;
	vecLines [ 10 ] = vecBottomLeftBack;	vecLines [ 11 ] = vecBottomRightBack;
	vecLines [ 12 ] = vecBottomLeftFront;	vecLines [ 13 ] = vecBottomLeftBack;
	vecLines [ 14 ] = vecBottomRightFront;	vecLines [ 15 ] = vecBottomRightBack;

	vecLines [ 16 ] = vecTopLeftFront;		vecLines [ 17 ] = vecBottomLeftFront;
	vecLines [ 18 ] = vecTopLeftBack;		vecLines [ 19 ] = vecBottomLeftBack;
	vecLines [ 20 ] = vecTopRightBack;		vecLines [ 21 ] = vecBottomRightBack;
	vecLines [ 22 ] = vecTopRightFront;		vecLines [ 23 ] = vecBottomRightFront;
	
	m_pD3D->DrawPrimitiveUP (
								D3DPT_LINELIST,
								12,
								&vecLines,
								sizeof ( float ) * 3
							);

	m_pD3D->SetVertexShader ( dwFVF );

	DrawDebug ( pNode->m_pNodes [ 0 ] );
	DrawDebug ( pNode->m_pNodes [ 1 ] );
	DrawDebug ( pNode->m_pNodes [ 2 ] );
	DrawDebug ( pNode->m_pNodes [ 3 ] );
	DrawDebug ( pNode->m_pNodes [ 4 ] );
	DrawDebug ( pNode->m_pNodes [ 5 ] );
	DrawDebug ( pNode->m_pNodes [ 6 ] );
	DrawDebug ( pNode->m_pNodes [ 7 ] );
}

void cNodeTree::DrawNode ( cNodeTree* pNode )
{
	// draw a node

	// check the node
	if ( !pNode )
		return;

	// make sure the node is visible
	if ( !CheckCube ( pNode->m_vecCentre.x, pNode->m_vecCentre.y, pNode->m_vecCentre.z, pNode->m_fWidth / 2 ) )
		return;

	// run through all meshes contained within the node
	for ( int iMesh = 0; iMesh < pNode->m_pMeshList.size ( ); iMesh++ )
	{
		// get the mesh
		sMesh* pTestMesh = pNode->m_pMeshList [ iMesh ];

		// check the mesh is visible
		if ( !CheckSphere ( pTestMesh->Collision.vecCentre.x, pTestMesh->Collision.vecCentre.y, pTestMesh->Collision.vecCentre.z, pTestMesh->Collision.fRadius ) )
			continue;
		
		// make sure we have some vertices
		if ( pTestMesh->dwVertexCount )
		{
			m_ObjectManager.DrawMesh ( pNode->m_pMeshList [ iMesh ] );
		}
	}

	// draw sub nodes
	DrawNode ( pNode->m_pNodes [ 0 ] );
	DrawNode ( pNode->m_pNodes [ 1 ] );
	DrawNode ( pNode->m_pNodes [ 2 ] );
	DrawNode ( pNode->m_pNodes [ 3 ] );
	DrawNode ( pNode->m_pNodes [ 4 ] );
	DrawNode ( pNode->m_pNodes [ 5 ] );
	DrawNode ( pNode->m_pNodes [ 6 ] );
	DrawNode ( pNode->m_pNodes [ 7 ] );
}

bool cNodeTree::Render ( void )
{
	// get object list ptr
	sObjectList* pList = &m_ObjectList;

	// create a scaling and position matrix
	D3DXMATRIX matScale, matTranslation, matRotation, matWorld, matRotateX, matRotateY, matRotateZ;
	D3DXMatrixScaling ( &matScale, 1.0f, 1.0f, 1.0f );
	D3DXMatrixTranslation ( &matTranslation, 0.0f, 0.0f, 0.0f );

	// handle rotations
	D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( 0.0f ) );
	D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( 0.0f ) );
	D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( 0.0f ) );

	// build up final rotation and world matrix
	matRotation = matRotateX * matRotateY * matRotateZ;
	matWorld    = matRotation * matScale * matTranslation;

	// set world transform for node tree base
	m_pD3D->SetTransform ( D3DTS_WORLD, &matWorld );

	// draw debug wires
	if ( m_bDebug )
		this->DrawDebug ( this );

	// draw node tree
	this->DrawNode ( this );

	// success
	return false;
}

bool GetIntersect ( D3DXVECTOR3* vecLineStart, D3DXVECTOR3* vecLineEnd, D3DXVECTOR3* pVertex, D3DXVECTOR3* pVecNormal, D3DXVECTOR3* pVecIntersection, float* pPercentage )
{
	// return the intersection point between a ray and a plane
	D3DXVECTOR3 vecDirection;
	D3DXVECTOR3 vecL1;
	float	    fLineLength;
	float		fDistanceFromPlane;

	vecDirection.x = vecLineEnd->x - vecLineStart->x;
	vecDirection.y = vecLineEnd->y - vecLineStart->y;
	vecDirection.z = vecLineEnd->z - vecLineStart->z;

	fLineLength = D3DXVec3Dot ( &vecDirection, pVecNormal );

	if ( fabsf ( fLineLength ) < EPSILON )
		return false; 

	vecL1.x = pVertex->x - vecLineStart->x;
	vecL1.y = pVertex->y - vecLineStart->y;
	vecL1.z = pVertex->z - vecLineStart->z;

	fDistanceFromPlane = D3DXVec3Dot ( &vecL1, pVecNormal );

	// how far from linestart, intersection is as a percentage of 0 to 1 
	*pPercentage = fDistanceFromPlane / fLineLength; 

	// the plane is behind the start of the line or the line does not reach the plane
	if ( *pPercentage < 0.0f || *pPercentage > 1.0f )
		return false;

	// add the percentage of the line to line start
	pVecIntersection->x = vecLineStart->x + vecDirection.x * ( *pPercentage );
	pVecIntersection->y = vecLineStart->y + vecDirection.y * ( *pPercentage );
	pVecIntersection->z = vecLineStart->z + vecDirection.z * ( *pPercentage );

	return true;
}

int ClassifyPoint ( D3DXVECTOR3* pVecPos, PLANE* pPlane ) 
{
	// given a plane and a point return the points position
	// relative to the plane itself - front, back, on

	float		 fResult;
	D3DXVECTOR3* vec1		  = &pPlane->PointOnPlane;
	D3DXVECTOR3  vecDirection = ( *vec1 ) - ( *pVecPos );

	fResult = D3DXVec3Dot ( &vecDirection, &pPlane->Normal );

	if ( fResult < -EPSILON )
		return CP_FRONT;

	if ( fResult > EPSILON )
		return CP_BACK;

	return CP_ONPLANE;
}

void SplitPolygon ( POLYGON* Poly, PLANE* Plane, POLYGON* FrontSplit, POLYGON* BackSplit )
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/*
		code taken from BSP DLL, need to put this in one place to be shared
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// this function is used to do ALL the clipping and splitting of portals 
	// and polygons, it takes a polygon and a plane and splits 
	// the polygon into to two seperate polygons, when used for clipping to 
	// a plane, call this function and then simple discard the front or
	// back

	D3DLVERTEX		FrontList [ 50 ];
	D3DLVERTEX		BackList  [ 50 ];
	
	int				FrontCounter = 0;
	int				BackCounter  = 0;

	int				PointLocation [ 50 ];
	
	int				InFront       = 0;
	int				Behind        = 0;
	int				OnPlane       = 0;
	int				CurrentVertex = 0;
	int				Location      = 0;

	// determine each points location relative to the plane
	for ( int i = 0; i < Poly->NumberOfVertices; i++ )
	{
		Location = ClassifyPoint ( ( D3DXVECTOR3* ) &Poly->VertexList [ i ], Plane );

		if ( Location == CP_FRONT )
		{
			InFront++;
		}
		else if ( Location == CP_BACK )
		{
			Behind++;
		}
		else
		{
			OnPlane++;
		}

		PointLocation [ i ] = Location;
	}

	if ( !InFront )
	{
		memcpy ( BackList, Poly->VertexList, Poly->NumberOfVertices * sizeof ( D3DLVERTEX ) );
		BackCounter = Poly->NumberOfVertices;
	}

	if ( !Behind )
	{
		memcpy ( FrontList, Poly->VertexList, Poly->NumberOfVertices * sizeof ( D3DLVERTEX ) );
		FrontCounter = Poly->NumberOfVertices;
	}

	if ( InFront && Behind )
	{
		for ( i = 0; i < Poly->NumberOfVertices; i++ )
		{
			// store current vertex remembering to MOD with number of vertices
			CurrentVertex = ( i + 1 ) % Poly->NumberOfVertices;

			if ( PointLocation [ i ] == CP_ONPLANE )
			{
				FrontList [ FrontCounter ] = Poly->VertexList [ i ];
				FrontCounter++;

				BackList [ BackCounter ] = Poly->VertexList [ i ];
				BackCounter++;

				continue;
			}

			if ( PointLocation [ i ] == CP_FRONT )
			{
				FrontList [ FrontCounter ] = Poly->VertexList [ i ];
				FrontCounter++;
			}
			else
			{
				BackList [ BackCounter ] = Poly->VertexList [ i ];
				BackCounter++;
			}
			
			// if the next vertex is not causing us to span the plane then continue
			if ( PointLocation [ CurrentVertex ] == CP_ONPLANE || PointLocation [ CurrentVertex ] == PointLocation [ i ] )
				continue;
			
			// Otherwise create the new vertex
			D3DXVECTOR3 IntersectPoint;
			float		percent;

			GetIntersect ( 
							( D3DXVECTOR3* ) &Poly->VertexList [ i ],
							( D3DXVECTOR3* ) &Poly->VertexList [ CurrentVertex ],
							&Plane->PointOnPlane,
							&Plane->Normal,
							&IntersectPoint,
							&percent
						  );

			// create new vertex and calculate new texture coordinate
			D3DLVERTEX copy;
			float deltax	= Poly->VertexList [ CurrentVertex ].tu - Poly->VertexList [ i ].tu;
			float deltay	= Poly->VertexList [ CurrentVertex ].tv - Poly->VertexList [ i ].tv;
			float texx		= Poly->VertexList [ i ].tu + ( deltax * percent );
			float texy		= Poly->VertexList [ i ].tv + ( deltay * percent );
			copy.x			= IntersectPoint.x; 
			copy.y          = IntersectPoint.y;
			copy.z          = IntersectPoint.z;
			copy.color		= Poly->VertexList [ i ].color;
			copy.tu			= texx;
			copy.tv         = texy;

			copy.nx		    = Poly->VertexList [ i ].nx;
			copy.ny		    = Poly->VertexList [ i ].ny;
			copy.nz		    = Poly->VertexList [ i ].nz;
	
			BackList  [ BackCounter++  ] = copy;			
			FrontList [ FrontCounter++ ] = copy;
		}
	}

	// reserve memory for front and back vertex lists 
	if ( FrontCounter )
		FrontSplit->VertexList = new D3DLVERTEX [ FrontCounter ];

	if ( BackCounter )
		BackSplit->VertexList = new D3DLVERTEX [ BackCounter ];

	FrontSplit->NumberOfVertices = 0;
	BackSplit->NumberOfVertices  = 0;

	// copy over the vertices into the new polys
	if ( FrontCounter )
	{
		FrontSplit->NumberOfVertices = FrontCounter;
		memcpy ( FrontSplit->VertexList, FrontList, FrontCounter * sizeof ( D3DLVERTEX ) );
	}
	
	if ( BackCounter )
	{
		BackSplit->NumberOfVertices = BackCounter;
		memcpy ( BackSplit->VertexList, BackList, BackCounter * sizeof ( D3DLVERTEX ) );
	}

	if ( BackCounter )
		BackSplit->NumberOfIndices	= ( BackSplit->NumberOfVertices  - 2 ) * 3;

	if ( FrontCounter )
		FrontSplit->NumberOfIndices	= ( FrontSplit->NumberOfVertices - 2 ) * 3;
	
	// reserve memory for front and back index lists 
	if ( FrontSplit->NumberOfIndices )
		FrontSplit->Indices	= new WORD [ FrontSplit->NumberOfIndices ];

	if ( BackSplit->NumberOfIndices )
		BackSplit->Indices= new WORD [ BackSplit->NumberOfIndices ];

	// fill in the indices
	short IndxBase;

	//
	if ( BackSplit->NumberOfVertices == 0 )
		BackSplit->NumberOfIndices = 0;

	if ( FrontSplit->NumberOfVertices == 0 )
		FrontSplit->NumberOfIndices = 0;

	for ( short loop = 0, v1 = 1, v2 = 2; loop < FrontSplit->NumberOfIndices / 3; loop++, v1 = v2, v2++ )
	{
		IndxBase = loop * 3;

		FrontSplit->Indices [ IndxBase     ] =  0;
		FrontSplit->Indices [ IndxBase + 1 ] = v1;
		FrontSplit->Indices [ IndxBase + 2 ] = v2;
	}

	for ( loop = 0, v1 = 1, v2 = 2; loop < BackSplit->NumberOfIndices / 3; loop++, v1 = v2, v2++ )
	{
		IndxBase = loop * 3;

		BackSplit->Indices [ IndxBase     ] =  0;
		BackSplit->Indices [ IndxBase + 1 ] = v1;
		BackSplit->Indices [ IndxBase + 2 ] = v2;
	}

	// copy extra values
	FrontSplit->Normal = Poly->Normal;
	BackSplit->Normal  = Poly->Normal;
}

void cNodeTree::RebuildMesh ( cNodeTree* pNode )
{
	// rebuild the mesh for the current node, we need to clip the polygons
	// against the planes of the node and then rebuild a new mesh

	// see if we have a mesh
	if ( !pNode )
		return;

	// set vars used
	POLYGON polyNodeObject;
	vector < sMesh* > pMeshList;
	sMesh* pCurrent = NULL;
	sMesh* pLast    = NULL;

	// if we have vertex data
	if ( pNode->m_pVertices )
	{
		// go through all vertex data
		for ( int iVertex = 0; iVertex < pNode->m_iTriangleCount * 3; iVertex++ )
		{
			// find mesh from vertex
			pCurrent = pNode->m_pVertices [ iVertex ].pMeshRef;

			// if mesh changes, add it
			if ( pCurrent != pLast )
			{
				// add to mesh list
				pMeshList.push_back ( pCurrent );

				// record add ptr
				pLast = pCurrent;
			}
		}
	}
	
	// go through mesh list
	for ( int iA = 0; iA < pMeshList.size ( ); iA++ )
	{
		sMesh* pMesh = pMeshList [ iA ];
		if ( !pMesh )
			return;

		sMesh* pMeshA = pMeshList [ 0 ];
		if ( !pMeshA )
			return;

		if ( !pMesh->dwVertexCount )
			return;

		// get the centre and size of the node
		D3DXVECTOR3 vecCentre = pNode->m_vecCentre;
		float       fSize     = pNode->m_fWidth;

		// divide the width, height and depth by 2
		float fWidth  = fSize / 2.0f;
		float fHeight = fSize / 2.0f;
		float fDepth  = fSize / 2.0f;

		// find the edges of the node
		D3DXVECTOR3 vecTopLeftFront     = D3DXVECTOR3 ( vecCentre.x - fWidth, vecCentre.y + fHeight, vecCentre.z + fDepth );
		D3DXVECTOR3 vecTopLeftBack      = D3DXVECTOR3 ( vecCentre.x - fWidth, vecCentre.y + fHeight, vecCentre.z - fDepth );
		D3DXVECTOR3 vecTopRightBack     = D3DXVECTOR3 ( vecCentre.x + fWidth, vecCentre.y + fHeight, vecCentre.z - fDepth );
		D3DXVECTOR3 vecTopRightFront    = D3DXVECTOR3 ( vecCentre.x + fWidth, vecCentre.y + fHeight, vecCentre.z + fDepth );
		D3DXVECTOR3 vecBottomLeftFront  = D3DXVECTOR3 ( vecCentre.x - fWidth, vecCentre.y - fHeight, vecCentre.z + fDepth );
		D3DXVECTOR3 vecBottomLeftBack   = D3DXVECTOR3 ( vecCentre.x - fWidth, vecCentre.y - fHeight, vecCentre.z - fDepth );
		D3DXVECTOR3 vecBottomRightBack  = D3DXVECTOR3 ( vecCentre.x + fWidth, vecCentre.y - fHeight, vecCentre.z - fDepth );
		D3DXVECTOR3 vecBottomRightFront = D3DXVECTOR3 ( vecCentre.x + fWidth, vecCentre.y - fHeight, vecCentre.z + fDepth );

		// store some polygons
		vector < POLYGON > polyObjectList;
		int iIndex = 0;

		sOffsetMap	 offsetMap;
		GetFVFOffsetMap ( pMesh->dwFVF, &offsetMap );

		for ( int iTriangle = 0; iTriangle < pMesh->dwIndexCount / 3; iTriangle++ )
		{
			POLYGON polyObject;

			polyObject.VertexList = new D3DLVERTEX [ 3 ];
			
			for ( int iTemp = 0; iTemp < 3; iTemp++ )
			{
				int iAa = pMesh->pIndices [ iIndex ];
				iIndex++;

				float fX = *( ( float* ) pMesh->pVertexData + offsetMap.dwX        + ( offsetMap.dwSize * iAa ) );
				float fY = *( ( float* ) pMesh->pVertexData + offsetMap.dwY        + ( offsetMap.dwSize * iAa ) );
				float fZ = *( ( float* ) pMesh->pVertexData + offsetMap.dwZ        + ( offsetMap.dwSize * iAa ) );
				float nx = *( ( float* ) pMesh->pVertexData + offsetMap.dwNX       + ( offsetMap.dwSize * iAa ) );
				float ny = *( ( float* ) pMesh->pVertexData + offsetMap.dwNY       + ( offsetMap.dwSize * iAa ) );
				float nz = *( ( float* ) pMesh->pVertexData + offsetMap.dwNZ       + ( offsetMap.dwSize * iAa ) );
				float tu = *( ( float* ) pMesh->pVertexData + offsetMap.dwTU [ 0 ] + ( offsetMap.dwSize * iAa ) );
				float tv = *( ( float* ) pMesh->pVertexData + offsetMap.dwTV [ 0 ] + ( offsetMap.dwSize * iAa ) );

				polyObject.VertexList [ iTemp ].x     = fX;
				polyObject.VertexList [ iTemp ].y     = fY;
				polyObject.VertexList [ iTemp ].z     = fZ;
				polyObject.VertexList [ iTemp ].nx    = nx;
				polyObject.VertexList [ iTemp ].ny    = ny;
				polyObject.VertexList [ iTemp ].nz    = nz;
				polyObject.VertexList [ iTemp ].tu    = tu;
				polyObject.VertexList [ iTemp ].tv    = tv;
				polyObject.VertexList [ iTemp ].color = D3DCOLOR_ARGB ( 255, 255, 255, 255 );
			}

			polyObject.NumberOfVertices = 3;
			polyObject.NumberOfIndices  = 3;
			polyObject.Indices          = new WORD [ 3 ];

			polyObject.Indices [ 0 ] = 0;
			polyObject.Indices [ 1 ] = 1;
			polyObject.Indices [ 2 ] = 2;

			polyObjectList.push_back ( polyObject );
		}

		int iCount = 0;

		vector < POLYGON > pass1Front;
		vector < POLYGON > pass1Back;

		POLYGON blank;
		memset ( &blank, 0, sizeof ( POLYGON ) );

		for ( int iTemp = 0; iTemp < polyObjectList.size ( ); iTemp++ )
		{
			pass1Front.push_back ( blank );
			pass1Back.push_back  ( blank );
		}
		
		// we need to check which polygons are within this node, to do this
		// we get the nodes planes and test the polygons against each one, we
		// don't need to check against all 8 planes, only 6 are needed

		PLANE planes [ 6 ];

		planes [ 0 ].PointOnPlane = vecBottomRightBack;						// right plane
		planes [ 0 ].Normal       = D3DXVECTOR3 ( -1.0f,  0.0f,  0.0f );	// to the left
		planes [ 1 ].PointOnPlane = vecBottomLeftBack;						// left plane
		planes [ 1 ].Normal       = D3DXVECTOR3 (  1.0f,  0.0f,  0.0f );	// to the right
		planes [ 2 ].PointOnPlane = vecBottomRightFront;					// front plane
		planes [ 2 ].Normal       = D3DXVECTOR3 (  0.0f,  0.0f, -1.0f );	// to the back
		planes [ 3 ].PointOnPlane = vecBottomRightFront;					// bottom right
		planes [ 3 ].Normal       = D3DXVECTOR3 (  0.0f,  1.0f,  0.0f );	// to the top
		planes [ 4 ].PointOnPlane = vecTopRightFront;						// top right
		planes [ 4 ].Normal       = D3DXVECTOR3 (  0.0f, -1.0f,  0.0f );	// to the bottom
		planes [ 5 ].PointOnPlane = vecTopLeftBack;							// top left
		planes [ 5 ].Normal       = D3DXVECTOR3 (  0.0f,  0.0f,  1.0f );	// towards us

		// go through each plane
		for ( int iPlane = 0; iPlane < 6; iPlane++ )
		{
			// go through all of the polygons
			for ( int iTemp = 0; iTemp < polyObjectList.size ( ); iTemp++ )
			{
				// make sure we have some vertices
				if ( !polyObjectList [ iTemp ].NumberOfVertices )
					continue;

				// split the polygons against the plane
				SplitPolygon ( &polyObjectList [ iTemp ], &planes [ iPlane ], &pass1Front [ iCount ], &pass1Back [ iCount ] );

				// delete the back split
				if ( pass1Back [ iCount ].NumberOfVertices )
				{
					SAFE_DELETE_ARRAY ( pass1Back [ iCount ].Indices    );
					SAFE_DELETE_ARRAY ( pass1Back [ iCount ].VertexList );
				}

				// leefix - 020303 - delete the original list item or mem leak!!
				if ( polyObjectList [ iTemp ].NumberOfVertices )
				{
					SAFE_DELETE_ARRAY ( polyObjectList [ iTemp ].Indices    );
					SAFE_DELETE_ARRAY ( polyObjectList [ iTemp ].VertexList );
				}

				// increment the counter
				iCount++;
			}

			polyObjectList.clear ( );

			for ( iTemp = 0; iTemp < pass1Front.size ( ); iTemp++ )
			{
				polyObjectList.push_back ( pass1Front [ iTemp ] );
			}

			POLYGON blank;
			memset ( &blank, 0, sizeof ( POLYGON ) );

			pass1Front.clear ( );
			pass1Back.clear ( );

			for ( iTemp = 0; iTemp < polyObjectList.size ( ); iTemp++ )
			{
				pass1Front.push_back ( blank );
				pass1Back.push_back  ( blank );
			}
			
			// reset the counter
			iCount = 0;
		}

		// count the number of polygons
		int iVertexCount = 0;

		for ( int iObject = 0; iObject < polyObjectList.size ( ); iObject++ )
		{
			iVertexCount += polyObjectList [ iObject ].NumberOfIndices;
		}

		// creating a final object
		polyNodeObject.VertexList       = new D3DLVERTEX [ iVertexCount ];
		polyNodeObject.NumberOfVertices = iVertexCount;
		polyNodeObject.NumberOfIndices  = iVertexCount;
		polyNodeObject.Indices          = new WORD [ iVertexCount ];

		for ( int iInd = 0; iInd < iVertexCount; iInd++ )
		{
			polyNodeObject.Indices [ iInd ] = iInd;
		}

		int iPosition   = 0;
		int iFrontSplit = 0;
		int iSplit      = 0;

		for ( iTemp = 0; iTemp < polyObjectList.size ( ); iTemp++ )
		{
			// make sure we have some vertices
			if ( !polyObjectList [ iTemp ].NumberOfVertices )
				continue;

			for ( int iVertex = 0; iVertex < polyObjectList [ iTemp ].NumberOfIndices; iVertex++ )
			{
				iSplit = polyObjectList [ iTemp ].Indices [ iVertex ];

				memcpy ( 
							&polyNodeObject.VertexList [ iPosition ],
							&polyObjectList [ iTemp ].VertexList [ iSplit ],
							sizeof ( D3DLVERTEX )
					   );

				iPosition++;
			}
		}

		if ( polyNodeObject.NumberOfVertices )
		{
			sMesh* pNewMesh = new sMesh;

			pNewMesh->bWireframe           = pMesh->bWireframe;
			pNewMesh->bLight               = pMesh->bLight;
			pNewMesh->bCull                = pMesh->bCull;
			pNewMesh->bFog                 = pMesh->bFog;
			pNewMesh->bAmbient             = pMesh->bAmbient;
			pNewMesh->bTransparency        = pMesh->bTransparency;
			pNewMesh->bGhost               = pMesh->bGhost;
			pNewMesh->iGhostMode           = pMesh->iGhostMode;
			pNewMesh->bVisible             = pMesh->bVisible;
			pNewMesh->bOverridePixelShader = pMesh->bOverridePixelShader;
			pNewMesh->dwPixelShader        = pMesh->dwPixelShader;
			pNewMesh->bUsesMaterial        = pMesh->bUsesMaterial;
			pNewMesh->pUseVertexShader     = pMesh->pUseVertexShader;
			pNewMesh->pVertexShaderEffect  = pMesh->pVertexShaderEffect;
			pNewMesh->bUsesMaterial        = pMesh->bUsesMaterial;
			pNewMesh->dwTextureCount       = pMesh->dwTextureCount;

			memcpy ( &pNewMesh->Collision, &pMesh->Collision, sizeof ( sCollisionData ) );
			memcpy ( &pNewMesh->mMaterial, &pMesh->mMaterial, sizeof ( D3DMATERIAL8   ) );

			pNewMesh->pTextures = new sTexture [ pNewMesh->dwTextureCount ];

			memcpy ( pNewMesh->pTextures, pMesh->pTextures, sizeof ( sTexture ) * pNewMesh->dwTextureCount );

			// recreate mesh data
			SetupMeshFVFData ( pNewMesh, pMesh->dwFVF, polyNodeObject.NumberOfVertices, polyNodeObject.NumberOfIndices );

			// get the offset map
			sOffsetMap offsetMap;
			GetFVFOffsetMap ( pNewMesh->dwFVF, &offsetMap );

			// run through all of the vertices
			for ( int iCurrentVertex = 0; iCurrentVertex < polyNodeObject.NumberOfVertices; iCurrentVertex++ )
			{
				// check for position data
				if ( pNewMesh->dwFVF & D3DFVF_XYZ )
				{
					*( ( float* ) pNewMesh->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * iCurrentVertex ) ) = polyNodeObject.VertexList [ iCurrentVertex ].x;
					*( ( float* ) pNewMesh->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * iCurrentVertex ) ) = polyNodeObject.VertexList [ iCurrentVertex ].y;
					*( ( float* ) pNewMesh->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * iCurrentVertex ) ) = polyNodeObject.VertexList [ iCurrentVertex ].z;
				}

				// check for normals
				if ( pNewMesh->dwFVF & D3DFVF_NORMAL )
				{
					*( ( float* ) pNewMesh->pVertexData + offsetMap.dwNX + ( offsetMap.dwSize * iCurrentVertex ) ) = polyNodeObject.VertexList [ iCurrentVertex ].nx;
					*( ( float* ) pNewMesh->pVertexData + offsetMap.dwNY + ( offsetMap.dwSize * iCurrentVertex ) ) = polyNodeObject.VertexList [ iCurrentVertex ].ny;
					*( ( float* ) pNewMesh->pVertexData + offsetMap.dwNZ + ( offsetMap.dwSize * iCurrentVertex ) ) = polyNodeObject.VertexList [ iCurrentVertex ].nz;
				}

				// texture coordinates
				if ( pNewMesh->dwFVF & D3DFVF_TEX1 )
				{
					*( ( float* ) pNewMesh->pVertexData + offsetMap.dwTU [ 0 ] + ( offsetMap.dwSize * iCurrentVertex ) ) = polyNodeObject.VertexList [ iCurrentVertex ].tu;
					*( ( float* ) pNewMesh->pVertexData + offsetMap.dwTV [ 0 ] + ( offsetMap.dwSize * iCurrentVertex ) ) = polyNodeObject.VertexList [ iCurrentVertex ].tv;
				}
			}

			// indices
			memcpy ( pNewMesh->pIndices, polyNodeObject.Indices, sizeof ( WORD ) * polyNodeObject.NumberOfIndices );

			pNode->m_pMeshList.push_back ( pNewMesh );

			for ( iTemp = 0; iTemp < polyObjectList.size ( ); iTemp++ )
			{
				// make sure we have some vertices
				if ( !polyObjectList [ iTemp ].NumberOfVertices )
					continue;

				SAFE_DELETE_ARRAY ( polyObjectList [ iTemp ].Indices );
				SAFE_DELETE_ARRAY ( polyObjectList [ iTemp ].VertexList );
			}

			SAFE_DELETE_ARRAY ( polyNodeObject.Indices );
			SAFE_DELETE_ARRAY ( polyNodeObject.VertexList );
		}
	}

	// rebuild the mesh for sub nodes
	RebuildMesh ( pNode->m_pNodes [ 0 ] );
	RebuildMesh ( pNode->m_pNodes [ 1 ] );
	RebuildMesh ( pNode->m_pNodes [ 2 ] );
	RebuildMesh ( pNode->m_pNodes [ 3 ] );
	RebuildMesh ( pNode->m_pNodes [ 4 ] );
	RebuildMesh ( pNode->m_pNodes [ 5 ] );
	RebuildMesh ( pNode->m_pNodes [ 6 ] );
	RebuildMesh ( pNode->m_pNodes [ 7 ] );
}
