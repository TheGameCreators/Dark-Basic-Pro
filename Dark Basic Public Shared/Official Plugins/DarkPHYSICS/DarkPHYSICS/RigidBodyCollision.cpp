
#include "globals.h"
#include "rigidbodycollision.h"
#include "rigidbodycreation.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY GET COLLISION DATA[%L%?dbPhyGetCollisionData@@YAHXZ%
	PHY GET COLLISION OBJECT A[%L%?dbPhyGetCollisionObjectA@@YAHXZ%
	PHY GET COLLISION OBJECT B[%L%?dbPhyGetCollisionObjectB@@YAHXZ%

	PHY GET COLLISION NORMAL FORCE X[%F%?dbPhyGetCollisionNormalForceXEx@@YAKXZ%
	PHY GET COLLISION NORMAL FORCE Y[%F%?dbPhyGetCollisionNormalForceYEx@@YAKXZ%
	PHY GET COLLISION NORMAL FORCE Z[%F%?dbPhyGetCollisionNormalForceZEx@@YAKXZ%
	PHY GET COLLISION FRICTION FORCE X[%F%?dbPhyGetCollisionFrictionForceXEx@@YAKXZ%
	PHY GET COLLISION FRICTION FORCE Y[%F%?dbPhyGetCollisionFrictionForceYEx@@YAKXZ%
	PHY GET COLLISION FRICTION FORCE Z[%F%?dbPhyGetCollisionFrictionForceZEx@@YAKXZ%


	PHY GET COLLISION[%LLL%?dbPhyGetCollision@@YAHHH@Z%
	PHY GET COLLISION COUNT[%LL%?dbPhyGetCollisionCount@@YAHH@Z%
	PHY GET COLLISION FRICTION FORCE X[%FLL%?dbPhyGetCollisionFrictionForceXEx@@YAKHH@Z%
	PHY GET COLLISION FRICTION FORCE Y[%FLL%?dbPhyGetCollisionFrictionForceYEx@@YAKHH@Z%
	PHY GET COLLISION FRICTION FORCE Z[%FLL%?dbPhyGetCollisionFrictionForceZEx@@YAKHH@Z%
	PHY GET COLLISION NORMAL FORCE X[%FLL%?dbPhyGetCollisionNormalForceXEx@@YAKHH@Z%
	PHY GET COLLISION NORMAL FORCE Y[%FLL%?dbPhyGetCollisionNormalForceYEx@@YAKHH@Z%
	PHY GET COLLISION NORMAL FORCE Z[%FLL%?dbPhyGetCollisionNormalForceZEx@@YAKHH@Z%
	PHY GET COLLISION CONTACT NORMAL X[%FLL%?dbPhyGetCollisionContactNormalXEx@@YAKHH@Z%
	PHY GET COLLISION CONTACT NORMAL Y[%FLL%?dbPhyGetCollisionContactNormalYEx@@YAKHH@Z%
	PHY GET COLLISION CONTACT NORMAL Z[%FLL%?dbPhyGetCollisionContactNormalZEx@@YAKHH@Z%
	PHY GET COLLISION CONTACT POINT X[%FLL%?dbPhyGetCollisionContactPointXEx@@YAKHH@Z%
	PHY GET COLLISION CONTACT POINT Y[%FLL%?dbPhyGetCollisionContactPointYEx@@YAKHH@Z%
	PHY GET COLLISION CONTACT POINT Z[%FLL%?dbPhyGetCollisionContactPointZEx@@YAKHH@Z%

	PHY SET COLLISION STATE%LL%?dbPhySetCollisionState@@YAXHH@Z%

	*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

int	dbPhyGetCollisionData ( void )
{
	// this function returns true if collision data is on the stack,
	// if this is true it then takes the top item off the stack to
	// be checked by the programmer and then pops it off the stack

	// do we have any data
	if ( PhysX::CollisionStack.size ( ) )
	{
		// clear collision data
		memset ( &PhysX::CollisionData, 0, sizeof ( PhysX::CollisionData ) );

		// get top item
		PhysX::CollisionData = PhysX::CollisionStack.top ( );

		// pop item off stack
		PhysX::CollisionStack.pop ( );

		// return true
		return 1;
	}

	// no data, return false
	return 0;
}

int	dbPhyGetCollisionObjectA ( void )
{
	// return object A involved in collision
	return PhysX::CollisionData.iObjectA;
}

int	dbPhyGetCollisionObjectB ( void )
{
	// return object B involved in collision
	return PhysX::CollisionData.iObjectB;
}

float dbPhyGetCollisionNormalForceX ( void )
{
	return PhysX::CollisionData.fNormalForceX;
}

float dbPhyGetCollisionNormalForceY	( void )
{
	return PhysX::CollisionData.fNormalForceY;
}

float dbPhyGetCollisionNormalForceZ	( void )
{
	return PhysX::CollisionData.fNormalForceZ;
}

float dbPhyGetCollisionFrictionForceX ( void )
{
	return PhysX::CollisionData.fFrictionForceX;
}

float dbPhyGetCollisionFrictionForceY ( void )
{
	return PhysX::CollisionData.fFrictionForceY;
}

float dbPhyGetCollisionFrictionForceZ ( void )
{
	return PhysX::CollisionData.fFrictionForceZ;
}

DWORD dbPhyGetCollisionNormalForceXEx ( void )
{
	float fValue = dbPhyGetCollisionNormalForceX ( );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionNormalForceYEx ( void )
{
	float fValue = dbPhyGetCollisionNormalForceY ( );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionNormalForceZEx ( void )
{
	float fValue = dbPhyGetCollisionNormalForceZ ( );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionFrictionForceXEx	( void )
{
	float fValue = dbPhyGetCollisionFrictionForceX ( );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionFrictionForceYEx	( void )
{
	float fValue = dbPhyGetCollisionFrictionForceY ( );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionFrictionForceZEx	( void )
{
	float fValue = dbPhyGetCollisionFrictionForceZ ( );
	return *( DWORD* ) &fValue;
}

void dbPhyGetWheelAddContactPoints ( int iObject, PhysX::CarWheelContact contact );

void PhysX::cContactReport::onContactNotify ( NxContactPair& pair, NxU32 events )
{
	//return;

	// the contact report will be called when two objects collide with each other

	// set up our variables, pointers to both objects and collision data
	PhysX::sPhysXObject* pA			= NULL;
	PhysX::sPhysXObject* pB			= NULL;
	PhysX::sCollision	 collision	= { 0 };
	int iVehicleIndex = 0;

	// check the first object
	if ( pair.actors [ 0 ] )
	{
		pA = ( PhysX::sPhysXObject* ) pair.actors [ 0 ]->userData;

		if ( pA )
			collision.iObjectA = pA->iID;

		iVehicleIndex = 0;
	}

	// check the second object
	if ( pair.actors [ 1 ] )
	{
		pB = ( PhysX::sPhysXObject* ) pair.actors [ 1 ]->userData;

		if ( pB )
			collision.iObjectB = pB->iID;

		iVehicleIndex = 1;
	}

	
	{
		if ( pA )
			pA->collisionList.clear ( );

		if ( pB )
			pB->collisionList.clear ( );
	}

	collision.fFrictionForceX = pair.sumFrictionForce.x;
	collision.fFrictionForceY = pair.sumFrictionForce.y;
	collision.fFrictionForceZ = pair.sumFrictionForce.z;

	//collision.fNormalForceX   = pair.sumNormalForce.x;
	//collision.fNormalForceY   = pair.sumNormalForce.y;
	//collision.fNormalForceZ   = pair.sumNormalForce.z;

	NxContactStreamIterator i ( pair.stream );

	while ( i.goNextPair ( ) )
	{
		NxShape* s = i.getShape ( iVehicleIndex );

		while ( i.goNextPatch ( ) )
		{
			const NxVec3& contactNormal = i.getPatchNormal ( );
			//const NxVec3& contactPoint  = i.getPoint       ( );

			//collision.fNormalForceX   = contactPoint.x;
			//collision.fNormalForceY   = contactPoint.y;
			//collision.fNormalForceZ   = contactPoint.z;

			while ( i.goNextPoint ( ) )
			{
				//const NxVec3& contactNormal = i.getPatchNormal ( );
				const NxVec3& contactPoint = i.getPoint ( );

				//
				NxVec3 contactForce = contactNormal * i.getPointNormalForce();
				NxVec3 contactArrowForceTip = contactPoint + contactForce * 0.1f;
				NxVec3 contactArrowFrictionTip = contactPoint + pair.sumFrictionForce* 0.1f;
				NxVec3 contactArrowPenetrationTip = contactPoint - contactNormal * i.getSeparation() * 20.0f;
				//


				{
					sObjectCollisionData data;

					if ( pA )
					{
						if ( pB )
							pA->iLastCollider = pB->iID;

						data.iObjectA = pA->iID;
					}

					if ( pB )
					{
						if ( pA )
							pB->iLastCollider = pA->iID;

						data.iObjectB = pB->iID;
					}

					data.vecFrictionForce			= D3DXVECTOR3 ( pair.sumFrictionForce.x,		pair.sumFrictionForce.y,		pair.sumFrictionForce.z );
					data.vecNormalForce				= D3DXVECTOR3 ( pair.sumNormalForce.x,			pair.sumNormalForce.y,			pair.sumNormalForce.z );
					data.vecContactNormal			= D3DXVECTOR3 ( contactNormal.x,				contactNormal.y,				contactNormal.z );
					data.vecContactPoint			= D3DXVECTOR3 ( contactPoint.x,					contactPoint.y,					contactPoint.z );
					data.contactForce				= D3DXVECTOR3 ( contactForce.x,					contactForce.y,					contactForce.z );
					data.contactArrowForceTip		= D3DXVECTOR3 ( contactArrowForceTip.x,         contactArrowForceTip.y,         contactArrowForceTip.z );
					data.contactArrowFrictionTip    = D3DXVECTOR3 ( contactArrowFrictionTip.x,      contactArrowFrictionTip.y,      contactArrowFrictionTip.z );
					data.contactArrowPenetrationTip = D3DXVECTOR3 ( contactArrowPenetrationTip.x,   contactArrowPenetrationTip.y,   contactArrowPenetrationTip.z );

					if ( pA )
						pA->collisionList.push_back ( data );

					if ( pB )
						pB->collisionList.push_back ( data );
				}

			
				if ( s->is(NX_SHAPE_CAPSULE ) )
				{
					PhysX::CarWheelContact cwc;
					cwc.car   = pair.actors [ iVehicleIndex ];
					cwc.wheel = s;
					cwc.contactPoint = contactPoint;

					if ( pA )
					{
						if ( pA->type == PhysX::eTypeVehicle )
							dbPhyGetWheelAddContactPoints ( pA->iID, cwc );
						else
						{
							if ( pB )
								dbPhyGetWheelAddContactPoints ( pB->iID, cwc );
						}
					}
				}
			}
		}		
	}

	// push collision data onto the stack if required
	if ( pA || pB )
	{
		if ( collision.iObjectA > collision.iObjectB )
		{
			int iTemp = collision.iObjectA;

			collision.iObjectA = collision.iObjectB;
			collision.iObjectB = iTemp;
		}

		PhysX::CollisionStack.push ( collision );
	}
	
	if ( PhysX::CollisionStack.size ( ) > 250 )
	{
		while ( PhysX::CollisionStack.empty ( ) == false )
			PhysX::CollisionStack.pop ( );
	}
}

int dbPhyGetCollision ( int iObjectA, int iObjectB )
{
	PhysX::sPhysXObject* pA = dbPhyGetObject ( iObjectA, true );
	
	if ( !pA )
		return 0;

	if ( iObjectB == 0 )
	{
		if ( pA->collisionList.size ( ) > 0 ) 
		{
			if ( pA->collisionList [ 0 ].iObjectA == iObjectA )
				return pA->collisionList [ 0 ].iObjectB;
			else
				return pA->collisionList [ 0 ].iObjectA;
		}

		return 0;
	}

	PhysX::sPhysXObject* pB = dbPhyGetObject ( iObjectB, true );

	if ( !pA || !pB )
		return 0;

	if ( pA->collisionList.size ( ) > 0 && pB->collisionList.size ( ) > 0 ) 
	{
		if ( pA->collisionList [ 0 ].iObjectA == iObjectA || pA->collisionList [ 0 ].iObjectB == iObjectB )
			return 1;

		if ( pA->collisionList [ 0 ].iObjectA == iObjectB || pA->collisionList [ 0 ].iObjectB == iObjectA )
			return 1;

		if ( pB->collisionList [ 0 ].iObjectA == iObjectA || pB->collisionList [ 0 ].iObjectB == iObjectB )
			return 1;

		if ( pB->collisionList [ 0 ].iObjectA == iObjectB || pB->collisionList [ 0 ].iObjectB == iObjectA )
			return 1;
	}
	
	return 0;
}

int dbPhyGetCollisionCount ( int iObject )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		return pObject->collisionList.size ( ) - 1;
	}

	return 0;
}

float dbPhyGetCollisionFrictionForceX ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecFrictionForce.x;
	}

	return 0.0f;
}

float dbPhyGetCollisionFrictionForceY ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecFrictionForce.y;
	}

	return 0.0f;
}

float dbPhyGetCollisionFrictionForceZ ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecFrictionForce.z;
	}

	return 0.0f;
}

float dbPhyGetCollisionNormalForceX ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecNormalForce.x;
	}

	return 0.0f;
}

float dbPhyGetCollisionNormalForceY ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecNormalForce.y;
	}

	return 0.0f;
}

float dbPhyGetCollisionNormalForceZ ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecNormalForce.z;
	}

	return 0.0f;
}

float dbPhyGetCollisionContactNormalX ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecContactNormal.x;
	}

	return 0.0f;
}

float dbPhyGetCollisionContactNormalY ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecContactNormal.y;
	}

	return 0.0f;
}

float dbPhyGetCollisionContactNormalZ ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecContactNormal.z;
	}

	return 0.0f;
}

float dbPhyGetCollisionContactPointX ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecContactPoint.x;
	}

	return 0.0f;
}

float dbPhyGetCollisionContactPointY ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecContactPoint.y;
	}

	return 0.0f;
}

float dbPhyGetCollisionContactPointZ ( int iObject, int iIndex )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iIndex < (int)pObject->collisionList.size ( ) )
			return pObject->collisionList [ iIndex ].vecContactPoint.z;
	}

	return 0.0f;
}

DWORD dbPhyGetCollisionFrictionForceXEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionFrictionForceX ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionFrictionForceYEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionFrictionForceY ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionFrictionForceZEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionFrictionForceZ ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionNormalForceXEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionNormalForceX ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionNormalForceYEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionNormalForceY ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionNormalForceZEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionNormalForceZ ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionContactNormalXEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionContactNormalX ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionContactNormalYEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionContactNormalY ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionContactNormalZEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionContactNormalZ ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionContactPointXEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionContactPointX ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionContactPointYEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionContactPointY ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetCollisionContactPointZEx ( int iObject, int iIndex )
{
	float fValue = dbPhyGetCollisionContactPointZ ( iObject, iIndex );
	return *( DWORD* ) &fValue;
}

void dbPhySetCollisionState ( int iObject, int iState )
{
	if ( PhysX::sPhysXObject* pObject = dbPhyGetObject ( iObject, true ) )
	{
		if ( iState == 0 )
		{
			pObject->pActorList [ 0 ]->raiseActorFlag ( NX_AF_DISABLE_COLLISION );
			pObject->pActorList [ 0 ]->raiseActorFlag ( NX_AF_DISABLE_RESPONSE );
		}
		else
		{
			pObject->pActorList [ 0 ]->clearActorFlag ( NX_AF_DISABLE_COLLISION );
			pObject->pActorList [ 0 ]->clearActorFlag ( NX_AF_DISABLE_RESPONSE );
		}
	}
}