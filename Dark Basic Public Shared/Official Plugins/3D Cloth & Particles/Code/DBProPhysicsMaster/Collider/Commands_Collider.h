#ifndef DBPROEMITTER_COMMANDS_COLLIDER_H
#define DBPROEMITTER_COMMANDS_COLLIDER_H

//Base class
DLLEXPORT void deleteCollider(int id);
//DLLEXPORT void bindColliderToEmitter(int colId, int parId);
//DLLEXPORT void unbindColliderFromEmitter(int colId, int parId);
//DLLEXPORT void bindColliderToCloth(int colId, int clothId);
//DLLEXPORT void unbindColliderFromCloth(int colId, int clothId);
//DLLEXPORT void bindColliderToRagdoll(int colId, int ragdollId);
//DLLEXPORT void unbindColliderFromRagdoll(int colId, int ragdollId);
//DLLEXPORT void bindColliderToVehicle(int colId, int vehicleId);
//DLLEXPORT void unbindColliderFromVehicle(int colId, int vehicleId);
DLLEXPORT void bindColliderToObject(int colId, int objId);
DLLEXPORT void unbindColliderFromObject(int colId, int objId);
DLLEXPORT void setColliderInteraction(int colId, int objectId, DWORD state);
DLLEXPORT void enableCollider(int id, DWORD state);
DLLEXPORT DWORD isColliderEnabled(int id);
DLLEXPORT void setColliderBounce(int colId, int objectId, float bnc);
DLLEXPORT void setColliderFriction(int colId, int objectId, float frc);
DLLEXPORT void setColliderInvert(int colId, int objectId, DWORD state);
DLLEXPORT void setColliderKiller(int colId, int objectId, DWORD state);
DLLEXPORT void setColliderUseColor(int colId, int objectId, DWORD state);
DLLEXPORT void setColliderColor(int colId, int objectId, int R, int G, int B, int A);
DLLEXPORT void setColliderResolve(int colId, int objectId, DWORD state);

//Box
DLLEXPORT void makeCollisionBox(int id);
DLLEXPORT void setCollisionBox(int id, float sx, float sy, float sz);

//Disc
DLLEXPORT void makeCollisionDisc(int id);
DLLEXPORT void setCollisionDisc(int id, float r);

//Plane
DLLEXPORT void makeCollisionPlane(int id);
//DLLEXPORT void setCollisionPlane(int id, float nx, float ny, float nz, float dist);

//Sphere
DLLEXPORT void makeCollisionSphere(int id);
DLLEXPORT void setCollisionSphere(int id, float rad);

//Square
DLLEXPORT void makeCollisionSquare(int id);
DLLEXPORT void setCollisionSquare(int id, float sx, float sy);



#endif