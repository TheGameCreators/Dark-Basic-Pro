
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbody6dofjoint.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE 6DOF JOINT%LLL%?dbPhyMake6DOFJoint@@YAXHHH@Z%
	
	PHY SET 6DOF JOINT LOCAL ANCHOR%LLFFF%?dbPhySet6DOFJointLocalAnchor@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT LOCAL AXIS%LLFFF%?dbPhySet6DOFJointLocalAxis@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT LOCAL NORMAL%LLFFF%?dbPhySet6DOFJointLocalNormal@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT GLOBAL ANCHOR%LLFFF%?dbPhySet6DOFJointGlobalAnchor@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT GLOBAL AXIS%LLFFF%?dbPhySet6DOFJointGlobalAxis@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT TWIST MOTION%LL%?dbPhySet6DOFJointTwistMotion@@YAXHH@Z%
	PHY SET 6DOF JOINT SWING1 MOTION%LL%?dbPhySet6DOFJointSwing1Motion@@YAXHH@Z%
	PHY SET 6DOF JOINT SWING2 MOTION%LL%?dbPhySet6DOFJointSwing2Motion@@YAXHH@Z%
	PHY SET 6DOF JOINT MOTION X%LL%?dbPhySet6DOFJointMotionX@@YAXHH@Z%
	PHY SET 6DOF JOINT MOTION Y%LL%?dbPhySet6DOFJointMotionY@@YAXHH@Z%
	PHY SET 6DOF JOINT MOTION Z%LL%?dbPhySet6DOFJointMotionZ@@YAXHH@Z%
	PHY SET 6DOF JOINT LINEAR LIMIT%LFFFF%?dbPhySet6DOFJointLinearLimit@@YAXHMMMM@Z%
	PHY SET 6DOF JOINT SWING1 LIMIT%LFFFF%?dbPhySet6DOFJointSwing1Limit@@YAXHMMMM@Z%
	PHY SET 6DOF JOINT SWING2 LIMIT%LFFFF%?dbPhySet6DOFJointSwing2Limit@@YAXHMMMM@Z%
	PHY SET 6DOF JOINT TWIST LIMIT%LFFFFFFFF%?dbPhySet6DOFJointTwistLimit@@YAXHMMMMMMMM@Z%
	PHY SET 6DOF JOINT DRIVE X%LLFFF%?dbPhySet6DOFJointDriveX@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT DRIVE Y%LLFFF%?dbPhySet6DOFJointDriveY@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT DRIVE Z%LLFFF%?dbPhySet6DOFJointDriveZ@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT TWIST DRIVE%LLFFF%?dbPhySet6DOFJointTwistDrive@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT SWING DRIVE%LLFFF%?dbPhySet6DOFJointSwingDrive@@YAXHHMMM@Z%
	PHY SET 6DOF JOINT SLERP DRIVE%LLFFF%?dbPhySet6DOFJointSlerpDrive@@YAXHH@Z%
	PHY SET 6DOF JOINT DRIVE POSITION%LFFF%?dbPhySet6DOFJointDrivePosition@@YAXHMMM@Z%
	PHY SET 6DOF JOINT DRIVE ROTATION%LFFF%?dbPhySet6DOFJointDriveRotation@@YAXHMMM@Z%
	PHY SET 6DOF JOINT DRIVE LINEAR VELOCITY%LFFF%?dbPhySet6DOFJointDriveLinearVelocity@@YAXHMMM@Z%
	PHY SET 6DOF JOINT DRIVE ANGULAR VELOCITY%LFFF%?dbPhySet6DOFJointDriveAngularVelocity@@YAXHMMM@Z%
	PHY SET 6DOF JOINT PROJECTION%LLF%?dbPhySet6DOFJointProjection@@YAXHHM@Z%
	PHY SET 6DOF JOINT SLERP DRIVE STATE%LL%?dbPhySet6DOFJointSlerpDriveState@@YAXHH@Z%
	PHY SET 6DOF JOINT JOINT GEAR%LL%?dbPhySet6DOFJointGear@@YAXHH@Z%

	PHY BUILD 6DOF JOINT%L%?dbPhyBuild6DOFJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void dbPhyMake6DOFJoint ( int iJoint, int iA, int iB )
{
	// make a new 6dof joint

	
	// set up the joint
	if ( !dbPhySetup6DOFJoint ( iJoint, iA, iB ) )
	{
		// something went wrong
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up 6DOF joint", "", 0 );
		return;
	}

	// now get the joint pointer
	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::e6DOFJoint );

	// check it is valid
	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "6DOF joint does not exist", "", 0 );
		return;
	}

	// create a new 6DOF joint description
	pJoint->pD6Desc = new NxD6JointDesc;

	// check memory
	if ( !pJoint->pD6Desc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for 6DOF joint", "", 0 );
		return;
	}

	// set to default values
	pJoint->pD6Desc->setToDefault ( );

	// store first actor
	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pD6Desc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	// store second actor
	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pD6Desc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	// check static state of object a
	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pD6Desc->actor [ 0 ] = NULL;

	// check static state of object b
	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pD6Desc->actor [ 1 ] = NULL;
}

bool dbPhyCheck6DOFJointIndex ( int iIndex )
{
	// check the range is valid for the index

	// check values
	if ( iIndex >=0 && iIndex <= 1 )
	{
		// valid so return true
		return true;
	}
	else
	{
		// invalid index so display error message
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid index used for 6DOF joint", "", 0 );
		return false;
	}
}

PhysX::sPhysXJoint* dbPhyGet6DOFJoint ( int iJoint, int iIndex, bool bIndexCheck )
{
	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::e6DOFJoint, true );

	if ( bIndexCheck )
	{
		if ( !dbPhyCheck6DOFJointIndex ( iIndex ) )
			return NULL;
	}

	return pJoint;
}

void dbPhySet6DOFJointLocalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, iIndex, true ) )
	{
		pJoint->pD6Desc->localAnchor [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySet6DOFJointLocalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, iIndex, true ) )
	{
		pJoint->pD6Desc->localAxis [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySet6DOFJointLocalNormal ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, iIndex, true ) )
	{
		pJoint->pD6Desc->localNormal [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySet6DOFJointGlobalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, iIndex, true ) )
	{
		pJoint->pD6Desc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySet6DOFJointGlobalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, iIndex, true ) )
	{
		pJoint->pD6Desc->setGlobalAxis ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySet6DOFJointTwistMotion ( int iJoint, int iMotion )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->twistMotion = ( NxD6JointMotion ) iMotion;
	}
}

void dbPhySet6DOFJointSwing1Motion ( int iJoint, int iMotion )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->swing1Motion = ( NxD6JointMotion ) iMotion;
	}
}

void dbPhySet6DOFJointSwing2Motion ( int iJoint, int iMotion )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->swing2Motion = ( NxD6JointMotion ) iMotion;
	}
}

void dbPhySet6DOFJointMotionX ( int iJoint, int iX )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->xMotion = ( NxD6JointMotion ) iX;
	}
}

void dbPhySet6DOFJointMotionY ( int iJoint, int iY )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->yMotion = ( NxD6JointMotion ) iY;
	}
}

void dbPhySet6DOFJointMotionZ ( int iJoint, int iZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->zMotion = ( NxD6JointMotion ) iZ;
	}
}

void dbPhySet6DOFJointLinearLimit ( int iJoint, float fValue, float fRestitution, float fSpring, float fDamping )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->linearLimit.value       = fValue;
		pJoint->pD6Desc->linearLimit.restitution = fRestitution;
		pJoint->pD6Desc->linearLimit.spring      = fSpring;
		pJoint->pD6Desc->linearLimit.damping     = fDamping;
	}
}

void dbPhySet6DOFJointSwing1Limit ( int iJoint, float fValue, float fRestitution, float fSpring, float fDamping )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->swing1Limit.value       = fValue;
		pJoint->pD6Desc->swing1Limit.restitution = fRestitution;
		pJoint->pD6Desc->swing1Limit.spring      = fSpring;
		pJoint->pD6Desc->swing1Limit.damping     = fDamping;
	}
}

void dbPhySet6DOFJointSwing2Limit ( int iJoint, float fValue, float fRestitution, float fSpring, float fDamping )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->swing2Limit.value       = fValue;
		pJoint->pD6Desc->swing2Limit.restitution = fRestitution;
		pJoint->pD6Desc->swing2Limit.spring      = fSpring;
		pJoint->pD6Desc->swing2Limit.damping     = fDamping;
	}
}

void dbPhySet6DOFJointTwistLimit ( int iJoint, float fDampingLow, float fRestitutionLow, float fSpringLow, float fValueLow, float fDampingHigh, float fRestitutionHigh, float fSpringHigh, float fValueHigh )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->twistLimit.low.damping      = fDampingLow;
		pJoint->pD6Desc->twistLimit.low.restitution  = fRestitutionLow;
		pJoint->pD6Desc->twistLimit.low.spring       = fSpringLow;
		pJoint->pD6Desc->twistLimit.low.value        = fValueLow;
		
		pJoint->pD6Desc->twistLimit.high.damping     = fDampingHigh;
		pJoint->pD6Desc->twistLimit.high.restitution = fRestitutionHigh;
		pJoint->pD6Desc->twistLimit.high.spring      = fSpringHigh;
		pJoint->pD6Desc->twistLimit.high.value       = fValueHigh;
	}
}

void dbPhySet6DOFJointDriveX ( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->xDrive.driveType  = iDriveType;
		pJoint->pD6Desc->xDrive.spring     = fSpring;
		pJoint->pD6Desc->xDrive.damping    = fDamping;
		pJoint->pD6Desc->xDrive.forceLimit = fForceLimit;
	}
}

void dbPhySet6DOFJointDriveY ( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->yDrive.driveType  = iDriveType;
		pJoint->pD6Desc->yDrive.spring     = fSpring;
		pJoint->pD6Desc->yDrive.damping    = fDamping;
		pJoint->pD6Desc->yDrive.forceLimit = fForceLimit;
	}
}

void dbPhySet6DOFJointDriveZ ( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->zDrive.driveType  = iDriveType;
		pJoint->pD6Desc->zDrive.spring     = fSpring;
		pJoint->pD6Desc->zDrive.damping    = fDamping;
		pJoint->pD6Desc->zDrive.forceLimit = fForceLimit;
	}
}

void dbPhySet6DOFJointTwistDrive ( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->twistDrive.driveType  = iDriveType;
		pJoint->pD6Desc->twistDrive.spring     = fSpring;
		pJoint->pD6Desc->twistDrive.damping    = fDamping;
		pJoint->pD6Desc->twistDrive.forceLimit = fForceLimit;
	}
}

void dbPhySet6DOFJointSwingDrive ( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->swingDrive.driveType  = iDriveType;
		pJoint->pD6Desc->swingDrive.spring     = fSpring;
		pJoint->pD6Desc->swingDrive.damping    = fDamping;
		pJoint->pD6Desc->swingDrive.forceLimit = fForceLimit;
	}
}

void dbPhySet6DOFJointSlerpDrive ( int iJoint, int iDriveType, float fSpring, float fDamping, float fForceLimit )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->slerpDrive.driveType  = iDriveType;
		pJoint->pD6Desc->slerpDrive.spring     = fSpring;
		pJoint->pD6Desc->slerpDrive.damping    = fDamping;
		pJoint->pD6Desc->slerpDrive.forceLimit = fForceLimit;
	}
}

void dbPhySet6DOFJointDrivePosition	( int iJoint, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->drivePosition = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySet6DOFJointDriveRotation ( int iJoint, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{

	}
}

void dbPhySet6DOFJointDriveLinearVelocity ( int iJoint, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->driveLinearVelocity = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySet6DOFJointDriveAngularVelocity ( int iJoint, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->driveAngularVelocity = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySet6DOFJointProjection ( int iJoint, int iMode, float fDistance )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->projectionMode     = NX_JPM_POINT_MINDIST;
		pJoint->pD6Desc->projectionDistance = ( NxReal ) fDistance;
	}
}

void dbPhySet6DOFJointSlerpDriveState ( int iJoint, int iState )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->flags |= NX_D6JOINT_SLERP_DRIVE;
	}
}

void dbPhySet6DOFJointGear ( int iJoint, int iState )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		pJoint->pD6Desc->flags |= NX_D6JOINT_GEAR_ENABLED;
	}
}

void dbPhyBuild6DOFJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGet6DOFJoint ( iJoint, -1, false ) )
	{
		if ( !pJoint->pD6Desc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "6DOF joint configuration is invalid", "", 0 );
			return;
		}


		pJoint->pD6Joint = ( NxD6Joint* ) PhysX::pScene->createJoint ( *pJoint->pD6Desc );
		pJoint->pJoint   = pJoint->pD6Joint;
	}
	else
	{
		return;
	}
}

bool dbPhySetup6DOFJoint ( int iJoint, int iA, int iB )
{
	// set up a 6dof joint

	// first determine if any joint with this ID exists
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		// joint already exists and with IDs being shared this is invalid
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up 6DOF joint as ID is already in use", "", 0 );
		return false;
	}

	// create memory for the joint
	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	// check memory is allocated
	if ( !pJoint )
	{
		// failure so display an error message
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for 6DOF joint", "", 0 );
		return false;
	}

	// store joint data
	pJoint->iID  = iJoint;
	pJoint->type = PhysX::e6DOFJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	// check that the objects are valid
	if ( !pJoint->pA || !pJoint->pB )
	{
		// invalid so display error message
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for 6DOF joint", "", 0 );
		return false;
	}

	// send joint to back of list
	PhysX::pJointList.push_back ( pJoint );

	// all sorted out
	return true;
}
