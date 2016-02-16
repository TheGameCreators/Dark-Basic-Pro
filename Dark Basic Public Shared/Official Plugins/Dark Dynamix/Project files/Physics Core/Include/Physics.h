#pragma once
#ifndef PHYSICS
#define PHYSICS
 

#define NOMINMAX
#include "Defines.h"
#include "NxPhysics.h"
#include "NxCooking.h"
#include "NxControllerManager.h"
#include "NxCapsuleController.h"
#include "NxBoxController.h"
#include "NXU_Helper.h"
#include <nxu_helper.h>
#include "Stream.h"
#include "UserAllocator.h"
//#include "UserData.h"

#ifdef COMPILE_FOR_IRRLICHT
   #include "IrrSync.h"
#else
   #include "ActorSync.h"
#endif

#include <map>
#include <vector>

#include "TetraGraphics.h"
#include "SoftMeshObj.h"
#include "NxMouseDrag.h"



//#include "TetraMesh.h"

using namespace std;

#ifdef DEBUG
#pragma comment(lib, "PhysXLoaderCHECKED.lib")
#pragma comment(lib, "PhysXCookingCHECKED.lib")
#pragma comment(lib, "NxCharacterCHECKED.lib")

#else
#pragma comment(lib, "PhysXLoader.lib")
#pragma comment(lib, "PhysXCooking.lib")
#pragma comment(lib, "NxCharacter.lib")
#endif

//-- SOFT BODY CLASSES
class Tet
{
public:
    float p1_;
	float p2_;
	float p3_;
	float p4_;
};







class Physics
{
public:
	Physics(void);
	~Physics(void);

	bool  start(void);   // 0 = fail, 1 = success, 2 = already initialised 
	bool  start(bool enableHardware);
	void  stop(void);
	int   getCurrentScene();
	void  setCurrentScene(int sceneID);
	bool  makeScene(int sceneID);
	bool  makeScene(int sceneID, bool hardwareScene);
	void  setTimeStep(float timeStep);
	float getTimeStep();
	void  releaseScene(int sceneID);
	int   getSceneCount();
    void  simulate();
	void  simulate(float timeStep);
	void  fetchResults();
	void  update();
	void  setGravity(float x, float y, float z);
	void  visualDebuggerOn();
	void  visualDebuggerOff();
	bool  visualDebuggerConnected();

	void  setParameter(int parameter, float value);
	float getParameter(int parameter);
	void  makeFluid(float rate, float life, int max);


	//--FLUIDS
   // NxFluid* makeFluid(int simulationMethod, unsigned int maxParticles, unsigned int flags);
    NxFluid* makeFluid( unsigned int maxParticles, float particleLife, float collisionRadiusMultiplier, unsigned int flags, NxVec3* posData = 0, NxU32 posCount = 0);
    void releaseFluid(NxFluid * fluid);

	int Physics::makeFluidEmitter(NxFluid* fluid);

	void  setActorPose(NxActor *actor, NxMat34 matPose);

	void  makeJointSpherical(NxActor *actorA, NxActor *actorB, float globAnchorX, float globAnchorY, float globAnchorZ);
	NxD6Joint* makeJoint(NxJointDesc &d6JointDesc);
    

	void  saveActor(NxActor* actor, string url, const char* userProp, const char* ID );
	NxActor* loadActor(string url);

	void  saveCollection(string url);

	//Overide following functions, then call this base function supplying correct
	//data from your rendered objects inside ud.
	//NxActor*  makeBox(UserData *ud, float &density);
    NxActor* makeBox(ActorSync *aSync, NxMat34 &mat, NxVec3 &dim, float &density, NxVec3 &locPos);
	//Capsule
	NxActor* makeCapsule(ActorSync *aSync, NxMat34 &mat, float &height, float &radius, float &density, NxVec3 &locPos); 
	NxActor* makeCapsule(ActorSync *aSync, NxMat34 &mat, float &height, float &radius, float &density, NxMat33 &locRot);
  
	NxActor* makeSphere(ActorSync *aSync, NxMat34 &mat, float &radius, float &density, NxVec3 &locPos);
 	NxActor* makeWheel(ActorSync *aSync, NxMat34 &mat, float &radius, float &density, NxVec3 &locPos);
      
	//SHAPES
	NxActor* makeCompoundActor(ActorSync *aSync, NxMat34 &mat, float &density);
	bool makeShapeBox(NxMat34 &mat, NxVec3 &dim, float &density);
	bool makeShapeSphere(NxMat34 &mat, float &radius, float &density);
	bool makeShapeCapsule(NxMat34 &mat, float &height, float &radius, float &density);
    bool makeShapeConvex(NxMat34 &mat, NxConvexMesh *cmesh, float &density);
    void clearShapes();

	//CCD
	bool makeCCDSkeletonBox(NxShape* shape, NxVec3 scaleMultiplier);
    void releaseCCDSkeleton(NxShape* shape);
    bool makeCCDSkeletonVertex(NxShape* shape);

	NxMaterial* makeMaterial(float restitution, float staticFriction, float dynamicFriction);

	//NxActor*  makeConvexMesh(UserDataMesh *ud, float &density);
	//NxActor*  makeConvexMesh(UserData *ud, std::string url, float &density);
	NxActor*  makeConvexMesh(ActorSync *aSync, std::string url, NxMat34 &mat, float &density);

	
	//NxConvexShapeDesc* makeShapeConvex(UserData *ud, std::string url);
	//NxConvexShapeDesc* makeShapeConvex(NxMat34 mat, std::string url);
	NxConvexShapeDesc* makeShapeConvex(NxMat34 mat, NxConvexMesh *mesh);
	NxTriangleMeshShapeDesc* makeShapeTriangleMesh(NxMat34 mat, NxTriangleMesh *mesh);
    NxWheelShapeDesc* makeShapeWheel(NxMat34 mat, float radius);

	//NEW CONVEX MESH
	bool cookConvexMesh(std::string url, 
		                     unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices);

	NxConvexMesh* cookConvexMesh(unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices);    

	NxConvexMesh* loadConvexMesh(std::string url);
	NxActor* makeConvexMesh(ActorSync *aSync, NxConvexMesh* mesh, NxMat34 &mat, float &density);
	bool releaseConvexMesh(NxConvexMesh* mesh);

	//CLOTH
	NxClothMesh* cookClothMesh(unsigned int vertexCount,
		                      unsigned int indexCount,
							  NxVec3 *vertices,
							  NxU32 *indices,
							  unsigned int flags);
	bool cookClothMesh(std::string url, 
		                     unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices,
							 unsigned int flags);
	NxClothMesh* loadClothMesh(std::string url);
	
	NxCloth* makeCloth(ClothSync *aSync, NxClothMesh* mesh, NxMat34 &mat, NxU32 flags);
	bool releaseClothMesh(NxClothMesh* mesh);
	//SOFT BODY
    //NxSoftBodyMesh* cookSoftBodyMesh(unsigned int vertexCount,
		  //                    unsigned int indexCount,
				//			  NxVec3 *vertices,
				//			  NxU32 *indices);
    NxSoftBodyMesh* cookSoftBodyMesh(char* tetFile);
    bool cookSoftBodyMesh(char* tetfile, const char* destFile);
	NxSoftBodyMesh* loadSoftBodyMesh(char* file);
	NxSoftBody* makeSoftBody(SoftBodySync *aSync, NxSoftBodyMesh* mesh, NxMat34 &mat, float density, NxU32 flags);
    bool releaseSoftBodyMesh(NxSoftBodyMesh *mesh);

    NxHeightField* makeHeightField(NxHeightFieldDesc *desc);
	NxHeightFieldDesc* makeHeightFieldDesc(unsigned int columns, unsigned int rows, float verticalExtent, float thickness, float convexEdgeThreshold);
    void heightFieldDescSetData(NxHeightFieldDesc* desc, unsigned int row, unsigned int column, int height);
    void heightFieldDescSetData(NxHeightFieldDesc* desc, unsigned int row, unsigned int column, int height, int materialIndex0, int materialIndex1, int tessFlag);
    NxActor* instanceHeightField(NxHeightField* heightField, NxVec3 position, float heightScale, float rowScale, float columnScale);  
    NxActor* instanceHeightField(NxHeightField* heightField, NxVec3 position, float heightScale, float rowScale, float columnScale, int materialIndexHighBits, int holeMaterial);  
    void releaseHeightFieldDesc(NxHeightFieldDesc* heightFieldDesc);
    void releaseHeightField(NxHeightField* heightField);
	
	//NxActor* makeConvexMesh(ActorSync *aSync, std::string url, NxMat34 &mat, float &density);



	//--FORCE FIELD
    // 1. make shape groups
	// 2. kernal
	// 3. force field
	NxForceFieldLinearKernel* makeForceFieldLinearKernel(NxVec3 &constant, NxVec3 &falloffLinear,
		                                                 NxVec3 &falloffQuadratic, NxVec3 &noise);
	NxForceFieldShapeGroup* makeFFShapeGroup(bool excludeGroup);
	bool makeFFShapeBox(NxMat34 &mat, NxVec3 &dim);
	bool makeFFShapeSphere(NxMat34 &mat, float &radius);
	bool makeFFShapeCapsule(NxMat34 &mat, float &height, float &radius);
    bool makeFFShapeConvex(NxMat34 &mat, NxConvexMesh *cmesh);
	NxForceField* makeForceField(NxForceFieldLinearKernel* kernel, NxForceFieldCoordinates coordinates, NxActor* actor);
	void makeForceField();
	void clearFFShapes();
    void releaseForceFieldLinearKernel(NxForceFieldLinearKernel* kernel);
	void releaseForceField(NxForceField* forceField);
	void releaseForceFieldShapeGroup(NxForceFieldShapeGroup* group);

	//TRIANGLE MESH
	bool cookTriangleMesh(std::string url, 
		                     unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices);
	NxTriangleMesh* cookTriangleMesh(unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices);
	//NxActor* makeTriangleMesh(std::string url, NxMat34 &mat);

	NxTriangleMesh* loadTriangleMesh(std::string url);
	NxActor* makeTriangleMesh(NxTriangleMesh* mesh, NxMat34 &mat);
	bool  releaseTriangleMesh(NxTriangleMesh* mesh);

	void  setShapeLocPos(NxActor *actor, int shapeID, NxVec3 locPos);
	void  setShapeLocRot(NxActor *actor, int shapeID, NxMat33 locRot);
	void  setShapeGlobPos(NxActor *actor, int shapeID, NxVec3 globPos);
	void  setShapeGlobRot(NxActor *actor, int shapeID, NxMat33 globRot);
	//bool  addShapeBox(NxActor* actor, UserData *ud);
	//Wrong? addShape, should not really add mesh as it has no shape yet
	//int   addShapeConvex(NxActor *actor, UserDataMesh *ud, std::string url);
	int   addShape(NxShapeDesc *shapeDesc, NxActor *actor); 

	bool makeGroundPlane();
    void releaseGroundPlane();


	//For alternate update of actors, needed for NET version
    NxActiveTransform* getActiveTransforms(NxU32 &count);

	//For access to debug renderable
	const NxDebugRenderable** getDebugRenderable();  //Return long pointer as
	                                                 //*dbgRenderable_ changes
	                                                 //when scene changes. Result is
	                                                 //current scene is rendered


	std::vector<NxActor*> getActorVec();


	//********** User Allocator ************//
    UserAllocator *ua_;

	//******** Character Controller ********//
    NxControllerManager *cManager_;

	map<int, NxScene*> sceneMap_;
	map<int, NxScene*>::iterator sceneIt_;

    //******* RENDERER *******//
	const NxDebugRenderable *dbgRenderable_;

	//****** PARTICLES ******//
	//NxVec3 gParticleBuffer[10000];
	//NxU32  gParticleBufferNum;
	//int renderedParticleCount;
    //int MAX_PARTICLES;
	//vector<int> waitingVec_;
	//vector<int> renderedVec_;
	//***********************//

	NxPhysicsSDK *physicsSDK_;
	NxScene *currentScene_;
protected:

	//---SHAPES
    std::vector<NxShapeDesc*> shapeDescVec_;
	std::vector<NxForceFieldShapeDesc*> ffShapeDescVec_;
	
	void initialise(void);  //Reset pointers, clear maps etc
	//NxPhysicsSDK *physicsSDK_; 
	int currentSceneKey_;
//	NxScene *currentScene_;
	NxReal timeStep_;
	//UserData *ud_;
	ActorSync *ud_;
    NxActiveTransform *at_;
	NxU32 count_; //active transforms

	//What about multiple scenes?
	NxActor *groundPlane_;

	//Serialization
 //   NXU::NxuPhysicsCollection *collection_;
	//MyUserNotify *userNotify_;


};

#endif