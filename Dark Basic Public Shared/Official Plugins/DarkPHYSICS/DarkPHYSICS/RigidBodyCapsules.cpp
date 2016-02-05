
#include "globals.h"
#include "rigidbodycreation.h"
#include "stream.h"
#include "material.h"
#include "rigidbodycapsules.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


/*
	PHY MAKE RIGID BODY STATIC CAPSULE%L%?dbPhyMakeRigidBodyStaticCapsule@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC CAPSULES%L%?dbPhyMakeRigidBodyStaticCapsules@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC CAPSULE%LL%?dbPhyMakeRigidBodyStaticCapsule@@YAXHH@Z%
	PHY MAKE RIGID BODY STATIC CAPSULES%LL%?dbPhyMakeRigidBodyStaticCapsules@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC CAPSULE%L%?dbPhyMakeRigidBodyDynamicCapsule@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC CAPSULES%L%?dbPhyMakeRigidBodyDynamicCapsules@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC CAPSULE%LL%?dbPhyMakeRigidBodyDynamicCapsule@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC CAPSULES%LL%?dbPhyMakeRigidBodyDynamicCapsules@@YAXHH@Z%

	void dbPhyMakeRigidBodyStaticCapsule	( int iObject );
	void dbPhyMakeRigidBodyStaticCapsules	( int iObject );
	void dbPhyMakeRigidBodyStaticCapsule	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyStaticCapsules	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyDynamicCapsule	( int iObject );
	void dbPhyMakeRigidBodyDynamicCapsules	( int iObject );
	void dbPhyMakeRigidBodyDynamicCapsule	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyDynamicCapsules	( int iObject, int iMaterial );
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeRigidBodyStaticCapsule ( int iObject )
{
	dbPhyMakeRigidBodyStaticCapsule ( iObject, -1 );
}

void dbPhyMakeRigidBodyStaticCapsules ( int iObject )
{

}

void dbPhyMakeRigidBodyStaticCapsule ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyCapsule, PhysX::eStatic, iMaterial );
}

void dbPhyMakeRigidBodyStaticCapsules ( int iObject, int iMaterial )
{

}

void dbPhyMakeRigidBodyDynamicCapsule ( int iObject )
{
	dbPhyMakeRigidBodyDynamicCapsule ( iObject, -1 );
}

void dbPhyMakeRigidBodyDynamicCapsules ( int iObject )
{

}

void dbPhyMakeRigidBodyDynamicCapsule ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodyCapsule, PhysX::eDynamic, iMaterial );
}

void dbPhyMakeRigidBodyDynamicCapsules ( int iObject, int iMaterial )
{

}

void dbPhyMakeRigidBodyCapsule ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial )
{
	NxCapsuleShapeDesc*	pCapsuleDesc = new NxCapsuleShapeDesc;

	PhysX::CalculateMeshBounds ( pObject->ppMeshList [ 0 ] );

	pCapsuleDesc->height      = ( ( pObject->collision.vecMax.y * pObject->position.vecScale.y ) - ( pObject->collision.vecMin.y * pObject->position.vecScale.y ) );
	pCapsuleDesc->radius      = ( ( pObject->collision.vecMax.x * pObject->position.vecScale.x ) - ( pObject->collision.vecMin.x * pObject->position.vecScale.x ) ) / 2;
	pCapsuleDesc->localPose.t = NxVec3 ( 0, 0, 0 );

	if ( pMaterial )
		pCapsuleDesc->materialIndex = pMaterial->matIndex;
	
	pActorDesc->shapes.pushBack ( pCapsuleDesc );
}
