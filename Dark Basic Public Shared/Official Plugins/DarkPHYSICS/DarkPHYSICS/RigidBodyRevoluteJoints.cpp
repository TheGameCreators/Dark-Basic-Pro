
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbodyrevolutejoints.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE REVOLUTE JOINT%LLLFFFFFF%?dbPhyMakeRevoluteJoint@@YAXHHHMMMMMM@Z%
	PHY MAKE REVOLUTE JOINT%LLL%?dbPhyMakeRevoluteJoint@@YAXHHH@Z%

	PHY SET REVOLUTE JOINT LOCAL ANCHOR%LLFFF%?dbPhySetRevoluteJointLocalAnchor@@YAXHHMMM@Z%
	PHY SET REVOLUTE JOINT LOCAL AXIS%LLFFF%?dbPhySetRevoluteJointLocalAxis@@YAXHHMMM@Z%
	PHY SET REVOLUTE JOINT GLOBAL ANCHOR%LLFFF%?dbPhySetRevoluteJointGlobalAnchor@@YAXHHMMM@Z%
	PHY SET REVOLUTE JOINT GLOBAL AXIS%LLFFF%?dbPhySetRevoluteJointGlobalAxis@@YAXHHMMM@Z%
	PHY SET REVOLUTE JOINT SPRING%LFF%?dbPhySetRevoluteJointSpring@@YAXHMM@Z%
	PHY SET REVOLUTE JOINT LIMIT%LFF%?dbPhySetRevoluteJointLimit@@YAXHMM@Z%
	PHY SET REVOLUTE JOINT MOTOR%LFFL%?dbPhySetRevoluteJointMotor@@YAXHMMH@Z%
	PHY SET REVOLUTE JOINT PROJECTION%LLFF%?dbPhySetRevoluteJointProjection@@YAXHHMM@Z%

	PHY BUILD REVOLUTE JOINT%L%?dbPhyBuildRevoluteJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeRevoluteJoint ( int iJoint, int iA, int iB, float fAxisX, float fAxisY, float fAxisZ, float fX, float fY, float fZ )
{
	dbPhyMakeRevoluteJoint ( iJoint, iA, iB );

	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		pJoint->pRevoluteDesc->setGlobalAxis   ( NxVec3 ( fAxisX, fAxisY, fAxisZ ) );
		pJoint->pRevoluteDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );

		dbPhyBuildRevoluteJoint ( iJoint );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up revolute joint", "", 0 );
		return;
	}
}

void dbPhyMakeRevoluteJoint ( int iJoint, int iA, int iB )
{
	if ( !dbPhySetupRevoluteJoint ( iJoint, iA, iB ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up revolute joint", "", 0 );
		return;
	}

	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint );

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Revolute joint does not exist", "", 0 );
		return;
	}

	pJoint->pRevoluteDesc = new NxRevoluteJointDesc;

	// check memory
	if ( !pJoint->pRevoluteDesc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for revolute joint", "", 0 );
		return;
	}

	pJoint->pRevoluteDesc->setToDefault ( );

	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pRevoluteDesc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pRevoluteDesc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pRevoluteDesc->actor [ 0 ] = NULL;

	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pRevoluteDesc->actor [ 1 ] = NULL;
}

void dbPhySetRevoluteJointLocalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		pJoint->pRevoluteDesc->localAnchor [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetRevoluteJointLocalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		pJoint->pRevoluteDesc->localAxis [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetRevoluteJointGlobalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		pJoint->pRevoluteDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetRevoluteJointGlobalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		pJoint->pRevoluteDesc->setGlobalAxis ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetRevoluteJointSpring ( int iJoint, float fTarget, float fSpring )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		NxSpringDesc springDesc;

		springDesc.targetValue = ( NxReal ) fTarget;
		springDesc.spring      = ( NxReal ) fSpring;

		pJoint->pRevoluteDesc->flags  |= NX_RJF_SPRING_ENABLED;
		pJoint->pRevoluteDesc->spring  = springDesc;
	}
}

void dbPhySetRevoluteJointLimit	( int iJoint, float fLow, float fHigh )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		NxJointLimitPairDesc limitDesc;

		limitDesc.low.value  = ( NxReal ) fLow;
		limitDesc.high.value = ( NxReal ) fHigh;
		
		pJoint->pRevoluteDesc->flags |= NX_RJF_LIMIT_ENABLED;
		pJoint->pRevoluteDesc->limit  = limitDesc;
	}
}

void dbPhySetRevoluteJointMotor ( int iJoint, float fVelocityTarget, float fMaxForce, int iFreeSpin )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		NxMotorDesc motorDesc;

		motorDesc.velTarget = ( NxReal ) fVelocityTarget;
		motorDesc.maxForce  = ( NxReal ) fMaxForce;
		motorDesc.freeSpin  = iFreeSpin;

		pJoint->pRevoluteDesc->flags |= NX_RJF_MOTOR_ENABLED;
		pJoint->pRevoluteDesc->motor  = motorDesc;
	}
}

void dbPhySetRevoluteJointProjection ( int iJoint, int iMode, float fDistance, float fAngle )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		pJoint->pRevoluteDesc->projectionMode     = NX_JPM_POINT_MINDIST;
		pJoint->pRevoluteDesc->projectionDistance = ( NxReal ) fDistance;
		pJoint->pRevoluteDesc->projectionAngle    = ( NxReal ) fAngle;
	}
}

void dbPhyBuildRevoluteJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eRevoluteJoint ) )
	{
		if ( !pJoint->pRevoluteDesc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Revolute joint configuration is invalid", "", 0 );
			return;
		}

	

		pJoint->pRevoluteJoint = ( NxRevoluteJoint* ) PhysX::pScene->createJoint ( *pJoint->pRevoluteDesc );
		pJoint->pJoint         = pJoint->pRevoluteJoint;
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot build revolute joint", "", 0 );
		return;
	}
}

bool dbPhySetupRevoluteJoint ( int iJoint, int iA, int iB )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up revolute joint as ID is already in use", "", 0 );
		return false;
	}

	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for revolute joint", "", 0 );
		return false;
	}

	pJoint->iID  = iJoint;
	pJoint->type = PhysX::eRevoluteJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	if ( !pJoint->pA || !pJoint->pB )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for revolute joint", "", 0 );
		return false;
	}

	PhysX::pJointList.push_back ( pJoint );

	return true;
}
