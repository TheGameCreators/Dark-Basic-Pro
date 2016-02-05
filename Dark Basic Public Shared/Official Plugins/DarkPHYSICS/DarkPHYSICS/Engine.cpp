     
#include "globals.h"
#include "engine.h"
#include "error.h"
#include <windows.h>

#ifdef DARKSDK_COMPILE
	#include "DarkSDK.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY START%0%?dbPhyStart@@YAXXZ%
	PHY UPDATE%0%?dbPhyUpdate@@YAXXZ%
	PHY END%0%?dbPhyEnd@@YAXXZ%
	PHY CLEAR%0%?dbPhyClear@@YAXXZ%

	PHY SET GRAVITY%FFF%?dbPhySetGravity@@YAXMMM@Z%
	PHY SET TIMING%FLL%dbPhySetTiming@@YAXMHH@Z%

	PHY SET SKIN WIDTH%F%?dbPhySetSkinWidth@@YAXM@Z%
	PHY SET DEFAULT SLEEP LINEAR VELOCITY SQUARED%F%?dbPhySetDefaultSleepLinearVelocitySquared@@YAXM@Z%
	PHY SET DEFAULT SLEEP ANGULAR VELOCITY SQUARED%F%?dbPhySetDefaultSleepAngularVelocitySquared@@YAXM@Z%
	PHY SET BOUNCE THRESHOLD%F%?dbPhySetBounceThreshold@@YAXM@Z%
	PHY SET DYNAMIC FRICTION%F%?dbPhySetDynamicFriction@@YAXM@Z%
	PHY SET STATIC FRICTION%F%?dbPhySetStaticFriction@@YAXM@Z%

	PHY SET MAX ANGULAR VELOCITY%F%?dbPhySetMaxAngularVelocity@@YAXM@Z%
	PHY SET CONTINUOUS CD%F%?dbPhySetContinousCD@@YAXM@Z%
	PHY SET ADAPTIVE FORCE%F%?dbPhySetAdaptiveForce@@YAXM@Z%
	PHY SET CONTROLLED FILTERING%F%?dbPhySetControlledFiltering@@YAXM@Z%
	PHY SET TRIGGER CALLBACK%F%?dbPhySetTriggerCallback@@YAXM@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyStart ( void )
{
	//return;

	//MessageBox ( NULL, "bent", "info", MB_OK );

	// set up function pointers
	#ifdef DARKSDK_COMPILE
		PhysX::GetObject					= dbGetObject;
		PhysX::PositionObject				= dbPositionObject;
		PhysX::RotateObject					= dbRotateObject;
		PhysX::GetFVFOffsetMap				= GetFVFOffsetMap;
		PhysX::CreateNewObject				= CreateNewObject;
		PhysX::SetupMeshFVFData				= SetupMeshFVFData;
		PhysX::CalculateMeshBounds			= CalculateMeshBounds;
		PhysX::SetNewObjectFinalProperties	= SetNewObjectFinalProperties;
		PhysX::SetTexture					= SetTexture;
		PhysX::SetObjectTransparency		= dbSetObjectTransparency;
		PhysX::DisableObjectZWrite			= dbDisableObjectZWrite;
		PhysX::SetObjectLight				= ( PhysX::SetObjectLightPFN ) dbSetObjectLight;
		PhysX::GhostObjectOn				= dbGhostObjectOn;
		PhysX::GhostObjectOn1				= dbGhostObjectOn;
		PhysX::GetCameraLook				= dbGetObjectCameraLook;
		PhysX::GetCameraPosition			= dbGetObjectCameraPosition;
		PhysX::GetCameraUp					= dbGetObjectCameraUp;
		PhysX::GetCameraRight				= dbGetObjectCameraRight;
		PhysX::RotateLimb					= dbRotateLimb;
		PhysX::OffsetLimb					= dbOffsetLimb;
		PhysX::GetLimbAngleX				= ( PhysX::GetLimbAnglePFN ) dbLimbAngleX;
		PhysX::GetLimbAngleY				= ( PhysX::GetLimbAnglePFN ) dbLimbAngleY;
		PhysX::GetLimbAngleZ				= ( PhysX::GetLimbAnglePFN ) dbLimbAngleZ;
		PhysX::GetLimbOffsetX				= ( PhysX::GetLimbOffsetPFN ) dbLimbOffsetX;
		PhysX::GetLimbOffsetY				= ( PhysX::GetLimbOffsetPFN ) dbLimbOffsetY;
		PhysX::GetLimbOffsetZ				= ( PhysX::GetLimbOffsetPFN ) dbLimbOffsetZ;
	#else
		PhysX::GetObject					= ( PhysX::GetObjectPFN						) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z" );
		PhysX::PositionObject				= ( PhysX::PositionObjectPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?Position@@YAXHMMM@Z" );
		PhysX::RotateObject					= ( PhysX::RotateObjectPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?Rotate@@YAXHMMM@Z" );
		PhysX::GetFVFOffsetMap				= ( PhysX::GetFVFOffsetMapPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetFVFOffsetMap@@YA_NPAUsMesh@@PAUsOffsetMap@@@Z" );
		PhysX::CreateNewObject				= ( PhysX::CreateNewObjectPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?CreateNewObject@@YA_NHPADH@Z" );
		PhysX::SetupMeshFVFData				= ( PhysX::SetupMeshFVFDataPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?SetupMeshFVFData@@YA_NPAUsMesh@@KKK@Z" );
		PhysX::CalculateMeshBounds			= ( PhysX::CalculateMeshBoundsPFN			) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?CalculateMeshBounds@@YA_NPAUsMesh@@@Z" );
		PhysX::SetNewObjectFinalProperties	= ( PhysX::SetNewObjectFinalPropertiesPFN	) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?SetNewObjectFinalProperties@@YA_NHM@Z" );
		PhysX::SetTexture					= ( PhysX::SetTexturePFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?SetTexture@@YAXHH@Z" );
		PhysX::SetObjectTransparency		= ( PhysX::SetObjectTransparencyPFN			) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?SetTransparency@@YAXHH@Z" );
		PhysX::DisableObjectZWrite			= ( PhysX::DisableObjectZWritePFN			) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?DisableZWrite@@YAXH@Z" );
		PhysX::SetObjectLight				= ( PhysX::SetObjectLightPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?SetLight@@YAXHH@Z" );
		PhysX::GhostObjectOn				= ( PhysX::GhostObjectOnPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GhostOn@@YAXH@Z" );
		PhysX::GhostObjectOn1				= ( PhysX::GhostObjectOn1PFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GhostOn@@YAXHH@Z" );
		PhysX::GetCameraLook				= ( PhysX::GetCameraLookPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetCameraLook@@YA?AUD3DXVECTOR3@@XZ" );
		PhysX::GetCameraPosition			= ( PhysX::GetCameraPositionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetCameraPosition@@YA?AUD3DXVECTOR3@@XZ" );
		PhysX::GetCameraUp					= ( PhysX::GetCameraUpPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetCameraUp@@YA?AUD3DXVECTOR3@@XZ" );
		PhysX::GetCameraRight				= ( PhysX::GetCameraRightPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetCameraRight@@YA?AUD3DXVECTOR3@@XZ" );
		PhysX::RotateLimb					= ( PhysX::RotateLimbPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?RotateLimb@@YAXHHMMM@Z" );
		PhysX::OffsetLimb					= ( PhysX::OffsetLimbPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?OffsetLimb@@YAXHHMMM@Z" );
		PhysX::GetLimbAngleX				= ( PhysX::GetLimbAnglePFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbAngleX@@YAKHH@Z" );
		PhysX::GetLimbAngleY				= ( PhysX::GetLimbAnglePFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbAngleY@@YAKHH@Z" );
		PhysX::GetLimbAngleZ				= ( PhysX::GetLimbAnglePFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbAngleZ@@YAKHH@Z" );
		PhysX::GetLimbOffsetX				= ( PhysX::GetLimbOffsetPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbOffsetX@@YAKHH@Z" );
		PhysX::GetLimbOffsetY				= ( PhysX::GetLimbOffsetPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbOffsetY@@YAKHH@Z" );
		PhysX::GetLimbOffsetZ				= ( PhysX::GetLimbOffsetPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbOffsetZ@@YAKHH@Z" );
		PhysX::GetKeyState					= ( PhysX::KeyStatePFN						) GetProcAddress ( PhysX::pGlobStruct->g_Input,   "?KeyState@@YAHH@Z" );
	#endif

	// start the physics engine

	//MessageBox ( NULL, "allocator", "info", MB_OK );

	PhysX::pAllocator = new UserAllocator;

	// create the engine
	//PhysX::pPhysicsSDK = NxCreatePhysicsSDK ( NX_PHYSICS_SDK_VERSION, PhysX::pAllocator, &g_ErrorStream );
	PhysX::pPhysicsSDK = NxCreatePhysicsSDK ( NX_PHYSICS_SDK_VERSION, PhysX::pAllocator, &PhysX::m_ErrorStream );
	//PhysX::pPhysicsSDK = NxCreatePhysicsSDK ( NX_PHYSICS_SDK_VERSION, PhysX::pAllocator );
		//PhysX::pPhysicsSDK = NxCreatePhysicsSDK ( NX_PHYSICS_SDK_VERSION );

	// check the engine is valid
	if ( !PhysX::pPhysicsSDK )
	{
		MessageBox ( NULL, "engine is invalid", "info", MB_OK );

		// error
		return;
	}

	// now set properties for the scene
	NxSceneDesc	sceneDesc;
	sceneDesc.gravity               = PhysX::vecGravity;
 //   sceneDesc.broadPhase            = NX_BROADPHASE_COHERENT;
 //   sceneDesc.collisionDetection    = true;

	//sceneDesc.simType                = NX_SIMULATION_HW;
	//sceneDesc.hwSceneType            = NX_HW_SCENE_TYPE_FLUID;
	//sceneDesc.hwSceneType            = NX_HW_SCENE_TYPE_FLUID_SOFTWARE;

	//sceneDesc.simType				= NX_SIMULATION_HW;
	//sceneDesc.hwSceneType			= NX_HW_SCENE_TYPE_RB;
	//sceneDesc.hwSceneType			= NX_HW_SCENE_TYPE_FLUID;

	// hardware
	//sceneDesc.simType				= NX_SIMULATION_HW;
	//sceneDesc.simType				= NX_SIMULATION_SW;
	//sceneDesc.hwSceneType            = NX_HW_SCENE_TYPE_RB;
	//sceneDesc.flags         |= NX_SF_SIMULATE_SEPARATE_THREAD;
	
	// create the scene
	PhysX::pScene = PhysX::pPhysicsSDK->createScene ( sceneDesc );

	// check scene is valid
	if ( !PhysX::pScene )
	{
		MessageBox ( NULL, "scene is invalid", "info", MB_OK );

		// error
		return;
	}

	PhysX::pPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);

	
	
	
	// set contact report and trigger report
	PhysX::pScene->setUserContactReport ( &PhysX::ContactReport );
	PhysX::pScene->setUserTriggerReport ( &PhysX::TriggerReport );

	// set all actors to report contacts for on touch event
	PhysX::pScene->setActorGroupPairFlags ( 0, 0, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH );


	// set our default material
	NxMaterial* defaultMaterial = PhysX::pScene->getMaterialFromIndex ( PhysX::SceneMaterialIndex );

	// check material pointer
	if ( !defaultMaterial )
	{
		// error
		return;
	}

	// set restitution and static and dynamic friction
	defaultMaterial->setRestitution     ( 0.50f );
	defaultMaterial->setStaticFriction  ( 1.00f );
	defaultMaterial->setDynamicFriction ( 0.75f );
	

	//PhysX::pPhysicsSDK->setParameter(NX_SKIN_WIDTH, -0.05);
	PhysX::pPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.005);

	//PhysX::pPhysicsSDK->setParameter(NX_SKIN_WIDTH, -1.5);

	//PhysX::pScene->setTiming ( 1.0 / 60.0, 8, NX_TIMESTEP_FIXED );
	//PhysX::pScene->setTiming ( 1.0 / 60.0, 8, NX_TIMESTEP_VARIABLE );

	//defaultMaterial->setRestitution     ( 0.0f );
	//defaultMaterial->setStaticFriction  ( 0.0f );
	//defaultMaterial->setDynamicFriction ( 0.0f );
	
	NxReal fElapsedTime = 1/60.0f;
	PhysX::pScene->simulate    ( fElapsedTime );
	PhysX::pScene->flushStream ( );
	
}

void dbPhyUpdateParticles				( void );
void dbPhyUpdateCloth ( void );

void dbPhyUpdate ( void )
{
	// update the physics engine

	if ( !PhysX::pScene )
	{
		// error
		return;
	}

	///
	while ( !PhysX::pScene->checkResults ( NX_RIGID_BODY_FINISHED, false ) )
	{
		// we can do something here, not sure what....
	}

	// complete results
	PhysX::pScene->fetchResults ( NX_RIGID_BODY_FINISHED, true );
	///

	dbPhyUpdateParticles ( );
	dbPhyUpdateCloth ( );

	// set up our local variables, number of actors, list of actors
	// and the time elapsed between the last call and this call
	int       iActorCount  = 0;
    NxActor** ppActorList  = NULL;
	NxReal    fElapsedTime = ( NxReal ) dbPhyUpdateTime ( );

	//if ( fElapsedTime > 0.1f )
	//	fElapsedTime = 0.1f;

	//fElapsedTime = 0.02f;
	fElapsedTime = 1/60.0f;

	// simulate the scene and flush the stream
	PhysX::pScene->simulate    ( fElapsedTime );
	PhysX::pScene->flushStream ( );

	/*
	// the physics is running in a thread so we can do some work in
	// this loop if required
	while ( !PhysX::pScene->checkResults ( NX_RIGID_BODY_FINISHED, false ) )
	{
		// we can do something here, not sure what....
	}

	// complete results
	PhysX::pScene->fetchResults ( NX_RIGID_BODY_FINISHED, true );
	*/
	
	// get number of actors and the actor list
	iActorCount = PhysX::pScene->getNbActors ( );
    ppActorList = PhysX::pScene->getActors   ( );

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
			if ( pPhysXObject->type != PhysX::eBoxTrigger && pPhysXObject->type != PhysX::eSphereTrigger )
			{
				// get the rotation angles from the matrix
				NxVec3 xyz;
				xyz = dbPhyEulerFromMat44 ( &( glmat [ 0 ] ) );

				// rotate and position the object
				//dbRotateObject   ( pPhysXObject->iID, D3DXToDegree ( xyz.x ), D3DXToDegree ( xyz.y ), D3DXToDegree ( xyz.z ) );
				//dbPositionObject ( pPhysXObject->iID, glmat [ 12 ], glmat [ 13 ], glmat [ 14 ] );

				if ( PhysX::RotateObject )
					PhysX::RotateObject ( pPhysXObject->iID, D3DXToDegree ( xyz.x ), D3DXToDegree ( xyz.y ), D3DXToDegree ( xyz.z ) );

				//if ( pPhysXObject->type == PhysX::eTypeVehicle )
				//	glmat [ 13 ] -= 6.2f;

				if ( PhysX::PositionObject )
				{
					//if ( pPhysXObject->iID == 2 )
						//PhysX::PositionObject ( pPhysXObject->iID, glmat [ 12 ], glmat [ 13 ], glmat [ 14 ] );
					//else
						PhysX::PositionObject ( pPhysXObject->iID, glmat [ 12 ], glmat [ 13 ], glmat [ 14 ] );
				}
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

void dbPhyEnd ( void )
{
	// this function is used to stop the physics processing

	// release the scene
	if ( PhysX::pPhysicsSDK )
	{
		PhysX::pPhysicsSDK->releaseScene ( *PhysX::pScene );
		PhysX::pScene = NULL;

		// release the sdk
		PhysX::pPhysicsSDK->release ( );
		PhysX::pPhysicsSDK = NULL;
	}
}

void dbPhyClear ( void )
{
	// a quick way of clearing all objects in the scene

	if ( PhysX::pScene == NULL )
		return;

	// run through all objects
	for ( DWORD dwObject = 0; dwObject < PhysX::pObjectList.size ( ); dwObject++ )
	{
		// get a pointer to the object
		PhysX::sPhysXObject* pObject = PhysX::pObjectList [ dwObject ];

		// if we have a valid object
		if ( pObject )
		{
			// see if we have an actor and
			if ( pObject->pActor )
			{
				// now we can release the actor
				PhysX::pScene->releaseActor ( *pObject->pActor );
			}

			// delete the physics object that we allocated
			// when creating it
			delete pObject;
			pObject = NULL;
		}
	}

	// finally clear the object list
	PhysX::pObjectList.clear ( );
}

void dbPhySetGravity ( float fX, float fY, float fZ )
{
	// set the gravity in the scene

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// now set the gravity
	PhysX::pScene->setGravity ( NxVec3 ( fX, fY, fZ ) );
}

void dbPhySetTiming ( float fMaxTimeStep, int iMaxIterations, int iMethod )
{
	// set the timing used by the scene

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// our method of timing, fixed by default
	NxTimeStepMethod method = NX_TIMESTEP_FIXED;

	// when choice is 1 use variable timing
	if ( iMethod == 1 )
		method = NX_TIMESTEP_VARIABLE;

	// now set the gravity
	PhysX::pScene->setTiming ( ( NxReal ) fMaxTimeStep, ( NxU32 ) iMaxIterations, method );
}

void dbPhySetSkinWidth ( float fValue )
{
	// the simulation deals with inaccuracy when stacking objects by letting
	// them slightly inter penetrate each other, the amount of permitted inter
	// penetration can be regulated at a scene level using the NX_SKIN_WIDTH
	// parameter

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set skin width
	PhysX::pPhysicsSDK->setParameter ( NX_SKIN_WIDTH, fValue );
}

void dbPhySetDefaultSleepLinearVelocitySquared ( float fValue )
{
	// the default linear velocity, squared, below which objects start going to sleep

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, fValue );
}

void dbPhySetDefaultSleepAngularVelocitySquared ( float fValue )
{
	// the default angular velocity, squared, below which objects start going to sleep

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, fValue );
}

void dbPhySetBounceThreshold ( float fValue )
{
	// a contact with a relative velocity below this will not bounce

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_BOUNCE_TRESHOLD, fValue );
}

void dbPhySetDynamicFriction ( float fValue )
{
	// this lets the user scale the magnitude of the dynamic friction
	// applied to all objects

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_DYN_FRICT_SCALING, fValue );
}

void dbPhySetStaticFriction ( float fValue )
{
	// this lets the user scale the magnitude of the static friction
	// applied to all objects

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_STA_FRICT_SCALING, fValue );
}

void dbPhySetMaxAngularVelocity ( float fValue )
{
	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_MAX_ANGULAR_VELOCITY, fValue );
}

void dbPhySetContinousCD ( float fValue )
{
	// enable / disable continuous collision detection ( 0.0f to disable )

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_CONTINUOUS_CD, fValue );
}

void dbPhySetAdaptiveForce ( float fValue )
{
	// used to enable adaptive forces to accelerate convergence of the solver

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_ADAPTIVE_FORCE, fValue );
}

void dbPhySetControlledFiltering ( float fValue )
{
	// controls default filtering for jointed bodies ( true = collision disabled )

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_COLL_VETO_JOINTED, fValue );
}

void dbPhySetTriggerCallback ( float fValue )
{
	// controls whether two touching triggers generate a callback or not

	// check scene is valid
	if ( !PhysX::pScene )
		return;

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_TRIGGER_TRIGGER_CALLBACK, fValue );
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