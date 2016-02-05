
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "rigidbodyfixedjoints.h"
#include "error.h"


/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE FIXED JOINT%LLL%?dbPhyMakeFixedJoint@@YAXHHH@Z%
	PHY MAKE FIXED JOINT%LLLL%?dbPhyMakeFixedJoint@@YAXHHHH@Z%

	PHY BUILD FIXED JOINT%L%?dbPhyBuildFixedJoint@@YAXH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void dbPhyMakeFixedJoint ( int iJoint, int iA, int iB )
{
	// make a regular fixed joint
	dbPhyMakeFixedJoint  ( iJoint, iA, iB, 0 );
	dbPhyBuildFixedJoint ( iJoint );
}

void dbPhyMakeFixedJoint ( int iJoint, int iA, int iB, int iExtras )
{
	// starting point of making a fixed joint, must be built after

	// set up a fixed joint
	if ( !dbPhySetupFixedJoint ( iJoint, iA, iB ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up fixed joint", "", 0 );
		return;
	}
	
	// get the joint
	PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eFixedJoint );

	// if joint is invalid then display an error message
	if ( !pJoint )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Fixed joint does not exist", "", 0 );
		return;
	}

	// create a new fixed joint description
	pJoint->pFixedDesc = new NxFixedJointDesc;

	// check memory
	if ( !pJoint->pFixedDesc )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to allocate memory for fixed joint", "", 0 );
		return;
	}

	// set to default values
	pJoint->pFixedDesc->setToDefault ( );

	// store the first actor data
	if ( pJoint->pA->pActorList.size ( ) > 0 )
		pJoint->pFixedDesc->actor [ 0 ] = pJoint->pA->pActorList [ 0 ];

	// store the second actor data
	if ( pJoint->pB->pActorList.size ( ) > 0 )
		pJoint->pFixedDesc->actor [ 1 ] = pJoint->pB->pActorList [ 0 ];

	// see if first actor is static
	if ( pJoint->pA->state == PhysX::eStatic )
		pJoint->pFixedDesc->actor [ 0 ] = NULL;

	// see if second actor is static
	if ( pJoint->pB->state == PhysX::eStatic )
		pJoint->pFixedDesc->actor [ 1 ] = NULL;
}

void dbPhyBuildFixedJoint ( int iJoint )
{
	// build a fixed joint

	// get the joint first
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eFixedJoint ) )
	{
		// return if joint has already been built
		if ( pJoint->pJoint )
			return;

		// check it is valid
		if ( !pJoint->pFixedDesc->isValid ( ) )
		{
			PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Fixed joint configuration is invalid", "", 0 );
			return;
		}

		// now make the joint
		pJoint->pFixedJoint = ( NxFixedJoint* ) PhysX::pScene->createJoint ( *pJoint->pFixedDesc );
		pJoint->pJoint      = pJoint->pFixedJoint;
	}
	else
	{
		// report an error
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot build fixed joint", "", 0 );
		return;
	}
}

bool dbPhySetupFixedJoint ( int iJoint, int iA, int iB )
{
	// initial set up for a fixed joint

	// first determine if any joint with this ID exists
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint ) )
	{
		// joint already exists and with IDs being shared this is invalid
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to set up fixed joint as ID is already in use", "", 0 );
		return false;
	}

	// create memory for the joint
	PhysX::sPhysXJoint* pJoint = new PhysX::sPhysXJoint;

	// check if joint is allocated
	if ( !pJoint )
	{
		// failure so display an error message
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Cannot allocate memory for fixed joint", "", 0 );
		return false;
	}

	memset ( pJoint, 0, sizeof ( PhysX::sPhysXJoint ) );

	// store joint data
	pJoint->iID  = iJoint;
	pJoint->type = PhysX::eFixedJoint;
	pJoint->pA	 = dbPhyGetObject ( iA );
	pJoint->pB	 = dbPhyGetObject ( iB );

	// check that the objects are valid
	if ( !pJoint->pA || !pJoint->pB )
	{
		// invalid so display error message
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid objects used for fixed joint", "", 0 );
		return false;
	}

	// send this joint to the back of the list
	PhysX::pJointList.push_back ( pJoint );

	// all done lets return true
	return true;
}
