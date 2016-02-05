     
#include "globals.h"
#include "engineproperties.h"
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
	PHY SET GRAVITY%FFF%?dbPhySetGravity@@YAXMMM@Z%
	PHY SET TIMING%FLL%?dbPhySetTiming@@YAXMHH@Z%
	PHY SET FIXED TIMING%F%?dbPhySetFixedTiming@@YAXM@Z%
	PHY SET AUTO FIXED TIMING%0%?dbPhySetAutoFixedTiming@@YAXXZ%

	PHY SET SKIN WIDTH%F%?dbPhySetSkinWidth@@YAXM@Z%
	PHY SET DEFAULT SLEEP LINEAR VELOCITY SQUARED%F%?dbPhySetDefaultSleepLinearVelocitySquared@@YAXM@Z%
	PHY SET DEFAULT SLEEP ANGULAR VELOCITY SQUARED%F%?dbPhySetDefaultSleepAngularVelocitySquared@@YAXM@Z%
	PHY SET BOUNCE THRESHOLD%F%?dbPhySetBounceThreshold@@YAXM@Z%
	PHY SET DYNAMIC FRICTION%F%?dbPhySetDynamicFriction@@YAXM@Z%
	PHY SET STATIC FRICTION%F%?dbPhySetStaticFriction@@YAXM@Z%

	PHY SET MAX ANGULAR VELOCITY%F%?dbPhySetMaxAngularVelocity@@YAXM@Z%
	PHY SET CONTINUOUS CD%F%?dbPhySetContinuousCD@@YAXM@Z%
	PHY SET ADAPTIVE FORCE%F%?dbPhySetAdaptiveForce@@YAXM@Z%
	PHY SET CONTROLLED FILTERING%F%?dbPhySetControlledFiltering@@YAXM@Z%
	PHY SET TRIGGER CALLBACK%F%?dbPhySetTriggerCallback@@YAXM@Z%

	PHY SET GROUP COLLISION%LLL%?dbPhySetGroupCollision@@YAXHHH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhySetFixedTiming ( float fElapsedTime )
{
	PhysX::iTimingMethod = 0;
	PhysX::fFixedTime    = fElapsedTime;
}

void dbPhySetAutoFixedTiming ( void )
{
	PhysX::iTimingMethod = 2;
}

void dbPhySetGravity ( float fX, float fY, float fZ )
{
	// set the gravity in the scene

	// check scene is valid
	if ( !PhysX::pScene )
	{
		// report an error
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set gravity", "", 0 );
		return;
	}

	// now set the gravity
	PhysX::pScene->setGravity ( NxVec3 ( fX, fY, fZ ) );
}

void dbPhySetTiming ( float fMaxTimeStep, int iMaxIterations, int iMethod )
{
	// set the timing used by the scene

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set timing", "", 0 );
		return;
	}

	// our method of timing, fixed by default
	NxTimeStepMethod method = NX_TIMESTEP_FIXED;

	// when choice is 1 use variable timing
	if ( iMethod == 1 )
	{
		method = NX_TIMESTEP_VARIABLE;
		PhysX::iTimingMethod = 1;
	}

	if ( iMethod == 0 )
	{
		//PhysX::iTimingMethod = 0;
		//PhysX::fFixedTime    = fMaxTimeStep;
	}

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
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set skin width", "", 0 );
		return;
	}

	// set skin width
	PhysX::pPhysicsSDK->setParameter ( NX_SKIN_WIDTH, fValue );
}

void dbPhySetDefaultSleepLinearVelocitySquared ( float fValue )
{
	// the default linear velocity, squared, below which objects start going to sleep

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set sleep linear velocity squared", "", 0 );
		return;
	}

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, fValue );
}

void dbPhySetDefaultSleepAngularVelocitySquared ( float fValue )
{
	// the default angular velocity, squared, below which objects start going to sleep

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set sleep angular velocity squared", "", 0 );
		return;
	}

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, fValue );
}

void dbPhySetBounceThreshold ( float fValue )
{
	// a contact with a relative velocity below this will not bounce

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set bounce threshold", "", 0 );
		return;
	}

	// set property
	//PhysX::pPhysicsSDK->setParameter ( NX_BOUNCE_TRESHOLD, fValue );
}

void dbPhySetDynamicFriction ( float fValue )
{
	// this lets the user scale the magnitude of the dynamic friction
	// applied to all objects

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set dynamic friction", "", 0 );
		return;
	}

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_DYN_FRICT_SCALING, fValue );
}

void dbPhySetStaticFriction ( float fValue )
{
	// this lets the user scale the magnitude of the static friction
	// applied to all objects

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set static friction", "", 0 );
		return;
	}

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_STA_FRICT_SCALING, fValue );
}

void dbPhySetMaxAngularVelocity ( float fValue )
{
	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set max angular velocity", "", 0 );
		return;
	}

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_MAX_ANGULAR_VELOCITY, fValue );
}

void dbPhySetContinuousCD ( float fValue )
{
	// enable / disable continuous collision detection ( 0.0f to disable )

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set CCD", "", 0 );
		return;
	}

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_CONTINUOUS_CD, fValue );
}

void dbPhySetAdaptiveForce ( float fValue )
{
	// used to enable adaptive forces to accelerate convergence of the solver

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set adaptive force", "", 0 );
		return;
	}

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_ADAPTIVE_FORCE, fValue );
}

void dbPhySetControlledFiltering ( float fValue )
{
	// controls default filtering for jointed bodies ( true = collision disabled )

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set controlled filtering", "", 0 );
		return;
	}

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_COLL_VETO_JOINTED, fValue );
}

void dbPhySetTriggerCallback ( float fValue )
{
	// controls whether two touching triggers generate a callback or not

	// check scene is valid
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set trigger callback", "", 0 );
		return;
	}

	// set property
	PhysX::pPhysicsSDK->setParameter ( NX_TRIGGER_TRIGGER_CALLBACK, fValue );
}

void dbPhySetGroupCollision ( int iA, int iB, int iState )
{
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to set group collision", "", 0 );
		return;
	}

	if ( iState==0 )
		PhysX::pScene->setGroupCollisionFlag ( iA, iB, false );
	else
		PhysX::pScene->setGroupCollisionFlag ( iA, iB, true );
}