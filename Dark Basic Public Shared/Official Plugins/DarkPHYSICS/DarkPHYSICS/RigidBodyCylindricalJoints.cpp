
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbodycylindricaljoints.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE CYLINDRICAL JOINT%LLLFFFFFF%?dbPhyMakeCylindricalJoint@@YAXHHHMMMMMM@Z%
	PHY MAKE CYLINDRICAL JOINT%LLL%?dbPhyMakeCylindricalJoint@@YAXHHH@Z%

	PHY SET CYLINDRICAL JOINT LOCAL ANCHOR%LLFFF%?dbPhySetCylindricalJointLocalAnchor@@YAXHHMMM@Z%
	PHY SET CYLINDRICAL JOINT LOCAL AXIS%LLFFF%?dbPhySetCylindricalJointLocalAxis@@YAXHHMMM@Z%
	PHY SET CYLINDRICAL JOINT GLOBAL ANCHOR%LLFFF%?dbPhySetCylindricalJointGlobalAnchor@@YAXHHMMM@Z%
	PHY SET CYLINDRICAL JOINT GLOBAL AXIS%LLFFF%?dbPhySetCylindricalJointGlobalAxis@@YAXHHMMM@Z%

	PHY BUILD CYLINDRICAL JOINT%L%?dbPhyBuildCylindricalJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeCylindricalJoint ( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ )
{
	dbPhyMakeCylindricalJoint ( iJoint, iA, iB );

	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eCylindricalJoint ) )
	{
		pJoint->pCylindricalDesc->setGlobalAxis   ( NxVec3 ( fAxisX, fAxisY, fAxisZ ) );
		pJoint->pCylindricalDesc->setGlobalAnchor ( NxVec3 ( fAnchorX, fAnchorY, fAnchorZ ) );

		dbPhyBuildCylindricalJoint ( iJoint );

		NxVec3 axis   ( fAxisX,   fAxisY,   fAxisZ );
		NxVec3 anchor ( fAnchorX, fAnchorY, fAnchorZ );

		pJoint->pCylindricalJoint->setLimitPoint ( anchor );
		pJoint->pCylindricalJoint->addLimitPlane ( -axis, anchor + 1.5 * axis );
		pJoint->pCylindricalJoint->addLimitPlane (  axis, anchor - 1.5 * axis );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cylindrical joint does not exist", "", 0 );
		return;
	}
}

void dbPhyMakeCylindricalJoint ( int iJoint, int iA, int iB )
{
	if ( !dbPhySetupCylindricalJoint ( iJoint, iA, iB ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up cylindrical joint", "", 0 );
		return;
	}

	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eCylindricalJoint );

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cylindrical joint does not exist", "", 0 );
		return;
	}

	pJoint->pCylindricalDesc = new NxCylindricalJointDesc;

	// check memory
	if ( !pJoint->pCylindricalDesc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for cylindrical joint", "", 0 );
		return;
	}

	pJoint->pCylindricalDesc->setToDefault ( );

	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pCylindricalDesc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pCylindricalDesc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pCylindricalDesc->actor [ 0 ] = NULL;

	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pCylindricalDesc->actor [ 1 ] = NULL;
}

void dbPhySetCylindricalJointLocalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eCylindricalJoint ) )
	{
		pJoint->pCylindricalDesc->localAnchor [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetCylindricalJointLocalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eCylindricalJoint ) )
	{
		pJoint->pCylindricalDesc->localAxis [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetCylindricalJointGlobalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eCylindricalJoint ) )
	{
		pJoint->pCylindricalDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetCylindricalJointGlobalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eCylindricalJoint ) )
	{
		pJoint->pCylindricalDesc->setGlobalAxis ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhyBuildCylindricalJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eCylindricalJoint ) )
	{
		if ( !pJoint->pCylindricalDesc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cylindrical joint is invalid", "", 0 );
			return;
		}

		pJoint->pCylindricalJoint = ( NxCylindricalJoint* ) PhysX::pScene->createJoint ( *pJoint->pCylindricalDesc );
		pJoint->pJoint            = pJoint->pCylindricalJoint;
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cylindrical joint does not exist", "", 0 );
		return;
	}
}

bool dbPhySetupCylindricalJoint ( int iJoint, int iA, int iB )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up cylindrical joint as ID is already in use", "", 0 );
		return false;
	}

	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for cylindrical joint", "", 0 );
		return false;
	}

	pJoint->iID  = iJoint;
	pJoint->type = PhysX::eCylindricalJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	if ( !pJoint->pA || !pJoint->pB )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for cylindrical joint", "", 0 );
		return false;
	}

	PhysX::pJointList.push_back ( pJoint );

	return true;
}
