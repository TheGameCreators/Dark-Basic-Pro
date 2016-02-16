#include "ActorSync.h"
using namespace SOFTBODY;  //
//***************************************//
//*************** SYNC ******************//
//***************************************//

//LIMB FROM ACTOR
LimbFromActor::LimbFromActor(int objectID, int limbID, NxVec3 scale){
	this->type = 2;
    object_ = objectID;
	limb_   = limbID;
	scale_  = scale;
}

LimbFromActor::~LimbFromActor(){
}

void LimbFromActor::update(NxActor *actor){
	D3DXMATRIX d3dMat;
	NxMat34 mat;

    mat = actor->getGlobalPose();
    mat.getColumnMajor44(d3dMat);
	//SCALE
	d3dMat = MathUtil::getScaledMatrix(d3dMat, scale_);

	//Hold obj instead?
	sObject* obj = dbGetObject(object_);
	obj->ppFrameList[limb_]->matOverride = d3dMat;
}
//*****************************************************//


//**** OBJECT FROM ACTOR ****//
// Most straight forward way to update an object.
// One actor relates to one object, no limbs or shapes are 
// considered.
ObjectFromActor::ObjectFromActor(int objectID){
	this->type = 1;
    this->object_ = objectID;
	this->limb_   = 0;
}

ObjectFromActor::~ObjectFromActor(){
}

void ObjectFromActor::update(NxActor *actor){
	D3DXVECTOR3 vec;
	NxQuat quat = actor->getGlobalOrientationQuat();
	NxVec3 pos = actor->getGlobalPosition();
	MathUtil::quaternionToEuler(quat, vec);
	dbRotateObject(object_, vec.x, vec.y, vec.z);
	dbPositionObject(object_, pos.x, pos.y, pos.z);
}

BoneFromActor::BoneFromActor(int objectID, int boneID, int meshID, NxVec3 scaleVec, D3DXMATRIX offsetMat){
	this->type = 1;
	//we use limb and bone interchangabley
	//this->bone_ = boneID;
    this->object_ = objectID;
	this->meshID_ = meshID;
	this->limb_   = boneID;
	this->scale_ = scaleVec;
	this->obj_ = dbGetObject(objectID);
	D3DXMatrixScaling(&scaleMat_, this->scale_.x, this->scale_.y, this->scale_.z);
	this->offsetMat_ = offsetMat;
}

void BoneFromActor::update(NxActor *actor){	
	if(obj_->position.bCustomBoneMatrix == true){
		NxMat34 phyMat = actor->getGlobalPose();
		D3DXMATRIX d3dmat;
		phyMat.getColumnMajor44((float*)d3dmat);
		*obj_->ppMeshList[meshID_]->pFrameMatrices[this->limb_] = scaleMat_ * (offsetMat_ * d3dmat);
	}
}

//void BoneFromActor::update(NxActor *actor){	
//	NxMat34 phyMat = actor->getGlobalPose();
//    D3DXMATRIX d3dmat;
//	phyMat.getColumnMajor44((float*)d3dmat);
//	sObject *obj = dbGetObject(this->object_);
//
//	//d3dmat._14 = 0;
//	//d3dmat._24 = 0;
//	//d3dmat._34 = 0;
//	//d3dmat._44 = 1;
//
//	//float fScaleY = obj->position.vecScale.y;
//	//d3dmat._11 *= fScaleY;
//	//d3dmat._12 *= fScaleY;
//	//d3dmat._13 *= fScaleY;
//	//d3dmat._21 *= fScaleY;
//	//d3dmat._22 *= fScaleY;
//	//d3dmat._23 *= fScaleY;
//	//d3dmat._31 *= fScaleY;
//	//d3dmat._32 *= fScaleY;
//	//d3dmat._33 *= fScaleY;
//
//	D3DXMATRIX matOffset;
//	D3DXMatrixTranslation( &matOffset, -offsetVec_.x, -offsetVec_.y, -offsetVec_.z );
//	D3DXMatrixMultiply( &d3dmat, &matOffset, &d3dmat );
//
//    obj->bAnimUpdateOnce = true;
//
//	//D3DXMATRIX mat; D3DXMatrixIdentity(&mat);
//	//obj->ppMeshList[1]->pFrameMatrices[this->bone_] = &d3dmat;
//	//obj->ppMeshList[1]->pBones[this->bone_].matTranslation = d3dmat;// * this->initMat_;
//}

BoneFromActor::~BoneFromActor(){
}

ObjectFromWheel::~ObjectFromWheel(){
}
ObjectFromWheel::ObjectFromWheel(int objectID){
	this->type = 1;
    this->object_ = objectID;
	this->axelRotation_ = 0;
	this->steerAngle_ = 0;
	//this->update = true;
	this->limb_   = 0;
}
void ObjectFromWheel::update(NxActor *actor){
	D3DXVECTOR3 vec;
	NxQuat quat = actor->getGlobalOrientationQuat();
	NxVec3 pos = actor->getGlobalPosition();
	MathUtil::quaternionToEuler(quat, vec);
	dbRotateObject(object_, vec.x, vec.y, vec.z);
	dbPositionObject(object_, pos.x, pos.y, pos.z);
}

void ObjectFromWheel::updateWheel(NxWheelShape* shape){
	if(!shape->getActor().isSleeping())
	{
		NxReal timeStep;
		NxU32 maxIter;
		NxTimeStepMethod tsm;
		shape->getActor().getScene().getTiming(timeStep, maxIter, tsm);
		//axel
		float axelSpeed = shape->getAxleSpeed();
		float step = timeStep * maxIter;
		this->axelRotation_ += (float)((axelSpeed * step) / 3.142 * 180);
		if(this->axelRotation_ > 360){
			this->axelRotation_ -= 360;
		}
		else if(this->axelRotation_ < -360){
			this->axelRotation_ += 360;
		}
		//Steer
		float steer = shape->getSteerAngle();
		//suspension
		float suspension = shape->getSuspensionTravel();
		NxWheelContactData cData;
		float springExt;
		if(shape->getContact(cData)){
			springExt = cData.contactPosition - shape->getRadius();
		}
		else{
			springExt = shape->getSuspensionTravel();
		}
		dbMoveObjectDown(object_, springExt);
		dbTurnObjectRight(this->object_, steer / 3.142f * 180);
		dbPitchObjectDown(object_, axelRotation_);
	}
}


void ObjectFromWheel::updateWheel(NxWheelShape* shape, float timeStep){
	if(!shape->getActor().isSleeping())
	{
		//NxReal timeStep;
		//NxU32 maxIter;
		//NxTimeStepMethod tsm;
		//shape->getActor().getScene().getTiming(timeStep, maxIter, tsm);
		//axel
		float axelSpeed = shape->getAxleSpeed();
		float step = timeStep;
		this->axelRotation_ += (float)((axelSpeed * step) / 3.142 * 180);
		if(this->axelRotation_ > 360){
			this->axelRotation_ -= 360;
		}
		else if(this->axelRotation_ < -360){
			this->axelRotation_ += 360;
		}
		//Steer
		float steer = shape->getSteerAngle();
		//suspension
		float suspension = shape->getSuspensionTravel();
		NxWheelContactData cData;
		float springExt;
		if(shape->getContact(cData)){
			springExt = cData.contactPosition - shape->getRadius();
		}
		else{
			springExt = shape->getSuspensionTravel();
		}
		dbMoveObjectDown(object_, springExt);
		dbTurnObjectRight(this->object_, steer / 3.142f * 180);
		dbPitchObjectDown(object_, axelRotation_);
	}
}
//*****************************************************//


LimbFromWheel::~LimbFromWheel(){
}
LimbFromWheel::LimbFromWheel(int objectID, int limbID, NxVec3 scale){
	this->type = 1;
    this->object_ = objectID;
	this->limb_ = limbID;
	this->scale_ = scale;
	this->axelRotation_ = 0;
	this->steerAngle_ = 0;
}

void LimbFromWheel::update(NxActor *actor){
	D3DXMATRIX d3dMat;
	NxMat34 mat;

    mat = actor->getGlobalPose();
    mat.getColumnMajor44(d3dMat);
	//SCALE
	d3dMat = MathUtil::getScaledMatrix(d3dMat, scale_);

	//Hold obj instead?
	sObject* obj = dbGetObject(object_);
	obj->ppFrameList[limb_]->matOverride = d3dMat;

	//D3DXVECTOR3 vec;
	//NxQuat quat = actor->getGlobalOrientationQuat();
	//NxVec3 pos = actor->getGlobalPosition();
	//MathUtil::quaternionToEuler(quat, vec);
	//dbRotateObject(object_, vec.x, vec.y, vec.z);
	//dbPositionObject(object_, pos.x, pos.y, pos.z);
}

void LimbFromWheel::updateWheel(NxWheelShape* shape){
	if(!shape->getActor().isSleeping())
	{
		NxReal timeStep;
		NxU32 maxIter;
		NxTimeStepMethod tsm;
		shape->getActor().getScene().getTiming(timeStep, maxIter, tsm);
		//axel
		float axelSpeed = shape->getAxleSpeed();
		float step = timeStep * maxIter;
		this->axelRotation_ += (float)((axelSpeed * step) / 3.142 * 180);
		if(this->axelRotation_ > 360){
			this->axelRotation_ -= 360;
		}
		else if(this->axelRotation_ < -360){
			this->axelRotation_ += 360;
		}
		//Steer
		float steer = shape->getSteerAngle();
		//suspension
		float suspension = shape->getSuspensionTravel();
		NxWheelContactData cData;
		float springExt;
		if(shape->getContact(cData)){
			springExt = cData.contactPosition - shape->getRadius();
		}
		else{
			springExt = shape->getSuspensionTravel();
		}
		sObject* obj = dbGetObject(object_);
		D3DXMATRIX d3dMat = obj->ppFrameList[limb_]->matOverride;
		D3DXMATRIX mat;

		MathUtil::setMatrix(mat, 0, -springExt / scale_.y, 0, axelRotation_, steer, 0);
		obj->ppFrameList[limb_]->matOverride = mat * d3dMat;
	}
}

void LimbFromWheel::updateWheel(NxWheelShape* shape, float step){
	if(!shape->getActor().isSleeping())
	{
		//NxReal timeStep;
		//NxU32 maxIter;
		//NxTimeStepMethod tsm;
		//shape->getActor().getScene().getTiming(timeStep, maxIter, tsm);
		////axel
		float axelSpeed = shape->getAxleSpeed();
		//float step = timeStep * maxIter;
		this->axelRotation_ += (float)((axelSpeed * step) / 3.142 * 180);
		if(this->axelRotation_ > 360){
			this->axelRotation_ -= 360;
		}
		else if(this->axelRotation_ < -360){
			this->axelRotation_ += 360;
		}
		//Steer
		float steer = shape->getSteerAngle();
		//suspension
		float suspension = shape->getSuspensionTravel();
		NxWheelContactData cData;
		float springExt;
		if(shape->getContact(cData)){
			springExt = cData.contactPosition - shape->getRadius();
		}
		else{
			springExt = shape->getSuspensionTravel();
		}
		sObject* obj = dbGetObject(object_);
		D3DXMATRIX d3dMat = obj->ppFrameList[limb_]->matOverride;
		D3DXMATRIX mat;

		MathUtil::setMatrix(mat, 0, -springExt / scale_.y, 0, axelRotation_, steer, 0);
		obj->ppFrameList[limb_]->matOverride = mat * d3dMat;
	}
}

//******* SOFT BODY SYNC ********/


static void barycentricCoords(const NxVec3 &p0, const NxVec3 &p1,const NxVec3 &p2, const NxVec3 &p3,const NxVec3 &p, NxVec3 &barycentricCoords)
// -----------------------------------------------------------------------------------
{
	NxVec3 q  = p-p3;
	NxVec3 q0 = p0-p3;
	NxVec3 q1 = p1-p3;
	NxVec3 q2 = p2-p3;

	NxMat33 m;
  m.setColumn(0,q0);
  m.setColumn(1,q1);
  m.setColumn(2,q2);

	NxReal det = m.determinant();

	m.setColumn(0, q);
	barycentricCoords.x = m.determinant();

	m.setColumn(0, q0); m.setColumn(1,q);
	barycentricCoords.y = m.determinant();

	m.setColumn(1, q1); m.setColumn(2,q);
	barycentricCoords.z = m.determinant();

	if (det != 0.0f)
		barycentricCoords /= det;
}


void SoftBodySync::setLinks(NxSoftBodyMeshDesc *tetDesc){
    //--Soft Body
    //Each vertex needs a link eg:
    // v - tet
    // 0   829
    // 1   476
    //... 
    //Array of NxU32(unsigned int)
    dbLockVertexDataForLimb(this->object_, 0);
    unsigned int objVertCount = dbGetVertexDataVertexCount();
    dbUnlockVertexData();
    links_ = new NxU32[objVertCount]; 
    //Each vertex also has barycentric coords
    bary_ = new NxVec3[objVertCount];   
    //--SETUP SOME STUFF
    NxBounds3 mBounds;
    mBounds.setEmpty();

    NxArray<NxVec3> mPositions;
    const float *vertices = (float*)tetDesc->vertices;
    NxU32 vCount = tetDesc->numVertices;
    //Set bounds for tet
    for(NxU32 i = 0; i < vCount; i++)
    {
       NxVec3 v(vertices);
       mPositions.push_back(v);
	   mBounds.include(v);
       vertices += 3;
    }

    MeshHash hash;	
	// hash tetrahedra for faster search
	hash.setGridSpacing(mBounds.min.distance(mBounds.max) * 0.1f);
	const unsigned int *indices = (const unsigned int*)tetDesc->tetrahedra;
	NxU32 tCount = tetDesc->numTetrahedra;
	NxArray<NxU32> mIndices;
    for (NxU32 i=0; i < tCount * 4; i++)
    {
       mIndices.push_back( *indices++ );
    }
    NxU32 *idx = &mIndices[0];
    NxU32 index = 0;
    NxArray< NxU32 > newIndices;
	//Add bounding boxes to hash
	for (NxU32 i = 0; i < tCount; i++)
	{
         NxU32 i1 = *idx++;
         NxU32 i2 = *idx++;
         NxU32 i3 = *idx++;
         NxU32 i4 = *idx++;
		 newIndices.push_back(i1);
		 newIndices.push_back(i2);
		 newIndices.push_back(i3);
		 newIndices.push_back(i4);
		 NxBounds3 tetraBounds;
  		 tetraBounds.setEmpty();
  		 tetraBounds.include(mPositions[i1]);
  		 tetraBounds.include(mPositions[i2]);
  		 tetraBounds.include(mPositions[i3]);
  		 tetraBounds.include(mPositions[i4]);
  		 hash.add(tetraBounds, index);
		 index++;
	}
    tCount = newIndices.size()/4;
	for (NxU32 index = 0; index < objVertCount; index++)
	{
	    dbLockVertexDataForLimb(this->object_, 0);
        
       #ifdef COMPILE_FOR_GDK
	    NxVec3 pos = NxVec3((NxReal)dbGetVertexDataPositionX(index), (NxReal)dbGetVertexDataPositionY(index), (NxReal)dbGetVertexDataPositionZ(index));
       #else
        DWORD tempX = dbGetVertexDataPositionX(index);
		DWORD tempY = dbGetVertexDataPositionY(index);
        DWORD tempZ = dbGetVertexDataPositionZ(index);

        float resultX = *(float*)&tempX;
		float resultY = *(float*)&tempY;
		float resultZ = *(float*)&tempZ;
		NxVec3 pos = NxVec3(resultX, resultY, resultZ); 
       #endif
		dbUnlockVertexData();

		NxArray<int> itemIndices;
		hash.queryUnique(pos, itemIndices);
		NxReal minDist = 0.0f;
		NxVec3 b;
		int isize,num;
        isize = num = (int)itemIndices.size();
		if (num == 0)
		  num = tCount;
        NxU32 *indices = &newIndices[0];
		for (int i = 0; i < num; i++)
   	    {
			int j = i;
			if ( isize > 0)
            {
			  j = itemIndices[i];
            }
            NxU32 *idx = &indices[j*4];
		    NxU32 i1 = *idx++;
		    NxU32 i2 = *idx++;
		    NxU32 i3 = *idx++;
		    NxU32 i4 = *idx++;
		    NxVec3 &p0 = mPositions[i1];
		    NxVec3 &p1 = mPositions[i2];
		    NxVec3 &p2 = mPositions[i3];
		    NxVec3 &p3 = mPositions[i4];
	        NxVec3 b;
	    	barycentricCoords(p0, p1, p2, p3, pos, b);
			// is the vertex inside the tetrahedron? If yes we take it
			if (b.x >= 0.0f && b.y >= 0.0f && b.z >= 0.0f && (b.x + b.y + b.z) <= 1.0f)
			{
				bary_[index] = b;
				links_[index] = j;
				break;
			}
			// otherwise, if we are not in any tetrahedron we take the closest one
			NxReal dist = 0.0f;
			if (b.x + b.y + b.z > 1.0f) dist = b.x + b.y + b.z - 1.0f;
			if (b.x < 0.0f) dist = (-b.x < dist) ? dist : -b.x;
			if (b.y < 0.0f) dist = (-b.y < dist) ? dist : -b.y;
			if (b.z < 0.0f) dist = (-b.z < dist) ? dist : -b.z;
			if (i == 0 || dist < minDist)
			{
				minDist = dist;
				bary_[index] = b;
				links_[index] = j;
			}
		}
	}
}




//****************** OBJECT FROM CLOTH  ***************//
ObjectFromCloth::ObjectFromCloth(int objectID, NxVec3 scale){
   object_ = objectID;
   scale_ = scale;
   dbLockVertexDataForLimb(objectID, 0);
   int vertexCount = dbGetVertexDataVertexCount();// * 2;
   int indexCount  = dbGetVertexDataIndexCount();
   dbUnlockVertexData();
   //release following through meshData_
   NxU32* indexBuffer   = new NxU32[indexCount];
   NxVec3* vertexBuffer = new NxVec3[vertexCount];
   NxVec3* normalBuffer = new NxVec3[vertexCount];
   //NxVec3* vertexBuffer = new NxVec3[vertexCount * 2];
   //NxVec3* normalBuffer = new NxVec3[vertexCount * 2];
   NxU32 *numVertices   = new NxU32();  //?
   NxU32 *numIndices    = new NxU32();
   //meshData_            = new NxMeshData();
   //Fill meshData_
   meshData_.indicesBegin = (void*)indexBuffer;
   meshData_.indicesByteStride = sizeof NxU32;
   meshData_.maxIndices = indexCount;
   meshData_.numIndicesPtr = numIndices;
   meshData_.verticesPosBegin = (void*)vertexBuffer;
   meshData_.verticesPosByteStride = sizeof NxVec3;
   meshData_.maxVertices = vertexCount;// * 2;
   meshData_.numVerticesPtr = numVertices;
  // meshData_.verticesNormalBegin = 0;
   meshData_.verticesNormalBegin = (void*)normalBuffer;
   meshData_.verticesNormalByteStride = sizeof NxVec3;

   //parentIndicesBegin - The pointer to the user specified buffer for vertex parent indices.
   //parentIndicesByteStride - The stride from the start of one parent index to the next.
   //maxParentIndices - Max number of parent indices the parent index buffer can contain.
   //numParentIndicesPtr - Pointer to an integer which receives the number of parent indices that have been generated.
   NxU32* parentIndexBuffer   = new NxU32[indexCount];  //?
   NxU32 *numParentIndices    = new NxU32();
   *numParentIndices = 0;
   //Tearing
   meshData_.parentIndicesByteStride = sizeof NxU32;
   meshData_.parentIndicesBegin = (void*)parentIndexBuffer;
   meshData_.maxParentIndices = indexCount; //?
   meshData_.numParentIndicesPtr = numParentIndices;

   meshData_.dirtyBufferFlagsPtr = new NxU32();
}

ObjectFromCloth::~ObjectFromCloth(){
	delete[] (NxU32*)meshData_.indicesBegin;
	delete[] (NxVec3*)meshData_.verticesPosBegin;
	delete meshData_.numIndicesPtr;
	delete meshData_.numVerticesPtr;
	delete meshData_.dirtyBufferFlagsPtr;
	//delete meshData_;
	//meshData_ = 0;
}

void ObjectFromCloth::update(NxCloth *cloth){
    if(*meshData_.dirtyBufferFlagsPtr & NX_MDF_VERTICES_POS_DIRTY)
	{
		NxU32 *parentIndices = (NxU32*)this->meshData_.parentIndicesBegin;
		NxVec3 *vertex = (NxVec3*)this->meshData_.verticesPosBegin;
		NxVec3 *normal = (NxVec3*)this->meshData_.verticesNormalBegin;
		dbLockVertexDataForLimb(this->object_, 0);
			//int vCount = dbGetVertexDataVertexCount();
			//int iCount = dbGetVertexDataIndexCount();
			int count = 0;
			for(unsigned int i = 0; i < *this->meshData_.numVerticesPtr; i++){
				dbSetVertexDataPosition(i, vertex->x, vertex->y, vertex->z);
				dbSetVertexDataNormals(i, normal->x, normal->y, normal->z);
				vertex++;
				normal++;
				if(*meshData_.dirtyBufferFlagsPtr & NX_MDF_PARENT_INDICES_DIRTY)
				{
					int parentPos = parentIndices[i];
					float u0, v0;// u1, v1;
					#ifdef COMPILE_FOR_GDK
						u0 = dbGetVertexDataU(parentPos, 0);
						v0 = dbGetVertexDataV(parentPos, 0);
						//u1 = dbGetVertexDataU(parentPos, 1);
						//v1 = dbGetVertexDataV(parentPos, 1);
					#else
						DWORD tempU0 = dbGetVertexDataU(parentPos);
						DWORD tempV0 = dbGetVertexDataV(parentPos);
						//DWORD tempU1 = dbGetVertexDataU(parentPos, 1);
						//DWORD tempV1 = dbGetVertexDataV(parentPos, 1);
						u0 = *(float*)&tempU0;
						v0 = *(float*)&tempV0;
						//u1 = *(float*)&tempU1;
						//v1 = *(float*)&tempV1;
					#endif
					dbSetVertexDataUV(i, u0, v0);
					//dbSetVertexDataUV(i, 1, u1, v1);
				}	
			}
            if(*meshData_.dirtyBufferFlagsPtr & NX_MDF_INDICES_DIRTY)
			{
				NxU32 *index = (NxU32*)this->meshData_.indicesBegin;
				for(unsigned int i = 0; i < *this->meshData_.numIndicesPtr; i++){
					dbSetIndexData(i, *index);
					index++;
				}
			}
		dbUnlockVertexData();
	}
}

//****************** OBJECT FROM SOFT BODY  ***************//
ObjectFromSoftBody::ObjectFromSoftBody(int objectID, NxVec3 scale){
   object_ = objectID;
   scale_ = scale;
   //Data now set from Physics core
   //dbLockVertexDataForLimb(objectID, 0);
   //int vertexCount = dbGetVertexDataVertexCount();
   //int indexCount  = dbGetVertexDataIndexCount();
   //dbUnlockVertexData();
   //release following through meshData_
   //NxU32* indexBuffer   = new NxU32[indexCount];
   //NxVec3* vertexBuffer = new NxVec3[vertexCount];
   //NxU32 *numVertices   = new NxU32();
   //NxU32 *numIndices    = new NxU32();
   //meshData_            = new NxMeshData();
   //Fill meshData_
/*   meshData_->indicesBegin = (void*)indexBuffer;
   meshData_->indicesByteStride = sizeof NxU32;
   meshData_->maxIndices = indexCount;
   meshData_->numIndicesPtr = numIndices;
   meshData_->verticesPosBegin = (void*)vertexBuffer;
   meshData_->verticesPosByteStride = sizeof NxVec3;
   meshData_->maxVertices = vertexCount;
   meshData_->numVerticesPtr = numVertices;
   meshData_->verticesNormalBegin = 0;  */ 
}


ObjectFromSoftBody::~ObjectFromSoftBody(){
	delete[] links_;
	delete[] bary_;
	delete[] meshData_->indicesBegin;
	delete[] meshData_->verticesPosBegin;
	delete meshData_->numIndicesPtr;
	delete meshData_->numVerticesPtr;
	delete meshData_;
}

void ObjectFromSoftBody::update(){
	NxVec3 *vertex;// = (NxVec3*)this->meshData_->verticesPosBegin;
	//NxU32 *vAmount = this->meshData_->numVerticesPtr;
   // NxU32 count = *vAmount;

	//-- Loop through all vertices in object
	dbLockVertexDataForLimb(this->object_, 0);
		unsigned int vCount = dbGetVertexDataVertexCount();
		NxU32 *ind = (NxU32*)this->meshData_->indicesBegin; 
		for(unsigned int i = 0; i < vCount; i++){
			//Get tet index
			vertex = (NxVec3*)this->meshData_->verticesPosBegin;
			int index = this->links_[i];
			//Get Vertex for index
			NxU32 *point = &ind[index * 4];
			NxU32 *i0 = point++;
			NxU32 *i1 = point++;
			NxU32 *i2 = point++;
			NxU32 *i3 = point++;
	        
  			const NxVec3 &p0 = vertex[*i0];
   			const NxVec3 &p1 = vertex[*i1];
			const NxVec3 &p2 = vertex[*i2];
  			const NxVec3 &p3 = vertex[*i3];

			NxVec3 &b = this->bary_[i];
			NxVec3 p = p0 * b.x + p1 * b.y + p2 * b.z + p3 * (1.0f - b.x - b.y - b.z);
			dbSetVertexDataPosition(i, p.x, p.y, p.z);
		}
	dbUnlockVertexData();
}



//****************** OBJECT FROM SHAPES ***************//
// Each shape in the actor updates a different object.
ObjectsFromShapes::ObjectsFromShapes(int objectID){
	this->type = 3;
	object_ = objectID;
}

ObjectsFromShapes::~ObjectsFromShapes(){
}



void ObjectsFromShapes::update(NxActor* actor){
	//Root object(shape)
	//Done different to keep euler angles
	D3DXVECTOR3 vec;
	NxQuat quat = actor->getGlobalOrientationQuat();
	NxVec3 pos = actor->getGlobalPosition();
	MathUtil::quaternionToEuler(quat, vec);
	dbRotateObject(object_, vec.x, vec.y, vec.z);
	dbPositionObject(object_, pos.x, pos.y, pos.z);

	D3DXMATRIX d3dMat;
	NxMat34 mat;
	//Loop through shapes if it has any(not root shape(zero))
	NxShape*const* shapes = actor->getShapes();
	for (unsigned int j = 1; j < actor->getNbShapes(); j++){
		//DBUserData *ud = (DBUserData*)shapes[j]->userData;
  //      mat = shapes[j]->getGlobalPose();
		//mat.getColumnMajor44(d3dMat);
		//dbSetObjectWorldMatrix(ud->object_, &d3dMat);
		//dbPositionObject(ud->object_, mat.t.x, mat.t.y, mat.t.z);
	}
}

//*****************************************************//


void ControllerSync::update(NxActor* actor){
	sObject* obj = dbGetObject(this->object_);
	NxVec3 pos = actor->getGlobalPosition();
	obj->position.vecPosition = D3DXVECTOR3(pos.x, pos.y - this->yOffset_, pos.z);
	//dbPositionObject(this->object_, obj.
	//bool stop = true;
}

////****************** LIMBS FROM SHAPES ***************//
//// Each shape in the actor updates a different object.
//LimbsFromShapes::LimbsFromShapes(int objectID){
//	object_ = objectID;
//}
//
//LimbsFromShapes::~LimbsFromShapes(){
//}
//
//void LimbsFromShapes::update(NxActor* actor){
//	//Root object(shape)
//	//Done different to keep euler angles
//	//D3DXVECTOR3 vec;
//	//NxQuat quat = actor->getGlobalOrientationQuat();
//	//NxVec3 pos = actor->getGlobalPosition();
//	//MathUtil::quaternionToEuler(quat, vec);
//	//dbRotateObject(object_, vec.x, vec.y, vec.z);
//	//dbPositionObject(object_, pos.x, pos.y, pos.z);
//
//	D3DXMATRIX d3dMat;
//	NxMat34 mat;
//	//Loop through shapes if it has any(not root shape(zero))
//	NxShape*const* shapes = actor->getShapes();
//	for (unsigned int j = 1; j < actor->getNbShapes(); j++){
//		DBUserData *ud = (DBUserData*)shapes[j]->userData;
//        mat = shapes[j]->getGlobalPose();
//		mat.getColumnMajor44(d3dMat);
//		dbSetObjectWorldMatrix(ud->object_, &d3dMat);
//		dbPositionObject(ud->object_, mat.t.x, mat.t.y, mat.t.z);
//	}
//}
//
////*****************************************************//
