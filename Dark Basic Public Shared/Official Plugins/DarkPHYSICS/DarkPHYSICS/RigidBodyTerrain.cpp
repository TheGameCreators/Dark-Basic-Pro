
#include "globals.h"
#include "rigidbodycreation.h"
#include "stream.h"
#include "material.h"
#include "rigidbodyterrain.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE RIGID BODY STATIC TERRAIN%L%?dbPhyMakeRigidBodyStaticTerrain@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC TERRAIN%LL%?dbPhyMakeRigidBodyStaticTerrain@@YAXHH@Z%
	PHY MAKE RIGID BODY STATIC TERRAIN%LS%?dbPhyMakeRigidBodyStaticTerrain@@YAXHPAD@Z%
	PHY MAKE RIGID BODY STATIC TERRAIN%LSL%?dbPhyMakeRigidBodyStaticTerrain@@YAXHPADH@Z%

	PHY LOAD RIGID BODY STATIC TERRAIN%LS%?dbPhyLoadRigidBodyStaticTerrain@@YAXHPAD@Z%
	PHY LOAD RIGID BODY STATIC TERRAIN%LSL%?dbPhyLoadRigidBodyStaticTerrain@@YAXHPADH@Z%

	PHY SET RIGID BODY TERRAIN EXTENT%F%?dbPhySetRigidBodyTerrainExtent@@YAXM@Z%

	void dbPhySetRigidBodyTerrainExtent ( float fExtent )
	

	DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject );
	DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject, int iMaterial );
	DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject, char* szFile );
	DARKSDK void dbPhyMakeRigidBodyStaticTerrain	( int iObject, char* szFile, int iMaterial );

	DARKSDK void dbPhyLoadRigidBodyStaticTerrain	( int iObject, char* szFile );
	DARKSDK void dbPhyLoadRigidBodyStaticTerrain	( int iObject, char* szFile, int iMaterial );
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeRigidBodyStaticTerrain ( int iObject )
{
	dbPhyMakeRigidBodyStaticTerrain ( iObject, "", -1 );
}

void dbPhyMakeRigidBodyStaticTerrain ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBodyStaticTerrain ( iObject, "", iMaterial );
}

void dbPhyMakeRigidBodyStaticTerrain ( int iObject, char* szFile )
{
	//dbPhyMakeRigidBodyStaticTerrain ( iObject, szFile, -1 );
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyTerrain, PhysX::eStatic, -1, 0, szFile );
}

void dbPhyMakeRigidBodyStaticTerrain ( int iObject, char* szFile, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyTerrain, PhysX::eStatic, iMaterial );
}

void dbPhyLoadRigidBodyStaticTerrain ( int iObject, char* szFile )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyTerrain, PhysX::eStatic, -1, 1, szFile );
}

void dbPhyLoadRigidBodyStaticTerrain ( int iObject, char* szFile, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyTerrain, PhysX::eStatic, iMaterial, 1, szFile );
}

void dbPhyMakeRigidBodyTerrain ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile )
{
	int iVertexMax = 0;
	int iIndexMax = 0;

	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		iVertexMax += pObject->ppMeshList [ iMesh ]->dwVertexCount;
		iIndexMax  += pObject->ppMeshList [ iMesh ]->dwIndexCount;
	}

	NxVec3* pVertices		= new NxVec3 [ iVertexMax ];
	int*	iTriangles		= new int [ iIndexMax ];
	int		iVertexCount	= 0;
	int		iTriangleCount	= 0;
	int		iTriangleOffset = 0;

	for ( int iMesh = 0; iMesh < pObject->iMeshCount; iMesh++ )
	{
		// mesh properties
		sOffsetMap offsetMap;
		PhysX::GetFVFOffsetMap ( pObject->ppMeshList [ iMesh ], &offsetMap );

		for ( DWORD i = 0; i < pObject->ppMeshList [ iMesh ]->dwVertexCount; i++ )
		{
			pVertices [ iVertexCount ].x = *( ( float* ) pObject->ppMeshList [ iMesh ]->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * i ) );
			pVertices [ iVertexCount ].y = *( ( float* ) pObject->ppMeshList [ iMesh ]->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * i ) );
			pVertices [ iVertexCount ].z = *( ( float* ) pObject->ppMeshList [ iMesh ]->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * i ) );
			iVertexCount++;
		}

		for ( DWORD i = 0; i < pObject->ppMeshList [ iMesh ]->dwIndexCount; i++ )
			iTriangles [ iTriangleCount++ ] = iTriangleOffset + pObject->ppMeshList [ iMesh ]->pIndices [ i ];

		// next mesh
		iTriangleOffset += pObject->ppMeshList [ iMesh ]->dwVertexCount;
	}

	// prepare mesh for cooking
	NxTriangleMeshDesc levelDesc;
	levelDesc.numVertices				= iVertexCount;
	levelDesc.numTriangles				= iTriangleCount / 3;
	levelDesc.pointStrideBytes			= sizeof ( NxVec3 );
	levelDesc.triangleStrideBytes		= 3 * sizeof ( int );
	levelDesc.points					= pVertices;
	levelDesc.triangles					= iTriangles;
	levelDesc.flags						= NX_CF_COMPUTE_CONVEX;
	levelDesc.heightFieldVerticalAxis	= NX_Y;
	//levelDesc.heightFieldVerticalExtent = -256.0f;
	levelDesc.heightFieldVerticalExtent = PhysX::fHeightFieldVerticalExtent;

	
	//////////
	char szFileName [ 256 ] = "";
	bool bDelete = false;

	if ( strcmp ( szFile, "" ) == 0 )
	{
		strcpy ( szFileName, "temp.mesh" );
		bDelete = true;
	}
	else
	{
		strcpy ( szFileName, szFile );
	}
	//////////

	// cook the mesh
	NxInitCooking ( );
	MemoryWriteBuffer buf;
	//bool status = NxCookTriangleMesh ( levelDesc, buf );

	bool bStatus = false;

	if ( iSaveMode == 0 )
		bStatus = NxCookTriangleMesh(levelDesc, UserStream(szFileName, false));
	else
		bStatus = true;

	if ( bStatus )
	{
		NxTriangleMeshShapeDesc* pLevelShapeDesc = new NxTriangleMeshShapeDesc;
		//pLevelShapeDesc->meshData = PhysX::pPhysicsSDK->createTriangleMesh ( MemoryReadBuffer ( buf.data ) );
		pLevelShapeDesc->meshData = PhysX::pPhysicsSDK->createTriangleMesh ( UserStream(szFileName, true) );

		if ( pMaterial )
			pLevelShapeDesc->materialIndex = pMaterial->matIndex;

		pActorDesc->shapes.pushBack ( pLevelShapeDesc );

		if ( bDelete == true )
		{
			DeleteFile ( szFileName );
		}
	}
}

void dbPhySetRigidBodyTerrainExtent ( float fExtent )
{
	PhysX::fHeightFieldVerticalExtent = fExtent;
}