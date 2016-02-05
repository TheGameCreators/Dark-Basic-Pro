
#ifndef _CAR_H_
#define _CAR_H_

DARKSDK void dbPhyCreateVehicle					( int iObject );
DARKSDK void dbPhyAddVehicleBody				( int iObject, float fSizeX, float fSizeY, float fSizeZ, float fOriginX, float fOriginY, float fOriginZ );
DARKSDK void dbPhyAddVehicleWheel				( int iObject, int iLimb, float fX, float fY, float fZ, float fRadius, float fHeight, int iLocation, int iDirection );
DARKSDK void dbPhyBuildVehicle					( int iObject );

DARKSDK void dbPhySetVehicleMass				( int iObject, float fMass );
DARKSDK void dbPhySetVehicleAuto				( int iObject, int iState );
DARKSDK void dbPhySetVehicleMaxMotor			( int iObject, float fMax );
DARKSDK void dbPhySetVehicleMotorForce			( int iObject, float fForce );
DARKSDK void dbPhySetVehicleWheelPower			( int iObject, int iFront, int iRear );
DARKSDK void dbPhySetVehicleSteeringDelta		( int iObject, float fDelta );
DARKSDK void dbPhySetVehicleMaxSteeringAngle	( int iObject, float fAngle );
DARKSDK void dbPhySetVehicleWheelRotation		( int iObject, int iWheel, int iDirection );
DARKSDK void dbPhySetVehicleSteeringAngle		( int iObject, float fAngle );
DARKSDK void dbPhySetVehicleSteeringValue		( int iObject, float fValue );

DARKSDK void dbPhySetVehicleSuspensionSpring	( int iObject, float fSpring );
DARKSDK void dbPhySetVehicleSuspensionDamping	( int iObject, float fDamping );
DARKSDK void dbPhySetVehicleSuspensionBias		( int iObject, float fBias );

DARKSDK void dbPhySetVehicleRestitution			( int iObject, float fValue );
DARKSDK void dbPhySetVehicleStaticFriction		( int iObject, float fValue );
DARKSDK void dbPhySetVehicleStaticFrictionV		( int iObject, float fValue );
DARKSDK void dbPhySetVehicleDynamicFriction		( int iObject, float fValue );
DARKSDK void dbPhySetVehicleDynamicFrictionV	( int iObject, float fValue );

DARKSDK void dbPhySetVehicleWheelMultiplier		( int iObject, float fValue );

DARKSDK DWORD dbPhyGetVehicleSteeringAngleEx	( int iObject );
DARKSDK DWORD dbPhyGetVehicleSteeringDeltaEx	( int iObject );
DARKSDK float dbPhyGetVehicleSteeringAngle		( int iObject );
DARKSDK float dbPhyGetVehicleSteeringDelta		( int iObject );

DARKSDK DWORD dbPhyGetVehicleMotorForceEx		( int iObject );
DARKSDK float dbPhyGetVehicleMotorForce		( int iObject );


void dbPhyUpdateVehicle					( void );
void dbPhyTestVehicle					( int iObject );

#endif _CAR_H_