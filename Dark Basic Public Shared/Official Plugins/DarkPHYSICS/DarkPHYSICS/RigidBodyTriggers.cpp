
#include "globals.h"
#include "rigidbodytriggers.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE TRIGGER BOX%LFFFFFFFFF%?dbPhyMakeTriggerBox@@YAXHMMMMMMMMM@Z%
	PHY MAKE TRIGGER SPHERE%LFFFF%?dbPhyMakeTriggerSphere@@YAXHMMMM@Z%
	PHY DELETE TRIGGER%L%?dbPhyDeleteTrigger@@YAXH@Z%

	PHY TRIGGER EXIST[%LL%?dbPhyTriggerExist@@YAHH@Z%
	PHY TRIGGER TYPE[%LL%?dbPhyTriggerType@@YAHH@Z%
	PHY GET TRIGGER DATA[%L%?dbPhyGetTriggerData@@YAHXZ%
	PHY GET TRIGGER OBJECT A[%L%?dbPhyGetTriggerObjectA@@YAHXZ%
	PHY GET TRIGGER OBJECT B[%L%?dbPhyGetTriggerObjectB@@YAHXZ%
	PHY GET TRIGGER ACTION[%L%?dbPhyGetTriggerAction@@YAHXZ%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


void dbPhyMakeTriggerBox ( int iTrigger, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fAngleX, float fAngleY, float fAngleZ )
{
	dbPhyMakeTriggerObject ( iTrigger, 0, fX, fY, fZ, fSizeX, fSizeY, fSizeZ, fAngleX, fAngleY, fAngleZ, 0.0f );
}

void dbPhyMakeTriggerSphere ( int iTrigger, float fX, float fY, float fZ, float fRadius )
{
	dbPhyMakeTriggerObject ( iTrigger, 1, fX, fY, fZ, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, fRadius );
}

void dbPhyDeleteTrigger ( int iTrigger )
{
	if ( PhysX::pScene == NULL )
		return;

	// go through all objects
	for ( DWORD dwTrigger = 0; dwTrigger < PhysX::pTriggerList.size ( ); dwTrigger++ )
	{
		// get a pointer to our object
		PhysX::sPhysXObject* pTrigger = PhysX::pTriggerList [ dwTrigger ];

		// see if we have a match
		if ( pTrigger->iID == iTrigger )
		{
			if ( pTrigger->type == PhysX::eBoxTrigger || pTrigger->type == PhysX::eBoxTrigger  )
			{
				if ( pTrigger->pActorList.size ( ) > 0 )
				{
					PhysX::pScene->releaseActor ( *pTrigger->pActorList [ 0 ] );

					// erase the item in the list
					PhysX::pTriggerList.erase ( PhysX::pTriggerList.begin ( ) + dwTrigger );

					// clear up the memory
					delete pTrigger;
					pTrigger = NULL;

					return;
				}
			}
		}
	}

	PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to delete trigger as it does not exist", "", 0 );
	return;
}

int dbPhyTriggerExist ( int iTrigger )
{
	if ( PhysX::sPhysXObject* pTrigger = dbPhyGetTrigger ( iTrigger ) )
	{
		if ( pTrigger->iID == iTrigger )
			return 1;
	}

	return 0;
}

int dbPhyTriggerType ( int iTrigger )
{
	if ( PhysX::sPhysXObject* pTrigger = dbPhyGetTrigger ( iTrigger, true ) )
	{
		if ( pTrigger->type = PhysX::eBoxTrigger )
			return 1;

		if ( pTrigger->type = PhysX::eSphereTrigger )
			return 1;
	}

	return 0;
}

void dbPhyMakeTriggerObject ( int iTrigger, int iType, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, float fAngleX, float fAngleY, float fAngleZ, float fRadius )
{
	NxActorDesc				actorDesc;
	NxBoxShapeDesc			boxDesc;
	NxSphereShapeDesc		sphereDesc;
	NxActor*				pActor = NULL;
	PhysX::sPhysXObject*	pPhysXObject = new PhysX::sPhysXObject;

	switch ( iType )
	{
		case 0:
		{
			pPhysXObject->type  = PhysX::eBoxTrigger;
			boxDesc.dimensions  = NxVec3 ( fSizeX, fSizeY, fSizeZ );
			boxDesc.shapeFlags |= NX_TRIGGER_ENABLE;
			actorDesc.shapes.pushBack ( &boxDesc );
		}
		break;

		case 1:
		{
			pPhysXObject->type     = PhysX::eSphereTrigger;
			sphereDesc.radius      = fRadius;
			sphereDesc.shapeFlags |= NX_TRIGGER_ENABLE;
			actorDesc.shapes.pushBack ( &sphereDesc );
		}
		break;
	}
	
	pActor = PhysX::pScene->createActor ( actorDesc );

	if ( !pActor || !pPhysXObject )
		return;

	dbPhySetTriggerDefaults ( pActor, fX, fY, fZ, fAngleX, fAngleY, fAngleZ );

	pPhysXObject->pScene  = PhysX::pScene;
	pPhysXObject->pObject = NULL;
	pPhysXObject->iID     = iTrigger;
	pPhysXObject->pActorList.push_back ( pActor );
	//pPhysXObject->pActor  = pActor;
	pActor->userData      = ( void* ) pPhysXObject;

	PhysX::pTriggerList.push_back ( pPhysXObject );
}

void dbPhySetTriggerDefaults ( NxActor* pActor, float fX, float fY, float fZ, float fAngleX, float fAngleY, float fAngleZ )
{
	if ( !pActor )
		return;

	NxMat33 matRotX;
	NxMat33 matRotY;
	NxMat33 matRotZ;
	NxMat33 matRotation;

	matRotX.rotX ( D3DXToRadian ( fAngleX ) );
	matRotY.rotY ( D3DXToRadian ( fAngleY ) );
	matRotZ.rotZ ( D3DXToRadian ( fAngleZ ) );

	matRotation = matRotX * matRotY * matRotZ;

	pActor->setGlobalPosition    ( NxVec3 ( fX, fY, fZ ) );
	pActor->setGlobalOrientation ( matRotation );
}

PhysX::sPhysXObject* dbPhyGetTrigger ( int iID, bool bDisplayError )
{
	for ( DWORD dwObject = 0; dwObject < PhysX::pTriggerList.size ( ); dwObject++ )
	{
		if ( PhysX::pTriggerList [ dwObject ]->iID == iID )
		{
			return PhysX::pTriggerList [ dwObject ];
		}
	}

	// display an error if needed
	if ( bDisplayError )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Trigger with specified ID does not exist", "", 0 );
		return NULL;
	}

	return NULL;
}

void PhysX::cTriggerReport::onTrigger ( NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status )
{
	NxActor&			 triggerActor = triggerShape.getActor ( );
	NxActor&			 objectActor  = otherShape.getActor   ( );
	PhysX::sPhysXObject* pTrigger     = ( PhysX::sPhysXObject* ) triggerActor.userData;
	PhysX::sPhysXObject* pObject      = ( PhysX::sPhysXObject* ) objectActor.userData;
	PhysX::sCollision	 trigger	  = { 0 };

  	if ( !pTrigger || !pObject )
		return;

	trigger.iObjectA = pTrigger->iID;
	trigger.iObjectB = pObject->iID;

	if ( status & NX_TRIGGER_ON_ENTER )
		trigger.action = PhysX::eActionEnter;
	
	if ( status & NX_TRIGGER_ON_LEAVE )
		trigger.action = PhysX::eActionLeave;

	PhysX::TriggerStack.push ( trigger );
}

int	dbPhyGetTriggerData ( void )
{
	if ( PhysX::TriggerStack.size ( ) )
	{
		memset ( &PhysX::TriggerData, 0, sizeof ( PhysX::TriggerData ) );

		PhysX::TriggerData = PhysX::TriggerStack.top ( );

		PhysX::TriggerStack.pop ( );

		return 1;
	}

	return 0;
}

int	dbPhyGetTriggerObjectA ( void )
{
	return PhysX::TriggerData.iObjectA;
}

int	dbPhyGetTriggerObjectB ( void )
{
	return PhysX::TriggerData.iObjectB;
}

int dbPhyGetTriggerAction ( void )
{
	return ( int ) PhysX::TriggerData.action;
}