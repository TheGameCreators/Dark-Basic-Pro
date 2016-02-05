#include "stdafx.h"

DLLEXPORT void deleteVehicle(int id)
{
	ALERT_ON_PERR(physics->deleteVehicle(id),PCMD_DELETE_VEHICLE);
}


DLLEXPORT void addVehicleCollisionPoint(int id, float px, float py, float pz)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(id,tmp),PCMD_ADD_VEHICLE_COLLISION_POINT);

	tmp->addNewCollisionPoint(Vector3(px,py,pz));
}

DLLEXPORT void addVehicleCollisionPoints(int id, int objectId)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(id,tmp),PCMD_ADD_VEHICLE_COLLISION_POINTS);

	RETURN_ON_PERR(tmp->addObjectToCollisionPoints(objectId),PCMD_ADD_VEHICLE_COLLISION_POINTS);
}


DLLEXPORT int addVehicleSuspension(int id, float px, float py, float pz,
								   float upx, float upy, float upz,
								   float fwdx, float fwdy, float fwdz)
{
	VehiclePtr tmp;
	RETURNVALUE_ON_PERR(physics->getVehicle(id,tmp),PCMD_ADD_VEHICLE_SUSPENSION,-1);

	int suspID=0;
	RETURNVALUE_ON_PERR(tmp->addSuspension(Vector3(px,py,pz),Vector3(upx,upy,upz),Vector3(fwdx,fwdy,fwdz),&suspID),PCMD_ADD_VEHICLE_SUSPENSION,-1);
	return suspID;
}

DLLEXPORT void setVehicleSuspensionLimits(int vehicleId, int suspId, float minDist, float maxDist)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_VEHICLE_SUSPENSION_LIMITS);

	RETURN_ON_PERR(tmp->setSuspensionLimits(suspId,minDist,maxDist),PCMD_SET_VEHICLE_SUSPENSION_LIMITS);
}

DLLEXPORT void setVehicleSuspensionPhysics(int vehicleId, int suspId, float spring, float damping)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_VEHICLE_SUSPENSION_PHYSICS);

	RETURN_ON_PERR(tmp->setSuspensionPhysics(suspId,spring,damping),PCMD_SET_VEHICLE_SUSPENSION_PHYSICS);
}

DLLEXPORT void setVehicleWheelSize(int vehicleId, int suspId, float radius, float width)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_VEHICLE_WHEEL_SIZE);

	RETURN_ON_PERR(tmp->setWheelSize(suspId,radius,width),PCMD_SET_VEHICLE_WHEEL_SIZE);
}


DLLEXPORT void addVehicleWheelImpulse(int vehicleId, int suspId, float impX, float impY, float impZ)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_ADD_VEHICLE_WHEEL_IMPULSE);

	RETURN_ON_PERR(tmp->addWheelImpulse(suspId,Vector3(impX,impY,impZ)),PCMD_ADD_VEHICLE_WHEEL_IMPULSE);
}


DLLEXPORT void addVehicleWheelForce(int vehicleId, int suspId, float frcX, float frcY, float frcZ)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_ADD_VEHICLE_WHEEL_FORCE);

	RETURN_ON_PERR(tmp->addWheelForce(suspId,Vector3(frcX,frcY,frcZ)),PCMD_ADD_VEHICLE_WHEEL_FORCE);
}



DLLEXPORT void setObjectAsVehicleBody(int vehicleId, int objectId)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_OBJECT_AS_VEHICLE_BODY);

	RETURN_ON_PERR(tmp->setBodyObject(objectId),PCMD_SET_OBJECT_AS_VEHICLE_BODY);
}

DLLEXPORT void setObjectAsVehicleWheel(int vehicleId, int suspId, int objectId, DWORD flip)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_OBJECT_AS_VEHICLE_WHEEL);

	RETURN_ON_PERR(tmp->setWheelObject(suspId,objectId,flip?true:false),PCMD_SET_OBJECT_AS_VEHICLE_WHEEL);
}

DLLEXPORT void removeVehicleBodyObject(int vehicleId, int objectId)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_REMOVE_VEHICLE_BODY_OBJECT);

	RETURN_ON_PERR(tmp->removeBodyObject(objectId),PCMD_REMOVE_VEHICLE_BODY_OBJECT);
}

DLLEXPORT void removeVehicleWheelObject(int vehicleId, int objectId)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_REMOVE_VEHICLE_WHEEL_OBJECT);

	RETURN_ON_PERR(tmp->removeWheelObject(objectId),PCMD_REMOVE_VEHICLE_WHEEL_OBJECT);
}

DLLEXPORT void drawVehicleDebugLines(int vehicleId, DWORD state)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_DRAW_VEHICLE_DEBUG_LINES);

	tmp->setDebugLines(state?true:false);
}

DLLEXPORT void setVehicleWheelSteer(int vehicleId, int suspId, float angle)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_VEHICLE_WHEEL_STEER);

	RETURN_ON_PERR(tmp->setWheelSteer(suspId,angle),PCMD_SET_VEHICLE_WHEEL_STEER);
}

DLLEXPORT void setVehicleWheelControls(int vehicleId, int suspId, float accel, float brake)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_VEHICLE_WHEEL_CONTROLS);

	RETURN_ON_PERR(tmp->setWheelControls(suspId,accel,brake),PCMD_SET_VEHICLE_WHEEL_CONTROLS);
}

DLLEXPORT void setVehicleCenterOfGravity(int vehicleId, float cx, float cy, float cz)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_VEHICLE_CENTER_OF_GRAVITY);

	tmp->setVehicleCenterOfGravity(cx,cy,cz);
}

DLLEXPORT void setVehicleMass(int vehicleId, float mass)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_VEHICLE_MASS);

	RETURN_ON_PERR(tmp->setVehicleMass(mass),PCMD_SET_VEHICLE_MASS);
}

DLLEXPORT void setVehicleMomentOfInertia(int vehicleId, float mx, float my, float mz)
{
	VehiclePtr tmp;
	RETURN_ON_PERR(physics->getVehicle(vehicleId,tmp),PCMD_SET_VEHICLE_MOMENT_OF_INERTIA);
	RETURN_ON_PERR(tmp->setVehicleMomentOfIntertia(mx,my,mz),PCMD_SET_VEHICLE_MOMENT_OF_INERTIA);
}


