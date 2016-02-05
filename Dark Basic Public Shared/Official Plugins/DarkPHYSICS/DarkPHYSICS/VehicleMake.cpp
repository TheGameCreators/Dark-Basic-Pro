
#include "globals.h"
#include "vehiclestructure.h"
#include "vehiclemake.h"

// custom data structure - used to help create wheels
#include "NxWheelDesc.h"


/*
	PHY CREATE VEHICLE%L%?dbPhyCreateVehicle@@YAXH@Z%
	PHY ADD VEHICLE BODY%LFFFFFF%?dbPhyAddVehicleBody@@YAXHMMMMMM@Z%
	PHY ADD VEHICLE WHEEL%LLFFFFFLL%?dbPhyAddVehicleWheel@@YAXHHMMMMMHH@Z%
	PHY BUILD VEHICLE%L%?dbPhyBuildVehicle@@YAXH@Z%
*/

D3DXMATRIX dbPhyGetMatrix1 ( int iObject, int iLimb )
{
	sObject* pObject = PhysX::GetObject ( iObject );

	D3DXMATRIX matScale, matTranslation, matRotation, matWorld, matRotateX, matRotateY, matRotateZ;

	D3DXMatrixScaling ( &matScale, pObject->position.vecScale.x, pObject->position.vecScale.y, pObject->position.vecScale.z );

	DWORD dwLimbPositionX  = PhysX::GetLimbPositionX  ( iObject, iLimb );
	DWORD dwLimbPositionY  = PhysX::GetLimbPositionY  ( iObject, iLimb );
	DWORD dwLimbPositionZ  = PhysX::GetLimbPositionZ  ( iObject, iLimb );
	DWORD dwLimbDirectionX = PhysX::GetLimbDirectionX ( iObject, iLimb );
	DWORD dwLimbDirectionY = PhysX::GetLimbDirectionY ( iObject, iLimb );
	DWORD dwLimbDirectionZ = PhysX::GetLimbDirectionZ ( iObject, iLimb );

	float fLimbPositionX   = *( float* ) &dwLimbPositionX;
	float fLimbPositionY   = *( float* ) &dwLimbPositionY;
	float fLimbPositionZ   = *( float* ) &dwLimbPositionZ;
	float fLimbDirectionX  = *( float* ) &dwLimbDirectionX;
	float fLimbDirectionY  = *( float* ) &dwLimbDirectionY;
	float fLimbDirectionZ  = *( float* ) &dwLimbDirectionZ;

	D3DXMatrixTranslation ( &matTranslation, fLimbPositionX, fLimbPositionY, fLimbPositionZ );

	D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( fLimbDirectionX ) );
	D3DXMatrixRotationY ( &matRotateY, D3DXToRadian ( fLimbDirectionY ) );
	D3DXMatrixRotationZ ( &matRotateZ, D3DXToRadian ( fLimbDirectionZ ) );

	matRotation = matRotateX  * matRotateY * matRotateZ;
	matWorld    = matRotation * matScale   * matTranslation;

	D3DXMATRIX mat = pObject->position.matWorld * pObject->ppFrameList [ iLimb ]->matAbsoluteWorld ;
	return mat;
}

void dbPhyCreateVehicle ( int iObject )
{
	PhysXVehicle::sVehicle* pVehicle = new PhysXVehicle::sVehicle;

	if ( !pVehicle )
		return;

	pVehicle->iID = iObject;
	pVehicle->actorDesc.setToDefault ( );

	dbPhySetVehicleDefaultProperties ( iObject, pVehicle );

	PhysXVehicle::pVehicleList.push_back ( pVehicle );
}

void dbPhySetVehicleDefaultProperties ( int iObject, PhysXVehicle::sVehicle* pVehicle )
{
	pVehicle->pObject						= PhysX::GetObject ( iObject );
	pVehicle->bAutoDrive					= false;
	pVehicle->bFrontWheelIsPowered			= true;
	pVehicle->bRearWheelIsPowered			= true;
	pVehicle->fMaxMotorPower				= 360.0f;
	pVehicle->fMotorForce					= 0.0f;
	pVehicle->fSteeringDelta				= 0.05f;
	pVehicle->fMaxSteeringAngle				= NxMath::degToRad ( 45.0f );
	pVehicle->fSuspensionSpring				= 100.0f;
	pVehicle->fSuspensionDamping			= 0.5f;
	pVehicle->fSuspensionBias				= 0.0f;
	pVehicle->fMass							= 20;
	pVehicle->bodyDesc.mass					= pVehicle->fMass;
	pVehicle->iWheelCount					= 0;
	pVehicle->iBoxCount						= 0;
	pVehicle->fOldPosX						= 0.0f;
	pVehicle->fOldPosY						= 0.0f;
	pVehicle->fOldPosZ						= 0.0f;
	pVehicle->fSteeringValue				= 0.0f;
	pVehicle->fSteeringAngle				= 0.0f;
	pVehicle->fWheelMultiplier				= 15.0f;

	pVehicle->bBuilt						= false;
}

void dbPhyCreateVehicleMaterial ( PhysXVehicle::sVehicle* pVehicle )
{
	pVehicle->material.restitution			= 0.0f;
	pVehicle->material.staticFriction		= 0.5f;
	pVehicle->material.dynamicFriction		= 0.5f;
	pVehicle->material.flags				|= NX_MF_DISABLE_FRICTION;
	pVehicle->materialIndex = PhysX::pScene->createMaterial ( pVehicle->material )->getMaterialIndex ( );
}

void dbPhySetDefaultVehiclePosition ( PhysXVehicle::sVehicle* pVehicle )
{
	NxVec3 vecPosition = NxVec3 ( pVehicle->pObject->position.vecPosition.x, pVehicle->pObject->position.vecPosition.y, pVehicle->pObject->position.vecPosition.z );

	NxMat33 rotX;
	NxMat33 rotY;
	NxMat33 rotZ;
	NxMat33 matRotation;
	
	rotX.rotX ( D3DXToRadian ( pVehicle->pObject->position.vecRotate.x ) );
	rotY.rotY ( D3DXToRadian ( pVehicle->pObject->position.vecRotate.y ) );
	rotZ.rotZ ( D3DXToRadian ( pVehicle->pObject->position.vecRotate.z ) );

	matRotation = rotX * rotY * rotZ;

	pVehicle->pActor->setGlobalPosition    ( vecPosition );
	pVehicle->pActor->setGlobalOrientation ( matRotation );

	pVehicle->fOldPosX = pVehicle->pObject->position.vecPosition.x;
	pVehicle->fOldPosY = pVehicle->pObject->position.vecPosition.y;
	pVehicle->fOldPosZ = pVehicle->pObject->position.vecPosition.z;
}

void dbPhyAddVehicleBody ( int iObject, float fSizeX, float fSizeY, float fSizeZ, float fOriginX, float fOriginY, float fOriginZ )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->boxDesc [ pVehicle->iBoxCount ].dimensions.set ( fSizeX, fSizeY, fSizeZ );
		pVehicle->boxDesc [ pVehicle->iBoxCount ].localPose.t = NxVec3 ( fOriginX, fOriginY, fOriginZ );
		pVehicle->iBoxCount++;
	}
}

void dbPhyAddVehicleWheel ( int iObject, int iLimb, float fWheelX, float fWheelY, float fWheelZ, float fRadius, float fHeight, int iLocation, int iDirection )
{
	// store now, create wheel later when BUILD (wheel shapes built after chassis actor)
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->wheels [ pVehicle->iWheelCount ].fRadius = fRadius;
		pVehicle->wheels [ pVehicle->iWheelCount ].fHeight = fHeight;
		pVehicle->wheels [ pVehicle->iWheelCount ].fWheelX = fWheelX;
		pVehicle->wheels [ pVehicle->iWheelCount ].fWheelY = fWheelY;
		pVehicle->wheels [ pVehicle->iWheelCount ].fWheelZ = fWheelZ;

		if ( iLocation == 0 )
			pVehicle->wheels [ pVehicle->iWheelCount ].bFront = true;
		else
			pVehicle->wheels [ pVehicle->iWheelCount ].bFront = false;

		pVehicle->wheels [ pVehicle->iWheelCount ].iDirection	= iDirection;
		pVehicle->wheels [ pVehicle->iWheelCount ].iLimb		= iLimb;
		pVehicle->iWheelCount++;
	}
}

NxWheelShape* AddWheelToActor ( PhysXVehicle::sVehicle* pVehicle, NxActor* actor, NxWheelDesc* wheelDesc )
{
	NxWheelShapeDesc wheelShapeDesc;
	wheelShapeDesc.materialIndex = pVehicle->materialIndex;
	wheelShapeDesc.localPose.t = wheelDesc->position;
	NxQuat q;
	q.fromAngleAxis(90, NxVec3(0,1,0));
	wheelShapeDesc.localPose.M.fromQuat(q);
	NxReal heightModifier = (wheelDesc->wheelSuspension + wheelDesc->wheelRadius) / wheelDesc->wheelSuspension;
	wheelShapeDesc.suspension.spring = wheelDesc->springRestitution*heightModifier;
	wheelShapeDesc.suspension.damper = 0;//wheelDesc->springDamping*heightModifier;
	wheelShapeDesc.suspension.targetValue = wheelDesc->springBias*heightModifier;
	wheelShapeDesc.radius = wheelDesc->wheelRadius;
	wheelShapeDesc.suspensionTravel = wheelDesc->wheelSuspension; 
	wheelShapeDesc.inverseWheelMass = 0.1f;
	wheelShapeDesc.lateralTireForceFunction.stiffnessFactor *= wheelDesc->frictionToSide;	
	wheelShapeDesc.longitudalTireForceFunction.stiffnessFactor *= wheelDesc->frictionToFront;	
    NxWheelShape* wheelShape = NULL;
	wheelShape = static_cast<NxWheelShape *>(actor->createShape(wheelShapeDesc));
	return wheelShape;
}

void CreateVehicleWheels ( PhysXVehicle::sVehicle* pVehicle )
{
	// clear wheel shape ptrs
	memset ( pVehicle->wheelptr, 0, sizeof(pVehicle->wheelptr) );

	// create wheel shapes
	NxWheelDesc wheelDesc;
	NxActor* actor = pVehicle->pActor;
	for ( int iW=0; iW<pVehicle->iWheelCount; iW++ )
	{
		wheelDesc.wheelApproximation = 10;
		wheelDesc.wheelRadius = pVehicle->wheels[iW].fRadius;
		wheelDesc.wheelWidth = pVehicle->wheels[iW].fRadius;
		wheelDesc.wheelSuspension = pVehicle->wheels[iW].fHeight;  
		wheelDesc.springRestitution = pVehicle->fSuspensionSpring;
		wheelDesc.springDamping = pVehicle->fSuspensionDamping;
		wheelDesc.springBias = pVehicle->fSuspensionBias;  
		wheelDesc.maxBrakeForce = 1;
		wheelDesc.position = NxVec3( pVehicle->wheels[iW].fWheelX, pVehicle->wheels[iW].fWheelY, pVehicle->wheels[iW].fWheelZ );
		wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
								NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT; 

		// create shape and add to vehicle
		pVehicle->wheelptr [ iW ] = AddWheelToActor ( pVehicle, actor, &wheelDesc );
	}
}

void dbPhyBuildVehicle ( int iObject )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		PhysX::sPhysXObject* pPhysXObject = new PhysX::sPhysXObject;
		if ( !pPhysXObject )
			return;

		// create vehicle body mass, material and box shapes
		pVehicle->bodyDesc.mass = pVehicle->fMass;
		dbPhyCreateVehicleMaterial ( pVehicle );
		for ( int i = 0; i < pVehicle->iBoxCount; i++ )
			pVehicle->actorDesc.shapes.push_back ( &pVehicle->boxDesc [ i ] );

		// create the actor
		pVehicle->actorDesc.body    = &pVehicle->bodyDesc;
		pVehicle->pActor            = PhysX::pScene->createActor ( pVehicle->actorDesc );
		pPhysXObject->pObject		= PhysX::GetObject ( iObject );
		pPhysXObject->iID			= iObject;
		pPhysXObject->pScene  = PhysX::pScene;
		pPhysXObject->pActorList.push_back ( pVehicle->pActor );
		pPhysXObject->type			= PhysX::eTypeVehicle;
		pVehicle->pActor->userData	= ( void* ) pPhysXObject;
		pVehicle->pActor->wakeUp ( 1e10 );
		dbPhySetDefaultVehiclePosition ( pVehicle );

		// 120706 - new wheel system - lower center of gravity on car body
		NxVec3 massPos = pVehicle->pActor->getCMassLocalPosition();
		massPos.y = -pVehicle->boxDesc [ 0 ].dimensions.y;
		pVehicle->pActor->setCMassOffsetLocalPosition(massPos);

		// 120706 - new wheels created after vehicle actor body
		CreateVehicleWheels ( pVehicle );

		// PhysX object added to object list
		PhysX::sPhysXObject* pPhysXObject1 = new PhysX::sPhysXObject;
		pPhysXObject1->pScene  = NULL;
		pPhysXObject1->pObject = NULL;
		pPhysXObject1->pActorList.push_back ( pVehicle->pActor );
		pPhysXObject1->iID     = iObject;
		pPhysXObject1->type     = PhysX::eTypeVehicle;
		PhysX::pObjectList.push_back ( pPhysXObject1 );

		pVehicle->bBuilt = true;
	}
}
