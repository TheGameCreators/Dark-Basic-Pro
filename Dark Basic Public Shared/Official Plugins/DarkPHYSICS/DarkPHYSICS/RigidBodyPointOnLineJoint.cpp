
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbodypointonlinejoint.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE POINT ON LINE JOINT%LLLFFFFFF%?dbPhyMakePointOnLineJoint@@YAXHHHMMMMMM@Z%
	PHY MAKE POINT ON LINE JOINT%LLL%?dbPhyMakePointOnLineJoint@@YAXHHH@Z%

	PHY SET POINT ON LINE JOINT LOCAL ANCHOR%LLFFF%?dbPhySetPointOnLineJointLocalAnchor@@YAXHHMMM@Z%
	PHY SET POINT ON LINE JOINT LOCAL AXIS%LLFFF%?dbPhySetPointOnLineJointLocalAxis@@YAXHHMMM@Z%
	PHY SET POINT ON LINE JOINT GLOBAL ANCHOR%LLFFF%?dbPhySetPointOnLineJointGlobalAnchor@@YAXHHMMM@Z%
	PHY SET POINT ON LINE JOINT GLOBAL AXIS%LLFFF%?dbPhySetPointOnLineJointGlobalAxis@@YAXHHMMM@Z%

	PHY BUILD POINT ON LINE JOINT%L%?dbPhyBuildPointOnLineJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakePointOnLineJoint ( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ )
{
	dbPhyMakePointOnLineJoint ( iJoint, iA, iB );

	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointOnLineJoint ) )
	{
		pJoint->pPointOnLineDesc->setGlobalAxis   ( NxVec3 ( fAxisX, fAxisY, fAxisZ ) );
		pJoint->pPointOnLineDesc->setGlobalAnchor ( NxVec3 ( fAnchorX, fAnchorY, fAnchorZ ) );

		dbPhyBuildPointOnLineJoint ( iJoint );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up point on line joint", "", 0 );
		return;
	}
}

void dbPhyMakePointOnLineJoint ( int iJoint, int iA, int iB )
{
	if ( !dbPhySetupPointOnLineJoint ( iJoint, iA, iB ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up point on line joint", "", 0 );
		return;
	}

	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointOnLineJoint );

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Point on line joint does not exist", "", 0 );
		return;
	}

	pJoint->pPointOnLineDesc = new NxPointOnLineJointDesc;

	// check memory
	if ( !pJoint->pPointOnLineDesc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for point on line joint", "", 0 );
		return;
	}

	pJoint->pPointOnLineDesc->setToDefault ( );

	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pPointOnLineDesc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pPointOnLineDesc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pPointOnLineDesc->actor [ 0 ] = NULL;

	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pPointOnLineDesc->actor [ 1 ] = NULL;
}

void dbPhySetPointOnLineJointLocalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointOnLineJoint ) )
	{
		pJoint->pPointOnLineDesc->localAnchor [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetPointOnLineJointLocalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointOnLineJoint ) )
	{
		pJoint->pPointOnLineDesc->localAxis [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetPointOnLineJointGlobalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointOnLineJoint ) )
	{
		pJoint->pPointOnLineDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetPointOnLineJointGlobalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointOnLineJoint ) )
	{
		pJoint->pPointOnLineDesc->setGlobalAxis ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhyBuildPointOnLineJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePointOnLineJoint ) )
	{
		if ( !pJoint->pPointOnLineDesc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for point on line joint", "", 0 );
			return;
		}

		pJoint->pPointOnLineJoint = ( NxPointOnLineJoint* ) PhysX::pScene->createJoint ( *pJoint->pPointOnLineDesc );
		pJoint->pJoint            = pJoint->pPointOnLineJoint;
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to build point on line joint", "", 0 );
		return;
	}
}

bool dbPhySetupPointOnLineJoint ( int iJoint, int iA, int iB )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up point on line joint as ID is already in use", "", 0 );
		return false;
	}

	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for point on line joint", "", 0 );
		return false;
	}

	pJoint->iID  = iJoint;
	pJoint->type = PhysX::ePointOnLineJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	if ( !pJoint->pA || !pJoint->pB )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for point on line joint", "", 0 );
		return false;
	}

	PhysX::pJointList.push_back ( pJoint );

	return true;
}
