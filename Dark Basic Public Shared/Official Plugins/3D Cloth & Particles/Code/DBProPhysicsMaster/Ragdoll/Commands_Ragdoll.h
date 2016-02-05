#ifndef DBPROEMITTER_COMMANDS_RAGDOLL_H
#define DBPROEMITTER_COMMANDS_RAGDOLL_H

//Base Class
DLLEXPORT void freezeRagdoll(int id, DWORD state);
DLLEXPORT void fixRagdollPointToObject(int id, int pntNo, int objectID);
DLLEXPORT void fixRagdollPointToObject(int id, int pntNo, int objectID, float px, float py, float pz);
DLLEXPORT void setRagdollObjectOffset(int id, int partNo, float ox, float oy, float oz);
DLLEXPORT void freeAllRagdollPoints(int id);
DLLEXPORT void drawRagdollDebugLines(int ragdollId, DWORD state);
DLLEXPORT void addRagdollForce(int id, int pntNo, float fx, float fy, float fz);
DLLEXPORT void addRagdollImpulse(int id, int pntNo, float ix, float iy, float iz);


//Basic
DLLEXPORT void makeRagdoll(int id);
DLLEXPORT void fixObjectToRagdoll(int id, int objectID, int partNo);
DLLEXPORT void setRagdollScales(int id, float globalScale, float upperArmScale, float lowerArmScale, float upperLegScale, float lowerLegScale, float chestHeightScale, float waistHeightScale);


#endif