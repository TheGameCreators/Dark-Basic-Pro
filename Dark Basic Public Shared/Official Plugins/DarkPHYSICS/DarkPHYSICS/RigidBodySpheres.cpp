
#include "globals.h"
#include "rigidbodycreation.h"
#include "stream.h"
#include "material.h"
#include "rigidbodyspheres.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE RIGID BODY STATIC SPHERE%L%?dbPhyMakeRigidBodyStaticSphere@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC SPHERES%L%?dbPhyMakeRigidBodyStaticSpheres@@YAXH@Z%
	PHY MAKE RIGID BODY STATIC SPHERE%LL%?dbPhyMakeRigidBodyStaticSphere@@YAXHH@Z%
	PHY MAKE RIGID BODY STATIC SPHERES%LL%?dbPhyMakeRigidBodyStaticSpheres@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC SPHERE%L%?dbPhyMakeRigidBodyDynamicSphere@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC SPHERES%L%?dbPhyMakeRigidBodyDynamicSpheres@@YAXH@Z%
	PHY MAKE RIGID BODY DYNAMIC SPHERE%LL%?dbPhyMakeRigidBodyDynamicSphere@@YAXHH@Z%
	PHY MAKE RIGID BODY DYNAMIC SPHERES%LL%?dbPhyMakeRigidBodyDynamicSpheres@@YAXHH@Z%

	void dbPhyMakeRigidBodyStaticSphere		( int iObject );
	void dbPhyMakeRigidBodyStaticSpheres	( int iObject );
	void dbPhyMakeRigidBodyStaticSphere		( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyStaticSpheres	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyDynamicSphere	( int iObject );
	void dbPhyMakeRigidBodyDynamicSpheres	( int iObject );
	void dbPhyMakeRigidBodyDynamicSphere	( int iObject, int iMaterial );
	void dbPhyMakeRigidBodyDynamicSpheres	( int iObject, int iMaterial );
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeRigidBodyStaticSphere	( int iObject )
{
	dbPhyMakeRigidBodyStaticSphere ( iObject, -1 );
}

void dbPhyMakeRigidBodyStaticSpheres ( int iObject )
{

}

void dbPhyMakeRigidBodyStaticSphere	( int iObject, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodySphere, PhysX::eStatic, iMaterial );
}

void dbPhyMakeRigidBodyStaticSpheres ( int iObject, int iMaterial )
{

}

void dbPhyMakeRigidBodyDynamicSphere ( int iObject )
{
	dbPhyMakeRigidBodyDynamicSphere ( iObject, -1 );
}

void dbPhyMakeRigidBodyDynamicSpheres ( int iObject )
{

}

void dbPhyMakeRigidBodyDynamicSphere ( int iObject, int iMaterial )
{
	dbPhyMakeRigidBody ( iObject, PhysX::eRigidBodySphere, PhysX::eDynamic, iMaterial );
}

void dbPhyMakeRigidBodyDynamicSpheres ( int iObject, int iMaterial )
{

}

void dbPhyMakeRigidBodySphere ( NxActorDesc* pActorDesc, sObject* pObject, PhysX::sPhysXMaterial* pMaterial )
{
	NxSphereShapeDesc* pSphereDesc = new NxSphereShapeDesc;
	
	pSphereDesc->radius      = pObject->collision.fScaledRadius;
	pSphereDesc->localPose.t = NxVec3 ( 0, 0, 0 );

	if ( pMaterial )
		pSphereDesc->materialIndex = pMaterial->matIndex;

	pActorDesc->shapes.pushBack ( pSphereDesc );
}
