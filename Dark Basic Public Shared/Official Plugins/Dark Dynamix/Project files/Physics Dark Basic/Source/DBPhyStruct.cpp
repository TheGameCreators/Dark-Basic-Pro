#ifndef DB_PHY_STRUCT
#define DB_PHY_STRUCT

#include "DBPhyStruct.h"

extern DBPhyStruct *phyGlobStruct_;


#ifdef COMPILE_WITH_TIME_LIMIT
const int  DBPhyStruct::timeLimit_ = GetTickCount() + 1000 * 60 * 5;
#else
   const int  DBPhyStruct::timeLimit_ = 0;
#endif



DBPhyStruct::DBPhyStruct(){
	limitMessageSent_ = false;
	this->clear();
    this->raycast_ = new myRaycastReport();
    this->contact_ = new MyContactReport();
    this->cContact_ = new MyCContactReport();
    this->triggerCallback_ = new MyTriggerCallback();

}

void DBPhyStruct::release(){
		if(d3dUtil_){
			delete d3dUtil_;
			d3dUtil_ = 0;
		}
		if(mDrag_){
			SOFTBODY::releaseMouseDrag(mDrag_);
		}
		mDrag_ = 0;
		dInfo_ = 0;
        phy_->stop();
        delete phy_;
		phy_ = 0;

		delete raycast_;
		raycast_ = 0;

		delete contact_;
		contact_ = 0;

		delete cContact_;
		cContact_ = 0;

		delete triggerCallback_;
		triggerCallback_ = 0;        

		//Release any remaining ActorSync objects
		std::list<ActorSync*>::iterator it;
		for(it = actorSyncList_.begin(); it != actorSyncList_.end(); it++){
            delete (*it);
			(*it) = 0;
		}
		//actorSyncList_.clear();

		//Release any remaining ClothSync objects
		std::list<ClothSync*>::iterator it2;
		for(it2 = clothSyncList_.begin(); it2 != clothSyncList_.end(); it2++){
            delete (*it2);
			(*it2) = 0;
		}
		//clothSyncList_.clear();

		//Release any remaining SoftBodySync objects
		std::list<SoftBodySync*>::iterator it3;
		for(it3 = softBodySyncList_.begin(); it3 != softBodySyncList_.end(); it3++){
            delete (*it3);
			(*it3) = 0;
		}
		//softBodySyncList_.clear();

		//Release any remaining JointDesc 
		std::map<int, NxD6JointDesc*>::iterator it4;
		for(it4 = jointDescMap_.begin(); it4 != jointDescMap_.end(); it4++){
			delete (*it4).second;
			(*it4).second = 0;
		}
		//jointDescMap_.clear();

		//Release any remaining TireFuncDesc
		std::map<int, NxTireFunctionDesc*>::iterator it5;
		for(it5 = tireFuncDescMap_.begin(); it5 != tireFuncDescMap_.end(); it5++){
            delete (*it5).second;
			(*it5).second = 0;
		}
		//tireFuncDescMap_.clear();

		//Release any remaining HeightfieldDesc
		std::map<int, NxHeightFieldDesc*>::iterator it6;
		for(it6 = heightFieldDescMap_.begin(); it6 != heightFieldDescMap_.end(); it6++){
            delete (*it6).second;
			(*it6).second = 0;
		}

		//Release any remaining Heightfield
        //Heightfields releaes by SDK, cleared when SDK released

		clear();  //clears all maps ect
}

void DBPhyStruct::clear(){
   forceMode = NX_FORCE;// = 0
   //NX_IMPULSE = 1
   maxRayDistance_ = 3.402823e+038;
   actorDeleted_ = false;
   phy_ = 0;
   d3dUtil_ = 0;
   phyScaleMultiplier_ = 1.0f;
   mDrag_ = 0;
   dInfo_ = 0;

   vec3Array_ = 0;
   vec3ArraySize_ = 0;

   actorMap_.clear();
   actorLimbMap_.clear();
   triangleMeshMap_.clear();
   convexMeshMap_.clear();
   clothMeshMap_.clear();
   softBodyMeshMap_.clear();
   softBodyMap_.clear();
   clothMap_.clear();
   materialMap_.clear();
   jointDescMap_.clear();
   jointMap_.clear();
   actorSyncList_.clear();
   clothSyncList_.clear();
   softBodySyncList_.clear();
   shapeDescVec_.clear();
   heightFieldDescMap_.clear();
   heightFieldMap_.clear();

   //clothRayHit_.;
   //NxU32 clothRayVertexID_;
}




//#include "DarkPhy2.h"
//#include "ActorSync.h"
//#include <map>
//#include <list>
//#include <vector>
//#include "Error.h"


//class DBPhyStruct
//{
//public:
//	DBPhyStruct::DBPhyStruct(){
//		this->clear();
//	}
//
//	void clear(){
//       phy_ = 0;
//	   d3dUtil_ = 0;
//	   phyScaleMultiplier_ = 1.0f;
//	   mDrag_ = 0;
//	   dInfo_ = 0;
//	   actorMap_.clear();
//	   actorLimbMap_.clear();
//	   triangleMeshMap_.clear();
//	   convexMeshMap_.clear();
//	   clothMeshMap_.clear();
//	   softBodyMeshMap_.clear();
//	   softBodyMap_.clear();
//	   clothMap_.clear();
//	   materialMap_.clear();
//	   jointDescMap_.clear();
//	   jointMap_.clear();
//	   actorSyncList_.clear();
//	   clothSyncList_.clear();
//	   softBodySyncList_.clear();
//	   shapeDescVec_.clear();
//	}
//
//	//For debug renderer
//	D3DUtil *d3dUtil_;// = 0;
//	//Core physics object
//	Physics *phy_;// = 0;
//	//Note when actor released, delete user data
//	std::map<int, NxActor*> actorMap_;
//	std::map<int, std::map<int, NxActor*>> actorLimbMap_;
//	std::map<int, NxTriangleMesh*> triangleMeshMap_;
//	std::map<int, NxConvexMesh*> convexMeshMap_;
//	std::map<int, NxClothMesh*> clothMeshMap_;
//	std::map<int, NxSoftBodyMesh*> softBodyMeshMap_;
//	std::map<int, NxSoftBody*> softBodyMap_;
//	std::map<int, NxCloth*> clothMap_;
//	std::map<int, NxMaterial*> materialMap_;
//
//	//NxD6JointDesc jointDesc_;
//	std::map<int, NxD6JointDesc*> jointDescMap_;
//	std::map<int, NxD6Joint*> jointMap_;
//
//	//Note: Keep pointers to ActorSync objects as we need to release them manually.
//	//We only need to release the ones left over - delete not called on actor.
//	std::list<ActorSync*> actorSyncList_;
//	std::list<ClothSync*> clothSyncList_;
//	std::list<SoftBodySync*> softBodySyncList_;
//	std::vector<NxShapeDesc*> shapeDescVec_;
//
//	float phyScaleMultiplier_;
//	SOFTBODY::NxMouseDrag *mDrag_;
//	SOFTBODY::NxDragInfo *dInfo_;
//};


#endif