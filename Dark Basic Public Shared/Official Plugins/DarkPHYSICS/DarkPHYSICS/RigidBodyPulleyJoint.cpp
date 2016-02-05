
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbodypulleyjoint.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE PULLEY JOINT%LLLFFFFFFFFFFFFFFF%?dbPhyMakePulleyJoint@@YAXHHHMMMMMMMMMMMMMMM@Z%
	PHY MAKE PULLEY JOINT%LLL%?dbPhyMakePulleyJoint@@YAXHHH@Z%

	PHY SET PULLEY JOINT LOCAL ANCHOR%LLFFF%?dbPhySetPulleyJointLocalAnchor@@YAXHHMMM@Z%
	PHY SET PULLEY JOINT GLOBAL ANCHOR%LLFFF%?dbPhySetPulleyJointGlobalAnchor@@YAXHHMMM@Z%
	PHY SET PULLEY JOINT LOCAL AXIS%LLFFF%?dbPhySetPulleyJointLocalAxis@@YAXHHMMM@Z%
	PHY SET PULLEY JOINT GLOBAL AXIS%LLFFF%?dbPhySetPulleyJointGlobalAxis@@YAXHHMMM@Z%

	PHY SET PULLEY JOINT SUSPENSION%LLFFF%?dbPhySetPulleyJointSuspension@@YAXHHMMM@Z%
	PHY SET PULLEY JOINT DISTANCE%LF%?dbPhySetPulleyJointDistance@@YAXHM@Z%
	PHY SET PULLEY JOINT STIFFNESS%LF%?dbPhySetPulleyJointStiffness@@YAXHM@Z%
	PHY SET PULLEY JOINT RATIO%LF%?dbPhySetPulleyJointRatio@@YAXHM@Z%
	PHY SET PULLEY JOINT RIGID%LL%?dbPhySetPulleyJointRigid@@YAXHH@Z%
	PHY SET PULLEY JOINT MOTOR%LL%?dbPhySetPulleyJointMotor@@YAXHH@Z%

	PHY BUILD PULLEY JOINT%L%?dbPhyBuildPulleyJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void dbPhyMakePulleyJoint ( int iJoint, int iA, int iB, float fX1, float fY1, float fZ1, float fX2, float fY2, float fZ2, float fX3, float fY3, float fZ3, float fX4, float fY4, float fZ4, float fDistance, float fStiffness, float fRatio )
{
	dbPhyMakePulleyJoint ( iJoint, iA, iB );

	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		dbPhyBuildPulleyJoint ( iJoint );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up pulley joint", "", 0 );
		return;
	}
}

void dbPhyMakePulleyJoint ( int iJoint, int iA, int iB )
{
	if ( !dbPhySetupPulleyJoint ( iJoint, iA, iB ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up pulley joint", "", 0 );
		return;
	}

	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint );

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Pulley joint does not exist", "", 0 );
		return;
	}

	pJoint->pPulleyDesc = new NxPulleyJointDesc;

	// check memory
	if ( !pJoint->pPulleyDesc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for pulley joint", "", 0 );
		return;
	}

	pJoint->pPulleyDesc->setToDefault ( );

	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pPulleyDesc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pPulleyDesc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pPulleyDesc->actor [ 0 ] = NULL;

	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pPulleyDesc->actor [ 1 ] = NULL;
}

void dbPhySetPulleyJointLocalAnchor	( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->localAnchor [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetPulleyJointGlobalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetPulleyJointLocalAxis	( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->localAxis [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetPulleyJointGlobalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->setGlobalAxis ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetPulleyJointSuspension ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->pulley [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetPulleyJointDistance ( int iJoint, float fDistance )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->distance = ( NxReal ) fDistance;
	}
}

void dbPhySetPulleyJointStiffness ( int iJoint, float fStiffness )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->stiffness = ( NxReal ) fStiffness;
	}
}

void dbPhySetPulleyJointRatio ( int iJoint, float fRatio )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->ratio = ( NxReal ) fRatio;
	}
}

void dbPhySetPulleyJointRigid ( int iJoint, int iState )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->flags |= NX_PJF_IS_RIGID;
	}
}

void dbPhySetPulleyJointMotor ( int iJoint, int iState )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		pJoint->pPulleyDesc->flags |=  NX_PJF_MOTOR_ENABLED;
	}
}

void dbPhyBuildPulleyJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePulleyJoint ) )
	{
		if ( !pJoint->pPulleyDesc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Pulley joint configuration is invalid", "", 0 );
			return;
		}

		pJoint->pPulleyJoint = ( NxPulleyJoint* ) PhysX::pScene->createJoint ( *pJoint->pPulleyDesc );
		pJoint->pJoint       = pJoint->pPulleyJoint;
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot build pulley joint", "", 0 );
		return;
	}
}

bool dbPhySetupPulleyJoint ( int iJoint, int iA, int iB )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up pulley joint as ID is already in use", "", 0 );
		return false;
	}

	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for pulley joint", "", 0 );
		return false;
	}

	pJoint->iID  = iJoint;
	pJoint->type = PhysX::ePulleyJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	if ( !pJoint->pA || !pJoint->pB )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for pulley joint", "", 0 );
		return false;
	}

	PhysX::pJointList.push_back ( pJoint );

	return true;
}
