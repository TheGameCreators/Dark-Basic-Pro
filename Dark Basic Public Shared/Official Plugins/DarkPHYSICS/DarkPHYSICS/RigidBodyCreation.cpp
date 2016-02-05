
#include "globals.h"
#include "rigidbodycreation.h"
#include "stream.h"
#include "material.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
` how to solve resetting on convex meshes
` if rotation is 90 degrees then force to 89.9
` if rotation is -90 degrees then force to -89.9
*/

/*
	PHY DELETE RIGID BODY%L%?dbPhyDeleteRigidBody@@YAXH@Z%

	PHY GET RIGID BODY EXIST[%LL%?dbPhyGetRigidBodyExist@@YAHH@Z%
	PHY GET RIGID BODY TYPE[%LL%?dbPhyGetRigidBodyType@@YAHH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include "rigidbodyspheres.h"
#include "rigidbodyboxes.h"
#include "rigidbodycapsules.h"
#include "rigidbodyspherejoints.h"
#include "rigidbodyfixedjoints.h"
#include "rigidbodyrevolutejoints.h"

void dbPhyDeleteRigidBody ( int iObject )
{
	// delete a single rigid body in a scene
	if ( PhysX::pScene == NULL )
		return;

	// go through all objects
	for ( DWORD dwObject = 0; dwObject < PhysX::pObjectList.size ( ); dwObject++ )
	{
		// see if we have a match
		if ( PhysX::pObjectList [ dwObject ]->iID == iObject )
		{
			// get a pointer to our object
			PhysX::sPhysXObject* pObject = PhysX::pObjectList [ dwObject ];

			// get object to real DBP object (if still exists)
			sObject* pDBPObject = PhysX::GetObject ( pObject->iID );

			// see if an actor exists
			if ( pObject->pActorList [ 0 ] )
			{
				// 051107 - give control of positioning back to db pro / gdk
				if ( pDBPObject ) pDBPObject->position.bCustomWorldMatrix = false;

				// release the actor
				PhysX::pScene->releaseActor ( *pObject->pActorList [ 0 ] );

				// erase the item in the list
				PhysX::pObjectList.erase ( PhysX::pObjectList.begin ( ) + dwObject );

				// clear up the memory
				delete pObject;
				pObject = NULL;
			}
			
			// break out
			break;
		}
	}
}

int dbPhyGetRigidBodyExist ( int iObject )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject ) )
	{
		return 1;
	}

	return 0;
}

int dbPhyGetRigidBodyType ( int iObject )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject ) )
	{
		PhysX::eType type = pObject->type;

		if ( type == PhysX::eRigidBodyBox )
			return 1;

		if ( type == PhysX::eRigidBodySphere )
			return 2;

		if ( type == PhysX::eRigidBodyCapsule )
			return 3;

		if ( type == PhysX::eRigidBodyMesh )
			return 4;
		
		if ( type == PhysX::eRigidBodyTerrain )
			return 5;
		
		if ( type == PhysX::eRigidBodyConvex )
			return 6;

		if ( type == PhysX::eRigidBodyPMAP )
			return 7;

		if ( type == PhysX::eRigidBodyRagdoll )
			return 8;

		/*
		eBoxTrigger,
		eSphereTrigger,
		eTypeVehicle,
		eBoxCharacterController,
		eCapsuleCharacterController
		*/

	}

	return 0;
}

NxQuat AnglesToQuat(const NxVec3& angles)
{
	NxQuat quat;
	NxVec3 a;
	NxReal cr, cp, cy, sr, sp, sy, cpcy, spsy;

	a.x = (NxPi/(NxReal)360.0) * angles.x;    // Pitch
	a.y = (NxPi/(NxReal)360.0) * angles.y;    // Yaw
	a.z = (NxPi/(NxReal)360.0) * angles.z;    // Roll

	cy = NxMath::cos(a.z);
	cp = NxMath::cos(a.y);
	cr = NxMath::cos(a.x);

	sy = NxMath::sin(a.z);
	sp = NxMath::sin(a.y);
	sr = NxMath::sin(a.x);

	cpcy = cp * cy;
	spsy = sp * sy;
	quat.w = cr * cpcy + sr * spsy;		
	quat.x = sr * cpcy - cr * spsy;		
	quat.y = cr * sp * cy + sr * cp * sy;	
	quat.z = cr * cp * sy - sr * sp * cy;

	return quat;
}

extern bool g_bIgnoreErrors;

void dbPhyMakeRigidBody ( int iObject, PhysX::eType eType, PhysX::eState eState, int iMaterial, int iSaveMode, char* szFile )
{

	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Simulation has not been set up, unable to create rigid body", "", 0 );
		return;
	}

	sObject* pObject = PhysX::GetObject ( iObject );

	if ( !pObject )
		return;

	PhysX::sPhysXObject*	pPhysXObject = new PhysX::sPhysXObject;
	NxActorDesc				actorDesc;
	NxBodyDesc				bodyDesc;
	NxActor*				pActor = NULL;

	if ( !pPhysXObject )
		return;

	pPhysXObject->iID     = iObject;

	switch ( eType )
	{
		case PhysX::eRigidBodyBox:     dbPhyMakeRigidBodyBox     ( &actorDesc, pObject, dbPhyGetMaterial ( iMaterial, true ) ); 									break;
		case PhysX::eRigidBodySphere:  dbPhyMakeRigidBodySphere  ( &actorDesc, pObject, dbPhyGetMaterial ( iMaterial, true ) ); 									break;
		case PhysX::eRigidBodyCapsule: dbPhyMakeRigidBodyCapsule ( &actorDesc, pObject, dbPhyGetMaterial ( iMaterial, true ) );										break;
		case PhysX::eRigidBodyMesh:    dbPhyMakeRigidBodyMesh    ( &actorDesc, pObject, dbPhyGetMaterial ( iMaterial, true ), iSaveMode, szFile, pPhysXObject );	break;
		case PhysX::eRigidBodyTerrain: dbPhyMakeRigidBodyTerrain ( &actorDesc, pObject, dbPhyGetMaterial ( iMaterial, true ), iSaveMode, szFile );					break;
		case PhysX::eRigidBodyConvex:  dbPhyMakeRigidBodyConvex  ( &actorDesc, pObject, dbPhyGetMaterial ( iMaterial, true ), iSaveMode, szFile );					break;
		case PhysX::eRigidBodyPMAP:    dbPhyMakeRigidBodyPMAP    ( &actorDesc, pObject, dbPhyGetMaterial ( iMaterial, true ), iSaveMode, szFile );					break;
	};

	/*
	if ( iObject == 1 )
		bodyDesc.mass = 1.0f;
	else
		bodyDesc.mass = 2.0f;
		*/


	// when dynamic give the actor the body
	if ( eState == PhysX::eDynamic )
	{
		actorDesc.body = &bodyDesc;

		//actorDesc.group = 1;

		pPhysXObject->state = PhysX::eDynamic;
	}
	else
	{
		pPhysXObject->state = PhysX::eStatic;

		//actorDesc.group = 1;
	}

	// set density and position
	actorDesc.density      = 10;
	//actorDesc.density      = 0.01f;

	//if ( eType != PhysX::eRigidBodyPMAP )
		actorDesc.globalPose.t = NxVec3 ( pObject->position.vecPosition.x, pObject->position.vecPosition.y, pObject->position.vecPosition.z );

	
	// create the actor
	if ( pPhysXObject->pActorList.size ( ) == 0 )
	{
		

		pActor = PhysX::pScene->createActor ( actorDesc );

		// check actor is valid
		if ( !pActor )
			return;

		g_bIgnoreErrors = true;
		pActor->setGlobalOrientationQuat(AnglesToQuat(NxVec3(pObject->position.vecRotate.x,pObject->position.vecRotate.y,pObject->position.vecRotate.z)));
		g_bIgnoreErrors = false;

		pPhysXObject->pActorList.push_back ( pActor );

		//pPhysXObject->pActor = pActor;

		pActor->userData      = ( void* ) pPhysXObject;
	}

	// set up custom data, store object ID and actor pointer
	pPhysXObject->pScene  = PhysX::pScene;
	pPhysXObject->pObject = pObject;
	pPhysXObject->iID     = iObject;
	
	pPhysXObject->type = eType;

	// send object into object list
	PhysX::pObjectList.push_back ( pPhysXObject );
}

float dbPhyObjectGetSizeX ( sObject* pObject )
{
	return ( pObject->collision.vecMax.x - pObject->collision.vecMin.x ) * pObject->position.vecScale.x;
}

float dbPhyObjectGetSizeY ( sObject* pObject )
{
	return ( pObject->collision.vecMax.y - pObject->collision.vecMin.y ) * pObject->position.vecScale.y;
}

float dbPhyObjectGetSizeZ ( sObject* pObject )
{
	return ( pObject->collision.vecMax.z - pObject->collision.vecMin.z ) * pObject->position.vecScale.z;
}

PhysX::sPhysXObject* dbPhyGetObject ( int iID, bool bDisplayError )
{
	for ( DWORD dwObject = 0; dwObject < PhysX::pObjectList.size ( ); dwObject++ )
	{
		if ( PhysX::pObjectList [ dwObject ]->iID == iID )
		{
			return PhysX::pObjectList [ dwObject ];
		}
	}

	// display an error if needed
	if ( bDisplayError )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Rigid body with specified ID does not exist", "", 0 );
		return NULL;
	}

	return NULL;
}