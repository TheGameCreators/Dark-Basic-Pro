
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbodypointinplanejoint.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE POINT IN PLANE JOINT%LLLFFFFFF%?dbPhyMakePointInPlaneJoint@@YAXHHHMMMMMM@Z%
	PHY MAKE POINT IN PLANE JOINT%LLL%?dbPhyMakePointInPlaneJoint@@YAXHHH@Z%

	PHY SET POINT IN PLANE LOCAL ANCHOR%LLFFF%?dbPhySetPointInPlaneJointLocalAnchor@@YAXHHMMM@Z%
	PHY SET POINT IN PLANE LOCAL AXIS%LLFFF%?dbPhySetPointInPlaneJointLocalAxis@@YAXHHMMM@Z%
	PHY SET POINT IN PLANE GLOBAL ANCHOR%LLFFF%?dbPhySetPointInPlaneJointGlobalAnchor@@YAXHHMMM@Z%
	PHY SET POINT IN PLANE GLOBAL AXIS%LLFFF%?dbPhySetPointInPlaneJointGlobalAxis@@YAXHHMMM@Z%

	PHY BUILD POINT IN PLANE JOINT%L%?dbPhyBuildPointInPlaneJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakePointInPlaneJoint ( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ )
{
	dbPhyMakePointInPlaneJoint ( iJoint, iA, iB );

	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointInPlaneJoint ) )
	{
		pJoint->pPointInPlaneDesc->setGlobalAxis   ( NxVec3 ( fAxisX, fAxisY, fAxisZ ) );
		pJoint->pPointInPlaneDesc->setGlobalAnchor ( NxVec3 ( fAnchorX, fAnchorY, fAnchorZ ) );

		dbPhyBuildPointInPlaneJoint ( iJoint );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to make point in plane joint", "", 0 );
		return;
	}
}

void dbPhyMakePointInPlaneJoint ( int iJoint, int iA, int iB )
{
	if ( !dbPhySetupPointInPlaneJoint ( iJoint, iA, iB ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up point in plane joint", "", 0 );
		return;
	}

	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointInPlaneJoint );

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Point in plane joint does not exist", "", 0 );
		return;
	}

	pJoint->pPointInPlaneDesc = new NxPointInPlaneJointDesc;

	// check memory
	if ( !pJoint->pPointInPlaneDesc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for point in plane joint", "", 0 );
		return;
	}

	pJoint->pPointInPlaneDesc->setToDefault ( );

	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pPointInPlaneDesc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pPointInPlaneDesc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pPointInPlaneDesc->actor [ 0 ] = NULL;

	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pPointInPlaneDesc->actor [ 1 ] = NULL;
}

void dbPhySetPointInPlaneJointLocalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointInPlaneJoint ) )
	{
		pJoint->pPointInPlaneDesc->localAnchor [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetPointInPlaneJointLocalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointInPlaneJoint ) )
	{
		pJoint->pPointInPlaneDesc->localAxis [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetPointInPlaneJointGlobalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointInPlaneJoint ) )
	{
		pJoint->pPointInPlaneDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetPointInPlaneJointGlobalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointInPlaneJoint ) )
	{
		pJoint->pPointInPlaneDesc->setGlobalAxis ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhyBuildPointInPlaneJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointInPlaneJoint ) )
	{
		if ( !pJoint->pPointInPlaneDesc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Point in plane joint configuration is invalid", "", 0 );
			return;
		}

		pJoint->pPointInPlaneJoint = ( NxPointInPlaneJoint* ) PhysX::pScene->createJoint ( *pJoint->pPointInPlaneDesc );
		pJoint->pJoint             = pJoint->pPointInPlaneJoint;
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot build point in plane joint", "", 0 );
		return;
	}
}

bool dbPhySetupPointInPlaneJoint ( int iJoint, int iA, int iB )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up point in plane joint as ID is already in use", "", 0 );
		return false;
	}

	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for point in plane joint", "", 0 );
		return false;
	}

	pJoint->iID  = iJoint;
	pJoint->type = PhysX::ePointInPlaneJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	if ( !pJoint->pA || !pJoint->pB )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for point in plane joint", "", 0 );
		return false;
	}

	PhysX::pJointList.push_back ( pJoint );

	return true;
}
