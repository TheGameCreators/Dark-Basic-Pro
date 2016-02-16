#include "Physics.h"


Physics::Physics(void)
{
	this->initialise();
}

Physics::~Physics(void)
{
	if(physicsSDK_)
		this->stop();
}

bool Physics::start(void)
{
	//Check already initialised
	if(physicsSDK_)
	   return false;

    //PhysicsSDK
    NxCreateModule("PhysXCore", NULL, NX_PHYSICS_SDK_VERSION);
    NxCreateModule("PhysXCooking", NULL, NX_PHYSICS_SDK_VERSION);
    physicsSDK_ = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	
	if(!physicsSDK_)
	   return false;

	this->makeScene(1);

	NxMaterial *mat = this->currentScene_->getMaterialFromIndex(0);
	mat->setDynamicFriction(0.5);
	mat->setStaticFriction(0.5);
	mat->setRestitution(0.5);

	ua_ = new UserAllocator();
	cManager_ = NxCreateControllerManager(ua_);

	//Works!!! Cool
	//collection_ = NXU::createCollection();
	//userNotify_ = new MyUserNotify();
	//NXU::addActor
	//NXU::addScene(*collection, *currentScene_);
	//NXU::saveCollection(collection, "Test Collection", NXU::FT_XML);
	//NXU::releaseCollection(collection);

	return true;
}

bool Physics::start(bool enableHardware)
{
	//Check already initialised
	if(physicsSDK_)
	   return false;

    //PhysicsSDK
    NxCreateModule("PhysXCore", NULL, NX_PHYSICS_SDK_VERSION);
    NxCreateModule("PhysXCooking", NULL, NX_PHYSICS_SDK_VERSION);
    
	NxPhysicsSDKDesc desc;
	if(enableHardware)
	{
	    desc.flags &= ~NX_SDKF_NO_HARDWARE;
	}
	physicsSDK_ = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, 0, desc);
	

	if(!physicsSDK_)
	   return false;

	this->makeScene(1);

	NxMaterial *mat = this->currentScene_->getMaterialFromIndex(0);
	mat->setDynamicFriction(0.5);
	mat->setStaticFriction(0.5);
    mat->setRestitution(0.5);

	ua_ = new UserAllocator();
	cManager_ = NxCreateControllerManager(ua_);

	//Works!!! Cool
	//collection_ = NXU::createCollection();
	//userNotify_ = new MyUserNotify();
	//NXU::addActor
	//NXU::addScene(*collection, *currentScene_);
	//NXU::saveCollection(collection, "Test Collection", NXU::FT_XML);
	//NXU::releaseCollection(collection);

	return true;
}


//std::vector<NxActor*> Physics::getActorVec(){
//	return this->userNotify_->actorVec;
//}
//
////NOTE: Since there are not discrete callbacks for shapes, the user can iterate on the shapes in the actor desc and cast 
////      the 'userData' pointer to a 'const char *' to process the 'userProperties' field for each shape.  
//NxActor* Physics::loadActor(string url){
//	collection_ = NXU::loadCollection(url.c_str(), NXU::FT_XML);
//	NXU::instantiateCollection(collection_, *physicsSDK_, currentScene_, NULL, userNotify_);
//	NXU::releaseCollection(collection_);
//	collection_ = 0;
//	//Return actor
//	return 0;
//}
//
//void Physics::saveCollection(string url){
//	NXU::saveCollection(collection_, url.c_str(), NXU::FT_XML);
//	NXU::releaseCollection(collection_);
//	collection_ = 0;
//}
//
//void Physics::saveActor(NxActor *actor, string url, const char* userProp, const char* ID ){
//	//ActorSync *aSync = 0;
//	//aSync = (ActorSync*)actor->userData;
// //   const char *userProp = 0;
//	//userProp = (const char*)aSync;
//	//userProp = "testing...";
//	NXU::addActor(*collection_, *actor, userProp, ID);
//	//NXU::saveCollection(collection_, url.c_str(), NXU::FT_XML);
//	//NXU::releaseCollection(collection_);
//	//collection_ = 0;
//
//	//NXU::releasePersistentMemory();
//
//}

void Physics::stop()
{  
	//Calling release on the SDK releases everything(scenes, actors etc)
	if(physicsSDK_)
	{
		this->fetchResults();
		if(this->visualDebuggerConnected()){
			this->visualDebuggerOff();
		}
		NxReleaseControllerManager(this->cManager_);
		physicsSDK_->release();
	}
	delete ua_;
	//Reset pointers, clear maps
	this->initialise();
}

void Physics::initialise()
{
    //PhysicsSDK
    physicsSDK_ = 0;

	//Clear our scene pointers
	sceneMap_.clear();
    currentScene_ = 0;
	currentSceneKey_ = 0;
	dbgRenderable_ = 0;
	timeStep_ = 1.0f/60.0f;
	ud_ = 0;
	at_ = 0;
	count_ = 0;
	cManager_ = 0;
}

void Physics::setCurrentScene(int aSceneID)
{
    #ifdef DEBUG
		if(!physicsSDK_)
			return;
		if(sceneMap_.count(aSceneID) == 0)
		  return;
    #endif

	currentScene_ = sceneMap_[aSceneID];
	currentSceneKey_ = aSceneID;
    dbgRenderable_ = currentScene_->getDebugRenderable();
}

int Physics::getCurrentScene()
{
    #ifdef DEBUG
		if(!physicsSDK_)
			return 0;
    #endif

	return currentSceneKey_;
}

bool Physics::makeScene(int sceneID)
{
	return makeScene(sceneID, false);
 //   #ifdef DEBUG
	//	if(sceneID < 1)
	//		return false;
	//	if(!physicsSDK_)
	//		return false;
 //   #endif
	////Only if scene does not already exist
	//if(sceneMap_.count(sceneID) == 0)
	//{
	//	NxSceneDesc sceneDesc;
	//	//sceneDesc.simType = 
 //       sceneDesc.flags |= NX_SF_ENABLE_ACTIVETRANSFORMS;
	//	
	//	//physicsSDK_->
	//	//if
	//	  //  sceneDesc.simType = NX_SIMULATION_HW;
	//	
	//	NxScene *aScene = 0;
	//	aScene = physicsSDK_->createScene(sceneDesc);
	//	if(!aScene)
	//	   return false;
	//	aScene->setTiming(timeStep_ / 4.0f, 4, NX_TIMESTEP_FIXED);
	//	sceneMap_[sceneID] = aScene;
	//	currentScene_ = aScene;
	//	currentSceneKey_ = sceneID;
	//    dbgRenderable_ = currentScene_->getDebugRenderable();
	//	return true;
	//}
	//return false;
}

bool Physics::makeScene(int sceneID, bool hardwareScene)
{
    #ifdef DEBUG
		if(sceneID < 1)
			return false;
		if(!physicsSDK_)
			return false;
    #endif
	//Only if scene does not already exist
	if(sceneMap_.count(sceneID) == 0)
	{
		NxSceneDesc sceneDesc;
		//sceneDesc.simType = 
        sceneDesc.flags |= NX_SF_ENABLE_ACTIVETRANSFORMS;
				
		if(hardwareScene)
		{
		    sceneDesc.simType = NX_SIMULATION_HW;
		}
		NxScene *aScene = 0;
		aScene = physicsSDK_->createScene(sceneDesc);
		if(!aScene)
		   return false;
		aScene->setTiming(timeStep_ / 4.0f, 4, NX_TIMESTEP_FIXED);
		sceneMap_[sceneID] = aScene;
		currentScene_ = aScene;
		currentSceneKey_ = sceneID;
	    dbgRenderable_ = currentScene_->getDebugRenderable();
		return true;
	}
	return false;
}


void Physics::setTimeStep(float timeStep){
    timeStep_ = timeStep;
	for(sceneIt_ = sceneMap_.begin(); sceneIt_ != sceneMap_.end(); ++sceneIt_){
		(*sceneIt_).second->setTiming(timeStep_ / 4.0f, 4, NX_TIMESTEP_FIXED);
	}
}

float Physics::getTimeStep(){
    return timeStep_;
}

void Physics::releaseScene(int sceneID)
{
    #ifdef DEBUG
		if(!physicsSDK_)
			return;
		//If scene does not exist
		if(sceneMap_.count(sceneID) == 0)
		   return;
    #endif
	//If releasing current scene reset variables
	if(sceneID == currentSceneKey_){
		currentScene_ = 0;
		currentSceneKey_ = 0;
		dbgRenderable_ = 0;
	}
	//Release from SDK
	physicsSDK_->releaseScene(*sceneMap_[sceneID]);
	//Release from map
	sceneMap_.erase(sceneID);
}

int Physics::getSceneCount()
{
    #ifdef DEBUG
		if(!physicsSDK_)
			return -1;
    #endif
	return  physicsSDK_->getNbScenes();
}

void Physics::simulate()
{
    #ifdef DEBUG
		if(!currentScene_)
			return;
    #endif
	currentScene_->simulate(timeStep_);
	currentScene_->flushStream();
}

void Physics::simulate(float timeStep)
{
    #ifdef DEBUG
		if(!currentScene_)
			return;
    #endif
	currentScene_->simulate(timeStep);
	currentScene_->flushStream();
}

void Physics::fetchResults()
{
    #ifdef DEBUG
		if(!currentScene_)
			return;
    #endif
	currentScene_->fetchResults(NX_RIGID_BODY_FINISHED, true);
	dbgRenderable_ = currentScene_->getDebugRenderable();
}

//void Physics::update(){
//    #ifdef DEBUG
//		if(!currentScene_)
//			return;
//    #endif
//	at_ = currentScene_->getActiveTransforms(count_);
//	if(count_ && at_){
//		for(NxU32 i = 0; i < count_; i++){
//			ud_ = (UserData*)at_[i].userData;
//			//ud_->update(at[i].actor2World);  //best way(matrix), put option in
//			ud_->update(at_[i].actor->getGlobalOrientationQuat(), at_[i].actor->getGlobalPosition());
//		}
//	}
//	at_ = 0;
//	count_ = 0;
//}


//Pass in actor to user data update(), that way user has more control
void Physics::update(){
    #ifdef DEBUG
		if(!currentScene_)
			return;
    #endif

    //CONTROLLERS
	this->cManager_->updateControllers();
	//for(int i = 0; i < this->cManager_->getNbControllers(); i++){
	//	this->cManager_->
	//}

	//ACTORS
	at_ = currentScene_->getActiveTransforms(count_);
	if(count_ && at_){
		for(NxU32 i = 0; i < count_; i++){
			//ud_ = (UserData*)at_[i].userData;
		    //ud_->update(at_[i].actor);
			ud_ = (ActorSync*)at_[i].userData;
			if(ud_)                                              // ADDED JUST AFTER v1.0 RELEASE !!!
		        ud_->update(at_[i].actor);//ud_ should have pointer to its own actor?
		}
	}
	at_ = 0;
	count_ = 0;


 //   //CONTROLLERS
	//this->cManager_->updateControllers();
	////for(int i = 0; i < this->cManager_->getNbControllers(); i++){
	////	this->cManager_->
	////}

	//CLOTH
	NxCloth *cloth;
	NxCloth** clothArray = currentScene_->getCloths();
	NxU32 clothCount = currentScene_->getNbCloths();
	ClothSync *cSync = 0;
	for(unsigned int i = 0; i < clothCount; i++){
		cloth = clothArray[i];
		//if(!cloth->isSleeping()){  //Deformation issue
		   cSync = (ClothSync*)cloth->userData;
		   cSync->update(cloth);
		//}
	}

	//SOFT BODY
	NxSoftBody *sb;
	NxSoftBody** sbArray = currentScene_->getSoftBodies();
	NxU32 sbCount = currentScene_->getNbSoftBodies();
	SoftBodySync *sbSync = 0;
	for(unsigned int i = 0; i < sbCount; i++){
		sb = sbArray[i];
		if(!sb->isSleeping()){
		   sbSync = (SoftBodySync*)sb->userData;
		   sbSync->update();
		}
	}

	//WHEELS
}

void Physics::setActorPose(NxActor *actor, NxMat34 matPose){
   // actor->
}


//inline?
void Physics::setGravity(float x, float y, float z){
    #ifdef DEBUG
		if(!currentScene_)
			return;
    #endif
	currentScene_->setGravity(NxVec3(x, y, z));
}

void Physics::visualDebuggerOn(){
    #ifdef DEBUG
		if(!physicsSDK_)
			return;
    #endif
	physicsSDK_->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
}

void Physics::visualDebuggerOff(){
    #ifdef DEBUG
		if(!physicsSDK_)
			return;
    #endif
	physicsSDK_->getFoundationSDK().getRemoteDebugger()->flush();
	physicsSDK_->getFoundationSDK().getRemoteDebugger()->disconnect();
}

bool Physics::visualDebuggerConnected(){
    #ifdef DEBUG
		if(!physicsSDK_)
			return false;
    #endif
	return physicsSDK_->getFoundationSDK().getRemoteDebugger()->isConnected();
}

void Physics::setParameter(int parameter, float value){
    #ifdef DEBUG
	if(!physicsSDK_)
		return;
    #endif
	physicsSDK_->setParameter((NxParameter)parameter, value);
}

float Physics::getParameter(int parameter){
    #ifdef DEBUG
	if(!physicsSDK_)
		return -1;
    #endif
	return physicsSDK_->getParameter((NxParameter)parameter);
}

NxActiveTransform* Physics::getActiveTransforms(NxU32 &count){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
		return currentScene_->getActiveTransforms(count);	
}

const NxDebugRenderable** Physics::getDebugRenderable(){
    #ifdef DEBUG
		if(!physicsSDK_)
			return 0;
		if(!dbgRenderable_)
			return 0;
    #endif

	return &this->dbgRenderable_;
}

//bool Physics::makeGroundPlane(){
//    NxActorDesc actorDesc;
//    NxPlaneShapeDesc planeDesc;
//    actorDesc.shapes.pushBack(&planeDesc);
//    groundPlane_ = currentScene_->createActor(actorDesc);
//	return true;
//}
//
//NxActor* Physics::makeBox(UserData *ud){	
//    #ifdef DEBUG
//		if(!currentScene_)
//			return 0;
//    #endif
//	 //ShapeDesc
//	 NxBoxShapeDesc shapeDesc;
//	 shapeDesc.dimensions = ud->dim_ / 2;
//	 //root shape shares user data with actor
//	 shapeDesc.userData = (void*)ud;
//     //ActorDesc
//	 NxActorDesc actorDesc;
//	 actorDesc.shapes.pushBack(&shapeDesc);
//     //BodyDesc
//     NxBodyDesc body;
//	 body.mass = 10;
//	 actorDesc.body = &body; 
//	 //Actor
//	 NxActor* actor;
//	 if(currentScene_ && actorDesc.isValid()){
//	    actor = currentScene_->createActor(actorDesc);
//	    actor->userData = (void*)ud;
//		actor->setGlobalOrientation(ud->mat_.M);
//		actor->setGlobalPosition(ud->mat_.t);
//		return actor;
//	 }
//     return 0;
//}
NxD6Joint* Physics::makeJoint(NxJointDesc &d6JointDesc){
    return (NxD6Joint*)currentScene_->createJoint(d6JointDesc);
}

void Physics::makeJointSpherical(NxActor *actorA, NxActor *actorB, float globAnchorX, float globAnchorY, float globAnchorZ){
 
	NxVec3 globalAnchor(globAnchorX, globAnchorY, globAnchorZ);
	NxVec3 globalAxis(0, 1, 0);

	NxD6JointDesc d6Desc;
    d6Desc.actor[0] = actorA;    
	d6Desc.actor[1] = actorB;

    d6Desc.setGlobalAnchor(globalAnchor);//Important when DOFs are unlocked.    
	d6Desc.setGlobalAxis(globalAxis);//Joint configuration. 

	//d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;//Create a fixed joint.    
	//d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;    
	//d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;    
	d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;    
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;    
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;
    d6Desc.projectionMode = NX_JPM_NONE;


	NxF32 swing1Limit = 30;
	NxF32 swing2Limit = 30;
    //Limit motion on the swing axis to lie within the elliptical cone specified by the     
	//angles swing1Limit and swing2Limit, relative to the parent frame.    
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_LIMITED;    
	d6Desc.swing1Limit.value=NxMath::degToRad(swing1Limit);    
	d6Desc.swing1Limit.damping=0.5f;    
	d6Desc.swing1Limit.restitution=0.5f;

    d6Desc.swing2Motion = NX_D6JOINT_MOTION_LIMITED;    
	d6Desc.swing2Limit.value=NxMath::degToRad(swing2Limit);    
	d6Desc.swing2Limit.damping=0.5f;    
	d6Desc.swing2Limit.restitution=0.5f;

	NxF32 twistLowLimit = -15;
	NxF32 twistHighLimit = 15;
   //Set a hard twist limit, which limits the twist angle to twistLowLimit < twistAngle < twistHighLimit    
	d6Desc.twistMotion = NX_D6JOINT_MOTION_LIMITED;                
	d6Desc.twistLimit.low.value=NxMath::degToRad(twistLowLimit); //d6Joint angles are specified in radians.    
	d6Desc.twistLimit.low.damping=0.0f;    
	d6Desc.twistLimit.low.restitution=0.0f;    
	d6Desc.twistLimit.low.spring=0.0f;    
	d6Desc.twistLimit.high.value=NxMath::degToRad(twistHighLimit);    
	d6Desc.twistLimit.high.damping=0.0f;    
	d6Desc.twistLimit.high.restitution=0.0f;    
	d6Desc.twistLimit.high.spring=0.0f;
  
	//d6Desc.jointFlags |= NX_JF_COLLISION_ENABLED;
	//sphericalDesc.jointFlags |= NX_JF_COLLISION_ENABLED;
    NxD6Joint *d6Joint=(NxD6Joint*)currentScene_->createJoint(d6Desc);
	
	//   NxSphericalJointDesc sphericalDesc;
 //   sphericalDesc.actor[0] = actorA;
	//sphericalDesc.actor[1] = actorB;
	//sphericalDesc.setGlobalAnchor(NxVec3(globAnchorX, globAnchorY, globAnchorZ));
	//sphericalDesc.localNormal[0] = NxVec3(1, 0, 0);
 //   sphericalDesc.localNormal[1] = NxVec3(1, 0, 0);
	//sphericalDesc.setGlobalAxis(NxVec3(0, 1, 0));
	//

	////Dont let collision effect whiule testing
	////WRONG, this breaks the joint limits
 //   //currentScene_->setActorPairFlags(*actorA,*actorB,NX_IGNORE_PAIR);

 // 

	//sphericalDesc.jointFlags |= NX_JF_COLLISION_ENABLED;


	//sphericalDesc.jointFlags |= NX_SJF_SWING_LIMIT_ENABLED;
	//sphericalDesc.jointFlags |= NX_SJF_TWIST_LIMIT_ENABLED;

	//sphericalDesc.swingLimit.value = 0.3 * NxPi;
	//sphericalDesc.twistLimit.high.value = -1.1 * NxPi;
	//sphericalDesc.twistLimit.low.value = 1.1 * NxPi;

	//bool valid = sphericalDesc.swingLimit.isValid();

	////Keep this
	//NxJoint* joint = currentScene_->createJoint(sphericalDesc);
	//bool stop = true;


//d6Desc.actor[0]->getGlobalPose().multiply(jointLocalPos, globalPos);
//d6Desc.actor[0]->getGlobalOrientation().multiply(NxVec3(1.0f, 0.0f, 0.0f), globalAxis);
//d6Desc.setGlobalAnchor(globalPos);
//d6Desc.setGlobalAxis(globalAxis);


}

//NxActor* Physics::makeConvexMesh(UserDataMesh *ud){
//    #ifdef DEBUG
//		if(!currentScene_)
//			return 0;
//    #endif
//
//		//COOK MESH
//		NxInitCooking();
//		NxConvexMeshDesc conMeshDesc;
//		conMeshDesc.numVertices = ud->vertexCount_;
//		conMeshDesc.numTriangles = ud->indexCount_ / 3;
//        conMeshDesc.pointStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
//        conMeshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
//		conMeshDesc.points = ud->vertices_;
//		conMeshDesc.triangles = ud->indices_;
//	    conMeshDesc.flags = NX_CF_COMPUTE_CONVEX;
//        //coneMeshDesc.flags = 0;
//        //MemoryWriteBuffer buf;
//		NxCookConvexMesh(conMeshDesc, UserStream("cookMeshTest", false)); 
//		NxCloseCooking();
//		
//		//CREATE MESH FROM COOKED
//        NxConvexMesh *pMesh = physicsSDK_->createConvexMesh(UserStream("cookMeshTest", true)); 
//	    NxConvexShapeDesc convexShapeDesc;
//		convexShapeDesc.meshData = pMesh;
//		// NxConvexMesh *pMesh = physicsSDK_->createConvexMesh(MemoryReadBuffer(buf.data));
//
//		//Create actor
//        NxActorDesc actorDesc;
//		actorDesc.shapes.pushBack(&convexShapeDesc);  //Might not work, get desc from pMesh
//        NxBodyDesc bodyDesc;
//		actorDesc.body = &bodyDesc;
//		actorDesc.density = 10;
//		//actorDesc.body = NULL;
//	    assert(actorDesc.isValid());
//        
//		NxActor *actor = currentScene_->createActor(actorDesc);
//	    actor->userData = (void*)ud;
//
//		//Set position
//		//actor-setGlobalPose(ud->mat_);
//		actor->setGlobalOrientation(ud->mat_.M);
//		actor->setGlobalPosition(ud->mat_.t);
//		//actor->setGlobalPosition(ud->position_);
//		
//
//	    return actor;
//
//		//return 0;
//
//
//}


void Physics::setShapeLocPos(NxActor *actor, int shapeIndex, NxVec3 locPos){
    #ifdef DEBUG
		if(!actor)
			return;
		if(shapeIndex > (int)actor->getNbShapes() - 1)
			return;
    #endif
	actor->getShapes()[shapeIndex]->setLocalPosition(locPos);
}

void Physics::setShapeLocRot(NxActor *actor, int shapeIndex, NxMat33 locRot){
    #ifdef DEBUG
		if(shapeIndex > (int)actor->getNbShapes() - 1)
			return;
    #endif
	actor->getShapes()[shapeIndex]->setLocalOrientation(locRot);
}


void Physics::setShapeGlobPos(NxActor *actor, int shapeIndex, NxVec3 globPos){
    #ifdef DEBUG
		if(shapeIndex > (int)actor->getNbShapes() - 1)
			return;
    #endif
		actor->getShapes()[shapeIndex]->setGlobalPosition(globPos);
}

void Physics::setShapeGlobRot(NxActor *actor, int shapeIndex, NxMat33 globRot){
    #ifdef DEBUG
		if(shapeIndex > (int)actor->getNbShapes() - 1)
			return;
    #endif
	actor->getShapes()[shapeIndex]->setGlobalOrientation(globRot);
}
//bool Physics::addShapeBox(NxActor *actor, UserData *ud){
//    #ifdef DEBUG
//		if(!currentScene_)
//			return 0;
//    #endif
//	//ShapeDesc
//	NxBoxShapeDesc shapeDesc;
//	shapeDesc.dimensions = ud->dim_ / 2;
//
//	//New Winner
//	NxMat34 inverseMat;  //Undoes transformation
//	actor->getGlobalPose().getInverse(inverseMat);
//	//Swapped below
//	shapeDesc.localPose.multiply(inverseMat, ud->mat_);
//	shapeDesc.localPose.t = actor->getGlobalPose()%ud->position_;
//
//	NxShape *shape = actor->createShape(shapeDesc);
//	shape->userData = (void*)ud;
//	return true;
//}

void Physics::releaseForceFieldLinearKernel(NxForceFieldLinearKernel* kernel){
    #ifdef DEBUG
		if(!currentScene_)
			return;
		if(!kernel)
			return;
    #endif
	kernel->getScene().releaseForceFieldLinearKernel(*kernel);	
}

void Physics::releaseForceField(NxForceField* forceField){
    #ifdef DEBUG
		if(!currentScene_)
			return;
		if(!forceField)
			return;
    #endif
		forceField->getScene().releaseForceField(*forceField);
}

void Physics::releaseForceFieldShapeGroup(NxForceFieldShapeGroup* group){
    #ifdef DEBUG
		if(!currentScene_)
			return;
		if(!group)
			return;
    #endif
		group->getScene().releaseForceFieldShapeGroup(*group);
}

NxForceFieldLinearKernel* Physics::makeForceFieldLinearKernel(NxVec3 &constant, NxVec3 &falloffLinear,
															  NxVec3 &falloffQuadratic, NxVec3 &noise){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
	//Kernal
	NxForceFieldLinearKernelDesc linearKernelDesc;
	linearKernelDesc.constant = constant;
	linearKernelDesc.falloffLinear = falloffLinear;
	linearKernelDesc.falloffQuadratic = falloffQuadratic;
	linearKernelDesc.noise = noise;
    return currentScene_->createForceFieldLinearKernel(linearKernelDesc);
}

NxForceFieldShapeGroup* Physics::makeFFShapeGroup(bool excludeGroup){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

    NxForceFieldShapeGroupDesc sgDesc;
	if(excludeGroup){
       sgDesc.flags = NX_FFSG_EXCLUDE_GROUP;
	}
	for(int i = 0; i < this->ffShapeDescVec_.size(); i++){
		sgDesc.shapes.pushBack(this->ffShapeDescVec_.at(i));
	}
   
    NxForceFieldShapeGroup* pGroup = 0;
	pGroup = this->currentScene_->createForceFieldShapeGroup(sgDesc);
    return pGroup;
}

bool Physics::makeFFShapeBox(NxMat34 &mat, NxVec3 &dim){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
	 //ShapeDesc
	 NxBoxForceFieldShapeDesc* shapeDesc = new NxBoxForceFieldShapeDesc();
	 shapeDesc->dimensions = dim / 2;
	 shapeDesc->pose.M = mat.M;
	 shapeDesc->pose.t = mat.t;

	 if(shapeDesc->isValid()){
		 this->ffShapeDescVec_.push_back(shapeDesc);
         return true;
	 }
	 else{
         return false;
	 }
}

bool Physics::makeFFShapeSphere(NxMat34 &mat, float &radius){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
	 //ShapeDesc
	 NxSphereForceFieldShapeDesc* shapeDesc = new NxSphereForceFieldShapeDesc();
	 shapeDesc->radius = radius / 2;
	 shapeDesc->pose.M = mat.M;
	 shapeDesc->pose.t = mat.t;

	 if(shapeDesc->isValid()){
		 this->ffShapeDescVec_.push_back(shapeDesc);
         return true;
	 }
	 else{
         return false;
	 }
}


bool Physics::makeFFShapeCapsule(NxMat34 &mat, float &height, float &radius){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
	 //ShapeDesc
	 NxCapsuleForceFieldShapeDesc* shapeDesc = new NxCapsuleForceFieldShapeDesc();
	 shapeDesc->radius = radius;
	 shapeDesc->height = height;
	 shapeDesc->pose.M = mat.M;
	 shapeDesc->pose.t = mat.t;

	 if(shapeDesc->isValid()){
		 this->ffShapeDescVec_.push_back(shapeDesc);
         return true;
	 }
	 else{
         return false;
	 }
}

bool Physics::makeFFShapeConvex(NxMat34 &mat, NxConvexMesh *cmesh){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

	 //ShapeDesc
	 NxConvexForceFieldShapeDesc* shapeDesc = new NxConvexForceFieldShapeDesc();
	 shapeDesc->meshData = cmesh;
	 shapeDesc->pose.M = mat.M;
	 shapeDesc->pose.t = mat.t;

	 if(shapeDesc->isValid()){
		 this->ffShapeDescVec_.push_back(shapeDesc);
         return true;
	 }
	 else{
         return false;
	 }
}

NxForceField* Physics::makeForceField(NxForceFieldLinearKernel* kernel, NxForceFieldCoordinates coordinates, NxActor* actor){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif  
    NxForceFieldDesc fieldDesc;
    fieldDesc.kernel = kernel;
	fieldDesc.coordinates = coordinates;
	if(actor){
       fieldDesc.actor = actor;
	}

    NxForceField *pForceField = 0; 
	pForceField = this->currentScene_->createForceField(fieldDesc);
	return pForceField;
}


void Physics::makeForceField(){
    #ifdef DEBUG
		if(!currentScene_)
			return;
    #endif

	//Kernal
	NxForceFieldLinearKernelDesc linearKernelDesc;
	linearKernelDesc.constant = NxVec3(-100, 0, 0);
    NxForceFieldLinearKernel* pLinearKernel;
    pLinearKernel = currentScene_->createForceFieldLinearKernel(linearKernelDesc);

	//ForceField
	NxForceFieldDesc fieldDesc;
	fieldDesc.kernel = pLinearKernel;
	fieldDesc.coordinates = NX_FFC_SPHERICAL;
	//fieldDesc.actor = 
    NxForceField *pForceField; 
    pForceField = currentScene_->createForceField(fieldDesc);

	//Shapes
    NxBoxForceFieldShapeDesc boxDesc;
    boxDesc.dimensions = NxVec3(100, 100, 100);
    NxBoxForceFieldShape *pBoxFFShape = (NxBoxForceFieldShape*)pForceField->getIncludeShapeGroup().createShape(boxDesc);    

}

void Physics::clearFFShapes(){
	for(unsigned int i = 0; i < this->ffShapeDescVec_.size(); i++){
		NxForceFieldShapeDesc* ffShapeDesc = this->ffShapeDescVec_.at(i);
		if(ffShapeDesc){
			delete ffShapeDesc;
		}
	}
	this->ffShapeDescVec_.clear();
}

//
//void Physics::makeFluid(float rate, float life, int max){
//    #ifdef DEBUG
//		if(!currentScene_)
//			return;
//    #endif
//	//// Fluid particle globals
//	//NxVec3 gParticleBuffer[10000];
//	NxU32  gParticleBufferCap = 10000;
//	//NxU32  gParticleBufferNum = 0;
//	MAX_PARTICLES = max;
//    gParticleBufferNum = 0;
//    renderedParticleCount = 0;
//
//	NxParticleData particles;
//
//	//Set structure to pass particles, and receive them after every simulation step    NxParticleData particles;
//	particles.numParticlesPtr = &gParticleBufferNum;
//	particles.bufferPos = &gParticleBuffer[0].x;
//	particles.bufferPosByteStride = sizeof(NxVec3);
//
//	//particles.u
//
//	//for ( unsigned int x = 0; x < gParticleBufferNum; x++ )
//	//{
//	//	gParticleBuffer[x].x = rand() % 4200 - 2100;
//	//	gParticleBuffer[x].y = 0;
//	//	gParticleBuffer[x].z = rand() % 4200 - 2100;
//	//	gParticleBuffer[x] *= 0.01;
//	//}
//
//	//Create Emitter.
//	NxFluidEmitterDesc emitterDesc;
//	emitterDesc.setToDefault();
//	emitterDesc.dimensionX = 7;//0.01;
//	emitterDesc.dimensionY = 1;//0.01;
//	emitterDesc.relPose.id();
//	
//	//emitterDesc.relPose.t.z=-32;
//	//emitterDesc.relPose.t.y=-8;
//	emitterDesc.rate = rate;//100.0f;
//	emitterDesc.repulsionCoefficient=1;
//	emitterDesc.randomAngle = 0.5f;
//	emitterDesc.fluidVelocityMagnitude = 10;
//	emitterDesc.maxParticles = (NxU32)MAX_PARTICLES;
//	emitterDesc.particleLifetime = life;//5.0f;//12.0f;
//	emitterDesc.type = NX_FE_CONSTANT_FLOW_RATE;
//	emitterDesc.shape = NX_FE_ELLIPSE;
//
//	////Create Emitter2.
//	//NxFluidEmitterDesc emitterDesc2;
//	//emitterDesc2.setToDefault();
//	//emitterDesc2.dimensionX = 2;//0.01;
//	//emitterDesc2.dimensionY = 1;//0.01;
//	//emitterDesc2.relPose.id();
//	//emitterDesc2.relPose.t.z=5;
//	//emitterDesc2.relPose.t.y=5;
//	//emitterDesc2.rate = rate;//100.0f;
//	//emitterDesc2.repulsionCoefficient=1;
//	//emitterDesc2.randomAngle = 1.5f;
//	//emitterDesc2.fluidVelocityMagnitude = 5;
//	//emitterDesc2.maxParticles = 10;(NxU32)MAX_PARTICLES;
//	//emitterDesc2.particleLifetime = life;//5.0f;//12.0f;
//	//emitterDesc2.type = NX_FE_CONSTANT_FLOW_RATE;
//	//emitterDesc2.shape = NX_FE_ELLIPSE;
//
//
//
//	//Create fluid    NxFluidDesc fluidDesc;    fluidDesc.setToDefault();  
//	NxFluidDesc fluidDesc;
//	fluidDesc.flags &= ~NX_FF_HARDWARE; // non hardware...
//	fluidDesc.simulationMethod = NX_F_NO_PARTICLE_INTERACTION;
//	//fluidDesc.simulationMethod = NX_F_SPH;
//	fluidDesc.maxParticles = MAX_PARTICLES;
//	fluidDesc.restParticlesPerMeter = 1;
//	fluidDesc.stiffness = 1;
//	fluidDesc.viscosity = 100;//1;
//	fluidDesc.damping=0;
//	
//	fluidDesc.collisionDistanceMultiplier = 1;
//	fluidDesc.kernelRadiusMultiplier = 2.5;
//	fluidDesc.restitutionForDynamicShapes = fluidDesc.restitutionForStaticShapes = 0.3f;
//	fluidDesc.dynamicFrictionForDynamicShapes=fluidDesc.dynamicFrictionForStaticShapes=0.1f;
//	fluidDesc.staticFrictionForDynamicShapes=fluidDesc.staticFrictionForStaticShapes=0.1f;
//	fluidDesc.particlesWriteData = particles;
//	fluidDesc.emitters.push_back(emitterDesc);
//    
//	//fluidDesc.emitters.push_back(emitterDesc2);
//
//	NxU32 valid = fluidDesc.checkValid();
//	NxFluid *fluid = currentScene_->createFluid(fluidDesc);
//
//	//fluid->setFlag(NX_FF_DISABLE_GRAVITY, true);
//
//}

NxFluid* Physics::makeFluid( unsigned int maxParticles, float particleLife, float collisionRadiusMultiplier, unsigned int flags, NxVec3* posData, NxU32 posCount){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
    NxVec3* particleBuf = new NxVec3[maxParticles];
    NxU32* numParticles = new NxU32();
    NxU32* id = new NxU32[maxParticles];

	//Set structure to pass particles, and receive them after every simulation step  
	NxParticleData particles;
	particles.numParticlesPtr = numParticles;
	*numParticles = maxParticles;
	particles.bufferPos = &particleBuf[0].x;
	particles.bufferPosByteStride = sizeof(NxVec3);
	particles.bufferId = id;
	particles.bufferIdByteStride = sizeof(NxU32);
    
	if(particleLife > 0)
	{
	    particles.bufferLife = new NxF32[maxParticles];
	    particles.bufferLifeByteStride = sizeof(NxF32);
	}

	if(posCount > 0)
	{
		for ( unsigned int i = 0; i < maxParticles; i++ )
		{
			if(i > posCount - 1)
			{
				continue;
			}
			else
			{
				if(particleLife > 0){
				    particles.bufferLife[i] = particleLife;
				}
			    particleBuf[i] = posData[i];
			}
		}
	}
	//Create fluid    NxFluidDesc fluidDesc;    fluidDesc.setToDefault();  
	NxFluidDesc fluidDesc;
	//SET TO FLAGS WHEN TESTED
	//fluidDesc.flags &= ~NX_FF_HARDWARE; // non hardware...
	fluidDesc.flags = flags;
//NX_FEF_VISUALIZATION 
//NX_FF_VISUALIZATION
//NX_FF_ENABLED
	//fluidDesc.simulationMethod = NX_F_NO_PARTICLE_INTERACTION;
	//fluidDesc.simulationMethod = NX_F_SPH;
	fluidDesc.maxParticles = maxParticles;
    fluidDesc.particlesWriteData = particles;

    fluidDesc.kernelRadiusMultiplier = 2.0f;
	fluidDesc.collisionDistanceMultiplier = collisionRadiusMultiplier;

	// Slows down with ths at 50
	fluidDesc.restParticlesPerMeter = 1.0f;    
	//fluidDesc.stiffness = 200.0f;    
	//fluidDesc.viscosity = 22.0f;    
	//fluidDesc.restDensity = 1.0f;    
	//fluidDesc.damping = 0.0f;    
	fluidDesc.simulationMethod = NX_F_NO_PARTICLE_INTERACTION;    
	
	fluidDesc.particleCreationIdWriteData.bufferId = new NxU32[maxParticles];
	fluidDesc.particleCreationIdWriteData.bufferIdByteStride = sizeof(NxU32);
	fluidDesc.particleCreationIdWriteData.numIdsPtr = new NxU32;
	fluidDesc.particleDeletionIdWriteData.bufferId = new NxU32[maxParticles];
	fluidDesc.particleDeletionIdWriteData.bufferIdByteStride = sizeof(NxU32);
	fluidDesc.particleDeletionIdWriteData.numIdsPtr = new NxU32;
	
	// just use friction
	//fluidDesc.dynamicFrictionForStaticShapes = 1.0f;

	if(posCount > 0)
	{
	    fluidDesc.initialParticleData = particles;    
	}


	NxU32 valid = fluidDesc.checkValid();
	NxFluid *fluid = 0;
	fluid = currentScene_->createFluid(fluidDesc);
	//fluid->setFlag(NX_FF_DISABLE_GRAVITY, true);
	//fluid->setExternalAcceleration(NxVec3(-1, 0, -1));

	return fluid;
	//fluid->setFlag(NX_FF_DISABLE_GRAVITY, true);

}

void Physics::releaseFluid(NxFluid * fluid){
	if(fluid){
		delete[] fluid->getParticlesWriteData().bufferId;
		delete[] fluid->getParticlesWriteData().bufferPos;
		delete   fluid->getParticlesWriteData().numParticlesPtr;

		if(fluid->getParticlesWriteData().bufferLife){
            delete[] fluid->getParticlesWriteData().bufferLife;
		}

		delete[] fluid->getParticleCreationIdWriteData().bufferId;
		delete   fluid->getParticleCreationIdWriteData().numIdsPtr;
		delete[] fluid->getParticleDeletionIdWriteData().bufferId;
		delete   fluid->getParticleDeletionIdWriteData().numIdsPtr;

		fluid->getScene().releaseFluid(*fluid);
	}
}


//int Physics::makeEmitter(NxFluid* fluid, float dimX, float dimY)
//{
//    #ifdef DEBUG
//		if(!currentScene_)
//			return 0;
//		if(!fluid)
//			return 0;
//    #endif
//	NxFluidEmitterDesc desc;
//	NxFluidEmitter * emitter = fluid->createEmitter(desc);
//    
//}

