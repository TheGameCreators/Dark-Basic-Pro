//#ifndef DB_PHY_STRUCT
//#define DB_PHY_STRUCT


#include "D3DUtil.h"
#include "Physics.h"

#ifndef COMPILE_FOR_IRRLICHT
   #include "DBCallBacks.h"
#endif

//#include "DarkPhy2.h"
#include "ActorSync.h"
#include "Error.h"
#include <map>
#include <list>
#include <vector>


class DBPhyStruct
{
public:
	DBPhyStruct::DBPhyStruct();
	void clear();
    void release();

	static const int timeLimit_; // Only if compiled with limit
    bool limitMessageSent_;
	//static bool timeLimitSet_;
	//static int  currentTime_; 

	//For debug renderer
	D3DUtil *d3dUtil_;// = 0;
	//Core physics object
	Physics *phy_;// = 0;
	//Note when actor released, delete user data
	std::map<int, NxActor*> actorMap_;
	std::map<int, std::map<int, NxActor*>> actorLimbMap_;
	std::map<int, NxTriangleMesh*> triangleMeshMap_;
	std::map<int, NxConvexMesh*> convexMeshMap_;
	std::map<int, NxClothMesh*> clothMeshMap_;
	std::map<int, NxSoftBodyMesh*> softBodyMeshMap_;
	std::map<int, NxSoftBody*> softBodyMap_;
	std::map<int, NxCloth*> clothMap_;
	std::map<int, NxMaterial*> materialMap_;
	std::map<int, NxFluid*> fluidMap_;

	//std::list<NxVec3> vec3List_;
    NxVec3 * vec3Array_;
	unsigned int vec3ArraySize_;

	//NxD6JointDesc jointDesc_;
	std::map<int, NxD6JointDesc*> jointDescMap_;
	std::map<int, NxD6Joint*> jointMap_;

	//Note: Keep pointers to ActorSync objects as we need to release them manually.
	//We only need to release the ones left over - delete not called on actor.
	std::list<ActorSync*> actorSyncList_;
	std::list<ClothSync*> clothSyncList_;
	std::list<SoftBodySync*> softBodySyncList_;
	std::vector<NxShapeDesc*> shapeDescVec_;

	//Wheel
	NxWheelContactData wheelCData;

    //Tire Function Desc
	std::map<int, NxTireFunctionDesc*> tireFuncDescMap_;

	//Heightfield Desc
	std::map<int, NxHeightFieldDesc*> heightFieldDescMap_;
	//Heightfield
	std::map<int, NxHeightField*> heightFieldMap_;

	//Controllers
	std::map<int, NxController*> controllerMap_;

	//Controller contact
	MyCContactReport *cContact_;

	//Raycast
	myRaycastReport *raycast_;
	float maxRayDistance_;

	//Contact Reports
	MyContactReport *contact_;

	//Trigger
	MyTriggerCallback *triggerCallback_;

	//Cloth Raycast
	NxVec3 clothRayHit_;
	NxU32 clothRayVertexID_;

	//Cloth soft body
	NxVec3 softBodyRayHit_;
	NxU32 softBodyRayVertexID_;

	// Force Field
	std::map<int, NxForceFieldLinearKernel*> kernelMap_;
	std::map<int, NxForceField*> ffMap_;
	std::map<int, NxForceFieldShapeGroup*> ffShapeGroup_;

	// Force Mode
	//int forceMode;
	NxForceMode forceMode;

	bool actorDeleted_;
	float phyScaleMultiplier_;
	SOFTBODY::NxMouseDrag *mDrag_;
	SOFTBODY::NxDragInfo *dInfo_;
};




//void ignitionError(){
//	Message::errorBox(string("Physics not initialised."), string("Error"));
//	
//}
//void startCalledError(){
//	Message::errorBox(string("Multiple initialisation."), string("Error"));
//}
//
////KEEP
////Static member  for message class
//int Message::errorFlagged = 0;
//int Message::maxErrors = 3;

//#endif