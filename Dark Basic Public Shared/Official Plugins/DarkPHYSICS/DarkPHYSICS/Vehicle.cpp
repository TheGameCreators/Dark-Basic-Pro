
#include "globals.h"
#include "vehicle.h"
//#include <darksdk.h>

sObject* dbGetObject ( int iID );

struct sVehicle;

float		dbPhyGetDistance					( float x1 , float y1 , float z1 , float x2 , float y2 , float z2);
void		dbPhySetVehicleDefaultProperties	( int iObject, sVehicle* pVehicle );
void		dbPhyCreateVehicleMaterial			( sVehicle* pVehicle );
void		dbPhySetDefaultVehiclePosition		( sVehicle* pVehicle );
void		dbPhyUpdateVehicleWheels			( sVehicle* pVehicle );
void		dbPhyDriveVehicle					( sVehicle* pVehicle );
void		dbPhyMoveVehicle					( sVehicle* pVehicle );
sVehicle*	dbPhyGetVehicle						( int iID );

/*
	PHY CREATE VEHICLE%L%?dbPhyCreateVehicle@@YAXH@Z%
	PHY ADD VEHICLE BODY%LFFFFFF%?dbPhyAddVehicleBody@@YAXHMMMMMM@Z%
	PHY ADD VEHICLE WHEEL%LLFFFFFLL%?dbPhyAddVehicleWheel@@YAXHHMMMMMHH@Z%
	PHY BUILD VEHICLE%L%?dbPhyBuildVehicle@@YAXH@Z%

	PHY SET VEHICLE MASS%LF%?dbPhySetVehicleMass@@YAXHM@Z%
	PHY SET VEHICLE AUTO%LL%?dbPhySetVehicleAuto@@YAXHH@Z%
	PHY SET VEHICLE MAX MOTOR%LF%?dbPhySetVehicleMaxMotor@@YAXHM@Z%
	PHY SET VEHICLE MOTOR FORCE%LF%?dbPhySetVehicleMotorForce@@YAXHM@Z%
	PHY SET VEHICLE WHEEL POWER%LLL%?dbPhySetVehicleWheelPower@@YAXHHH@Z%
	PHY SET VEHICLE STEERING DELTA%LF%?dbPhySetVehicleSteeringDelta@@YAXHM@Z%
	PHY SET VEHICLE MAX STEERING ANGLE%LF%?dbPhySetVehicleMaxSteeringAngle@@YAXHM@Z%
	PHY SET VEHICLE WHEEL ROTATION%LLL%?dbPhySetVehicleWheelRotation@@YAXHHH@Z%

	PHY SET VEHICLE SUSPENSION SPRING%LF%?dbPhySetVehicleSuspensionSpring@@YAXHM@Z%
	PHY SET VEHICLE SUSPENSION DAMPING%LF%?dbPhySetVehicleSuspensionDamping@@YAXHM@Z%
	PHY SET VEHICLE SUSPENSION BIAS%LF%?dbPhySetVehicleSuspensionBias@@YAXHM@Z%

	PHY SET VEHICLE RESTITUTION%LF%?dbPhySetVehicleRestitution@@YAXHM@Z%
	PHY SET VEHICLE STATIC FRICTION%LF%?dbPhySetVehicleStaticFriction@@YAXHM@Z%
	PHY SET VEHICLE STATIC FRICTION V%LF%?dbPhySetVehicleStaticFrictionV@@YAXHM@Z%
	PHY SET VEHICLE DYNAMIC FRICTION%LF%?dbPhySetVehicleDynamicFriction@@YAXHM@Z%
	PHY SET VEHICLE DYNAMIC FRICTION V%LF%?dbPhySetVehicleDynamicFrictionV@@YAXHM@Z%
	
	PHY SET VEHICLE WHEEL MULTIPLIER%LF%?dbPhySetVehicleWheelMultiplier@@YAXHM@Z%
	PHY SET VEHICLE STEERING ANGLE%LF%?dbPhySetVehicleSteeringAngle@@YAXHM@Z%
	PHY SET VEHICLE STEERING VALUE%LF%?dbPhySetVehicleSteeringValue@@YAXHM@Z%
	PHY GET VEHICLE STEERING ANGLE[%FL%?dbPhyGetVehicleSteeringAngleEx@@YAKH@Z%
	PHY GET VEHICLE STEERING DELTA[%FL%?dbPhyGetVehicleSteeringDeltaEx@@YAKH@Z%

	PHY GET VEHICLE MOTOR FORCE[%FL%?dbPhyGetVehicleMotorForceEx@@YAKH@Z%
*/

struct sVehicleWheelData
{
	int  iDirection;
	int  iLimb;
	bool bFront;
};

struct sVehicle
{
	int									iID;

	bool								bAutoDrive;

	bool								bFrontWheelIsPowered;
	bool								bRearWheelIsPowered;
	NxReal								fMaxMotorPower;
	NxReal								fMotorForce;

	NxReal								fSteeringDelta;
	NxReal								fMaxSteeringAngle;
	
	NxReal								fSuspensionSpring;
	NxReal								fSuspensionDamping;
	NxReal								fSuspensionBias;
	NxSpringDesc						spring;

	NxReal								fMass;

	NxMaterialDesc						material;
	NxMaterialIndex 					materialIndex;

	sObject*							pObject;

	NxActor*							pActor;

	NxBodyDesc							bodyDesc;
	NxBoxShapeDesc						boxDesc [ 12 ];
	int									iBoxCount;
	NxCapsuleShapeDesc					capsuleDesc [ 4 ];
	NxActorDesc							actorDesc;

	sVehicleWheelData					wheels [ 4 ];
	int									iWheelCount;

	float								fOldPosX;
	float								fOldPosY;
	float								fOldPosZ;

	float								fSteeringValue;
	float								fSteeringAngle;

	NxArray < PhysX::CarWheelContact > wheelContactPoints;

	float								fWheelMultiplier;
};

std::vector < sVehicle* > pVehicleList;

void dbPhySetVehicleAuto ( int iObject, int iState )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		if ( iState == 0 )
			pVehicle->bAutoDrive = false;

		if ( iState == 1 )
			pVehicle->bAutoDrive = true;
	}
}

void dbPhySetVehicleWheelPower ( int iObject, int iFront, int iRear )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		if ( iFront == 0 ) pVehicle->bFrontWheelIsPowered = false;
		if ( iFront == 1 ) pVehicle->bFrontWheelIsPowered = true;
		if ( iRear  == 0 ) pVehicle->bRearWheelIsPowered  = false;
		if ( iRear  == 1 ) pVehicle->bRearWheelIsPowered  = true;
	}
}

void dbPhySetVehicleMaxMotor ( int iObject, float fMax )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fMaxMotorPower = fMax;
	}
}

void dbPhySetVehicleMotorForce ( int iObject, float fForce )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fMotorForce = fForce;
	}
}

void dbPhySetVehicleSteeringDelta ( int iObject, float fDelta )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fSteeringDelta = fDelta;
	}
}

void dbPhySetVehicleMaxSteeringAngle ( int iObject, float fAngle )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fMaxSteeringAngle = fAngle;
	}
}

void dbPhySetVehicleSuspensionSpring ( int iObject, float fSpring )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fSuspensionSpring = fSpring;
	}
}

void dbPhySetVehicleSuspensionDamping ( int iObject, float fDamping )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fSuspensionDamping = fDamping;
	}
}

void dbPhySetVehicleSuspensionBias ( int iObject, float fBias )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fSuspensionBias = fBias;
	}
}

void dbPhySetVehicleMass ( int iObject, float fMass )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fMass = fMass;
	}
}


void dbPhyTestVehicle ( int iObject )
{
	/*
	dbPhyCreateVehicle		( iObject );

	dbPhyAddVehicleBody		( iObject, 12.0f, 5.0f, 5.0f, 2.0f, 5.0f, 0.0f );

	//float fRadius = 20.0f;
	//float fHeight = 1.0f;

	float fRadius1 = 2.0f;
	float fHeight1 = 0.1f;

	dbPhyAddVehicleWheel	( iObject, 5,  8.5f, 0.0f,  5.0f, fRadius1, fHeight1, 0, 0 );
	dbPhyAddVehicleWheel	( iObject, 6,  8.5f, 0.0f, -5.0f, fRadius1, fHeight1, 0, 0 );
	dbPhyAddVehicleWheel	( iObject, 7, -8.5f, 0.0f, -5.0f, fRadius1, fHeight1, 1, 0 );
	dbPhyAddVehicleWheel	( iObject, 8, -8.5f, 0.0f,  5.0f, fRadius1, fHeight1, 1, 0 );
	
	dbPhySetVehicleAuto		( iObject, 1 );

	dbPhyBuildVehicle		( iObject );
	
	return;
	*/
	
	
	NxReal width  = 1.1f;
	NxReal height = 1.0f;
	NxReal length = 2.2f;

	float fWheelX = 1.5f;
	float fWheelY = 0.0f;
	float fWheelZ = 1.0f;

	
	dbPhyCreateVehicle		( iObject );

	dbPhyAddVehicleBody		( iObject, width, height, length, 0.0f, height, 0.0f );

	// 5.0, 0.1

	//float fRadius = 1.0f;
	//float fHeight = 0.5f;

	float fRadius = 1.0f;
	float fHeight = 0.2f;

	dbPhyAddVehicleWheel	( iObject, 5,  fWheelX, fWheelY,  fWheelZ, fRadius, fHeight, 0, 0 );
	dbPhyAddVehicleWheel	( iObject, 7,  fWheelX, fWheelY, -fWheelZ, fRadius, fHeight, 0, 1 );
	dbPhyAddVehicleWheel	( iObject, 3, -fWheelX, fWheelY, -fWheelZ, fRadius, fHeight, 1, 0 );
	dbPhyAddVehicleWheel	( iObject, 9, -fWheelX, fWheelY,  fWheelZ, fRadius, fHeight, 1, 1 );
	
	dbPhySetVehicleAuto		( iObject, 1 );

	
	//dbPhySetVehicleStaticFriction ( iObject, 12.0f );
	dbPhySetVehicleDynamicFriction ( iObject, 0.4f );
	dbPhySetVehicleStaticFriction ( iObject, 15.0f );
	//dbPhySetVehicleStaticFriction ( iObject, 2.0f );
	//dbPhySetVehicleMaxMotor ( iObject, 2.0f );
	dbPhySetVehicleMaxMotor ( iObject, 500.0f );
	dbPhySetVehicleSteeringDelta ( iObject, 0.25f );
	dbPhySetVehicleStaticFrictionV ( iObject, 0.01f );
	dbPhySetVehicleDynamicFrictionV ( iObject, 0.01f );
	dbPhySetVehicleMaxSteeringAngle ( iObject, NxMath::degToRad ( 60.0f ) );

	//dbPhySetVehicleMass ( iObject, 1.0f );
	
	/*

	//dbPhySetVehicleStaticFriction ( iObject, 12.0f );
	dbPhySetVehicleDynamicFriction ( iObject, 0.5f );
	//dbPhySetVehicleStaticFriction ( iObject, 12.0f );
	dbPhySetVehicleStaticFriction ( iObject, 2.0f );
	dbPhySetVehicleMaxMotor ( iObject, 100.0f );
	dbPhySetVehicleSteeringDelta ( iObject, 0.25f );
	dbPhySetVehicleStaticFrictionV ( iObject, 0.01f );
	dbPhySetVehicleDynamicFrictionV ( iObject, 0.01f );
	dbPhySetVehicleMaxSteeringAngle ( iObject, NxMath::degToRad ( 60.0f ) );

	dbPhySetVehicleMass ( iObject, 50.0f );
	*/

	dbPhyBuildVehicle		( iObject );
	
	/*
	pVehicle->material.restitution			= 0.0f;
	pVehicle->material.staticFriction		= 25.0f;
	pVehicle->material.staticFrictionV		= 0.5f;
	pVehicle->material.dynamicFriction		= 0.8f;
	pVehicle->material.dynamicFrictionV		= 0.5f;
	*/

	sObject* pObject = PhysX::GetObject ( iObject );

	/*
	dbMakeObjectBox			( 10, length * 2 , height * 2, width * 2 );
	dbSetObjectWireframe	( 10, 1 );
	dbOffsetLimb			( 10, 0, 0, height, 0 );
	dbFixObjectPivot		( 10 );


	dbMakeObjectSphere ( 11, 0.2f );
	dbMakeObjectSphere ( 12, 0.2f );
	dbMakeObjectSphere ( 13, 0.2f );
	dbMakeObjectSphere ( 14, 0.2f );

	dbOffsetLimb ( 11, 0,  fWheelX, fWheelY,  fWheelZ );
	dbOffsetLimb ( 12, 0,  fWheelX, fWheelY, -fWheelZ );
	dbOffsetLimb ( 13, 0, -fWheelX, fWheelY, -fWheelZ );
	dbOffsetLimb ( 14, 0, -fWheelX, fWheelY,  fWheelZ );
	*/
};

void dbPhyGetWheelAddContactPoints ( int iObject, PhysX::CarWheelContact contact )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->wheelContactPoints.pushBack ( contact );
	}
}

void dbPhyCreateVehicle ( int iObject )
{
	sVehicle* pVehicle = new sVehicle;

	if ( !pVehicle )
	{
		return;
	}

	pVehicle->iID = iObject;

	pVehicle->actorDesc.setToDefault ( );

	dbPhySetVehicleDefaultProperties ( iObject, pVehicle );

	pVehicleList.push_back ( pVehicle );
}

void dbPhySetVehicleDefaultProperties ( int iObject, sVehicle* pVehicle )
{
	pVehicle->pObject						= PhysX::GetObject ( iObject );

	pVehicle->bAutoDrive					= false;
	pVehicle->bFrontWheelIsPowered			= true;
	pVehicle->bRearWheelIsPowered			= true;
	pVehicle->fMaxMotorPower				= 360.0f;
	pVehicle->fMotorForce					= 0.0f;
	pVehicle->fSteeringDelta				= 0.05f;
	pVehicle->fMaxSteeringAngle				= NxMath::degToRad ( 45.0f );
	pVehicle->fSuspensionSpring				= 70.0f;
	pVehicle->fSuspensionDamping			= 30.0f;
	pVehicle->fSuspensionBias				= -0.25f;
	pVehicle->fMass							= 80;

	pVehicle->spring.spring					= pVehicle->fSuspensionSpring;
	pVehicle->spring.damper					= pVehicle->fSuspensionDamping;
	pVehicle->spring.targetValue			= pVehicle->fSuspensionBias;

	pVehicle->material.restitution			= 0.0f;
	pVehicle->material.staticFriction		= 25.0f;
	pVehicle->material.staticFrictionV		= 0.5f;
	pVehicle->material.dynamicFriction		= 0.8f;
	pVehicle->material.dynamicFrictionV		= 0.5f;
	pVehicle->material.frictionCombineMode	= NX_CM_MULTIPLY;
	pVehicle->material.flags				|= ( NX_MF_SPRING_CONTACT | NX_MF_ANISOTROPIC );
	pVehicle->material.spring				= &pVehicle->spring;
	
	pVehicle->bodyDesc.mass					= pVehicle->fMass;

	pVehicle->iWheelCount					= 0;
	pVehicle->iBoxCount						= 0;

	pVehicle->fOldPosX						= 0.0f;
	pVehicle->fOldPosY						= 0.0f;
	pVehicle->fOldPosZ						= 0.0f;
	pVehicle->fSteeringValue				= 0.0f;
	pVehicle->fSteeringAngle				= 0.0f;

	pVehicle->fWheelMultiplier = 15.0f;

	pVehicle->material.dirOfAnisotropy.set ( 1, 0, 0 );
}

void dbPhySetVehicleWheelMultiplier ( int iObject, float fValue )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fWheelMultiplier = fValue;
	}
}

void dbPhySetVehicleRestitution ( int iObject, float fValue )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->material.restitution = fValue;
	}
}

void dbPhySetVehicleStaticFriction ( int iObject, float fValue )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->material.staticFriction = fValue;
	}
}

void dbPhySetVehicleStaticFrictionV	( int iObject, float fValue )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->material.staticFrictionV = fValue;
	}
}

void dbPhySetVehicleDynamicFriction	( int iObject, float fValue )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->material.dynamicFriction = fValue;
	}
}

void dbPhySetVehicleDynamicFrictionV ( int iObject, float fValue )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->material.dynamicFrictionV = fValue;
	}
}

void dbPhyCreateVehicleMaterial ( sVehicle* pVehicle )
{
	pVehicle->spring.spring					= pVehicle->fSuspensionSpring;
	pVehicle->spring.damper					= pVehicle->fSuspensionDamping;
	pVehicle->spring.targetValue			= pVehicle->fSuspensionBias;

	pVehicle->materialIndex = PhysX::pScene->createMaterial ( pVehicle->material )->getMaterialIndex ( );
}

void dbPhySetDefaultVehiclePosition ( sVehicle* pVehicle )
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
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->boxDesc [ pVehicle->iBoxCount ].dimensions.set ( fSizeX, fSizeY, fSizeZ );
		pVehicle->boxDesc [ pVehicle->iBoxCount ].localPose.t = NxVec3 ( fOriginX, fOriginY, fOriginZ );

		pVehicle->iBoxCount++;
	}
}

void dbPhyAddVehicleWheel ( int iObject, int iLimb, float fWheelX, float fWheelY, float fWheelZ, float fRadius, float fHeight, int iLocation, int iDirection )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->capsuleDesc [ pVehicle->iWheelCount ].radius			= fRadius;
		pVehicle->capsuleDesc [ pVehicle->iWheelCount ].height			= fHeight;
		pVehicle->capsuleDesc [ pVehicle->iWheelCount ].flags			= NX_SWEPT_SHAPE;
		pVehicle->capsuleDesc [ pVehicle->iWheelCount ].userData		= &( pVehicle->wheels [ pVehicle->iWheelCount ] );

		

		pVehicle->capsuleDesc [ pVehicle->iWheelCount ].localPose.M.setColumn ( 1, NxVec3 ( 0, -1,  0 ) );
		pVehicle->capsuleDesc [ pVehicle->iWheelCount ].localPose.M.setColumn ( 2, NxVec3 ( 0,  0, -1 ) );
		pVehicle->capsuleDesc [ pVehicle->iWheelCount ].localPose.t.set       ( fWheelX, fWheelY, fWheelZ );

		if ( iLocation == 0 )
			pVehicle->wheels [ pVehicle->iWheelCount ].bFront = true;
		else
			pVehicle->wheels [ pVehicle->iWheelCount ].bFront = false;

		pVehicle->wheels [ pVehicle->iWheelCount ].iDirection	= iDirection;
		pVehicle->wheels [ pVehicle->iWheelCount ].iLimb		= iLimb;
		pVehicle->iWheelCount++;
	}
}

void dbPhyBuildVehicle ( int iObject )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		PhysX::sPhysXObject* pPhysXObject = new PhysX::sPhysXObject;

		if ( !pPhysXObject )
		{
			return;
		}

		pVehicle->bodyDesc.mass = pVehicle->fMass;

		dbPhyCreateVehicleMaterial ( pVehicle );

		for ( int i = 0; i < pVehicle->iBoxCount; i++ )
		{
			pVehicle->actorDesc.shapes.push_back ( &pVehicle->boxDesc [ i ] );
		}

		for ( int i = 0; i < pVehicle->iWheelCount; i++ )
		{
			pVehicle->capsuleDesc [ i ].materialIndex = pVehicle->materialIndex;
			pVehicle->actorDesc.shapes.pushBack ( &pVehicle->capsuleDesc [ i ] );
		}

		pVehicle->actorDesc.body    = &pVehicle->bodyDesc;
		pVehicle->pActor            = PhysX::pScene->createActor ( pVehicle->actorDesc );
		pPhysXObject->pObject		= PhysX::GetObject ( iObject );
		pPhysXObject->iID			= iObject;
		pPhysXObject->pScene  = PhysX::pScene;
		//pPhysXObject->pActor		= pVehicle->pActor;
		pPhysXObject->pActorList.push_back ( pVehicle->pActor );
		pPhysXObject->type			= PhysX::eTypeVehicle;
		pVehicle->pActor->userData	= ( void* ) pPhysXObject;

		pVehicle->pActor->wakeUp ( 1e10 );

		dbPhySetDefaultVehiclePosition ( pVehicle );
	}
	
	/////////////////////
	/*
	NxReal length = 12.0f;
	NxReal height =  5.0f;
	NxReal width  =  5.0f;

	float fWheelX = 8.5f;
	float fWheelY = 0.0f;
	float fWheelZ = 5.0f;

	dbMakeObjectBox			( 10, length * 2 , height * 2, width * 2 );
	dbSetObjectWireframe	( 10, 1 );
	dbOffsetLimb			( 10, 0, 2, height, 0 );
	dbFixObjectPivot		( 10 );

	dbMakeObjectSphere ( 11, 2.0f );
	dbMakeObjectSphere ( 12, 2.0f );
	dbMakeObjectSphere ( 13, 2.0f );
	dbMakeObjectSphere ( 14, 2.0f );

	dbOffsetLimb ( 11, 0, fWheelX, fWheelY, fWheelZ );
	dbOffsetLimb ( 12, 0, fWheelX, fWheelY, -fWheelZ );
	dbOffsetLimb ( 13, 0, -fWheelX, fWheelY, -fWheelZ );
	dbOffsetLimb ( 14, 0, -fWheelX, fWheelY, fWheelZ );
	*/
	/////////////////////
}

sVehicle* dbPhyGetVehicle ( int iID )
{
	for ( DWORD dwVehicle = 0; dwVehicle < pVehicleList.size ( ); dwVehicle++ )
	{
		if ( pVehicleList [ dwVehicle ]->iID == iID )
			return pVehicleList [ dwVehicle ];
	}

	return NULL;
}

void dbPhySetVehicleWheelRotation ( int iObject, int iWheel, int iDirection )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->wheels [ iWheel ].iDirection = iDirection; 
	}
}

void dbPhySetVehicleSteeringAngle ( int iObject, float fAngle )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fSteeringAngle = fAngle; 
	}
}

void dbPhySetVehicleSteeringValue ( int iObject, float fValue )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->fSteeringValue = fValue; 
	}
}

float GetLimbAngleXEx ( int iID, int iLimbID )
{
	sObject* pObject = PhysX::GetObject ( iID );
	sFrame* pFrame = pObject->ppFrameList [ iLimbID ];
	//UpdateRealtimeFrameVectors ( pObject, pFrame );
	return pFrame->vecRotation.x;
	return 0.0f;
}

DWORD dbPhyGetVehicleSteeringAngleEx ( int iObject )
{
	float fValue = dbPhyGetVehicleSteeringAngle ( iObject );
	return *( DWORD* ) &fValue;
}

DWORD dbPhyGetVehicleSteeringDeltaEx ( int iObject )
{
	float fValue = dbPhyGetVehicleSteeringDelta ( iObject );
	return *( DWORD* ) &fValue;
}

float dbPhyGetVehicleSteeringAngle ( int iObject )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		return pVehicle->fSteeringAngle;
	}
	return 0.0f;

}

float dbPhyGetVehicleSteeringDelta ( int iObject )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		return pVehicle->fSteeringDelta;
	}

	return 0.0f;
}

DWORD dbPhyGetVehicleMotorForceEx ( int iObject )
{
	float fValue = dbPhyGetVehicleMotorForce ( iObject );
	return *( DWORD* ) &fValue;
}

float dbPhyGetVehicleMotorForce ( int iObject )
{
	if ( sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		return pVehicle->fMotorForce;
	}

	return 0.0f;
}

void dbPhyUpdateVehicleWheels ( sVehicle* pVehicle )
{
	sObject* pObject = PhysX::GetObject ( pVehicle->iID );

	float dist = dbPhyGetDistance	( 
										pObject->position.vecPosition.x,
										pObject->position.vecPosition.y,
										pObject->position.vecPosition.z,
										pVehicle->fOldPosX,
										pVehicle->fOldPosY,
										pVehicle->fOldPosZ
									);

	dist *= pVehicle->fWheelMultiplier;

	float fA = dist;
	float fB = dist;
	float fC = dist;
	float fD = dist;

	//pVehicle->wheels [ 1 ].iDirection = 1;
	//pVehicle->wheels [ 3 ].iDirection = 1;

	if ( pVehicle->wheels [ 0 ].iDirection == 1 ) fA = -fA;
	if ( pVehicle->wheels [ 1 ].iDirection == 1 ) fB = -fB;
	if ( pVehicle->wheels [ 2 ].iDirection == 1 ) fC = -fC;
	if ( pVehicle->wheels [ 3 ].iDirection == 1 ) fD = -fD;

	DWORD dwA = PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb );
	DWORD dwB = PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb );
	DWORD dwC = PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb );
	DWORD dwD = PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb );

	float fA1 = *(float*)&dwA;
	float fB1 = *(float*)&dwB;
	float fC1 = *(float*)&dwC;
	float fD1 = *(float*)&dwD;
	
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb, GetLimbAngleXEx ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb ) + fA, 0, 0 );
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb, GetLimbAngleXEx ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb ) + fB, 0, 0 );
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb, GetLimbAngleXEx ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb ) + fC, pVehicle->fSteeringValue, 0 );
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb, GetLimbAngleXEx ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb ) + fD, pVehicle->fSteeringValue, 0 );

	/*
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb, PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb ) + fA, 0, 0 );
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb, PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb ) + fB, 0, 0 );
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb, PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb ) + fC, pVehicle->fSteeringValue, 0 );
	PhysX::RotateLimb ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb, PhysX::GetLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb ) + fD, pVehicle->fSteeringValue, 0 );
	*/

	//dbRotateLimb ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb, dbLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb ) + dist, 0, 0 );
	//dbRotateLimb ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb, dbLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb ) - dist, 0, 0 );
	//dbRotateLimb ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb, dbLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb ) + dist, pVehicle->fSteeringValue, 0 );
	//dbRotateLimb ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb, dbLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb ) - dist, pVehicle->fSteeringValue, 0 );


	//dbRotateLimb ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb, dbLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 0 ].iLimb ) + dist, 0, 0 );
	//dbRotateLimb ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb, dbLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 1 ].iLimb ) + dist, 0, 0 );
	//dbRotateLimb ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb, dbLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 2 ].iLimb ) + dist, pVehicle->fSteeringValue, 0 );
	//dbRotateLimb ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb, dbLimbAngleX ( pVehicle->iID, pVehicle->wheels [ 3 ].iLimb ) + dist, pVehicle->fSteeringValue, 0 );

	pVehicle->fOldPosX = pVehicle->pObject->position.vecPosition.x;
	pVehicle->fOldPosY = pVehicle->pObject->position.vecPosition.y;
	pVehicle->fOldPosZ = pVehicle->pObject->position.vecPosition.z;
}

void dbPhyDriveVehicle ( sVehicle* pVehicle )
{
	if ( pVehicle->bAutoDrive == false )
		return;

	if ( PhysX::GetKeyState ( 200 ) == 1 )
	{
		pVehicle->fMotorForce = pVehicle->fMaxMotorPower;
	}
	else if ( PhysX::GetKeyState ( 208 ) == 1 )
	{
		pVehicle->fMotorForce = 0.0f;
	}
	else if ( pVehicle->fMotorForce != 0.0f )
	{
		pVehicle->fMotorForce = 0.0f;

		if ( pVehicle->fMotorForce < 0.0f )
			pVehicle->fMotorForce = 0.0f;
	}

	if ( PhysX::GetKeyState ( 205 ) == 1 )
	{
		if ( pVehicle->fSteeringAngle > ( -1.0f + pVehicle->fSteeringDelta ) )
			pVehicle->fSteeringAngle -= pVehicle->fSteeringDelta / 2;
	}
	else if ( PhysX::GetKeyState ( 203 ) )
	{
		if ( pVehicle->fSteeringAngle < ( 1.0f - pVehicle->fSteeringDelta ) )
			pVehicle->fSteeringAngle += pVehicle->fSteeringDelta / 2;
	}
	else 
	{
		if ( pVehicle->fSteeringAngle > 0 )
		{
			pVehicle->fSteeringAngle -= pVehicle->fSteeringDelta * 2;

			if ( pVehicle->fSteeringAngle < 0 )
				pVehicle->fSteeringAngle = 0;
		}
		else if ( pVehicle->fSteeringAngle < 0 )
		{
			pVehicle->fSteeringAngle += pVehicle->fSteeringDelta * 2;

			if ( pVehicle->fSteeringAngle > 0 )
				pVehicle->fSteeringAngle = 0;
		}
	}

	pVehicle->fSteeringValue = -( pVehicle->fSteeringAngle * 25 );
}

void dbPhyMoveVehicle ( sVehicle* pVehicle )
{
	NxReal steeringAngle = pVehicle->fSteeringAngle * pVehicle->fMaxSteeringAngle;

	NxArray < PhysX::CarWheelContact >::iterator i = pVehicle->wheelContactPoints.begin ( );

	while ( i != pVehicle->wheelContactPoints.end ( ) )
	{
		PhysX::CarWheelContact& cwc = *i;

		sVehicleWheelData* wheelData = ( sVehicleWheelData* ) ( cwc.wheel->userData );

		// apply to powered wheels only
		if ( wheelData->bFront )
		{
			//steering
			NxMat33 wheelOrientation = cwc.wheel->getLocalOrientation ( );

			wheelOrientation.setColumn ( 0,  NxVec3 ( NxMath::cos ( steeringAngle ), 0,  NxMath::sin ( steeringAngle ) ) );
			wheelOrientation.setColumn ( 2,  NxVec3 ( NxMath::sin ( steeringAngle ), 0, -NxMath::cos ( steeringAngle ) ) );

			cwc.wheel->setLocalOrientation ( wheelOrientation );

			if ( pVehicle->bFrontWheelIsPowered )
			{
				// get the world space orientation:
				wheelOrientation = cwc.wheel->getGlobalOrientation ( );
				NxVec3 steeringDirection;
				wheelOrientation.getColumn ( 0, steeringDirection );

				// the power direction of the front wheel is the wheel's axis as it is steered
				if ( pVehicle->fMotorForce )
					cwc.car->addForceAtPos ( steeringDirection * pVehicle->fMotorForce, cwc.contactPoint );
			}
		}

		if ( !wheelData->bFront && pVehicle->bRearWheelIsPowered )
		{
			//get the orientation of this car:
			NxMat33 m = cwc.car->getGlobalOrientation ( );
			NxVec3 carForwardAxis;
			m.getColumn ( 0, carForwardAxis );

			// the power direction of the rear wheel is always the car's length axis
			cwc.car->addForceAtPos ( carForwardAxis * pVehicle->fMotorForce,cwc.contactPoint );
		}

		i++;
	}

	pVehicle->wheelContactPoints.clear ( );
}

void dbPhyUpdateVehicle ( void )
{
	for ( DWORD dwVehicle = 0; dwVehicle < pVehicleList.size ( ); dwVehicle++ )
	{
		sVehicle* pVehicle = pVehicleList [ dwVehicle ];

		dbPhyUpdateVehicleWheels ( pVehicle );
		dbPhyDriveVehicle		 ( pVehicle );
		dbPhyMoveVehicle		 ( pVehicle );
	}
}

float dbPhyGetDistance ( float x1 , float y1 , float z1 , float x2 , float y2 , float z2)
{
	float dist;
	float dx;
	float dy;
	float dz;

	dx = x2 - x1;
	dy = y2 - y1;
	dz = z2 - z1;

	dist = ( float ) sqrt ( ( dx * dx ) + ( dy * dy ) + ( dz * dz ) );

	return dist;
}