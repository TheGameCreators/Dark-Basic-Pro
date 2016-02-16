#pragma once
#ifndef DARKPHY2
#define DARKPHY2


// ********************** Dark Physics 2 *********************** //

// GDK Header file, all inline functions are a straight 
// wrap of the core Physics object.

#define WIN32_LEAN_AND_MEAN
#include "Defines.h"
#include "Physics.h"
#include <windows.h>
#include "MathUtil.h"
#include "GlobStruct.h"
#include "D3DUtil.h"

#ifdef COMPILE_FOR_GDK         // GDK Lib
extern GlobStruct* g_pGlob;
#include "DarkGDK.h"
#define EXPORT
#define EXPORTC

#else                          // DBPro(dll)
  #include "DarkPhy2(dbpro).h"
  #define bool DWORD
#endif

//CORE
EXPORT  bool  dynStart(void);
EXPORTC void  dynStop(void);
EXPORTC int   dynGetCurrentScene(void);
EXPORTC void  dynSetCurrentScene(int sceneID);
EXPORT  bool  dynMakeScene(int sceneID);
EXPORTC void  dynDeleteScene(int sceneID);
EXPORTC int   dynGetSceneCount(void);
EXPORTC void  dynUpdate();
EXPORTC void  dynFetchResults();
EXPORT  void  dynSimulate();
EXPORTC void  dynSetGravity(float x, float y, float z);
EXPORTC void  dynVisualDebuggerOn();
EXPORTC void  dynVisualDebuggerOff();
EXPORTC bool  dynVisualDebuggerConnected();
EXPORTC void  dynDebugRender();
EXPORTC void  dynSetParameter(int parameter, float value);

EXPORTC void  dynSetScaleMultiplier(float scaleMultiplier);
EXPORTC DWORDFLOAT dynGetScaleMultiplier();  //DBPRO?

//*******************  SHAPES  ***************************//
//V1.1 CHANGED FROM EXPORTC 
EXPORT  void  dynShapeSetLocPos(int objectID, int shapeID, float posX, float posY, float posZ);
EXPORT  void  dynShapeSetLocRot(int objectID, int shapeID, float angleX, float angleY, float angleZ);
EXPORT  void  dynShapeSetGlobPos(int objectID, int shapeID, float posX, float posY, float posZ);
EXPORT  void  dynShapeSetGlobRot(int objectID, int shapeID, float angleX, float angleY, float angleZ);


//load vehicle
//load actor




//*****************  CONSTRUCTION  ***********************//
//RELEASE
EXPORT void  dynDeleteActor(int actorID);
EXPORT void  dynDeleteActor(int actorID, int limbID);
EXPORT void  dynSetPosition(int actorID, float x, float y, float z);
EXPORT void  dynSetRotation(int actorID, float x, float y, float z);

// GROUND PLANE
EXPORTC bool  dynMakeGroundPlane();
// BOX
EXPORT  bool  dynMakeBox(int objectID, float density = 10.0f);
EXPORT  bool  dynMakeBox(int objectID, int limbID, float sizeX, float sizeY, float sizeZ);

// CONVEX
  //cook//
EXPORT  bool  dynCookConvexMesh(int objectID, char* file);
EXPORT  bool  dynCookConvexMesh(int objectID, int limbID, char* file);
EXPORTC bool  dynLoadConvexMesh(char* file, int convexMeshID);
EXPORT  bool  dynCookConvexMesh(int objectID, int convexMeshID);
EXPORT  bool  dynCookConvexMesh(int objectID, int limbID, int convexMeshID);
  //make//
EXPORT  bool  dynMakeConvexMesh(int objectID, int convexMeshID, float density); 
EXPORT  bool  dynMakeConvexMesh(int objectID, int limbID, int convexMeshID, float density); 
EXPORTC bool  dynDeleteConvexMesh(int convexMeshID);
  //shape//
EXPORT  int   dynMakeShapeConvex(int objectID, int convexMeshID);
EXPORT  int   dynMakeShapeConvex(int objectID, int limbID, int convexMeshID);


//TRIANGLE MESH
  //cook, save and load
EXPORT  bool  dynCookTriangleMesh(int objectID, char* file);
EXPORT  bool  dynCookTriangleMesh(int objectID, int limbID, char* file);
EXPORTC bool  dynLoadTriangleMesh(char* file, int triangleMeshID);
  //cook
EXPORT  bool  dynCookTriangleMesh(int objectID, int triangleMeshID);
EXPORT  bool  dynCookTriangleMesh(int objectID, int limbID, int triangleMeshID);

EXPORT  bool  dynMakeTriangleMesh(int objectID, int triangleMeshID); 
EXPORT  bool  dynMakeTriangleMesh(int objectID, int limbID, int triangleMeshID); 
EXPORTC bool  dynDeleteTriangleMesh(int triangleMeshID);

 //shape//
EXPORT  int   dynMakeShapeTriangleMesh(int objectID, int triangleMeshID);
EXPORT  int   dynMakeShapeTriangleMesh(int objectID, int limbID, int triangleMeshID);

//ADD SHAPES//
EXPORT  bool  dynAddShapes(int actorID);
EXPORT  bool  dynAddShapes(int actorID, int limbID);


//RAGDOLL
EXPORTC void  dynSetLimbs(int objectID, bool physics);
EXPORTC void  dynSetActorToLimb(int objectID, int limbID);
EXPORTC void  dynSetActorsToLimbs(int objectID);

//FORCES
EXPORT  void  dynAddForce(int objectID, float x, float y, float z);
EXPORT  void  dynAddLocalForce(int objectID, float x, float y, float z);
EXPORT  void  dynAddTorque(int objectID, float x, float y, float z);
EXPORT  void  dynAddLocalTorque(int objectID, float x, float y, float z);

//SPHERE
EXPORT  bool  dynMakeSphere(int objectID, float density = 10.0f);
//EXPORT  bool  dynMakeSphere(int objectID, int limbID, float sizeX, float sizeY, float sizeZ);

//MATERIAL
EXPORTC bool  dynMakeMaterial(int materialID, float restitution, float staticFriction, float dynamicFriction);
EXPORT  void  dynSetMaterial(int objectID, int shapeIndex, int materialID);
//EXPORT  void dynSetMaterial(int objectID, int limbID, int shapeIndex, int materialID);
EXPORT  int   dynGetMaterial(int objectID, int shapeIndex);
//EXPORT  int   dynGetMaterial(int objectID, int limbID, int shapeIndex);




//LIMB FORCES
EXPORT  void  dynAddForce(int objectID, int limbID, float x, float y, float z);
EXPORT  void  dynAddLocalForce(int objectID, int limbID, float x, float y, float z);
EXPORT  void  dynAddTorque(int objectID, int limbID, float x, float y, float z);
EXPORT  void  dynAddLocalTorque(int objectID, int limbID, float x, float y, float z);

EXPORT  bool  dynMakeCapsule(int objectID, float height, float radius, float density);
//VELOCITY
EXPORT  void  dynGetAngularVelocity(int objectID, int vector3ID);
EXPORT  void  dynGetLinearVelocity(int objectID, int vector3ID);
EXPORT  void  dynSetAngularVelocity(int objectID, float x, float y, float z);
EXPORT  void  dynSetLinearVelocity(int objectID, float x, float y, float z);
//EXPORT  void  dynGetAngularVelocity(int objectID, int limbID, int vector3ID);
//EXPORT  void  dynGetLinearVelocity(int objectID, int limbID, int vector3ID);
//EXPORT  void  dynSetAngularVelocity(int objectID, int limbID, float x, float y, float z);
//EXPORT  void  dynSetLinearVelocity(int objectID, int limbID, float x, float y, float z);

//Damping?
//phySetPosition, dynSetRotation *done above
EXPORT  void  dynSetMaxAngularVelocity(int objectID, float value);
EXPORT  void  dynSetLinearMomentum(int objectID, float x, float y, float z);
EXPORT  void  dynGetLinearMomentum(int objectID, int vector3ID);
EXPORT  void  dynSetAngularMomentum(int objectID, float x, float y, float z);
EXPORT  void  dynGetAngularMomentum(int objectID, int vector3ID);

EXPORT  void  dynSetLinearDamping(int objectID, float value);
EXPORT  void  dynSetAngularDamping(int objectID, float value);

EXPORT  DWORDFLOAT dynGetMass(int objectID);
EXPORT  DWORDFLOAT dynGetMaxAngularVelocity(int objectID);
EXPORT  DWORDFLOAT dynGetAngularDamping(int objectID);
EXPORT  DWORDFLOAT dynGetLinearDamping(int objectID);
EXPORTC DWORDFLOAT dynGetParameter(int parameter);

EXPORT void dynSetMass(int objectID, float mass);
EXPORT bool dynUpdateMassFromShapes(int objectID, float density, float totalMass);

EXPORTC void  dynSetLimb(int objectID, int limbID, bool physics);



//HEIGHTFIELD
EXPORTC  bool dynMakeHeightFieldDesc(int ID, int columns, int rows, float verticalExtent, float thickness, float convexEdgeThreshold);
EXPORT  void dynHeightFieldDescSetData(int heightFieldDescID, int row, int column, int height);
EXPORT  void dynHeightFieldDescSetData(int heightFieldDescID, int row, int column, int height, int materialIndex0, int materialIndex1, int tessFlag);
EXPORTC  bool dynMakeHeightField(int ID, int heightFieldDescID); 
EXPORT  bool dynInstanceHeightField(int objectID, int heightFieldID, float heightScale, float rowScale, float columnScale);
EXPORT  bool dynInstanceHeightField(int objectID, int heightFieldID, float heightScale, float rowScale, float columnScale, int materialIndexHighBits, int holeMaterial);
EXPORTC void dynDeleteHeightFieldDesc(int ID);
EXPORTC void dynDeleteHeightField(int ID);

//CHARACTER CONTROLLER
EXPORT  bool dynMakeControllerCapsule(int objectID, float yOffset);
EXPORT  bool dynMakeControllerBox(int objectID, float yOffset);
EXPORT  bool dynMakeControllerCapsule(int objectID, float yOffset, float slopeLimit, float stepOffset);
EXPORT  bool dynMakeControllerBox(int objectID, float yOffset, float slopeLimit, float stepOffset);
EXPORTC void dynControllerSetStepOffset(int ID, float stepOffset);
//EXPORTC void dynControllerSetSlopeLimit(int ID, float slopeLimit);
EXPORTC void dynControllerBoxSetExtents(int ID, float x, float y, float z);
EXPORTC void dynControllerCapsuleSetRadius(int ID, float radius);
EXPORTC void dynControllerCapsuleSetHeight(int ID, float height);
EXPORTC void dynControllerSetYOffset(int ID, float yOffset);
//EXPORTC void dynControllerReportSceneChanged(); //not done
//EXPORTC void dynControllerMove(int ID, float x, float y, float z);//Overload this(collision groups etc) 
EXPORT  int dynControllerMove(int ID, float x, float y, float z);
EXPORTC void dynDeleteController(int ID);

//Collision Groups
EXPORTC void dynGroupSetCollision(int groupA, int groupB, bool enable); //--shapes only?
EXPORT  void dynSetGroup(int actorID, int group); //-- Actors
EXPORTC void dynGroupPairSetFlags(int groupA, int groupB, int flag);

//Contact reports
EXPORT void dynActorPairSetFlags(int actorA, int actorB, int flag);
EXPORT int  dynContactGetData();
EXPORT int  dynContactGetActorA();
EXPORT int  dynContactGetActorB();
EXPORT int  dynContactGetFlag();

//Raycast
EXPORT int dynRaycastAllShapes(int origVec3ID, int dirVec3ID);
EXPORT int dynRaycastHitGetData();
EXPORT int dynRaycastHitGetObject();

EXPORT DWORDFLOAT dynRaycastHitGetDistance();
EXPORT void  dynRaycastHitGetWorldImpact(int vector3ID);
EXPORT void  dynRaycastHitGetWorldNormal(int vector3ID);
EXPORT DWORDFLOAT dynRaycastHitGetU();
EXPORT DWORDFLOAT dynRaycastHitGetV();

EXPORT int dynRaycastAllShapes(int origVec3ID, int dirVec3ID, int flag, int groups);
EXPORT int dynRaycastHitGetLimb();
EXPORT void dynRaycastSetMaxDistance(float distance);
EXPORT DWORDFLOAT dynRaycastGetMaxDistance();

EXPORT int dynRaycastAllBounds(int origVec3ID, int dirVec3ID, int flag, int groups);
EXPORT bool dynRaycastAnyShape(int origVec3ID, int dirVec3ID, int flag, int groups);
EXPORT bool dynRaycastAnyBounds(int origVec3ID, int dirVec3ID, int flag, int groups);
EXPORT bool dynRaycastClosestShape(int origVec3ID, int dirVec3ID, int flag, int groups);
EXPORT bool dynRaycastClosestBounds(int origVec3ID, int dirVec3ID, int flag, int groups);

//-- DBPro not done
//EXPORTC bool dyn

//--General
EXPORTC  void dynPickGetRayDirection(int vector3ID);
EXPORTC  void dynSetToNewObject(int newObjectID, int newLimbID, int oldObjectID, int oldLimbID);
EXPORT   bool dynIsDynamic(int objectID);
EXPORT   void dynSetCMassLocPos(int objectID, float x, float y, float z);

EXPORTC  void dynPick(int cameraID, int screenX, int screenY, int flag);
EXPORTC  void dynPickDeselect();
EXPORTC  void dynPickDepth(int delta);
EXPORTC  void dynPickSetScreenDim(int screenWidth, int screenHeight);

EXPORT   void dynSimulate(float timeStep);
EXPORTC  void dynSetTiming(float maxTimeStep, int maxIter, int timeStepMethod);
EXPORTC  int  dynTriangleMeshGetReferenceCount(int meshID);
EXPORTC  int  dynConvexMeshGetReferenceCount(int meshID);

EXPORT void dynAddLocalForceAtLocalPos(int objectID, float x, float y, float z, float posX, float posY, float posZ);



EXPORTC void dynControllerCollectCHits(bool collect);
EXPORTC void dynControllerCollectSHits(bool collect);
//--Controller Hits
EXPORTC int  dynControllerCHitGetData();
EXPORTC int  dynControllerCHitGetControllerA();
EXPORTC int  dynControllerCHitGetControllerB();
//--Shape Hits
EXPORTC int  dynControllerSHitGetData();
EXPORTC int  dynControllerSHitGetShape();
EXPORTC void dynControllerSHitGetDirection(int vector3ID);
EXPORTC DWORDFLOAT dynControllerSHitGetLength(); //
EXPORTC int  dynControllerSHitGetController();
EXPORTC void dynControllerSHitGetWorldNormal(int vector3ID);
EXPORTC void dynControllerSHitGetWorldPos(int vector3ID);
EXPORTC void dynControllerSetPosition(int ID, float x, float y, float z);
EXPORTC void dynControllerGetPosition(int ID, int vector3ID);

//-- CCD
EXPORTC bool dynMakeCCDSkeletonBox(int objectID, int limbID);
EXPORTC void dynDeleteCCDSkeleton(int objectID, int limbID);
EXPORTC void dynCCDSetMotionThreshold(int objectID, int limbID, float threshold);

//-- TRIGGERS
EXPORTC void dynTriggerSetShape(int objectID, int shapeIndex, int flag, bool value);
EXPORTC int dynTriggerGetData();
EXPORTC int dynTriggerGetTriggerObject();
EXPORTC int dynTriggerGetOtherObject();
EXPORTC int dynTriggerGetStatus();

//-- FREEZE pos/rot
EXPORTC void dynFreezePositionX(int objectID, bool freeze);
EXPORTC void dynFreezePositionY(int objectID, bool freeze);
EXPORTC void dynFreezePositionZ(int objectID, bool freeze);
EXPORTC void dynFreezeRotationX(int objectID, bool freeze);
EXPORTC void dynFreezeRotationY(int objectID, bool freeze);
EXPORTC void dynFreezeRotationZ(int objectID, bool freeze);


//-- GRAVITY
EXPORTC void dynEnableGravity(int objectID, bool gravity);


//-- OVERLAP TESTS
EXPORTC bool dynOverlapCheckSphere(int positionVec3ID, float radius, int flag);
EXPORTC bool dynOverlapCheckAABB(int worldBoundsMinVec3ID, int worldBoundsMaxVec3ID, int flag);
EXPORTC bool dynOverlapCheckCapsule(int pos1Vec3ID, int pos2Vec3ID, float radius, int flag);
EXPORTC bool dynOverlapCheckOBB(int positionVec3ID, int dimensionVec3ID, int rotationVec3ID, int flag);


//dbpro done?
EXPORT bool dynActorExists(int objectID);
EXPORT bool dynActorExists(int objectID, int limbID);
EXPORTC bool dynTriangleMeshExists(int meshID);
EXPORTC bool dynConvexMeshExists(int meshID);
EXPORTC bool dynControllerExists(int objectID);


//#ifdef COMPILE_DYNAMIX_FULL   //no longer used for header


//JOINTS
EXPORTC bool  dynMakeJointDesc(int jointDescID);
EXPORTC bool  dynMakeJoint(int jointID, int jointDescID);
EXPORTC void  dynDeleteJointDesc(int jointDescID);
EXPORTC void  dynDeleteJoint(int jointID);

EXPORT  void  dynJointDescSetActorA(int jointDescID, int objectID);
EXPORT  void  dynJointDescSetActorB(int jointDescID, int objectID);
EXPORT  void  dynJointDescSetActorA(int jointDescID, int objectID, int limbID);
EXPORT  void  dynJointDescSetActorB(int jointDescID, int objectID, int limbID);
EXPORTC void  dynJointDescSetGlobalAnchor(int jointDescID, float posX, float posY, float posZ);
EXPORTC void  dynJointDescSetGlobalAxis(int jointDescID, float axisX, float axisY, float axisZ);

EXPORTC void  dynJointDescSetSwing1Motion(int jointDescID, int motion);
EXPORTC void  dynJointDescSetSwing2Motion(int jointDescID, int motion);
EXPORTC void  dynJointDescSetTwistMotion(int jointDescID, int motion);
EXPORTC void  dynJointDescSetMotionX(int jointDescID, int motion);
EXPORTC void  dynJointDescSetMotionY(int jointDescID, int motion);
EXPORTC void  dynJointDescSetMotionZ(int jointDescID, int motion);

//Joint Limits
EXPORTC void  dynJointDescSetSwing1Limit(int jointDescID, float value, float spring, float damping, float restitution);
EXPORTC void  dynJointDescSetSwing2Limit(int jointDescID, float value, float spring, float damping, float restitution);
EXPORTC void  dynJointDescSetTwistLimitHigh(int jointDescID, float value, float spring, float damping, float restitution);
EXPORTC void  dynJointDescSetTwistLimitLow(int jointDescID, float value, float spring, float damping, float restitution);



//FORCE FIELD
EXPORT  bool  dynMakeForceField();



//SOFT BODY
EXPORT  bool dynMakeSoftBody(int objectID, int softBodyMeshID, float density);
EXPORT  bool dynMakeSoftBody(int objectID, int softBodyMeshID, float density, unsigned int flags); 
EXPORT  bool dynCookSoftBodyMesh(int softBodyMeshID, char* tetFile);
EXPORT  bool dynCookSoftBodyMesh(char* destinationFile, char* sourceTetFile);
EXPORT  bool dynMakeSoftBodyDebug(int objectID, int softBodyID);
EXPORT  void dynUpdateSoftBodyDebug(int objectID, int softBodyID);
EXPORTC  bool dynLoadSoftBodyMesh(int softBodyMeshID, char* file);
EXPORTC  bool dynDeleteSoftBodyMesh(int softBodyMeshID);
EXPORT  void dynDeleteSoftBody(int softBodyID);

EXPORT  void dynSoftBodyAttachToShape(int softBodyID, int objectID, int shapeIndex, int attachmentFlag);
EXPORT  void dynSoftBodyAttachToShape(int softBodyID, int objectID, int limbID, int shapeIndex, int attachmentFlag);
EXPORT  void dynSoftBodyDetachFromShape(int softBodyID, int objectID, int shapeIndex);
EXPORT  void dynSoftBodyAttachVertexToShape(int softBodyID, int vertexIndex, int objectID, int shapeIndex, float localX, float localY, float localZ, int attachmentFlag);
EXPORT  void dynSoftBodyAttachVertexToShape(int softBodyID, int vertexIndex, int objectID, int limbID, int shapeIndex, float localX, float localY, float localZ, int attachmentFlag);

//exists?

//DBPro not done
//Altered function name from dynSet
EXPORTC void dynSoftBodySetStretchingStiffness(int softBodyID, float value);
EXPORTC DWORDFLOAT dynSoftBodyGetStretchingStiffness(int softBodyID);

EXPORT  bool dynMakeSoftBody(int objectID, int softBodyID, float density, unsigned int flags);

EXPORTC  void dynSoftBodyAttachToCollidingShapes(int softBodyID, int attachmentFlag);
EXPORTC  void dynSoftBodySetVolumeStiffness(int softBodyID, float value); //set flag
EXPORTC  void dynSoftBodySetParticleRadius(int softBodyID, float value);
EXPORTC  void dynSoftBodySetHardStretchLimitationFactor(int softBodyID, float value);
EXPORTC  void dynSoftBodySetDampingCoefficient(int softBodyID, float value);//set flag
EXPORTC  void dynSoftBodySetFriction(int softBodyID, float value);
EXPORTC  void dynSoftBodySetAttachmentTearFactor(int softBodyID, float value);
EXPORTC  void dynSoftBodySetSelfCollisionThickness(int softBodyID, float value);
EXPORTC  void dynSoftBodySetCollisionResponseCoefficient(int softBodyID, float value);
EXPORTC  void dynSoftBodySetAttachmentResponseCoefficient(int softBodyID, float value);
EXPORTC  DWORDFLOAT dynSoftBodyGetVolumeStiffness(int softBodyID); //set flag
EXPORTC  DWORDFLOAT dynSoftBodyGetParticleRadius(int softBodyID);
EXPORTC  DWORDFLOAT dynSoftBodyGetHardStretchLimitationFactor(int softBodyID);
EXPORTC  DWORDFLOAT dynSoftBodyGetDampingCoefficient(int softBodyID);//set flag
EXPORTC  DWORDFLOAT dynSoftBodyGetFriction(int softBodyID);
EXPORTC  DWORDFLOAT dynSoftBodyGetAttachmentTearFactor(int softBodyID);
EXPORTC  DWORDFLOAT dynSoftBodyGetSelfCollisionThickness(int softBodyID);
EXPORTC  DWORDFLOAT dynSoftBodyGetCollisionResponseCoefficient(int softBodyID);
EXPORTC  DWORDFLOAT dynSoftBodyGetAttachmentResponseCoefficient(int softBodyID);
EXPORTC  void dynSoftBodyFreeVertex(int softBodyID, int vertexID);
EXPORTC  void dynSoftBodySetGroup(int softBodyID, int group);
EXPORTC  int dynSoftBodyGetGroup(int softBodyID);
EXPORTC  void dynSoftBodySetExternalAcceleration(int softBodyID, int accelerationVec3ID);
EXPORTC  void dynSoftBodyGetExternalAcceleration(int softBodyID, int accelerationVec3ID);
EXPORTC  bool dynSoftBodyIsSleeping(int softBodyID);
EXPORTC  void dynSoftBodyWakeUp(int softBodyID, float wakeCounterValue);
EXPORTC  void dynSoftBodyPutToSleep(int softBodyID);
EXPORTC  void dynSoftBodySetFlags(int softBodyID, unsigned int flags);
EXPORTC  unsigned int dynSoftBodyGetFlags(int softBodyID);
EXPORTC  bool dynSoftBodyRaycast(int softBodyID, int origVec3ID, int dirVec3ID);
EXPORTC  void dynSoftBodyRaycastGetHitPos(int posVec3ID);
EXPORTC  int dynSoftBodyRaycastGetVertexID();
EXPORTC  void dynSoftBodyAddForceAtVertex(int softBodyID, int vertexID, int forceVec3ID);
EXPORTC  void dynSoftBodyAddForceAtPos(int softBodyID, int posVec3ID, float magnitude, float radius);
EXPORTC  void dynSoftBodyAddDirectedForceAtPos(int softBodyID, int posVec3ID, int forceVec3ID, float radius);

EXPORTC  bool dynSoftBodyExists(int softBodyID);
EXPORTC  bool dynSoftBodyMeshExists(int softBodyMeshID);
//end

EXPORT   bool dynIsWheel(int objectID, int shapeIndex);
//--Wheels
EXPORT   bool dynMakeWheel(int objectID, float density);
EXPORT   bool dynWheelGetContact(int objectID, int shapeIndex);
EXPORTC  DWORDFLOAT dynWheelGetContactPosition(); 
EXPORT   void dynWheelUpdate(int objectID, int shapeIndex);
EXPORT   DWORDFLOAT dynWheelGetSuspensionTravel(int objectID, int shapeIndex);
EXPORT   DWORDFLOAT dynWheelGetAxleSpeed(int objectID, int shapeIndex);
EXPORT   DWORDFLOAT dynWheelGetRadius(int objectID, int shapeIndex);
EXPORT   void  dynWheelSetSteerAngle(int objectID, int shapeIndex, float angle);
EXPORT   void  dynWheelSetMotorTorque(int objectID, int shapeIndex, float torque);
EXPORT   void  dynWheelSetBrakeTorque(int objectID, int shapeIndex, float torque);

EXPORT   void dynWheelSetSuspensionTravel(int objectID, int shapeIndex, float suspensionTravel);
EXPORT   void dynWheelSetSuspensionSpring(int objectID, int shapeIndex, float spring, float damper, float targetValue);


//wheel as limb
EXPORT   bool dynMakeWheel(int objectID, int limbID, float radius, float density);
EXPORT   void dynWheelUpdate(int objectID, int limbID, int shapeIndex);
EXPORT   void dynWheelSetMotorTorque(int objectID, int limbID, int shapeIndex, float torque);

EXPORTC  bool dynMakeTireFuncDesc(int ID);
EXPORTC  void dynTireFuncDescSetAsymptote(int ID, float asymptoteSlip, float asymptoteValue);
EXPORTC  void dynTireFuncDescSetExtremum(int ID, float extremumSlip, float extremumValue);
EXPORTC  void dynTireFuncDescSetStiffnessFactor(int ID, float stiffnessFactor);
EXPORTC  void dynDeleteTireFuncDesc(int ID);
EXPORTC  void dynTireFuncDescSetToDefault(int ID);
EXPORTC  bool dynTireFuncDescIsValid(int ID);
EXPORTC  DWORDFLOAT dynTireFuncDescGetHermiteEval(int ID, float t);

EXPORT   void dynWheelSetLatTireForceFunc(int objectID, int shapeIndex, int tireFuncDescID);  
EXPORT   void dynWheelSetLongTireForceFunc(int objectID, int shapeIndex, int tireFuncDescID);  
EXPORT   void dynWheelSetLatTireForceFunc(int objectID, int limbID, int shapeIndex, int tireFuncDesc);  
EXPORT   void dynWheelSetLongTireForceFunc(int objectID, int limbID, int shapeIndex, int tireFuncDesc);  


//*****************     CLOTH      ***********************//
EXPORT  bool dynCookClothMesh(int objectID, int clothMeshID, unsigned int flags);
EXPORT  bool dynMakeCloth(int objectID, int clothMeshID); 
EXPORT  bool dynCookClothMesh(int objectID, char* file, unsigned int flags);
EXPORTC  bool dynLoadClothMesh(char* file, int clothMeshID);
EXPORTC  void dynDeleteClothMesh(int clothMeshID); 
EXPORTC  void dynDeleteCloth(int objectID);
EXPORTC  bool dynClothExists(int objectID);
EXPORTC  void dynClothSetFlags(int objectID, unsigned int flags);
EXPORTC  void dynClothAttachToShape(int clothID, int objectID, int shapeIndex, unsigned int attachmentFlags);
EXPORTC  void dynClothAttachToCollidingShapes(int clothID, unsigned int attachmentFlags);
EXPORTC  void dynClothDetachFromShape(int clothID, int objectID, int shapeIndex);
EXPORTC  void dynClothSetBendingStiffness(int clothID, float value);
EXPORTC  DWORDFLOAT dynClothGetBendingStiffness(int clothID);
EXPORTC  void dynClothSetStretchingStiffness(int clothID, float value);
EXPORTC  DWORDFLOAT dynClothGetStretchingStiffness(int clothID);
EXPORTC  void dynClothSetHardStretchLimitationFactor(int clothID, float value);
EXPORTC  DWORDFLOAT dynClothGetHardStretchLimitationFactor(int clothID);
EXPORTC  void dynClothSetDampingCoefficient(int clothID, float value);
EXPORTC  DWORDFLOAT dynClothGetDampingCoefficient(int clothID);
EXPORTC  void dynClothSetFriction(int clothID, float value);
EXPORTC  DWORDFLOAT dynClothGetFriction(int clothID);
EXPORTC  void dynClothSetPressure(int clothID, float value);
EXPORTC  DWORDFLOAT dynClothGetPressure(int clothID);
EXPORTC  void dynClothSetAttachmentTearFactor(int clothID, float value);
EXPORTC  DWORDFLOAT dynClothGetAttachmentTearFactor(int clothID);
EXPORTC  void dynClothSetThickness(int clothID, float value);
EXPORTC  DWORDFLOAT dynClothGetThickness(int clothID);
EXPORTC  void dynClothSetSelfCollisionThickness(int clothID, float value);
EXPORTC  DWORDFLOAT dynClothGetSelfCollisionThickness(int clothID);
EXPORTC  unsigned int dynClothGetFlags(int clothID);
EXPORT  void dynClothSetGroup(int clothID, int collisionGroup);
EXPORT  int  dynClothGetGroup(int clothID);

EXPORTC unsigned int dynUtilExpandVertexBuffer(int objectID, int extraVertexCount, int extraIndexCount); 

EXPORTC bool dynClothMeshExists(int clothMeshID);
EXPORTC void dynClothAttachToCore(int clothID, int objectID, float impulseThreshold, float penetrationDepth=0.0f, float maxDeformationDistance = 0.0f);
EXPORT  bool dynStart(bool enableHardware); 
EXPORTC bool dynHardwarePresent();
EXPORT  bool dynMakeScene(int ID, bool hardwareScene);
EXPORTC void dynClothSetWindAcceleration(int clothID, int normalVec3ID);
EXPORTC void dynClothGetWindAcceleration(int clothID, int normalVec3ID);
EXPORTC bool dynClothRaycast(int clothID, int origVec3ID, int dirVec3ID);
EXPORTC void dynClothRaycastGetHitPos(int posVec3ID);
EXPORTC int dynClothRaycastGetVertexID();
EXPORTC bool dynClothTearVertex(int clothID, int vertexID, int normalVec3ID);
EXPORTC void dynClothGetVertexPos(int clothID, int vertexID, int posVec3ID);
EXPORTC bool dynClothIsSleeping(int clothID);
EXPORTC void dynClothPutToSleep(int clothID);
EXPORTC void dynClothWakeUp(int clothID, float wakeCounterValue);
EXPORTC  void dynClothAddForceAtVertex(int clothID, int vertexID, int forceVec3ID);
EXPORTC  void dynClothAddForceAtPos(int clothID, int posVec3ID, float magnitude, float radius);
EXPORTC  void dynClothAddDirectedForceAtPos(int clothID, int posVec3ID, int forceVec3ID, float radius);

EXPORT  bool dynMakeCloth(int objectID, int clothMeshID, unsigned int flags); 

//-- UTIL
EXPORTC void dynUtilArrayVec3Init(unsigned int size);
EXPORTC void dynUtilArrayVec3Set(unsigned int position, int inVec3ID);
EXPORTC void dynUtilArrayVec3Get(unsigned int position, int outVec3ID);
EXPORTC void dynUtilArrayVec3Clear();
//-- FLUIDS
EXPORTC bool dynMakeFluid(int fluidID, unsigned int maxParticles, float particleLife, float collisionRadiusMultiplier, unsigned int flags, bool usePositionData); 
EXPORTC void dynDeleteFluid(int fluidID);
EXPORTC void dynFluidSyncObjects(int fluidID, int initialObjectID);
EXPORTC void dynFluidFadeObjects(int fluidID, int initialObjectID, float particleLife, float fadeIn, float fadeOut);
EXPORTC void dynFluidSetObjectsToCamera(int fluidID, int initialObjectID, int cameraID);
EXPORTC int  dynFluidMakeEmitter(int fluidID, float sizeX, float sizeY, float particleLife, int type, int posVec3ID, int rotVec3ID);
EXPORTC void dynEmitterSetFrameShape(int fluidID, int emitterIndex, int objectID, int shapeIndex); 
EXPORTC int dynFluidGetCollisionMethod(int fluidID);
EXPORTC void dynFluidSetCollisionMethod(int fluidID, int flag);
EXPORTC void dynEmitterSetRandomAngle(int fluidID, int emitterIndex, float angle);
EXPORTC DWORDFLOAT dynEmitterGetRandomAngle(int fluidID, int emitterIndex);
//-- not done dbPro
EXPORTC bool dynFluidExists(int fluidID);
EXPORTC void dynFluidSetSimulationMethod(int fluidID, int method);
EXPORTC int dynFluidGetSimulationMethod(int fluidID);
EXPORTC void dynFluidSetStiffness(int fluidID, float value);
EXPORTC DWORDFLOAT dynFluidGetStiffness(int fluidID);
EXPORTC void dynFluidSetViscosity(int fluidID, float value);
EXPORTC DWORDFLOAT dynFluidGetViscosity(int fluidID);
EXPORTC void dynFluidSetDamping(int fluidID, float value);
EXPORTC DWORDFLOAT dynFluidGetDamping(int fluidID);
EXPORTC void dynFluidSetExternalAcceleration(int fluidID, float x, float y, float z);
EXPORTC void dynFluidGetExternalAcceleration(int fluidID, int vector3ID);
EXPORTC void dynFluidDeleteEmitter(int fluidID, int emitterIndex);
EXPORTC void dynEmitterSetRate(int fluidID, int emitterIndex, float rate);
EXPORTC DWORDFLOAT dynEmitterGetRate(int fluidID, int emitterIndex);
EXPORTC void dynEmitterSetVelocityMagnitude(int fluidID, int emitterIndex, float velocityMagnitude);
EXPORTC DWORDFLOAT dynEmitterGetVelocityMagnitude(int fluidID, int emitterIndex);

// FORCE FIELDS
EXPORTC bool dynMakeFFShapeBox(int posVector3ID, int rotVector3ID, int dimVector3ID);
EXPORTC bool dynMakeFFShapeSphere(int posVector3ID, int rotVector3ID, float radius);
EXPORTC bool dynMakeFFShapeCapsule(int posVector3ID, int rotVector3ID, float radius, float height);
EXPORTC bool dynMakeFFShapeConvex(int posVector3ID, int rotVector3ID, int convexMeshID);
EXPORTC bool dynMakeFFLinearKernel(int ID, int constantVector3ID, int falloffLinearVector3ID, int falloffQuadraticVector3ID, int noiseVector3ID);
EXPORTC bool dynMakeForceField(int ID, int kernelID, int coordinates, int actorID);
EXPORTC bool dynMakeFFShapeGroup(int ID, bool excludeGroup);
EXPORTC void dynFFAddShapeGroup(int ID, int shapeGroupID);
EXPORTC void dynFFRemoveShapeGroup(int ID, int shapeGroupID);

EXPORTC void dynFFClearShapes();
EXPORTC void dynDeleteFFLinearKernel(int kernelID);
EXPORTC void dynDeleteFFShapeGroup(int shapeGroupID);
EXPORTC void dynDeleteForceField(int ID);
EXPORTC bool dynFFShapeGroupExists(int ID);
EXPORTC bool dynFFLinearKernelExists(int ID);
EXPORTC bool dynForceFieldExists(int ID);

EXPORTC bool dynJointDescExists(int ID);
EXPORTC bool dynMaterialExists(int ID);

EXPORT  void dynWheelUpdate(int objectID, int shapeIndex, float timestep);
EXPORT  void dynWheelUpdate(int objectID, int limbID, int shapeIndex, float timestep);
EXPORTC void dynFluidScaleObjects(int fluidID, int initialObjectID, float particleLife, float scaleStart, float scaleFinish);

//Kinematic
EXPORTC void dynKinematicOn(int objectID);
EXPORTC void dynKinematicOff(int objectID);
EXPORTC void dynKinematicMovePos(int objectID, float posX, float posY, float posZ);
EXPORTC void dynKinematicMoveRot(int objectID, float rotX, float rotY, float rotZ);

EXPORTC void dynJointDescSetFlags(int jointDescID, int flags);

//Shapes
EXPORTC bool dynMakeShapeBox(int objectID, float density);
EXPORTC bool dynMakeShapeSphere(int objectID, float density);
EXPORTC bool dynMakeShapeCapsule(int objectID, float height, float radius, float density);
EXPORTC bool dynMakeCompoundFromShapes(int objectID, float density);
EXPORTC void dynClearShapes();

EXPORTC void dynDeleteGroundPlane();

//Bone
//EXPORTC bool dynCookConvexMeshFromBone(int objectID, int meshID, int boneID);
//EXPORTC bool dynMakeConvexMeshFromBone(int sourceObjectID, int sourceBoneID, int objectID, int meshID, int boneID, float density = 10.0);

//EXPORTC bool dynMakeBone(int objectID, int bone, const D3DXMATRIX *pObjMatrix, const sMesh *pMesh,
//						 float fCenterX, float fCenterY, float fCenterZ, 
//						 float fExtentX, float fExtentY, float fExtentZ);


//-- NEW v1.1
//-- Merge with current functions
EXPORT  bool  dynCookConvexMesh(int objectID, int boneID, int meshID, char* file);
EXPORT  bool  dynCookConvexMesh(int objectID, int boneID, int meshID, int convexMeshID);
EXPORT  bool  dynMakeConvexMesh(int objectID, int boneID, int meshID, int convexMeshID, float density); 

EXPORTC void  dynBoneSyncActor(int objectID, int meshID, int boneID);
EXPORTC void  dynBoneClearMatrix(int objectID, int meshID, int boneID);
EXPORTC int   dynUtilGetBoneCount(int objectID, int meshID);
EXPORTC int   dynUtilGetMeshCount(int objectID);
EXPORTC void  dynSetBones(int objectID, bool physics);

//EXPORT void dynSetUpdateObject(int sourceObjectID, int newObjectID, int bone); 

//EXPORTC float  dynBoneGetPosX(int objectID, int meshID, int boneID);
//EXPORTC float  dynBoneGetPosY(int objectID, int meshID, int boneID);
//EXPORTC float  dynBoneGetPosZ(int objectID, int meshID, int boneID);


EXPORTC void dynBoneGetPos(int objectID, int meshID, int boneID, int vec3Out);

EXPORT DWORDFLOAT dynGetMass(int objectID, int limbID);
EXPORT void dynSetMass(int objectID, int limbID, float mass);

EXPORT  void  dynSetLinearDamping(int objectID, int limbID, float value);
EXPORT  void  dynSetAngularDamping(int objectID, int limbID, float value);
EXPORT  DWORDFLOAT dynGetAngularDamping(int objectID, int limbID);
EXPORT  DWORDFLOAT dynGetLinearDamping(int objectID, int limbID);

EXPORT  void dynSetLinearMomentum(int objectID, int limbID, float x, float y, float z);
EXPORT  void dynSetAngularMomentum(int objectID, int limbID, float x, float y, float z);
EXPORT  void dynGetAngularMomentum(int objectID, int limbID, int vec3Out);
EXPORT  void dynGetLinearMomentum(int objectID, int limbID, int vec3Out);


EXPORT  void dynActorRaiseFlag(int objectID, unsigned int flag);
EXPORT  void dynActorRaiseFlag(int objectID, int limbID, unsigned int flag);
EXPORT  void dynActorClearFlag(int objectID, unsigned int flag);
EXPORT  void dynActorClearFlag(int objectID, int limbID, unsigned int flag);

EXPORT  void dynWakeUp(int objectID, float wakeCounterValue);
EXPORT  void dynPutToSleep(int objectID);
EXPORT  void dynWakeUp(int objectID, int limbID, float wakeCounterValue);
EXPORT  void dynPutToSleep(int objectID, int limbID);

EXPORT  bool dynMakeBox(int objectID, int boneID, int meshID, float density);
EXPORT  bool dynMakeSphere(int objectID, int boneID, int meshID, float density);

EXPORT  bool dynMakeCapsule(int objectID, int boneID, int meshID, float density);
//NOT NEEDED
//EXPORT  bool dynMakeCapsule(int objectID, int boneID, int meshID, float density, float locRotX, float locRotY, float locRotZ);

EXPORT  void dynShapeSetLocPos(int objectID, int LimbID, int shapeID, float posX, float posY, float posZ);
EXPORT  void dynShapeSetLocRot(int objectID, int limbID, int shapeID, float angleX, float angleY, float angleZ);
EXPORT  void dynShapeSetGlobPos(int objectID, int limbID, int shapeID, float posX, float posY, float posZ);
EXPORT  void dynShapeSetGlobRot(int objectID, int limbID, int shapeID, float angleX, float angleY, float angleZ);

EXPORT  void dynBodyRaiseFlag(int objectID, unsigned int flag);
EXPORT  void dynBodyRaiseFlag(int objectID, int limbID, unsigned int flag);
EXPORT  void dynBodyClearFlag(int objectID, unsigned int flag);
EXPORT  void dynBodyClearFlag(int objectID, int limbID, unsigned int flag);

EXPORT  bool dynBodyReadFlag(int objectID, unsigned int flag);
EXPORT  bool dynBodyReadFlag(int objectID, int limbID, unsigned int flag);
EXPORT  bool dynActorReadFlag(int objectID, unsigned int flag);
EXPORT  bool dynActorReadFlag(int objectID, int limbID, unsigned int flag);

EXPORT  int dynControllerMove(int ID, float x, float y, float z, unsigned int activeGroups);
EXPORTC void dynControllerReportSceneChanged(int ID);

EXPORTC bool dynJointExists(int jointID);

EXPORTC void dynJointDescSetMaxTorque(int jointDescID, float value);
EXPORTC void dynJointDescSetMaxForce(int jointDescID, float value);
EXPORTC bool dynJointIsBroken(int jointID);

EXPORTC void dynJointDescSetLocalAnchor(int jointDescID, int AorB, float x, float y, float z);
EXPORTC void dynJointDescSetLocalAxis(int jointDescID, int AorB, float x, float y, float z);
EXPORTC void dynJointDescSetLocalNormal(int jointDescID, int AorB, float x, float y, float z);
EXPORTC void dynJointDescGetLocalAnchor(int jointDescID, int AorB, int vector3Out);
EXPORTC void dynJointDescGetLocalAxis(int jointDescID, int AorB, int vector3Out);
EXPORTC void dynJointDescGetLocalNormal(int jointDescID, int AorB, int vector3Out);

EXPORTC bool dynJointDescIsValid(int jointDescID);

EXPORTC void dynMaterialSetDirOfAnisotropy(int ID, float x, float y, float z);
EXPORTC void dynMaterialSetDynamicFriction(int ID, float value);
EXPORTC void dynMaterialSetDynamicFrictionV(int ID, float value);
EXPORTC void dynMaterialSetStaticFriction(int ID, float value);
EXPORTC void dynMaterialSetStaticFrictionV(int ID, float value);
EXPORTC void dynMaterialSetFlags(int ID, unsigned int flags);

EXPORTC void dynMaterialGetDirOfAnisotropy(int ID, int vec3Out);
EXPORTC DWORDFLOAT dynMaterialGetDynamicFriction(int ID);
EXPORTC DWORDFLOAT dynMaterialGetDynamicFrictionV(int ID);
EXPORTC DWORDFLOAT dynMaterialGetStaticFriction(int ID);
EXPORTC DWORDFLOAT dynMaterialGetStaticFrictionV(int ID);
EXPORTC unsigned int dynMaterialGetFlags(int ID);
EXPORTC int dynMaterialGetMaterialIndex(int ID); 


EXPORTC void dynSetForceMode(int mode);
EXPORT void dynAddForceAtPos(int objectID, float x, float y, float z, float posX, float posY, float posZ); 
EXPORT void dynAddForceAtPos(int objectID, int limbID, float x, float y, float z, float posX, float posY, float posZ); 
EXPORT void dynAddForceAtLocalPos(int objectID, float x, float y, float z, float posX, float posY, float posZ); 
EXPORT void dynAddForceAtLocalPos(int objectID, int limbID, float x, float y, float z, float posX, float posY, float posZ); 
EXPORT void dynAddLocalForceAtPos(int objectID, float x, float y, float z, float posX, float posY, float posZ); 
EXPORT void dynAddLocalForceAtPos(int objectID, int limbID, float x, float y, float z, float posX, float posY, float posZ); 
EXPORT void dynAddLocalForceAtLocalPos(int objectID, int limbID, float x, float y, float z, float posX, float posY, float posZ); 

EXPORTC unsigned int dynRaycastHitGetFaceID();
EXPORTC bool dynMakeCCDSkeletonVertex(int objectID, int limbID);

// new
EXPORTC void dynJointDescSetSwingDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring);
EXPORTC void dynJointDescSetTwistDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring);
EXPORTC void dynJointDescSetXDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring);
EXPORTC void dynJointDescSetYDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring);
EXPORTC void dynJointDescSetZDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring);
EXPORTC void dynJointDescSetSlerpDrive(int jointDescID, int driveType, float forceLimit, float damping, float spring);
EXPORTC void dynJointDescSetGearRatio(int jointDescID, float ratio);

EXPORTC void dynJointSetDriveAngularVelocity(int jointID, float x, float y, float z); 
EXPORTC void dynJointSetDriveLinearVelocity(int jointID, float x, float y, float z); 
EXPORTC void dynJointSetDriveOrientation(int jointID, float w, float x, float y, float z); 
EXPORTC void dynJointSetDriveOrientationFromGlobalAxis(int jointID, float x, float y, float z); 
EXPORTC void dynJointSetDriveOrientationFromLocalAxis(int jointID, float x, float y, float z); 
EXPORTC void dynJointSetDriveOrientationFromAngleAxis(int jointID, float axisX, float axisY, float axisZ, float angle); 
EXPORTC void dynJointSetDrivePosition(int jointID, float x, float y, float z); 

EXPORTC void dynFluidFadeObjectsAlpha(int fluidID, int initialObjectID, float particleLife, float fadeIn, float fadeOut);
EXPORTC void dynShapeSetFlag(int objectID, int shapeIndex, int flag, bool value); 

EXPORTC void dynUtilSetCameraToObject(int cameraID, int objectID, float offsetPosX, float offsetPosY, float offsetPosZ,
					   float offsetPointX, float offsetPointY, float offsetPointZ,
					   float smooth);




//-- V1.11 
EXPORTC int dynContactGetLimbA();
EXPORTC int dynContactGetLimbB();
EXPORT  void dynSetGroup(int objectID, int limbID, int group);
EXPORT  int dynGetGroup(int objectID, int limbID);

//-- Changed to EXPORT from EXPORTC, function already exists
//-- GROUPS
EXPORT  int  dynGetGroup(int actorID);

EXPORT  void dynSoftBodyDetachFromShape(int softBodyID, int objectID, int limbID, int shapeIndex);
//Joint Limits
EXPORTC void dynJointDescSetLinearLimit(int jointDescID, float value, float spring, float damping, float restitution);
 
EXPORT void dynSetShapeGroup(int objectID, int shapeIndex, int group);  // -1 for all shapes
EXPORT void dynSetShapeGroup(int objectID, int limbID, int shapeIndex, int group);  // -1 for all shapes
EXPORT void dynShapePairSetFlags(int actorA, int shapeIndexA, int actorB, int shapeIndexB, int flag);
  
EXPORT void dynGetOrientation( int objectID, int outVec3ID );
EXPORT void dynGetPosition( int objectID, int outVec3ID );
EXPORT void dynGetOrientation( int objectID, int limbID, int outVec3ID );
EXPORT void dynGetPosition( int objectID, int limbID, int outVec3ID );

EXPORT bool dynIsSleeping( int objectID );
EXPORT bool dynIsSleeping( int objectID, int limbID );

EXPORT int dynGetVersion( );

EXPORT  void  dynGetAngularVelocity(int objectID, int limbID, int vector3ID);
EXPORT  void  dynGetLinearVelocity(int objectID, int limbID, int vector3ID);
EXPORT  void  dynSetAngularVelocity(int objectID, int limbID, float x, float y, float z);
EXPORT  void  dynSetLinearVelocity(int objectID, int limbID, float x, float y, float z);






//*****************  SERIALIZATION  **********************//
EXPORT  void  dynSaveScene(char* file);
EXPORTC void  dynLoadRagdoll(int objectID, char* file);


//TEST
//EXPORT void dynMakeFluid(int boundingObjectID, int particleObjectID, float rate, float life, int max);
//EXPORT void dynUpdateFluid();

//#endif  (compile dynnamix full) not used for header

#endif

#ifndef COMPILE_FOR_GDK
   #undef bool 
#endif