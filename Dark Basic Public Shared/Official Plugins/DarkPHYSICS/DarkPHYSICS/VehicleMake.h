
#ifndef _VEHICLE_MAKE_H_
#define _VEHICLE_MAKE_H_

DARKSDK void dbPhyCreateVehicle					( int iObject );
DARKSDK void dbPhyAddVehicleBody				( int iObject, float fSizeX, float fSizeY, float fSizeZ, float fOriginX, float fOriginY, float fOriginZ );
DARKSDK void dbPhyAddVehicleWheel				( int iObject, int iLimb, float fX, float fY, float fZ, float fRadius, float fHeight, int iLocation, int iDirection );
DARKSDK void dbPhyBuildVehicle					( int iObject );


void dbPhySetVehicleDefaultProperties	( int iObject, PhysXVehicle::sVehicle* pVehicle );
void dbPhyCreateVehicleMaterial			( PhysXVehicle::sVehicle* pVehicle );
void dbPhySetDefaultVehiclePosition		( PhysXVehicle::sVehicle* pVehicle );

#endif _VEHICLE_MAKE_H_