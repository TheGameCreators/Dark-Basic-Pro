
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbodyspherejoints.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE SPHERE JOINT%LLLFFF%?dbPhyMakeSphereJoint@@YAXHHHMMM@Z%
	PHY MAKE SPHERE JOINT%LLL%?dbPhyMakeSphereJoint@@YAXHHH@Z%

	PHY SET SPHERE JOINT LOCAL ANCHOR%LLFFF%?dbPhySetSphereJointLocalAnchor@@YAXHHMMM@Z%
	PHY SET SPHERE JOINT LOCAL AXIS%LLFFF%?dbPhySetSphereJointLocalAxis@@YAXHHMMM@Z%
	PHY SET SPHERE JOINT GLOBAL ANCHOR%LLFFF%?dbPhySetSphereJointGlobalAnchor@@YAXHHMMM@Z%
	PHY SET SPHERE JOINT GLOBAL AXIS%LLFFF%?dbPhySetSphereJointGlobalAxis@@YAXHHMMM@Z%

	PHY SET SPHERE JOINT SWING AXIS%LFFF%?dbPhySetSphereJointSwingAxis@@YAXHMMM@Z%
	PHY SET SPHERE JOINT SWING LIMIT%LFFF%?dbPhySetSphereJointSwingLimit@@YAXHMMM@Z%
	PHY SET SPHERE JOINT TWIST LIMIT%LFF%?dbPhySetSphereJointTwistLimit@@YAXHMM@Z%
	PHY SET SPHERE JOINT TWIST SPRING%LFF%?dbPhySetSphereJointTwistSpring@@YAXHMM@Z%
	PHY SET SPHERE JOINT SWING SPRING%LFF%?dbPhySetSphereJointSwingSpring@@YAXHMM@Z%
	PHY SET SPHERE JOINT SPRING%LFF%?dbPhySetSphereJointSpring@@YAXHMM@Z%
	PHY SET SPHERE JOINT PROJECTION%LLF%?dbPhySetSphereJointProjection@@YAXHHM@Z%

	PHY BUILD SPHERE JOINT%L%?dbPhyBuildSphereJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void dbPhyMakeSphereJoint ( int iJoint, int iA, int iB, float fX, float fY, float fZ )
{
	dbPhyMakeSphereJoint ( iJoint, iA, iB );

	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		pJoint->pSphericalDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );

		dbPhyBuildSphereJoint ( iJoint );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to make sphere joint", "", 0 );
		return;
	}
}

void dbPhyMakeSphereJoint ( int iJoint, int iA, int iB )
{
	if ( !dbPhySetupSphereJoint ( iJoint, iA, iB ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up sphere joint", "", 0 );
		return;
	}

	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint );

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Sphere joint does not exist", "", 0 );
		return;
	}

	pJoint->pSphericalDesc = new NxSphericalJointDesc;

	// check memory
	if ( !pJoint->pSphericalDesc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for sphere joint", "", 0 );
		return;
	}

	pJoint->pSphericalDesc->setToDefault ( );

	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pSphericalDesc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pSphericalDesc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pSphericalDesc->actor [ 0 ] = NULL;

	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pSphericalDesc->actor [ 1 ] = NULL;
}

void dbPhySetSphereJointLocalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		pJoint->pSphericalDesc->localAnchor [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetSphereJointLocalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		pJoint->pSphericalDesc->localAxis [ iIndex ] = NxVec3 ( fX, fY, fZ );	
	}
}

void dbPhySetSphereJointGlobalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		pJoint->pSphericalDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetSphereJointGlobalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		pJoint->pSphericalDesc->setGlobalAxis ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetSphereJointSwingAxis ( int iJoint, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		pJoint->pSphericalDesc->swingAxis = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetSphereJointSwingLimit ( int iJoint, float fValue, float fRestitution, float fHardness )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		NxJointLimitDesc limitDesc;

		limitDesc.value       = ( NxReal ) fValue;
		limitDesc.restitution = ( NxReal ) fRestitution;
		limitDesc.hardness    = ( NxReal ) fRestitution;
		
		pJoint->pSphericalDesc->flags      |= NX_SJF_SWING_LIMIT_ENABLED;
		pJoint->pSphericalDesc->swingLimit  = limitDesc;
	}
}

void dbPhySetSphereJointTwistLimit ( int iJoint, float fLow, float fHigh )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		NxJointLimitPairDesc limitDesc;

		limitDesc.low.value  = ( NxReal ) fLow;
		limitDesc.high.value = ( NxReal ) fHigh;
		
		pJoint->pSphericalDesc->flags      |= NX_SJF_TWIST_LIMIT_ENABLED;
		pJoint->pSphericalDesc->twistLimit  = limitDesc;
	}
}

void dbPhySetSphereJointTwistSpring ( int iJoint, float fTarget, float fSpring )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		NxSpringDesc springDesc;

		springDesc.targetValue = ( NxReal ) fTarget;
		springDesc.spring      = ( NxReal ) fSpring;

		pJoint->pSphericalDesc->flags       |= NX_SJF_TWIST_SPRING_ENABLED;
		pJoint->pSphericalDesc->twistSpring  = springDesc;
	}
}

void dbPhySetSphereJointSwingSpring ( int iJoint, float fTarget, float fSpring )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		NxSpringDesc springDesc;

		springDesc.targetValue = ( NxReal ) fTarget;
		springDesc.spring      = ( NxReal ) fSpring;

		pJoint->pSphericalDesc->flags       |= NX_SJF_SWING_SPRING_ENABLED;
		pJoint->pSphericalDesc->swingSpring  = springDesc;
	}
}

void dbPhySetSphereJointSpring ( int iJoint, float fTarget, float fSpring )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		NxSpringDesc springDesc;

		springDesc.targetValue = ( NxReal ) fTarget;
		springDesc.spring      = ( NxReal ) fSpring;

		pJoint->pSphericalDesc->flags       |= NX_SJF_JOINT_SPRING_ENABLED;
		pJoint->pSphericalDesc->jointSpring  = springDesc;
	}
}

void dbPhySetSphereJointProjection ( int iJoint, int iMode, float fDistance )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		pJoint->pSphericalDesc->projectionMode     = NX_JPM_POINT_MINDIST;
		pJoint->pSphericalDesc->projectionDistance = ( NxReal ) fDistance;
	}
}

void dbPhyBuildSphereJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eSphereJoint ) )
	{
		if ( !pJoint->pSphericalDesc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Sphere joint configuration is invalid", "", 0 );
			return;
		}

		pJoint->pSphereJoint = ( NxSphericalJoint* ) PhysX::pScene->createJoint ( *pJoint->pSphericalDesc );
		pJoint->pJoint       = pJoint->pSphereJoint;
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot build sphere joint", "", 0 );
		return;
	}
}

bool dbPhySetupSphereJoint ( int iJoint, int iA, int iB )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up sphere joint as ID is already in use", "", 0 );
		return false;
	}

	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	memset ( pJoint, 0, sizeof ( PhysX::sPhysXJoint ) );

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for sphere joint", "", 0 );
		return false;
	}

	pJoint->iID  = iJoint;
	pJoint->type = PhysX::eSphereJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	if ( !pJoint->pA || !pJoint->pB )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for sphere joint", "", 0 );
		return false;
	}

	PhysX::pJointList.push_back ( pJoint );

	return true;
}
