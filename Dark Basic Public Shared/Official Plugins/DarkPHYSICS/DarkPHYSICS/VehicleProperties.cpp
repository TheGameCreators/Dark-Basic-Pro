
#include "globals.h"
#include "vehiclestructure.h"
#include "vehicleproperties.h"


/*
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

	PHY SET VEHICLE BRAKE TORQUE%LF%?dbPhySetVehicleBrakeTorque@@YAXHM@Z%


	PHY SET VEHICLE BRAKE TORQUE%LLF%?dbPhySetVehicleBrakeTorque@@YAXHHM@Z%
	PHY SET VEHICLE MOTOR TORQUE%LLF%?dbPhySetVehicleMotorTorque@@YAXHHM@Z%
	PHY SET VEHICLE STEERING ANGLE%LLF%?dbPhySetVehicleSteeringAngle@@YAXHHM@Z%
	PHY SET VEHICLE WHEEL RADIUS%LLF%?dbPhySetVehicleWheelRadius@@YAXHHM@Z%
	PHY SET VEHICLE SUSPENSION TRAVEL%LLF%?dbPhySetVehicleSuspensionTravel@@YAXHHM@Z%
	PHY SET VEHICLE LONGITUDINAL TIRE FORCE FUNCTION%LLFFFFF%?dbPhySetVehicleLongitudalTireForceFunction@@YAXHHMMMMM@Z%
	PHY SET VEHICLE LATERAL TIRE FORCE FUNCTION%LLFFFFF%?dbPhySetVehicleLateralTireForceFunction@@YAXHHMMMMM@Z%
	PHY SET VEHICLE WHEEL FLAGS%LLF%?dbPhySetVehicleWheelFlags@@YAXHHH@Z%

	DARKSDK void  dbPhySetVehicleBrakeTorque					( int iObject, int iWheel, float fValue );
	DARKSDK void  dbPhySetVehicleMotorTorque					( int iObject, int iWheel, float fValue );
	DARKSDK void  dbPhySetVehicleSteeringAngle					( int iObject, int iWheel, float fValue );
	DARKSDK void  dbPhySetVehicleWheelRadius					( int iObject, int iWheel, float fValue );
	DARKSDK void  dbPhySetVehicleSuspensionTravel				( int iObject, int iWheel, float fValue );
	DARKSDK void  dbPhySetVehicleLongitudalTireForceFunction 	( int iObject, int iWheel, float fExtremumSlip, float fExtremumValue, float fAsymptoteSlip, float fAsymptoteValue, float fStiffnessFactor );
	DARKSDK void  dbPhySetVehicleLateralTireForceFunction	 	( int iObject, int iWheel, float fExtremumSlip, float fExtremumValue, float fAsymptoteSlip, float fAsymptoteValue, float fStiffnessFactor );
	DARKSDK void  dbPhySetVehicleWheelFlags						( int iObject, int iWheel, int iFlag );
*/

void dbPhySetVehicleAuto ( int iObject, int iState )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		if ( iState == 0 )
			pVehicle->bAutoDrive = false;

		if ( iState == 1 )
			pVehicle->bAutoDrive = true;
	}
}

void dbPhySetVehicleWheelPower ( int iObject, int iFront, int iRear )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		if ( iFront == 0 ) pVehicle->bFrontWheelIsPowered = false;
		if ( iFront == 1 ) pVehicle->bFrontWheelIsPowered = true;
		if ( iRear  == 0 ) pVehicle->bRearWheelIsPowered  = false;
		if ( iRear  == 1 ) pVehicle->bRearWheelIsPowered  = true;
	}
}

void dbPhySetVehicleMaxMotor ( int iObject, float fMax )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fMaxMotorPower = fMax;
	}
}

void dbPhySetVehicleMotorForce ( int iObject, float fForce )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fMotorForce = fForce;
	}
}

void dbPhySetVehicleSteeringDelta ( int iObject, float fDelta )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fSteeringDelta = fDelta;
	}
}

void dbPhySetVehicleMaxSteeringAngle ( int iObject, float fAngle )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fMaxSteeringAngle = fAngle;
	}
}

void dbPhySetVehicleSuspensionSpring ( int iObject, float fSpring )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fSuspensionSpring = fSpring;
	}
}

void dbPhySetVehicleSuspensionDamping ( int iObject, float fDamping )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fSuspensionDamping = fDamping;
	}
}

void dbPhySetVehicleSuspensionBias ( int iObject, float fBias )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fSuspensionBias = fBias;
	}
}

void dbPhySetVehicleMass ( int iObject, float fMass )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fMass = fMass;
	}
}

void dbPhyGetWheelAddContactPoints ( int iObject, PhysX::CarWheelContact contact )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject ) )
	{
		pVehicle->wheelContactPoints.pushBack ( contact );
	}
}

void dbPhySetVehicleWheelMultiplier ( int iObject, float fValue )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fWheelMultiplier = fValue;
	}
}

void dbPhySetVehicleRestitution ( int iObject, float fValue )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->material.restitution = fValue;
	}
}

void dbPhySetVehicleStaticFriction ( int iObject, float fValue )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->material.staticFriction = fValue;
	}
}

void dbPhySetVehicleStaticFrictionV	( int iObject, float fValue )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->material.staticFrictionV = fValue;
	}
}

void dbPhySetVehicleDynamicFriction	( int iObject, float fValue )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->material.dynamicFriction = fValue;
	}
}

void dbPhySetVehicleDynamicFrictionV ( int iObject, float fValue )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->material.dynamicFrictionV = fValue;
	}
}

void dbPhySetVehicleWheelRotation ( int iObject, int iWheel, int iDirection )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->wheels [ iWheel ].iDirection = iDirection; 
	}
}

void dbPhySetVehicleSteeringAngle ( int iObject, float fAngle )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fSteeringAngle = fAngle; 
	}
}

void dbPhySetVehicleSteeringValue ( int iObject, float fValue )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		pVehicle->fSteeringValue = fValue; 
	}
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
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		return pVehicle->fSteeringAngle;
	}

	return 0.0f;
}

float dbPhyGetVehicleSteeringDelta ( int iObject )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
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
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		return pVehicle->fMotorForce;
	}

	return 0.0f;
}

void dbPhySetVehicleBrakeTorque ( int iObject, float fValue )
{
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		for ( int iWheel = 0; iWheel < pVehicle->iWheelCount; iWheel++ )
		{
			pVehicle->wheelptr [ iWheel ]->setBrakeTorque ( fValue );
		}
	}
}

void dbPhySetVehicleBrakeTorque ( int iObject, int iWheel, float fValue )
{
	//  - 090806
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		if ( iWheel < pVehicle->iWheelCount )
			pVehicle->wheelptr [ iWheel ]->setBrakeTorque ( fValue );
	}
}

void dbPhySetVehicleMotorTorque ( int iObject, int iWheel, float fValue )
{
	//  - 090806
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		if ( iWheel < pVehicle->iWheelCount )
			pVehicle->wheelptr [ iWheel ]->setMotorTorque ( fValue );
	}
}

void dbPhySetVehicleSteeringAngle ( int iObject, int iWheel, float fValue )
{
	//  - 090806
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		if ( iWheel < pVehicle->iWheelCount )
			pVehicle->wheelptr [ iWheel ]->setSteerAngle ( fValue );
	}
}

void dbPhySetVehicleWheelRadius	( int iObject, int iWheel, float fValue )
{
	//  - 090806
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		if ( iWheel < pVehicle->iWheelCount )
			pVehicle->wheelptr [ iWheel ]->setRadius ( fValue );
	}
}

void dbPhySetVehicleSuspensionTravel ( int iObject, int iWheel, float fValue )
{
	//  - 090806
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		if ( iWheel < pVehicle->iWheelCount )
			pVehicle->wheelptr [ iWheel ]->setSuspensionTravel ( fValue );
	}
}

void dbPhySetVehicleLongitudalTireForceFunction ( int iObject, int iWheel, float fExtremumSlip, float fExtremumValue, float fAsymptoteSlip, float fAsymptoteValue, float fStiffnessFactor )
{
	//  - 090806
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		NxTireFunctionDesc tire;

		tire.extremumSlip    = fExtremumSlip;
		tire.extremumValue   = fExtremumValue;
		tire.asymptoteSlip   = fAsymptoteSlip;
		tire.asymptoteValue  = fAsymptoteValue;
		tire.stiffnessFactor = fStiffnessFactor;

		if ( iWheel < pVehicle->iWheelCount )
			pVehicle->wheelptr [ iWheel ]->setLongitudalTireForceFunction ( tire );
	}
}

void dbPhySetVehicleLateralTireForceFunction ( int iObject, int iWheel, float fExtremumSlip, float fExtremumValue, float fAsymptoteSlip, float fAsymptoteValue, float fStiffnessFactor )
{
	//  - 090806
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		NxTireFunctionDesc tire;

		tire.extremumSlip    = fExtremumSlip;
		tire.extremumValue   = fExtremumValue;
		tire.asymptoteSlip   = fAsymptoteSlip;
		tire.asymptoteValue  = fAsymptoteValue;
		tire.stiffnessFactor = fStiffnessFactor;

		if ( iWheel < pVehicle->iWheelCount )
			pVehicle->wheelptr [ iWheel ]->setLateralTireForceFunction ( tire );
	}
}

void dbPhySetVehicleWheelFlags ( int iObject, int iWheel, int iFlag )
{
	//  - 090806
	if ( PhysXVehicle::sVehicle* pVehicle = dbPhyGetVehicle ( iObject, true ) )
	{
		if ( iWheel < pVehicle->iWheelCount )
			pVehicle->wheelptr [ iWheel ]->setWheelFlags ( iFlag );
	}
}
