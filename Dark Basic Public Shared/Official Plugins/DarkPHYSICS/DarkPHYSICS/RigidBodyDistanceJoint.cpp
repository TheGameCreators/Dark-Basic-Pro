
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbodydistancejoint.h"
#include "error.h"


/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE DISTANCE JOINT%LLLFFFFF%?dbPhyMakeDistanceJoint@@YAXHHHMMMMM@Z%
	PHY MAKE DISTANCE JOINT%LLL%?dbPhyMakeDistanceJoint@@YAXHHH@Z%

	PHY SET DISTANCE JOINT LOCAL ANCHOR%LLFFF%?dbPhySetDistanceJointLocalAnchor@@YAXHHMMM@Z%
	PHY SET DISTANCE JOINT GLOBAL ANCHOR%LLFFF%?dbPhySetDistanceJointGlobalAnchor@@YAXHHMMM@Z%
	PHY SET DISTANCE JOINT LOCAL AXIS%LLFFF%?dbPhySetDistanceJointLocalAxis@@YAXHHMMM@Z%
	PHY SET DISTANCE JOINT GLOBAL AXIS%LLFFF%?dbPhySetDistanceJointGlobalAxis@@YAXHHMMM@Z%

	PHY SET DISTANCE JOINT MAX%LF%?dbPhySetDistanceJointMax@@YAXHM@Z%
	PHY SET DISTANCE JOINT MIN%LF%?dbPhySetDistanceJointMin@@YAXHM@Z%
	PHY SET DISTANCE JOINT SPRING%LFFF%?dbPhySetDistanceJointSpring@@YAXHMMM@Z%

	PHY BUILD DISTANCE JOINT%L%?dbPhyBuildDistanceJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeDistanceJoint ( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fMinDistance, float fMaxDistance )
{
	dbPhyMakeDistanceJoint ( iJoint, iA, iB );

	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint ) )
	{
		NxVec3 anchor0		= NxVec3 (  0, 1, 0 );
		NxVec3 anchor1		= NxVec3 (  0, 1, 0 );
		NxVec3 globalAxis	= NxVec3 ( -1, 0, 0 );

		pJoint->pDistanceDesc->localAnchor [ 0 ] = anchor0;
		pJoint->pDistanceDesc->localAnchor [ 1 ] = anchor1;
		pJoint->pDistanceDesc->setGlobalAxis ( globalAxis );

		NxVec3 dist = pJoint->pB->pActorList [ 0 ]->getGlobalPose ( ) * anchor1 - pJoint->pA->pActorList [ 0 ]->getGlobalPose ( ) * anchor0;

		pJoint->pDistanceDesc->maxDistance = dist.magnitude ( ) * 1.5f;
		pJoint->pDistanceDesc->minDistance = dist.magnitude ( ) * 0.5f;

		NxSpringDesc spring;
		spring.spring = 100;
		spring.damper = 0.5;
		pJoint->pDistanceDesc->spring = spring;
		pJoint->pDistanceDesc->flags = (NX_DJF_MIN_DISTANCE_ENABLED | NX_DJF_MAX_DISTANCE_ENABLED); 
		pJoint->pDistanceDesc->flags |= NX_DJF_SPRING_ENABLED;
		pJoint->pDistanceDesc->jointFlags |= NX_JF_COLLISION_ENABLED;
		
		dbPhyBuildDistanceJoint ( iJoint );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to make distance joint", "", 0 );
		return;
	}
}

void dbPhyMakeDistanceJoint	( int iJoint, int iA, int iB )
{
	if ( !dbPhySetupDistanceJoint ( iJoint, iA, iB ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up distance joint", "", 0 );
		return;
	}

	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint );

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Distance joint does not exist", "", 0 );
		return;
	}

	pJoint->pDistanceDesc = new NxDistanceJointDesc;

	if ( !pJoint->pDistanceDesc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for distance joint", "", 0 );
		return;
	}

	pJoint->pDistanceDesc->setToDefault ( );

	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pDistanceDesc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pDistanceDesc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pDistanceDesc->actor [ 0 ] = NULL;

	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pDistanceDesc->actor [ 1 ] = NULL;
}

void dbPhySetDistanceJointLocalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint ) )
	{
		pJoint->pDistanceDesc->localAnchor [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetDistanceJointGlobalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint ) )
	{
		pJoint->pDistanceDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetDistanceJointLocalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint ) )
	{
		pJoint->pDistanceDesc->localAxis [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetDistanceJointGlobalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint ) )
	{
		pJoint->pDistanceDesc->setGlobalAxis ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetDistanceJointMax ( int iJoint, float fMax )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint ) )
	{
		pJoint->pDistanceDesc->flags      |= NX_DJF_MAX_DISTANCE_ENABLED;
		pJoint->pDistanceDesc->maxDistance = ( NxReal ) fMax;
	}
}

void dbPhySetDistanceJointMin ( int iJoint, float fMin )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint ) )
	{
		pJoint->pDistanceDesc->flags      |= NX_DJF_MIN_DISTANCE_ENABLED;
		pJoint->pDistanceDesc->minDistance = ( NxReal ) fMin;
	}
}

void dbPhySetDistanceJointSpring ( int iJoint, float fTarget, float fSpring, float fDamper )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint ) )
	{
		NxSpringDesc springDesc;

		springDesc.targetValue = ( NxReal ) fTarget;
		springDesc.spring      = ( NxReal ) fSpring;
		springDesc.damper      = ( NxReal ) fDamper;

		pJoint->pDistanceDesc->flags  |= NX_DJF_SPRING_ENABLED;
		pJoint->pDistanceDesc->spring  = springDesc;
	}
}

void dbPhyBuildDistanceJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eDistanceJoint ) )
	{
		if ( !pJoint->pDistanceDesc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Distance joint configuration is invalid", "", 0 );
			return;
		}

		pJoint->pDistanceJoint = ( NxDistanceJoint* ) PhysX::pScene->createJoint ( *pJoint->pDistanceDesc );
		pJoint->pJoint         = pJoint->pDistanceJoint;
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot build distance joint", "", 0 );
		return;
	}
}

bool dbPhySetupDistanceJoint ( int iJoint, int iA, int iB )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up distance joint as ID is already in use", "", 0 );
		return false;
	}

	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for distance joint", "", 0 );
		return false;
	}

	pJoint->iID  = iJoint;
	pJoint->type = PhysX::eDistanceJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	if ( !pJoint->pA || !pJoint->pB )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for distance joint", "", 0 );
		return false;
	}

	PhysX::pJointList.push_back ( pJoint );
	return true;
}
