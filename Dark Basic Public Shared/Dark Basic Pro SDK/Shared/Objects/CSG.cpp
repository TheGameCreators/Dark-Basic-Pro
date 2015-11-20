#include "csg.h"
#include "CommonC.h"

#include ".\\Compiler\\CBSPTree.h"
#include ".\\Compiler\\cCompiler.h"

#include <algorithm>
#include <vector>
using namespace std;

void TransformVertices ( sMesh* pMesh, D3DXMATRIX matWorld );

cCSG::cCSG ( )
{

}

cCSG::~cCSG ( )
{

}

bool operator == ( const cCSG::sVertex& a, const cCSG::sVertex& b )
{
	if ( a.x == b.x && a.y == b.y && a.z == b.z )
	{
		if ( a.tu == b.tu && a.tv == b.tv )
		{
			if ( a.nx == b.nx && a.ny == b.ny && a.nz == b.nz )
				return true;
		}
	}
	
	return false;
}

bool cCSG::Union ( sMesh* pMeshA, D3DXMATRIX OriginalMatA, sMesh* pMeshB, D3DXMATRIX OriginalMatB )
{
	TransformVertices ( pMeshA, OriginalMatA );
	TransformVertices ( pMeshB, OriginalMatB );
	CalculateMeshBounds ( pMeshA );
	CalculateMeshBounds ( pMeshB );

	cCompiler	compiler;
	compiler.m_OptionsHSR.bEnabled				= true;
	compiler.m_OptionsESR.bEnabled				= false;
    compiler.m_OptionsBSP.bEnabled				= true;
	compiler.m_OptionsBSP.iTreeType				= 0;
	compiler.m_OptionsBSP.bLinkDetailBrushes	= false;
    compiler.m_OptionsPRT.bEnabled				= false;
    compiler.m_OptionsPVS.bEnabled				= false;
    compiler.m_OptionsTJR.bEnabled				= true;

	compiler.CompileScene (	pMeshA, &OriginalMatA, pMeshB, &OriginalMatB );

	sVertex* pVerticesA = NULL;
	sVertex* pVerticesB = NULL;
	int		 iCountA    = 0;

	{
		compiler.BuildPolygonList ( true );
		
		pVerticesA = ( sVertex* ) compiler.m_pVertices;
		iCountA    = compiler.m_iVertexCount;

		//////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////
		std::vector < sVertex > vertices;
		std::vector < sVertex > newVertices;
		std::vector < int     > newIndices;
		std::vector < int     > newIndicesFinal;

		{
			int iIndex = 0;

			for ( iIndex = 0; iIndex < iCountA; iIndex++ )
				newIndices.push_back ( iIndex );
			
			for ( int iVertex = 0; iVertex < iCountA; iVertex++ )
			{
				for ( int iSubVertex = 0; iSubVertex < iVertex; iSubVertex++ )
				{
					if ( pVerticesA [ iVertex ] == pVerticesA [ iSubVertex ] )
					{
						newIndices [ iSubVertex ] = newIndices [ iVertex ];

						break;
					}
				}
			}

			int iPos = 0;

			std::vector < int > addList;
			
			for ( int i = 0; i < iCountA; i++ )
			{
				std::vector < int >::iterator find = std::find ( addList.begin ( ), addList.end ( ), newIndices [ i ] );
				
				// mike - 020206 - addition for vs8
				//if ( find == NULL || find == addList.end ( ) )
				if ( find == addList.end ( ) )
				{
					newVertices.push_back ( pVerticesA [ newIndices [ i ] ] );
					addList.push_back ( newIndices [ i ] );
					newIndicesFinal.push_back ( iPos++ );
				}
				else
				{
					for ( int a = 0; a < (int)addList.size ( ); a++ )
					{
						if ( addList [ a ] == newIndices [ i ] )
						{
							newIndicesFinal.push_back ( a );
							break;
						}
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////
		
		
		int iVertexCount = newVertices.size ( );
		int iIndexCount  = newIndicesFinal.size ( );

		// replace mesh data directly!
		sOffsetMap offsetMap;
		SAFE_DELETE_ARRAY ( pMeshA->pVertexData );
		SAFE_DELETE_ARRAY ( pMeshA->pIndices );
		GetFVFOffsetMap ( pMeshA, &offsetMap );
		SetupMeshFVFData ( pMeshA, pMeshA->dwFVF, iVertexCount, iIndexCount );
		for ( int iIndex = 0; iIndex < iIndexCount; iIndex++ )
		{
			pMeshA->pIndices [ iIndex ] = newIndicesFinal [ iIndex ];
		}
		for ( int iVertex = 0; iVertex < iVertexCount; iVertex++ )
		{
			// copy the vertex into the mesh vertex data
			memcpy ( ( ( sVertex* ) pMeshA->pVertexData + iVertex ), &newVertices [ iVertex ], sizeof ( sVertex ) );
		}
		pMeshA->iDrawVertexCount = iVertexCount;
		pMeshA->iDrawPrimitives  = iIndexCount / 3;
	}

	D3DXMatrixInverse ( &OriginalMatA, NULL, &OriginalMatA );
	D3DXMatrixInverse ( &OriginalMatB, NULL, &OriginalMatB );
	TransformVertices ( pMeshA, OriginalMatA );
	TransformVertices ( pMeshB, OriginalMatB );
	CalculateMeshBounds ( pMeshA );
	CalculateMeshBounds ( pMeshB );

	return true;
}

bool cCSG::Difference ( sMesh* pMeshA, D3DXMATRIX OriginalMatA, sMesh* pMeshB, D3DXMATRIX OriginalMatB )
{
	// perform a difference operation on two meshes
	// check parameters
	if ( !pMeshA || !pMeshB )
		return false;

	// set up two compiler variables
	cCompiler	compilerA,
				compilerB;

	// transform the mesh vertices by the input matrices
	TransformVertices ( pMeshA, OriginalMatA );
	TransformVertices ( pMeshB, OriginalMatB );

	// get the new mesh bounds
	CalculateMeshBounds ( pMeshA );
	CalculateMeshBounds ( pMeshB );

	// only perform operation if the mesh bounding boxes intersect
	float fErrorBit = 0.1f;
	if (
			pMeshA->Collision.vecMax.x+fErrorBit >= pMeshB->Collision.vecMin.x && pMeshA->Collision.vecMin.x-fErrorBit <= pMeshB->Collision.vecMax.x &&
			pMeshA->Collision.vecMax.y+fErrorBit >= pMeshB->Collision.vecMin.y && pMeshA->Collision.vecMin.y-fErrorBit <= pMeshB->Collision.vecMax.y &&
			pMeshA->Collision.vecMax.z+fErrorBit >= pMeshB->Collision.vecMin.z && pMeshA->Collision.vecMin.z-fErrorBit <= pMeshB->Collision.vecMax.z
	   )
	{
		// set up compilerA options
		compilerA.m_OptionsHSR.bEnabled			  = false;		// no hidden surface removal
		compilerA.m_OptionsESR.bEnabled			  = false;		// no exterior surface removal
		compilerA.m_OptionsBSP.bEnabled			  = true;		// turn BSP on
		compilerA.m_OptionsBSP.iTreeType		  = 0;			// non split BSP tree
		compilerA.m_OptionsBSP.bLinkDetailBrushes = false;		// no detail brushes
		compilerA.m_OptionsPRT.bEnabled			  = false;		// no portals
		compilerA.m_OptionsPVS.bEnabled			  = false;		// no potential visibility set
		compilerA.m_OptionsTJR.bEnabled			  = false;		// no t-junction removal

		// set up compilerB options
		compilerB.m_OptionsHSR.bEnabled			  = false;		// no hidden surface removal
		compilerB.m_OptionsESR.bEnabled			  = false;		// no exterior surface removal
		compilerB.m_OptionsBSP.bEnabled			  = true;		// turn BSP on
		compilerB.m_OptionsBSP.iTreeType		  = 0;			// non split BSP tree
		compilerB.m_OptionsBSP.bLinkDetailBrushes = false;		// no detail brushes
		compilerB.m_OptionsPRT.bEnabled			  = false;		// no portals
		compilerB.m_OptionsPVS.bEnabled			  = false;		// no potential visibility set
		compilerB.m_OptionsTJR.bEnabled			  = false;		// no t-junction removal

		// create BSP trees for meshA and meshB (can fail if mesh not trilist)
		if ( compilerA.CompileScene ( pMeshA, &OriginalMatA, NULL, NULL )==false ) return false;
		if ( compilerB.CompileScene ( pMeshB, &OriginalMatB, NULL, NULL )==false ) return false;

		// clip the trees against each other
		compilerB.Clip ( compilerA.m_pBSPTree, true  );
		compilerA.Clip ( compilerB.m_pBSPTree, false );

		// build new polygon lists
		compilerA.BuildPolygonList ( false );
		compilerB.BuildPolygonList ( false );

		// finally rebuild meshA
		BuildVertexAndIndexList ( &compilerA, &compilerB, pMeshA );
	}

	// we need to transform the vertices of the new meshes against
	// the inverse of the original matrices, this is done so the
	// original world position is not altered in any way
	D3DXMatrixInverse ( &OriginalMatA, NULL, &OriginalMatA );
	D3DXMatrixInverse ( &OriginalMatB, NULL, &OriginalMatB );

	// transform vertices by inverse matrices
	TransformVertices ( pMeshA, OriginalMatA );
	TransformVertices ( pMeshB, OriginalMatB );

	// recalculate mesh bounds
	CalculateMeshBounds ( pMeshA );
	CalculateMeshBounds ( pMeshB );

	// generate normals for both meshes as vertices have changed
	GenerateNormals ( pMeshA );
	GenerateNormals ( pMeshB );
	
	// all finished
	return true;
}

bool cCSG::Intersection ( sMesh* pMeshA, D3DXMATRIX OriginalMatA, sMesh* pMeshB, D3DXMATRIX OriginalMatB )
{
	// check parameters
	if ( !pMeshA || !pMeshB )
		return false;

	// set up two compiler variables
	cCompiler	compilerA,
				compilerB;

	// transform the mesh vertices by the input matrices
	TransformVertices ( pMeshA, OriginalMatA );
	TransformVertices ( pMeshB, OriginalMatB );

	// get the new mesh bounds
	CalculateMeshBounds ( pMeshA );
	CalculateMeshBounds ( pMeshB );

	// only perform operation if the mesh bounding boxes intersect
	float fErrorBit = 0.1f;
	if (
			pMeshA->Collision.vecMax.x+fErrorBit >= pMeshB->Collision.vecMin.x && pMeshA->Collision.vecMin.x-fErrorBit <= pMeshB->Collision.vecMax.x &&
			pMeshA->Collision.vecMax.y+fErrorBit >= pMeshB->Collision.vecMin.y && pMeshA->Collision.vecMin.y-fErrorBit <= pMeshB->Collision.vecMax.y &&
			pMeshA->Collision.vecMax.z+fErrorBit >= pMeshB->Collision.vecMin.z && pMeshA->Collision.vecMin.z-fErrorBit <= pMeshB->Collision.vecMax.z
	   )
	{
		// set up compilerA options
		compilerA.m_OptionsHSR.bEnabled			  = false;		// no hidden surface removal
		compilerA.m_OptionsESR.bEnabled			  = false;		// no exterior surface removal
		compilerA.m_OptionsBSP.bEnabled			  = true;		// turn BSP on
		compilerA.m_OptionsBSP.iTreeType		  = 0;			// non split BSP tree
		compilerA.m_OptionsBSP.bLinkDetailBrushes = false;		// no detail brushes
		compilerA.m_OptionsPRT.bEnabled			  = false;		// no portals
		compilerA.m_OptionsPVS.bEnabled			  = false;		// no potential visibility set
		compilerA.m_OptionsTJR.bEnabled			  = false;		// no t-junction removal

		// set up compilerB options
		compilerB.m_OptionsHSR.bEnabled			  = false;		// no hidden surface removal
		compilerB.m_OptionsESR.bEnabled			  = false;		// no exterior surface removal
		compilerB.m_OptionsBSP.bEnabled			  = true;		// turn BSP on
		compilerB.m_OptionsBSP.iTreeType		  = 0;			// non split BSP tree
		compilerB.m_OptionsBSP.bLinkDetailBrushes = false;		// no detail brushes
		compilerB.m_OptionsPRT.bEnabled			  = false;		// no portals
		compilerB.m_OptionsPVS.bEnabled			  = false;		// no potential visibility set
		compilerB.m_OptionsTJR.bEnabled			  = false;		// no t-junction removal

		// create BSP trees for meshA and meshB
		compilerA.CompileScene ( pMeshA, &OriginalMatA, NULL, NULL );
		compilerB.CompileScene ( pMeshB, &OriginalMatB, NULL, NULL );

		// this clip approach is good for HSR
		compilerB.Clip ( compilerA.m_pBSPTree, true  );
		compilerA.BuildPolygonList ( false );

		// finally rebuild meshA
		BuildVertexAndIndexList ( &compilerA, &compilerB, pMeshA );
	}

	// we need to transform the vertices of the new meshes against
	// the inverse of the original matrices, this is done so the
	// original world position is not altered in any way
	D3DXMatrixInverse ( &OriginalMatA, NULL, &OriginalMatA );
	D3DXMatrixInverse ( &OriginalMatB, NULL, &OriginalMatB );

	// transform vertices by inverse matrices
	TransformVertices ( pMeshA, OriginalMatA );
	TransformVertices ( pMeshB, OriginalMatB );

	// recalculate mesh bounds
	CalculateMeshBounds ( pMeshA );
	CalculateMeshBounds ( pMeshB );

	// generate normals for both meshes as vertices have changed
	GenerateNormals ( pMeshA );
	GenerateNormals ( pMeshB );
	
	// all finished
	return true;
}

void ClipMeshWithBoundArea ( sMesh* pMesh, D3DXVECTOR3* pvecMin, D3DXVECTOR3* pvecMax )
{
	// get the offset map for the vertices
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// only those that have indices to trim
	if ( !pMesh->pIndices )
		return;

	// get number of faces and make a deposit buffer for new mesh indices
	DWORD dwNumFaces = pMesh->dwIndexCount / 3;
	WORD* pNewIndices = new WORD [ pMesh->dwIndexCount ];
	int newi = 0;

	// go through all polys
	float* pBase = (float*)pMesh->pVertexData+offsetMap.dwX;
	for ( DWORD i = 0; i < dwNumFaces; i++ )
	{
		// get face vectors
		WORD wFace0, wFace1, wFace2;
		wFace0 = pMesh->pIndices [ 3 * i + 0 ];
		wFace1 = pMesh->pIndices [ 3 * i + 1 ];
		wFace2 = pMesh->pIndices [ 3 * i + 2 ];

		// get vertex positions
		D3DXVECTOR3 v[3];
		v[0] = *(D3DXVECTOR3*)((float*)pBase+(wFace0*offsetMap.dwSize));
		v[1] = *(D3DXVECTOR3*)((float*)pBase+(wFace1*offsetMap.dwSize));
		v[2] = *(D3DXVECTOR3*)((float*)pBase+(wFace2*offsetMap.dwSize));

		// if all fall within bound area, remove face
		bool bAllFallWithin=true;
		for ( int vi=0; vi<3; vi++ )
		{
			if ( v[vi].x >= pvecMin->x && v[vi].x <= pvecMax->x 
			&&	 v[vi].y >= pvecMin->y && v[vi].y <= pvecMax->y 
			&&	 v[vi].z >= pvecMin->z && v[vi].z <= pvecMax->z )
			{
				// yes the vertex is inside boundarea
			}
			else
			{
				bAllFallWithin=false;
			}
		}
		if ( bAllFallWithin==false )
		{
			// copy retained poly in new buffer
			pNewIndices [ 3 * newi + 0 ] = wFace0;
			pNewIndices [ 3 * newi + 1 ] = wFace1;
			pNewIndices [ 3 * newi + 2 ] = wFace2;
			newi++;
		}
	}

	// construct new index buffer data
	memcpy ( pMesh->pIndices, pNewIndices, newi * 3 * sizeof(WORD) );
	pMesh->dwIndexCount = newi * 3;
	pMesh->iDrawPrimitives = newi;

	// delete temp buffer
	SAFE_DELETE(pNewIndices);
}

bool cCSG::Clip ( sMesh* pMeshA, D3DXMATRIX OriginalMatA, sMesh* pMeshB, D3DXMATRIX OriginalMatB )
{
	// check parameters
	if ( !pMeshA || !pMeshB )
		return false;

	// if punch is more than a box, ensure it is SOLID
	if ( pMeshB->dwIndexCount > 36 )
		if ( CheckIfMeshSolid ( pMeshB, 5, 5, 5 ) < 2 )
			return false;

	// get the new mesh bounds
	D3DXVECTOR3 vecMinA = pMeshA->Collision.vecMin;
	D3DXVECTOR3 vecMaxA = pMeshA->Collision.vecMax;
	D3DXVec3TransformCoord ( &vecMinA, &vecMinA, &OriginalMatA );
	D3DXVec3TransformCoord ( &vecMaxA, &vecMaxA, &OriginalMatA );
	D3DXVECTOR3 vecMinB = pMeshB->Collision.vecMin;
	D3DXVECTOR3 vecMaxB = pMeshB->Collision.vecMax;
	D3DXVec3TransformCoord ( &vecMinB, &vecMinB, &OriginalMatB );
	D3DXVec3TransformCoord ( &vecMaxB, &vecMaxB, &OriginalMatB );

	// Ensure min and max are correct
	D3DXVECTOR3 vecT;
	if ( vecMinA.x>vecMaxA.x ) { vecT.x=vecMinA.x; vecMinA.x=vecMaxA.x; vecMaxA.x=vecT.x; }
	if ( vecMinA.y>vecMaxA.y ) { vecT.y=vecMinA.y; vecMinA.y=vecMaxA.y; vecMaxA.y=vecT.y; }
	if ( vecMinA.z>vecMaxA.z ) { vecT.z=vecMinA.z; vecMinA.z=vecMaxA.z; vecMaxA.z=vecT.z; }
	if ( vecMinB.x>vecMaxB.x ) { vecT.x=vecMinB.x; vecMinB.x=vecMaxB.x; vecMaxB.x=vecT.x; }
	if ( vecMinB.y>vecMaxB.y ) { vecT.y=vecMinB.y; vecMinB.y=vecMaxB.y; vecMaxB.y=vecT.y; }
	if ( vecMinB.z>vecMaxB.z ) { vecT.z=vecMinB.z; vecMinB.z=vecMaxB.z; vecMaxB.z=vecT.z; }

	// If both bounds are IDENTICAL, ignore!
	if ( vecMinA.x!=vecMinB.x || vecMaxA.x!=vecMaxB.x
	||   vecMinA.y!=vecMinB.y || vecMaxA.y!=vecMaxB.y
	||   vecMinA.z!=vecMinB.z || vecMaxA.z!=vecMaxB.z )
	{
		// only perform operation if the mesh bounding boxes intersect
		float fErrorBit = 0.1f;
		vecMinB.x-=fErrorBit; vecMinB.y-=fErrorBit; vecMinB.z-=fErrorBit;
		vecMaxB.x+=fErrorBit; vecMaxB.y+=fErrorBit; vecMaxB.z+=fErrorBit;
		if ( vecMaxA.x >= vecMinB.x && vecMaxA.y >= vecMinB.y
		&&   vecMaxA.z >= vecMinB.z && vecMinA.x <= vecMaxB.x 
		&&   vecMinA.y <= vecMaxB.y && vecMinA.z <= vecMaxB.z )
		{
			// adjust punch bound area (B) by transform of A
			D3DXMatrixInverse ( &OriginalMatA, NULL, &OriginalMatA );
			D3DXVec3TransformCoord ( &vecMinB, &vecMinB, &OriginalMatA );
			D3DXVec3TransformCoord ( &vecMaxB, &vecMaxB, &OriginalMatA );

			// Ensure min and max are correct
			if ( vecMinB.x>vecMaxB.x ) { vecT.x=vecMinB.x; vecMinB.x=vecMaxB.x; vecMaxB.x=vecT.x; }
			if ( vecMinB.y>vecMaxB.y ) { vecT.y=vecMinB.y; vecMinB.y=vecMaxB.y; vecMaxB.y=vecT.y; }
			if ( vecMinB.z>vecMaxB.z ) { vecT.z=vecMinB.z; vecMinB.z=vecMaxB.z; vecMaxB.z=vecT.z; }

			// remove all polygons from A that fall inside bound area of B
			ClipMeshWithBoundArea ( pMeshA, &vecMinB, &vecMaxB );
		}
	}

	// all finished
	return true;
}

void cCSG::BuildVertexAndIndexList ( cCompiler* pA, cCompiler* pB, sMesh* pMesh )
{
	// build new vertex and index lists
	// check parameters
	if ( !pA || !pB || !pMesh )
		return;

	sOffsetMap	offsetMap;					// offset map
	sVertex*	pVerticesA   = NULL;		// vertices from a
	sVertex*	pVerticesB   = NULL;		// vertices from b
	int			iCountA      = 0;			// vertex count from a
	int			iCountB      = 0;			// vertex count from b
	int			iVertex      = 0;			// vertex offset
	int			iIndex       = 0;			// index offset
	int			iVertexCount = 0;			// final number of vertices
	int			iIndexCount  = 0;			// final number of indices
	int			iTempA       = 0;			// used for temp storage
	int			iTempB       = 0;			// used for temp storage

	// lists for storing data
	std::vector < sVertex > vertexListFromCompiler;
	std::vector < int     > indexListFromCompiler;
	std::vector < sVertex > vertexListFinal;
	std::vector < int     > indexListFinal;
	std::vector < int     > tempList;

	// get vertex pointers
	pVerticesA = ( sVertex* ) pA->m_pVertices;
	pVerticesB = ( sVertex* ) pB->m_pVertices;

	// store vertex count
	iCountA = pA->m_iVertexCount;
	iCountB = pB->m_iVertexCount;

	// copy first set of vertices from compiler
	for ( iVertex = 0; iVertex < iCountA; iVertex++ )
		vertexListFromCompiler.push_back ( pVerticesA [ iVertex ] );

	int i = 0;

	for ( iVertex = iCountA; iVertex < iCountA + iCountB; iVertex += 3 )
	{
		vertexListFromCompiler.push_back ( pVerticesB [ i + 2 ] );
		vertexListFromCompiler.push_back ( pVerticesB [ i + 1 ] );
		vertexListFromCompiler.push_back ( pVerticesB [ i + 0 ] );

		i += 3;
	}

	// copy indices, use a 0, 1, 2 .. list initially
	for ( iIndex = 0; iIndex < iCountA + iCountB; iIndex++ )
		indexListFromCompiler.push_back ( iIndex );

	// go through all vertices
	for ( iVertex = 0; iVertex < (int)vertexListFromCompiler.size ( ); iVertex++ )
	{
		// find vertices below current vertex in list
		for ( iTempA = 0; iTempA < iVertex; iTempA++ )
		{
			// do we have a match
			if ( vertexListFromCompiler [ iVertex ] == vertexListFromCompiler [ iTempA ] )
			{
				// store the offset
				indexListFromCompiler [ iTempA ] = indexListFromCompiler [ iVertex ];
				break;
			}
		}
	}

	// now sort indices
	for ( iIndex = 0, iTempA = 0; iIndex < iCountA + iCountB; iIndex++ )
	{
		// find an index matching the index id
		std::vector<int>::iterator find = std::find ( tempList.begin ( ), tempList.end ( ), indexListFromCompiler [ iIndex ] );

		// if we don't find anything
		//if ( find == NULL || find == tempList.end ( ) )
		// mike - 020206 - addition for vs8
		if ( find == tempList.end ( ) )
		{
			// add vertex into list
			vertexListFinal.push_back ( vertexListFromCompiler [ indexListFromCompiler [ iIndex ] ] );

			// send index into list
			tempList.push_back ( indexListFromCompiler [ iIndex ] );

			// store final index value
			indexListFinal.push_back ( iTempA++ );
		}
		else
		{
			// see if we have any matches
			for ( iTempB = 0; iTempB < (int)tempList.size ( ); iTempB++ )
			{
				if ( tempList [ iTempB ] == indexListFromCompiler [ iIndex ] )
				{
					indexListFinal.push_back ( iTempB );
					break;
				}
			}
		}
	}

	// store new vertex and index counts
	iVertexCount = vertexListFinal.size ( );
	iIndexCount  = indexListFinal.size  ( );
	
	// delete original data
	SAFE_DELETE_ARRAY ( pMesh->pVertexData );
	SAFE_DELETE_ARRAY ( pMesh->pIndices );

	// create new data
	SetupMeshFVFData ( pMesh, pMesh->dwFVF, iVertexCount, iIndexCount );

	// get the offset map
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// copy new indices across to mesh
	for ( iIndex = 0; iIndex < iIndexCount; iIndex++ )
		pMesh->pIndices [ iIndex ] = indexListFinal [ iIndex ];

	// and now copy vertices across to mesh
	for ( iVertex = 0; iVertex < iVertexCount; iVertex++ )
		memcpy ( ( ( sVertex* ) pMesh->pVertexData + iVertex ), &vertexListFinal [ iVertex ], sizeof ( sVertex ) );
	
	// set values for mesh
	pMesh->iDrawVertexCount = iVertexCount;
	pMesh->iDrawPrimitives  = iIndexCount / 3;

	// combine polys that are subsets of another
	CombineSubsetPolygonsInMesh ( pMesh );

	return;
}

void TransformVertices ( sMesh* pMesh, D3DXMATRIX matWorld )
{
	// get the offset map for the vertices
	sOffsetMap	offsetMap;
	GetFVFOffsetMap ( pMesh, &offsetMap );

	// now we need to transform the vertices for each object
	for ( int iVertex = 0; iVertex < ( int ) pMesh->dwVertexCount; iVertex++ )
	{
		// get a pointer to the vertex data
		BYTE* pVertex = pMesh->pVertexData;

		// transform position
		if ( offsetMap.dwZ!=0 )
		{
			D3DXVECTOR3 vecPosition = D3DXVECTOR3 ( 
													*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ),
													*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) )
												  );

			D3DXVec3TransformCoord ( &vecPosition, &vecPosition, &matWorld );

			*( ( float* ) pVertex + offsetMap.dwX + ( offsetMap.dwSize * iVertex ) ) = vecPosition.x;
			*( ( float* ) pVertex + offsetMap.dwY + ( offsetMap.dwSize * iVertex ) ) = vecPosition.y;
			*( ( float* ) pVertex + offsetMap.dwZ + ( offsetMap.dwSize * iVertex ) ) = vecPosition.z;
		}
	}
}
