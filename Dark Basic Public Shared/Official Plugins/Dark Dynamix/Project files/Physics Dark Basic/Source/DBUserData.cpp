#include "DBUserData.h"

//**************************************//
//************ DBUserData **************//
//**************************************//

//DBUserData::DBUserData(void){
//    as = 0;
//}
//
//DBUserData::~DBUserData(void){
//	if(as){
//       delete as;
//	   as = 0;
//	}
//}
//
//DBUserData::DBUserData(int id){
//    object_ = id;
//    sObject *obj = 0;
//	obj = dbGetObject(object_);
//
//    NxVec3 position   = (NxVec3)obj->position.vecPosition;
//    NxVec3 rotation   = (NxVec3)obj->position.vecRotate;
//    scale_      = (NxVec3)obj->position.vecScale;
//    dim_  = (NxVec3)(obj->collision.vecMax - obj->collision.vecMin);
//	dim_.arrayMultiply(dim_, scale_);
//
//	D3DXMATRIX mat;
//	dbGetObjectWorldMatrix(object_, 0, &mat);
//	MathUtil::RemoveScaleFromMatrix(mat);
//	mat_.setColumnMajor44(mat.m);
//
//	//Create updater object 
//	as = new ObjectFromActor(object_);
//}
//
//
////NEW METHOD
//DBUserData::DBUserData(int objectID, int limbID, float scale){
//    object_ = objectID;
//	limb_ = limbID;
//
//	sObject *obj = dbGetObject(object_);
//
//	//Dont need to keep scale or mat?
//	//Object scale * limb scale
//    scale_           = (NxVec3)obj->position.vecScale;
//	NxVec3 limbScale = (NxVec3)obj->ppFrameList[limb_]->vecScale;
//	//scale_.arrayMultiply((NxVec3)obj->ppFrameList[limb]->vecScale, scale_);
//	//User scale adjust * model scale * limb scale
//	scale_.x = scale * scale_.x * limbScale.x;
//	scale_.y = scale * scale_.y * limbScale.y;
//	scale_.z = scale * scale_.z * limbScale.z;
//
//
//    D3DXMATRIX mat;
//	dbGetObjectWorldMatrix(object_, limb_, &mat);
//	MathUtil::RemoveScaleFromMatrix(mat);
//	mat_.setColumnMajor44(mat.m);
//
//	//Only override if actor, not shape
//	//When overriding we may need to set override matrix
//	dbGetObjectWorldMatrix(object_, limb_, &mat);//with scale
//	obj->ppFrameList[limb_]->matOverride = mat;
//	obj->ppFrameList[limb_]->bOverride = true;
//
//
//	//Create updater object 
//	as = new LimbFromActor(object_, limb_, scale_);
//}
//
//
//
//DBUserData::DBUserData(int id, int limbID, float sizeX, float sizeY, float sizeZ){
//    object_ = id;
//	limb_   = limbID;
//    sObject *obj = 0;
//	obj = dbGetObject(object_);
//
//	NxVec3 position = NxVec3(dbLimbPositionX(id, limbID), 
//		               dbLimbPositionY(id, limbID),
//					   dbLimbPositionZ(id, limbID));
//	NxVec3 rotation = NxVec3(dbLimbDirectionX(id, limbID),
//                       dbLimbDirectionY(id, limbID),
//                       dbLimbDirectionZ(id, limbID));
//	dim_            = NxVec3(sizeX, sizeY, sizeZ);
//    scale_       = (NxVec3)obj->position.vecScale;
//	scale_.arrayMultiply((NxVec3)obj->ppFrameList[limb_]->vecScale, scale_);
//
//	D3DXMATRIX mat;
//	dbGetObjectWorldMatrix(object_, limb_, &mat);
//	MathUtil::RemoveScaleFromMatrix(mat);
//	mat_.setColumnMajor44(mat.m);
//
//	obj->ppFrameList[limb_]->bOverride = true;
//	as = new LimbFromActor(object_, limb_, scale_);
//}
//
//
//void DBUserData::update(NxActor* actor){
//	as->update(actor);
//}


//**************************************//
//********** DBUserDataMesh ************//
//**************************************//

//DBUserDataMesh::DBUserDataMesh(void){
//    as = 0;
//}
//
//DBUserDataMesh::~DBUserDataMesh(void){
//	if(as){
//	   delete as;
//       as = 0;
//	}
//	if(vertices_){
//	   delete []vertices_;
//	   vertices_ = 0;
//	}
//	if(indices_){
//	   delete []indices_;
//	   indices_ = 0;
//	}
//}
//
//DBUserDataMesh::DBUserDataMesh(int id){
//	object_ = id;
//    sObject *obj = 0;
//	obj = dbGetObject(object_);
//	
//    scale_       = (NxVec3)obj->position.vecScale;
//    //vertexCount_ = obj->ppFrameList[0]->pMesh->dwVertexCount;
//    //indexCount_  = obj->ppFrameList[0]->pMesh->dwVertexCount;
//    //vertices_    = DBUtil::getVertexData(obj->ppFrameList[0]->pMesh, scale_);
//    //indices_     = DBUtil::getIndexData(obj->ppFrameList[0]->pMesh);
//
//	D3DXMATRIX mat;
//	dbGetObjectWorldMatrix(object_, 0, &mat);
//	MathUtil::RemoveScaleFromMatrix(mat);
//	mat_.setColumnMajor44(mat.m);
//
//	//Create updater object 
//	as = new ObjectFromActor(object_);
//
//	
//}
//
//
//DBUserDataMesh::DBUserDataMesh(int object, int limb, float scale){
//    limb_ = limb;
//	object_ = object;
//    sObject *obj = 0;
//	obj = dbGetObject(object_);
//
//	//Object scale * limb scale
//    scale_       = (NxVec3)obj->position.vecScale;
//	NxVec3 limbScale = (NxVec3)obj->ppFrameList[limb]->vecScale;
//	//scale_.arrayMultiply((NxVec3)obj->ppFrameList[limb]->vecScale, scale_);
//	//User scale adjust * model scale * limb scale
//	scale_.x = scale * scale_.x * limbScale.x;
//	scale_.y = scale * scale_.y * limbScale.y;
//	scale_.z = scale * scale_.z * limbScale.z;
//
//	//position_ = (NxVec3)obj->ppFrameList[limb_]->vecPosition;
//	//rotation_ = (NxVec3)obj->ppFrameList[limb_]->vecRotation;
//	
//	//vertexCount_ = obj->ppFrameList[limb]->pMesh->dwVertexCount;
//	//indexCount_  = obj->ppFrameList[limb]->pMesh->dwIndexCount;
//	//vertices_ = DBUtil::getVertexData(obj->ppFrameList[limb_]->pMesh, scale_);
//	//indices_ = DBUtil::getIndexData(obj->ppFrameList[limb_]->pMesh);
//
//	D3DXMATRIX mat;
//	dbGetObjectWorldMatrix(object_, limb_, &mat);
//	MathUtil::RemoveScaleFromMatrix(mat);
//	mat_.setColumnMajor44(mat.m);
//
//
//	//Only override if actor, not shape
//	//When overriding we may need to set override matrix
//	dbGetObjectWorldMatrix(object_, limb_, &mat);//with scale
//	obj->ppFrameList[limb]->matOverride = mat;
//	obj->ppFrameList[limb]->bOverride = true;
//	as = new LimbFromActor(object_, limb_, scale_);
//}
//
//void DBUserDataMesh::update(NxActor* actor){
//	as->update(actor);
//}


