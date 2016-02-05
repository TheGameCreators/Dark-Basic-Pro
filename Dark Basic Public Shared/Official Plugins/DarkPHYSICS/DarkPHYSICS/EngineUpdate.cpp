     
#include "globals.h"
#include "engineupdate.h"
#include "error.h"
#include "cRuntimeErrors.h"
#include <windows.h>

#ifdef DARKSDK_COMPILE
	#include "DarkSDK.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY UPDATE%0%?dbPhyUpdate@@YAXXZ%
	PHY UPDATE%L%?dbPhyUpdate@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyUpdateSoftBody ( void );

void dbPhyUpdateFetch ( void )
{
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "No scene available, cannot update the simulation", "", 0 );
		return;
	}

	// store original scene
	NxScene* pOriginalScene = PhysX::pScene;

	// run through all scenes
	for ( int i = 0; i < ( int ) PhysX::pSceneList.size ( ); i++ )
	{
		// check scene is valid
		if ( !PhysX::pSceneList [ i ]->pScene )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid scene, failed to update the simulation", "", 0 );
			return;
		}

		// store scene pointer
		PhysX::pScene = PhysX::pSceneList [ i ]->pScene;

		// the physics thread has been running since we left this function last time
		PhysX::pScene->fetchResults ( NX_RIGID_BODY_FINISHED, true );

		// check scene is writable
		assert ( PhysX::pScene->isWritable ( ) );

		// actor count and list from results
		int       iActorCount = PhysX::pScene->getNbActors ( );
		NxActor** ppActorList = PhysX::pScene->getActors   ( );

		// loop round until no actors left
		while ( iActorCount-- )
		{
			// get pointer to actor, set shape pointer to null, get the
			// global pose of the actor and then get orientation and position
			NxActor* pActor    = *ppActorList++;
			NxShape* pShape    = NULL;
			NxMat34  matPose   = pActor->getGlobalPose ( );
			NxMat33  matOrient = matPose.M;
			NxVec3   vecPos    = matPose.t;	

			// get a gl matrix from the orientation matrix
			float glmat [ 16 ];
			matOrient.getColumnMajorStride4 ( &( glmat [ 0 ] ) );
			vecPos.get ( &( glmat [ 12 ] ) );

			// wipe parts of the matrix we don't need
			glmat [  3 ] = 0.0f;
			glmat [  7 ] = 0.0f;
			glmat [ 11 ] = 0.0f;
			glmat [ 15 ] = 1.0f;

			// get custom data from the actor
			PhysX::sPhysXObject* pPhysXObject = ( PhysX::sPhysXObject* ) pActor->userData;
			
			// if we have some custom data then we can update the object
			if ( pPhysXObject )
			{
				// skip certain objects
				if ( pPhysXObject->type == PhysX::eBoxCharacterController || pPhysXObject->type == PhysX::eCapsuleCharacterController )
					continue;

				if ( pPhysXObject->type != PhysX::eBoxTrigger && pPhysXObject->type != PhysX::eSphereTrigger )
				{
					NxVec3 xyz;
					xyz = dbPhyEulerFromMat44 ( &( glmat [ 0 ] ) );
					sObject* pObject = PhysX::GetObject ( pPhysXObject->iID );

					if ( !pObject )
						continue;
					
					D3DXMATRIX matScale;
					D3DXVECTOR3 vecScale = pObject->position.vecScale;
					D3DXMatrixScaling ( &matScale, vecScale.x, vecScale.y, vecScale.z );

					D3DXMATRIX matOrigAbsWorldFromPhysics = * ( D3DXMATRIX* ) &glmat;
					D3DXMATRIX matAbsWorldFromPhysics = * ( D3DXMATRIX* ) &glmat;

					matScale *= matAbsWorldFromPhysics;
					
					//pObject->collision.fScaledLargestRadius = 0.0f;
					matScale._41 = matOrigAbsWorldFromPhysics._41;
					matScale._42 = matOrigAbsWorldFromPhysics._42;
					matScale._43 = matOrigAbsWorldFromPhysics._43;

					// mike - 070806 - if we're dealing with a static mesh take position from object
					if ( pPhysXObject->state == PhysX::eStatic && pPhysXObject->type == PhysX::eRigidBodyMesh )
					{
						continue;

						matScale._41 = pObject->position.vecPosition.x;
						matScale._42 = pObject->position.vecPosition.y;
						matScale._43 = pObject->position.vecPosition.z;

						matAbsWorldFromPhysics._41 = matScale._41;
						matAbsWorldFromPhysics._42 = matScale._42;
						matAbsWorldFromPhysics._43 = matScale._43;
					}
					
					memcpy ( &pObject->position.matWorld, &matScale, sizeof ( D3DXMATRIX ) );
					pObject->position.bCustomWorldMatrix = true;

					PhysX::PositionObject ( pPhysXObject->iID, matAbsWorldFromPhysics._41, matAbsWorldFromPhysics._42, matAbsWorldFromPhysics._43 );
					PhysX::RotateObject ( pPhysXObject->iID, D3DXToDegree ( xyz.x ), D3DXToDegree ( xyz.y ), D3DXToDegree ( xyz.z ) );
				}
			}
		}

		NxReal maxTimestep;
		NxTimeStepMethod method;
		NxU32 maxIter;
		NxU32 numSubSteps;

		PhysX::pScene->getTiming ( maxTimestep, maxIter, method, &numSubSteps );

		if ( numSubSteps )
			PhysX::CharacterController.updateControllers ( );
	}

	dbPhyUpdateParticles ( );
	dbPhyUpdateCloth     ( );
	dbPhyUpdateVehicle   ( );
	dbPhyUpdateFluid     ( ); 
	dbPhyUpdateRagdoll	 ( );
	dbPhyUpdateSoftBody  ( );

	PhysX::pScene = pOriginalScene;
}

void dbPhyUpdateSimulate ( void )
{
	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid scene, failed to update the simulation", "", 0 );
		return;
	}

	NxScene* pOriginalScene = PhysX::pScene;

	for ( int i = 0; i < ( int ) PhysX::pSceneList.size ( ); i++ )
	{
		PhysX::pScene = PhysX::pSceneList [ i ]->pScene;

		if ( !PhysX::pScene )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid scene, failed to update the simulation", "", 0 );
			return;
		}

		// set up our local variables, number of actors, list of actors
		// and the time elapsed between the last call and this call
		NxReal    fElapsedTime = ( NxReal ) dbPhyUpdateTime ( );

		if ( PhysX::iTimingMethod == 0 )
			fElapsedTime = PhysX::fFixedTime;

		if ( PhysX::iTimingMethod == 2 )
		{
			float fTime = ( float ) PhysX::GetDisplayFPS ( );

			if ( fTime < 1.0f )
				fTime = 60.0f;

			float fTimeStep = 1.0f / fTime;

			PhysX::pScene->setTiming ( fTimeStep, 8, NX_TIMESTEP_FIXED );
			fElapsedTime = fTimeStep;
		}

		
		// simulate the scene and flush the stream
		PhysX::pScene->simulate    ( fElapsedTime );
		PhysX::pScene->flushStream ( );
	}

	PhysX::pScene = pOriginalScene;
}

void dbPhyUpdate ( void )
{
	dbPhyUpdateSimulate ( );
	dbPhyUpdateFetch    ( );
}

void dbPhyUpdate ( int iUserControlledUpdate )
{
	switch ( iUserControlledUpdate )
	{
		case 0 : dbPhyUpdateSimulate ( ); break;
		case 1 : dbPhyUpdateFetch    ( ); break;
	}
}

float dbPhyUpdateTime ( void )
{
    float deltaTime;
	static __int64 Time, LastTime;
    __int64 freq;

    QueryPerformanceCounter   ( ( LARGE_INTEGER* ) &Time );
    QueryPerformanceFrequency ( ( LARGE_INTEGER* ) &freq );

    deltaTime = ( float ) ( Time - LastTime ) / ( float ) freq;
    LastTime  = Time;

    return deltaTime;
}

NxVec3 dbPhyEulerFromMat44 ( float *m )
{
	float heading = (float)atan2( m[1], m[0] );
	float attitude = (float)atan2( m[6], m[10] );
	float bank = (float)asin( -m[2] );
	NxVec3 angs;
	if (fabs( m[2]) > 1 ) {
		angs.x = 0.0f;
		angs.y = 3.1415926543f * m[2];
		angs.z = 0.0f;
	} else {
		angs.x = attitude;
		angs.y = bank;
		angs.z = heading;
	}
	return angs;
}
