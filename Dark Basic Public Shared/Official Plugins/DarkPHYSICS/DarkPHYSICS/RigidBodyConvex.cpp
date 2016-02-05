
#include "globals.h"
#include "rigidbodycreation.h"
#include "stream.h"
#include "material.h"
#include "rigidbodyconvex.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE RIGID BODY STATIC CONVEX%L%?dbPhyMakeRigidBodyStaticConvex@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC CONVEXES%L%?dbPhyMakeRigidBodyStaticConvexes@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC CONVEX%LL%?dbPhyMakeRigidBodyStaticConvex@@YAXHH@Z%
	PHY MAKE RIGID BODY STATIC CONVEXES%LL%?dbPhyMakeRigidBodyStaticConvexes@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC CONVEX%L%?dbPhyMakeRigidBodyDynamicConvex@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC CONVEXES%L%?dbPhyMakeRigidBodyDynamicConvexes@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC CONVEX%LL%?dbPhyMakeRigidBodyDynamicConvex@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC CONVEXES%LL%?dbPhyMakeRigidBodyDynamicConvexes@@YAXHH@Z%

	PHY MAKE RIGID BODY STATIC CONVEX%LS%?dbPhyMakeRigidBodyStaticConvex@@YAXHPAD@Z%
	PHY MAKE RIGID BODY STATIC CONVEXES%LS%?dbPhyMakeRigidBodyStaticConvexes@@YAXHPAD@Z%
	PHY MAKE RIGID BODY STATIC CONVEX%LSL%?dbPhyMakeRigidBodyStaticConvex@@YAXHPADH@Z%
	PHY MAKE RIGID BODY STATIC CONVEXES%LSL%?dbPhyMakeRigidBodyStaticConvexes@@YAXHPADH@Z%
	PHY MAKE RIGID BODY DYNAMIC CONVEX%LS%?dbPhyMakeRigidBodyDynamicConvex@@YAXHPAD@Z%
	PHY MAKE RIGID BODY DYNAMIC CONVEXES%LS%?dbPhyMakeRigidBodyDynamicConvexes@@YAXHPAD@Z%
	PHY MAKE RIGID BODY DYNAMIC CONVEX%LSL%?dbPhyMakeRigidBodyDynamicConvex@@YAXHPADH@Z%
	PHY MAKE RIGID BODY DYNAMIC CONVEXES%LSL%?dbPhyMakeRigidBodyDynamicConvexes@@YAXHPADH@Z%

	PHY LOAD RIGID BODY STATIC CONVEX%LS%?dbPhyLoadRigidBodyStaticConvex@@YAXHPAD@Z%
	PHY LOAD RIGID BODY STATIC CONVEXES%LS%?dbPhyLoadRigidBodyStaticConvexes@@YAXHPAD@Z%
	PHY LOAD RIGID BODY STATIC CONVEX%LSL%?dbPhyLoadRigidBodyStaticConvex@@YAXHPADH@Z%
	PHY LOAD RIGID BODY STATIC CONVEXES%LSL%?dbPhyLoadRigidBodyStaticConvexes@@YAXHPADH@Z%
	PHY LOAD RIGID BODY DYNAMIC CONVEX%LS%?dbPhyLoadRigidBodyDynamicConvex@@YAXHPAD@Z%
	PHY LOAD RIGID BODY DYNAMIC CONVEXES%LS%?dbPhyLoadRigidBodyDynamicConvexes@@YAXHPAD@Z%
	PHY LOAD RIGID BODY DYNAMIC CONVEX%LSL%?dbPhyLoadRigidBodyDynamicConvex@@YAXHPADH@Z%
	PHY LOAD RIGID BODY DYNAMIC CONVEXES%LSL%?dbPhyLoadRigidBodyDynamicConvexes@@YAXHPADH@Z%

	void dbPhyMakeRigidBodyStaticConvex		( int iObject );
	void dbPhyMakeRigidBodyStaticConvexes	( int iObject );
	void dbPhyMakeRigidBodyStaticConvex		( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyStaticConvexes	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyDynamicConvex	( int iObject );
	void dbPhyMakeRigidBodyDynamicConvexes	( int iObject );
	void dbPhyMakeRigidBodyDynamicConvex	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyDynamicConvexes	( int iObject, int iMaterial );

	void dbPhyMakeRigidBodyStaticConvex		( int iObject, char* szFile );
	void dbPhyMakeRigidBodyStaticConvexes	( int iObject, char* szFile );
	void dbPhyMakeRigidBodyStaticConvex		( int iObject, char* szFile, int iMaterial );
	void dbPhyMakeRigidBodyStaticConvexes	( int iObject, char* szFile, int iMaterial );
	void dbPhyMakeRigidBodyDynamicConvex	( int iObject, char* szFile );
	void dbPhyMakeRigidBodyDynamicConvexes	( int iObject, char* szFile );
	void dbPhyMakeRigidBodyDynamicConvex	( int iObject, char* szFile, int iMaterial );
	void dbPhyMakeRigidBodyDynamicConvexes	( int iObject, char* szFile, int iMaterial );
	
	void dbPhyLoadRigidBodyStaticConvex		( int iObject, char* szFile );
	void dbPhyLoadRigidBodyStaticConvexes	( int iObject, char* szFile );
	void dbPhyLoadRigidBodyStaticConvex		( int iObject, char* szFile, int iMaterial );
	void dbPhyLoadRigidBodyStaticConvexes	( int iObject, char* szFile, int iMaterial );
	void dbPhyLoadRigidBodyDynamicConvex	( int iObject, char* szFile );
	void dbPhyLoadRigidBodyDynamicConvexes	( int iObject, char* szFile );
	void dbPhyLoadRigidBodyDynamicConvex	( int iObject, char* szFile, int iMaterial );
	void dbPhyLoadRigidBodyDynamicConvexes	( int iObject, char* szFile, int iMaterial );
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeRigidBodyStaticConvex	( int iObject )
{
	dbPhyMakeRigidBodyStaticConvex ( iObject, -1 );
}

void dbPhyMakeRigidBodyStaticConvexes ( int iObject )
{

}

void dbPhyMakeRigidBodyStaticConvex	( int iObject, int iMaterial )
{
	dbPhyMakeRigidBodyStaticConvex ( iObject, "", iMaterial );
}

void dbPhyMakeRigidBodyStaticConvexes ( int iObject, int iMaterial )
{

}

void dbPhyMakeRigidBodyDynamicConvex ( int iObject )
{
	dbPhyMakeRigidBodyDynamicConvex ( iObject, -1 );
}

void dbPhyMakeRigidBodyDynamicConvexes ( int iObject )
{

}

void dbPhyMakeRigidBodyDynamicConvex ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBodyDynamicConvex ( iObject, "", iMaterial );
}

void dbPhyMakeRigidBodyDynamicConvexes ( int iObject, int iMaterial )
{

}

void dbPhyMakeRigidBodyStaticConvex ( int iObject, char* szFile )
{
	dbPhyMakeRigidBodyStaticConvex ( iObject, szFile, -1 );
}

void dbPhyMakeRigidBodyStaticConvexes ( int iObject, char* szFile )
{

}

void dbPhyMakeRigidBodyStaticConvex ( int iObject, char* szFile, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyConvex, PhysX::eStatic, iMaterial );
}

void dbPhyMakeRigidBodyStaticConvexes ( int iObject, char* szFile, int iMaterial )
{
	
}

void dbPhyMakeRigidBodyDynamicConvex ( int iObject, char* szFile )
{
	dbPhyMakeRigidBodyDynamicConvex ( iObject, szFile, -1 );
}

void dbPhyMakeRigidBodyDynamicConvexes ( int iObject, char* szFile )
{

}

void dbPhyMakeRigidBodyDynamicConvex ( int iObject, char* szFile, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyConvex, PhysX::eDynamic, iMaterial );
}

void dbPhyMakeRigidBodyDynamicConvexes ( int iObject, char* szFile, int iMaterial )
{

}

void dbPhyLoadRigidBodyStaticConvex	( int iObject, char* szFile )
{

}

void dbPhyLoadRigidBodyStaticConvexes ( int iObject, char* szFile )
{

}

void dbPhyLoadRigidBodyStaticConvex ( int iObject, char* szFile, int iMaterial )
{

}

void dbPhyLoadRigidBodyStaticConvexes ( int iObject, char* szFile, int iMaterial )
{

}

void dbPhyLoadRigidBodyDynamicConvex ( int iObject, char* szFile )
{

}

void dbPhyLoadRigidBodyDynamicConvexes ( int iObject, char* szFile )
{

}

void dbPhyLoadRigidBodyDynamicConvex ( int iObject, char* szFile, int iMaterial )
{

}

void dbPhyLoadRigidBodyDynamicConvexes ( int iObject, char* szFile, int iMaterial )
{

}

void dbPhyMakeRigidBodyConvex ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile )
{
	NxConvexShapeDesc* pConvexShapeDesc = new NxConvexShapeDesc;

	sMesh* pMesh = pObject->ppMeshList [ 0 ];

	NxVec3* verts = new NxVec3[pMesh->dwVertexCount];
	int*	iTriangles		= new int [ pMesh->dwIndexCount ];
	
	sOffsetMap offsetMap;
	PhysX::GetFVFOffsetMap ( pObject->ppMeshList [ 0 ], &offsetMap );

	for ( int i = 0; i < ( int ) pMesh->dwVertexCount; i++ )
	{
		verts [ i ].x = *( ( float* ) pObject->ppMeshList [ 0 ]->pVertexData + offsetMap.dwX + ( offsetMap.dwSize * i ) );
		verts [ i ].y = *( ( float* ) pObject->ppMeshList [ 0 ]->pVertexData + offsetMap.dwY + ( offsetMap.dwSize * i ) );
		verts [ i ].z = *( ( float* ) pObject->ppMeshList [ 0 ]->pVertexData + offsetMap.dwZ + ( offsetMap.dwSize * i ) );
	}

	for ( int i = 0; i < ( int ) pMesh->dwIndexCount; i++ )
			iTriangles [ i ] = pMesh->pIndices [ i ];

	NxConvexMeshDesc convexDesc;
	convexDesc.numTriangles			= pMesh->dwIndexCount / 3;
	convexDesc.numVertices			= pMesh->dwVertexCount;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.triangleStrideBytes  = 3 * sizeof ( int );
	convexDesc.points				= verts;
	convexDesc.triangles			= iTriangles;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	//convexDesc.materialIndex = materialIce;


	//convexDesc.flags				= NX_CF_COMPUTE_CONVEX | NX_CF_16_BIT_INDICES;
	
	//NxInitCooking();

	// Cooking from memory
	MemoryWriteBuffer buf;

	if(NxCookConvexMesh(convexDesc, buf))
	{

		
		pConvexShapeDesc->meshData = PhysX::pPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));

		if ( pMaterial )
			pConvexShapeDesc->materialIndex = pMaterial->matIndex;

		if(pConvexShapeDesc->meshData)
		{
			pActorDesc->shapes.pushBack ( pConvexShapeDesc );

		}
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create convex mesh", "", 0 );
	}
}

/*
float fCylinderVerts [ ] = 
{
   0.000000,-0.500000,0.000000,
   0.000000,0.500000,0.000000,
   -1.000000,-0.500000,0.000000,
   -1.000000,0.500000,0.000000,
   -0.707107,-0.500000,0.707107,
   -0.707107,0.500000,0.707107,
   0.000000,-0.500000,1.000000,
   0.000000,0.500000,1.000000,
   0.707107,-0.500000,0.707107,
   0.707107,0.500000,0.707107,
   1.000000,-0.500000,0.000000,
   1.000000,0.500000,0.000000,
   0.707107,-0.500000,-0.707107,
   0.707107,0.500000,-0.707107,
   0.000000,-0.500000,-1.000000,
   0.000000,0.500000,-1.000000,
   -0.707107,-0.500000,-0.707107,
   -0.707107,0.500000,-0.707107
};

int iCylinderIndices [ ] =
{
   4,2,0,
   2,4,5,
   2,5,3,
   3,5,1,
   6,4,0,
   4,6,7,
   4,7,5,
   5,7,1,
   8,6,0,
   6,8,9,
   6,9,7,
   7,9,1,
   10,8,0,
   8,10,11,
   8,11,9,
   9,11,1,
   12,10,0,
   10,12,1,
   10,13,11,
   11,13,1,
   14,12,0,
   12,14,15,
   12,15,1,
   13,15,1,
   16,14,0,
   14,16,17,
   14,17,15,
   15,17,1,
   2,16,0,
   16,2,3,
   16,3,17,
   17,3,1
};

void dbPhyMakeRigidBodyConvex ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, int iSaveMode, char* szFile )
{
	NxConvexShapeDesc* pConvexShapeDesc = new NxConvexShapeDesc;

	sMesh* pMesh = pObject->ppMeshList [ 0 ];

	NxVec3* verts = new NxVec3 [ 18 ];
	int*	iTriangles		= new int [ 32 * 3 ];
	
	sOffsetMap offsetMap;
	PhysX::GetFVFOffsetMap ( pObject->ppMeshList [ 0 ], &offsetMap );

	for ( int i = 0; i < ( int ) 18; i++ )
	{
		D3DXVECTOR3 vecPos = D3DXVECTOR3 ( fCylinderVerts [ i * 3 + 0 ], fCylinderVerts [ i * 3 + 1 ], fCylinderVerts [ i * 3 + 2 ] );

		D3DXMATRIX matWorld;

		float fXSize = pObject->collision.vecMax.x - pObject->collision.vecMin.x;
		float fYSize = pObject->collision.vecMax.y - pObject->collision.vecMin.y;
		float fZSize = pObject->collision.vecMax.z - pObject->collision.vecMin.z;

		D3DXMatrixScaling ( &matWorld, fXSize, fYSize, fZSize );

		D3DXVec3TransformCoord ( &vecPos, &vecPos, &matWorld );

		verts [ i ].x = vecPos.x;
		verts [ i ].y = vecPos.y;
		verts [ i ].z = vecPos.z;
	}

	for ( int i = 0; i < ( int ) 32 * 3; i++ )
			iTriangles [ i ] = iCylinderIndices [ i ];

	NxConvexMeshDesc convexDesc;
	convexDesc.numTriangles			= 32;
	convexDesc.numVertices			= 18;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.triangleStrideBytes  = 3 * sizeof ( int );
	convexDesc.points				= verts;
	convexDesc.triangles			= iTriangles;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;
	
	// Cooking from memory
	MemoryWriteBuffer buf;

	if(NxCookConvexMesh(convexDesc, buf))
	{

		
		pConvexShapeDesc->meshData = PhysX::pPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));

		if ( pMaterial )
			pConvexShapeDesc->materialIndex = pMaterial->matIndex;

		if(pConvexShapeDesc->meshData)
		{
			pActorDesc->shapes.pushBack ( pConvexShapeDesc );

		}
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create convex mesh", "", 0 );
	}
}
*/