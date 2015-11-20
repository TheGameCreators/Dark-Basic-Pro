#include "cNodeTree.h"
#include "cFrustum.h"

/*
extern LPDIRECT3DDEVICE8		m_pD3D;

cNodeTreeMesh::cNodeTreeMesh ( )
{
	m_TreeType    = OCTREE;
	m_ParentNode  = NULL;
	m_NumGroups   = 0;
	m_Groups      = NULL;
	m_Polygons    = NULL;
	m_NumPolygons = 0;
}

cNodeTreeMesh::~cNodeTreeMesh ( )
{
	Free ( );
}

BOOL cNodeTreeMesh::Create ( ID3DXMesh*	pMeshData, LPDIRECT3DTEXTURE8* pTextures, D3DMATERIAL8* pMaterials, DWORD dwNumMaterials, int TreeType, float MaxSize, long MaxPolygons, D3DXMATRIX matObject )
{
	ID3DXMesh*		LoadMesh;
	unsigned short*	IndexPtr;
	unsigned long*	Attributes;
	float           MaxX,
					MaxY,
					MaxZ;
	unsigned long   i;

	BYTE**				Ptr;

	D3DXVECTOR3         m_Min, m_Max;      // Bounding box
	float				m_Radius;

	m_dwNumVertices = pMeshData->GetNumVertices ( );

	// Mike, creating copy of mesh for hack-slow mesh collision check (replace with node tree cleverness)
//	m_Mesh           = pMeshData;
	pMeshData->CloneMeshFVF(0, pMeshData->GetFVF(), m_pD3D, &m_Mesh);

	m_pTextures      = pTextures;
	m_pMaterials     = pMaterials;
	m_dwNumMaterials = dwNumMaterials;
	
	// get mesh info
	LoadMesh      = m_Mesh;
	m_VertexFVF   = LoadMesh->GetFVF ( );
	m_VertexSize  = D3DXGetFVFVertexSize ( m_VertexFVF );
	m_NumPolygons = LoadMesh->GetNumFaces ( );
	m_MaxPolygons = MaxPolygons;

	// create the polygon list and texture groups
	m_Polygons    = new sPolygon [ m_NumPolygons ] ( );
	m_NumGroups   = dwNumMaterials;
	m_Groups      = new sGroup [ m_NumGroups ] ( );

	if ( SUCCEEDED ( LoadMesh->LockVertexBuffer ( D3DLOCK_READONLY, ( BYTE** ) &Ptr ) ) )
	{
		D3DXComputeBoundingBox ( ( void* ) Ptr, m_dwNumVertices, m_VertexFVF, &m_Min, &m_Max );
		D3DXComputeBoundingSphere ( ( void* ) Ptr, m_dwNumVertices, m_VertexFVF, &D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f ), &m_Radius );
		LoadMesh->UnlockVertexBuffer ( );
	}

	// lock the index and attribute buffers
	LoadMesh->LockIndexBuffer     ( D3DLOCK_READONLY, ( BYTE** ) &IndexPtr );
	LoadMesh->LockAttributeBuffer ( D3DLOCK_READONLY, &Attributes );

	// load polygon information into structures
	for ( i = 0; i < m_NumPolygons; i++ )
	{
		m_Polygons [ i ].Vertex0 = *IndexPtr++;
		m_Polygons [ i ].Vertex1 = *IndexPtr++;
		m_Polygons [ i ].Vertex2 = *IndexPtr++;

		m_Polygons [ i ].Group = Attributes [ i ];
		m_Polygons [ i ].Timer = 0;

		m_Groups [ Attributes [ i ] ].NumPolygons++;
	}

	// unlock buffers
	LoadMesh->UnlockAttributeBuffer ( );
	LoadMesh->UnlockIndexBuffer     ( );

	// build the group vertex index buffers
	for ( i = 0; i < m_NumGroups; i++ )
	{
		if ( m_Groups [ i ].NumPolygons != 0 )
		{
			m_pD3D->CreateIndexBuffer (
											m_Groups [ i ].NumPolygons * 3 * sizeof ( unsigned short ),
											D3DUSAGE_WRITEONLY,
											D3DFMT_INDEX16,
											D3DPOOL_MANAGED, 
											&m_Groups [ i ].IndexBuffer
									  );
		}
	}

	// get the size of the bounding cube
	MaxX      = ( float ) max ( fabs ( m_Min.x ), fabs ( m_Max.x ) );
	MaxY      = ( float ) max ( fabs ( m_Min.y ), fabs ( m_Max.y ) );
	MaxZ      = ( float ) max ( fabs ( m_Min.z ), fabs ( m_Max.z ) );
	m_Size    = max ( MaxX, max ( MaxY, MaxZ ) ) * 2.0f;
	m_MaxSize = MaxSize;

	// create the parent node
	m_ParentNode = new sNode ( );

	// sort polygons into nodes
	LoadMesh->LockVertexBuffer ( D3DLOCK_READONLY, ( BYTE** ) &m_VertexPtr );

// function takes mesh - no adjustments
//		struct sVertices
//		{
//			D3DXVECTOR3 vecPos;
//			D3DXVECTOR3 vecNormal;
//			//DWORD		dwDiffuse;
//			float		tu, tv;
//		};
//		sVertices* pData = ( sVertices* ) m_VertexPtr;
//		for ( int iTemp = 0; iTemp < (int)LoadMesh->GetNumVertices ( ); iTemp++ )
//		{
//			D3DXVECTOR3 vecNew;
//
//			D3DXVec3TransformCoord ( &vecNew, &pData [ iTemp ].vecPos, &matObject ); 
//
//			pData [ iTemp ].vecPos = vecNew;
//		}
	
	SortNode ( m_ParentNode, 0.0f, 0.0f, 0.0f, m_Size );

	LoadMesh->UnlockIndexBuffer ( );
	LoadMesh->UnlockVertexBuffer ( );

	m_Timer = 0;

	return TRUE;
}

BOOL cNodeTreeMesh::Free ( )
{
	// locally created copy of mesh
	m_Mesh->Release();

	delete m_ParentNode;
	m_ParentNode = NULL;

	m_NumPolygons = 0;
	delete [] m_Polygons;
	m_Polygons = NULL;

	m_NumGroups = 0;
	delete [ ] m_Groups;
	m_Groups = NULL;

	return TRUE;
}

void cNodeTreeMesh::SortNode ( sNode *Node, float XPos, float YPos, float ZPos, float Size )
{
	unsigned long i, Num;
	float         XOff, YOff, ZOff;

	if ( Node == NULL )
		return;

	// store node coordinates and size
	Node->XPos = XPos;
	Node->YPos = ( m_TreeType == QUADTREE ) ? 0.0f : YPos;
	Node->ZPos = ZPos;
	Node->Size = Size;

	// see if there are any polygons in the node
	if ( !( Num = CountPolygons ( XPos, YPos, ZPos, Size ) ) )
		return;

	// split node if size > maximum and too many polygons
	if ( Size > m_MaxSize && Num > m_MaxPolygons )
	{
		for ( i = 0; i < ( unsigned long ) ( ( m_TreeType == QUADTREE ) ? 4 : 8 ); i++ )
		{
			XOff = ( ( ( i % 2 ) < 1 ) ? -1.0f : 1.0f ) * ( Size / 4.0f );
			ZOff = ( ( ( i % 4 ) < 2 ) ? -1.0f : 1.0f ) * ( Size / 4.0f );
			YOff = ( ( ( i % 8 ) < 4 ) ? -1.0f : 1.0f ) * ( Size / 4.0f );

			// see if any polygons in new node bounding box
			if ( CountPolygons ( XPos + XOff, YPos + YOff, ZPos + ZOff, Size / 2.0f ) )
			{
				Node->Nodes [ i ] = new sNode ( );  // create new child node

				// sort the polygons with the new child node
				SortNode ( Node->Nodes [ i ], XPos + XOff, YPos + YOff, ZPos + ZOff, Size / 2.0f );
			}
		}

		return;
	}

	// allocate space for vertex indices
	Node->NumPolygons = Num;
	Node->PolygonList = new unsigned long [ Num ];

	// scan through polygon list, storing pointers and assigning them
	Num = 0;

	for ( i = 0; i < m_NumPolygons; i++ )
	{
		// add polygon to node list if contained in 3D space
		if ( IsPolygonContained ( &m_Polygons [ i ], XPos, YPos, ZPos, Size ) == TRUE )
			Node->PolygonList [ Num++ ] = i;
	}
}

BOOL cNodeTreeMesh::IsPolygonContained ( sPolygon* Polygon, float XPos, float YPos, float ZPos, float Size )
{
	float    XMin, XMax, YMin, YMax, ZMin, ZMax;
	sVertex* Vertex [ 3 ];

	// get the polygon's vertices
	Vertex [ 0 ] = ( sVertex* ) &m_VertexPtr [ m_VertexSize * Polygon->Vertex0 ];
	Vertex [ 1 ] = ( sVertex* ) &m_VertexPtr [ m_VertexSize * Polygon->Vertex1 ];
	Vertex [ 2 ] = ( sVertex* ) &m_VertexPtr [ m_VertexSize * Polygon->Vertex2 ];

	// check against X axis of specified 3D space
	XMin = min ( Vertex [ 0 ]->x, min ( Vertex [ 1 ]->x, Vertex [ 2 ]->x ) );
	XMax = max ( Vertex [ 0 ]->x, max ( Vertex [ 1 ]->x, Vertex [ 2 ]->x ) );

	if ( XMax < ( XPos - Size / 2.0f ) )
		return FALSE;

	if ( XMin > ( XPos + Size / 2.0f ) )
		return FALSE;

	// check against Y axis of specified 3D space ( only if octree tree type )
	if ( m_TreeType == OCTREE )
	{
		YMin = min ( Vertex [ 0 ]->y, min ( Vertex [ 1 ]->y, Vertex [ 2 ]->y ) );
		YMax = max ( Vertex [ 0 ]->y, max ( Vertex [ 1 ]->y, Vertex [ 2 ]->y ) );

		if ( YMax < ( YPos - Size / 2.0f ) )
			return FALSE;

		if ( YMin > ( YPos + Size / 2.0f ) )
			return FALSE;
	}

	// check against Z axis of specified 3D space
	ZMin = min ( Vertex [ 0 ]->z, min ( Vertex [ 1 ]->z, Vertex [ 2 ]->z ) );
	ZMax = max ( Vertex [ 0 ]->z, max ( Vertex [ 1 ]->z, Vertex [ 2 ]->z ) );

	if ( ZMax < ( ZPos - Size / 2.0f ) )
		return FALSE;

	if ( ZMin > ( ZPos + Size / 2.0f ) )
		return FALSE;

	return TRUE;
}

unsigned long cNodeTreeMesh::CountPolygons ( float XPos, float YPos, float ZPos, float Size )
{
	unsigned long i, Num;

	// return if no polygons to process
	if ( !m_NumPolygons )
		return 0;

	// go through every polygon and keep count of those contained in the specified 3D space
	Num = 0;

	for ( i = 0; i < m_NumPolygons; i++ )
	{
		if ( IsPolygonContained ( &m_Polygons [ i ], XPos, YPos, ZPos, Size ) == TRUE )
			Num++;
	}

	return Num;
}

BOOL cNodeTreeMesh::Render ( float ZDistance )
{
	D3DXMATRIX				Matrix;
	IDirect3DVertexBuffer8*	pVB = NULL;
	unsigned long			i;

	// error checking
	if ( m_ParentNode == NULL || !m_NumPolygons )
		return FALSE;

	// construct the viewing frustum
	//SetupFrustum ( ZDistance );

	// set the world transformation matrix to identity, so that
	// level mesh is rendered around the origin it was designed
	D3DXMatrixIdentity ( &Matrix );

	m_pD3D->SetTransform ( D3DTS_WORLD, &Matrix );

	// lock group index buffer
	for ( i = 0; i < m_NumGroups; i++ )
	{
		if ( m_Groups [ i ].NumPolygons )
		{
			m_Groups [ i ].IndexBuffer->Lock (
												0,
												m_Groups [ i ].NumPolygons * 3 * sizeof ( unsigned short ),
												( BYTE** ) &m_Groups [ i ].IndexPtr,
												0
											 );
		}

		m_Groups [ i ].NumPolygonsToDraw = 0;
	}

	// increase frame timer
	m_Timer++;

	// add polygons to be drawn into group lists
	AddNode ( m_ParentNode );

	// get vertex buffer pointer
	m_Mesh->GetVertexBuffer ( &pVB );

	// set vertex shader and source
	m_pD3D->SetStreamSource ( 0, pVB, m_VertexSize );
	m_pD3D->SetVertexShader ( m_VertexFVF );

	// unlock vertex buffers and draw
	for ( i = 0; i < m_NumGroups; i++ )
	{
		if ( m_Groups [ i ].NumPolygons )
		{
			m_Groups [ i ].IndexBuffer->Unlock ( );

			pVB->Unlock ( );
		}

		if ( m_Groups [ i ].NumPolygonsToDraw )
		{
			//m_pD3D->SetMaterial ( &m_pMaterials [ i ] );

			D3DMATERIAL8 mtrl;
			
			memset ( &mtrl, 0, sizeof ( mtrl ) );
			mtrl.Diffuse.r = 1.0f;
			mtrl.Diffuse.g = 1.0f;
			mtrl.Diffuse.b = 1.0f;
			mtrl.Diffuse.a = 1.0f;
				
			mtrl.Ambient.r = 1.0f;
			mtrl.Ambient.g = 1.0f;
			mtrl.Ambient.b = 1.0f;
			mtrl.Ambient.a = 1.0f;

			m_pD3D->SetMaterial ( &mtrl );
			m_pD3D->SetTexture ( 0, m_pTextures [ i ] );
			m_pD3D->SetIndices ( m_Groups [ i ].IndexBuffer, 0 );

			m_pD3D->DrawIndexedPrimitive
										(
											D3DPT_TRIANGLELIST,
											0,
											m_Mesh->GetNumVertices ( ),
											0,
											m_Groups [ i ].NumPolygonsToDraw
										);
		}
	}

	// release vertex buffer
	if ( pVB != NULL )
		pVB->Release ( );

	return TRUE;
}

void cNodeTreeMesh::AddNode ( sNode* Node )
{
	unsigned long	i, Group;
	sPolygon*		Polygon;
	short			Num;

	if ( Node == NULL )
		return;

	// perform frustum check based on tree type
	if ( m_TreeType == QUADTREE )
	{
		if ( CheckRectangle ( Node->XPos, 0.0f, Node->ZPos, Node->Size / 2.0f, m_Size / 2.0f, Node->Size / 2.0f ) == FALSE )
			return;
		}
		else
		{
			if ( CheckRectangle ( Node->XPos, Node->YPos, Node->ZPos, Node->Size / 2.0f, Node->Size / 2.0f, Node->Size / 2.0f ) == FALSE )
				return;
		}

	// scan other nodes
	Num = 0;

	for ( i = 0; i < ( unsigned long ) ( ( m_TreeType == QUADTREE ) ? 4 : 8 ); i++ )
	{
		if ( Node->Nodes [ i ] != NULL )
		{
			Num++;
			AddNode ( Node->Nodes [ i ] );
		}
	}

	// don't need to go on if there was other nodes
	if ( Num )
		return;

	// add contained polygons ( if any )
	if ( Node->NumPolygons != 0 )
	{
		for ( i = 0; i < Node->NumPolygons; i++ )
		{
			// get pointer to polygon
			Polygon = &m_Polygons [ Node->PolygonList [ i ] ];

			// only draw if not done already this frame
			if ( Polygon->Timer != m_Timer )
			{
				// set polygon as processed this frame
				Polygon->Timer = m_Timer;

				// get texture group of polygon
				Group = Polygon->Group;

				// make sure group is okay and material is not transparent
				if ( Group < m_NumGroups && m_pMaterials [ Group ].Diffuse.a != 0.0f )
				{
					// copy indices into index buffer
					*m_Groups [ Group ].IndexPtr++ = Polygon->Vertex0;
					*m_Groups [ Group ].IndexPtr++ = Polygon->Vertex1;
					*m_Groups [ Group ].IndexPtr++ = Polygon->Vertex2;

					// increase count of polygons to draw in group
					m_Groups [ Group ].NumPolygonsToDraw++;
				}
			}
		}
	}
}


float cNodeTreeMesh::GetClosestHeight ( float XPos, float YPos, float ZPos )
{
	float YAbove, YBelow;

	YAbove = GetHeightAbove ( XPos, YPos, ZPos );
	YBelow = GetHeightBelow ( XPos, YPos, ZPos );

	if ( fabs ( YAbove - YPos ) < fabs ( YBelow - YPos ) )
		return YAbove;

	return YBelow;
}

float cNodeTreeMesh::GetHeightBelow ( float XPos, float YPos, float ZPos )
{
	BOOL  Hit;
	float u, v, Dist;
	DWORD FaceIndex;

	D3DXIntersect ( m_Mesh, &D3DXVECTOR3 ( XPos, YPos, ZPos ), &D3DXVECTOR3 ( 0.0f, -1.0f, 0.0f ), &Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL );
	
	if ( Hit == TRUE )
		return YPos - Dist;

	return YPos;
}

float cNodeTreeMesh::GetHeightAbove ( float XPos, float YPos, float ZPos )
{ 
	BOOL  Hit;
	float u, v, Dist;
	DWORD FaceIndex;

	D3DXIntersect ( m_Mesh, &D3DXVECTOR3 ( XPos, YPos, ZPos ), &D3DXVECTOR3 ( 0.0f, 1.0f, 0.0f ), &Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL );
	
	if ( Hit == TRUE )
		return YPos + Dist;

	return YPos;
}

BOOL cNodeTreeMesh::CheckIfNodePointWithinSphere ( float X, float Y, float Z, float Radius, float* Length )
{
	// Checks each vertex to see if within sphere, and returns distance to it if hit
	BYTE* Ptr;
	BOOL Hit=FALSE;
	if ( SUCCEEDED ( m_Mesh->LockVertexBuffer ( D3DLOCK_READONLY, ( BYTE** ) &Ptr ) ) )
	{
		float* pV = (float*)Ptr;
		DWORD dwAddFloats=m_VertexSize/4;
		for(DWORD v=0; v<m_dwNumVertices; v++)
		{
			//fast distance check
			D3DXVECTOR3* pVec = (D3DXVECTOR3*)Ptr;
			float XDiff = (float)fabs(X-pVec->x);
			if(XDiff<Radius)
			{
				float YDiff = (float)fabs(Y-pVec->y);
				if(YDiff<Radius)
				{
					float ZDiff = (float)fabs(Z-pVec->z);
					if(ZDiff<Radius)
					{
						//final detailed check
						float Dist = ( float ) sqrt ( XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff );
						if ( Dist < Radius )
						{
							Hit=TRUE;
							if(Length) *Length=Dist;
							break;
						}
					}
				}
			}
			pV+=dwAddFloats;
		}
		m_Mesh->UnlockVertexBuffer ( );
	}
	return Hit;
}

BOOL cNodeTreeMesh::CheckIntersect ( float XStart, float YStart, float ZStart, float XEnd, float YEnd, float ZEnd, float* Length )
{
	BOOL  Hit;
	float u, v, Dist;
	float XDiff, YDiff, ZDiff, Size;
	DWORD FaceIndex;
	D3DXVECTOR3 vecDir;

	XDiff = XEnd - XStart;
	YDiff = YEnd - YStart;
	ZDiff = ZEnd - ZStart;

	D3DXVec3Normalize ( &vecDir, &D3DXVECTOR3 ( XDiff, YDiff, ZDiff ) );
	
	// get the project, view, and inversed view matrices
	D3DXMATRIX	matProj,
				matView,
				matInv;

	//m_pD3D->GetTransform ( D3DTS_PROJECTION, &matProj );
	//m_pD3D->GetTransform ( D3DTS_VIEW,       &matView );

	//D3DXMatrixInverse ( &matInv, NULL, &matView );

	//vecDir.x = vecTemp.x * matInv._11 + vecTemp.y * matInv._21 + vecTemp.z * matInv._31;
	//vecDir.y = vecTemp.x * matInv._12 + vecTemp.y * matInv._22 + vecTemp.z * matInv._32;
	//vecDir.z = vecTemp.x * matInv._13 + vecTemp.y * matInv._23 + vecTemp.z * matInv._33;

	D3DXIntersect ( m_Mesh, &D3DXVECTOR3 ( XStart, YStart, ZStart ), &vecDir, &Hit, &FaceIndex, &u, &v, &Dist, NULL, NULL );

	if ( Hit == TRUE )
	{
		Size = ( float ) sqrt ( XDiff * XDiff + YDiff * YDiff + ZDiff * ZDiff );

		if ( Dist > Size )
			Hit = FALSE;
		else
		{
			if ( Length != NULL )
				*Length = Dist;
		}
	}
	return Hit;
}

void CreateSortedArray ( int* piArray, int** ppiSorted, int iCurrentSize, int* piFinalSize )
{
	// returns a sorted array
	
	int iSize  = iCurrentSize;
	int iTemp  = 0;

	// sort the array
	QuickSortArray ( piArray, 0, iSize );

//	for ( iTemp = 0; iTemp < iSize; iTemp++ )
//		cout << piArray [ iTemp ] << " ";


}

void QuickSortArray ( int* array, int low, int high )
{
	// this function takes an array of any size
	// and will resort it into a correct order
	// e.g. sending the function a list of 4,2,3,1
	// will result in a list of 1,2,3,4
	// the low and high values are used to specify
	// the start and end point for the search

	// initialize pointers
	int top    = low;
	int	bottom = high - 1;
	int part_index;
	int part_value;

	// do nothing if low >= high
	if ( low < high )
	{
		// check if elements are sequential
		if ( high == ( low + 1 ) )
		{
			if ( array [ low ] > array [ high ] )
				SwapInts ( array, high, low );
		}
		else
		{
			// choose a partition element and swap 
			// it with the last value in the array
			part_index = ( int ) ( ( low + high ) / 2 );
			part_value = array [ part_index ];

			SwapInts ( array, high, part_index );
		
			do
			{
				// increment the top pointer
				while ( ( array [ top ] <= part_value ) && ( top <= bottom ) )
					top++;
				
				// decrement the bottom pointer
				while ( ( array [ bottom ] > part_value ) && ( top <= bottom ) )
					bottom--;
				
				// swap elements if pointers have not met
				if ( top < bottom )
					SwapInts ( array, top, bottom );

			} while ( top < bottom );

			// put the partition element
			// back where it belongs
			SwapInts ( array, top, high );

			// recursive calls
			QuickSortArray ( array, low,     top - 1 );
			QuickSortArray ( array, top + 1, high    );
		}
	}
}

// swaps array n1 with array n2
void SwapInts ( int* array, int n1, int n2 )
{
	int temp;

	temp         = array [ n1 ];
	array [ n1 ] = array [ n2 ];
	array [ n2 ] = temp;
}
*/