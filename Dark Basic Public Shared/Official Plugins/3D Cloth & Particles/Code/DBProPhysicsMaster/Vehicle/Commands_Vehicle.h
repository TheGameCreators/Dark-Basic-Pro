#ifndef DBPROEMITTER_COMMANDS_VEHICLE_H
#define DBPROEMITTER_COMMANDS_VEHICLE_H

//Base Class
DLLEXPORT void deleteVehicle(int id);
DLLEXPORT void addVehicleCollisionPoint(int id, float px, float py, float pz);
DLLEXPORT void addVehicleCollisionPoints(int id, int objectId);
DLLEXPORT int addVehicleSuspension(int id, float px, float py, float pz, float upx, float upy, float upz, float fwdx, float fwdy, float fwdz);
DLLEXPORT void setVehicleSuspensionLimits(int vehicleId, int suspId, float minDist, float maxDist);
DLLEXPORT void setVehicleSuspensionPhysics(int vehicleId, int suspId, float spring, float damping);
DLLEXPORT void setVehicleWheelSize(int vehicleId, int suspId, float radius, float width);
DLLEXPORT void addVehicleWheelImpulse(int vehicleId, int suspId, float impX, float impY, float impZ);
DLLEXPORT void addVehicleWheelForce(int vehicleId, int suspId, float frcX, float frcY, float frcZ);
DLLEXPORT void setObjectAsVehicleBody(int vehicleId, int objectId);
DLLEXPORT void setObjectAsVehicleWheel(int vehicleId, int suspId, int objectId, DWORD flip);
DLLEXPORT void removeVehicleBodyObject(int vehicleId, int objectId);
DLLEXPORT void removeVehicleWheelObject(int vehicleId, int objectId);
DLLEXPORT void drawVehicleDebugLines(int vehicleId, DWORD state);
DLLEXPORT void setVehicleWheelSteer(int vehicleId, int suspId, float angle);
DLLEXPORT void setVehicleWheelControls(int vehicleId, int suspId, float accel, float brake);
DLLEXPORT void setVehicleCenterOfGravity(int vehicleId, float cx, float cy, float cz);
DLLEXPORT void setVehicleMass(int vehicleId, float mass);
DLLEXPORT void setVehicleMomentOfInertia(int vehicleId, float mx, float my, float mz);

//Basic
DLLEXPORT void makeVehicle(int id);


#endif