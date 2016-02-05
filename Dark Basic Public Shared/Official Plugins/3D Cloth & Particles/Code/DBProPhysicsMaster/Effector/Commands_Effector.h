#ifndef DBPROEMITTER_COMMANDS_EFFECTOR_H
#define DBPROEMITTER_COMMANDS_EFFECTOR_H

//Base class
DLLEXPORT void deleteEffector(int id);
//DLLEXPORT void bindEffectorToEmitter(int effId, int parId);
//DLLEXPORT void unbindEffectorFromEmitter(int effId, int parId);
//DLLEXPORT void bindEffectorToCloth(int effId, int clothId);
//DLLEXPORT void unbindEffectorFromCloth(int effId, int clothId);
//DLLEXPORT void bindEffectorToRagdoll(int effId, int ragdollId);
//DLLEXPORT void unbindEffectorFromRagdoll(int effId, int ragdollId);
//DLLEXPORT void bindEffectorToVehicle(int effId, int vehicleId);
//DLLEXPORT void unbindEffectorFromVehicle(int effId, int vehicleId);
DLLEXPORT void bindEffectorToObject(int effId, int objId);
DLLEXPORT void unbindEffectorFromObject(int effId, int objId);
DLLEXPORT void setEffectorInteraction(int effId, int objectId, float pc);
DLLEXPORT void enableEffector(int id, DWORD state);
DLLEXPORT DWORD isEffectorEnabled(int id);
DLLEXPORT void setEffectorFallOff(int effId, int objectId, DWORD state);
DLLEXPORT void setEffectorFallOffPerParticle(int effId, int objectId, DWORD state);
DLLEXPORT void useEffectorSphereFalloff(int effId, int objectId);
DLLEXPORT void useEffectorCylinderFalloff(int effId, int objectId);
DLLEXPORT void setEffectorFalloffRadius(int effId, int objectId, float mnRad, float mxRad);
DLLEXPORT void setEffectorFalloffHeight(int effId, int objectId, float mnHgt, float mxHgt);

//Chaos
DLLEXPORT void makeChaosEffector(int id);
DLLEXPORT void setChaosEffector(int id, float chaos);

//Damping
DLLEXPORT void makeDampingEffector(int id);
DLLEXPORT void setDampingEffector(int id, float damping);

//Drag
DLLEXPORT void makeDragEffector(int id);
DLLEXPORT void setDragEffector(int id, float drag);

//Flock
DLLEXPORT void makeFlockEffector(int id);
DLLEXPORT void setFlockEffector(int id, float flock);
DLLEXPORT void setFlockEffectorUseCenter(int id, DWORD state);

//Force
DLLEXPORT void makeForceEffector(int id);
DLLEXPORT void setForceEffector(int id, float fx, float fy, float fz);

//Gravity
DLLEXPORT void makeGravityEffector(int id);
DLLEXPORT void setGravityEffector(int id, float x, float y, float z);

//Point
DLLEXPORT void makePointEffector(int id);
DLLEXPORT void setPointEffector(int id, float frc);
DLLEXPORT void setPointEffectorFalloffNone(int id);
DLLEXPORT void setPointEffectorFalloffInverse(int id);
DLLEXPORT void setPointEffectorFalloffInverseSquare(int id);
DLLEXPORT void setPointEffectorUseKillRadius(int id, DWORD state);
DLLEXPORT void setPointEffectorKillRadius(int id, float rad);

//Vortex
DLLEXPORT void makeVortexEffector(int id);
DLLEXPORT void setVortexEffector(int id, float vortex);

//Wind
DLLEXPORT void makeWindEffector(int id);
DLLEXPORT void setWindEffector(int id, float fx, float fy, float fz);

//WrapRound
DLLEXPORT void makeWrapRoundEffector(int id);
DLLEXPORT void setWrapRoundEffector(int id, float sx, float sy, float sz);


//Size
DLLEXPORT void makeSizeEffector(int id);
DLLEXPORT int sizeEffectorAddKey(int id, float time, float size);
DLLEXPORT void sizeEffectorDeleteKeyAtTime(int id, float time);
DLLEXPORT void sizeEffectorDeleteKey(int id, int n);
DLLEXPORT int sizeEffectorGetNumKeys(int id);
DLLEXPORT DWORD sizeEffectorGetKeyValue(int id, int n);
DLLEXPORT DWORD sizeEffectorGetKeyTime(int id, int n);
DLLEXPORT void sizeEffectorUseOnCollide(int id, DWORD state);

//Color
DLLEXPORT void makeColorEffector(int id);
DLLEXPORT int colorEffectorAddKey(int id, float time, int r, int g, int b, int a);
DLLEXPORT void colorEffectorDeleteKeyAtTime(int id, float time);
DLLEXPORT void colorEffectorDeleteKey(int id, int n);
DLLEXPORT int colorEffectorGetNumKeys(int id);
DLLEXPORT int colorEffectorGetKeyRedValue(int id, int n);
DLLEXPORT int colorEffectorGetKeyGreenValue(int id, int n);
DLLEXPORT int colorEffectorGetKeyBlueValue(int id, int n);
DLLEXPORT int colorEffectorGetKeyAlphaValue(int id, int n);
DLLEXPORT DWORD colorEffectorGetKeyTime(int id, int n);
DLLEXPORT void colorEffectorUseOnCollide(int id, DWORD state);



#endif