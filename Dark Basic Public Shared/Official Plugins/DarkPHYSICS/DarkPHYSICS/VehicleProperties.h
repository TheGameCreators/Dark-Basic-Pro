
#ifndef _VEHICLE_PROPERTIES_H
#define _VEHICLE_PROPERTIES_H

DARKSDK void dbPhySetVehicleMass							( int iObject, float fMass );
DARKSDK void dbPhySetVehicleAuto							( int iObject, int iState );
DARKSDK void dbPhySetVehicleMaxMotor						( int iObject, float fMax );
DARKSDK void dbPhySetVehicleMotorForce						( int iObject, float fForce );
DARKSDK void dbPhySetVehicleWheelPower						( int iObject, int iFront, int iRear );
DARKSDK void dbPhySetVehicleSteeringDelta					( int iObject, float fDelta );
DARKSDK void dbPhySetVehicleMaxSteeringAngle				( int iObject, float fAngle );
DARKSDK void dbPhySetVehicleWheelRotation					( int iObject, int iWheel, int iDirection );
DARKSDK void dbPhySetVehicleSteeringAngle					( int iObject, float fAngle );
DARKSDK void dbPhySetVehicleSteeringValue					( int iObject, float fValue );

DARKSDK void dbPhySetVehicleSuspensionSpring				( int iObject, float fSpring );
DARKSDK void dbPhySetVehicleSuspensionDamping				( int iObject, float fDamping );
DARKSDK void dbPhySetVehicleSuspensionBias					( int iObject, float fBias );

DARKSDK void dbPhySetVehicleRestitution						( int iObject, float fValue );
DARKSDK void dbPhySetVehicleStaticFriction					( int iObject, float fValue );
DARKSDK void dbPhySetVehicleStaticFrictionV					( int iObject, float fValue );
DARKSDK void dbPhySetVehicleDynamicFriction					( int iObject, float fValue );
DARKSDK void dbPhySetVehicleDynamicFrictionV				( int iObject, float fValue );

DARKSDK void dbPhySetVehicleWheelMultiplier					( int iObject, float fValue );

DARKSDK DWORD dbPhyGetVehicleSteeringAngleEx				( int iObject );
DARKSDK DWORD dbPhyGetVehicleSteeringDeltaEx				( int iObject );
DARKSDK float dbPhyGetVehicleSteeringAngle					( int iObject );
DARKSDK float dbPhyGetVehicleSteeringDelta					( int iObject );
DARKSDK DWORD dbPhyGetVehicleMotorForceEx					( int iObject );
DARKSDK float dbPhyGetVehicleMotorForce						( int iObject );

DARKSDK void  dbPhySetVehicleBrakeTorque					( int iObject, float fValue );

DARKSDK void  dbPhySetVehicleBrakeTorque					( int iObject, int iWheel, float fValue );
DARKSDK void  dbPhySetVehicleMotorTorque					( int iObject, int iWheel, float fValue );
DARKSDK void  dbPhySetVehicleSteeringAngle					( int iObject, int iWheel, float fValue );
DARKSDK void  dbPhySetVehicleWheelRadius					( int iObject, int iWheel, float fValue );
DARKSDK void  dbPhySetVehicleSuspensionTravel				( int iObject, int iWheel, float fValue );
DARKSDK void  dbPhySetVehicleLongitudalTireForceFunction 	( int iObject, int iWheel, float fExtremumSlip, float fExtremumValue, float fAsymptoteSlip, float fAsymptoteValue, float fStiffnessFactor );
DARKSDK void  dbPhySetVehicleLateralTireForceFunction	 	( int iObject, int iWheel, float fExtremumSlip, float fExtremumValue, float fAsymptoteSlip, float fAsymptoteValue, float fStiffnessFactor );
DARKSDK void  dbPhySetVehicleWheelFlags						( int iObject, int iWheel, int iFlag );


#endif _VEHICLE_PROPERTIES_H