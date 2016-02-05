
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbodyprismaticjoints.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE PRISMATIC JOINT%LLLFFFFFF%?dbPhyMakePrismaticJoint@@YAXHHH@Z%
	PHY MAKE PRISMATIC JOINT%LLL%?dbPhyMakePrismaticJoint@@YAXHHH@Z%

	PHY SET PRIMSATIC JOINT LOCAL ANCHOR%LLFFF%?dbPhySetPrismaticJointLocalAnchor@@YAXHHMMM@Z%
	PHY SET PRIMSATIC JOINT LOCAL AXIS%LLFFF%?dbPhySetPrismaticJointLocalAxis@@YAXHHMMM@Z%
	PHY SET PRIMSATIC JOINT GLOBAL ANCHOR%LLFFF%?dbPhySetPrismaticJointGlobalAnchor@@YAXHHMMM@Z%
	PHY SET PRIMSATIC JOINT GLOBAL AXIS%LLFFF%?dbPhySetPrismaticJointGlobalAxis@@YAXHHMMM@Z%

	PHY BUILD PRIMSATIC JOINT%L%?dbPhyBuildPrismaticJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakePrismaticJoint ( int iJoint, int iA, int iB, float fAnchorX, float fAnchorY, float fAnchorZ, float fAxisX, float fAxisY, float fAxisZ )
{
	dbPhyMakePrismaticJoint ( iJoint, iA, iB );

	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePrismaticJoint ) )
	{
		pJoint->pPrismaticDesc->setGlobalAxis   ( NxVec3 ( fAxisX, fAxisY, fAxisZ ) );
		pJoint->pPrismaticDesc->setGlobalAnchor ( NxVec3 ( fAnchorX, fAnchorY, fAnchorZ ) );

		dbPhyBuildPrismaticJoint ( iJoint );
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up prismatic joint", "", 0 );
		return;
	}
}

void dbPhyMakePrismaticJoint ( int iJoint, int iA, int iB )
{
	if ( !dbPhySetupPrismaticJoint ( iJoint, iA, iB ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up prismatic joint", "", 0 );
		return;
	}

	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint );

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Prismatic joint does not exist", "", 0 );
		return;
	}

	pJoint->pPrismaticDesc = new NxPrismaticJointDesc;

	// check memory
	if ( !pJoint->pPrismaticDesc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for prismatic joint", "", 0 );
		return;
	}

	pJoint->pPrismaticDesc->setToDefault ( );

	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pPrismaticDesc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pPrismaticDesc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pPrismaticDesc->actor [ 0 ] = NULL;

	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pPrismaticDesc->actor [ 1 ] = NULL;
}

void dbPhySetPrismaticJointLocalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePrismaticJoint ) )
	{
		pJoint->pPrismaticDesc->localAnchor [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetPrismaticJointLocalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePrismaticJoint ) )
	{
		pJoint->pPrismaticDesc->localAxis [ iIndex ] = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetPrismaticJointGlobalAnchor ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePrismaticJoint ) )
	{
		pJoint->pPrismaticDesc->setGlobalAnchor ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetPrismaticJointGlobalAxis ( int iJoint, int iIndex, float fX, float fY, float fZ )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePrismaticJoint ) )
	{
		pJoint->pPrismaticDesc->setGlobalAxis ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhyBuildPrismaticJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::ePrismaticJoint ) )
	{
		if ( !pJoint->pPrismaticDesc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Prismatic joint configuration is invalid", "", 0 );
			return;
		}

		pJoint->pPrismaticJoint = ( NxPrismaticJoint* ) PhysX::pScene->createJoint ( *pJoint->pPrismaticDesc );
		pJoint->pJoint          = pJoint->pPrismaticJoint;
	}
	else
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot build prismatic joint", "", 0 );
		return;
	}
}

bool dbPhySetupPrismaticJoint ( int iJoint, int iA, int iB )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up prismatic joint as ID is already in use", "", 0 );
		return false;
	}

	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for prismatic joint", "", 0 );
		return false;
	}

	pJoint->iID  = iJoint;
	pJoint->type = PhysX::ePrismaticJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	if ( !pJoint->pA || !pJoint->pB )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for prismatic joint", "", 0 );
		return false;
	}

	PhysX::pJointList.push_back ( pJoint );

	return true;
}
