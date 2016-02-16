#pragma once

#include "UserData.h"
#include "Globals.h"
#include "MathUtil.h"
#include "DBUtil.h"
#include "ActorSync.h"

//class ActorSync;


//class DBUserData :
//	public UserData
//{
//public:
//	DBUserData(void);
//	~DBUserData(void);
//	DBUserData(int id);
//	DBUserData(int id, int limbID, float sizeX, float sizeY, float sizeZ);
//	DBUserData(int id, int limbID, float scale);
//	
//	void update(NxActor* actor);
//
//	ActorSync *as;
//	int object_;
//	int limb_;
//	//NxVec3 scale_;
//};



//class DBUserDataMesh :
//	public UserDataMesh
//{
//public:
//	DBUserDataMesh(void);
//	~DBUserDataMesh(void);
//
//	//Pass in update type
//	DBUserDataMesh(int id);
//	DBUserDataMesh(int object, int limb, float scale);
//	void update(NxActor* actor);
//
//	ActorSync *as;
//	int object_;
//	int limb_;
//};