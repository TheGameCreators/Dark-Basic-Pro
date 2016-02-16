#include "Physics.h"


using namespace SOFTBODY;
class MyMesh : public SoftMeshInterface
{
public:
	class Tet
	{
	public:
        NxVec3 p1_;
		NxVec3 p2_;
		NxVec3 p3_;
		NxVec3 p4_;
	};

	virtual void softMeshIndices(NxU32 *i1, NxU32 *i2, NxU32 *i3, NxU32 *i4){
		indicesVec_.push_back(*i1);
		indicesVec_.push_back(*i2);
		indicesVec_.push_back(*i3);
		indicesVec_.push_back(*i4);
	}

	void softMeshPosition(NxVec3 *pos){
		posVec_.push_back(*pos);
	}
	void softMeshMaterial(const TetraMaterial &tm){
	};
	void softMeshTriangle(const TetraGraphicsVertex &v1,const TetraGraphicsVertex &v2,const TetraGraphicsVertex &v3){
		vertVec_.push_back(v1);
		vertVec_.push_back(v2);
		vertVec_.push_back(v3);
		bool stop = true;
	};
	void softMeshTetrahedron(const float *p1,const float *p2,const float *p3,const float *p4){
	    Tet tet;
		NxVec3 p1Vec(p1);
		NxVec3 p2Vec(p2);
		NxVec3 p3Vec(p3);
		NxVec3 p4Vec(p4);
	    tet.p1_ = p1Vec;
		tet.p2_ = p2Vec;
		tet.p3_ = p3Vec;
		tet.p4_ = p4Vec;
		tetVec_.push_back(tet);
	};
	std::vector<NxU32> indicesVec_;
	std::vector<NxVec3> posVec_;
	std::vector<Tet> tetVec_;
	std::vector<TetraGraphicsVertex> vertVec_;
};


//class UserNotify : public NXU_userNotify
//{
//    public:
//    NxSoftBodyMesh* sbm;
//	NxSoftBodyDesc* sbd;
//
//	bool NXU_preNotifySoftBody(NxSoftBodyDesc &t, const char *userProperties){
//		bool stop = true;
//	   return true;
//	};
//};


void Physics::releaseCCDSkeleton(NxShape* shape){
    NxCCDSkeleton *ccd = 0;
	ccd = shape->getCCDSkeleton();
	if(ccd){
		shape->setCCDSkeleton(0);
        this->physicsSDK_->releaseCCDSkeleton(*ccd);
	}
}

bool Physics::makeCCDSkeletonVertex(NxShape* shape){
	if(!shape)
		return false;

	//NxVec3 extents; 
    NxSimpleTriangleMesh triMesh;
	//if(shape->isBox())
	//{
 //       NxBoxShape *boxShape=shape->isBox();
 //       extents = boxShape->getDimensions();
	//}
	//else if(shape->isSphere())
	//{
	//    NxSphereShape *sphereShape = shape->isSphere(); 
	//	float radius = sphereShape->getRadius();
	//	extents.x = radius;
	//	extents.y = radius;
	//	extents.z = radius;
	//}
	//else if(shape->isConvexMesh()){
	//	NxConvexShape *convexShape = shape->isConvexMesh();
	//	NxBounds3 bounds;
	//	convexShape->getWorldBounds(bounds);
	//	bounds.getDimensions(extents);
	//	extents = extents / 2.0f;
	//}
	//else if(shape->isCapsule()){
	//	NxCapsuleShape *capsuleShape = shape->isCapsule();
	//	float radius = capsuleShape->getRadius();
	//	extents = NxVec3(radius, radius, radius);
	//}
	//else
	//{
 //       return false;
	//}

/*	if(boxShape!=NULL)        
	{ */           
        //NxVec3 extents = boxShape->getDimensions();
        //extents.arrayMultiply(extents, scaleMultiplier);
		//NxBox obb=NxBox(NxVec3(0.0f,0.0f,0.0f), extents, shape->getLocalOrientation());//NxMat33(NX_IDENTITY_MATRIX));
		triMesh.points=new NxVec3[1];
		triMesh.numVertices=1;
		triMesh.pointStrideBytes=sizeof(NxVec3);
		triMesh.numTriangles=0;//2*6;            
		triMesh.triangles=0;//new NxU32[2*6*3];            
		triMesh.triangleStrideBytes=0;//sizeof(NxU32)*3;            
		triMesh.flags=0;            
		//NxComputeBoxPoints(obb,(NxVec3 *)triMesh.points);            
		//memcpy((NxU32 *)triMesh.triangles,NxGetBoxTriangles(),sizeof(NxU32)*2*6*3);
	//}
	//else
	//	NX_ASSERT(!"Invalid shape type");

	NX_ASSERT(triMesh.isValid());

    NxCCDSkeleton *newSkeleton= this->physicsSDK_->createCCDSkeleton(triMesh);
    //Free mesh
	//delete []triMesh.triangles;
	delete []triMesh.points;
    
	shape->setFlag(NX_SF_DYNAMIC_DYNAMIC_CCD, true);
	shape->setCCDSkeleton(newSkeleton);
	return true;
}


bool Physics::makeCCDSkeletonBox(NxShape* shape, NxVec3 scaleMultiplier){
	if(!shape)
		return false;

	NxVec3 extents; 
    NxSimpleTriangleMesh triMesh;
	if(shape->isBox())
	{
        NxBoxShape *boxShape=shape->isBox();
        extents = boxShape->getDimensions();
	}
	else if(shape->isSphere())
	{
	    NxSphereShape *sphereShape = shape->isSphere(); 
		float radius = sphereShape->getRadius();
		extents.x = radius;
		extents.y = radius;
		extents.z = radius;
	}
	else if(shape->isConvexMesh()){
		NxConvexShape *convexShape = shape->isConvexMesh();
		NxBounds3 bounds;
		convexShape->getWorldBounds(bounds);
		bounds.getDimensions(extents);
		extents = extents / 2.0f;
	}
	else if(shape->isCapsule()){
		NxCapsuleShape *capsuleShape = shape->isCapsule();
		float radius = capsuleShape->getRadius();
		extents = NxVec3(radius, radius, radius);
	}
	else
	{
        return false;
	}

/*	if(boxShape!=NULL)        
	{ */           
        //NxVec3 extents = boxShape->getDimensions();
        extents.arrayMultiply(extents, scaleMultiplier);
		NxBox obb=NxBox(NxVec3(0.0f,0.0f,0.0f), extents, shape->getLocalOrientation());//NxMat33(NX_IDENTITY_MATRIX));
		triMesh.points=new NxVec3[8];
		triMesh.numVertices=8;
		triMesh.pointStrideBytes=sizeof(NxVec3);
		triMesh.numTriangles=2*6;            
		triMesh.triangles=new NxU32[2*6*3];            
		triMesh.triangleStrideBytes=sizeof(NxU32)*3;            
		triMesh.flags=0;            
		NxComputeBoxPoints(obb,(NxVec3 *)triMesh.points);            
		memcpy((NxU32 *)triMesh.triangles,NxGetBoxTriangles(),sizeof(NxU32)*2*6*3);
	//}
	//else
	//	NX_ASSERT(!"Invalid shape type");

	NX_ASSERT(triMesh.isValid());

    NxCCDSkeleton *newSkeleton= this->physicsSDK_->createCCDSkeleton(triMesh);
    //Free mesh
	delete []triMesh.triangles;
	delete []triMesh.points;
    
	shape->setFlag(NX_SF_DYNAMIC_DYNAMIC_CCD, true);
	shape->setCCDSkeleton(newSkeleton);
	return true;
}


bool Physics::makeGroundPlane(){
    #ifdef DEBUG
		if(!currentScene_)
			return false;
    #endif
    NxActorDesc actorDesc;
    NxPlaneShapeDesc planeDesc;
    actorDesc.shapes.pushBack(&planeDesc);
    groundPlane_ = currentScene_->createActor(actorDesc);
	return true;
}

void Physics::releaseGroundPlane(){
    #ifdef DEBUG
		if(!currentScene_)
			return;
    #endif
	if(groundPlane_){
        groundPlane_->getScene().releaseActor(*groundPlane_);
	}
}

NxActor* Physics::makeCompoundActor(ActorSync *aSync, NxMat34 &mat, float &density){
     //ActorDesc
	 NxActorDesc actorDesc;
	 for(unsigned int i = 0; i < this->shapeDescVec_.size(); i++){
		 //Add shapes
		 actorDesc.shapes.pushBack(this->shapeDescVec_.at(i));
	 }

     //BodyDesc
	 if(density){
        NxBodyDesc body;
		actorDesc.density = density;
	    actorDesc.body = &body;
	 }

	 //Actor
	 NxActor* actor = 0;
	 if(actorDesc.isValid()){
	    actor = currentScene_->createActor(actorDesc);
	    actor->userData = (void*)aSync;
		actor->setGlobalOrientation(mat.M);
		actor->setGlobalPosition(mat.t);
		return actor;
	 }
     return 0;
}

//-- add shape to shape desc vector
bool Physics::makeShapeBox(NxMat34 &mat, NxVec3 &dim, float &density){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

	 //ShapeDesc
	 NxBoxShapeDesc* shapeDesc = new NxBoxShapeDesc();
	 shapeDesc->dimensions = dim / 2;
	 shapeDesc->localPose.M = mat.M;
	 shapeDesc->localPose.t = mat.t;
	 shapeDesc->density = density;

	 if(shapeDesc->isValid()){
		 this->shapeDescVec_.push_back(shapeDesc);
         return true;
	 }
	 else{
         return false;
	 }
}

bool Physics::makeShapeSphere(NxMat34 &mat, float &radius, float &density){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

	 //ShapeDesc
	 NxSphereShapeDesc* shapeDesc = new NxSphereShapeDesc();
	 shapeDesc->radius = radius / 2;
	 shapeDesc->localPose.M = mat.M;
	 shapeDesc->localPose.t = mat.t;
	 shapeDesc->density = density;

	 if(shapeDesc->isValid()){
		 this->shapeDescVec_.push_back(shapeDesc);
         return true;
	 }
	 else{
         return false;
	 }
}

bool Physics::makeShapeCapsule(NxMat34 &mat, float &height, float &radius, float &density){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

	 //ShapeDesc
	 NxCapsuleShapeDesc* shapeDesc = new NxCapsuleShapeDesc();
	 shapeDesc->radius = radius;
	 shapeDesc->height = height;
	 shapeDesc->localPose.M = mat.M;
	 shapeDesc->localPose.t = mat.t;
	 shapeDesc->density = density;

	 if(shapeDesc->isValid()){
		 this->shapeDescVec_.push_back(shapeDesc);
         return true;
	 }
	 else{
         return false;
	 }
}
//-- add shape to shape desc vector
bool Physics::makeShapeConvex(NxMat34 &mat, NxConvexMesh *cmesh, float &density){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

	 //ShapeDesc
	 NxConvexShapeDesc* shapeDesc = new NxConvexShapeDesc();
	 shapeDesc->meshData = cmesh;
	 shapeDesc->density = density;
	 shapeDesc->localPose.M = mat.M;
	 shapeDesc->localPose.t = mat.t;

	 if(shapeDesc->isValid()){
		 this->shapeDescVec_.push_back(shapeDesc);
         return true;
	 }
	 else{
         return false;
	 }
}

void Physics::clearShapes(){
	for(unsigned int i = 0; i < this->shapeDescVec_.size(); i++){
		NxShapeDesc* shapeDesc = this->shapeDescVec_.at(i);
		if(shapeDesc){
			delete shapeDesc;
		}
	}
	this->shapeDescVec_.clear();
}

NxActor* Physics::makeBox(ActorSync *aSync, NxMat34 &mat, NxVec3 &dim, float &density, NxVec3 &locPos){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

	 //ShapeDesc
	 NxBoxShapeDesc shapeDesc;
	 shapeDesc.dimensions = dim / 2;
	 shapeDesc.localPose.t = locPos; 

     //ActorDesc
	 NxActorDesc actorDesc;
     actorDesc.shapes.pushBack(&shapeDesc);

     //BodyDesc
	 if(density){
        NxBodyDesc body;
		actorDesc.density = density;
	    actorDesc.body = &body;
	 }
	 //Actor
	 NxActor* actor;
	 if(actorDesc.isValid()){
	    actor = currentScene_->createActor(actorDesc);
	    actor->userData = (void*)aSync;
		actor->setGlobalOrientation(mat.M);
		actor->setGlobalPosition(mat.t);
		return actor;
	 }
     return 0;
}

NxActor* Physics::makeCapsule(ActorSync *aSync, NxMat34 &mat, float &height, float &radius, float &density, NxMat33 &locPose){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
	 //ShapeDesc
	 NxCapsuleShapeDesc shapeDesc;
	 shapeDesc.height = height;
	 shapeDesc.radius = radius;
	 shapeDesc.localPose.M = locPose;

     //ActorDesc
	 NxActorDesc actorDesc;
     actorDesc.shapes.pushBack(&shapeDesc);

     //BodyDesc
	 if(density){
        NxBodyDesc body;
		actorDesc.density = density;
	    actorDesc.body = &body;
	 }
	 //Actor
	 NxActor* actor;
	 if(actorDesc.isValid()){
	    actor = currentScene_->createActor(actorDesc);
	    actor->userData = (void*)aSync;
		actor->setGlobalOrientation(mat.M);
		actor->setGlobalPosition(mat.t);
		return actor;
	 }
     return 0;
}


NxActor* Physics::makeCapsule(ActorSync *aSync, NxMat34 &mat, float &height, float &radius, float &density, NxVec3 &locPos){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
	 //ShapeDesc
	 NxCapsuleShapeDesc shapeDesc;
	 shapeDesc.height = height;
	 shapeDesc.radius = radius;
	 shapeDesc.localPose.t = locPos;

     //ActorDesc
	 NxActorDesc actorDesc;
     actorDesc.shapes.pushBack(&shapeDesc);

     //BodyDesc
	 if(density){
        NxBodyDesc body;
		actorDesc.density = density;
	    actorDesc.body = &body;
	 }
	 //Actor
	 NxActor* actor;
	 if(actorDesc.isValid()){
	    actor = currentScene_->createActor(actorDesc);
	    actor->userData = (void*)aSync;
		actor->setGlobalOrientation(mat.M);
		actor->setGlobalPosition(mat.t);
		return actor;
	 }
     return 0;
}



NxActor* Physics::makeSphere(ActorSync *aSync, NxMat34 &mat, float &radius, float &density, NxVec3 &locPos){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

	 //ShapeDesc
	 NxSphereShapeDesc shapeDesc;
	 shapeDesc.radius = radius / 2;
	 shapeDesc.localPose.t = locPos;

     //ActorDesc
	 NxActorDesc actorDesc;
     actorDesc.shapes.pushBack(&shapeDesc);

     //BodyDesc
	 if(density){
        NxBodyDesc body;
		actorDesc.density = density;
	    actorDesc.body = &body;
	 }
	 //Actor
	 NxActor* actor;
	 if(actorDesc.isValid()){
	    actor = currentScene_->createActor(actorDesc);
	    actor->userData = (void*)aSync;
		actor->setGlobalOrientation(mat.M);
		actor->setGlobalPosition(mat.t);
		return actor;
	 }
     return 0;
}


NxActor* Physics::makeWheel(ActorSync *aSync, NxMat34 &mat, float &radius, float &density, NxVec3 &locPos){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

	 //ShapeDesc
	 NxWheelShapeDesc shapeDesc;
	 shapeDesc.localPose.t = locPos;
	// shapeDesc.
	 shapeDesc.radius = radius / 2;
//NX_WF_INPUT_LNG_SLIPVELOCITY
//NX_WF_INPUT_LAT_SLIPVELOCITY
     //ActorDesc
	 NxActorDesc actorDesc;
     actorDesc.shapes.pushBack(&shapeDesc);

     //BodyDesc
	 if(density){
        NxBodyDesc body;
		actorDesc.density = density;
	    actorDesc.body = &body;
	 }
	 //Actor
	 NxActor* actor;
	 if(actorDesc.isValid()){
	    actor = currentScene_->createActor(actorDesc);
	    actor->userData = (void*)aSync;
		actor->setGlobalOrientation(mat.M);
		actor->setGlobalPosition(mat.t);
		return actor;
	 }
     return 0;
}



NxMaterial* Physics::makeMaterial(float restitution, float staticFriction, float dynamicFriction){
    #ifdef DEBUG
	   if(!currentScene_)
		return 0;
    #endif
	   NxMaterialDesc desc;
	   desc.restitution = restitution;
	   desc.staticFriction = staticFriction;
	   desc.dynamicFriction = dynamicFriction;
	   NxMaterial* material = 0;
	   material = currentScene_->createMaterial(desc);
	   return material;
}

////Not needed?
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
//	shapeDesc.localPose.t = actor->getGlobalPose()%ud->mat_.t;//position_;
//	//shapeDesc.localPose.t = actor->getGlobalPose()%ud->position_;
//
//	NxShape *shape = actor->createShape(shapeDesc);
//	shape->userData = (void*)ud;
//	return true;
//}

//bool Physics::cookConvexMesh(MeshData *mesh, std::string url){
//    #ifdef DEBUG
//		if(!currentScene_)
//			return false;
//    #endif
//		NxConvexMeshDesc conMeshDesc;
//		conMeshDesc.numVertices = mesh->vertexCount_;
//		conMeshDesc.numTriangles = mesh->indexCount_ / 3;
//        conMeshDesc.pointStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
//        conMeshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
//		conMeshDesc.points = mesh->vertices_;
//		conMeshDesc.triangles = mesh->indices_;
//	    conMeshDesc.flags = NX_CF_COMPUTE_CONVEX;
//		//COOK MESH
//		NxInitCooking();
//		bool result = NxCookConvexMesh(conMeshDesc, UserStream(url.c_str(), false)); 
//		NxCloseCooking();
//		return result;
//}
bool Physics::cookConvexMesh(std::string url, 
		                     unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices){

		NxConvexMeshDesc conMeshDesc;
		conMeshDesc.numVertices = vertexCount;
		conMeshDesc.numTriangles = indexCount / 3;
        conMeshDesc.pointStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
        conMeshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
		conMeshDesc.points = vertices;
		conMeshDesc.triangles = indices;
	    conMeshDesc.flags = NX_CF_COMPUTE_CONVEX;
		//COOK MESH

		if(conMeshDesc.checkValid() != 0)
			return false;

		NxInitCooking();
		bool result = NxCookConvexMesh(conMeshDesc, UserStream(url.c_str(), false)); 
		NxCloseCooking();
		return result;
}

NxConvexMesh* Physics::cookConvexMesh(unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices){

		NxConvexMeshDesc conMeshDesc;
		conMeshDesc.numVertices = vertexCount;
		conMeshDesc.numTriangles = indexCount / 3;
        conMeshDesc.pointStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
        conMeshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
		conMeshDesc.points = vertices;
		conMeshDesc.triangles = indices;
	    conMeshDesc.flags = NX_CF_COMPUTE_CONVEX;
		//COOK MESH

		if(conMeshDesc.checkValid() != 0)
			return false;

		//COOK MESH TO MEMORY
		NxInitCooking();
		MemoryWriteBuffer buf;
		bool result = NxCookConvexMesh(conMeshDesc, buf);
		NxCloseCooking();

		if(!result)
			return 0;

        //CREATE MESH FROM COOKED
	    NxConvexMesh *pMesh = 0;
		pMesh = physicsSDK_->createConvexMesh(MemoryReadBuffer(buf.data));
		return pMesh;
}

NxConvexMesh* Physics::loadConvexMesh(std::string url){
	NxConvexMesh *pMesh = 0;
	pMesh = physicsSDK_->createConvexMesh(UserStream(url.c_str(), true));
	if(pMesh)
		return pMesh;
	else
	    return 0;
}

bool Physics::cookTriangleMesh(std::string url, 
		                       unsigned int vertexCount,
		                       unsigned int indexCount,
							   NxVec3 *vertices,
							   NxU32 *indices){

		NxTriangleMeshDesc meshDesc;
		meshDesc.numVertices = vertexCount;

        meshDesc.pointStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
        meshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
		meshDesc.points = vertices;
		if(indexCount > 0){
		   meshDesc.numTriangles = indexCount / 3;
		   meshDesc.triangles = indices;
		}
	    meshDesc.flags = 0;

		if(!meshDesc.isValid())
			return false;
		//COOK MESH
		NxInitCooking();
		bool result = NxCookTriangleMesh(meshDesc, UserStream(url.c_str(), false)); 
		NxCloseCooking();
		return result;
}

NxTriangleMesh* Physics::cookTriangleMesh(unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices){
		NxTriangleMeshDesc meshDesc;
		meshDesc.numVertices = vertexCount;
        
        meshDesc.pointStrideBytes = sizeof(NxVec3);    //Number of bytes from one vertex to the next.
        meshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
		meshDesc.points = vertices;
		if(indexCount > 0){
		   meshDesc.numTriangles = indexCount / 3;
		   meshDesc.triangles = indices;
		}
	    meshDesc.flags = 0;

		if(!meshDesc.isValid())
			return false;

		//COOK MESH TO MEMORY
		NxInitCooking();
		MemoryWriteBuffer buf;
		bool result = NxCookTriangleMesh(meshDesc, buf);
		NxCloseCooking();

		if(!result)
			return 0;

        //CREATE MESH FROM COOKED
	    NxTriangleMesh *pMesh = 0;
		pMesh = physicsSDK_->createTriangleMesh(MemoryReadBuffer(buf.data));
		buf.clear();
		return pMesh;
}


NxTriangleMesh* Physics::loadTriangleMesh(std::string url){
	NxTriangleMesh *pMesh = 0;
	pMesh = physicsSDK_->createTriangleMesh(UserStream(url.c_str(), true));
	if(pMesh)
		return pMesh;
	else
	    return 0;
}

NxActor* Physics::makeTriangleMesh(NxTriangleMesh* mesh, NxMat34 &mat){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif

	//Create triangle mesh instance
	NxTriangleMeshShapeDesc meshShapeDesc;
	meshShapeDesc.meshData = mesh;
	if(!meshShapeDesc.isValid())
		return 0;

	//Create actor
    NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&meshShapeDesc);  //Might not work, get desc from pMesh   
	//BodyDesc
	actorDesc.body = 0;

    assert(actorDesc.isValid());
	NxActor *actor = 0;
	actor = currentScene_->createActor(actorDesc);
	actor->setGlobalOrientation(mat.M);
	actor->setGlobalPosition(mat.t);
    return actor;
}


NxHeightField* Physics::makeHeightField(NxHeightFieldDesc *desc){
	NxHeightField* heightField = physicsSDK_->createHeightField(*desc);
	if(heightField){
        return heightField;
	}
	else return 0;
}

NxHeightFieldDesc* Physics::makeHeightFieldDesc(unsigned int columns, unsigned int rows, float verticalExtent, float thickness, float convexEdgeThreshold){
    NxHeightFieldDesc *heightFieldDesc   = new NxHeightFieldDesc();
	heightFieldDesc->nbColumns           = columns;
	heightFieldDesc->nbRows              = rows;
	heightFieldDesc->verticalExtent      = verticalExtent;
	heightFieldDesc->thickness           = thickness;
	heightFieldDesc->convexEdgeThreshold = convexEdgeThreshold;

    //Allocate storage for samples.    
	heightFieldDesc->samples             = new NxU32[columns*rows];    
	heightFieldDesc->sampleStride        = sizeof(NxU32);

	if(heightFieldDesc->isValid()){
	    return heightFieldDesc;
	}
	else{
        return 0;
	}
}

void Physics::heightFieldDescSetData(NxHeightFieldDesc* desc, unsigned int row, unsigned int column, int height){	
	if(row >= desc->nbRows || column >= desc->nbColumns){
       return;
	}
	if(row < 0 || column < 0){
       return;
	}
	int arrPos = (row) * desc->nbColumns + (column);                 //((row-1) * (column-1)) + (column-1); 
    NxU8* currentByte = (NxU8*)desc->samples;
	currentByte += arrPos * desc->sampleStride;
	NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;            
	currentSample->height = height;
}

void Physics::heightFieldDescSetData(NxHeightFieldDesc* desc, unsigned int row, unsigned int column, int height, int materialIndex0, int materialIndex1, int tessFlag){	
	if(row >= desc->nbRows || column >= desc->nbColumns){
       return;
	}
	if(row < 0 || column < 0){
       return;
	}
	int arrPos = (row) * desc->nbColumns + (column);                 //((row-1) * (column-1)) + (column-1); 
    NxU8* currentByte = (NxU8*)desc->samples;
	currentByte += arrPos * desc->sampleStride;
	NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;            
	currentSample->height = height;
	currentSample->materialIndex0 = materialIndex0;
	currentSample->materialIndex1 = materialIndex1;
	currentSample->tessFlag = tessFlag;
}

NxActor* Physics::instanceHeightField(NxHeightField* heightField, NxVec3 position, float heightScale, float rowScale, float columnScale){
    NxHeightFieldShapeDesc heightFieldShapeDesc;
	heightFieldShapeDesc.heightField     = heightField;
    heightFieldShapeDesc.heightScale     = heightScale;
    heightFieldShapeDesc.rowScale        = rowScale;
    heightFieldShapeDesc.columnScale     = columnScale;
    heightFieldShapeDesc.materialIndexHighBits = 0;
    //heightFieldShapeDesc.holeMaterial = 2;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&heightFieldShapeDesc);
	actorDesc.globalPose.t = position;
	NxActor* actor = 0;
    actor = this->currentScene_->createActor(actorDesc);
    return actor;
}

NxActor* Physics::instanceHeightField(NxHeightField* heightField, NxVec3 position, float heightScale, float rowScale, float columnScale, int materialIndexHighBits, int holeMaterial){
    NxHeightFieldShapeDesc heightFieldShapeDesc;
	heightFieldShapeDesc.heightField     = heightField;
    heightFieldShapeDesc.heightScale     = heightScale;
    heightFieldShapeDesc.rowScale        = rowScale;
    heightFieldShapeDesc.columnScale     = columnScale;
    heightFieldShapeDesc.materialIndexHighBits = materialIndexHighBits;
    heightFieldShapeDesc.holeMaterial = holeMaterial;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&heightFieldShapeDesc);
	actorDesc.globalPose.t = position;
	NxActor* actor = 0;
	actor = this->currentScene_->createActor(actorDesc);
    return actor;
}

void Physics::releaseHeightFieldDesc(NxHeightFieldDesc* desc){
	if(desc->samples){ 
		delete[] desc->samples;
	}
	delete desc;
}

void Physics::releaseHeightField(NxHeightField* hf){
    //--reference count should have been checked
	this->physicsSDK_->releaseHeightField(*hf);
}

bool Physics::releaseTriangleMesh(NxTriangleMesh* pMesh){
	if(pMesh->getReferenceCount() == 0){
	   physicsSDK_->releaseTriangleMesh(*pMesh);
	   return true;
	}
	else{
	   return false;
	}
}

//NxActor* Physics::makeTriangleMesh(std::string url, NxMat34 &mat){
//    #ifdef DEBUG
//		if(!currentScene_)
//			return 0;
//    #endif
//
//	//CREATE MESH FROM COOKED
//	//NOTE: Keep the pMesh somewhere and instance from there, not load?
//	//eg CookTriMesh
//	//   LoadTriMesh
//	//   MakeTriMesh
//	NxTriangleMesh *pMesh = 0;
//	pMesh = physicsSDK_->createTriangleMesh(UserStream(url.c_str(), true));
//	if(!pMesh)
//	   return 0;
//
//	//Create triangle mesh instance
//	NxTriangleMeshShapeDesc meshShapeDesc;
//	meshShapeDesc.meshData = pMesh;
//	if(!meshShapeDesc.isValid())
//		return 0;
//
//	//Create actor
//    NxActorDesc actorDesc;
//	actorDesc.shapes.pushBack(&meshShapeDesc);  //Might not work, get desc from pMesh   
//
//	//BodyDesc
//	actorDesc.body = 0;
//
//    assert(actorDesc.isValid());
//	NxActor *actor = currentScene_->createActor(actorDesc);
//	actor->setGlobalOrientation(mat.M);
//	actor->setGlobalPosition(mat.t);
//    return actor;
//}


//bool Physics::cookConvexMesh(UserDataMesh *ud, std::string url){
//  //  #ifdef DEBUG
//		//if(!currentScene_)
//		//	return false;
//  //  #endif
//		NxConvexMeshDesc conMeshDesc;
//		conMeshDesc.numVertices = ud->vertexCount_;
//		conMeshDesc.numTriangles = ud->indexCount_ / 3;
//        conMeshDesc.pointStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
//        conMeshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
//		conMeshDesc.points = ud->vertices_;
//		conMeshDesc.triangles = ud->indices_;
//	    conMeshDesc.flags = NX_CF_COMPUTE_CONVEX;
//		//COOK MESH
//		NxInitCooking();
//		bool result = NxCookConvexMesh(conMeshDesc, UserStream(url.c_str(), false)); 
//		NxCloseCooking();
//		return result;
//}

NxActor* Physics::makeConvexMesh(ActorSync *aSync, NxConvexMesh* pMesh, NxMat34 &mat, float &density){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
	NxConvexShapeDesc convexShapeDesc;
	convexShapeDesc.meshData = pMesh;
	convexShapeDesc.userData = (void*)aSync;
	if(!convexShapeDesc.isValid())
		return 0;
	//Create actor
    NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&convexShapeDesc);  //Might not work, get desc from pMesh   
	//BodyDesc
	if(density){
	   NxBodyDesc body;
       actorDesc.density = density;
	   actorDesc.body = &body;
	}
    assert(actorDesc.isValid());
	NxActor *actor = currentScene_->createActor(actorDesc);
    actor->userData = (void*)aSync;
	actor->setGlobalOrientation(mat.M);
	actor->setGlobalPosition(mat.t);
    return actor;
}

bool Physics::releaseConvexMesh(NxConvexMesh* pMesh){
	if(pMesh->getReferenceCount() == 0){
	   physicsSDK_->releaseConvexMesh(*pMesh);
	   return true;
	}
	else{
	   return false;
	}
}



//CLOTH

NxClothMesh* Physics::cookClothMesh(unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices,
							 unsigned int flags){
		NxClothMeshDesc clothMeshDesc;
		clothMeshDesc.numVertices = vertexCount;
        clothMeshDesc.pointStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
		clothMeshDesc.points = vertices;
		if(indexCount > 0){
            clothMeshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
		    clothMeshDesc.numTriangles = indexCount / 3;
		    clothMeshDesc.triangles = indices;
		}
		clothMeshDesc.flags = flags;
		if(clothMeshDesc.checkValid() != 0)
			return false;

		//COOK MESH TO MEMORY
		NxInitCooking();
		MemoryWriteBuffer buf;
		bool result = NxCookClothMesh(clothMeshDesc, buf);
		NxCloseCooking();

		if(!result)
			return 0;

        //CREATE MESH FROM COOKED
	    NxClothMesh *pMesh = 0;
		pMesh = physicsSDK_->createClothMesh(MemoryReadBuffer(buf.data));
		return pMesh;
}

bool Physics::cookClothMesh(std::string url, 
		                     unsigned int vertexCount,
		                     unsigned int indexCount,
							 NxVec3 *vertices,
							 NxU32 *indices,
							 unsigned int flags){

		NxClothMeshDesc clothMeshDesc;
		clothMeshDesc.numVertices = vertexCount;
        clothMeshDesc.pointStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
		clothMeshDesc.points = vertices;
		if(indexCount > 0){
		    clothMeshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
			clothMeshDesc.numTriangles = indexCount / 3;
		    clothMeshDesc.triangles = indices;
		}
        //NX_MF_HARDWARE_MESH
		clothMeshDesc.flags = flags; //NX_CLOTH_MESH_TEARABLE;
		if(clothMeshDesc.checkValid() != 0)
			return false;

		NxInitCooking();
		bool result = NxCookClothMesh(clothMeshDesc, UserStream(url.c_str(), false)); 
		NxCloseCooking();
		return result;
}

NxClothMesh* Physics::loadClothMesh(std::string url){
	NxClothMesh *pMesh = 0;
	pMesh = physicsSDK_->createClothMesh(UserStream(url.c_str(), true));
	if(pMesh)
		return pMesh;
	else
	    return 0;
}

bool Physics::releaseClothMesh(NxClothMesh* pMesh){
	if(pMesh->getReferenceCount() == 0){
	   physicsSDK_->releaseClothMesh(*pMesh);
	   return true;
	}
	else{
	   return false;
	}
}
//NxU32* indexBuffer = new NxU32[7000];
//NxVec3* vertexBuffer = new NxVec3[5000];
//NxVec3* verticesNormalBuffer = new NxVec3[5000];
//NxU32 numVertices;
//NxU32 numIndices;
//NxU32 dirtyFlag;
//
//NxU32 numParentIndices;
//NxMeshData *receiveBuffers = new NxMeshData();
NxCloth* Physics::makeCloth(ClothSync *aSync, NxClothMesh* pMesh, NxMat34 &mat, NxU32 flags){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
	NxClothDesc clothDesc;

	clothDesc.clothMesh = pMesh;
	clothDesc.meshData = aSync->meshData_;
	clothDesc.thickness = 1.0f;
	clothDesc.globalPose = mat;
	clothDesc.flags = flags;
    //clothDesc.flags |= NX_CLF_TEARABLE;
	//clothDesc.meshData = pMesh;
	//ShapeDesc.userData = (void*)aSync;
//	if(!clothDesc.isValid())
//		return 0;
	NxU32 valid = clothDesc.checkValid();
	if(clothDesc.checkValid())
		return 0;
	//Create actor
	NxCloth *cloth = 0;
	cloth = currentScene_->createCloth(clothDesc);
	
	if(cloth){
		cloth->userData = (void*)aSync;
		return cloth;
	}
	else{
		return 0;
	}
}


NxSoftBodyMesh* Physics::cookSoftBodyMesh(char* file){
    //--Create mesh structure to hold parsed data
	MyMesh *mesh = new MyMesh();
	SOFTBODY::loadSoftMeshObj("", file, mesh);
	//--Place parsed position data in structure for cooking
	unsigned int vertCount = mesh->posVec_.size();
	NxVec3 *vertArray = new NxVec3[vertCount * 4];
	for(unsigned int i = 0; i < vertCount; i++){  
		vertArray[i] = mesh->posVec_.at(i);
	}
	//--Place parsed index data in a structure for cooking
	unsigned int indexCount = mesh->indicesVec_.size();
	unsigned int *indexArray = new unsigned int[indexCount];
	for(unsigned int i = 0; i < indexCount; i++){
		indexArray[i] = mesh->indicesVec_.at(i);
	}
	//--We now have data memory in structures for cooking
	//--we can release parsed data
	delete mesh;
	mesh = 0;
	//--Create mesh desc
	NxSoftBodyMeshDesc meshDesc;
	meshDesc.numVertices = vertCount;
	meshDesc.vertexStrideBytes = sizeof(NxVec3);           //--Number of bytes from one vertex to the next.
	meshDesc.vertices = vertArray;
	if(indexCount > 0){
	    meshDesc.tetrahedronStrideBytes = 4*sizeof(NxU32); //--Number of bytes from one tet to the next.
		meshDesc.numTetrahedra = indexCount / 4;
	    meshDesc.tetrahedra = indexArray;
	}
    //--Chesck valid
	if(!meshDesc.isValid())
		return false;
	//--COOK MESH TO MEMORY
	NxInitCooking();
	MemoryWriteBuffer buf;
	bool result = NxCookSoftBodyMesh(meshDesc, buf);
	NxCloseCooking();
	if(!result)
		return 0;
    //--CREATE MESH FROM COOKED
    NxSoftBodyMesh *pMesh = 0;
	pMesh = physicsSDK_->createSoftBodyMesh(MemoryReadBuffer(buf.data));
	//--RELEASE OUR VERTEX/INDEX MEMORY
	delete vertArray;
    delete indexArray;
	//--RETURN MESH
	return pMesh;
}


NxSoftBodyMesh* Physics::loadSoftBodyMesh(char* file){
	NxSoftBodyMesh *pMesh = 0;
	pMesh = physicsSDK_->createSoftBodyMesh(UserStream(file, true));
	if(pMesh)
		return pMesh;
	else
	    return 0;
}


bool Physics::cookSoftBodyMesh(char* file, const char* destFile){
    //--Create mesh structure to hold parsed data
	MyMesh *mesh = new MyMesh();
	SOFTBODY::loadSoftMeshObj("", file, mesh);
	//--Place parsed position data in structure for cooking
	unsigned int vertCount = mesh->posVec_.size();
	NxVec3 *vertArray = new NxVec3[vertCount * 4];
	for(unsigned int i = 0; i < vertCount; i++){  
		vertArray[i] = mesh->posVec_.at(i);
	}
	//--Place parsed index data in a structure for cooking
	unsigned int indexCount = mesh->indicesVec_.size();
	unsigned int *indexArray = new unsigned int[indexCount];
	for(unsigned int i = 0; i < indexCount; i++){
		indexArray[i] = mesh->indicesVec_.at(i);
	}
	//--We now have data memory in structures for cooking
	//--we can release parsed data
	delete mesh;
	mesh = 0;
	//--Create mesh desc
	NxSoftBodyMeshDesc meshDesc;
	meshDesc.numVertices = vertCount;
	meshDesc.vertexStrideBytes = sizeof(NxVec3);           //--Number of bytes from one vertex to the next.
	meshDesc.vertices = vertArray;
	if(indexCount > 0){
	    meshDesc.tetrahedronStrideBytes = 4*sizeof(NxU32); //--Number of bytes from one tet to the next.
		meshDesc.numTetrahedra = indexCount / 4;
	    meshDesc.tetrahedra = indexArray;
	}
    //--Chesck valid
	if(!meshDesc.isValid())
		return false;
	//--COOK MESH TO FILE
	NxInitCooking();
	bool result = NxCookSoftBodyMesh(meshDesc, UserStream(destFile, false)); 
	NxCloseCooking();
    delete vertArray;
    delete indexArray;
	return result;
}



//NxSoftBodyMesh* Physics::cookSoftBodyMesh(unsigned int vertexCount,
//		                     unsigned int indexCount,
//							 NxVec3 *vertices,
//							 NxU32 *indices){
//
//		NxSoftBodyMeshDesc meshDesc;
//		meshDesc.numVertices = vertexCount;
//		//point
//		//meshDesc.vertexStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
//	    meshDesc.vertexStrideBytes = sizeof(Tet);
//		meshDesc.vertices = vertices;
//
//
//		NxU32* ind = new NxU32[vertexCount];
//		for(int i = 0; i < vertexCount; i++){
//           ind[i] = i + 1;
//		}
//
//		if(indexCount > 0){
//		   meshDesc.tetrahedronStrideBytes = 4*sizeof(NxU32);//Number of bytes from one triangle to the next.
//		   meshDesc.numTetrahedra = indexCount / 4;
//		   meshDesc.tetrahedra = indices;
//		}
//		else{
//           meshDesc.tetrahedronStrideBytes = 4*sizeof(NxU32);
//		   meshDesc.tetrahedra = ind;
//		   meshDesc.numTetrahedra = vertexCount / 4;
//		}
//
//
//		//COOK MESH
//		if(meshDesc.checkValid() != 0)
//			return false;
//
//		//COOK MESH TO MEMORY
//		NxInitCooking();
//		MemoryWriteBuffer buf;
//		
//		bool result = NxCookSoftBodyMesh(meshDesc, buf);
//		NxCloseCooking();
//
//		if(!result)
//			return 0;
//
//        //CREATE MESH FROM COOKED
//	    NxSoftBodyMesh *pMesh = 0;
//		pMesh = physicsSDK_->createSoftBodyMesh(MemoryReadBuffer(buf.data));
//		//clear buffer?
//		return pMesh;
//}

NxSoftBody* Physics::makeSoftBody(SoftBodySync *sbSync, NxSoftBodyMesh* pMesh, NxMat34 &mat, float density, NxU32 flags){
    #ifdef DEBUG
		if(!currentScene_)
			return 0;
    #endif
	NxSoftBodyDesc desc;
	desc.softBodyMesh = pMesh;
	//save to desc so we can pass data to sync object
	NxSoftBodyMeshDesc meshDesc;
	pMesh->saveToDesc(meshDesc);
	//Pass tet mesh to user data
	//All mesh memory is released when sbSync is deleted
	NxMeshData *meshData = new NxMeshData();
	NxU32 tetCount = meshDesc.numTetrahedra;
	NxU32 vertexCount  = meshDesc.numVertices;
	NxU32 indexCount = tetCount * 4;
    //release following through meshData_
    NxU32* indexBuffer   = new NxU32[indexCount * 4];
    NxVec3* vertexBuffer = new NxVec3[vertexCount];
    NxU32 *numVertices   = new NxU32();
    NxU32 *numIndices    = new NxU32();
    //Fill meshData_
    meshData->indicesBegin = (void*)indexBuffer;
    meshData->indicesByteStride = sizeof NxU32;
    meshData->maxIndices = indexCount;
    meshData->numIndicesPtr = numIndices;
    meshData->verticesPosBegin = (void*)vertexBuffer;
    meshData->verticesPosByteStride = sizeof NxVec3;
    meshData->maxVertices = vertexCount;
    meshData->numVerticesPtr = numVertices;
    
	sbSync->meshData_ = meshData;
	desc.meshData = *meshData;
	desc.globalPose = mat;
	
	desc.density = density;
	desc.flags = flags;

	//Set links
	sbSync->setLinks(&meshDesc);
	
	if(!desc.isValid())
		return 0;
	NxU32 valid = desc.checkValid();
	//Create actor
	NxSoftBody *softBody = 0;
	softBody = currentScene_->createSoftBody(desc);
	if(softBody){
		softBody->userData = (void*)sbSync;
		return softBody;
	}
	else{
		return 0;
	}
}


bool Physics::releaseSoftBodyMesh(NxSoftBodyMesh* pMesh){
	if(pMesh->getReferenceCount() == 0){
	   physicsSDK_->releaseSoftBodyMesh(*pMesh);
	   return true;
	}
	else{
	   return false;
	}
}


//NxActor*  Physics::makeConvexMesh(ActorSync *aSync, std::string url, NxMat34 &mat, float &density){
//    #ifdef DEBUG
//		if(!currentScene_)
//			return 0;
//    #endif
//	//CREATE MESH FROM COOKED
//	NxConvexMesh *pMesh = 0;
//	pMesh = physicsSDK_->createConvexMesh(UserStream(url.c_str(), true)); 
//	if(!pMesh)
//		return 0;
//	//NxConvexShapeDesc *convexShapeDesc = this->makeShapeConvex(ud, url);
//	//NOTE: Shape 0 and actor shaoe same user data, dont delete twice
//	//NOTE: Cant use makeShapeConvex as it sets local pose to ud matrix
//	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = pMesh;
//	convexShapeDesc.userData = (void*)aSync;
//	if(!convexShapeDesc.isValid())
//		return 0;
//	//Create actor
//    NxActorDesc actorDesc;
//	actorDesc.shapes.pushBack(&convexShapeDesc);  //Might not work, get desc from pMesh   
//	//BodyDesc
//	if(density){
//	   NxBodyDesc body;
//       actorDesc.density = density;
//	   actorDesc.body = &body;
//	}
//    assert(actorDesc.isValid());
//	NxActor *actor = currentScene_->createActor(actorDesc);
//    actor->userData = (void*)aSync;
//	actor->setGlobalOrientation(mat.M);
//	actor->setGlobalPosition(mat.t);
//    return actor;
//}

//NxActor* Physics::makeConvexMesh(UserData *ud, std::string url, float &density){
//    #ifdef DEBUG
//		if(!currentScene_)
//			return 0;
//    #endif
//	//CREATE MESH FROM COOKED
//	NxConvexMesh *pMesh = 0;
//	pMesh = physicsSDK_->createConvexMesh(UserStream(url.c_str(), true)); 
//	if(!pMesh)
//		return 0;
//	//NxConvexShapeDesc *convexShapeDesc = this->makeShapeConvex(ud, url);
//	//NOTE: Shape 0 and actor shaoe same user data, dont delete twice
//	//NOTE: Cant use makeShapeConvex as it sets local pose to ud matrix
//	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = pMesh;
//	convexShapeDesc.userData = (void*)ud;
//	if(!convexShapeDesc.isValid())
//		return 0;
//	//Create actor
//    NxActorDesc actorDesc;
//	actorDesc.shapes.pushBack(&convexShapeDesc);  //Might not work, get desc from pMesh   
//	//BodyDesc
//	if(density){
//	   NxBodyDesc body;
//       actorDesc.density = density;
//	   actorDesc.body = &body;
//	}
//    assert(actorDesc.isValid());
//	NxActor *actor = currentScene_->createActor(actorDesc);
//    actor->userData = (void*)ud;
//	actor->setGlobalOrientation(ud->mat_.M);
//	actor->setGlobalPosition(ud->mat_.t);
//    return actor;
//}

////NOTE: This function creates memory for a shape and returns a pointer
//NxConvexShapeDesc* Physics::makeShapeConvex(NxMat34 mat, std::string url){
//	//CREATE MESH FROM COOKED
//	NxConvexMesh *pMesh = 0;
//	pMesh = physicsSDK_->createConvexMesh(UserStream(url.c_str(), true)); 
//	if(!pMesh)
//		return 0;
//
//	NxConvexShapeDesc *convexShapeDesc = new NxConvexShapeDesc();
//	convexShapeDesc->meshData = pMesh;
//	convexShapeDesc->localPose = mat;
//	return convexShapeDesc;
//}

//NOTE: This function creates memory for a shape and returns a pointer
NxConvexShapeDesc* Physics::makeShapeConvex(NxMat34 mat, NxConvexMesh *pMesh){
	NxConvexShapeDesc *convexShapeDesc = new NxConvexShapeDesc();
	convexShapeDesc->meshData = pMesh;
	convexShapeDesc->localPose = mat;
	return convexShapeDesc;
}

//NOTE: This function creates memory for a shape and returns a pointer(not released in 'Physics')
NxTriangleMeshShapeDesc* Physics::makeShapeTriangleMesh(NxMat34 mat, NxTriangleMesh *pMesh){
	NxTriangleMeshShapeDesc *triangleMeshShapeDesc = new NxTriangleMeshShapeDesc();
	triangleMeshShapeDesc->meshData = pMesh;
	triangleMeshShapeDesc->localPose = mat;
	return triangleMeshShapeDesc;
}

NxWheelShapeDesc* Physics::makeShapeWheel(NxMat34 mat, float radius){
    //...
    //...
	return 0;
}

int Physics::addShape(NxShapeDesc *shapeDesc, NxActor *actor){
	//New Winner
	NxMat34 inverseMat;  //Undoes transformation
	actor->getGlobalPose().getInverse(inverseMat);

	//UserData* ud = (UserData*)shapeDesc->userData;
	//Swapped below
	NxMat34 mat;
	mat = shapeDesc->localPose;
	shapeDesc->localPose.multiply(inverseMat, mat);
	shapeDesc->localPose.t = actor->getGlobalPose()%mat.t;//;
	NxShape *shape = actor->createShape(*shapeDesc);
	return actor->getNbShapes() - 1;


	////New Winner
	//NxMat34 inverseMat;  //Undoes transformation
	//actor->getGlobalPose().getInverse(inverseMat);

	//UserData* ud = (UserData*)shapeDesc->userData;
	////Swapped below
	//shapeDesc->localPose.multiply(inverseMat, ud->mat_);
	//shapeDesc->localPose.t = actor->getGlobalPose()%ud->mat_.t;//;
	//NxShape *shape = actor->createShape(*shapeDesc);
	//return actor->getNbShapes() - 1;
}

//int Physics::addShapeConvex(NxActor *actor, UserDataMesh *ud, std::string url){
//	NxConvexShapeDesc *convexShapeDesc = this->makeShapeConvex(ud, url);
//	if(!convexShapeDesc->isValid())
//		return 0;
//     
//	//New Winner
//	NxMat34 inverseMat;  //Undoes transformation
//	actor->getGlobalPose().getInverse(inverseMat);
//	//Swapped below
//	convexShapeDesc->localPose.multiply(inverseMat, ud->mat_);
//	convexShapeDesc->localPose.t = actor->getGlobalPose()%ud->mat_.t;//position_;
//
//	NxShape *shape = actor->createShape(*convexShapeDesc);
//	shape->userData = (void*)ud;
//	return actor->getNbShapes() - 1;
//}

//NxActor* Physics::makeConvexMesh(UserDataMesh *ud, float &density){
//    #ifdef DEBUG
//		if(!currentScene_)
//			return 0;
//    #endif
//		NxConvexMeshDesc conMeshDesc;
//		conMeshDesc.numVertices = ud->vertexCount_;
//		conMeshDesc.numTriangles = ud->indexCount_ / 3;
//        conMeshDesc.pointStrideBytes = sizeof(NxVec3);//Number of bytes from one vertex to the next.
//        conMeshDesc.triangleStrideBytes = 3*sizeof(NxU32);//Number of bytes from one triangle to the next.
//		conMeshDesc.points = ud->vertices_;
//		conMeshDesc.triangles = ud->indices_;
//	    conMeshDesc.flags = NX_CF_COMPUTE_CONVEX;
//       
//		//COOK MESH
//		NxInitCooking();
//		MemoryWriteBuffer buf;
//		NxCookConvexMesh(conMeshDesc, buf);
//		NxCloseCooking();
//		
//		//CREATE MESH FROM COOKED
//		NxConvexShapeDesc convexShapeDesc;
//	    convexShapeDesc.meshData = physicsSDK_->createConvexMesh(MemoryReadBuffer(buf.data));
//
//
//        // Please note about the created Convex Mesh, user needs to release it when no one uses it to save memory. It can be detected
//        // by API "meshData->getReferenceCount() == 0". And, the release API is //"gPhysicsSDK->releaseConvexMesh(*convexShapeDesc.meshData);"
//
//		//Create actor
//        NxActorDesc actorDesc;
//		actorDesc.shapes.pushBack(&convexShapeDesc);  
//        
//		//BodyDesc
//		if(density){
//		   NxBodyDesc body;
//           actorDesc.density = density;
//		   actorDesc.body = &body;
//		}
//	    assert(actorDesc.isValid());
//        
//		NxActor *actor = currentScene_->createActor(actorDesc);
//	    actor->userData = (void*)ud;
//
//		actor->setGlobalOrientation(ud->mat_.M);
//		actor->setGlobalPosition(ud->mat_.t);
//		
//		//physicsSDK_->releaseConvexMesh(*convexShapeDesc.meshData);
//
//	    return actor;
//}






















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
//
//	//fluid->setFlag(NX_FF_DISABLE_GRAVITY, true);
//
//}