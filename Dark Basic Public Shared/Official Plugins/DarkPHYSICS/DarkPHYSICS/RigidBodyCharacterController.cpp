
#include "globals.h"
#include "rigidbodycharactercontroller.h"
#include "error.h"

/////////////////////////////////////////////////////////////////////////////////////
// STRING TABLE /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*
	PHY MAKE BOX CHARACTER CONTROLLER%LFFFFFFLFF%?dbPhyMakeBoxCharacterController@@YAXHMMMMMMHMM@Z%
	PHY MAKE CAPSULE CHARACTER CONTROLLER%LFFFFFLFF%?dbPhyMakeCapsuleCharacterController@@YAXHMMMMMHMM@Z%

	PHY MOVE CHARACTER CONTROLLER%LF%?dbPhyMoveCharacterController@@YAXHM@Z%

	PHY GET CHARACTER CONTROLLER EXIST[%LL%?dbPhyGetCharacterControllerExist@@YAHH@Z%
	PHY DELETE CHARACTER CONTROLLER%L%?dbPhyDeleteCharacterController@@YAXH@Z%

	PHY SET CHARACTER CONTROLLER DISPLACEMENT%LFF%?dbPhySetCharacterControllerDisplacement@@YAXHMM@Z%
	PHY SET CHARACTER CONTROLLER DISPLACEMENT%LFFF%?dbPhySetCharacterControllerDisplacement@@YAXHMMM@Z%
	PHY SET CHARACTER CONTROLLER DISPLACEMENT%LL%?dbPhySetCharacterControllerDisplacement@@YAXHH@Z%
	PHY SET CHARACTER CONTROLLER SHARPNESS%LF%?dbPhySetCharacterControllerSharpness@@YAXHM@Z%
	PHY SET CHARACTER CONTROLLER EXTENTS%LFFF%?dbPhySetCharacterControllerExtents@@YAXHMMM@Z%
	PHY SET CHARACTER CONTROLLER EXTENTS%LFF%?dbPhySetCharacterControllerExtents@@YAXHMM@Z%
	PHY SET CHARACTER CONTROLLER MIN DISTANCE%LF%?dbPhySetCharacterControllerMinDistance@@YAXHM@Z%
*/

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

// CharacterController



namespace PhysX
{
	struct sCharacterController
	{
		int						iID;
		int						iType;
		NxController**			ppController;

		int						iDisplacementState;
		float					fDisplacementForward;
		float					fDisplacementBackward;

		NxVec3					vecDisplacement;

		float					fSharpness;

		float					fMinDistance;

		union
		{
			NxBoxController**		 ppBoxController;
			NxCapsuleController**	 ppCapsuleController;
		};

		union
		{
			NxBoxControllerDesc*	 pBoxDescription;
			NxCapsuleControllerDesc* pCapsuleDescription;
		};

		PhysX::sPhysXObject*	pUserData;
	};	

	std::vector < sCharacterController* > pControllerList;
}

PhysX::sCharacterController* dbPhyGetCharacterController ( int iID, bool bDisplayError = false )
{
	for ( DWORD dwController = 0; dwController < PhysX::pControllerList.size ( ); dwController++ )
	{
		if ( PhysX::pControllerList [ dwController ]->iID == iID )
		{
			return PhysX::pControllerList [ dwController ];
		}
	}

	return NULL;
}

NxController* dbPhyGetControllerData ( PhysX::sCharacterController* pController )
{
	if ( !pController )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid controller data for 'dbPhyGetControllerData'", "", 0 );
		return NULL;
	}

	NxController* pData = NULL;

	switch ( pController->iType )
	{
		case 0: pData = pController->ppBoxController     [ 0 ]; break;
		case 1: pData = pController->ppCapsuleController [ 0 ]; break;
	}

	if ( !pData )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid controller type for 'dbPhyGetControllerData'", "", 0 );
		return NULL;
	}

	return pData;
}


class ControllerHitReport : public NxUserControllerHitReport
{
	
	public:
	virtual NxControllerAction  onShapeHit(const NxControllerShapeHit& hit)
	{
		if(1 && hit.shape)
		{
			NxCollisionGroup group = hit.shape->getGroup();
			NxActor& actor = hit.shape->getActor();
		
			PhysX::sPhysXObject* pA	 = ( PhysX::sPhysXObject* ) actor.userData;

			PhysX::sPhysXObject* pB	 = ( PhysX::sPhysXObject* ) hit.controller->getActor ( )->userData;

			NxActor* pActor = hit.controller->getActor ( );

			if(actor.isDynamic() && pA )
			{
				PhysX::sCollision	 collision	= { 0 };

				collision.iObjectA = pA->iID;
				collision.iObjectB = pB->iID;

				PhysX::CollisionStack.push ( collision );
			}
		}

		//return NX_ACTION_PUSH;
		return NX_ACTION_NONE;
	}

	virtual NxControllerAction  onControllerHit(const NxControllersHit& hit)
	{
		return NX_ACTION_NONE;
		return NX_ACTION_NONE;
	}

} gControllerHitReport;


void dbPhySetupController ( PhysX::sCharacterController** pController, int iID )
{
	if ( !PhysX::pScene )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid scene for making a character controller", "", 0 );
		return;
	}

	if ( *pController = dbPhyGetCharacterController ( iID ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Character controller already exists, cannot overwrite it", "", 0 );
		return;
	}

	if ( !( *pController = new PhysX::sCharacterController ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate memory in for character controller", "", 0 );
		return;
	}

	memset ( *pController, 0, sizeof ( PhysX::sCharacterController ) );
}

void dbPhyMakeBoxCharacterController ( int iID, float fX, float fY, float fZ, float fSizeX, float fSizeY, float fSizeZ, int iUp, float fStep, float fSlopeLimit )
{
	PhysX::sCharacterController* pController = NULL;

	dbPhySetupController ( &pController, iID );

	if ( ! ( pController->pBoxDescription = new NxBoxControllerDesc ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate descriptor in 'phy make box character controller'", "", 0 );
		return;
	}

	pController->iID							= iID;
	pController->iType							= 0;
	pController->pBoxDescription->position.x	= fX;
	pController->pBoxDescription->position.y	= fY;
	pController->pBoxDescription->position.z	= fZ;
	pController->pBoxDescription->extents.x		= fSizeX;
	pController->pBoxDescription->extents.y		= fSizeY;
	pController->pBoxDescription->extents.z		= fSizeZ;
	pController->pBoxDescription->upDirection	= NX_Y;
	pController->pBoxDescription->slopeLimit	= cosf ( NxMath::degToRad ( fSlopeLimit ) );
	pController->pBoxDescription->skinWidth		= 0.1f;
	pController->pBoxDescription->stepOffset	= fStep;
	pController->pBoxDescription->callback	    = &gControllerHitReport;

	pController->iDisplacementState				= 1;
	pController->fDisplacementForward			= 120.0f;
	pController->fDisplacementBackward			= 240.0f;
	pController->fSharpness						= 0.25f;
	pController->fMinDistance					= 0.000001f;
	
	if ( ! ( pController->ppBoxController = new NxBoxController* ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate controller memory in 'phy make box character controller'", "", 0 );
		return;
	}

	if ( pController->pBoxDescription->isValid ( ) == false )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid parameters used in 'phy make box character controller'", "", 0 );
		return;
	}

	if ( ! ( *pController->ppBoxController = ( NxBoxController* ) PhysX::CharacterController.createController ( PhysX::pScene, *pController->pBoxDescription ) ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create controller in 'phy make box character controller'", "", 0 );
		return;
	}

	PhysX::sPhysXObject* pUserData	= new PhysX::sPhysXObject;
	pUserData->iID = iID;
	pUserData->type = PhysX::eBoxCharacterController;
	pController->ppCapsuleController [ 0 ]->getActor ( )->userData = ( void* ) pUserData;
	
	PhysX::pControllerList.push_back ( pController );
}


void dbPhyMakeCapsuleCharacterController ( int iID, float fX, float fY, float fZ, float fRadius, float fHeight, int iUp, float fStep, float fSlopeLimit )
{
	PhysX::sCharacterController* pController = NULL;

	dbPhySetupController ( &pController, iID );

	if ( ! ( pController->pCapsuleDescription = new NxCapsuleControllerDesc ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate descriptor in 'phy make capsule character controller'", "", 0 );
		return;
	}

	pController->iID								= iID;
	pController->iType								= 1;
	pController->pCapsuleDescription->position.x	= fX;
	pController->pCapsuleDescription->position.y	= fY;
	pController->pCapsuleDescription->position.z	= fZ;
	pController->pCapsuleDescription->radius		= fRadius;
	pController->pCapsuleDescription->height		= fHeight;
	pController->pCapsuleDescription->upDirection	= NX_Y;
	pController->pCapsuleDescription->slopeLimit	= cosf ( NxMath::degToRad ( fSlopeLimit ) );
	pController->pCapsuleDescription->skinWidth		= 0.1f;
	pController->pCapsuleDescription->stepOffset	= fStep;


	pController->iDisplacementState				= 1;
	pController->fDisplacementForward			= 120.0f;
	pController->fDisplacementBackward			= 240.0f;
	pController->fSharpness						= 0.25f;
	pController->fMinDistance					= 0.000001f;

	if ( ! ( pController->ppCapsuleController = new NxCapsuleController* ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Unable to allocate controller memory in 'phy make capsule character controller'", "", 0 );
		return;
	}

	if ( pController->pCapsuleDescription->isValid ( ) == false )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Invalid parameters used in 'phy make capsule character controller'", "", 0 );
		return;
	}

	if ( ! ( *pController->ppCapsuleController = ( NxCapsuleController* ) PhysX::CharacterController.createController ( PhysX::pScene, *pController->pCapsuleDescription ) ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Failed to create controller in 'phy make capsule character controller'", "", 0 );
		return;
	}

	PhysX::sPhysXObject* pUserData	= new PhysX::sPhysXObject;
	pUserData->iID = iID;
	pUserData->type = PhysX::eCapsuleCharacterController;
	pController->ppCapsuleController [ 0 ]->getActor ( )->userData = ( void* ) pUserData;

	PhysX::pControllerList.push_back ( pController );
}

void dbPhySetCharacterControllerDisplacement ( int iID, float fForward, float fBackward )
{
	if ( PhysX::sCharacterController* pController = dbPhyGetCharacterController ( iID ) )
	{
		pController->iDisplacementState    = 1;
		pController->fDisplacementForward  = fForward;
		pController->fDisplacementBackward = fBackward;
	}
}

void dbPhySetCharacterControllerDisplacement ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sCharacterController* pController = dbPhyGetCharacterController ( iID ) )
	{
		pController->iDisplacementState    = 2;
		pController->vecDisplacement	   = NxVec3 ( fX, fY, fZ );
	}
}

void dbPhySetCharacterControllerDisplacement ( int iID, int iState )
{
	if ( PhysX::sCharacterController* pController = dbPhyGetCharacterController ( iID ) )
	{
		pController->iDisplacementState = iState;
	}
}

void dbPhySetCharacterControllerSharpness ( int iID, float fValue )
{
	if ( PhysX::sCharacterController* pController = dbPhyGetCharacterController ( iID ) )
	{
		pController->fSharpness = fValue;
	}
}

void dbPhySetCharacterControllerExtents ( int iID, float fX, float fY, float fZ )
{
	if ( PhysX::sCharacterController* pController = dbPhyGetCharacterController ( iID ) )
	{
		if ( pController->iType == 0 )
			pController->ppBoxController [ 0 ]->setExtents ( NxVec3 ( fX, fY, fZ ) );
	}
}

void dbPhySetCharacterControllerExtents ( int iID, float fRadius, float fHeight )
{
	if ( PhysX::sCharacterController* pController = dbPhyGetCharacterController ( iID ) )
	{
		if ( pController->iType == 1 )
		{
			pController->ppCapsuleController [ 0 ]->setRadius ( fRadius );
			pController->ppCapsuleController [ 0 ]->setHeight ( fHeight );
		}
	}
}

void dbPhySetCharacterControllerMinDistance ( int iID, float fValue )
{
	if ( PhysX::sCharacterController* pController = dbPhyGetCharacterController ( iID ) )
	{
		pController->fMinDistance = fValue;
	}
}

void dbPhyMoveCharacterController ( int iID, float fSpeed )
{
	PhysX::sCharacterController* pController = NULL;
	
	if ( ! ( pController = dbPhyGetCharacterController ( iID ) ) )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Character controller does not exist in 'phy move character controller'", "", 0 );
		return;
	}

	NxVec3   vecDisplacement = PhysX::vecGravity;
	sObject* pObject         = PhysX::GetObject ( iID );

	if ( !pObject )
	{
		PhysX::m_ErrorStream.reportError ( ( NxErrorCode ) 0, "Object associated with character controller does not exist in 'phy move character controller'", "", 0 );
		return;
	}

	NxVec3 horizontalDisp = NxVec3 ( pObject->position.vecLook.x, pObject->position.vecLook.y, pObject->position.vecLook.z );
	horizontalDisp.normalize();
	vecDisplacement += horizontalDisp * fSpeed;

	if ( pController->iDisplacementState == 1 )
	{
		if ( fSpeed > 0.0 )
			vecDisplacement.y -= pController->fDisplacementForward;
	
		if ( fSpeed < 0.0 )
			vecDisplacement.y -= pController->fDisplacementBackward;
	}

	if ( NxController* pData = dbPhyGetControllerData ( pController ) )
	{
		NxF32  sharpness      = 0.25f;
		NxU32  collisionFlags = 0;
		NxVec3 d = vecDisplacement * ( 1 / 60.0f );
		
		NxU32 collisionGroups = 1;
		//pData->move ( d, collisionGroups, 0.000001f, collisionFlags, pController->fSharpness );
		pData->move ( d, collisionGroups, pController->fMinDistance, collisionFlags, pController->fSharpness );

		const NxExtendedVec3& vecPos = pData->getFilteredPosition ( );
		PhysX::PositionObject ( iID, (float)vecPos.x, (float)vecPos.y, (float)vecPos.z );
	}
}

int dbPhyGetCharacterControllerExist ( int iID )
{
	if ( PhysX::sCharacterController* pController = dbPhyGetCharacterController ( iID ) )
		return 1;

	return 0;
}

void dbPhyDeleteCharacterController ( int iID )
{
	// delete a single rigid body in a scene
	if ( PhysX::pScene == NULL )
		return;

	for ( DWORD dwController = 0; dwController < PhysX::pControllerList.size ( ); dwController++ )
	{
		if ( PhysX::pControllerList [ dwController ]->iID == iID )
		{
			PhysX::sCharacterController* pController = PhysX::pControllerList [ dwController ];

			if ( pController->iType == 0 )
			{
				PhysX::CharacterController.releaseController ( *pController->ppBoxController [ 0 ] );
			}

			if ( pController->iType == 1 )
			{
				PhysX::CharacterController.releaseController ( *pController->ppCapsuleController [ 0 ] );
			}
			
			PhysX::pControllerList.erase ( PhysX::pControllerList.begin ( ) + dwController );

			// clear up the memory
			delete pController;
			pController = NULL;
			
			return;
		}
	}
}
