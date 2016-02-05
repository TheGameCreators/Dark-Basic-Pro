
/////////////////////////////////////////////////////////////////////////////////////
// CLOTH INCLUDES ///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include <darksdk.h>
#include "globals.h"
#include "cloth.h"
#include "rigidbodycreation.h"
#include "clothobject.h"
#include "clothstructure.h"

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// INTERNAL CLOTH FUNCTION LISTINGS /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
// CLOTH FUNCTIONS //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////






/*
cClothObject *regularCloth = NULL;

void dbPhyMakeCloth ( int iID )
{
	PhysX::sPhysXObject* pA  = dbPhyGetObject ( 2 );
	NxActor *box2 = pA->pActor;
	
	//
	NxD6JointDesc d6Desc;
	d6Desc.actor[0] = NULL;
	d6Desc.actor[1] = box2;
	NxVec3 globalAnchor(0,7,0);
	d6Desc.localAnchor[0] = globalAnchor;

	d6Desc.localAxis[0]   = NxVec3(1,0,0);
	d6Desc.localNormal[0] = NxVec3(0,1,0);
	d6Desc.localAxis[1]   = NxVec3(1,0,0);
	d6Desc.localNormal[1] = NxVec3(0,1,0);

	d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.xMotion = NX_D6JOINT_MOTION_FREE;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;

	NxJoint* d6Joint = PhysX::pScene->createJoint(d6Desc);
	//

	//
	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3(-3,7.1,0);
	clothDesc.thickness = 0.2f;
	clothDesc.friction = 0.5f;
	clothDesc.flags |= NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	//clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;
	clothDesc.flags |= NX_CLF_TEARABLE;

	regularCloth = new cClothObject(PhysX::pScene, clothDesc, 8.0f, 7.0f, 0.2f, "");
	
	
	regularCloth->getNxCloth()->attachToShape(*box2->getShapes(), NX_CLOTH_INTERACTION_TWOWAY);
	//

	//
	iID = 3;
	CreateNewObject ( iID, "particle", 1 );

	sObject*				pObject			= NULL;
	DWORD					dwVertexCount	= regularCloth->mReceiveBuffers.maxVertices;
	DWORD					dwIndexCount	= regularCloth->mReceiveBuffers.maxIndices * 3;
	int						iOffset			= 0;
	int						iValue			= 0;
	int						iIndex			= 0;

	pObject = dbGetObject ( iID );

	if ( !pObject )
		return;

	SetupMeshFVFData ( pObject->pFrame->pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, dwIndexCount );

	pObject->bTransparentObject              = true;
	pObject->pFrame->pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pObject->pFrame->pMesh->iDrawVertexCount = ( int ) regularCloth->mReceiveBuffers.numVerticesPtr;
	pObject->pFrame->pMesh->iDrawPrimitives  = ( int ) regularCloth->mReceiveBuffers.numTrianglesPtr;
	pObject->pFrame->pMesh->bCull			 = false;

	CalculateMeshBounds			( pObject->pFrame->pMesh );
	SetNewObjectFinalProperties ( iID, 0.0f );
	SetTexture                  ( iID, 0 );
}

void dbPhyUpdateCloth ( void )
{
	sObject*				pObject			= NULL;
	DWORD					dwVertexCount	= regularCloth->mReceiveBuffers.maxVertices;
	DWORD					dwIndexCount	= regularCloth->mReceiveBuffers.maxIndices;
	int						iOffset			= 0;
	int						iValue			= 0;
	int						iIndex			= 0;

	pObject = dbGetObject ( 3 );

	pObject->pFrame->pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
	pObject->pFrame->pMesh->iDrawVertexCount = ( int ) *regularCloth->mReceiveBuffers.numVerticesPtr;
	pObject->pFrame->pMesh->iDrawPrimitives  = ( int ) *regularCloth->mReceiveBuffers.numTrianglesPtr;

	struct sClothVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float tu, tv;
	};

	sClothVertex* pVertex		= ( sClothVertex* ) pObject->pFrame->pMesh->pVertexData;
	iOffset		= 0;
	int iOffset1 = 0;

	for ( int z = 0; z < pObject->pFrame->pMesh->iDrawVertexCount; z++ )
	{
		float* pArray = ( float* ) regularCloth->mReceiveBuffers.verticesPosBegin;
		float* pArray1 = ( float* ) regularCloth->mReceiveBuffers.verticesNormalBegin ;

		pVertex [ z ].x   = pArray [ iOffset++ ];
        pVertex [ z ].y   = pArray [ iOffset++ ];
        pVertex [ z ].z   = pArray [ iOffset++ ];
		pVertex [ z ].nx  = pArray1 [ iOffset1++ ];
        pVertex [ z ].ny  = pArray1 [ iOffset1++ ];
        pVertex [ z ].nz  = pArray1 [ iOffset1++ ];
		pVertex [ z ].tu  = regularCloth->mTexCoords[2*z];
		pVertex [ z ].tv  = regularCloth->mTexCoords[2*z+1];
	}

	iOffset = 0;

	for ( int iIndex = 0; iIndex < ( int ) regularCloth->mReceiveBuffers.maxIndices; iIndex++ )
	{
		int* pArray = ( int* ) regularCloth->mReceiveBuffers.indicesBegin;

		pObject->pFrame->pMesh->pIndices [ iOffset ] = pArray [ iOffset++ ];
		pObject->pFrame->pMesh->pIndices [ iOffset ] = pArray [ iOffset++ ];
		pObject->pFrame->pMesh->pIndices [ iOffset ] = pArray [ iOffset++ ];
	}

	pObject->pFrame->pMesh->bVBRefreshRequired = true;
}
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////