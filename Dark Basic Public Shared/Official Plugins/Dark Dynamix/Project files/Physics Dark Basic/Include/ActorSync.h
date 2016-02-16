#pragma once

#include "DBUserData.h"
#include "MeshHash.h"

////-- New, ragdol example would not compile
//#include "Defines.h"
//#ifdef COMPILE_FOR_GDK
//    #include"DarkGDK.h"
//#endif


class ActorSync
{
public:
	virtual void update(NxActor *actor){};// = 0;
	int type;
	int object_;
	int limb_;
	sObject* obj_;
	NxVec3 scale_;
};

class ClothSync
{
public:
	ClothSync(){};
	virtual ~ClothSync(){};
	virtual void update(NxCloth *cloth){};
	int type;
	int object_;
	int limb_;
	NxVec3 scale_;
	NxMeshData meshData_;
};

class SoftBodySync
{
public:
	SoftBodySync(){};
	virtual ~SoftBodySync(){};
    virtual void update(){};
	void setLinks(NxSoftBodyMeshDesc *tetDesc);
	int type;
	int object_;
	int limb_;
	NxVec3 scale_;
	NxMeshData *meshData_;

	//setLinks sets the folllowing data, size of arrays is the amount of 
	//vertices in the model
    NxU32* links_;  
    //Each vertex also has barycentric coords
    NxVec3* bary_; 
}; 

class ObjectFromCloth : public ClothSync
{
public:
    ObjectFromCloth(int objectID, NxVec3 scale);
	virtual ~ObjectFromCloth();
	void update(NxCloth *cloth);
};

class ObjectFromSoftBody : public SoftBodySync
{
public:
    ObjectFromSoftBody(int objectID, NxVec3 scale);
	virtual ~ObjectFromSoftBody();
	void update();
};

class LimbFromActor : public ActorSync
{
public:
	LimbFromActor(int objectID, int limbID, NxVec3 scale);
	~LimbFromActor();
	void update(NxActor *actor);

//	NxVec3 scale_;
//	int object_;
//	int limb_;
};

//Could this class hold actor or shape?
class ObjectFromActor : public ActorSync
{
public:
	ObjectFromActor(int objectID);
	~ObjectFromActor();
	void update(NxActor *actor);
//	int object_;
};

class BoneFromActor : public ActorSync
{
public:
	BoneFromActor(int objectID, int boneID, int meshID, NxVec3 scaleVec, D3DXMATRIX offsetMat);
	~BoneFromActor();
	void update(NxActor *actor);
	//int bone_;
	int meshID_;
	//sObject* obj_;
	D3DXMATRIX scaleMat_;
	D3DXMATRIX offsetMat_;
};

class ObjectFromWheel : public ActorSync
{
public:
	ObjectFromWheel(int objectID);
	~ObjectFromWheel();
	void update(NxActor *actor);
	void updateWheel(NxWheelShape* shape);
	void updateWheel(NxWheelShape* shape, float timestep);
	float axelRotation_;
	float steerAngle_;
};




class LimbFromWheel : public ActorSync
{
public:
	LimbFromWheel(int objectID, int limbID, NxVec3 scale);
	~LimbFromWheel();
	void update(NxActor *actor);
	void updateWheel(NxWheelShape* shape);
	void updateWheel(NxWheelShape* shape, float timestep);
	float axelRotation_;
	float steerAngle_;
};


class ObjectsFromShapes : public ActorSync
{
public:
	ObjectsFromShapes(int objectID);
	~ObjectsFromShapes();

	void update(NxActor *actor);
//	int object_;
};

class ControllerSync : public ActorSync
{
public:
	void update(NxActor *actor);
	//Get user data
	//position
	float yOffset_;

};


//
//class ControllerUserData
//{
//public:
//	ControllerUserData(int objectID, float yOffset){};
//	~ControllerUserData(){};
//
//	float yOffset_;
//	int objectID_;
//
//	//void update(NxController* controller){
//        
//	
//};
//class LimbsFromShapes : public ActorSync
//{
//public:
//	LimbsFromShapes(int objectID);
//	~LimbsFromShapes();
//
//	void update(NxActor *actor);
//};