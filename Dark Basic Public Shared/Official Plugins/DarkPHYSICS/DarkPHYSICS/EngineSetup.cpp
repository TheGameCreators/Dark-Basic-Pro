     
#include "globals.h"
#include "enginesetup.h"
#include "error.h"
#include "cRuntimeErrors.h"
#include <windows.h>
#include <shellapi.h>
#include "EngineUpdate.h"
#include <time.h>
#include "DxFluidRenderer.h"


#ifdef DARKSDK_COMPILE
	#include "DarkSDK.h"
#endif

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY ENABLE DEBUG%0%?dbPhyEnableDebug@@YAXXZ%
	PHY START%0%?dbPhyStart@@YAXXZ%
	PHY START%LLL%?dbPhyStart@@YAXHHH@Z%
	PHY END%0%?dbPhyEnd@@YAXXZ%
	PHY CLEAR%0%?dbPhyClear@@YAXXZ%
	PHYSX[%L%?dbPhysX@@YAHXZ%

	PHY MAKE SCENE%LLLL%?dbPhyMakeScene@@YAXHHHH@Z%%
	PHY SET CURRENT SCENE%L%?dbPhySetCurrentScene@@YAXH@Z%

	PHY SET GROUND PLANE%L%?dbPhySetGroundPlane@@YAXH@Z%
*/

void dbPhyClearFluids ( void );
void dbPhyClearParticles ( void );
void dbPhyClearMaterials ( void );
void dbPhyClearCloth ( int iID );
void dbPhyClearJoints ( void );

DWORD GetLimbAngleX ( int iID, int iLimbID );
DWORD GetLimbAngleY ( int iID, int iLimbID );
DWORD GetLimbAngleZ ( int iID, int iLimbID );
DWORD GetLimbOffsetX ( int iID, int iLimbID );
DWORD GetLimbOffsetY ( int iID, int iLimbID );
DWORD GetLimbOffsetZ ( int iID, int iLimbID );

bool ConvertLocalMeshToTriList ( sMesh* pMesh );

void dbPhySetupSoftBody ( void );

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyEnableDebug ( void )
{
	PhysX::iDebug = 1;
}

bool PhyCheckTrialStatus ( void )
{
	// if compiled as a trial version
	#ifdef TRIALVERSION
	// get todays date
	struct tm *newtime;
	time_t long_time;
	time( &long_time );         
	newtime = localtime( &long_time );

	// if within trial range, okay, else shutdown!
	bool bShutDown=true;
	if ( newtime->tm_year==(2008-1900) )
	if ( newtime->tm_mon>=5 && newtime->tm_mon<=8 )
	  bShutDown=false;

	// expire if outside range
	if ( bShutDown==true )
	{
		MessageBox ( NULL, "This Trial Expires After SEPT 30TH 2008", "Trial Has Expired", MB_OK);
		return false;
	}

	// otherwise allow
	return true;
	#else
	// allow for full version
	return true;
	#endif
}

void dbPhyStart ( void )
{
	if ( PhyCheckTrialStatus()==true )
	{
		// will start the simulation with default settings
		// to run in software mode

		// set default values
		PhysX::iSimulationType      =  0;
		PhysX::iSimulationSceneType = -1;
		PhysX::iSimulationFlags     = -1;

		// start the process
		dbPhyStartProcess ( 0 );
	}
}

void dbPhyStart ( int iSimulationType, int iSimulationSceneType, int iSimulationFlags )
{
	if ( PhyCheckTrialStatus()==true )
	{
		// will start the simulation and allow the user to
		// specify how things are set up

		// take in parameters
		PhysX::iSimulationType      = iSimulationType;
		PhysX::iSimulationSceneType = iSimulationSceneType;
		PhysX::iSimulationFlags     = iSimulationFlags;

		// now start the process
		dbPhyStartProcess ( 0 );
	}
}

void dbPhyEnd ( void )
{
	// this function is used to stop the physics processing

	dbPhyClear ( );

	NxCloseCooking ( );

	// release the scene
	if ( PhysX::pPhysicsSDK )
	{
		for ( int i = 0; i < (int)PhysX::pSceneList.size ( ); i++ )
		{
			if ( PhysX::pSceneList [ i ]->pScene )
			{
				PhysX::pScene = PhysX::pSceneList [ i ]->pScene;

				// CAN OFTEN GET REALLY SLOW DELAYS ON CLEAN UP - TEST CODE TO SEE IF THIS HELPS
				PhysX::pScene->resetPollForWork  ( );
				PhysX::pScene->shutdownWorkerThreads ( );
				PhysX::pScene->flushCaches ( );
				PhysX::pScene->flushStream ( );


				PhysX::pPhysicsSDK->releaseScene ( *PhysX::pScene );
				PhysX::pScene = NULL;
			}
		}

		// clear the list
		PhysX::pSceneList.clear ( );

		// can this cause a crash?
		NxCloseCooking ( );

		// release the sdk
		PhysX::pPhysicsSDK->release ( );
		PhysX::pPhysicsSDK = NULL;
	}
}

void dbPhyClear ( void )
{
    PhysX::fluidSurfaceRenderer->onLostDevice();
    PhysX::fluidSurfaceRenderer->onDestroyDevice();
    delete PhysX::fluidSurfaceRenderer;	
    PhysX::fluidSurfaceRenderer = NULL;

    // a quick way of clearing all objects in the scene
	dbPhyClearFluids    ( );
	dbPhyClearParticles ( );
	dbPhyClearCloth     ( -1 );
	dbPhyClearMaterials ( );
	dbPhyClearJoints ( );

	for ( DWORD dwObject = 0; dwObject < PhysX::pObjectList.size ( ); dwObject++ )
	{
		// get a pointer to the object
		PhysX::sPhysXObject* pObject = PhysX::pObjectList [ dwObject ];

		// if we have a valid object
		if ( pObject )
		{
			for ( DWORD dwActor = 0; dwActor < pObject->pActorList.size ( ); dwActor++ )
			{
				if ( pObject->pScene )
					pObject->pScene->releaseActor ( *pObject->pActorList [ dwActor ] );
			}

			pObject->pActorList.clear ( );

			// delete the physics object that we allocated
			// when creating it
			delete pObject;
			pObject = NULL;
		}
	}
	
	// finally clear the object list
	PhysX::pObjectList.clear ( );

}

int dbPhysX ( void )
{
	// this command is used to determine the presence of the hardware,
	// a local copy of the sdk is created to handle this

	// set up our variables
	NxPhysicsSDK*	pPhysicsSDK	= NULL;
	bool			version		= NX_HW_VERSION_NONE;
	int				iReturn		= 0;

	if ( PhysX::pScene )
	{
		// see if hardware is available
		if ( version = PhysX::pPhysicsSDK->getHWVersion ( ) != NX_HW_VERSION_NONE )
		{
			// if it is set the return value to 1
			iReturn = 1;
		}

		return iReturn;
	}

	// attempt to get access to the sdk
	if ( !( pPhysicsSDK = NxCreatePhysicsSDK ( NX_PHYSICS_SDK_VERSION ) ) )
	{
		// report an error
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set up SDK in dbPhysX", "", 0 );
		return -1;
	}

	// see if hardware is available
	if ( version = pPhysicsSDK->getHWVersion ( ) != NX_HW_VERSION_NONE )
	{
		// if it is set the return value to 1
		iReturn = 1;
	}

	// release the local copy of the SDK
	pPhysicsSDK->release ( );
	pPhysicsSDK = NULL;

	// and now return the value
	return iReturn;
}

void dbPhyMakeScene ( int iIndex, int iSimulationType, int iSimulationSceneType, int iSimulationFlags )
{
	PhysX::iSimulationType      = iSimulationType;
	PhysX::iSimulationSceneType = iSimulationSceneType;
	PhysX::iSimulationFlags     = iSimulationFlags;

	dbPhySetupScene				( iIndex );
	dbPhySetCurrentScene		( iIndex );
	dbPhySetupDefaultSettings	( );
}

void dbPhySetCurrentScene ( int iIndex )
{
	for ( int i = 0; i < (int)PhysX::pSceneList.size ( ); i++ )
	{
		if ( PhysX::pSceneList [ i ]->iID == iIndex )
		{
			PhysX::pScene = PhysX::pSceneList [ i ]->pScene;
			break;
		}
	}

	//PhysX::pScene = PhysX::pSceneList [ iIndex ];
}


int GetNumDependencies ( void )
{
	// let DB Pro know how many DLLs we need from the plugins-licensed folder
	return 5;
}

const char* GetDependencyID ( int n )
{
	// return names of the DLLs required for DB Pro
	switch ( n )
	{
		case 0 : return "PhysXLoader.dll";
		case 1 : return "NxCharacter.dll";
		case 2 : return "NxExtensions.dll";
		case 3 : return "DBProInputDebug.dll";
		case 4 : return "NxCooking.dll";
	}

	return "";
}


void dbPhyStartProcess ( int iID )
{
	// start the process of setting up the physics engine, this involves
	// several steps, going from setting up the function pointers, then
	// getting access to the sdk, setting up the scene and last of all
	// some default settings are used

	// AGEIA TRIAL : 15th August 2006 to 15th October 2006
	// Use Trial Period (forced trial period, i.e. Ageia)
#ifdef TRIAL
	// AGEIA TRIAL : August 2006 to October 2006
	// Use Trial Period (forced trial period, i.e. Ageia)
	struct tm *newtime;
	time_t long_time;
	time( &long_time ); 
	newtime = localtime( &long_time );
	// if within trial range, okay, else shutdown!
	bool bShutDown=true;
	//if ( newtime->tm_year==(2006-1900) ) // Only 2006
	if ( newtime->tm_year==(2006-1900) ) // Only 2006
	{
	if ( newtime->tm_mon>=7 && newtime->tm_mon<=10 ) // Only within AUG(7)-OCT(9)
	{
	// the only valid period for the compiler to function
	bShutDown = false;

	}
	}

	if ( bShutDown )
	{
// report an error
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Trial version of Dark Physics has expired", "", 0 );
			return;
	}
#endif

	dbPhySetupFunctionPointers ( );
	dbPhySetupSDK              ( );
	dbPhySetupScene            ( iID );
	dbPhySetupDefaultSettings  ( );

	dbPhySetupSoftBody ( );
}

void dbPhySetupFunctionPointers ( void )
{
	// set up function pointers

	//  before we do anything check the global data is ok
	#ifndef DARKSDK_COMPILE
		if ( !PhysX::pGlobStruct )
		{
			// report an error
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Global data is invalid in dbPhySetupFunctionPointers", "", 0 );
			return;
		}
	#endif

	// set up function pointers based on whether we are compiling
	// for the sdk or for db pro
	#ifdef DARKSDK_COMPILE

		PhysX::GetDirect3D					= dbGetDirect3D;
		PhysX::GetDirect3DDevice			= dbGetDirect3DDevice;

		PhysX::GetObject					= dbGetObject;
		PhysX::DeleteObject					= dbDeleteObject;
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

		PhysX::GetLimbAngleX				= GetLimbAngleX;
		PhysX::GetLimbAngleY				= GetLimbAngleY;
		PhysX::GetLimbAngleZ				= GetLimbAngleZ;
		PhysX::GetLimbOffsetX				= GetLimbOffsetX;
		PhysX::GetLimbOffsetY				= GetLimbOffsetY;
		PhysX::GetLimbOffsetZ				= GetLimbOffsetZ;

		PhysX::GetKeyState					= dbKeyState;
		PhysX::GetDisplayFPS				= dbScreenFPS;

		PhysX::GetCameraInternalData		= dbGetCameraInternalData;

		PhysX::ConvertLocalMeshToTriList	= ConvertLocalMeshToTriList;
	#else
		PhysX::GetDirect3D					= ( PhysX::GetDirect3DPFN					) GetProcAddress ( PhysX::pGlobStruct->g_GFX, "?GetDirect3D@@YAPAUIDirect3D9@@XZ" );
		PhysX::GetDirect3DDevice			= ( PhysX::GetDirect3DDevicePFN				) GetProcAddress ( PhysX::pGlobStruct->g_GFX, "?GetDirect3DDevice@@YAPAUIDirect3DDevice9@@XZ" );

		PhysX::GetObject					= ( PhysX::GetObjectPFN						) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetObjectA@@YAPAUsObject@@H@Z" );
		PhysX::DeleteObject					= ( PhysX::DeleteObjectPFN					) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?DeleteObject@@YA_NH@Z" );
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
		PhysX::GetDisplayFPS				= ( PhysX::GetDisplayFPSPFN					) GetProcAddress ( PhysX::pGlobStruct->g_GFX,     "?GetDisplayFPS@@YAHXZ" );

		PhysX::GetLimbPositionX				= ( PhysX::GetLimbPositionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbXPosition@@YAKHH@Z" );
		PhysX::GetLimbPositionY				= ( PhysX::GetLimbPositionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbYPosition@@YAKHH@Z" );
		PhysX::GetLimbPositionZ				= ( PhysX::GetLimbPositionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbZPosition@@YAKHH@Z" );
		PhysX::GetLimbDirectionX			= ( PhysX::GetLimbDirectionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbXDirection@@YAKHH@Z" );
		PhysX::GetLimbDirectionY			= ( PhysX::GetLimbDirectionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbYDirection@@YAKHH@Z" );
		PhysX::GetLimbDirectionZ			= ( PhysX::GetLimbDirectionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbZDirection@@YAKHH@Z" );

		PhysX::GetLimbScaleX				= ( PhysX::GetLimbPositionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbScaleX@@YAKHH@Z" );
		PhysX::GetLimbScaleY				= ( PhysX::GetLimbPositionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbScaleY@@YAKHH@Z" );
		PhysX::GetLimbScaleZ				= ( PhysX::GetLimbPositionPFN				) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?GetLimbScaleZ@@YAKHH@Z" );

		PhysX::GetCameraInternalData		= ( PhysX::GetCameraInternalDataPFN			) GetProcAddress ( PhysX::pGlobStruct->g_Camera3D, "?GetInternalData@@YAPAXH@Z" );

		PhysX::ConvertLocalMeshToTriList	= ( PhysX::ConvertLocalMeshToTriListPFN		) GetProcAddress ( PhysX::pGlobStruct->g_Basic3D, "?ConvertLocalMeshToTriList@@YA_NPAUsMesh@@@Z" );
	#endif
}

void dbPhySetupSDK ( void )
{
	// set up the sdk

	// check pointers first
	if ( PhysX::pAllocator && PhysX::pPhysicsSDK )
	{
		// no need to set up twice
		return;
	}

	// create a new custom allocator object
	PhysX::pAllocator = new UserAllocator;

	// check memory is valid
	if ( !PhysX::pAllocator )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up memory allocator", "", 0 );
		return;
	}

	// get access to the sdk passing in our custom allocator and error stream
	PhysX::pPhysicsSDK = NxCreatePhysicsSDK ( NX_PHYSICS_SDK_VERSION, 0, &PhysX::m_ErrorStream );
	
	// check the engine is valid
	if ( !PhysX::pPhysicsSDK )
	{
		ShellExecute ( NULL, NULL, "http://www.nvidia.com/object/physx_8.09.04_whql.html", NULL, NULL, NULL );
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid PhysX Runtime - check drivers are installed - http://www.nvidia.com/object/physx_8.09.04_whql.html", "", 0 );
		return;
	}

	// when using debug compile hook up to the remote debugger
	if ( PhysX::iDebug )
	{
		PhysX::pPhysicsSDK->getFoundationSDK ( ).getRemoteDebugger ( )->connect ( "localhost", 5425 );
	}

	//NxInitCooking ( NULL, &PhysX::m_ErrorStream );

	PhysX::fluidSurfaceRenderer = new DxFluidRenderer;	
	PhysX::fluidSurfaceRenderer->SetFluidColor ( D3DXVECTOR4(0.5f, 0.7f, 1.0f, 1.0f) );//blue
	IDirect3DDevice9 * pd3dDevice = PhysX::GetDirect3DDevice();
	PhysX::fluidSurfaceRenderer->onCreateDevice(pd3dDevice);
	PhysX::fluidSurfaceRenderer->onResetDevice(pd3dDevice);
}

void dbPhySetFluidRendererParticleRadius ( float v )
{
	PhysX::fluidSurfaceRenderer->SetParticleRadius ( v );
}
void dbPhySetFluidRendererParticleSurfaceScale ( float v )
{
	PhysX::fluidSurfaceRenderer->SetParticleSurfaceScale ( v );
}
void dbPhySetFluidRendererDensityThreshold ( float v )
{
	PhysX::fluidSurfaceRenderer->SetDensityThreshold ( v );
}
void dbPhySetFluidRendererFluidColor ( float r, float g, float b, float a )
{
	PhysX::fluidSurfaceRenderer->SetFluidColor ( D3DXVECTOR4(r,g,b,a) );
}
void dbPhySetFluidRendererFluidColorFalloff ( float r, float g, float b, float a )
{
	PhysX::fluidSurfaceRenderer->SetFluidColorFalloff ( D3DXVECTOR4(r,g,b,a) );
}
void dbPhySetFluidRendererColorFalloffScale ( float v )
{
	PhysX::fluidSurfaceRenderer->SetColorFalloffScale ( v );
}
void dbPhySetFluidRendererSpecularColor ( float r, float g, float b, float a )
{
	PhysX::fluidSurfaceRenderer->SetSpecularColor ( D3DXVECTOR4(r,g,b,a) );
}
void dbPhySetFluidRendererShininess ( float v )
{
	PhysX::fluidSurfaceRenderer->SetShininess ( v );
}
void dbPhySetFluidRendererFresnelColor ( float r, float g, float b, float a )
{
	PhysX::fluidSurfaceRenderer->SetFresnelColor ( D3DXVECTOR4(r,g,b,a) );
}

void dbPhySetGroundPlane ( int iState )
{
	if ( iState == 1 )
		PhysX::bEnableGroundPlane = true;
	else
		PhysX::bEnableGroundPlane = false;
}

void dbPhySetupScene ( int iID )
{
	// set properties for the scene and create it

	if ( !PhysX::pPhysicsSDK )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid link to runtime, check that phy start has been called", "", 0 );
		return;
	}

	// declare the scene description variable
	NxSceneDesc	sceneDesc;
	NxScene* pScene	= NULL;

	// set default gravity
	sceneDesc.gravity = PhysX::vecGravity;

	// change simulation type if necessary
	switch ( PhysX::iSimulationType )
	{
		case 0: sceneDesc.simType = NX_SIMULATION_SW;						break;
		case 1: sceneDesc.simType = NX_SIMULATION_HW;						break;
	}

	/*
	// change the type of scene if necessary
	switch ( PhysX::iSimulationSceneType )
	{
		case 0: sceneDesc.hwSceneType = NX_HW_SCENE_TYPE_RB;				break;
		case 1: sceneDesc.hwSceneType = NX_HW_SCENE_TYPE_FLUID;				break;
		case 2: sceneDesc.hwSceneType = NX_HW_SCENE_TYPE_FLUID_SOFTWARE;	break;
		case 3: sceneDesc.hwSceneType = NX_HW_SCENE_TYPE_CLOTH;				break;
	}
	*/

	/*
	// add in any flags if required
	switch ( PhysX::iSimulationFlags )
	{
		case 0: sceneDesc.flags |= NX_SF_SIMULATE_SEPARATE_THREAD;			break;
		case 1: sceneDesc.flags |= NX_SF_ENABLE_MULTITHREAD;				break;
	}
	*/

	// LEEGPU - ?? EMULATE single core so CPU does not have unfair advantage over GPU!
	sceneDesc.flags |= NX_SF_ENABLE_MULTITHREAD;
    
	sceneDesc.groundPlane = PhysX::bEnableGroundPlane;

	// before continuing check the scene description is valid
	if ( !sceneDesc.isValid ( ) )
	{
		// if invalid then display an error message
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid scene parameters specified for 'phy start'", "", 0 );
		return;
	}
	
	// create the scene
	pScene = PhysX::pPhysicsSDK->createScene ( sceneDesc );

	// check scene was created okay
	if ( !pScene )
	{
		// display an error message
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to create scene for simulation", "", 0 );
		return;
	}

	if ( PhysX::pSceneList.size ( ) == 0 )
	{
		PhysX::pScene = pScene;
	}

	PhysX::sSceneContainer* pSceneContainer = new PhysX::sSceneContainer;

	if ( !pSceneContainer )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate memory for scene container", "", 0 );
		return;
	}

	pSceneContainer->iID    = iID;
	pSceneContainer->pScene = pScene;

	PhysX::pSceneList.push_back ( pSceneContainer );
}

void dbPhySetupDefaultSettings ( void )
{
	// set up default settings for the scene

	// check the sdk and scene is valid
	if ( !PhysX::pPhysicsSDK || !PhysX::pScene )
	{
		// show an error message
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid scene settings used", "", 0 );
		return;
	}

	// set certain properties when we are in software mode
	if ( PhysX::iSimulationSceneType != 1 )
	{
		// set contact report and trigger report
		PhysX::pScene->setUserContactReport ( &PhysX::ContactReport );
		PhysX::pScene->setUserTriggerReport ( &PhysX::TriggerReport );

		// set all actors to report contacts for on touch event
		PhysX::pScene->setActorGroupPairFlags ( 0, 0, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_END_TOUCH | NX_NOTIFY_ON_TOUCH | NX_NOTIFY_FORCES );

		// set our default material
		NxMaterial* defaultMaterial = PhysX::pScene->getMaterialFromIndex ( PhysX::SceneMaterialIndex );

		// check material pointer
		if ( !defaultMaterial )
		{
			// show an error message
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to extract default material when setting default scene properties", "", 0 );
			return;
		}

		// set restitution and static and dynamic friction
		defaultMaterial->setRestitution     ( 0.50f );
		defaultMaterial->setStaticFriction  ( 1.00f );
		defaultMaterial->setDynamicFriction ( 0.75f );
	}
	
	// adjust the default skin width parameter
	PhysX::pPhysicsSDK->setParameter ( NX_SKIN_WIDTH, 0.005f );

	// set the default timing
	PhysX::pScene->setTiming ( 1.0f / 60.0f, 8, NX_TIMESTEP_FIXED );

	// set timing method to 0 and the fixed time value
	PhysX::iTimingMethod = 0;
	PhysX::fFixedTime    = 1.0f / 60.0f;

	NxInitCooking ( );
}