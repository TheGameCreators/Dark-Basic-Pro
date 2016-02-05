
#include "globals.h"
#include "rigidbodycreation.h"
#include "stream.h"
#include "material.h"
#include "rigidbodyboxes.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE RIGID BODY STATIC BOX%L%?dbPhyMakeRigidBodyStaticBox@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC BOXES%L%?dbPhyMakeRigidBodyStaticBoxes@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC BOX%LL%?dbPhyMakeRigidBodyStaticBox@@YAXHH@Z%
	PHY MAKE RIGID BODY STATIC BOXES%LL%?dbPhyMakeRigidBodyStaticBoxes@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC BOX%L%?dbPhyMakeRigidBodyDynamicBox@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC BOXES%L%?dbPhyMakeRigidBodyDynamicBoxes@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC BOX%LL%?dbPhyMakeRigidBodyDynamicBox@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC BOXES%LL%?dbPhyMakeRigidBodyDynamicBoxes@@YAXHH@Z%

	void dbPhyMakeRigidBodyStaticBox	( int iObject );
	void dbPhyMakeRigidBodyStaticBoxes	( int iObject );
	void dbPhyMakeRigidBodyStaticBox	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyStaticBoxes	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyDynamicBox	( int iObject );
	void dbPhyMakeRigidBodyDynamicBoxes	( int iObject );
	void dbPhyMakeRigidBodyDynamicBox	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyDynamicBoxes	( int iObject, int iMaterial );
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void dbPhyMakeRigidBodyStaticBox ( int iObject )
{
	dbPhyMakeRigidBodyStaticBox ( iObject, -1 );
}

void dbPhyMakeRigidBodyStaticBoxes ( int iObject )
{

}

void dbPhyMakeRigidBodyStaticBox ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyBox, PhysX::eStatic, iMaterial );
}

void dbPhyMakeRigidBodyStaticBoxes ( int iObject, int iMaterial )
{

}

void dbPhyMakeRigidBodyDynamicBox ( int iObject )
{
	dbPhyMakeRigidBodyDynamicBox ( iObject, -1 );
}

void dbPhyMakeRigidBodyDynamicBoxes ( int iObject )
{
	dbPhyMakeRigidBodyDynamicBoxes ( iObject, -1 );
}

void dbPhyMakeRigidBodyDynamicBox ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyBox, PhysX::eDynamic, iMaterial );
}

void dbPhyMakeRigidBodyDynamicCCDBox ( int iObject )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyBox, PhysX::eDynamic, -1, true );
}

void dbPhyMakeRigidBodyDynamicCCDBox ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyBox, PhysX::eDynamic, iMaterial, true );
}

void dbPhyMakeRigidBodyDynamicBoxes ( int iObject, int iMaterial )
{
	sObject* pObject = PhysX::GetObject ( iObject );

	if ( !pObject )
		return;

	PhysX::sPhysXObject*	pPhysXObject = new PhysX::sPhysXObject;
	NxActor*				pActor;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxBoxShapeDesc box [ 100 ];

	/////
	for ( int i = 0; i < pObject->iFrameCount; i++ )
	{
		sFrame* pFrame  = pObject->ppFrameList [ i ];

		if ( !pFrame->pMesh )
			continue;
		
		sMesh* pMesh = pObject->ppFrameList [ i ]->pMesh;

		int iLimb = i;

		DWORD dwLimbPositionX  = PhysX::GetLimbPositionX  ( iObject, iLimb );
		DWORD dwLimbPositionY  = PhysX::GetLimbPositionY  ( iObject, iLimb );
		DWORD dwLimbPositionZ  = PhysX::GetLimbPositionZ  ( iObject, iLimb );
		DWORD dwLimbDirectionX = PhysX::GetLimbDirectionX ( iObject, iLimb );
		DWORD dwLimbDirectionY = PhysX::GetLimbDirectionY ( iObject, iLimb );
		DWORD dwLimbDirectionZ = PhysX::GetLimbDirectionZ ( iObject, iLimb );

		float fLimbPositionX   = *( float* ) &dwLimbPositionX;
		float fLimbPositionY   = *( float* ) &dwLimbPositionY;
		float fLimbPositionZ   = *( float* ) &dwLimbPositionZ;
		float fLimbDirectionX  = *( float* ) &dwLimbDirectionX;
		float fLimbDirectionY  = *( float* ) &dwLimbDirectionY;
		float fLimbDirectionZ  = *( float* ) &dwLimbDirectionZ;

		D3DXVECTOR3 vecMin = pMesh->Collision.vecMin;
		D3DXVECTOR3 vecMax = pMesh->Collision.vecMax;

		box [ i ].dimensions.set ( ( vecMax.x - vecMin.x ) / 2, ( vecMax.y - vecMin.y ) / 2, ( vecMax.z - vecMin.z ) / 2 );

		float fCentreX = pMesh->Collision.vecCentre.x * pObject->position.vecScale.x;
		float fCentreY = pMesh->Collision.vecCentre.y * pObject->position.vecScale.y;
		float fCentreZ = pMesh->Collision.vecCentre.z * pObject->position.vecScale.z;

		box [ i ].localPose.t = NxVec3 ( fLimbPositionX, fLimbPositionY, fLimbPositionZ );

		actorDesc.shapes.pushBack ( &box [ i ] );
	}
	/////

	// set density and position
	actorDesc.density      = 10;
	actorDesc.globalPose.t = NxVec3 ( pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );
			actorDesc.body = &bodyDesc;
		pPhysXObject->state = PhysX::eDynamic;

	pActor = PhysX::pScene->createActor ( actorDesc );

	pPhysXObject->pObject = pObject;
	pPhysXObject->iID     = iObject;
	pPhysXObject->pActorList.push_back ( pActor );
	pActor->userData      = ( void* ) pPhysXObject;

	PhysX::pObjectList.push_back ( pPhysXObject );
}

void dbPhyMakeRigidBodyBox ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial, bool bCCD )
{
	NxBoxShapeDesc*	pBoxDesc = new NxBoxShapeDesc;
	
	pBoxDesc->dimensions.set ( dbPhyObjectGetSizeX ( pObject ) / 2, dbPhyObjectGetSizeY ( pObject ) / 2, dbPhyObjectGetSizeZ ( pObject ) / 2 );

	float fCentreX = pObject->collision.vecCentre.x * pObject->position.vecScale.x;
	float fCentreY = pObject->collision.vecCentre.y * pObject->position.vecScale.y;
	float fCentreZ = pObject->collision.vecCentre.z * pObject->position.vecScale.z;

	pBoxDesc->localPose.t = NxVec3 ( fCentreX, fCentreY, fCentreZ );

	if ( pMaterial )
		pBoxDesc->materialIndex = pMaterial->matIndex;

	pActorDesc->shapes.pushBack ( pBoxDesc );
}
