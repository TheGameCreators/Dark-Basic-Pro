
#include "globals.h"
#include "rigidbodycreation.h"
#include "rigidbodyjoints.h"
#include "error.h"

/*
	struct sPhysXJoint
	{
		int		   iID;
		eJointType type;

		union joint
		{
			NxSphericalJoint*		pSphereJoint;
			NxRevoluteJoint*		pRevoluteJoint;
			NxPrismaticJoint*		pPrismaticJoint;
			NxCylindricalJoint*		pClyindricalJoint;
			NxFixedJoint*			pFixedJoint;
			NxDistanceJoint*		pDistanceJoint;
			NxPointInPlaneJoint*	pPointInPlaneJoint;
			NxPointOnLineJoint*		pPointOnLineJoint;
			NxPulleyJoint*			pPulleyJoint;
		};
	};
*/

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY DELETE JOINT%L%?dbPhyDeleteJoint@@YAXH@Z%
	PHY GET JOINT EXIST[%LL%?dbPhyGetJointExist@@YAHH@Z%
	PHY GET JOINT TYPE[%LL%?dbPhyGetJointType@@YAHH@Z%
	PHY SET JOINT BREAK LIMITS%LFF%?dbPhySetJointBreakLimits@@YAXHMM@Z%

	PHY GET JOINT STATE[%LL%?dbPhyGetJointState@@YAHH@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


namespace PhysXJoints
{
	PhysX::sPhysXObject*	pA;
	PhysX::sPhysXObject*	pB;
	PhysX::sPhysXJoint*		pJoint;
};

void dbPhyClearJoints ( void )
{
	// delete a single joint in a scene
	if ( PhysX::pScene == NULL )
		return;

	// go through all objects
	for ( DWORD dwJoint = 0; dwJoint < PhysX::pJointList.size ( ); dwJoint++ )
	{
		// see if we have a match
		//if ( PhysX::pJointList [ dwJoint ]->iID == iJoint )
		{
			// get a pointer to our object
			PhysX::sPhysXJoint* pJoint = PhysX::pJointList [ dwJoint ];

			if ( pJoint->pJoint )
			{
				// release the joint
				PhysX::pScene->releaseJoint ( *pJoint->pJoint );
				
				// erase the item in the list
				PhysX::pJointList.erase ( PhysX::pJointList.begin ( ) + dwJoint );

				// clear up the memory
				delete pJoint;
				pJoint = NULL;
			}

			// break out
			break;
		}
	}
}

bool dbPhySetupJoint ( int iJoint, int iA, int iB, NxJointDesc* pDescription )
{
	if ( dbPhyCheckJoint ( iJoint ) )
		return false;

	PhysXJoints::pA		= dbPhyGetObject ( iA );
	PhysXJoints::pB		= dbPhyGetObject ( iB );
	PhysXJoints::pJoint = new PhysX::sPhysXJoint;

	if ( !PhysXJoints::pA || !PhysXJoints::pB )
		return false;

	if ( !PhysXJoints::pJoint )
		return false;

	pDescription->actor [ 0 ] = PhysXJoints::pA->pActorList [ 0 ];
	pDescription->actor [ 1 ] = PhysXJoints::pB->pActorList [ 0 ];

	if ( PhysXJoints::pA->state == PhysX::eStatic )
		pDescription->actor [ 0 ] = NULL;

	if ( PhysXJoints::pB->state == PhysX::eStatic )
		pDescription->actor [ 1 ] = NULL;

	return true;
}

void dbPhySetJointBreakLimits ( int iJoint, float fMaxForce, float fMaxTorque )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint ) )
	{
		pJoint->pJoint->setBreakable ( fMaxForce, fMaxTorque );
	}
}

bool dbPhyCheckJoint ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint ) )
	{
		return true;
	}

	return false;
}

int dbPhyGetJointState ( int iJoint )
{
	/*
	NX_JS_UNBOUND = 0;
	NX_JS_SIMULATING = 1;
	NX_JS_BROKEN = 2;
	*/

	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint, PhysX::eUnknownJoint, true ) )
		return pJoint->pJoint->getState ( );

	return 0;
}

PhysX::sPhysXJoint* dbPhyGetJoint ( int iID, PhysX::eJointType eJoint, bool bDisplayError )
{
	for ( DWORD dwJoint = 0; dwJoint < PhysX::pJointList.size ( ); dwJoint++ )
	{
		if ( PhysX::pJointList [ dwJoint ]->iID == iID )
		{
			if ( eJoint == PhysX::eUnknownJoint )
				return PhysX::pJointList [ dwJoint ];

			if ( eJoint == PhysX::pJointList [ dwJoint ]->type )
				return PhysX::pJointList [ dwJoint ];
		}
	}

	if ( bDisplayError )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Joint with specified ID does not exist", "", 0 );
		return NULL;
	}

	return NULL;
}

void dbPhyDeleteJoint ( int iJoint )
{
	// delete a single joint in a scene
	if ( PhysX::pScene == NULL )
		return;

	// go through all objects
	for ( DWORD dwJoint = 0; dwJoint < PhysX::pJointList.size ( ); dwJoint++ )
	{
		// see if we have a match
		if ( PhysX::pJointList [ dwJoint ]->iID == iJoint )
		{
			// get a pointer to our object
			PhysX::sPhysXJoint* pJoint = PhysX::pJointList [ dwJoint ];

			if ( pJoint->pJoint )
			{
				/*
				if ( pJoint->type == PhysX::eFixedJoint )
				{
					delete pJoint->pFixedDesc;
					pJoint->pFixedDesc = NULL;

					pJoint->pFixedJoint->setBreakable ( 0.0f, 0.0f );

					PhysX::pScene->releaseJoint ( *pJoint->pFixedJoint );

					pJoint->pFixedJoint = NULL; 
				}
				*/

				// release the joint
				PhysX::pScene->releaseJoint ( *pJoint->pJoint );
				
				// erase the item in the list
				PhysX::pJointList.erase ( PhysX::pJointList.begin ( ) + dwJoint );

				// clear up the memory
				delete pJoint;
				pJoint = NULL;
			}

			// break out
			break;
		}
	}
}

int dbPhyGetJointExist ( int iJoint )
{
	if ( PhysX::sPhysXJoint* pJoint = dbPhyGetJoint ( iJoint ) )
		return 1;

	return 0;
}

int dbPhyGetJointType ( int iJoint )
{
	return 1;
}